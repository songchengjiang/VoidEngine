#ifndef _IBL_TEST_
#define _IBL_TEST_
#include "BaseTest.h"

class IBLTest : public BaseTest
{
public:
	IBLTest() {
		auto IBLMats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "materials/IBL.vemtl", "IBL"));
		veNode *root = _sceneManager->createNode("root");
		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "sphere-0"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(20.0f));
			root->addChild(entity);


			//entity->setMaterialArray(IBLMats);
			//for (unsigned int i = 0; i < entity->getMeshCount(); ++i) {
			//	entity->getMesh(i)->setMaterial(IBLMats->getMaterial(0));
			//}
		}

		{
            veNode *lightNode = _sceneManager->createNode("pointNode");
			veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
            lightNode->addComponent(point);
			veTransformer *lightTranser = new veTransformer;
			lightNode->addComponent(lightTranser);
			//point->addComponent(new LightUpdater(15.0f, 0.0f));
			point->setIntensity(1.0f);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 3.0f));

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
		_sceneManager->setAmbientColor(veVec3(0.0f));
	};
	~IBLTest() {};

protected:


};

#endif