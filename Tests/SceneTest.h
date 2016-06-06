#ifndef _SCENE_TEST_
#define _SCENE_TEST_
#include "BaseTest.h"
#include "ExtCore/imgui/ImGuiComponent.h"

static veEntity *INTER_ENTITY = nullptr;

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
                veVec3 start = sm->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
                veVec3 end = sm->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
                auto ray = sm->createRay(start, end);
                ray->setStart(start);
                ray->setEnd(end);
                ray->apply(sm);
                if (!ray->getIntersections().empty()) {
                    INTER_ENTITY = dynamic_cast<veEntity *>(ray->getIntersections()[0].renderable);
                }else{
                    INTER_ENTITY = nullptr;
                }
            }
        }
        return false;
    }
};

class SceneTest : public BaseTest
{
public:
	SceneTest() {
		veNode *root = _sceneManager->createNode("root");
        
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/Aircraft/Aircraft.vem", "Aircraft-entity"));
			veNode *node = _sceneManager->createNode("Aircraft-node");
			node->addRenderableObject(entity);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			root->addChild(node);
		}
        
        {
            veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "Plane-entity"));
            veNode *node = _sceneManager->createNode("Plane-node");
            node->addRenderableObject(entity);
            veTransformer *transer = new veTransformer;
            node->addComponent(transer);
            root->addChild(node);
            transer->setScale(veVec3(30.0f));
            transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
        }
        
        
        std::function<void()> LightingUIFunc = nullptr;
        {
            veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
            veTransformer *lightTranser = new veTransformer;
            point->addComponent(lightTranser);
            point->addComponent(new LightUpdater(20.0f, 10.0f));
            point->setIntensity(1.0f);
            lightTranser->setPosition(veVec3(20.0f, 10.0f, 0.0f));
            
            veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
            veNode *lightModel = _sceneManager->createNode("lightnode2");
            lightModel->addRenderableObject(lightentity);
            lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
            point->addChild(lightModel);
            point->shadowEnable(true);
            point->setUseSoftShadow(true);
            point->setShadowSoftness(0.05f);
            point->setShadowBias(0.025f);
            root->addChild(point);
            
            LightingUIFunc = [point]{
                float shadowBias = point->getShadowBias();
                ImGui::SliderFloat("ShadowBias", &shadowBias, 0.0f, 1.0f);
                point->setShadowBias(shadowBias);
            };
        }
        
        
        auto EntityUIFunc = []{
            auto materials = INTER_ENTITY->getMaterialArray();
            
            for (size_t i = 0; i < materials->getMaterialNum(); ++i){
                auto matpass = materials->getMaterial(i)->activeTechnique()->getPass(0);
                if (ImGui::TreeNode(materials->getMaterial(i)->getName().c_str())){
                    auto diffuse = matpass->getUniform("u_diffuse");
                    if (diffuse){
                        veVec3 col;
                        diffuse->getValue(col);
                        ImGui::ColorEdit3("DiffuseColor", &col.x());
                        diffuse->setValue(col);
                    }
                    
                    auto specular = matpass->getUniform("u_specular");
                    if (specular){
                        veVec3 col;
                        specular->getValue(col);
                        ImGui::ColorEdit3("SpecularColor", &col.x());
                        specular->setValue(col);
                    }
                    
                    auto roughness = matpass->getUniform("u_roughness");
                    if (roughness){
                        float val;
                        roughness->getValue(val);
                        ImGui::SliderFloat("Roughness", &val, 0.0f, 1.0f);
                        roughness->setValue(val);
                    }
                    
                    auto fresnel = matpass->getUniform("u_fresnel");
                    if (fresnel){
                        float val;
                        fresnel->getValue(val);
                        ImGui::SliderFloat("Fresnel", &val, 0.0f, 1.0f);
                        fresnel->setValue(val);
                    }
                    
                    auto displacement = matpass->getUniform("u_displacement");
                    if (displacement){
                        float val;
                        displacement->getValue(val);
                        ImGui::DragFloat("Displacement", &val, 0.0001f, 0.0f, 1.0f, "%.4f");
                        displacement->setValue(val);
                    }
                    ImGui::TreePop();
                }
            }
        };
        
        auto imguiComp = new veImGuiComponent;
        imguiComp->setGuiRenderFunc([=]{
            static bool show_window = true;
            if (show_window)
            {
                ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiSetCond_FirstUseEver);
                ImGui::Begin("Paramters", &show_window);
                
                if (ImGui::CollapsingHeader("Lighting"))
                {
                    LightingUIFunc();
                }
                
                if (INTER_ENTITY){
                    if (ImGui::CollapsingHeader(INTER_ENTITY->getName().c_str()))
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