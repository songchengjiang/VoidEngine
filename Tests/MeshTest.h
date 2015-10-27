#ifndef _MESH_TEST_
#define _MESH_TEST_
#include "BaseTest.h"

class MeshTest : public BaseTest
{
public:
	MeshTest() {
		veNode *root = _sceneManager->createNode("root");
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot-0"));
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
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "plane-0"));
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
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere-0"));
			veNode *node = _sceneManager->createNode("node2");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			root->addChild(node);

		}

		veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
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

			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
			player->start();
			player->setLoopAnimation(true);
			player->attachEntity(entity);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-1"));
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

			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player1", animationContainer);
			player->start();
			player->setFrameRate(10.0f);
			player->setLoopAnimation(true);
			player->attachEntity(entity);
		}

		auto debuger = new veOctreeDebuger;
		debuger->debugDrawBoundingBoxWireframe(true);
		debuger->debugDrawOctree(false);
		_sceneManager->getRootNode()->addRenderableObject(debuger);
		_sceneManager->getRootNode()->addChild(root);
	}
	~MeshTest() {};

private:

};

#endif