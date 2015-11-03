#include "VoidEngine.h"
#include "TestCreator.h"

int main(int argc, char **argv)
{
	TestCreator::createTest();
	return veDirector::instance()->run();
}