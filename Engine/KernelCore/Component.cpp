#include "Component.h"

veComponent::veComponent()
	: USE_VE_PTR_INIT
	, _filter(veEvent::VE_ALL_EVENT)
    , _isEnabled(true)
{
}

veComponent::~veComponent()
{

}
