#ifndef _SCENE_TEST_
#define _SCENE_TEST_
#include "BaseTest.h"
#include "ExtCore/imgui/ImGuiComponent.h"
#include "UtilCore/GizmoComponent.h"
#include <algorithm>

static veNode *INTER_NODE = nullptr;
static VE_Ptr<veGizmoComponent> GIZMO_COMPONENT = nullptr;
static bool GIZMO_SELECTOR = false;

#define GIZMO_COMPONENT_ORDER -1
#define UI_COMPONENT_ORDER    -2

class EntityPicker : public veComponent
{
public:
    EntityPicker()
    {
    }
    ~EntityPicker() {}
    
    virtual bool handle(veSceneManager *sm, const veEvent &event) override {
        if (event.getEventType() & veEvent::VE_MOUSE_EVENT || event.getEventType() & veEvent::VE_TOUCH_EVENT) {
            veVec2 screenCoords;
            bool state = false;
            if (event.getEventType() == veEvent::VE_PRESS && event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_LEFT) {
                screenCoords = veVec2(event.getMouseX(), event.getMouseY());
                state = true;
            }
            else if (event.getEventType() == veEvent::VE_TOUCH_START) {
                auto touchs = event.getTouches();
                if (touchs.size() == 1) {
                    screenCoords = veVec2(touchs[0].x, touchs[0].y);
                    state = true;
                }
            }
            
            if (state) {
//                if (event.getModKeySymbol() == veEvent::VE_MOD_ALT) {
//                    auto nodeList = GIZMO_COMPONENT->getAttachedNodeList();
//                    for (auto &node : nodeList){
//                        node->removeComponent(GIZMO_COMPONENT.get());
//                    }
//                }
                
                veVec3 start = sm->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
                veVec3 end = sm->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
                auto ray = sm->createRay(start, end);
                ray->setStart(start);
                ray->setEnd(end);
                ray->apply(sm);
                if (!ray->getIntersections().empty()) {
//                    for (auto &inter : ray->getIntersections()){
//                        inter.node->addComponent(GIZMO_COMPONENT.get());
//                    }
                    INTER_NODE = ray->getIntersections()[0].node;
//                    if (event.getModKeySymbol() == veEvent::VE_MOD_ALT)
//                        INTER_NODE->addComponent(GIZMO_COMPONENT.get());
                    
                    if (GIZMO_SELECTOR){
                        auto nodeList = GIZMO_COMPONENT->getAttachedNodeList();
                        auto niter = std::find(nodeList.begin(), nodeList.end(), INTER_NODE);
                        if (niter != nodeList.end()){
                            INTER_NODE->removeComponent(GIZMO_COMPONENT.get());
                        }else{
                            INTER_NODE->addComponent(GIZMO_COMPONENT.get());
                        }
                        return true;
                    }
                }else{
                    INTER_NODE = nullptr;
                    
                }
            }
        }
    
//        if (event.getEventType() == veEvent::VE_DOWN){
//            if (event.getKeySymbol() == veEvent::VE_KEY_Q){
//                GIZMO_COMPONENT->setGizmoType(veGizmoComponent::GizmoType::GT_TRANSLATION);
//            }else if (event.getKeySymbol() == veEvent::VE_KEY_W){
//                GIZMO_COMPONENT->setGizmoType(veGizmoComponent::GizmoType::GT_ROTATION);
//            }else if (event.getKeySymbol() == veEvent::VE_KEY_E){
//                GIZMO_COMPONENT->setGizmoType(veGizmoComponent::GizmoType::GT_SCALE);
//            }
//        }
        return false;
    }
};

class SceneTest : public BaseTest
{
public:
	SceneTest() {
		veNode *root = _sceneManager->createNode("root");
        
        GIZMO_COMPONENT = new veGizmoComponent;
        GIZMO_COMPONENT->setUpdateOrder(GIZMO_COMPONENT_ORDER);
        GIZMO_COMPONENT->setGizmoType(veGizmoComponent::GizmoType::GT_TRANSLATION);
        
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/Aircraft/Aircraft.vem", "Aircraft-entity"));
			veNode *node = _sceneManager->createNode("Aircraft-node");
			node->addRenderableObject(entity);
            //node->addComponent(GIZMO_COMPONENT.get());
			veTransformer *transer = new veTransformer;
            //transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y) * veQuat(veMath::QUARTER_PI, veVec3::UNIT_X) * veQuat(veMath::HALF_PI, veVec3::UNIT_Z));
			node->addComponent(transer);
			root->addChild(node);
		}
        
        {
            veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/box.vem", "box-entity"));
            veNode *node = _sceneManager->createNode("box-node");
            node->addRenderableObject(entity);
            //node->addComponent(GIZMO_COMPONENT.get());
            veTransformer *transer = new veTransformer;
            node->addComponent(transer);
            transer->setPosition(veVec3(0.0f, 5.0f, 0.0f));
            //transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_X) * veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
            transer->setScale(veVec3(2.0f));
            root->addChild(node);
        }
        
        {
            veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "Plane-entity"));
            veNode *node = _sceneManager->createNode("Plane-node");
            node->addRenderableObject(entity);
            veTransformer *transer = new veTransformer;
            node->addComponent(transer);
            //node->addComponent(GIZMO_COMPONENT.get());
            root->addChild(node);
            transer->setScale(veVec3(30.0f));
            transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
        }
        
        
        std::function<void()> LightingUIFunc = nullptr;
        {
            veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
            veTransformer *lightTranser = new veTransformer;
            point->addComponent(lightTranser);
            //point->addComponent(new LightUpdater(20.0f, 10.0f));
            point->setIntensity(1.0f);
            lightTranser->setPosition(veVec3(20.0f, 10.0f, 0.0f));
            
            veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
            veNode *lightModel = _sceneManager->createNode("lightnode2");
            lightModel->addRenderableObject(lightentity);
            lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
            point->addChild(lightModel);
            point->setEnabled(true);
            point->shadowEnable(true);
            point->setUseSoftShadow(true);
            point->setShadowSoftness(0.05f);
            point->setShadowBias(0.025f);
            root->addChild(point);
            
            LightingUIFunc = [point, lightTranser]{
                bool enabled = point->isEnabled();
                ImGui::Checkbox("Enabled", &enabled);
                point->setEnabled(enabled);
                
                veVec3 possition = lightTranser->getPosition();
                ImGui::DragFloat3("Position", &possition.x());
                lightTranser->setPosition(possition);
                
                veVec3 color = point->getColor();
                ImGui::ColorEdit3("Color", &color.x());
                point->setColor(color);
                
                float intensity = point->getIntensity();
                ImGui::DragFloat("Intensity", &intensity, 0.1f);
                point->setIntensity(intensity);
                
                float attenuationRange = point->getAttenuationRange();
                ImGui::InputFloat("AttenuationRange", &attenuationRange);
                point->setAttenuationRange(attenuationRange);
                
                bool shadowEnable = point->isShadowEnabled();
                ImGui::Checkbox("ShadowEnable", &shadowEnable);
                point->shadowEnable(shadowEnable);
                
                float shadowStrength = point->getShadowStrength();
                ImGui::SliderFloat("ShadowStrength", &shadowStrength, 0.0f, 1.0f);
                point->setShadowStrength(shadowStrength);
                
                bool isUseSoftShadow = point->isUseSoftShadow();
                ImGui::Checkbox("UseSoftShadow", &isUseSoftShadow);
                point->setUseSoftShadow(isUseSoftShadow);
                
                float softness = point->getShadowSoftness();
                ImGui::DragFloat("ShadowSoftness", &softness, 0.01f, 0.0f, 1.0f);
                point->setShadowSoftness(softness);
                
                float shadowBias = point->getShadowBias();
                ImGui::DragFloat("ShadowBias", &shadowBias, 0.0001f, 0.0f, 1.0f, "%.5f");
                point->setShadowBias(shadowBias);
                
                veVec2 shadowRes = point->getShadowResolution();
                ImGui::InputFloat2("ShadowResolution", &shadowRes.x());
                point->setShadowResolution(shadowRes);
                
                veVec2 shadowArea = point->getShadowArea();
                ImGui::InputFloat2("ShadowArea", &shadowArea.x());
                point->setShadowArea(shadowArea);
            };
        }
        
        
        auto EntityUIFunc = []{
            
            
            if (ImGui::CollapsingHeader("Transform")) {
                
                veMat4 mat = INTER_NODE->getMatrix();
                veVec3 pos, scl; veQuat rot;
                mat.decomposition(&pos, &scl, &rot);
                veQuat eularX = veQuat(-rot.yaw(), veVec3::UNIT_Y) * rot;
                veQuat eularZ = veQuat(-eularX.pitch(false), veVec3::UNIT_X) * eularX;
                veVec3 eularRot(veMath::veDegree(eularX.pitch(false)), veMath::veDegree(rot.yaw()), veMath::veDegree(eularZ.roll(false)));
                
                ImGui::InputFloat3("Position", &pos.x());
                ImGui::InputFloat3("Rotation", &eularRot.x());
                ImGui::InputFloat3("Scale", &scl.x());
                
                rot = veQuat(veMath::veRadian(eularRot.y()), veVec3::UNIT_Y) * veQuat(veMath::veRadian(eularRot.x()), veVec3::UNIT_X) * veQuat(veMath::veRadian(eularRot.z()), veVec3::UNIT_Z);
                INTER_NODE->setMatrix(veMat4::transform(pos, scl, rot));
            }
            
            if (ImGui::CollapsingHeader("Materials")) {
                veEntity *entity = dynamic_cast<veEntity *>(INTER_NODE->getRenderableObject(0));
                auto materials = entity->getMaterialArray();
                
                for (size_t i = 0; i < materials->getMaterialNum(); ++i){
                    auto matpass = materials->getMaterial(i)->activeTechnique()->getPass(0);
                    if (ImGui::TreeNode(materials->getMaterial(i)->getName().c_str())){
                        for (size_t u = 0; u < matpass->getUniformNum(); ++u){
                            auto unifrom = matpass->getUniform(u);
                            switch (unifrom->getType()) {
                                case veUniform::Type::INT:
                                {
                                    int val;
                                    unifrom->getValue(val);
                                    ImGui::InputInt(unifrom->getName().c_str(), &val);
                                    unifrom->setValue(val);
                                    
                                }
                                    break;
                                case veUniform::Type::BOOL:
                                {
                                    bool val;
                                    unifrom->getValue(val);
                                    ImGui::Checkbox(unifrom->getName().c_str(), &val);
                                    unifrom->setValue(val);
                                    
                                }
                                    break;
                                case veUniform::Type::REAL:
                                {
                                    float val;
                                    unifrom->getValue(val);
                                    ImGui::SliderFloat(unifrom->getName().c_str(), &val, 0.0f, 1.0f);
                                    unifrom->setValue(val);
                                    
                                }
                                    break;
                                case veUniform::Type::VEC2:
                                {
                                    veVec2 val;
                                    unifrom->getValue(val);
                                    ImGui::SliderFloat2(unifrom->getName().c_str(), &val.x(), 0.0f, 1.0f);
                                    unifrom->setValue(val);
                                    
                                }
                                    break;
                                case veUniform::Type::VEC3:
                                {
                                    veVec3 val;
                                    unifrom->getValue(val);
                                    ImGui::SliderFloat3(unifrom->getName().c_str(), &val.x(), 0.0f, 1.0f);
                                    unifrom->setValue(val);
                                    
                                }
                                    break;
                                case veUniform::Type::VEC4:
                                {
                                    veVec4 val;
                                    unifrom->getValue(val);
                                    ImGui::ColorEdit4(unifrom->getName().c_str(), &val.x());
                                    unifrom->setValue(val);
                                    
                                }
                                    break;
                                    
                                default:
                                    break;
                            }
                        }
                        
                        for (size_t t = 0; t < matpass->getTextureNum(); ++t){
                            auto tex = matpass->getTexture(t);
                            ImGui::Text("%s(%d x %d)", tex->getName().c_str(), tex->getWidth(), tex->getHeight());
                            ImGui::Image((void *)(intptr_t)(tex->glTex()), ImVec2(256, 256));
                        }
                        ImGui::TreePop();
                    }
                }
            }
        };
        
        auto imguiComp = new veImGuiComponent;
        imguiComp->setUpdateOrder(UI_COMPONENT_ORDER);
        imguiComp->setGuiRenderFunc([=]{

            ImGui::SetNextWindowSize(ImVec2(veApplication::instance()->width() * 0.2f,veApplication::instance()->height() * 0.05f), ImGuiSetCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(veApplication::instance()->width() - veApplication::instance()->width() * 0.2f,veApplication::instance()->height() - veApplication::instance()->height() * 0.05f), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Status", nullptr);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            static bool show_window = true;
            if (show_window)
            {
                ImGui::SetNextWindowSize(ImVec2(veApplication::instance()->width() * 0.3f,veApplication::instance()->height()), ImGuiSetCond_FirstUseEver);
                ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiSetCond_FirstUseEver);
                ImGui::Begin("Paramters", &show_window);

                ImGui::Checkbox("Gizmo Selector", &GIZMO_SELECTOR);
                
                static int e = 0;
                ImGui::RadioButton("Tranlation", &e, 0);ImGui::SameLine();
                ImGui::RadioButton("Rotation", &e, 1);ImGui::SameLine();
                ImGui::RadioButton("Scale", &e, 2);
                if (e == 0){
                    GIZMO_COMPONENT->setGizmoType(veGizmoComponent::GizmoType::GT_TRANSLATION);
                }else if (e == 1){
                    GIZMO_COMPONENT->setGizmoType(veGizmoComponent::GizmoType::GT_ROTATION);
                }else{
                    GIZMO_COMPONENT->setGizmoType(veGizmoComponent::GizmoType::GT_SCALE);
                }
                
                if (ImGui::CollapsingHeader("Lighting"))
                {
                    LightingUIFunc();
                }
                
                if (INTER_NODE){
                    if (ImGui::CollapsingHeader(INTER_NODE->getName().c_str()))
                    {
                        EntityUIFunc();
                    }
                }
                
                ImGui::End();
            }
            
//            static bool show_test_window = true;
//            if (show_test_window){
//                ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
//                ImGui::ShowTestWindow(&show_test_window);
//            }
        });
        
        _sceneManager->addComponent(imguiComp);

		auto debuger = new veOctreeDebuger(_sceneManager);
		debuger->debugDrawBoundingBoxWireframe(true);
		debuger->debugDrawOctree(false);
		//_sceneManager->getRootNode()->addRenderableObject(debuger);
		_sceneManager->getRootNode()->addChild(root);
        _sceneManager->setAmbientColor(veVec3(0.2f));
        
        auto entityPicker = new EntityPicker;
        _sceneManager->addComponent(entityPicker);
	}
	~SceneTest() {};

private:

};

#endif