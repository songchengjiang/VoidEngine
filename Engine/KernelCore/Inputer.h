#ifndef _VE_INPUTER_
#define _VE_INPUTER_
#include "Component.h"
#include "Event.h"
#include "Registrar.h"

class veVisualiser;
class VE_EXPORT veInputer : public veComponent
{
public:
	veInputer();
	~veInputer();

	virtual bool input(const veEvent &event, const veVisualiser *vs) = 0;

	void setFilter(const veEvent::EventType filter) { _filter = filter; };
	veEvent::EventType getFilter() const { return _filter; };

protected:

	veEvent::EventType _filter;
	unsigned int _id;
};

typedef veRegistrar<unsigned int, veInputer> veInputerRegistrar;

#endif
