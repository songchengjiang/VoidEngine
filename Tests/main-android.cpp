#include <jni.h>
#include <errno.h>
#include <KernelCore/Application-android.h>
#include "android_native_app_glue.h"
#include "VoidEngine.h"
#include "TestCreator.h"

void android_main(struct android_app* app){
    static_cast<veApplicationAndroid *>(veApplication::instance())->setAndroidApp(app);
    veApplication::instance()->createViewer(800, 600, "VoidEngineTest");
    TestCreator::createTest();
    veApplication::instance()->run();
}