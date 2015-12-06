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
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			//entity->getMaterialArray()->getMaterial(0)->activateTechnique(entity->getMaterialArray()->getMaterial(0)->getTechnique("CookTorrance_OrenNayarRendering"));
			veNode *node = _sceneManager->createNode("node0");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
			transer->setRotation(veQuat(veMath::veRadian(40.0), veVec3(1.0f, 1.0f, 1.0f)));
			root->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			//entity->getMaterialArray()->getMaterial(0)->activateTechnique(entity->getMaterialArray()->getMaterial(0)->getTechnique("CookTorrance_OrenNayarRendering"));
			veNode *node = _sceneManager->createNode("node11");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(0.0f, -10.0f, 0.0f));
			root->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/box.vem", "box"));
			veNode *node = _sceneManager->createNode("node1");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(20.0f));
			root->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere"));
			//entity->getMaterialArray()->getMaterial(0)->activateTechnique(entity->getMaterialArray()->getMaterial(0)->getTechnique("CookTorrance_OrenNayarRendering"));
			veNode *node = _sceneManager->createNode("node3");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(5.0f, 2.0f, 2.0f));
			root->addChild(node);
			node->setMask(~LIGHT_MASK);
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
			root->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
			player->start();
			player->setLoopAnimation(true);
			player->attachEntity(entity);
		}

		{
			veLight *spot = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/spot0.velight", "spot0"));
			veTransformer *lightTranser = new veTransformer;
			spot->addComponent(lightTranser);
			spot->addComponent(new LightUpdater(5.0f, 5.0f));
			spot->setIntensity(1.0f);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode2");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightModel->setMask(~LIGHT_MASK);
			spot->addChild(lightModel);
			spot->setMask(LIGHT_MASK);
			root->addChild(spot);
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
			veLight *directional = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional0"));
			veTransformer *lightTranser = new veTransformer;
			directional->addComponent(lightTranser);
			lightTranser->setPosition(veVec3(10.0f, 0.0f, 0.0f));
			lightTranser->setRotation(veQuat(veMath::HALF_PI, veVec3(0.0f, 1.0f, 0.0f)));
			directional->setIntensity(0.3f);

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode0");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightModel->setMask(~LIGHT_MASK);
			directional->addChild(lightModel);
			directional->setMask(LIGHT_MASK);
			root->addChild(directional);
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
			veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
			veTransformer *lightTranser = new veTransformer;
			point->addComponent(lightTranser);
			point->addComponent(new LightUpdater(5.0f, -3.0f));
			point->setIntensity(0.6f);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 10.0f));

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode1");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightModel->setMask(~LIGHT_MASK);
			point->addChild(lightModel);
			point->setMask(LIGHT_MASK);
			root->addChild(point);
			//point0->setVisible(true);
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
		//	root->addChild(point);
		//}

		{
			auto skyBox = _sceneManager->createSkyBox("skybox");
			_sceneManager->setSkyBox(skyBox);
			veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-snow.vemtl", "skybox-mats"));
			skyBox->setMaterialArray(materials);
		}

		auto debuger = new veOctreeDebuger;
		debuger->debugDrawBoundingBoxWireframe(false);
		debuger->debugDrawOctree(true);
		//_sceneManager->getRootNode()->addRenderableObject(debuger);
		_sceneManager->getRootNode()->addChild(root);
	};
	~ShadowTest() {};

protected:


};

#endif