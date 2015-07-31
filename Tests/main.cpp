#include "VoidEngine.h"

int main(int argc, char **argv)
{
	veMat4 m;
	m.makeTransform(veVec3(10.0f, 0.0f, 0.0f), veVec3(0.5f), veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
	Printer::print(m);

	while (true);
	return 0;
}
