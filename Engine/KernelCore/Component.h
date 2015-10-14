#ifndef _VE_COMPONENT_
#define _VE_COMPONENT_
#include "Prerequisites.h"
#include "Event.h"

class veNode;
class veVisualiser;
class veSceneManager;
class veCamera;
class VE_EXPORT veComponent
{
public:
	veComponent();
	virtual ~veComponent();

	USE_VE_PTR;

	virtual bool handle(veNode *node, veVisualiser *vs, const veEvent &event) { return false; }
	virtual void update(veNode *node, veSceneManager *sm) {}
    virtual void render(veCamera *camera){}

	void setEventFilter(const veEvent::EventType filter) { _filter = filter; };
	veEvent::EventType getEventFilter() const { return _filter; };
    
    void setEnable(bool isEnable) { _isEnabled = isEnable; }
    bool getEnabled() { return _isEnabled; }

protected:

	veEvent::EventType _filter;
    bool _isEnabled;
};

#endif
