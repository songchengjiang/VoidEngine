#ifndef _MESH_TEST_
#define _MESH_TEST_
#include "BaseTest.h"

class MeshTest : public BaseTest
{
public:
	MeshTest() {
		veNode *root = _sceneManager->createNode();
		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(-5.0f, -5.0f, 0.0f));
			transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
			transer->setScale(veVec3(2.0f));
			root->addChild(node);
		}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(-5.0f, 5.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			root->addChild(node);
		}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			root->addChild(node);

		}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			transer->setPosition(veVec3(5.0f, 5.0f, 0.0f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			root->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim"));
			animationContainer->start();
			animationContainer->setLoopAnimation(true);
			node->addComponent(animationContainer);
		}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			transer->setPosition(veVec3(5.0f, -5.0f, 0.0f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			root->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim"));
			animationContainer->start();
			animationContainer->setFrameRate(10.0f);
			animationContainer->setLoopAnimation(true);
			node->addComponent(animationContainer);
		}

		auto debuger = new veDebuger;
		debuger->debugDrawBoundingBoxWireframe(true);
		debuger->setDebugDrawColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
		root->addComponent(debuger);

		_sceneManager->getRootNode()->addChild(root);
	}
	~MeshTest() {};

private:

};

#endif