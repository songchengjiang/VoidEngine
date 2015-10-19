#ifndef _PERFORMANCE_TEST_
#define _PERFORMANCE_TEST_
#include "BaseTest.h"
static const std::string MODEL_FILES[] = { "models/teapot.vem", "models/sphere.vem", "models/laoshu_ani_v03.vem" };
static const std::string MODEL_ANIM_FILES[] = { "", "", "models/laoshu_ani_v03.veanim" };

class PerformanceUpdater : public veComponent
{
public:
	PerformanceUpdater(){

	}

	virtual void update(veNode *node, veSceneManager *sm) override {
		char buf[256];
		sprintf(buf, "Frame Rate: %f", 1.0f / sm->getDeltaTime());
		_text->setContent(buf);
	}

	void setFrameText(veText *text) { _text = text; }

private:

	veText *_text;
};

class PerformanceTest : public BaseTest
{
public:

	PerformanceTest() {
		int lineCount = 10;
		float lineHalfSize = (lineCount / 2) * 100;
		veNode *root = _sceneManager->createNode();
		auto pu = new PerformanceUpdater();
		for (float  z = -lineHalfSize; z <= lineHalfSize; z += 100) {
			for (float  y = -lineHalfSize; y <= lineHalfSize; y += 100) {
				for (float x = -lineHalfSize; x <= lineHalfSize; x += 100) {
					//unsigned int idx = (veMath::randomUnitization() * 0.999f) * (sizeof(MODEL_FILES) / sizeof(MODEL_FILES[0]));
					unsigned int idx = 2;
					veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, MODEL_FILES[idx]));
					veNode *node = _sceneManager->createNode();
					node->addRenderableObject(entity);
					veTransformer *transer = new veTransformer;
					node->addComponent(transer);
					transer->setPosition(veVec3(x, y, z));
					transer->setScale(veVec3(2.0f));
					root->addChild(node);

					if (!MODEL_ANIM_FILES[idx].empty()) {
						veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, MODEL_ANIM_FILES[idx]));
						animationContainer->start();
						animationContainer->setLoopAnimation(true);
						entity->setAnimationContainer(animationContainer);
					}
				}
			}
		}

		{
			auto text = _sceneManager->createText(new veFont("fonts/arial.ttf", 32));
			text->setColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
			auto node = _sceneManager->createNode();
			node->addRenderableObject(text);
			root->addChild(node);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.8f, 0.0f));

			pu->setFrameText(text);
		}

		_camera->setViewMatrixAslookAt(veVec3(0.0f, 0.0f, lineHalfSize * 1.2f), veVec3::ZERO, veVec3::UNIT_Y);
		_defaultCameraDistance = lineHalfSize * 1.2f;
		_defaultCameraZoomScale = 10.0f;
		_sceneManager->getRootNode()->addComponent(pu);

		auto debuger = new veOctreeDebuger;
		debuger->debugDrawOctree(true);
		debuger->setDebugDrawColor(veVec4(0.0f, 1.0f, 0.0f, 1.0f));
		_sceneManager->getRootNode()->addRenderableObject(debuger);

		_sceneManager->getRootNode()->addChild(root);
	};
	~PerformanceTest() {};

protected:


};

#endif