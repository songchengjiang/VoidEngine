#ifndef _LIGHT_TEST_
#define _LIGHT_TEST_
#include "BaseTest.h"

class LightTest : public BaseTest
{
public:
	LightTest() {
		veNode *root = new veNode;
		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/teapot.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(4.0f));
			root->addChild(node);

			veLight *light = static_cast<veLight *>(veFile::instance()->readFile("lights/point0.velight"));
			root->addChild(light);
		}

		_visualiser->setSceneNode(root);
	};
	~LightTest() {};

protected:


};

#endif