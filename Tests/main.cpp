#include "VoidEngine.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
#include "TestCreator.h"
#include "Configuration.h"

int main(int argc, char **argv)
{
#if (VE_PLATFORM == VE_PLATFORM_MAC)
    veConfiguration::VE_DEVICE_PIXEL_RATIO = 2.0f;
#endif
    veApplication::instance()->createViewer(800, 600, "VoidEngineTest");
	TestCreator::createTest();
	return veApplication::instance()->run();
}

#endif