#ifndef _BASE_TEST_
#define _BASE_TEST_
#include "VoidEngine.h"

class BaseTest
{
public:
	BaseTest() {
		_visualiser = veDirector::DIRECTOR.createVisualiser(800, 600, "Game");
		_visualiser->getCamera(0)->setViewMatrixAslookAt(veVec3(0.0f, 0.0f, 10.0f), veVec3::ZERO, veVec3::UNIT_Y);
	};
	~BaseTest() {};

	int run() {
		return veDirector::DIRECTOR.run();
	}

protected:

	veVisualiser *_visualiser;
};

#endif