#ifndef _NORMAL_MAPPING_TEST_
#define _NORMAL_MAPPING_TEST_
#include "BaseTest.h"

class NormalMappingTest : public BaseTest
{
public:
	NormalMappingTest() {
		veNode *root = _sceneManager->createNode("root");
		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/box.vem", "box-0"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(10.0f));
			root->addChild(entity);
		}

		{
            veNode *lightNode = _sceneManager->createNode("pointNode");
			veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
            lightNode->addComponent(point);
			veTransformer *lightTranser = new veTransformer;
			lightNode->addComponent(lightTranser);
			lightNode->addComponent(new LightUpdater(point, 15.0f, 0.0f));
			point->setIntensity(1.0f);

			veNode *lightentity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
			lightentity->setMatrix(veMat4::scale(veVec3(0.2f)));
			lightNode->addChild(lightentity);
			point->shadowEnable(false);
			point->setUseSoftShadow(true);
			point->setShadowSoftness(0.05f);
			point->setShadowBias(0.001f);
			root->addChild(lightNode);
		}

		_sceneManager->getRootNode()->addChild(root);
	};
	~NormalMappingTest() {};

protected:


};

#endif