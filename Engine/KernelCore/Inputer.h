#ifndef _VE_INPUTER_
#define _VE_INPUTER_
#include "Event.h"
#include "Registrar.h"
#include "Component.h"

class veVisualiser;
class veNode;
class VE_EXPORT veInputer : public veComponent
{
public:
	veInputer();
	~veInputer();

	void setFilter(const veEvent::EventType filter) { _filter = filter; };
	veEvent::EventType getFilter() const { return _filter; };

protected:

	veEvent::EventType _filter;
};

#endif
