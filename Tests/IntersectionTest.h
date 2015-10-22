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
			if (event.getEventType() == veEvent::VE_PRESS && event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_LEFT) {
				veVec2 screenCoords = veVec2(event.getMouseX(), event.getMouseY());
				veVec3 start = sm->getVisualiser()->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
				veVec3 end = sm->getVisualiser()->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
				veRay ray(start, end);
				ray.apply(sm);
				if (!ray.getIntersections().empty()) {
					char str[256];
					for (auto &inters : ray.getIntersections()) {
						_lines.push_back(std::make_pair(inters.worldPosition, inters.worldPosition + inters.worldNormal * 2.0f));
						sprintf(str, "Intersection info: position(%f, %f, %f) \n \t \t normal(%f, %f, %f)"
							, inters.worldPosition.x(), inters.worldPosition.y(), inters.worldPosition.z()
							, inters.worldNormal.x(), inters.worldNormal.y(), inters.worldNormal.z());
						veLog(str);
					}
					return true;
				}
			}
			return false;
		}

		if (event.getEventType() & veEvent::VE_KEYBOARD_EVENT) {
			if (event.getEventType() == veEvent::VE_DOWN && event.getKeySymbol() == veEvent::VE_KEY_S) {
				_animationContainer->pause();
			}
		}

		return false;
	}

	virtual void update(veNode *node, veSceneManager *sm) override{
		for (auto &line : _lines) {
			debuger->debugDrawLine(line.first, line.second, veVec4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}

	veDebuger *debuger;
	veAnimationContainer* _animationContainer;
	std::vector<std::pair<veVec3, veVec3> > _lines;
};


class IntersectionTest : public BaseTest
{
public:
	IntersectionTest() {
		auto ih = new IntersectionHandler;
		veNode *root = _sceneManager->createNode();
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
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
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "plane"));
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
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere"));
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
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
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

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim-0"));
			animationContainer->start();
			animationContainer->setLoopAnimation(true);
			entity->setAnimationContainer(animationContainer);
			ih->_animationContainer = animationContainer;
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-1"));
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

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim-1"));
			animationContainer->start();
			animationContainer->setFrameRate(10.0f);
			animationContainer->setLoopAnimation(true);
			entity->setAnimationContainer(animationContainer);
		}

		auto debuger = new veOctreeDebuger;
		debuger->debugDrawBoundingBoxWireframe(true, veVec4(1.0f, 0.0f, 0.0f, 0.1f));
		debuger->debugDrawOctree(true, veVec4(1.0f, 1.0f, 1.0f, 0.1f));
		debuger->setLineWidth(2.0f);
		_sceneManager->getRootNode()->addRenderableObject(debuger);

		ih->debuger = debuger;
		root->addComponent(ih);

		_sceneManager->getRootNode()->addChild(root);
	}
	~IntersectionTest() {};

private:

};

#endif