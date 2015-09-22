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
        
//        veMaterialArray *matAry = static_cast<veMaterialArray *>(veFile::instance()->readFile("models/laoshu_ani_v03.vemtl"));
        
		veLightManager::instance()->loadLightTemplates("lights/point.velightemplates");

		veNode *root = new veNode;
		veNode *scene = new veNode;
		scene->setMask(RTT_CAMERA, true);
		root->addChild(scene);


		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/laoshu_ani_v03.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(0.3f));
			transer->setPosition(veVec3(5.0f, 0.0f, 0.0f));
			transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_X));
			root->addChild(node);

			veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile("models/laoshu_ani_v03.veanim"));
			animationContainer->start();
			animationContainer->setLoopAnimation(true);
			node->addComponent(animationContainer);
		}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/teapot.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
			transer->setScale(veVec3(2.0f));
			transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_X));
			scene->addChild(node);
		}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/sphere.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3(0.0f, 0.0f, 2.0f));
			transer->setScale(veVec3(2.0f));
			scene->addChild(node);
		}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/plane.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(20.0f));
			scene->addChild(node);
		}

		{
			veLight *directional0 = static_cast<veLight *>(veFile::instance()->readFile("lights/directional0.velight"));
			veTransformer *lightTranser = new veTransformer;
			directional0->addComponent(lightTranser);
			//directional0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(5.0f, 5.0f, 5.0f));
			auto param = directional0->getParameter("intensity");
			param->set(0.1f);

			veNode *lightModel = static_cast<veNode *>(veFile::instance()->readFile("models/sphere.vem"));
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			directional0->addChild(lightModel);
			scene->addChild(directional0);
		}

		{
			veLight *directional0 = static_cast<veLight *>(veFile::instance()->readFile("lights/directional0.velight"));
			veTransformer *lightTranser = new veTransformer;
			directional0->addComponent(lightTranser);
			directional0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			auto param = directional0->getParameter("intensity");
			param->set(veMath::randomUnitization());

			veNode *lightModel = static_cast<veNode *>(veFile::instance()->readFile("models/sphere.vem"));
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			directional0->addChild(lightModel);
			scene->addChild(directional0);
		}

		{
			veLight *point0 = static_cast<veLight *>(veFile::instance()->readFile("lights/point0.velight"));
			veTransformer *lightTranser = new veTransformer;
			point0->addComponent(lightTranser);
			point0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			auto param = point0->getParameter("intensity");
			param->set(veMath::randomUnitization());

			veNode *lightModel = static_cast<veNode *>(veFile::instance()->readFile("models/sphere.vem"));
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			point0->addChild(lightModel);
			scene->addChild(point0);
		}

		{
			veLight *spot0 = static_cast<veLight *>(veFile::instance()->readFile("lights/spot0.velight"));
			veTransformer *lightTranser = new veTransformer;
			spot0->addComponent(lightTranser);
			spot0->addComponent(new LightUpdater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			auto param = spot0->getParameter("intensity");
			param->set(veMath::randomUnitization());

			veNode *lightModel = static_cast<veNode *>(veFile::instance()->readFile("models/sphere.vem"));
			lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
			spot0->addChild(lightModel);
			scene->addChild(spot0);
		}

		{
			VE_Ptr<veMaterialArray> mats = static_cast<veMaterialArray *>(veFile::instance()->readFile("models/DeferredRendering.vemtl"));
			veNode *node = new veNode;
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			//transer->setScale(veVec3(0.5f));

			auto overlay = new veOverlay;
			node->addRenderableObject(overlay);
			overlay->setMaterial(mats->getMaterial(0));
			root->addChild(node);
			node->setMask(DEFERRED_CAMERA);
		}

		{
			veCamera *camera = static_cast<veCamera *>(veFile::instance()->readFile("cameras/deferred.vecamera"));
			camera->setMask(RTT_CAMERA);
			root->addChild(camera);
			_camera = camera;
		}

		_visualiser->setSceneNode(root);
		_visualiser->getCamera()->setMask(DEFERRED_CAMERA);
	}

private:

};

#endif