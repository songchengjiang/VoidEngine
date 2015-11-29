#ifndef _CUBE_MAP_TEST_
#define _CUBE_MAP_TEST_
#include "BaseTest.h"

#define CUBE_MAP_CAMERA 0x0000000f

class CubeMapTest : public BaseTest
{
public:
	CubeMapTest() {
		veNode *root = _sceneManager->createNode("root");

		{
			auto skyBox = _sceneManager->createSkyBox("skybox");
			_sceneManager->setSkyBox(skyBox);

			veMaterialArray *materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "skyboxs/skybox-snow.vemtl", "skybox-mats"));
			skyBox->setMaterialArray(materials);
		}

		{
			veNode *cm = _sceneManager->createNode("cubemapRoot");
			root->addChild(cm);

			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot-0"));
			auto material = entity->getMaterialArray()->getMaterial("mat-0");
			material->activateTechnique(material->getTechnique("CubeMapReflectionRendering"));
			veNode *node = _sceneManager->createNode("node0");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			cm->addChild(node);
			node->setMask(~CUBE_MAP_CAMERA);

			veCamera *posx = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cubmapPOSX.vecamera", "cubmapPOSX"));
			veCamera *negx = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cubmapNEGX.vecamera", "cubmapNEGX"));
			veCamera *posy = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cubmapPOSY.vecamera", "cubmapPOSY"));
			veCamera *negy = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cubmapNEGY.vecamera", "cubmapNEGY"));
			veCamera *posz = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cubmapPOSZ.vecamera", "cubmapPOSZ"));
			veCamera *negz = static_cast<veCamera*>(veFile::instance()->readFile(_sceneManager, "cameras/cubmapNEGZ.vecamera", "cubmapNEGZ"));
			posx->setMask(CUBE_MAP_CAMERA);
			negx->setMask(CUBE_MAP_CAMERA);
			posy->setMask(CUBE_MAP_CAMERA);
			negy->setMask(CUBE_MAP_CAMERA);
			posz->setMask(CUBE_MAP_CAMERA); 
			negz->setMask(CUBE_MAP_CAMERA);

			cm->addChild(posx);
			cm->addChild(negx);
			cm->addChild(posy);
			cm->addChild(negy);
			cm->addChild(posz);
			cm->addChild(negz);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot-1"));
			auto material = entity->getMaterialArray()->getMaterial("mat-0");
			material->activateTechnique(material->getTechnique("ReflectionRendering"));
			veNode *node = _sceneManager->createNode("node1");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(10.0f, 0.0f, 0.0f));
			root->addChild(node);
		}

		_sceneManager->getRootNode()->addChild(root);
	};
	~CubeMapTest() {};

protected:


};

#endif