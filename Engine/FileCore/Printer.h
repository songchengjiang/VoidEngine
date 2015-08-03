#ifndef _VE_PRINTER_
#define _VE_PRINTER_
#include "Prerequisites.h"
#include "BaseCore/Matrix3.h"
#include "BaseCore/Matrix4.h"
#include "BaseCore/Quaternion.h"
#include "BaseCore/Vector2.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Vector4.h"

class VE_EXPORT vePrinter
{
public:
	vePrinter();
	~vePrinter();

	static void print(const bool val);
	static void print(const veReal val);
	static void print(const veVec2 &v);
	static void print(const veVec3 &v);
	static void print(const veVec4 &v);
	static void print(const veQuat &q);
	static void print(const veMat3 &m);
	static void print(const veMat4 &m);
};

#endif

