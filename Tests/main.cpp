#include "VoidEngine.h"
#include "MeshTest.h"
#include "ImageTest.h"
#include "RenToTex.h"
#include "CameraTest.h"

int main(int argc, char **argv)
{
	auto test = new CameraTest;
	//auto test = new RenToTex;
	//auto test = new MeshTest;
	//auto test = new ImageTest;
	return test->run();
}
