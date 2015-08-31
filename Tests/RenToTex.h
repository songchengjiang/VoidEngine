#ifndef _REN_TO_TEX_TEST_
#define _REN_TO_TEX_TEST_
#include "BaseTest.h"

class RenToTex : public BaseTest
{
public:
	RenToTex() {
		veNode *root = new veNode;
		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/teapot.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(1.0f, 0.0f, 0.0f));
			//transer->setScale(veVec3(1.0f));
			//transer->setRotation(veQuat(veMath::PI, veVec3::UNIT_X));
			root->addChild(node);
		}

		{
			veMaterialArray *mats = static_cast<veMaterialArray *>(veFile::instance()->readFile("models/DeferredRendering.vemtl"));
			veNode *node = new veNode;
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);

			auto overlay = new veOverlay;
			node->addRenderableObject(overlay);
			overlay->setMaterial(mats->getMaterial(0));
			root->addChild(node);
		}

		_visualiser->setSceneNode(root);
	}
	~RenToTex() {};

private:

};

#endif