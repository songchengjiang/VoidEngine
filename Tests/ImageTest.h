#ifndef _IMAGE_TEST_
#define _IMAGE_TEST_
#include "BaseTest.h"

class ImageTest : public BaseTest
{
public:
	ImageTest() {
		veNode *root = _sceneManager->createNode("root");
		{
			veNode *node = _sceneManager->createNode("node0");
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(-0.5f, -0.5f, 0.0f));
			transer->setScale(veVec3(0.5f));

			veTexture *texture = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/Desk.exr", "desk"));
			auto overlay = _sceneManager->createOverlay("image0", texture);
			node->addRenderableObject(overlay);

			root->addChild(node);
		}

		{
			veNode *node = _sceneManager->createNode("node1");
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));

			veTexture *texture = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/kill.png", "kill"));
			auto overlay = _sceneManager->createOverlay("image1", texture);
			overlay->setRenderOrder(10);
			node->addRenderableObject(overlay);
			//overlay->setAlphaThreshold(1.0f);

			root->addChild(node);
		}

		{
			veNode *node = _sceneManager->createNode("node2");
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.5f, 0.5f, 0.0f));
			transer->setScale(veVec3(0.5f));

			veTexture *texture = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/sphere.jpg", "sphere"));
			auto overlay = _sceneManager->createOverlay("image2", texture);
			node->addRenderableObject(overlay);

			root->addChild(node);
		}

		_sceneManager->getRootNode()->addChild(root);
	};
	~ImageTest() {};

protected:


};

#endif