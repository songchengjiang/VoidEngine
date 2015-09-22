#ifndef _IMAGE_TEST_
#define _IMAGE_TEST_
#include "BaseTest.h"

class ImageTest : public BaseTest
{
public:
	ImageTest() {
		veNode *root = new veNode;
		{
			veNode *node = new veNode;
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(-0.5f, -0.5f, 0.0f));
			transer->setScale(veVec3(0.5f));

			auto overlay = new veOverlay;
			node->addRenderableObject(overlay);

			veImage *image = static_cast<veImage *>(veFile::instance()->readFile("textures/Desk.exr"));
			overlay->setImage(image);

			root->addChild(node);
		}

		{
			veNode *node = new veNode;
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));

			auto overlay = new veOverlay;
			node->addRenderableObject(overlay);

			veImage *image = static_cast<veImage *>(veFile::instance()->readFile("textures/kill.png"));
			overlay->setImage(image);
			//overlay->setAlphaThreshold(1.0f);

			root->addChild(node);
		}

		/*{
			veNode *node = new veNode;
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.5f, 0.5f, 0.0f));
			transer->setScale(veVec3(0.5f));

			auto overlay = new veOverlay;
			node->addRenderableObject(overlay);

			veImage *image = static_cast<veImage *>(veFile::instance()->readFile("textures/sphere.jpg"));
			overlay->setImage(image);

			root->addChild(node);
		}*/

		_visualiser->setSceneNode(root);
	};
	~ImageTest() {};

protected:


};

#endif