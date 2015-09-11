#ifndef _REN_TO_TEX_TEST_
#define _REN_TO_TEX_TEST_
#include "BaseTest.h"

#define MAIN_CAMERA 0x01
#define RTT_CAMERA  0x02

class RenToTex : public BaseTest
{
public:
	RenToTex() {
		veNode *root = new veNode;
		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/teapot.vem"));
			auto mat = node->getChild(0)->getChild(0)->getRenderableObject(0)->getMaterial();
			mat->activateTechnique(mat->getTechnique("deferredRender"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(1.0f, 0.0f, 0.0f));
			//transer->setScale(veVec3(1.0f));
			root->addChild(node);
			node->setMask(RTT_CAMERA);
		}

		{
			veMaterialArray *mats = static_cast<veMaterialArray *>(veFile::instance()->readFile("models/DeferredRendering.vemtl"));
			veNode *node = new veNode;
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setScale(veVec3(0.5f));

			auto overlay = new veOverlay;
			node->addRenderableObject(overlay);
			overlay->setMaterial(mats->getMaterial(0));
			root->addChild(node);
			node->setMask(MAIN_CAMERA);
		}

		{
			veCamera *camera = static_cast<veCamera *>(veFile::instance()->readFile("cameras/rtt.vecamera"));
			camera->setMask(RTT_CAMERA);
			root->addChild(camera);
		}

		_visualiser->setSceneNode(root);
		_visualiser->getCamera()->setMask(MAIN_CAMERA);
	}
	~RenToTex() {};

private:

};

#endif