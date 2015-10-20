#ifndef _INTERSECTION_TEST_
#define _INTERSECTION_TEST_
#include "BaseTest.h"

class IntersectionHandler : public veComponent
{
public:
	IntersectionHandler()
	{
	}
	~IntersectionHandler() {}

	virtual bool handle(veNode *node, veSceneManager *sm, const veEvent &event) override {
		if (event.getEventType() & veEvent::VE_MOUSE_EVENT) {
			if (event.getEventType() == veEvent::VE_PRESS) {
				veVec2 screenCoords = veVec2(event.getMouseX(), event.getMouseY());
				start = sm->getVisualiser()->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
				end = sm->getVisualiser()->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
				veRay ray(start, end);
				ray.apply(sm);
				if (!ray.getIntersections().empty()) {
					for (auto &inters : ray.getIntersections()) {
					}
					veLog("Intersection!");
					return true;
				}
			}
			return false;
		}
	}

	virtual void update(veNode *node, veSceneManager *sm) override{
		debuger->debugDrawLine(start, end);
	}

	veDebuger *debuger;
	veVec3 start;
	veVec3 end;
};


class IntersectionTest : public BaseTest
{
public:
	IntersectionTest() {
		veNode *root = _sceneManager->createNode();
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem"));
			entity->setName("teapot");
			veNode *node = _sceneManager->createNode();
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(-5.0f, -5.0f, 0.0f));
			transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
			transer->setScale(veVec3(2.0f));
			root->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem"));
			entity->setName("plane");
			veNode *node = _sceneManager->createNode();
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(-5.0f, 5.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			root->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem"));
			entity->setName("sphere");
			veNode *node = _sceneManager->createNode();
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			root->addChild(node);

		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem"));
			entity->setName("laoshu-1");
			veNode *node = _sceneManager->createNode();
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			transer->setPosition(veVec3(5.0f, 5.0f, 0.0f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			root->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim"));
			animationContainer->start();
			animationContainer->setLoopAnimation(true);
			entity->setAnimationContainer(animationContainer);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem"));
			entity->setName("laoshu-2");
			veNode *node = _sceneManager->createNode();
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			transer->setPosition(veVec3(5.0f, -5.0f, 0.0f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			root->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim"));
			animationContainer->start();
			animationContainer->setFrameRate(10.0f);
			animationContainer->setLoopAnimation(true);
			entity->setAnimationContainer(animationContainer);
		}

		auto debuger = new veOctreeDebuger;
		debuger->debugDrawBoundingBoxWireframe(true);
		debuger->debugDrawOctree(true);
		debuger->setDebugDrawColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
		_sceneManager->getRootNode()->addRenderableObject(debuger);

		auto ih = new IntersectionHandler;
		ih->debuger = debuger;
		root->addComponent(ih);

		_sceneManager->getRootNode()->addChild(root);
	}
	~IntersectionTest() {};

private:

};

#endif