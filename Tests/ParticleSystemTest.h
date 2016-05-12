#ifndef _PARTICLE_SYSTEM_TEST_
#define _PARTICLE_SYSTEM_TEST_
#include "BaseTest.h"

class ParticleSystemTest : public BaseTest
{
public:
    ParticleSystemTest() {
        veNode *root = _sceneManager->createNode("root");
        {
//            auto psMats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "materials/starEffect.vemtl", "starEffect"));
//            veParticleSystem *ps = _sceneManager->createParticleSystem("ps1");
//            ps->setMaterialArray(psMats);
//            
//            auto emitter = new veSphereEmitter;
//            //emitter->setRotation(veQuat(veMath::veRadian(45.0f), veVec3::UNIT_Z));
//            //emitter->setPosition(veVec3(5.0f, 0.0f, 0.0f));
//            auto val = new veValueFixed;
//            val->setValue(veMath::veRadian(90.0f));
//            //emitter->setAngle(val);
//            val = new veValueFixed;
//            val->setValue(100.f);
//            emitter->setEmissionRate(val);
//            ps->addEmitter(emitter);
//            
//            auto affector = new veColorAffector;
//            affector->addColor(0.0f, veVec4(1.0f, 1.0f, 0.0f, 1.0f));
//            affector->addColor(1.0f, veVec4(0.0f, 1.0f, 0.0f, 0.0f));
//            ps->addAffector(affector);
//            
//            auto sclAffector = new veScaleAffector;
//            val = new veValueFixed;
//            val->setValue(1.01f);
//            //sclAffector->setScaleX(val);
//            //ps->addAffector(sclAffector);
//            
//            auto sclVelAffector = new veScaleVelocityAffector;
//            val = new veValueFixed;
//            val->setValue(1.01f);
//            sclVelAffector->setScaleVelocity(val);
//            //ps->addAffector(sclVelAffector);
//            
//            auto gravAffector = new veGravityAffector;
//            gravAffector->setGravity(50.f);
//            ps->addAffector(gravAffector);
//            
//            auto vortexAffector = new veVortexAffector;
//            val = new veValueFixed;
//            val->setValue(veMath::veRadian(200.0f));
//            vortexAffector->setRotationSpeed(val);
//            ps->addAffector(vortexAffector);
//            
//            auto sineForce = new veSineForceAffector;
//            //sineForce->setRotation(veQuat(veMath::veRadian(45.0f), veVec3::UNIT_Z));
//            sineForce->setFrequencyMaximun(5.f);
//            sineForce->setFrequencyMinimum(-5.f);
//            sineForce->setForceScalar(10.f);
//            ps->addAffector(sineForce);
//            
//            auto renderer = new veQuatRenderer;
//            ps->setRenderer(renderer);
//            ps->start();
            
            auto ps = static_cast<veParticleSystem *>(veFile::instance()->readFile(_sceneManager, "effects/star.veparticle", "starPS"));
            veNode *node = _sceneManager->createNode("node");
            node->addRenderableObject(ps);
            veTransformer *transer = new veTransformer;
            node->addComponent(transer);
            transer->setPosition(veVec3(-5.0f, 0.0f, 0.0f));
            root->addChild(node);
            
            node->setUpdateCallback([transer](veSceneManager *sm, veNode *node){
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
                transer->setPosition(veVec3(currentDis, 0.0f, 0.0f));
            });
        }
        
        _sceneManager->getRootNode()->addChild(root);
    };
    ~ParticleSystemTest() {};
    
protected:
    
    
};

#endif