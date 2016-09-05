#ifndef _VRHELPER_H_
#define _VRHELPER_H_

#include <jni.h>

struct veVRHelper
{
    JNIEnv  *javaEnv;
    JavaVM  *javaVM;
    jobject  activity;
};


#endif
