#ifndef _VE_COMPONENT_
#define _VE_COMPONENT_
#include "Prerequisites.h"
#include "Event.h"

class veNode;
class veSceneManager;
class VE_EXPORT veComponent
{
public:
	veComponent();
	virtual ~veComponent();

	USE_VE_PTR;

	virtual bool handle(veNode *node, veSceneManager *sm, const veEvent &event) { return false; }
	virtual void update(veNode *node, veSceneManager *sm) {}

	void setEventFilter(const veEvent::EventType filter) { _filter = filter; };
	veEvent::EventType getEventFilter() const { return _filter; };
    
    void setEnable(bool isEnable) { _isEnabled = isEnable; }
    bool getEnabled() { return _isEnabled; }

protected:

	veEvent::EventType _filter;
    bool _isEnabled;
};

#endif
