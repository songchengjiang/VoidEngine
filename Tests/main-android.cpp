#include <jni.h>
#include <errno.h>
#include "KernelCore/android_native_app_glue.h"
#include "VoidEngine.h"
#include "TestCreator.h"

void android_main(struct android_app* app){
    veApplication::instance()->initWindow(800, 600, "", app);
    TestCreator::createTest();
    veApplication::instance()->run();
}