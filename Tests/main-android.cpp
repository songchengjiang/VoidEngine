#include <jni.h>
#include <errno.h>
#include "VoidEngine.h"
#include "TestCreator.h"

void android_activity_onCreated(jobject activity){
    veApplication::instance()->createViewer(800, 600, "VoidEngineTest");
    TestCreator::createTest();
    veApplication::instance()->run();
}

void android_activity_onDestroy(jobject activity){
    veApplication::instance()->stop();
}
//void android_main(struct android_app* app){
//    static_cast<veApplicationAndroid *>(veApplication::instance())->setAndroidApp(app);
//    veApplication::instance()->createViewer(800, 600, "VoidEngineTest");
//    TestCreator::createTest();
//    veApplication::instance()->run();
//}