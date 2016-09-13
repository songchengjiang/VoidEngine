#include "PostProcesser.h"
#include "SceneManager.h"
#include "FrameBufferObject.h"
#include "Configuration.h"

static const char* BLEND_SCENE_V_SHADER = " \
    layout(location = 0) in vec3 position; \n \
    layout(location = 1) in vec3 normal; \n \
    layout(location = 2) in vec2 texcoord; \n \
    out vec2 v_texcoord; \n \
    void main() \n \
    {                   \n \
        v_texcoord = texcoord; \n \
        gl_Position = vec4(position, 1.0); \n \
    }";

static const char* BLEND_SCENE_F_SHADER = " \
    uniform vec3 u_ambient; \n \
    uniform sampler2D u_sceneTex; \n \
    in vec2 v_texcoord; \n \
    layout(location = 0) out vec4 fragColor; \n \
    void main() {  \n \
        fragColor = vec4(texture(u_sceneTex, v_texcoord).xyz, 0.5); \
        \n \
    }";

vePostProcesser::vePostProcesser(veSceneManager *sm)
	: USE_VE_PTR_INIT
    , _isEnabled(true)
	, _sceneManager(sm)
{
	_renderer = new vePostProcesserRenderer;
	_renderer->setRenderStageMask(_renderer->getRenderStageMask() & ~veRenderer::LIGHTINGING);
}

vePostProcesser::~vePostProcesser()
{

}

void vePostProcesser::process(veRenderPipeline *pipeline, veCamera *camera, veTexture* sceneColorTex, veTexture* sceneDepthTex, unsigned int contextID)
{
    auto &vp = camera->getViewport();
    veVec2 size = veVec2(vp.width - vp.x, vp.height - vp.y);
    if (!_fbo.valid()) {
        _fbo = _sceneManager->createFrameBufferObject(_name + std::string("_VE_POST_PROCESSER_FBO_"));
    }
    _fbo->setFrameBufferSize(size);
    
	for (unsigned int i = 0; i < _materials->getMaterialNum(); ++i) {
		auto material = _materials->getMaterial(i);
		for (unsigned int p = 0; p < material->activeTechnique()->getPassNum(); ++p) {
			auto pass = material->activeTechnique()->getPass(p);
            
            auto outputTex = pass->getTexture(vePass::OUTPUT_TEXTURE);
            if (outputTex) {
                outputTex->storage(size.x(), size.y(), 1, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, nullptr, 1);
                _fbo->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex);
                _fbo->bind(contextID, camera->getClearMask(), GL_DRAW_FRAMEBUFFER);
                pipeline->renderToPostProcesser(this, camera, contextID);
                _fbo->unBind();
            }
            
			auto colTex = pass->getTexture(vePass::COLOR_BUFFER_TEXTURE);
            if (colTex) {
                pass->setTexture(vePass::COLOR_BUFFER_TEXTURE, sceneColorTex);
            }
            auto depTex = pass->getTexture(vePass::DEPTH_STENCIL_BUFFER_TEXTURE);
            if (depTex) {
                pass->setTexture(vePass::DEPTH_STENCIL_BUFFER_TEXTURE, sceneDepthTex);
            }
            _renderer->render(_sceneManager->getRootNode(), pass, camera, contextID);
		}
	}
    
    if (!_processedTexture.valid()) {
        _processedTexture = _sceneManager->createTexture("_VE_POST_PROCESSER_TEXTURE_");
    }
    _processedTexture->storage(size.x(), size.y(), 1, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, nullptr, 1);
    _fbo->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _processedTexture.get());
    _fbo->bind(contextID, camera->getClearMask(), GL_DRAW_FRAMEBUFFER);
    pipeline->renderToPostProcesser(this, camera, contextID);
    _fbo->unBind();
    
    if (!_postProcesserSurface.valid()) {
        auto blendSceneMat = new veMaterial;
        auto blendSceneTech = new veTechnique;
        auto pass = new vePass;
        blendSceneMat->addTechnique(blendSceneTech);
        blendSceneTech->addPass(pass);
        _postProcesserSurface = _sceneManager->createSurface("_VE_POST_PROCESSER_SURFACE_");
        _postProcesserSurface->setMaterial(blendSceneMat);
        
        pass->setRenderPass(vePass::FORWARD_PASS);
        pass->depthTest() = false;
        pass->depthWrite() = false;
        pass->cullFace() = true;
        pass->cullFaceMode() = GL_BACK;
        pass->blendFunc() = veBlendFunc::ALPHA;
        pass->setShader(new veShader(veShader::VERTEX_SHADER, BLEND_SCENE_V_SHADER));
        pass->setShader(new veShader(veShader::FRAGMENT_SHADER, BLEND_SCENE_F_SHADER));
        pass->addUniform(new veUniform("u_sceneTex", 0));
        pass->setTexture(vePass::COLOR_BUFFER_TEXTURE, _processedTexture.get());
    }
}
