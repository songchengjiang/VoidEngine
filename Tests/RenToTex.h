#ifndef _REN_TO_TEX_TEST_
#define _REN_TO_TEX_TEST_
#include "BaseTest.h"

#define DEFERRED_CAMERA 0x01
#define RTT_CAMERA  0x02

float randomNEGONEtoPOSONE(){
	return veMath::randomUnitization() * 2.0 - 1.0;
}

class RenToTex : public BaseTest
{
public:
	RenToTex() {
	}
	~RenToTex() {};

	virtual void init() override {
        
        
		_sceneManager->loadLightConfiguration("lights/point.velightemplates");

		veNode *root = _sceneManager->createNode("root");
		veNode *scene = _sceneManager->createNode("scene");
		scene->setMask(RTT_CAMERA, true);
		root->addChild(scene);

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
			veNode *node = _sceneManager->createNode("node0");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			transer->setPosition(veVec3(5.0f, 0.0f, 0.0f));
			transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_X));
			scene->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim-0"));
			animationContainer->start();
			animationContainer->setLoopAnimation(true);
			entity->setAnimationContainer(animationContainer);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot"));
			veNode *node = _sceneManager->createNode("node1");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_X));
			scene->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "sphere"));
			veNode *node = _sceneManager->createNode("node2");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 2.0f));
			transer->setScale(veVec3(2.0f));
			scene->addChild(node);
		}

		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "plane"));
			veNode *node = _sceneManager->createNode("node3");
			node->addRenderableObject(entity);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(20.0f));
			scene->addChild(node);
		}

		{
			veLight *directional0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional0"));
			veTransformer *lightTranser = new veTransformer;
			directional0->addComponent(lightTranser);
			//directional0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(5.0f, 5.0f, 5.0f));
			auto param = directional0->getParameter("intensity");
			param->set(0.1f);

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode0");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			directional0->addChild(lightModel);
			scene->addChild(directional0);
		}

		{
			veLight *directional0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/directional0.velight", "directional1"));
			veTransformer *lightTranser = new veTransformer;
			directional0->addComponent(lightTranser);
			directional0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			auto param = directional0->getParameter("intensity");
			param->set(veMath::randomUnitization());

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "directional1-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode1");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			directional0->addChild(lightModel);
			scene->addChild(directional0);
		}

		{
			veLight *point0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
			veTransformer *lightTranser = new veTransformer;
			point0->addComponent(lightTranser);
			point0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			auto param = point0->getParameter("intensity");
			param->set(veMath::randomUnitization());

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode2");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			point0->addChild(lightModel);
			scene->addChild(point0);
		}

		{
			veLight *spot0 = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/spot0.velight", "spot0"));
			veTransformer *lightTranser = new veTransformer;
			spot0->addComponent(lightTranser);
			spot0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			auto param = spot0->getParameter("intensity");
			param->set(veMath::randomUnitization());

			veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
			veNode *lightModel = _sceneManager->createNode("lightnode3");
			lightModel->addRenderableObject(lightentity);
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			spot0->addChild(lightModel);
			scene->addChild(spot0);
		}

		{
			veMaterialArray *mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "models/DeferredRendering.vemtl", "DeferredRenderingMaterial"));
			veNode *node = _sceneManager->createNode("node4");
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setScale(veVec3(0.5f));

			auto overlay = _sceneManager->createSurface("overlay");
			node->addRenderableObject(overlay);
			overlay->setMaterialArray(mats);
			root->addChild(node);
			node->setMask(DEFERRED_CAMERA);
		}

		{
			veCamera *camera = static_cast<veCamera *>(veFile::instance()->readFile(_sceneManager, "cameras/deferred.vecamera", "deferredCamera"));
			camera->setMask(RTT_CAMERA);
			root->addChild(camera);
			_camera = camera;
		}

		_sceneManager->getRootNode()->addChild(root);
		_sceneManager->getVisualiser()->getCamera()->setMask(DEFERRED_CAMERA);
	}

private:

};

#endif