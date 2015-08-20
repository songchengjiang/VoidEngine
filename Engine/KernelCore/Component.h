#ifndef _VE_COMPONENT_
#define _VE_COMPONENT_
#include "Prerequisites.h"
#include "Event.h"

class veNode;
class veVisualiser;
class VE_EXPORT veComponent
{
	USE_VE_PTR
public:
	veComponent();
	virtual ~veComponent();

	virtual bool handle(veNode *node, veVisualiser *vs, const veEvent &event) { return false; }
	virtual void update(veNode *node, veVisualiser *vs) {};

	void setEventFilter(const veEvent::EventType filter) { _filter = filter; };
	veEvent::EventType getEventFilter() const { return _filter; };

protected:

	veEvent::EventType _filter;
};

#endif
