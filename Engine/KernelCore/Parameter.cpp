#include "Parameter.h"

veParameter::veParameter(int val)
	: USE_VE_PTR_INIT
{
	set(val);
}

veParameter::veParameter(bool val)
	: USE_VE_PTR_INIT
{
	set(val);
}

veParameter::veParameter(veReal val)
	: USE_VE_PTR_INIT
{
	set(val);
}

veParameter::veParameter(const veVec2 &val)
	: USE_VE_PTR_INIT
{
	set(val);
}

veParameter::veParameter(const veVec3 & val)
	: USE_VE_PTR_INIT
{
	set(val);
}

veParameter::veParameter(const veVec4 & val)
	: USE_VE_PTR_INIT
{
	set(val);
}

veParameter::veParameter()
	: USE_VE_PTR_INIT
{

}

veParameter::~veParameter()
{

}

void veParameter::set(int val)
{
	_type = Type::INT;
	_values[0] = val;
}

void veParameter::set(bool val)
{
	_type = Type::BOOL;
	_values[0] = val == true? 1: 0;
}

void veParameter::set(veReal val)
{
	_type = Type::REAL;
	_values[0] = val;
}

void veParameter::set(const veVec2 &val)
{
	_type = Type::VEC2;
	_values[0] = val.x();
	_values[1] = val.y();
}

void veParameter::set(const veVec3 & val)
{
	_type = Type::VEC3;
	_values[0] = val.x();
	_values[1] = val.y();
	_values[2] = val.z();
}

void veParameter::set(const veVec4 & val)
{
	_type = Type::VEC4;
	_values[0] = val.x();
	_values[1] = val.y();
	_values[2] = val.z();
	_values[3] = val.w();
}

bool veParameter::get(int &val) const
{
	if (_type != Type::INT) return false;
	val = (int)_values[0];
	return true;
}

bool veParameter::get(bool &val) const
{
	if (_type != Type::BOOL) return false;
	val = _values[0] == 0? false: true;
	return true;
}

bool veParameter::get(veReal &val) const
{
	if (_type != Type::REAL) return false;
	val = _values[0];
	return true;
}

bool veParameter::get(veVec2 &val) const
{
	if (_type != Type::VEC2) return false;
	val.x() = _values[0];
	val.y() = _values[1];
	return true;
}

bool veParameter::get(veVec3 & val) const
{
	if (_type != Type::VEC3) return false;
	val.x() = _values[0];
	val.y() = _values[1];
	val.z() = _values[2];
	return true;
}

bool veParameter::get(veVec4 & val) const
{
	if (_type != Type::VEC4) return false;
	val.x() = _values[0];
	val.y() = _values[1];
	val.z() = _values[2];
	val.w() = _values[3];
	return true;
}
