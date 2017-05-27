#include "test.h"
#include "android/log.h"


int testNDK() {
    __android_log_print(ANDROID_LOG_ERROR, "Native_Tag", "C文件里面的日志 4444444444");
    return 0;
}
