#ifndef _MESH_TEST_
#define _MESH_TEST_
#include "BaseTest.h"

class MeshTest : public BaseTest
{
public:
	MeshTest() {
		veNode *root = new veNode;
		//{
		//	veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/sphere.vem"));
		//	//node->addComponent(new KeyboardInputer);
		//	veTransformer *transer = new veTransformer;
		//	node->addComponent(transer);
		//	transer->setPosition(veVec3(1.0f, 0.0f, 0.0f));
		//	transer->setScale(veVec3(1.0f));
		//	transer->setRotation(veQuat(veMath::PI, veVec3::UNIT_X));
		//	root->addChild(node);
		//}

		//{
		//	veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/plane.vem"));
		//	//node->addComponent(new KeyboardInputer);
		//	veTransformer *transer = new veTransformer;
		//	node->addComponent(transer);
		//	transer->setPosition(veVec3(-1.0f, 0.0f, 0.0f));
		//	transer->setScale(veVec3(1.0f));
		//	//transer->setRotation(veQuat(veMath::PI, veVec3::UNIT_X));
		//	root->addChild(node);
		//}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/laoshu_ani_v03.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, -100.0f));
			transer->setScale(veVec3(1.0f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			root->addChild(node);
		}

		_visualiser->setSceneNode(root);
	}
	~MeshTest() {};

private:

};

#endif