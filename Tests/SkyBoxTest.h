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
			skyBox->setMaterial(materials->getMaterial(0));
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot-0"));
			auto material = entity->getChild(0)->getChild(0)->getChild(0)->getRenderableObject(0)->getMaterial();
			material->activateTechnique(material->getTechnique("ReflectionRendering"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
			root->addChild(entity);
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot-1"));
			auto material = entity->getChild(0)->getChild(0)->getChild(0)->getRenderableObject(0)->getMaterial();
			material->activateTechnique(material->getTechnique("RefractionRendering"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(5.0f, 0.0f, 0.0f));
			root->addChild(entity);
		}

		_sceneManager->getRootNode()->addChild(root);
	};
	~SkyBoxTest() {};

protected:


};

#endif