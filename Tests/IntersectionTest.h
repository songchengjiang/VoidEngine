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

	virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override {
		if (event.getEventType() & veEvent::VE_MOUSE_EVENT || event.getEventType() & veEvent::VE_TOUCH_EVENT) {
			veVec2 screenCoords;
			bool state = false;
			if (event.getEventType() == veEvent::VE_PRESS && event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_LEFT) {
				screenCoords = veVec2(event.getMouseX(), event.getMouseY());
				state = true;
			}
			else if (event.getEventType() == veEvent::VE_TOUCH_START) {
				auto touchs = event.getTouches();
				if (touchs.size() == 1) {
					screenCoords = veVec2(touchs[0].x, touchs[0].y);
					state = true;
				}
			}

			if (state) {
				veVec3 start = viewer->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
				veVec3 end = viewer->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
				if (!_ray.valid()) {
					_ray = sm->createRay(start, end);
				}
				_ray->setStart(start);
				_ray->setEnd(end);
				_ray->apply(sm);
                if (!_ray->getIntersections().empty()) {
                    for (auto &inters : _ray->getIntersections()) {
                        this->_lines.push_back(std::make_pair(inters.worldPosition, inters.worldPosition + inters.worldNormal * 2.0f));
                        veLog("Intersection info: position(%f, %f, %f) \n \t \t normal(%f, %f, %f)\n"
                              , inters.worldPosition.x(), inters.worldPosition.y(), inters.worldPosition.z()
                              , inters.worldNormal.x(), inters.worldNormal.y(), inters.worldNormal.z());
                    }
                }
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

	virtual void update(veSceneManager *sm) override{
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
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setPosition(veVec3(-5.0f, -5.0f, 0.0f));
			transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
			transer->setScale(veVec3(2.0f));
			root->addChild(entity);
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "plane"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setPosition(veVec3(-5.0f, 5.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			root->addChild(entity);
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			root->addChild(entity);

		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			transer->setPosition(veVec3(5.0f, 5.0f, 0.0f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			root->addChild(entity);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim-0"));
			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
			player->start();
			player->setLoopAnimation(true);
			player->attachNode(entity);

			ih->_animationPlayer = player;
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-1"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			transer->setPosition(veVec3(5.0f, -5.0f, 0.0f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			root->addChild(entity);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim-1"));
			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player1", animationContainer);
			player->start();
			player->setFrameRate(10.0f);
			player->setLoopAnimation(true);
			player->attachNode(entity);
		}

		auto debuger = new veOctreeDebuger(_sceneManager);
		debuger->debugDrawBoundingBoxWireframe(true, veVec4(1.0f, 0.0f, 0.0f, 0.1f));
		debuger->debugDrawOctree(true, veVec4(1.0f, 1.0f, 1.0f, 0.1f));
		//debuger->setLineWidth(2.0f);
		_sceneManager->getRootNode()->addRenderableObject(debuger);

		ih->debuger = debuger;
		root->addComponent(ih);

		_sceneManager->getRootNode()->addChild(root);
	}
	~IntersectionTest() {};

private:

};

#endif
