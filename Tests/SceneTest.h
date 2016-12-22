#ifndef _SCENE_TEST_
#define _SCENE_TEST_
#include "BaseTest.h"
#include "ExtCore/imgui/ImGuiComponent.h"
#include "UtilCore/GizmoComponent.h"
#include <algorithm>
#include <sstream>
#include "UpdatorComponent.h"

static veNode *INTER_NODE = nullptr;
static VE_Ptr<veGizmoComponent> GIZMO_COMPONENT = nullptr;
static bool GIZMO_SELECTOR = false;

#define GIZMO_COMPONENT_ORDER -1
#define UI_COMPONENT_ORDER    -2

#define MASTER_CAMERA_FLAG 1
#define SECOND_CAMERA_FLAG (1 << 1)

#define LIGHTING_FLAG 128

class EntityPicker : public veComponent
{
public:
    EntityPicker()
    {
    }
    ~EntityPicker() {}
    
    virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override {
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
                
                veVec3 start = viewer->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
                veVec3 end = viewer->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
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
                        GIZMO_COMPONENT->addHandleNode(INTER_NODE);
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
        GIZMO_COMPONENT->setGizmoType(veGizmoComponent::GizmoType::GT_TRANSLATION);
        veNode *gizmoNode = _sceneManager->createNode("gizmoNode");
        gizmoNode->addComponent(GIZMO_COMPONENT.get());
        gizmoNode->setVisible(false);
        _sceneManager->getRootNode()->addChild(gizmoNode);
        
        std::string fontFile = "fonts/arial.ttf";
        {
            auto text = _sceneManager->createText("text0", new veFont(fontFile, 64), "Hello Void Engine");
            text->setColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
            auto node = _sceneManager->createNode("node0");
            node->addRenderableObject(text);
            veTransformer *transer = new veTransformer;
            node->addComponent(transer);
            transer->setPosition(veVec3(0.0f, 0.8f, 0.0f));
            root->addChild(node);
            node->setMask(MASTER_CAMERA_FLAG);
        }
        
		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/Aircraft/Aircraft.vem", "Aircraft-entity"));
			veTransformer *transer = new veTransformer;
            //transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y) * veQuat(veMath::QUARTER_PI, veVec3::UNIT_X) * veQuat(veMath::HALF_PI, veVec3::UNIT_Z));
			entity->addComponent(transer);
			root->addChild(entity);
            
            {
                auto jetps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/jet.veparticle", "JetPS"));
                auto jetsmokeps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/jetsmoke.veparticle", "JetSmokePS"));
                veNode *leftJetNode = _sceneManager->createNode("LeftJetNode");
                leftJetNode->setMatrix(veMat4::transform(veVec3(2.5f, 1.8f, -2.5f), veVec3::UNIT_SCALE, veQuat(-veMath::HALF_PI, veVec3::UNIT_X)));
                leftJetNode->addRenderableObject(jetps);
                leftJetNode->addRenderableObject(jetsmokeps);
                leftJetNode->setMask(leftJetNode->getMask() & ~LIGHTING_FLAG);
                entity->addChild(leftJetNode);
            }
            
            {
                auto jetps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/jet.veparticle", "JetPS"));
                auto jetsmokeps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/jetsmoke.veparticle", "JetSmokePS"));
                veNode *rightJetNode = _sceneManager->createNode("RightJetNode");
                rightJetNode->setMatrix(veMat4::transform(veVec3(-2.5f, 1.8f, -2.5f), veVec3::UNIT_SCALE, veQuat(-veMath::HALF_PI, veVec3::UNIT_X)));
                rightJetNode->addRenderableObject(jetps);
                rightJetNode->addRenderableObject(jetsmokeps);
                rightJetNode->setMask(rightJetNode->getMask() & ~LIGHTING_FLAG);
                entity->addChild(rightJetNode);
            }
            
		}
        
        {
            veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/box.vem", "box-entity"));
            //node->addComponent(GIZMO_COMPONENT.get());
            veTransformer *transer = new veTransformer;
            entity->addComponent(transer);
            transer->setPosition(veVec3(0.0f, 5.0f, 0.0f));
            //transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_X) * veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
            transer->setScale(veVec3(2.0f));
            
            auto mat = entity->findChildBy("Box001")->getRenderableObject(0)->getMaterial();
            mat->activateTechnique(mat->getTechnique("metals"));
            
            root->addChild(entity);
        }
        
        {
            veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "Plane-entity"));
            veTransformer *transer = new veTransformer;
            entity->addComponent(transer);
            //node->addComponent(GIZMO_COMPONENT.get());
            root->addChild(entity);
            transer->setScale(veVec3(100.0f));
            transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
            
            auto mat = entity->findChildBy("Plane001")->getRenderableObject(0)->getMaterial();
            mat->activateTechnique(mat->getTechnique("ground"));
            
        }
        
        {
            veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
            root->addChild(entity);
            
            veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
            veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
            player->start(122, 141);
            player->setLoopAnimation(true);
            player->setFrameRate(60.0f);
            player->attachNode(entity);
            
            float radius = 12.0f;
            
            auto updator = new UpdatorComponent;
            entity->addComponent(updator);
            updator->UpdateCallback = [=](veSceneManager *sm, veNode *node) {
                static float angle = 0.0f;
                node->setMatrix(veMat4::rotation(veQuat(angle, veVec3::UNIT_Y)) * veMat4::translation(veVec3(0.0f, 0.0f, -radius)) * veMat4::rotation(veQuat(veMath::HALF_PI, veVec3::NEGATIVE_UNIT_Y)) * veMat4::scale(0.2f));
                angle += sm->getDeltaTime();
            };
            
//            auto ps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/star.veparticle", "starPS"));
//            veNode *node = _sceneManager->createNode("node");
//            node->addRenderableObject(ps);
//            entity->addChild(node);
        }
        
        {
            veNode *lightNode = _sceneManager->createNode("IBNode");
            veIBLight *ibLight = static_cast<veIBLight *>(_sceneManager->createLight(veLight::IB, "ibLight"));
            lightNode->addComponent(ibLight);
            veTransformer *lightTranser = new veTransformer;
            lightNode->addComponent(lightTranser);
            //point->addComponent(new LightUpdater(15.0f, 0.0f));
            ibLight->setIntensity(1.0f);
            
            veTexture *diffLighting = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/arches_irradiance.ktx", "DiffLighting"));
            diffLighting->setWrapMode(veTexture::REPEAT);
            diffLighting->setFilterMode(veTexture::LINEAR);
            veTexture *specLighting = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/arches_radiance.ktx", "SpecLighting"));
            specLighting->setWrapMode(veTexture::REPEAT);
            specLighting->setFilterMode(veTexture::LINEAR_MIP_MAP_LINEAR);
            ibLight->setDiffuseLightingTexture(diffLighting);
            ibLight->setSpecularLightingTexture(specLighting);
            root->addChild(lightNode);
        }
    

        {
            auto skyBox = _sceneManager->createSkyBox("skybox");
            _sceneManager->setSkyBox(skyBox);
            veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-arches.vemtl", "skybox-mats"));
            skyBox->setMaterial(materials->getMaterial(0));
        }
        
        
        {
            auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/oldTV.vemtl", "oldTV-mats"));
            auto postProcesser = _sceneManager->createPostProcesser("oldTV");
            postProcesser->setMaterialArray(mats);
            postProcesser->setEnabled(false);
            _mainViewer->getCamera()->addPostProcesser(postProcesser);
        }
        
        {
            auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/ASCII.vemtl", "ASCII-mats"));
            auto postProcesser = _sceneManager->createPostProcesser("ASCII");
            postProcesser->setMaterialArray(mats);
            postProcesser->setEnabled(false);
            _mainViewer->getCamera()->addPostProcesser(postProcesser);
        }
        

        {
        	auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/tiling.vemtl", "tiling-mats"));
        	auto postProcesser = _sceneManager->createPostProcesser("tiling");
        	postProcesser->setMaterialArray(mats);
            postProcesser->setEnabled(false);
            _mainViewer->getCamera()->addPostProcesser(postProcesser);
        }
        
        
        {
        	auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/grey.vemtl", "grey-mats"));
        	auto postProcesser = _sceneManager->createPostProcesser("grey");
        	postProcesser->setMaterialArray(mats);
            postProcesser->setEnabled(false);
            _mainViewer->getCamera()->addPostProcesser(postProcesser);
        }

		{
			auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/tonemapping.vemtl", "tonemapping-mats"));
			auto postProcesser = _sceneManager->createPostProcesser("tonemapping");
			postProcesser->setMaterialArray(mats);
			postProcesser->setEnabled(false);
			_mainViewer->getCamera()->addPostProcesser(postProcesser);
		}

		{
			auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/bloom.vemtl", "bloom-mats"));
			auto postProcesser = _sceneManager->createPostProcesser("bloom");
			postProcesser->setMaterialArray(mats);
			postProcesser->setEnabled(false);
			_mainViewer->getCamera()->addPostProcesser(postProcesser);
		}

		{
			auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/DOF.vemtl", "DOF-mats"));
			auto postProcesser = _sceneManager->createPostProcesser("DOF");
			postProcesser->setMaterialArray(mats);
			postProcesser->setEnabled(true);
			_mainViewer->getCamera()->addPostProcesser(postProcesser);
		}
        
        {
            auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/lensflare.vemtl", "lensflare-mats"));
            auto postProcesser = _sceneManager->createPostProcesser("lensflare");
            postProcesser->setMaterialArray(mats);
            postProcesser->setEnabled(false);
            _mainViewer->getCamera()->addPostProcesser(postProcesser);
        }

		std::function<void(veMaterial *, veViewer *, veReal, veReal)> materialUIFunc = [this](veMaterial *mat, veViewer *viewer, veReal valMin, veReal valMax) {
			if (ImGui::TreeNode(mat->getName().c_str())) {
				for (size_t tech = 0; tech < mat->getTechniqueNum(); ++tech) {
					auto technique = mat->getTechnique(tech);
					if (ImGui::TreeNode(technique->getName().c_str())) {

						for (size_t p = 0; p < technique->getPassNum(); ++p) {
							auto matpass = technique->getPass(p);
							std::stringstream ss;
							ss << p;
							if (ImGui::TreeNode(ss.str().c_str())) {
								for (size_t u = 0; u < matpass->getUniformNum(); ++u) {
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
										ImGui::DragFloat(unifrom->getName().c_str(), &val, 0.01f, valMin, valMax);
										unifrom->setValue(val);

									}
									break;
									case veUniform::Type::VEC2:
									{
										veVec2 val;
										unifrom->getValue(val);
										ImGui::SliderFloat2(unifrom->getName().c_str(), &val.x(), valMin, valMax);
										unifrom->setValue(val);

									}
									break;
									case veUniform::Type::VEC3:
									{
										veVec3 val;
										unifrom->getValue(val);
										ImGui::SliderFloat3(unifrom->getName().c_str(), &val.x(), valMin, valMax);
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

								for (size_t t = 0; t < matpass->getTextureNum(); ++t) {
									auto tex = matpass->getTexture(t);
									ImGui::Text("%s(%d x %d)", tex->getName().c_str(), tex->getWidth(), tex->getHeight());
									ImGui::Image((void *)(intptr_t)(tex->glTex(viewer->getContextID())), ImVec2(256, 256));
								}
								ImGui::TreePop();
							};

						}

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
		};

		std::function<void(veViewer *)> PostPorcesserUIFunc = [=](veViewer *viewer) {
			for (auto processer : _mainViewer->getCamera()->getPostProcesserList())
			{
				if (ImGui::TreeNode(processer->getName().c_str())) {
					bool enabled = processer->isEnabled();
					ImGui::Checkbox("Enabled", &enabled);
					processer->setEnabled(enabled);
					materialUIFunc(processer->getMaterialArray()->getMaterial(0), viewer, 0.0f, 10.0f);
					ImGui::TreePop();
				}
			}
		};
        
        std::function<void()> LightingUIFunc = nullptr;
        {
            veNode *lightNode = _sceneManager->createNode("directionalLightNode");
            lightNode->setMask(LIGHTING_FLAG);
            veLight *directional = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional0"));
            lightNode->addComponent(directional);
            veTransformer *lightTranser = new veTransformer;
            //lightNode->addComponent(lightTranser);
            //point->addComponent(new LightUpdater(20.0f, 10.0f));
            directional->setIntensity(1.0f);
            lightNode->setMatrix(veMat4::lookAt(veVec3(10.0f, 9.0f, 17.5f), veVec3::ZERO, veVec3::UNIT_Y));
            
            veNode *lightentity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
            lightentity->setMatrix(veMat4::scale(veVec3(0.2f)));
            lightNode->addChild(lightentity);
            directional->setEnabled(true);
            directional->shadowEnable(true);
            directional->setUseSoftShadow(true);
            directional->setShadowSoftness(0.001f);
            directional->setShadowBias(0.006f);
            root->addChild(lightNode);
            
            LightingUIFunc = [directional, lightTranser]{
                bool enabled = directional->isEnabled();
                ImGui::Checkbox("Enabled", &enabled);
                directional->setEnabled(enabled);
                
                veVec3 possition = lightTranser->getPosition();
                ImGui::DragFloat3("Position", &possition.x());
                lightTranser->setPosition(possition);
                
                veVec3 color = directional->getColor();
                ImGui::ColorEdit3("Color", &color.x());
                directional->setColor(color);
                
                float intensity = directional->getIntensity();
                ImGui::DragFloat("Intensity", &intensity, 0.1f);
                directional->setIntensity(intensity);
                
                float attenuationRange = directional->getAttenuationRange();
                ImGui::InputFloat("AttenuationRange", &attenuationRange);
                directional->setAttenuationRange(attenuationRange);
                
                bool shadowEnable = directional->isShadowEnabled();
                ImGui::Checkbox("ShadowEnable", &shadowEnable);
                directional->shadowEnable(shadowEnable);
                
                float shadowStrength = directional->getShadowStrength();
                ImGui::SliderFloat("ShadowStrength", &shadowStrength, 0.0f, 1.0f);
                directional->setShadowStrength(shadowStrength);
                
                bool isUseSoftShadow = directional->isUseSoftShadow();
                ImGui::Checkbox("UseSoftShadow", &isUseSoftShadow);
                directional->setUseSoftShadow(isUseSoftShadow);
                
                float softness = directional->getShadowSoftness();
                ImGui::DragFloat("ShadowSoftness", &softness, 0.001f, 0.0f, 1.0f);
                directional->setShadowSoftness(softness);
                
                float shadowBias = directional->getShadowBias();
                ImGui::DragFloat("ShadowBias", &shadowBias, 0.0001f, 0.0f, 1.0f, "%.5f");
                directional->setShadowBias(shadowBias);
                
                veVec2 shadowRes = directional->getShadowResolution();
                ImGui::InputFloat2("ShadowResolution", &shadowRes.x());
                directional->setShadowResolution(shadowRes);
                
            };
        }
        
        
        auto EntityUIFunc = [=](veViewer *viewer){
            
            
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
                veMesh *mesh = dynamic_cast<veMesh *>(INTER_NODE->getRenderableObject(0));
                auto material = mesh->getMaterial();
                int e = 0;
                for (size_t tec = 0; tec < material->getTechniqueNum(); ++tec) {
                    if (material->activeTechnique() == material->getTechnique(tec)) {
                        break;
                    }
                    ++e;
                }
                for (int tec = 0; tec < material->getTechniqueNum(); ++tec) {
                    ImGui::RadioButton(material->getTechnique(tec)->getName().c_str(), &e, tec);
                    if (tec != material->getTechniqueNum() - 1)
                        ImGui::SameLine();
                }
                
                if (material->activeTechnique() != material->getTechnique(e)) {
                    material->activateTechnique(material->getTechnique(e));
                }

				materialUIFunc(material, viewer, 0.0f, 1.0f);
            }
        };
        
        auto imguiComp = new veImGuiComponent;
        imguiComp->setGuiRenderFunc([=](veViewer *viewer){

            ImGui::SetNextWindowSize(ImVec2(_mainViewer->width() * 0.2f,_mainViewer->height() * 0.05f), ImGuiSetCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(_mainViewer->width() - _mainViewer->width() * 0.2f,_mainViewer->height() - _mainViewer->height() * 0.05f), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Status", nullptr);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            static bool show_window = true;
            if (show_window)
            {
                ImGui::SetNextWindowSize(ImVec2(_mainViewer->width() * 0.3f,_mainViewer->height()), ImGuiSetCond_FirstUseEver);
                ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiSetCond_FirstUseEver);
                ImGui::Begin("Paramters", &show_window);

                ImGui::Checkbox("Gizmo Selector", &GIZMO_SELECTOR);
                GIZMO_COMPONENT->getAttachedNode()->setVisible(GIZMO_SELECTOR);
                
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
                
                if (ImGui::CollapsingHeader("PostProcessers"))
                {
					PostPorcesserUIFunc(viewer);
                }
                
                if (ImGui::CollapsingHeader("Lighting"))
                {
                    LightingUIFunc();
                }
                
                if (INTER_NODE){
                    if (ImGui::CollapsingHeader(INTER_NODE->getName().c_str()))
                    {
                        EntityUIFunc(viewer);
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
        
        
        _sceneManager->getRootNode()->addComponent(imguiComp);

		auto debuger = new veOctreeDebuger(_sceneManager);
		debuger->debugDrawBoundingBoxWireframe(true);
		debuger->debugDrawOctree(false);
		//_sceneManager->getRootNode()->addRenderableObject(debuger);
		_sceneManager->getRootNode()->addChild(root);
        _sceneManager->setAmbientColor(veVec3(0.0f));
        
        auto entityPicker = new EntityPicker;
        _sceneManager->getRootNode()->addComponent(entityPicker);
        
//        auto viewer = veApplication::instance()->createViewer(800, 600, "Debug", _mainViewer);
//        viewer->setSceneManager(_sceneManager);
//        int width = viewer->width();
//        int height = viewer->height();
//        auto camera = _sceneManager->createCamera("SecondCamera", {0, 0, width, height });
//        camera->setProjectionMatrixAsPerspective(60.0f, (float)width / (float)height, 1.0f, 1000.0f);
//        camera->setViewMatrixAslookAt(veVec3(30.0f, 30.0f, 0.0f), veVec3::ZERO, veVec3::UNIT_Y);
//        _sceneManager->getRootNode()->addChild(camera);
//        viewer->setCamera(camera);
//        camera->setMask(SECOND_CAMERA_FLAG);
        
        
        _camera->setMask(MASTER_CAMERA_FLAG);
	}
	~SceneTest() {};

private:

};

#endif
