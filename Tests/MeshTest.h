#ifndef _MESH_TEST_
#define _MESH_TEST_
#include "BaseTest.h"
#include "UpdatorComponent.h"


class Handler : public veComponent
{
public:
    
    virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override{
        if (event.getEventType() & veEvent::VE_DOWN) {
            if (event.getKeySymbol() == veEvent::VE_KEY_L) {
                veNode *lightNode = _attachedNode->findChildBy("pointLightNode");
                veLight *light = lightNode->getComponent<vePointLight>();
                bool isON = !light->isEnabled();
                
                veVec3 color = veVec3(1.0f);
                if (light) {
                    light->setEnabled(isON);
                    //light->setColor(color);
                    //light->setIntensity(2.0f);
                }
                
                veNode *node = _attachedNode->findChildBy("lightBulb0001_GES");
                if (node && 0 < node->getRenderableObjectCount()) {
                    auto mesh = node->getRenderableObject(0);
                    mesh->getMaterial()->getTechnique(0)->getPass(0)->getUniform("u_diffuse")->setValue(isON? veVec3(20.0f): color);
                    //mesh->getMaterial()->getTechnique(0)->getPass(0)->getUniform("u_alpha")->setValue(isON? 1.0f: 0.6f);
                }
                
                node = _attachedNode->findChildBy("lampshadeInner_GES");
                if (node && 0 < node->getRenderableObjectCount()) {
                    auto mesh = node->getRenderableObject(0);
                    mesh->getMaterial()->getTechnique(0)->getPass(0)->getUniform("u_diffuse")->setValue(isON? veVec3(4.f): color);
                    //mesh->getMaterial()->getTechnique(0)->getPass(0)->getUniform("u_alpha")->setValue(isON? 1.0f: 0.6f);
                }
                
            }
        }
        return false;
    }

};

void setRenderableObjectMat(veNode *node, veMaterial *material)
{
    if (0 < node->getRenderableObjectCount()) {
        for (size_t i = 0; i < node->getRenderableObjectCount(); ++i){
            auto ro = node->getRenderableObject(i);
            ro->setMaterial(material);
        }
    }
    for (size_t i = 0; i < node->getChildCount(); ++i){
        veNode *child = node->getChild(i);
        setRenderableObjectMat(child, material);
    }
}

class MeshTest : public BaseTest
{
public:
	MeshTest() {
		//int n = rand();
		veNode *root = _sceneManager->createNode("root");
        //auto reflectionMat = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "materials/Standard_Forward.vemtl", "Standard_Forward"));
		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/Aircraft/Aircraft.vem", "Aircraft"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			//transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
			transer->setScale(veVec3(0.05f));
			root->addChild(entity);
            //setRenderableObjectMat(entity, reflectionMat->getMaterial(0));
            
            
            {
                auto jetps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/jet.veparticle", "JetPS"));
                auto jetsmokeps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/jetsmoke.veparticle", "JetSmokePS"));
                veNode *leftJetNode = _sceneManager->createNode("LeftJetNode");
                leftJetNode->setMatrix(veMat4::transform(veVec3(2.5f, 1.8f, -2.5f), veVec3::UNIT_SCALE, veQuat(-veMath::HALF_PI, veVec3::UNIT_X)));
                leftJetNode->addRenderableObject(jetps);
                leftJetNode->addRenderableObject(jetsmokeps);
                //leftJetNode->setMask(leftJetNode->getMask() & ~LIGHTING_FLAG);
                entity->addChild(leftJetNode);
            }
            
            {
                auto jetps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/jet.veparticle", "JetPS"));
                auto jetsmokeps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/jetsmoke.veparticle", "JetSmokePS"));
                veNode *rightJetNode = _sceneManager->createNode("RightJetNode");
                rightJetNode->setMatrix(veMat4::transform(veVec3(-2.5f, 1.8f, -2.5f), veVec3::UNIT_SCALE, veQuat(-veMath::HALF_PI, veVec3::UNIT_X)));
                rightJetNode->addRenderableObject(jetps);
                rightJetNode->addRenderableObject(jetsmokeps);
                //rightJetNode->setMask(rightJetNode->getMask() & ~LIGHTING_FLAG);
                entity->addChild(rightJetNode);
            }
		}
        
        {
            veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "Plane-entity"));
            veTransformer *transer = new veTransformer;
            entity->addComponent(transer);
            //node->addComponent(GIZMO_COMPONENT.get());
            root->addChild(entity);
            transer->setScale(veVec3(0.07f));
            //transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
            
            auto mat = entity->findChildBy("Plane")->getRenderableObject(0)->getMaterial();
            mat->activateTechnique(mat->getTechnique("grid"));
            
        }
        
        
        {
            veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/shadowPlane.vem", "Plane-entity"));
            veTransformer *transer = new veTransformer;
            entity->addComponent(transer);
            //node->addComponent(GIZMO_COMPONENT.get());
            root->addChild(entity);
            transer->setScale(veVec3(0.07f));
            //transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
            
        }


        {
            veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/lamp/lamp.vem", "chair"));
            veTransformer *transer = new veTransformer;
            entity->addComponent(transer);
            //node->addComponent(GIZMO_COMPONENT.get());
            root->addChild(entity);
            entity->addComponent(new Handler());
            //transer->setScale(veVec3(10.0f));
            //transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
            {
                veNode *lightNode = _sceneManager->createNode("pointLightNode");
                veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point"));
                lightNode->addComponent(point);
                //veTransformer *lightTranser = new veTransformer;
                //lightNode->addComponent(lightTranser);
                //point->addComponent(new LightUpdater(20.0f, 10.0f));
                lightNode->setMatrix(veMat4::translation(veVec3(0.0f, 3.5f, 0.0f)));
                //lightNode->setMatrix(veMat4::lookAt(veVec3(10.0f, 9.0f, 17.5f), veVec3::ZERO, veVec3::UNIT_Y));
                
//                veNode *lightentity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
//                lightentity->setMatrix(veMat4::scale(veVec3(0.3f)));
//                lightNode->addChild(lightentity);
                point->setEnabled(false);
//                point->shadowEnable(true);
//                point->setUseSoftShadow(true);
//                point->setShadowSoftness(0.001f);
//                point->setColor(veVec3(1.0f, 1.0f, 1.0f));
//                point->setShadowBias(0.006f);
//                point->setIntensity(10.0f);
//                point->setAttenuationRange(20.0f);
                entity->addChild(lightNode);
            }
            //entity->addComponent(new veNodeManipulator);
        }
        
        {
            veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
            root->addChild(entity);

            veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
            veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
            player->start(122, 141);
            player->setLoopAnimation(true);
            player->setFrameRate(30.0f);
            entity->addComponent(player);

            float radius = 5.0f;

            auto updator = new UpdatorComponent;
            entity->addComponent(updator);
            updator->UpdateCallback = [=](veSceneManager *sm, veNode *node) {
                static float angle = 0.0f;
                node->setMatrix(veMat4::rotation(veQuat(angle, veVec3::UNIT_Y)) * veMat4::translation(veVec3(0.0f, 0.0f, -radius)) * veMat4::rotation(veQuat(veMath::HALF_PI, veVec3::NEGATIVE_UNIT_Y)) * veMat4::scale(0.1f));
                angle += sm->getDeltaTime();
            };
        }
//
//        {
//            veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/box.vem", "box-entity"));
//            //node->addComponent(GIZMO_COMPONENT.get());
//            veTransformer *transer = new veTransformer;
//            entity->addComponent(transer);
//            transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
//            //transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_X) * veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
//            transer->setScale(veVec3(2.0f));
//            
//            //auto mat = entity->findChildBy("Box001")->getRenderableObject(0)->getMaterial();
//            //mat->activateTechnique(mat->getTechnique("metals"));
//            
//            root->addChild(entity);
//        }

//		{
//			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "plane-0"));
//			//node->addComponent(new KeyboardInputer);
//			veTransformer *transer = new veTransformer;
//			entity->addComponent(transer);
//			transer->setPosition(veVec3(-5.0f, 5.0f, 0.0f));
//			transer->setScale(veVec3(2.0f));
//			root->addChild(entity);
//		}
//
//		{
//			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere-0"));
//			//node->addComponent(new KeyboardInputer);
//			veTransformer *transer = new veTransformer;
//			entity->addComponent(transer);
//			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
//			transer->setScale(veVec3(2.0f));
//			root->addChild(entity);
//
//		}
//
//		veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
//		{
//			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
//			//node->addComponent(new KeyboardInputer);
//			veTransformer *transer = new veTransformer;
//			entity->addComponent(transer);
//			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
//			transer->setScale(veVec3(0.3f));
//			transer->setPosition(veVec3(5.0f, 5.0f, 0.0f));
//			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
//			root->addChild(entity);
//
//			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
//			player->start();
//			player->setLoopAnimation(true);
//			player->attachNode(entity);
//		}
//
//		{
//			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-1"));
//			//node->addComponent(new KeyboardInputer);
//			veTransformer *transer = new veTransformer;
//			entity->addComponent(transer);
//			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
//			transer->setScale(veVec3(0.3f));
//			transer->setPosition(veVec3(5.0f, -5.0f, 0.0f));
//			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
//			root->addChild(entity);
//
//			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player1", animationContainer);
//			player->start();
//			player->setFrameRate(10.0f);
//			player->setLoopAnimation(true);
//			player->attachNode(entity);
//		}

//		auto debuger = new veOctreeDebuger(_sceneManager);
//		debuger->debugDrawBoundingBoxWireframe(true);
//		debuger->debugDrawOctree(false);
//		_sceneManager->getRootNode()->addRenderableObject(debuger);
        
        
        {
            veTexture *irradiance = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/Alexs_irradiance.png", "irradiance"));
            irradiance->setWrapMode(veTexture::CLAMP);
            irradiance->setFilterMode(veTexture::LINEAR);
            veTexture *radiance = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/Alexs_radiance.png", "radiance"));
            radiance->setWrapMode(veTexture::REPEAT);
            radiance->setFilterMode(veTexture::LINEAR_MIP_MAP_LINEAR);
            veTexture *lut = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/ibl_brdf_lut.png", "lut"));
            lut->setWrapMode(veTexture::CLAMP);
            lut->setFilterMode(veTexture::LINEAR);
            _camera->setIrradianceTexture(irradiance);
            _camera->setRadianceTexture(radiance);
            _camera->setLutTexture(lut);
        }
        
//        {
//            veNode *lightNode = _sceneManager->createNode("directionalLightNode");
//            veLight *directional = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional"));
//            lightNode->addComponent(directional);
//            veTransformer *lightTranser = new veTransformer;
//            //lightNode->addComponent(lightTranser);
//            //point->addComponent(new LightUpdater(20.0f, 10.0f));
//            directional->setIntensity(1.0f);
//            lightNode->setMatrix(veMat4::lookAt(veVec3(1.0f, 1.0f, 1.0f), veVec3::ZERO, veVec3::UNIT_Y));
//            
//            veNode *lightentity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
//            lightentity->setMatrix(veMat4::scale(veVec3(0.2f)));
//            lightNode->addChild(lightentity);
//            directional->setEnabled(true);
//            directional->shadowEnable(true);
//            directional->setUseSoftShadow(true);
//            directional->setShadowSoftness(0.001f);
//            directional->setShadowBias(0.006f);
//            directional->setIntensity(1.0f);
//            root->addChild(lightNode);
//        }
        
//        {
//            auto skyBox = new veSkyBox(500.0f);
//            veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-GrandCanyon.vemtl", "skybox-mats"));
//            skyBox->setMaterial(materials->getMaterial(0));
//            _cameraNode->addComponent(skyBox);
//        }
        
		_sceneManager->getRootNode()->addChild(root);
        _camera->setClearColor(veVec4(0.5f));
	}
	~MeshTest() {};

private:

};

#endif
