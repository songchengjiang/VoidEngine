#include "VoidEngine.h"
#include "BaseTest.h"
#include "MeshTest.h"
#include "ImageTest.h"
#include "RenToTex.h"
#include "CameraTest.h"
#include "LightTest.h"
#include "TextTex.h"

int main(int argc, char **argv)
{
	//auto test = new LightTest;
	//auto test = new CameraTest;
	//auto test = new RenToTex;
	//auto test = new MeshTest;
	//auto test = new ImageTest;
	auto test = new TextTex;
	return test->run();
}