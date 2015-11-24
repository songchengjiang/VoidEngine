#ifndef _CAMERA_TEST_
#define _CAMERA_TEST_
#include "BaseTest.h"

int CURRENT_CAMERA = 0;

class keyboardInput : public veComponent 
{
public:
	keyboardInput()
	{}
	~keyboardInput(){}

	virtual bool handle(veNode *node, veSceneManager *sm, const veEvent &event) override{
		if (event.getEventType() == veEvent::VE_DOWN) {
			switch (event.getKeySymbol())
			{
			case veEvent::VE_KEY_LEFT:
			{
				--CURRENT_CAMERA;
				if (CURRENT_CAMERA < 0) CURRENT_CAMERA = int(node->getChildCount()) - 1;
				auto cam = static_cast<veCamera *>(node->getChild(CURRENT_CAMERA));
				sm->setCamera(cam);
			}
				break;

			case veEvent::VE_KEY_RIGHT:
			{
				++CURRENT_CAMERA;
				if ((int)node->getChildCount() <= CURRENT_CAMERA) CURRENT_CAMERA = 0;
				auto cam = static_cast<veCamera *>(node->getChild(CURRENT_CAMERA));
				sm->setCamera(cam);
			}
				break;

			default:
				break;
			}
		}
		return false;
	}

	virtual void update(veNode *node, veSceneManager *sm) override {

	}
};

class CameraTest : public BaseTest
{
public:
	CameraTest() {
		veNode *root = _sceneManager->createNode("root");
		{
			veNode *node = _sceneManager->createNode("node0");
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(1.0f));
			//transer->setRotation(veQuat(veMath::PI, veVec3::UNIT_X));
			root->addChild(node);
		}

		{
			veCamera *cam0 = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cam0.vecamera", "cam0"));
			veCamera *cam1 = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cam1.vecamera", "cam1"));
			veCamera *cam2 = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cam2.vecamera", "cam2"));
			veCamera *cam3 = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cam3.vecamera", "cam3"));
			//cam1->setClearColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
			//cam2->setClearColor(veVec4(0.0f, 1.0f, 0.0f, 1.0f));
			//cam3->setClearColor(veVec4(0.0f, 0.0f, 1.0f, 1.0f));
			veNode *cameras = _sceneManager->createNode("cameras node");
			cameras->addChild(cam0);
			cameras->addChild(cam1);
			cameras->addChild(cam2);
			cameras->addChild(cam3);
			cameras->addComponent(new keyboardInput());
			root->addChild(cameras);
		}

		_sceneManager->getRootNode()->addChild(root);
	}
	~CameraTest() {};

private:

};

#endif