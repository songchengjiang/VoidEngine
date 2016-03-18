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
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "sphere-0"));
			veNode *node = _sceneManager->createNode("node");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(20.0f));
			root->addChild(node);


			//entity->setMaterialArray(IBLMats);
			//for (unsigned int i = 0; i < entity->getMeshCount(); ++i) {
			//	entity->getMesh(i)->setMaterial(IBLMats->getMaterial(0));
			//}
		}

		{
			veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
			veTransformer *lightTranser = new veTransformer;
			point->addComponent(lightTranser);
			//point->addComponent(new LightUpdater(15.0f, 0.0f));
			point->setIntensity(1.0f);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 3.0f));

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode1");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			point->addChild(lightModel);
			point->shadowEnable(false);
			point->setUseSoftShadow(true);
			point->setShadowSoftness(0.05f);
			point->setShadowBias(0.001f);
			root->addChild(point);
		}

		_sceneManager->getRootNode()->addChild(root);
		_sceneManager->setAmbientColor(veVec3(0.0f));
	};
	~IBLTest() {};

protected:


};

#endif