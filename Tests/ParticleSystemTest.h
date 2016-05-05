#ifndef _PARTICLE_SYSTEM_TEST_
#define _PARTICLE_SYSTEM_TEST_
#include "BaseTest.h"

class ParticleSystemTest : public BaseTest
{
public:
    ParticleSystemTest() {
        veNode *root = _sceneManager->createNode("root");
        {
            auto psMats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "materials/starEffect.vemtl", "starEffect"));
            veParticleSystem *ps = _sceneManager->createParticleSystem("ps1");
            ps->setMaterialArray(psMats);
            auto emitter = new vePointEmitter;
            ps->addEmitter(emitter);
            auto affector = new veColorAffector;
            affector->addColor(0.0f, veVec4(1.0f, 1.0f, 0.0f, 1.0f));
            affector->addColor(1.0f, veVec4(0.0f, 1.0f, 0.0f, 1.0f));
            ps->addAffector(affector);
            auto renderer = new veQuatRenderer;
            ps->setRenderer(renderer);
            ps->start();
            
            
            veNode *node = _sceneManager->createNode("node");
            node->addRenderableObject(ps);
            veTransformer *transer = new veTransformer;
            node->addComponent(transer);
            transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
            root->addChild(node);
        }
        
        _sceneManager->getRootNode()->addChild(root);
    };
    ~ParticleSystemTest() {};
    
protected:
    
    
};

#endif