#ifndef _SCENE_TEST_
#define _SCENE_TEST_
#include "BaseTest.h"

class SceneTest : public BaseTest
{
public:
	SceneTest() {
		veNode *root = _sceneManager->createNode("root");
		{
			veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/Aircraft/Aircraft.vem", "Aircraft-entity"));
			veNode *node = _sceneManager->createNode("Aircraft-node");
			node->addRenderableObject(entity);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			root->addChild(node);
		}
        
        {
            veEntity *entity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/plane.vem", "Plane-entity"));
            veNode *node = _sceneManager->createNode("Plane-node");
            node->addRenderableObject(entity);
            veTransformer *transer = new veTransformer;
            node->addComponent(transer);
            root->addChild(node);
            transer->setScale(veVec3(30.0f));
            transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
        }
        
        {
            veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
            veTransformer *lightTranser = new veTransformer;
            point->addComponent(lightTranser);
            point->addComponent(new LightUpdater(20.0f, 10.0f));
            point->setIntensity(1.0f);
            lightTranser->setPosition(veVec3(20.0f, 10.0f, 0.0f));
            
            veEntity *lightentity = static_cast<veEntity *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "spot0-sphere"));
            veNode *lightModel = _sceneManager->createNode("lightnode2");
            lightModel->addRenderableObject(lightentity);
            lightModel->setMatrix(veMat4::scale(veVec3(0.2f)));
            point->addChild(lightModel);
            point->shadowEnable(true);
            point->setUseSoftShadow(true);
            point->setShadowSoftness(0.05f);
            point->setShadowBias(0.025f);
            root->addChild(point);
        }

		auto debuger = new veOctreeDebuger(_sceneManager);
		debuger->debugDrawBoundingBoxWireframe(true);
		debuger->debugDrawOctree(false);
		//_sceneManager->getRootNode()->addRenderableObject(debuger);
		_sceneManager->getRootNode()->addChild(root);
        _sceneManager->setAmbientColor(veVec3(0.2f));
	}
	~SceneTest() {};

private:

};

#endif