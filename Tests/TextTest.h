#ifndef _TEXT_TEST_
#define _TEXT_TEST_
#include "BaseTest.h"

class TextUpdater : public veComponent 
{
public:
	TextUpdater(veText *text) 
		: _text(text){

	}

	virtual void update(veNode *node, veVisualiser *vs) override {
		char buf[256];
		sprintf(buf, "Frame Rate: %f", 1.0f / vs->getDeltaTime());
		_text->setContent(buf);
	}

private:

	veText *_text;
};

class TextTest : public BaseTest
{
public:
	TextTest() {
		veNode *root = new veNode;
		{
			auto text = new veText(new veFont("fonts/Vera.ttf", 64), "Void Engine!");
			text->setColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
			auto node = new veNode;
			node->addRenderableObject(text);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.5f, 0.0f));
			root->addChild(node);
		}

		{
			auto text = new veText(new veFont("fonts/arial.ttf"));
			//text->setColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
			auto node = new veNode;
			node->addRenderableObject(text);
			node->addComponent(new TextUpdater(text));
			root->addChild(node);
		}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/laoshu_ani_v03.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			root->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile("models/laoshu_ani_v03.veanim"));
			animationContainer->start();
			animationContainer->setLoopAnimation(true);
			node->addComponent(animationContainer);
		}

		_visualiser->setSceneNode(root);
	}
	~TextTest() {};

private:

};

#endif