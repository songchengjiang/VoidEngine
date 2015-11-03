#ifndef _LIGHT_TEST_
#define _LIGHT_TEST_
#include "BaseTest.h"

class LightTest : public BaseTest
{
public:
	LightTest() {
		_sceneManager->loadLightConfiguration("lights/point.velightemplates");

		veRenderableObject *obj = nullptr;
		veNode *root = _sceneManager->createNode("root");
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			veNode *node = _sceneManager->createNode("node0");
			node->addRenderableObject(entity);
			obj = entity;
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
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
			transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
			root->addChild(node);
		}

		{
			veLight *directional0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional0"));
			veTransformer *lightTranser = new veTransformer;
			directional0->addComponent(lightTranser);
			directional0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			auto param = directional0->getParameter("intensity");
			param->set(veMath::veRandomUnitization());

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode0");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			directional0->addChild(lightModel);
			root->addChild(directional0);
		}

		{
			veLight *point0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
			veTransformer *lightTranser = new veTransformer;
			point0->addComponent(lightTranser);
			point0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			auto param = point0->getParameter("intensity");
			param->set(veMath::veRandomUnitization());

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode1");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			point0->addChild(lightModel);
			root->addChild(point0);
		}

		{
			veLight *spot0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/spot0.velight", "spot0"));
			veTransformer *lightTranser = new veTransformer;
			spot0->addComponent(lightTranser);
			spot0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			auto param = spot0->getParameter("intensity");
			param->set(veMath::veRandomUnitization());

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode2");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			spot0->addChild(lightModel);
			root->addChild(spot0);
		}

		auto debuger = new veOctreeDebuger;
		debuger->debugDrawBoundingBoxWireframe(false);
		debuger->debugDrawOctree(true);
		_sceneManager->getRootNode()->addRenderableObject(debuger);
		_sceneManager->getRootNode()->addChild(root);
	};
	~LightTest() {};

protected:


};

#endif