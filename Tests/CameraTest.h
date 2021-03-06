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

	virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override{
		if (_attachedNodeList.empty()) return false;

		if (event.getEventType() == veEvent::VE_DOWN) {
			switch (event.getKeySymbol())
			{
			case veEvent::VE_KEY_LEFT:
			{
				--CURRENT_CAMERA;
				if (CURRENT_CAMERA < 0) CURRENT_CAMERA = int(_attachedNodeList[0]->getChildCount()) - 1;
				auto cam = static_cast<veCamera *>(_attachedNodeList[0]->getChild(CURRENT_CAMERA));
				viewer->setCamera(cam);
			}
				break;

			case veEvent::VE_KEY_RIGHT:
			{
				++CURRENT_CAMERA;
				if ((int)_attachedNodeList[0]->getChildCount() <= CURRENT_CAMERA) CURRENT_CAMERA = 0;
				auto cam = static_cast<veCamera *>(_attachedNodeList[0]->getChild(CURRENT_CAMERA));
				viewer->setCamera(cam);
			}
				break;

			default:
				break;
			}
		}
		return false;
	}

	virtual void beforeUpdate(veSceneManager *sm) override {

	}
};

class CameraTest : public BaseTest
{
public:
	CameraTest() {
		veNode *root = _sceneManager->createNode("root");
		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(1.0f));
			//transer->setRotation(veQuat(veMath::PI, veVec3::UNIT_X));
			root->addChild(entity);
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