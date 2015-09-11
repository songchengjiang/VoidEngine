#ifndef _CAMERA_TEST_
#define _CAMERA_TEST_
#include "BaseTest.h"

int CURRENT_CAMERA = 0;

class keyboardInput : public veInputer 
{
public:
	keyboardInput(veVisualiser *vs)
		: _visualiser(vs)
	{}
	~keyboardInput(){}

	virtual bool handle(veNode *node, veVisualiser *vs, const veEvent &event) {
		if (event.getEventType() == veEvent::VE_DOWN) {
			switch (event.getKeySymbol())
			{
			case veEvent::VE_KEY_LEFT:
			{
				--CURRENT_CAMERA;
				if (CURRENT_CAMERA < 0) CURRENT_CAMERA = node->getChildCount() - 1;
				auto cam = static_cast<veCamera *>(node->getChild(CURRENT_CAMERA));
				_visualiser->setCamera(cam);
			}
				break;

			case veEvent::VE_KEY_RIGHT:
			{
				++CURRENT_CAMERA;
				if (node->getChildCount() <= CURRENT_CAMERA) CURRENT_CAMERA = 0;
				auto cam = static_cast<veCamera *>(node->getChild(CURRENT_CAMERA));
				_visualiser->setCamera(cam);
			}
				break;

			default:
				break;
			}
		}
		return false;
	}

private:

	veVisualiser *_visualiser;
};

class CameraTest : public BaseTest
{
public:
	CameraTest() {
		veNode *root = new veNode;
		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/teapot.vem"));
			auto mat = node->getChild(0)->getChild(0)->getRenderableObject(0)->getMaterial();
			mat->activateTechnique(mat->getTechnique("forwardRender"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(1.0f));
			//transer->setRotation(veQuat(veMath::PI, veVec3::UNIT_X));
			root->addChild(node);
		}

		{
			veCamera *cam0 = static_cast<veCamera*>(veFile::instance()->readFile("cameras/cam0.vecamera"));
			veCamera *cam1 = static_cast<veCamera*>(veFile::instance()->readFile("cameras/cam1.vecamera"));
			veCamera *cam2 = static_cast<veCamera*>(veFile::instance()->readFile("cameras/cam2.vecamera"));
			veCamera *cam3 = static_cast<veCamera*>(veFile::instance()->readFile("cameras/cam3.vecamera"));
			//cam1->setClearColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
			//cam2->setClearColor(veVec4(0.0f, 1.0f, 0.0f, 1.0f));
			//cam3->setClearColor(veVec4(0.0f, 0.0f, 1.0f, 1.0f));
			veNode *cameras = new veNode;
			cameras->addChild(cam0);
			cameras->addChild(cam1);
			cameras->addChild(cam2);
			cameras->addChild(cam3);
			cameras->addComponent(new keyboardInput(_visualiser));
			root->addChild(cameras);
		}

		_visualiser->setSceneNode(root);
	}
	~CameraTest() {};

private:

};

#endif