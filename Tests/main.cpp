#include "VoidEngine.h"
#include "TestCreator.h"

int main(int argc, char **argv)
{
	veApplication::instance()->initWindow(800, 600, "VoidEngineTest");
	TestCreator::createTest();
	return veApplication::instance()->run();
}