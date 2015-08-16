#ifndef _VE_COMPONENT_
#define _VE_COMPONENT_
#include "Prerequisites.h"
#include "Event.h"

class veNode;
class veVisualiser;
class VE_EXPORT veComponent
{
public:
	veComponent();
	virtual ~veComponent();

	virtual bool handle(double deltaTime, veNode *node, veVisualiser *vs, const veEvent &event) { return false; }
	virtual void update(double deltaTime, veNode *node, veVisualiser *vs) {};

	void setEventFilter(const veEvent::EventType filter) { _filter = filter; };
	veEvent::EventType getEventFilter() const { return _filter; };

protected:

	veEvent::EventType _filter;
};

#endif
