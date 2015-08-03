#include "VoidEngine.h"

int main(int argc, char **argv)
{
	veMat4 m;
	m.makeTransform(veVec3(10.0f, 0.0f, 0.0f), veVec3(0.5f), veQuat(veMath::QUARTER_PI, veVec3::UNIT_Y));
	vePrinter::print(m);

	VE_Ptr<veRenderer> renderer = new veRenderer;
	VE_Ptr<veRenderer> renderer2 = renderer;

	if (renderer == renderer2){
		vePrinter::print(true);
	}

	while (true);
	return 0;
}
