#include "ParticleSystem.h"
#include "KernelCore/SceneManager.h"
#include "ParticleRenderer.h"
#include <algorithm>

static const unsigned int DEFAULT_QUOTA = 500;

veParticleSystem::veParticleSystem(veSceneManager *sm)
    : veRenderableObject(sm)
    , _isPrepare(false)
    , _partilceQuota(DEFAULT_QUOTA)
    , _timeElapseSinceStart(0.f)
{

}

veParticleSystem::~veParticleSystem()
{
    stop();
    _particlePool.removeAllDatas();
}

void veParticleSystem::start()
{
    for (auto emitter : _emitterList){
        emitter->notifyStart();
    }
    
    for (auto affector : _affectorList){
        affector->notifyStart();
    }
}

void veParticleSystem::pause()
{
    for (auto emitter : _emitterList){
        emitter->notifyPause();
    }
    
    for (auto affector : _affectorList){
        affector->notifyPause();
    }
}

void veParticleSystem::resume()
{
    for (auto emitter : _emitterList){
        emitter->notifyResume();
    }
    
    for (auto affector : _affectorList){
        affector->notifyResume();
    }
}

void veParticleSystem::stop()
{
    for (auto emitter : _emitterList){
        emitter->notifyStop();
    }
    
    for (auto affector : _affectorList){
        affector->notifyStop();
    }
    unPrepare();
}

void veParticleSystem::prepare()
{
    if (!_isPrepare){
        for (auto emitter : _emitterList){
            emitter->prepare();
        }
        
        for (auto affector : _affectorList){
            affector->prepare();
        }
        
        for (unsigned int i = 0; i < _partilceQuota; ++i){
            _particlePool.addData(new veParticle());
        }
        
        _isPrepare = true;
    }
}

void veParticleSystem::unPrepare()
{
    if (_isPrepare){
        for (auto emitter : _emitterList){
            emitter->unPrepare();
        }
        
        for (auto affector : _affectorList){
            affector->unPrepare();
        }
        _particlePool.lockAllDatas();
        _isPrepare = false;
    }
}

void veParticleSystem::update(veNode *node, veSceneManager *sm)
{
    double deltaTime = sm->getDeltaTime();
    prepare();
    if (!_emitterList.empty()){
        emitPartilces(deltaTime);
        preProcess(deltaTime);
        process(deltaTime);
        postProcess(deltaTime);
    }
    _timeElapseSinceStart += deltaTime;
}

void veParticleSystem::preProcess(double deltaTime)
{
    for (auto emitter : _emitterList){
        if (emitter->isEnabled())
            emitter->preProcess(deltaTime);
    }
    
    for (auto affector : _affectorList){
        if (affector->isEnabled())
            affector->preProcess(deltaTime);
    }
}

void veParticleSystem::process(double deltaTime)
{
    bool firstActiveParticle = true;
    veParticle *particle = static_cast<veParticle *>(_particlePool.getFirst());
    
    while (particle){
        
        if (!particle->isExpired(deltaTime)){
            particle->process();
            
            for (auto affect : _affectorList) {
                if (affect->isEnabled()){
                    affect->process(particle, deltaTime, firstActiveParticle);
                }
            }
            
            firstActiveParticle = false;
            // Keep latest position
            //particle->latestPosition = particle->position;
            
            particle->position += particle->direction * particle->velocity * deltaTime;
            particle->emitter->makeLocalParticles(particle);
        }else{
            _particlePool.lockLatestData();
        }
        
        if (particle->hasEventFlags(veParticle::PEF_EXPIRED)){
            particle->setEventFlags(0);
            particle->addEventFlags(veParticle::PEF_EXPIRED);
        }else{
            particle->setEventFlags(0);
        }
        
        particle->timeToLive -= deltaTime;
        particle = static_cast<veParticle *>(_particlePool.getNext());
    }
    
    if (_renderer.valid())
        static_cast<veParticleRenderer *>(_renderer.get())->update();
}

void veParticleSystem::postProcess(double deltaTime)
{
    for (auto emitter : _emitterList){
        if (emitter->isEnabled())
            emitter->postProcess(deltaTime);
    }
    
    for (auto affector : _affectorList){
        if (affector->isEnabled())
            affector->postProcess(deltaTime);
    }
}

void veParticleSystem::emitPartilces(double deltaTime)
{
    for (auto emitter : _emitterList){
        unsigned short requested = emitter->caculateRequestedParticles(deltaTime);
        if (emitter->isEnabled())
            excuteEmitPartilces(emitter.get(), requested, deltaTime);
    }
}

void veParticleSystem::excuteEmitPartilces(veEmitter *emitter, unsigned short requested, double deltaTime)
{
    double timePoint = 0.f;
    double timeInc = deltaTime / requested;
    for (unsigned short i = 0; i < requested; ++i){
        auto particle = _particlePool.createData();
        if (!particle) return;
        
        particle->timeFraction = 0.0f;
        particle->setEventFlags(0);
        particle->addEventFlags(veParticle::PEF_EMITTED);
        
        emitter->initParticleForEmission(particle);
        particle->position += particle->direction * timePoint;
        timePoint += timeInc;
    }
}

int veParticleSystem::addEmitter(veEmitter *emitter)
{
    if (!emitter) return -1;
    auto iter = std::find(_emitterList.begin(), _emitterList.end(), emitter);
    if (iter != _emitterList.end()) return -1;
    emitter->setParticleSystem(this);
    _emitterList.push_back(emitter);
    return int(_emitterList.size() - 1);
}

bool veParticleSystem::removeEmitter(veEmitter *emitter)
{
    auto iter = std::find(_emitterList.begin(), _emitterList.end(), emitter);
    if (iter == _emitterList.end()) return false;
    (*iter)->setParticleSystem(nullptr);
    _emitterList.erase(iter);
    return true;
}

veEmitter* veParticleSystem::removeEmitter(unsigned int eIndex)
{
    veAssert(eIndex < _emitterList.size());
    veEmitter* emitter = _emitterList[eIndex].get();
    emitter->setParticleSystem(nullptr);
    _emitterList.erase(_emitterList.begin() + eIndex);
    return emitter;
}

veEmitter* veParticleSystem::getEmitter(unsigned int eIndex)
{
    veAssert(eIndex < _emitterList.size());
    return _emitterList[eIndex].get();
}

int veParticleSystem::addAffector(veAffector *affector)
{
    if (!affector) return -1;
    auto iter = std::find(_affectorList.begin(), _affectorList.end(), affector);
    if (iter != _affectorList.end()) return -1;
    affector->setParticleSystem(this);
    _affectorList.push_back(affector);
    return int(_affectorList.size() - 1);
}

bool veParticleSystem::removeAffector(veAffector *affector)
{
    auto iter = std::find(_affectorList.begin(), _affectorList.end(), affector);
    if (iter == _affectorList.end()) return false;
    _affectorList.erase(iter);
    return true;
}

veAffector* veParticleSystem::removeAffector(unsigned int aIndex)
{
    veAssert(aIndex < _affectorList.size());
    veAffector* affector = _affectorList[aIndex].get();
    _affectorList.erase(_affectorList.begin() + aIndex);
    return affector;
}

veAffector* veParticleSystem::getAffector(unsigned int aIndex)
{
    veAssert(aIndex < _affectorList.size());
    return _affectorList[aIndex].get();
}


