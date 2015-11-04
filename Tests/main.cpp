#include "VoidEngine.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
#include "TestCreator.h"

int main(int argc, char **argv)
{
	veApplication::instance()->initWindow(800, 600, "VoidEngineTest", nullptr);
	TestCreator::createTest();
	return veApplication::instance()->run();
}

#endif