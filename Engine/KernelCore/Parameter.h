#ifndef _VE_PARAMETER_
#define _VE_PARAMETER_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "BaseCore/Vector2.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Vector4.h"
#include "BaseCore/Matrix3.h"
#include "BaseCore/Matrix4.h"

class VE_EXPORT veParameter
{
public:

	enum class Type
	{
		INT,
		BOOL,
		REAL,
		VEC2,
		VEC3,
		VEC4,
	};

	veParameter();
	veParameter(int val);
	veParameter(bool val);
	veParameter(veReal val);
	veParameter(const veVec2 &val);
	veParameter(const veVec3 & val);
	veParameter(const veVec4 & val);
	~veParameter();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void set(int val);
	void set(bool val);
	void set(veReal val);
	void set(const veVec2 &val);
	void set(const veVec3 & val);
	void set(const veVec4 & val);

	bool get(int &val) const;
	bool get(bool &val) const;
	bool get(veReal &val) const;
	bool get(veVec2 &val) const;
	bool get(veVec3 & val) const;
	bool get(veVec4 & val) const;

	Type getType() const { return _type; }

private:

	Type _type;
	veReal _values[4];
};

typedef std::vector< VE_Ptr<veParameter> > veParameterList;

#endif