#ifndef _TERRAIN_TEST_
#define _TERRAIN_TEST_
#include "BaseTest.h"

class TerrainTest : public BaseTest
{
public:
	TerrainTest() {
		//int n = rand();
		veNode *root = _sceneManager->createNode("root");
//		{
//			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/teapot.vem", "teapot-0"));
//			//node->addComponent(new KeyboardInputer);
//			veTransformer *transer = new veTransformer;
//			entity->addComponent(transer);
//			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
//			transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
//			transer->setScale(veVec3(2.0f));
//			root->addChild(entity);
//		}
        
        {
            veNode *lightNode = _sceneManager->createNode("pointNode");
            veLight *point = static_cast<veLight *>(veFile::instance()->readFile(_sceneManager, "lights/point0.velight", "point0"));
            lightNode->addComponent(point);
            veTransformer *lightTranser = new veTransformer;
            lightNode->addComponent(lightTranser);
            lightNode->addComponent(new LightUpdater(point, 20.0f, 10.0f));
            point->setIntensity(1.0f);
            point->setAttenuationRange(500.0f);
            lightTranser->setPosition(veVec3(0.0f, 0.0f, 50.0f));
            
            veNode *lightentity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/sphere.vem", "point0-sphere"));
            lightentity->setMatrix(veMat4::scale(veVec3(0.2f)));
            lightNode->addChild(lightentity);
            point->shadowEnable(false);
            point->setUseSoftShadow(true);
            point->setShadowSoftness(0.05f);
            point->setShadowBias(0.001f);
            root->addChild(lightNode);
        }
        
        
        {
            veNode *terrainNode = _sceneManager->createNode("Terrain");
            veTerrain *terrain = new veTerrain;
            terrain->build();
            terrainNode->addComponent(terrain);
            root->addChild(terrainNode);
        }
        

		auto debuger = new veOctreeDebuger(_sceneManager);
		debuger->debugDrawBoundingBoxWireframe(true);
		debuger->debugDrawOctree(false);
		_sceneManager->getRootNode()->addRenderableObject(debuger);
		_sceneManager->getRootNode()->addChild(root);
        _sceneManager->setAmbientColor(veVec3(0.0f));
	}
	~TerrainTest() {};

private:

};

#endif