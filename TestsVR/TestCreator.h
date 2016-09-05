#ifndef _TEST_CREATOR_
#define _TEST_CREATOR_
#include "BaseTest.h"

class TestCreator
{
public:

	static void createTest(const veVRHelper &vrHelper) {
        auto test = new BaseTest(vrHelper);
		test->init();
	}

private:
	TestCreator();
	~TestCreator();
};

#endif