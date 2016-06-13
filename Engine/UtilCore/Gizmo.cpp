#include "Gizmo.h"
#include "KernelCore/SceneManager.h"
#include "GizmoRenderer.h"
#include "Constants.h"

#define GIZMO_SCALE_SQUARED_DISTANCE 100.0f

veGizmo::veGizmo(veSceneManager *sm)
    : veRenderableObject(sm)
    , _scale(1.0f)
{
    _renderer = new veGizmoRenderer;
    _materials = sm->createMaterialArray("_VE_GIZMO_MATERIALS_");
    auto material = new veMaterial;
    _materials->addMaterial(material);
    auto technique = new veTechnique;
    material->addTechnique(technique);
    auto pass = new vePass;
    technique->addPass(pass);
    pass->setRenderPass(vePass::FORWARD_PASS);
    pass->depthTest() = false;
    pass->depthWrite() = false;
    pass->cullFace() = false;
    pass->castShadow() = false;
    pass->blendFunc() = veBlendFunc::ALPHA;
    
    const char* vertex_shader = " \
    layout(location = 0) in vec3 position; \n \
    layout(location = 1) in vec4 color; \n \
    uniform mat4 u_ModelViewProjectMat; \n \
    out vec4 v_color; \n \
    void main() \n \
    {                   \n \
        v_color = color; \n \
        gl_Position = u_ModelViewProjectMat * vec4(position, 1.0); \n \
    }";
    
    const char* fragment_shader = " \
    in vec4 v_color; \n \
    layout(location=0) out vec4 fragColor;\n \
    void main() {  \n \
        fragColor = v_color; \n \
    }";
    
    pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
    pass->setShader(new veShader(veShader::VERTEX_SHADER, vertex_shader));
    pass->setShader(new veShader(veShader::FRAGMENT_SHADER, fragment_shader));
    
    _ray = _sceneManager->createRay(veVec3::ZERO, veVec3::ZERO);
    _ray->setDiscardBackFace(false);
}

veGizmo::~veGizmo()
{

}

void veGizmo::update(veNode *node, veSceneManager *sm)
{
    if (_isDirtyBoundingBox){
        _boundingBox.min() = veVec3(-1.0f * _scale);
        _boundingBox.max() = veVec3(1.0f * _scale);
        _isDirtyBoundingBox = false;
    }
}
