#include "DeferredRenderPipeline.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Constants.h"
#include "Configuration.h"
#include "Shaders/Light.glsl.h"

veDeferredRenderPipeline::veDeferredRenderPipeline(veSceneManager *sm)
: veRenderPipeline(sm)
{
    initLightingParams();
}

veDeferredRenderPipeline::~veDeferredRenderPipeline()
{
    
}

void veDeferredRenderPipeline::renderScene(veCamera *camera, unsigned int contextID)
{
    unsigned int defaultClearMask = camera->getClearMask();
    unsigned int deferredClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
    camera->setClearMask(deferredClearMask);
    if (_sceneManager->getSkyBox())
        _sceneManager->getSkyBox()->render(camera, contextID);
    
    auto &vp = camera->getViewport();
    auto &params = getCameraParams(camera);
    veVec2 size = veVec2(vp.width - vp.x, vp.height - vp.y);
    params.DS->storage(size.x(), size.y(), 1, GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr, 1);
    params.RT0->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
    params.RT1->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
    params.RT2->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
    params.FBO->setFrameBufferSize(size);
    params.FBO->bind(contextID, camera->getClearMask());
    prepareForDraws(camera);
    draw(camera, camera->getRenderQueue()->deferredRenderGroup);
    params.FBO->unBind();
    
    renderLights(camera, contextID);
    if (!camera->getPostProcesserList().empty()) {
        bool isFirstProcesser = true;
        for (auto &iter : camera->getPostProcesserList()) {
            auto processer = iter.get();
            if (processer->isEnabled()) {
                processer->process(this, camera, isFirstProcesser, contextID);
                isFirstProcesser = false;
            }
        }
    }
    
    if (camera->getFrameBufferObject())
        camera->getFrameBufferObject()->bind(contextID, camera->getClearMask(), GL_DRAW_FRAMEBUFFER);
    
    camera->setClearMask(defaultClearMask);
    
    params.FBO->blitFramebuffer(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST, contextID);
    camera->setClearMask(GL_COLOR_BUFFER_BIT);
    prepareForDraws(camera);
    draw(camera, camera->getRenderQueue()->forwardRenderGroup);
    camera->setClearMask(defaultClearMask);
    if (camera->getFrameBufferObject())
        camera->getFrameBufferObject()->unBind();
    
}

void veDeferredRenderPipeline::renderToPostProcesser(vePostProcesser *processer, veCamera *camera, unsigned int bufferMask, unsigned int contextID)
{
    unsigned int clearMask = camera->getClearMask();
    auto &params = getCameraParams(camera);
    if (bufferMask & (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) {
        params.FBO->blitFramebuffer(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST, contextID);
        camera->setClearMask(GL_COLOR_BUFFER_BIT);
    }
    prepareForDraws(camera);
    draw(camera, camera->getRenderQueue()->forwardRenderGroup);
    camera->setClearMask(clearMask);
}

veDeferredRenderPipeline::CameraRenderParams& veDeferredRenderPipeline::getCameraParams(veCamera *camera)
{
    auto &params = _cameraRenderParamsList[camera];
    if (!params.FBO.valid()) {
        params.FBO = _sceneManager->createFrameBufferObject(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_FBO_"));
        params.DS  = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_DS_"));
        params.RT0 = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_RT0_"));
        params.RT1 = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_RT1_"));
        params.RT2 = _sceneManager->createTexture(camera->getName() + std::string("_VE_DEFERRED_RENDER_PIPELINE_RT2_"));
        
        params.DS->setFilterMode(veTexture::NEAREST);
        params.RT0->setFilterMode(veTexture::NEAREST);
        params.RT1->setFilterMode(veTexture::NEAREST);
        params.RT2->setFilterMode(veTexture::NEAREST);
        
        
        params.FBO->attach(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, params.DS.get());
        params.FBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, params.RT0.get());
        params.FBO->attach(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, params.RT1.get());
        params.FBO->attach(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, params.RT2.get());
    }
    return params;
}

void veDeferredRenderPipeline::initLightingParams()
{
    _ambientLightRenderer     = new veScreenLightRenderer;
    _directionalLightRenderer = new veScreenLightRenderer;
    _pointLightRenderer       = new veSphereLightRenderer;
    _spotLightRenderer        = new veConeLightRenderer;
    _iBLightRenderer          = new veScreenLightRenderer;
    
    _ambientLightMaterial = createAmbientLightMaterial();
}

veMaterial* veDeferredRenderPipeline::createAmbientLightMaterial()
{
    auto material = new veMaterial;
    auto technique = new veTechnique;
    auto pass = new vePass;
    material->addTechnique(technique);
    technique->addPass(pass);
    
    pass->setRenderPass(vePass::FORWARD_PASS);
    pass->depthTest() = false;
    pass->depthWrite() = false;
    pass->stencilTest() = false;
    pass->cullFace() = true;
    pass->cullFaceMode() = GL_BACK;
    pass->blendFunc().src = GL_ONE;
    pass->blendFunc().dst = GL_ONE;
    pass->blendEquation() = GL_FUNC_ADD;
    pass->setShader(new veShader(veShader::VERTEX_SHADER, SCREEN_BASED_LIGHT_VERTEX_SHADER));
    pass->setShader(new veShader(veShader::FRAGMENT_SHADER, AMBIENT_LIGHT_FRAGMENT_SHADER));
    pass->addUniform(new veUniform("u_ambient", veVec3::ZERO));
    pass->addUniform(new veUniform("u_RT1", 0));
    
    return material;
}

veMaterial* veDeferredRenderPipeline::createDirectionalLightMaterial(veLight *light)
{
    auto material = new veMaterial;
    auto technique = new veTechnique;
    auto pass = new vePass;
    material->addTechnique(technique);
    technique->addPass(pass);
    
    initLightCommomParams(light, pass);
    pass->setRenderPass(vePass::FORWARD_PASS);
    pass->depthTest() = false;
    pass->depthWrite() = false;
    pass->stencilTest() = false;
    pass->cullFace() = true;
    pass->cullFaceMode() = GL_BACK;
    pass->blendFunc().src = GL_ONE;
    pass->blendFunc().dst = GL_ONE;
    pass->blendEquation() = GL_FUNC_ADD;
    pass->setShader(new veShader(veShader::VERTEX_SHADER, SCREEN_BASED_LIGHT_VERTEX_SHADER));
    pass->setShader(new veShader(veShader::FRAGMENT_SHADER, DIRECTIONAL_LIGHT_FRAGMENT_SHADER));
    pass->addUniform(new veUniform("u_lightDirection", veVec3::ZERO));
    pass->addUniform(new veUniform("u_lightMatrixs", &veMat4::IDENTITY, 1));
    pass->addUniform(new veUniform("u_shadowCascadedLevels", 0.f));
    pass->addUniform(new veUniform("u_shadowCascadedCount", 0.f));
    
    return material;
}

veMaterial* veDeferredRenderPipeline::createPointLightMaterial(veLight *light)
{
    auto material = new veMaterial;
    auto technique = new veTechnique;
    auto pass0 = new vePass;
    auto pass1 = new vePass;
    material->addTechnique(technique);
    technique->addPass(pass0);
    technique->addPass(pass1);
    
    pass0->setRenderPass(vePass::FORWARD_PASS);
    pass0->depthTest() = true;
    pass0->depthWrite() = false;
    pass0->stencilTest() = true;
    pass0->cullFace() = false;
    pass0->stencilFunc() = { GL_ALWAYS, 0, 0, GL_ALWAYS, 0, 0 };
    pass0->stencilOp() = { GL_KEEP, GL_DECR_WRAP, GL_KEEP, GL_KEEP, GL_INCR_WRAP, GL_KEEP };
    pass0->blendFunc() = veBlendFunc::DISABLE;
    pass0->setShader(new veShader(veShader::VERTEX_SHADER, WORLD_BASED_LIGHT_VERTEX_SHADER));
    pass0->setShader(new veShader(veShader::FRAGMENT_SHADER, "layout(location=0) out vec4 fragColor;void main(){}"));
    pass0->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
    pass0->setApplyFunctionCallback([]() {
        glClear(GL_STENCIL_BUFFER_BIT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    });
    
    initLightCommomParams(light, pass1);
    pass1->setRenderPass(vePass::FORWARD_PASS);
    pass1->depthTest() = false;
    pass1->depthWrite() = false;
    pass1->stencilTest() = true;
    pass1->cullFace() = true;
    pass1->cullFaceMode() = GL_FRONT;
    pass1->blendFunc().src = GL_ONE;
    pass1->blendFunc().dst = GL_ONE;
    pass1->blendEquation() = GL_FUNC_ADD;
    pass1->stencilFunc() = { GL_NOTEQUAL, 0, 0xFF, GL_NOTEQUAL, 0, 0xFF };
    pass1->setShader(new veShader(veShader::VERTEX_SHADER, WORLD_BASED_LIGHT_VERTEX_SHADER));
    pass1->setShader(new veShader(veShader::FRAGMENT_SHADER, POINT_LIGHT_FRAGMENT_SHADER));
    pass1->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
    pass1->addUniform(new veUniform("u_lightPosition", veVec3(0.0f)));
    pass1->addUniform(new veUniform("u_lightMatrix", veMat4::IDENTITY));
    pass1->addUniform(new veUniform("u_lightARI", 0.0f));
    pass1->setApplyFunctionCallback([]() {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    });
    
    return material;
}

veMaterial* veDeferredRenderPipeline::createSpotLightMaterial(veLight *light)
{
    auto material = new veMaterial;
    auto technique = new veTechnique;
    auto pass0 = new vePass;
    auto pass1 = new vePass;
    material->addTechnique(technique);
    technique->addPass(pass0);
    technique->addPass(pass1);
    
    pass0->setRenderPass(vePass::FORWARD_PASS);
    pass0->depthTest() = true;
    pass0->depthWrite() = false;
    pass0->stencilTest() = true;
    pass0->cullFace() = false;
    pass0->stencilFunc() = { GL_ALWAYS, 0, 0, GL_ALWAYS, 0, 0 };
    pass0->stencilOp() = { GL_KEEP, GL_DECR_WRAP, GL_KEEP, GL_KEEP, GL_INCR_WRAP, GL_KEEP };
    pass0->blendFunc() = veBlendFunc::DISABLE;
    pass0->setShader(new veShader(veShader::VERTEX_SHADER, WORLD_BASED_LIGHT_VERTEX_SHADER));
    pass0->setShader(new veShader(veShader::FRAGMENT_SHADER, "layout(location=0) out vec4 fragColor;void main(){}"));
    pass0->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
    pass0->setApplyFunctionCallback([]() {
        glClear(GL_STENCIL_BUFFER_BIT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    });
    
    initLightCommomParams(light, pass1);
    pass1->setRenderPass(vePass::FORWARD_PASS);
    pass1->depthTest() = false;
    pass1->depthWrite() = false;
    pass1->stencilTest() = true;
    pass1->cullFace() = true;
    pass1->cullFaceMode() = GL_FRONT;
    pass1->blendFunc().src = GL_ONE;
    pass1->blendFunc().dst = GL_ONE;
    pass1->blendEquation() = GL_FUNC_ADD;
    pass1->stencilFunc() = { GL_NOTEQUAL, 0, 0xFF, GL_NOTEQUAL, 0, 0xFF };
    pass1->setShader(new veShader(veShader::VERTEX_SHADER, WORLD_BASED_LIGHT_VERTEX_SHADER));
    pass1->setShader(new veShader(veShader::FRAGMENT_SHADER, SPOT_LIGHT_FRAGMENT_SHADER));
    pass1->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
    pass1->addUniform(new veUniform("u_lightDirection", veVec3(0.0f)));
    pass1->addUniform(new veUniform("u_lightPosition", veVec3(0.0f)));
    pass1->addUniform(new veUniform("u_lightMatrix", veMat4::IDENTITY));
    pass1->addUniform(new veUniform("u_lightARI", 0.0f));
    pass1->addUniform(new veUniform("u_lightInnerAngleCos", 0.0f));
    pass1->addUniform(new veUniform("u_lightOuterAngleCos", 0.0f));
    pass1->setApplyFunctionCallback([]() {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    });
    
    return material;
}

void veDeferredRenderPipeline::initLightCommomParams(veLight *light, vePass *pass)
{
    pass->addUniform(new veUniform("u_InvViewMat", INV_V_MATRIX));
    pass->addUniform(new veUniform("u_InvViewProjectMat", INV_VP_MATRIX));
    pass->addUniform(new veUniform("u_screenWidth", SCREEN_WIDTH));
    pass->addUniform(new veUniform("u_screenHeight", SCREEN_HEIGHT));
    pass->addUniform(new veUniform("u_cameraPosInWorld", veVec3::ZERO));
    pass->addUniform(new veUniform("u_lightColor", veVec3::ZERO));
    pass->addUniform(new veUniform("u_lightIntensity", 0.0f));
    pass->addUniform(new veUniform("u_lightShadowEnabled", 0.0f));
    pass->addUniform(new veUniform("u_lightShadowBias", 0.0f));
    pass->addUniform(new veUniform("u_lightShadowStrength", 0.0f));
    pass->addUniform(new veUniform("u_lightShadowSoft", 0.0f));
    pass->addUniform(new veUniform("u_lightShadowSoftness", 0.0f));
    pass->addUniform(new veUniform("u_radianceMipMapCount", 1.0f));
    
    pass->addUniform(new veUniform("u_depthTex", 0));
    pass->addUniform(new veUniform("u_RT0", 1));
    pass->addUniform(new veUniform("u_RT1", 2));
    pass->addUniform(new veUniform("u_RT2", 3));
    pass->addUniform(new veUniform("u_shadowTex", 4));
    pass->addUniform(new veUniform("u_irradiance", 5));
    pass->addUniform(new veUniform("u_radiance", 6));
}

void veDeferredRenderPipeline::updateLightCommomParams(veLight *light, vePass *pass, veCamera *camera)
{
    veMat4 cameraWorldMat = camera->getNodeToWorldMatrix();
    pass->getUniform("u_cameraPosInWorld")->setValue(veVec3(cameraWorldMat[0][3], cameraWorldMat[1][3], cameraWorldMat[2][3]));
    pass->getUniform("u_lightColor")->setValue(light->getColor());
    pass->getUniform("u_lightIntensity")->setValue(light->getIntensity());
    pass->getUniform("u_lightShadowEnabled")->setValue(light->isShadowEnabled() ? 1.0f : 0.0f);
    pass->getUniform("u_lightShadowBias")->setValue(light->getShadowBias());
    pass->getUniform("u_lightShadowStrength")->setValue(light->getShadowStrength());
    pass->getUniform("u_lightShadowSoft")->setValue(light->isUseSoftShadow() ? 1.0f : 0.0f);
    pass->getUniform("u_lightShadowSoftness")->setValue(light->getShadowSoftness());
    pass->getUniform("u_radianceMipMapCount")->setValue((veReal)camera->getRadianceTexture()->getMipMapLevelCount());
    
    auto &params = _cameraRenderParamsList[camera];
    pass->setTexture(vePass::DEPTH_STENCIL_BUFFER_TEXTURE, params.DS.get());
    pass->setTexture(vePass::DIFFUSE_TEXTURE, params.RT0.get());
    pass->setTexture(vePass::ROUGHNESS_TEXTURE, params.RT1.get());
    pass->setTexture(vePass::METALLIC_TEXTURE, params.RT2.get());
    pass->setTexture(vePass::SHADOW_TEXTURE, light->getShadowTexture());
    pass->setTexture(vePass::IRRADIANCE_TEXTURE, camera->getIrradianceTexture());
    pass->setTexture(vePass::RADIANCE_TEXTURE, camera->getRadianceTexture());
}

void veDeferredRenderPipeline::renderLights(veCamera *camera, unsigned int contextID)
{
    
    renderAmbientLight(_sceneManager->getAmbientColor(), camera, contextID);
    
    auto &lightListMap = _sceneManager->getLightListMap();
    if (!lightListMap.empty()) {
        {
            auto iter = lightListMap.find(veLight::POINT);
            if (iter != lightListMap.end()) {
                auto &pointLightList = iter->second;
                for (auto &light : pointLightList) {
                    if (light->isEnabled()) {
                        cullPointLight(light.get(), camera, contextID);
                        renderPointLight(light.get(), camera, contextID);
                    }
                }
            }
        }
        
        {
            auto iter = lightListMap.find(veLight::SPOT);
            if (iter != lightListMap.end()) {
                auto &spotLightList = iter->second;
                for (auto &light : spotLightList) {
                    if (light->isEnabled()) {
                        cullSpotLight(light.get(), camera, contextID);
                        renderSpotLight(light.get(), camera, contextID);
                    }
                }
            }
        }
        
        {
            auto iter = lightListMap.find(veLight::DIRECTIONAL);
            if (iter != lightListMap.end()) {
                auto &directionalLightList = iter->second;
                for (auto &light : directionalLightList) {
                    if (light->isEnabled()) {
                        renderDirectionalLight(light.get(), camera, contextID);
                    }
                }
            }
        }

    }
}

void veDeferredRenderPipeline::cullPointLight(veLight *light, veCamera *camera, unsigned int contextID)
{
    auto &material = _lightRenderParamsList[light];
    if (!material.valid()) {
        _lightRenderParamsList[light] = createPointLightMaterial(light);
    }
    vePass *pass = material->getTechnique(0)->getPass(0);
    
    veMat4 lightInWorld = light->getAttachedNode()->getNodeToWorldMatrix();
    _pointLightRenderer->render(lightInWorld * veMat4::scale(veVec3(light->getAttenuationRange() * 1.1f)), pass, camera, contextID);
}

void veDeferredRenderPipeline::cullSpotLight(veLight *light, veCamera *camera, unsigned int contextID)
{
    auto &material = _lightRenderParamsList[light];
    if (!material.valid()) {
        _lightRenderParamsList[light] = createSpotLightMaterial(light);
    }
    vePass *pass = material->getTechnique(0)->getPass(0);
    veSpotLight *spotLight = static_cast<veSpotLight *>(light);
    
    float rangeScale = spotLight->getAttenuationRange() * spotLight->getOuterAngle() / 45.0f;
    veMat4 lightInWorld = light->getAttachedNode()->getNodeToWorldMatrix();
    _spotLightRenderer->render(lightInWorld * veMat4::scale(veVec3(rangeScale, rangeScale, spotLight->getAttenuationRange())), pass, camera, contextID);
}

void veDeferredRenderPipeline::renderAmbientLight(const veVec3 &ambient, veCamera *camera, unsigned int contextID)
{
    vePass *pass = _ambientLightMaterial->getTechnique(0)->getPass(0);
    pass->getUniform("u_ambient")->setValue(ambient);
    auto &params = _cameraRenderParamsList[camera];
    pass->setTexture(vePass::COLOR_BUFFER_TEXTURE, params.RT1.get());
    _ambientLightRenderer->render(veMat4::IDENTITY, pass, camera, contextID);
}

void veDeferredRenderPipeline::renderDirectionalLight(veLight *light, veCamera *camera, unsigned int contextID)
{
    auto &material = _lightRenderParamsList[light];
    if (!material.valid()) {
        _lightRenderParamsList[light] = createDirectionalLightMaterial(light);
    }
    auto dLight = static_cast<veDirectionalLight *>(light);
    
    vePass *pass = material->getTechnique(0)->getPass(0);
    updateLightCommomParams(light, pass, camera);
    pass->getUniform("u_shadowCascadedCount")->setValue((veReal)static_cast<veDirectionalLight *>(light)->getShadowCascadedCount());

    
    veMat4 lightInWorld = light->getAttachedNode()->getNodeToWorldMatrix();
    lightInWorld[0][3] = lightInWorld[1][3] = lightInWorld[2][3] = 0.0f;
    if (light->isShadowEnabled()) {
        float shadowCascadedLevels[4];
        veMat4 lightShadowMats[4];
        caculateDirectionalLightCascadedParams(lightInWorld, light, camera, shadowCascadedLevels);
        renderDirectionalLightShadow(lightInWorld, light, camera, contextID, lightShadowMats);
        pass->getUniform("u_shadowCascadedLevels")->setValue(shadowCascadedLevels, dLight->getShadowCascadedCount());
        pass->getUniform("u_lightMatrixs")->setValue(lightShadowMats, dLight->getShadowCascadedCount());
    }
    
    
    veVec3 direction = lightInWorld * -veVec3::UNIT_Z;
    direction.normalize();
    pass->getUniform("u_lightDirection")->setValue(direction);
    _directionalLightRenderer->render(lightInWorld, pass, camera, contextID);
}

void veDeferredRenderPipeline::renderPointLight(veLight *light, veCamera *camera, unsigned int contextID)
{
    auto &material = _lightRenderParamsList[light];
    
    vePass *pass = material->getTechnique(0)->getPass(1);
    updateLightCommomParams(light, pass, camera);
    
    vePointLight *pointLight = static_cast<vePointLight *>(light);
    pass->getUniform("u_lightARI")->setValue(pointLight->getAttenuationRangeInverse());

    veMat4 lightInWorld = light->getAttachedNode()->getNodeToWorldMatrix();
    if (light->isShadowEnabled()) {
        veMat4 lightShadowMat;
        renderPointLightShadow(lightInWorld, light, contextID, lightShadowMat);
        pass->getUniform("u_lightMatrix")->setValue(lightShadowMat);
    }
    pass->getUniform("u_lightPosition")->setValue(veVec3(lightInWorld[0][3], lightInWorld[1][3], lightInWorld[2][3]));
    _pointLightRenderer->render(lightInWorld * veMat4::scale(veVec3(pointLight->getAttenuationRange() * 1.1f)), pass, camera, contextID);
}

void veDeferredRenderPipeline::renderSpotLight(veLight *light, veCamera *camera, unsigned int contextID)
{
    auto &material = _lightRenderParamsList[light];
    
    vePass *pass = material->getTechnique(0)->getPass(1);
    updateLightCommomParams(light, pass, camera);
    
    veSpotLight *spotLight = static_cast<veSpotLight *>(light);
    
    pass->getUniform("u_lightARI")->setValue(spotLight->getAttenuationRangeInverse());
    pass->getUniform("u_lightInnerAngleCos")->setValue(spotLight->getInnerAngleCos());
    pass->getUniform("u_lightOuterAngleCos")->setValue(spotLight->getOuterAngleCos());
    float rangeScale = spotLight->getAttenuationRange() * spotLight->getOuterAngle() / 45.0f;
    
    veMat4 lightInWorld = light->getAttachedNode()->getNodeToWorldMatrix();
    if (light->isShadowEnabled()) {
        veMat4 lightShadowMat;
        renderSpotLightShadow(lightInWorld, light, contextID, lightShadowMat);
        pass->getUniform("u_lightMatrix")->setValue(lightShadowMat);
    }
    
    pass->getUniform("u_lightPosition")->setValue(veVec3(lightInWorld[0][3], lightInWorld[1][3], lightInWorld[2][3]));
    veMat4 lightInWorldNoTrans = lightInWorld;
    lightInWorldNoTrans[0][3] = lightInWorldNoTrans[1][3] = lightInWorldNoTrans[2][3] = 0.0f;
    veVec3 direction = lightInWorldNoTrans * -veVec3::UNIT_Z;
    direction.normalize();
    pass->getUniform("u_lightDirection")->setValue(direction);
    
    _spotLightRenderer->render(lightInWorld * veMat4::scale(veVec3(rangeScale, rangeScale, spotLight->getAttenuationRange())), pass, camera, contextID);
}
