#ifndef _SHADOW_TEST_
#define _SHADOW_TEST_
#include "BaseTest.h"

#define LIGHT_MASK 0x0000000f

class ShadowTest : public BaseTest
{
public:
	ShadowTest() {

		veNode *root = _sceneManager->createNode("root");
		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			//entity->getMaterialArray()->getMaterial(0)->activateTechnique(entity->getMaterialArray()->getMaterial(0)->getTechnique("CookTorrance_OrenNayarRendering"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
			transer->setRotation(veQuat(veMath::veRadian(40.0), veVec3(1.0f, 1.0f, 1.0f)));
			root->addChild(entity);
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			//entity->getMaterialArray()->getMaterial(0)->activateTechnique(entity->getMaterialArray()->getMaterial(0)->getTechnique("CookTorrance_OrenNayarRendering"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(0.0f, -10.0f, 0.0f));
			root->addChild(entity);
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/cornellbox.vem", "cornellbox"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			entity->setMask(~LIGHT_MASK);
			transer->setScale(veVec3(20.0f));
			root->addChild(entity);
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere"));
			//entity->getMaterialArray()->getMaterial(0)->activateTechnique(entity->getMaterialArray()->getMaterial(0)->getTechnique("CookTorrance_OrenNayarRendering"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setPosition(veVec3(5.0f, 2.0f, 2.0f));
			root->addChild(entity);
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
			//node->addComponent(new KeyboardInputer);
			//veTransformer *transer = new veTransformer;
			//node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			//transer->setScale(veVec3(0.3f));
			root->addChild(entity);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
			player->start(122, 141);
			player->setLoopAnimation(true);
			player->attachNode(entity);

			float radius = 7.0f;
            
            auto updator = new UpdatorComponent;
            entity->addComponent(updator);
            updator->UpdateCallback = [=](veSceneManager *sm, veNode *node) {
                static float angle = 0.0f;
                node->setMatrix(veMat4::rotation(veQuat(angle, veVec3::UNIT_Y)) * veMat4::translation(veVec3(0.0f, -10.0f, -radius)) * veMat4::rotation(veQuat(veMath::HALF_PI, veVec3::NEGATIVE_UNIT_Y)) * veMat4::scale(0.3f));
                angle += sm->getDeltaTime();
            };
		}

		{
            veNode *lightNode = _sceneManager->createNode("spotNode");
			veLight *spot = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/spot0.velight", "spot0"));
            lightNode->addComponent(spot);
			veTransformer *lightTranser = new veTransformer;
			lightNode->addComponent(lightTranser);
			spot->setIntensity(1.0f);
			spot->setColor(veVec3(1.0f, 0.0f, 0.0f));
			lightTranser->setPosition(veVec3(0.0f, 10.0f, 10.0f));
			lightTranser->setRotation(veQuat(veMath::QUARTER_PI, veVec3::NEGATIVE_UNIT_X));

			veNode *lightentity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
			lightentity->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightentity->setMask(~LIGHT_MASK);
			lightNode->addChild(lightentity);
			lightNode->setMask(LIGHT_MASK);
			spot->shadowEnable(true);
			spot->setUseSoftShadow(true);
			spot->setShadowSoftness(0.002f);
			spot->setShadowBias(0.0015f);
			root->addChild(lightNode);
		}

		//{
		//	veLight *spot = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/spot0.velight", "spot1"));
		//	veTransformer *lightTranser = new veTransformer;
		//	spot->addComponent(lightTranser);
		//	spot->addComponent(new LightUpdater(5.0f, -5.0f));
		//	spot->setIntensity(1.0f);
		//	lightTranser->setPosition(veVec3(0.0f, 0.0f, -5.0f));
		//	lightTranser->setRotation(veQuat(veMath::PI, veVec3(0.0f, 1.0f, 0.0f)));

		//	veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
		//	veNode *lightModel = _sceneManager->createNode("lightnode2");
		//	lightModel->addRenderableObject(lightentity);
		//	lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
		//	lightModel->setMask(~LIGHT_MASK);
		//	spot->addChild(lightModel);
		//	spot->setMask(LIGHT_MASK);
		//	root->addChild(spot);
		//}


		{
            veNode *lightNode = _sceneManager->createNode("directionalNode");
			veLight *directional = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional0"));
            lightNode->addComponent(directional);
			veTransformer *lightTranser = new veTransformer;
			lightNode->addComponent(lightTranser);
			lightTranser->setPosition(veVec3(10.0f, 0.0f, 0.0f));
			lightTranser->setRotation(veQuat(veMath::HALF_PI, veVec3(0.0f, 1.0f, 0.0f)));
			directional->setIntensity(0.3f);

			veNode *lightentity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional0-sphere"));
			lightentity->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightentity->setMask(~LIGHT_MASK);
			lightNode->addChild(lightentity);
			lightNode->setMask(LIGHT_MASK);
			directional->shadowEnable(true);
			directional->setUseSoftShadow(true);
			directional->setShadowSoftness(0.002f);
			root->addChild(lightNode);
		}

		//{
		//	veLight *directional = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional1"));
		//	veTransformer *lightTranser = new veTransformer;
		//	directional->addComponent(lightTranser);
		//	lightTranser->setPosition(veVec3(0.0f, 0.0f, 10.0f));
		//	directional->setIntensity(0.3f);

		//	veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional0-sphere"));
		//	veNode *lightModel = _sceneManager->createNode("lightnode0");
		//	lightModel->addRenderableObject(lightentity);
		//	lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
		//	lightModel->setMask(~LIGHT_MASK);
		//	directional->addChild(lightModel);
		//	directional->setMask(LIGHT_MASK);
		//	root->addChild(directional);
		//}

		{
            veNode *lightNode = _sceneManager->createNode("pointNode");
			veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
            lightNode->addComponent(point);
			veTransformer *lightTranser = new veTransformer;
			lightNode->addComponent(lightTranser);
			lightNode->addComponent(new LightUpdater(point, 5.0f, -3.0f));
			point->setIntensity(0.6f);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 10.0f));

			veNode *lightentity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
			lightentity->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightentity->setMask(~LIGHT_MASK);
			lightNode->addChild(lightentity);
			lightNode->setMask(LIGHT_MASK);
			point->shadowEnable(true);
			point->setUseSoftShadow(true);
			point->setShadowSoftness(0.05f);
			point->setShadowBias(0.001f);
			root->addChild(lightNode);
		}

		//{
		//	veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point1"));
		//	veTransformer *lightTranser = new veTransformer;
		//	point->addComponent(lightTranser);
		//	point->addComponent(new LightUpdater(5.0f, -5.0f));
		//	point->setIntensity(0.6f);

		//	veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
		//	veNode *lightModel = _sceneManager->createNode("lightnode1");
		//	lightModel->addRenderableObject(lightentity);
		//	lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
		//	lightModel->setMask(~LIGHT_MASK);
		//	point->addChild(lightModel);
		//	point->setMask(LIGHT_MASK);
		//	point->setUseSoftShadow(true);
		//	root->addChild(point);
		//}

		{
			auto skyBox = _sceneManager->createSkyBox("skybox");
			_sceneManager->setSkyBox(skyBox);
			veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-snow.vemtl", "skybox-mats"));
			skyBox->setMaterial(materials->getMaterial(0));
		}

		_sceneManager->setAmbientColor(veVec3::ZERO);
		auto debuger = new veOctreeDebuger(_sceneManager);
		debuger->debugDrawBoundingBoxWireframe(false);
		debuger->debugDrawOctree(true);
		//_sceneManager->getRootNode()->addRenderableObject(debuger);
		_sceneManager->getRootNode()->addChild(root);
	};
	~ShadowTest() {};

protected:


};

#endif
