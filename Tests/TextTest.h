#ifndef _TEXT_TEST_
#define _TEXT_TEST_
#include "BaseTest.h"

class TextUpdater : public veComponent 
{
public:
	TextUpdater(veText *text) 
		: _text(text){

	}

	virtual void update(veNode *node, veSceneManager *sm) override {
		char buf[256];
		sprintf(buf, "Frame Rate: %f", 1.0f / sm->getDeltaTime());
		_text->setContent(buf);
	}

private:

	veText *_text;
};

class TextTest : public BaseTest
{
public:
	TextTest() {
		veNode *root = _sceneManager->createNode("root");

		std::string fontFile = "fonts/arial.ttf";
		{
			auto text = _sceneManager->createText(new veFont(fontFile, 64), "Hello Void Engine");
			text->setColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
			auto node = _sceneManager->createNode("node0");
			node->addRenderableObject(text);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.5f, 0.0f));
			root->addChild(node);
		}

		{
			auto text = _sceneManager->createText(new veFont(fontFile));
			//text->setColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
			auto node = _sceneManager->createNode("node1");
			node->addRenderableObject(text);
			node->addComponent(new TextUpdater(text));
			root->addChild(node);
		}

		{
			veNode *mouseNode = _sceneManager->createNode("MouseNode");
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu"));
			veNode *node = _sceneManager->createNode("node2");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			mouseNode->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
			animationContainer->start();
			animationContainer->setLoopAnimation(true);
			entity->setAnimationContainer(animationContainer);

			auto text = _sceneManager->createText(new veFont(fontFile), "Boss");
			text->setTextType(veText::PLANE);
			//text->setColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
			mouseNode->addRenderableObject(text);
			root->addChild(mouseNode);
		}

		_sceneManager->getRootNode()->addChild(root);
	}
	~TextTest() {};

private:

};

#endif