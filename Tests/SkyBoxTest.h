#ifndef _SKY_BOX_TEST_
#define _SKY_BOX_TEST_
#include "BaseTest.h"

class SkyBoxTest : public BaseTest
{
public:
	SkyBoxTest() {
		veNode *root = _sceneManager->createNode("root");
		{
			veNode *node = _sceneManager->createNode("skyboxNode");
			auto skyBox = _sceneManager->createSkyBox("skybox");
			node->addRenderableObject(skyBox);
			root->addChild(node);

			veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-canyon.vemtl", "skybox-mats"));
			skyBox->setMaterialArray(materials);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
			veNode *node = _sceneManager->createNode("node3");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
			root->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim-0"));
			animationContainer->start();
			animationContainer->setLoopAnimation(true);
			entity->setAnimationContainer(animationContainer);
		}

		_sceneManager->getRootNode()->addChild(root);
	};
	~SkyBoxTest() {};

protected:


};

#endif