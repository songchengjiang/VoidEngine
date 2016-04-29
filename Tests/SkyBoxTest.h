#ifndef _SKY_BOX_TEST_
#define _SKY_BOX_TEST_
#include "BaseTest.h"

class SkyBoxTest : public BaseTest
{
public:
	SkyBoxTest() {
		veNode *root = _sceneManager->createNode("root");
		{
			auto skyBox = _sceneManager->createSkyBox("skybox");
			_sceneManager->setSkyBox(skyBox);

			veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-snow.vemtl", "skybox-mats"));
			skyBox->setMaterialArray(materials);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot-0"));
			auto material = entity->getMaterialArray()->getMaterial("mat-0");
			material->activateTechnique(material->getTechnique("ReflectionRendering"));
			veNode *node = _sceneManager->createNode("node0");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
			root->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot-1"));
			auto material = entity->getMaterialArray()->getMaterial("mat-0");
			material->activateTechnique(material->getTechnique("RefractionRendering"));
			veNode *node = _sceneManager->createNode("node1");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(5.0f, 0.0f, 0.0f));
			root->addChild(node);
		}

		_sceneManager->getRootNode()->addChild(root);
	};
	~SkyBoxTest() {};

protected:


};

#endif