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
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "sphere-0"));
			veNode *node = _sceneManager->createNode("node");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			root->addChild(node);


			entity->setMaterialArray(IBLMats);
			for (unsigned int i = 0; i < entity->getMeshCount(); ++i) {
				entity->getMesh(i)->setMaterial(IBLMats->getMaterial(0));
			}
		}

		_sceneManager->getRootNode()->addChild(root);
	};
	~IBLTest() {};

protected:


};

#endif