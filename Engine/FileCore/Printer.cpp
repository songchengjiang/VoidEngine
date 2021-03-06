#include "Printer.h"
#include <iostream>

static char outputString[512];

void vePrinter::print(const char* str)
{
	std::cout << str << std::endl;
}

void vePrinter::print(const bool val)
{
	sprintf(outputString, "bool: %s", val == true ? "True" : "False");
	std::cout << outputString << std::endl;
}

void vePrinter::print(const veReal val)
{
	sprintf(outputString, "veReal: %f", val);
	std::cout << outputString << std::endl;
}

void vePrinter::print(const veVec2 &v)
{
	sprintf(outputString, "veVec2: (%f, %f)", v.x(), v.y());
	std::cout << outputString << std::endl;
}

void vePrinter::print(const veVec3 &v)
{
	sprintf(outputString, "veVec3: (%f, %f, %f)", v.x(), v.y(), v.z());
	std::cout << outputString << std::endl;
}

void vePrinter::print(const veVec4 &v)
{
	sprintf(outputString, "veVec4: (%f, %f, %f, %f)", v.x(), v.y(), v.z(), v.w());
	std::cout << outputString << std::endl;
}

void vePrinter::print(const veQuat &q)
{
	sprintf(outputString, "veQuat: (%f, %f, %f, %f)", q.w(), q.x(), q.y(), q.z());
	std::cout << outputString << std::endl;
}

void vePrinter::print(const veMat3 &m)
{
	sprintf(outputString, "veMat3: {%.6f, %.6f, %.6f \n        %.6f, %.6f, %.6f \n        %.6f, %.6f, %.6f}",
		m[0][0], m[0][1], m[0][2],
		m[1][0], m[1][1], m[1][2],
		m[2][0], m[2][1], m[2][2]);
	std::cout << outputString << std::endl;
}

void vePrinter::print(const veMat4 &m)
{
	sprintf(outputString, "veMat4: {%.6f, %.6f, %.6f, %.6f \n        %.6f, %.6f, %.6f, %.6f \n        %.6f, %.6f, %.6f, %.6f \n        %.6f, %.6f, %.6f, %.6f}", 
		m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3]);
	std::cout << outputString << std::endl;
}