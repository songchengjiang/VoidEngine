#ifndef _VE_INPUTER_
#define _VE_INPUTER_
#include "Event.h"
#include "Registrar.h"

class veVisualiser;
class veNode;
class VE_EXPORT veInputer
{
public:
	veInputer();
	~veInputer();

	virtual bool input(veNode *node, const veEvent &event, const veVisualiser *vs) = 0;

	void setFilter(const veEvent::EventType filter) { _filter = filter; };
	veEvent::EventType getFilter() const { return _filter; };

protected:

	veEvent::EventType _filter;
};

typedef veRegistrar<unsigned int, veInputer> veInputerRegistrar;

#endif
