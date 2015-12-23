#ifndef _LIGHT_TEST_
#define _LIGHT_TEST_
#include "BaseTest.h"

class LightTest : public BaseTest
{
public:
	LightTest() {

		veNode *root = _sceneManager->createNode("root");

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "plane"));
			veNode *node = _sceneManager->createNode("plane");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setRotation(veQuat(veMath::HALF_PI, veVec3::NEGATIVE_UNIT_X));
			transer->setScale(veVec3(110.0f));
			root->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
			veNode *node = _sceneManager->createNode("laoshu");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.5f));
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
			spot->addComponent(new LightUpdater(50.0f, 50.0f));
			spot->setIntensity(1.0f);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode2");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			spot->addChild(lightModel);
			spot->setUseSoftShadow(true);
			spot->setShadowSoftness(0.005f);
			spot->setShadowBias(0.0015f);
			root->addChild(spot);
		}


		{
			veLight *directional = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional0"));
			veTransformer *lightTranser = new veTransformer;
			directional->addComponent(lightTranser);
			lightTranser->setPosition(veVec3(0.0f, -20.0f, 0.0f));
			lightTranser->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_X));
			directional->setIntensity(0.8f);

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode0");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			directional->addChild(lightModel);
			directional->setUseSoftShadow(true);
			directional->setShadowSoftness(0.005f);
			root->addChild(directional);
		}


		for (int x = -50; x <= 50; x += 10) {
			for (int z = -50; z <= 50; z += 10) {
				if (x != 0 || z != 0) {
					veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
					veNode *node = _sceneManager->createNode("node");
					node->addRenderableObject(entity);
					veTransformer *transer = new veTransformer;
					node->addComponent(transer);
					transer->setScale(veVec3(2.0f));
					transer->setPosition(veVec3(x, 0.0f, z));
					root->addChild(node);
				}
			}
		}

		for (int x = -45; x <= 45; x += 10) {
			for (int z = -45; z <= 45; z += 10) {
				veNode *node = _sceneManager->createNode("pointNode");
				veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point1"));
				point->addComponent(new LightUpdater(3.0f, 0.0f));
				point->setIntensity(1.0f);
				point->setAttenuationRange(10.0f);
				veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
				veNode *lightModel = _sceneManager->createNode("lightnode1");
				lightModel->addRenderableObject(lightentity);
				lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
				point->addChild(lightModel);
				node->addChild(point);
				root->addChild(node);

				veTransformer *lightTranser = new veTransformer;
				node->addComponent(lightTranser);
				lightTranser->setPosition(veVec3(x, 2.0f, z));
			}
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