#include "BaseForceAffector.h"
#include "ParticleSystem.h"
#include "KernelCore/Node.h"

veBaseForceAffector::veBaseForceAffector()
    : _forceApplication(veBaseForceAffector::FA_ADD)
    , _forceScalar(1.f)
{
}

veBaseForceAffector::~veBaseForceAffector()
{
    
}

void veBaseForceAffector::preProcess(double deltaTime)
{
    veQuat worldRotation;
    auto node = _system->getParents()[0];
    auto nTow = node->getNodeToWorldMatrix() * veMat4::transform(_position, veVec3::UNIT_SCALE, _rotation);
    nTow.decomposition(nullptr, nullptr, &worldRotation);
    _forceVector = worldRotation * veVec3::UNIT_Y * _forceScalar;
}