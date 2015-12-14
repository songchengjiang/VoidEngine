#ifndef _LIGHT_TEST_
#define _LIGHT_TEST_
#include "BaseTest.h"

#define LIGHT_MASK 0x0000000f

class LightTest : public BaseTest
{
public:
	LightTest() {

		veRenderableObject *obj = nullptr;
		veNode *root = _sceneManager->createNode("root");
		root->setMatrix(veMat4::rotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X)));
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			entity->getMaterialArray()->getMaterial(0)->activateTechnique(entity->getMaterialArray()->getMaterial(0)->getTechnique("CookTorrance_OrenNayarRendering"));
			veNode *node = _sceneManager->createNode("node0");
			node->addRenderableObject(entity);
			obj = entity;
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
			transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_X));
			root->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "plane"));
			veNode *node = _sceneManager->createNode("node1");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(20.0f));
			root->addChild(node);
		}

		{
			veNode *node = _sceneManager->createNode("node2");
			node->addRenderableObject(obj);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
			transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
			root->addChild(node);
		}


		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere"));
			entity->getMaterialArray()->getMaterial(0)->activateTechnique(entity->getMaterialArray()->getMaterial(0)->getTechnique("CookTorrance_OrenNayarRendering"));
			veNode *node = _sceneManager->createNode("node3");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(5.0f, 0.0f, 2.0f));
			root->addChild(node);
		}


		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
			veNode *node = _sceneManager->createNode("node4");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_X));
			root->addChild(node);
			node->setMask(~LIGHT_MASK);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
			player->start();
			player->setLoopAnimation(true);
			player->attachEntity(entity);
		}

		{
			veNode *node = _sceneManager->createNode("directional0Node");
			veLight *directional0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional0"));
			node->addRenderableObject(directional0);
			veTransformer *lightTranser = new veTransformer;
			node->addComponent(lightTranser);
			node->addComponent(new LightUpdater(10.0f, 10.0f));
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			directional0->setIntensity(veMath::veRandomUnitization());

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode0");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightModel->setMask(~LIGHT_MASK);
			node->addChild(lightModel);
			node->setMask(LIGHT_MASK);
			root->addChild(node);
		}

		{
			veNode *node = _sceneManager->createNode("point0Node");
			veLight *point0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
			node->addRenderableObject(point0);
			veTransformer *lightTranser = new veTransformer;
			node->addComponent(lightTranser);
			node->addComponent(new LightUpdater(5.0f, 5.0f));
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			point0->setIntensity(veMath::veRandomUnitization());

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode1");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightModel->setMask(~LIGHT_MASK);
			node->addChild(lightModel);
			node->setMask(LIGHT_MASK);
			root->addChild(node);
		}

		{
			veNode *node = _sceneManager->createNode("spot0Node");
			veLight *spot0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/spot0.velight", "spot0"));
			node->addRenderableObject(spot0);
			veTransformer *lightTranser = new veTransformer;
			node->addComponent(lightTranser);
			node->addComponent(new LightUpdater(10.0f, 10.0f));
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			spot0->setIntensity(2.0);

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode2");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightModel->setMask(~LIGHT_MASK);
			node->addChild(lightModel);
			node->setMask(LIGHT_MASK);
			root->addChild(node);
		}

		{
			auto skyBox = _sceneManager->createSkyBox("skybox");
			_sceneManager->setSkyBox(skyBox);
			veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-snow.vemtl", "skybox-mats"));
			skyBox->setMaterialArray(materials);
		}

		auto debuger = new veOctreeDebuger(_sceneManager);
		debuger->debugDrawBoundingBoxWireframe(false);
		debuger->debugDrawOctree(true);
		//_sceneManager->getRootNode()->addRenderableObject(debuger);
		_sceneManager->getRootNode()->addChild(root);
	};
	~LightTest() {};

protected:


};

#endif