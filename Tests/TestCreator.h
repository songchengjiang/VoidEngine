#ifndef _TEST_CREATOR_
#define _TEST_CREATOR_
#include "BaseTest.h"
#include "LightTest.h"
#include "CameraTest.h"
#include "RenToTex.h"
#include "MeshTest.h"
#include "ImageTest.h"
#include "TextTest.h"
#include "PerformanceTest.h"
#include "IntersectionTest.h"
#include "SkyBoxTest.h"
//
class TestCreator
{
public:

	static void createTest() {
		//auto test = new LightTest;
		//auto test = new CameraTest;
		//auto test = new RenToTex;
		auto test = new MeshTest;
		//auto test = new ImageTest;
		//auto test = new TextTest;
		//auto test = new PerformanceTest;
		//auto test = new IntersectionTest;
		//auto test = new SkyBoxTest;
		test->init();
	}

private:
	TestCreator();
	~TestCreator();
};

#endif