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
			skyBox->setMaterial(materials->getMaterial(0));
		}

		{
			veNode *cm = _sceneManager->createNode("cubemapRoot");
			root->addChild(cm);

			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere"));
			auto material = entity->getChild(0)->getChild(0)->getChild(0)->getRenderableObject(0)->getMaterial();
			material->activateTechnique(material->getTechnique("CubeMapReflectionRendering"));
			entity->setMatrix(veMat4::scale(2.0f));
			cm->addChild(entity);
			entity->setMask(~CUBE_MAP_CAMERA);

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

			veTransformer *transer = new veTransformer;
			cm->addComponent(transer);

			cm->setUpdateCallback([=](veSceneManager *sm, veNode *node) {
				static bool reverse = false;
				static float maxDis = 5.0f;
				static float currentDis = -5.0f;
				if (reverse) {
					currentDis -= 5.0f * sm->getDeltaTime();
					if (currentDis < -maxDis) {
						currentDis = -maxDis;
						reverse = false;
					}
				}
				else {
					currentDis += 5.0f * sm->getDeltaTime();
					if (maxDis < currentDis) {
						currentDis = maxDis;
						reverse = true;
					}
				}
				transer->setPosition(veVec3(0.0f, currentDis, 0.0f));
			});
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot-1"));
			auto material = entity->getChild(0)->getChild(0)->getChild(0)->getRenderableObject(0)->getMaterial();
			material->activateTechnique(material->getTechnique("ReflectionRendering"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setPosition(veVec3(5.0f, 0.0f, 0.0f));
			root->addChild(entity);
		}

		{
			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			entity->addComponent(transer);
			transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			root->addChild(entity);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
			player->start();
			player->setLoopAnimation(true);
			player->attachNode(entity);
		}

		//auto pass = new vePass;
		//pass->setShader(new veShader(veShader::VERTEX_SHADER, std::string("shaders/AchieveDepthShader.vert")));
		//pass->setShader(new veShader(veShader::FRAGMENT_SHADER, std::string("shaders/AchieveDepthShader.frag")));
		//pass->addUniform(new veUniform("u_ModelViewProjectMat", std::string("MVP_MATRIX")));
		//pass->addUniform(new veUniform("u_BoneMates", std::string("BONE_MATRIXES")));
		//_sceneManager->getCamera()->setGlobalRenderPass(pass);


		_sceneManager->getRootNode()->addChild(root);
	};
	~CubeMapTest() {};

protected:


};

#endif