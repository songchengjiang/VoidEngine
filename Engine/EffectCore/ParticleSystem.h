#ifndef _VE_PARTICLE_SYSTEM_
#define _VE_PARTICLE_SYSTEM_
#include "Prerequisites.h"
#include "KernelCore/RenderableObject.h"
#include "Particle.h"
#include "Emitter.h"
#include "Affector.h"
#include <mutex>

class VE_EXPORT veParticleSystem : public veRenderableObject
{
    friend class veSceneManager;
public:
    
    ~veParticleSystem();
    
    virtual void update(veNode *node, veSceneManager *sm) override;
    virtual void setRenderer(veRenderer *renderer) override { _renderer = renderer; _renderer->setRenderStageMask(veRenderer::RENDERING);}
    
    void start();
    void pause();
    void resume();
    void stop();
    
    int addEmitter(veEmitter *emitter);
    bool removeEmitter(veEmitter *emitter);
    veEmitter* removeEmitter(unsigned int eIndex);
    veEmitter* getEmitter(unsigned int eIndex);
    size_t getEmitterCount() const { return _emitterList.size(); }
    
    int addAffector(veAffector *affector);
    bool removeAffector(veAffector *affector);
    veAffector* removeAffector(unsigned int aIndex);
    veAffector* getAffector(unsigned int aIndex);
    size_t getAffectorCount() const { return _affectorList.size(); }
    
    const ParticlePool& getParticles() const { return _particlePool; }
    double getTimeElapsedSinceStart() const { return _timeElapseSinceStart; }
    
    void setParticleQuota(unsigned int quota) { _partilceQuota = quota; }
    unsigned int getParticleQuota() const { return _partilceQuota; }

    void lock();
    void unLock();
    
protected:
    
    veParticleSystem(veSceneManager *sm);
    
    void prepare();
    void unPrepare();
    void preProcess(double deltaTime);
    void process(double deltaTime);
    void postProcess(double deltaTime);
    void emitPartilces(double deltaTime);
    void excuteEmitPartilces(veEmitter *emitter, unsigned short requested, double deltaTime);
    
    void updateBoundingBox(veParticle *particle);
    
protected:
    
    ParticlePool _particlePool;
    unsigned int _partilceQuota;
    bool         _isPrepare;
    double       _timeElapseSinceStart;
    std::mutex   _dataMutex;
    
    std::vector< VE_Ptr<veEmitter> >  _emitterList;
    std::vector< VE_Ptr<veAffector> > _affectorList;
};

typedef std::vector< VE_Ptr<veParticleSystem> > veParticleSystemList;

#endif
