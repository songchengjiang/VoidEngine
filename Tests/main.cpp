#include "VoidEngine.h"
#include "MeshTest.h"
#include "ImageTest.h"
#include "RenToTex.h"

int main(int argc, char **argv)
{
	//auto test = new RenToTex;
	auto test = new MeshTest;
	//auto test = new ImageTest;
	return test->run();
}
