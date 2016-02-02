#ifndef _NORMAL_MAPPING_TEST_
#define _NORMAL_MAPPING_TEST_
#include "BaseTest.h"

class NormalMappingTest : public BaseTest
{
public:
	NormalMappingTest() {
		veNode *root = _sceneManager->createNode("root");
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/box.vem", "box-0"));
			veNode *node = _sceneManager->createNode("node");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(10.0f));
			root->addChild(node);
		}

		{
			veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
			veTransformer *lightTranser = new veTransformer;
			point->addComponent(lightTranser);
			point->addComponent(new LightUpdater(15.0f, 0.0f));
			point->setIntensity(1.0f);

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode1");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			point->addChild(lightModel);
			point->shadowEnable(true);
			point->setUseSoftShadow(true);
			point->setShadowSoftness(0.05f);
			point->setShadowBias(0.001f);
			root->addChild(point);
		}

		_sceneManager->getRootNode()->addChild(root);
	};
	~NormalMappingTest() {};

protected:


};

#endif