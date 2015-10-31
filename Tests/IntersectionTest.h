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
				if (!_ray.valid()) {
					_ray = sm->createRay(start, end);
				}
				_ray->setStart(start);
				_ray->setEnd(end);
				_ray->apply(sm, [this] {
					if (!_ray->getIntersections().empty()) {
						for (auto &inters : _ray->getIntersections()) {
							this->_lines.push_back(std::make_pair(inters.worldPosition, inters.worldPosition + inters.worldNormal * 2.0f));
							veLog("Intersection info: position(%f, %f, %f) \n \t \t normal(%f, %f, %f)\n"
								, inters.worldPosition.x(), inters.worldPosition.y(), inters.worldPosition.z()
								, inters.worldNormal.x(), inters.worldNormal.y(), inters.worldNormal.z());
						}
					}
				});
			}
			return false;
		}

		if (event.getEventType() & veEvent::VE_KEYBOARD_EVENT) {
			if (event.getEventType() == veEvent::VE_DOWN && event.getKeySymbol() == veEvent::VE_KEY_S) {
				_animationPlayer->pause();
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
	veAnimationPlayer* _animationPlayer;
	std::vector<std::pair<veVec3, veVec3> > _lines;
	VE_Ptr<veRay> _ray;
};


class IntersectionTest : public BaseTest
{
public:
	IntersectionTest() {
		auto ih = new IntersectionHandler;
		veNode *root = _sceneManager->createNode("root");
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			entity->setName("teapot");
			veNode *node = _sceneManager->createNode("node0");
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
			veNode *node = _sceneManager->createNode("node1");
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
			veNode *node = _sceneManager->createNode("node2");
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
			veNode *node = _sceneManager->createNode("node3");
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
			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
			player->start();
			player->setLoopAnimation(true);
			player->attachEntity(entity);

			ih->_animationPlayer = player;
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-1"));
			entity->setName("laoshu-2");
			veNode *node = _sceneManager->createNode("node4");
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
			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player1", animationContainer);
			player->start();
			player->setFrameRate(10.0f);
			player->setLoopAnimation(true);
			player->attachEntity(entity);
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