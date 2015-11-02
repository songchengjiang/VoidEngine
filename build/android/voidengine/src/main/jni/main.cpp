//
// Created by songcheng.jiang on 2015/11/2.
//

#include "native_app_glue/android_native_app_glue.h"
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "VoidEngine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "VoidEngine", __VA_ARGS__))

void android_main(struct android_app* app){
}

