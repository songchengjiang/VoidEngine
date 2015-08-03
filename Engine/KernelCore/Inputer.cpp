#include "Inputer.h"
#include "IDGenerator.h"

veInputer::veInputer()
	: _filter(veEvent::VE_ALL_EVENT)
{
	_id = IDGenerator::generateUniqueID();
	veInputerRegistrar::instance()->reg(_id, this);
}

veInputer::~veInputer()
{
	veInputerRegistrar::instance()->unReg(_id);
}