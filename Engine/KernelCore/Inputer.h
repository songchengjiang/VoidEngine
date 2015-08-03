#ifndef _VE_INPUTER_
#define _VE_INPUTER_
#include "Component.h"
#include "Event.h"

class VE_EXPORT veInputer : public veComponent
{
public:

	enum InputerType
	{
		Keyboard,
		Mouse,
		Touch,
	};

	virtual bool handle(const veEvent &event) = 0;

protected:

	veInputer(InputerType type);
	~veInputer();

protected:

	InputerType _inputerType;
};

#endif
