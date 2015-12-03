#ifndef _SHADOW_TEST_
#define _SHADOW_TEST_
#include "BaseTest.h"

#define LIGHT_MASK 0x0000000f

class ShadowTest : public BaseTest
{
public:
	ShadowTest() {

		veNode *root = _sceneManager->createNode("root");
		root->setMatrix(veMat4::rotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X)));
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
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere"));
			//entity->getMaterialArray()->getMaterial(0)->activateTechnique(entity->getMaterialArray()->getMaterial(0)->getTechnique("CookTorrance_OrenNayarRendering"));
			veNode *node = _sceneManager->createNode("node3");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(5.0f, 0.0f, 2.0f));
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
			transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_X));
			root->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
			player->start();
			player->setLoopAnimation(true);
			player->attachEntity(entity);
		}

		{
			veLight *spot0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/spot0.velight", "spot0"));
			veTransformer *lightTranser = new veTransformer;
			spot0->addComponent(lightTranser);
			spot0->addComponent(new LightUpdater(5.0f, 5.0f));
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			spot0->shadowEnable(true);
			spot0->setShadowResolution(veVec2(512.0f));
			spot0->setShadowBias(0.01f);

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode2");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightModel->setMask(~LIGHT_MASK);
			spot0->addChild(lightModel);
			spot0->setMask(LIGHT_MASK);
			root->addChild(spot0);

			//{
			//	spot0->_shadowTexture = _sceneManager->createTexture(std::string("-ShadowTexture"), veTexture::TEXTURE_2D);
			//	veNode *node = _sceneManager->createNode("node10");
			//	veTransformer *transer = new veTransformer;
			//	node->addComponent(transer);
			//	transer->setPosition(veVec3(-0.5f, -0.5f, 0.0f));
			//	transer->setScale(veVec3(0.5f));

			//	auto overlay = _sceneManager->createImage("image0", spot0->getShadowTexture());
			//	node->addRenderableObject(overlay);

			//	_sceneManager->getRootNode()->addChild(node);
			//}
		}

		{
			veLight *directional0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional0"));
			veTransformer *lightTranser = new veTransformer;
			directional0->addComponent(lightTranser);
			directional0->addComponent(new LightUpdater(10.0f, 10.0f));
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			//directional0->setIntensity(veMath::veRandomUnitization());
			directional0->shadowEnable(true);
			directional0->setShadowArea(veVec2(20.0f));
			directional0->setShadowResolution(veVec2(512.0f));
			directional0->setShadowBias(0.0003f);

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode0");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightModel->setMask(~LIGHT_MASK);
			directional0->addChild(lightModel);
			directional0->setMask(LIGHT_MASK);
			root->addChild(directional0);
		}

		//{
		//	veLight *point0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
		//	veTransformer *lightTranser = new veTransformer;
		//	point0->addComponent(lightTranser);
		//	point0->addComponent(new LightUpdater);
		//	lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
		//	point0->setIntensity(veMath::veRandomUnitization());

		//	veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
		//	veNode *lightModel = _sceneManager->createNode("lightnode1");
		//	lightModel->addRenderableObject(lightentity);
		//	lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
		//	lightModel->setMask(~LIGHT_MASK);
		//	point0->addChild(lightModel);
		//	point0->setMask(LIGHT_MASK);
		//	root->addChild(point0);
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