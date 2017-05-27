#include <jni.h>
#include <string>
#include "android/log.h"
#include "test.h"
#include "P2Pclient.h"




// todo 1. 这是原始模板，参数稍有不同.
extern "C"
JNIEXPORT jstring
JNICALL
Java_com_dwyane_jni01_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {

    std::string hello = "Hello from C++ 原来的";
    return env->NewStringUTF(hello.c_str());
}

// todo 2. 后来生成的，参数稍有不同
extern "C"
JNIEXPORT jstring
JNICALL
//Java_com_dwyane_jni01_NativeManager_stringFromJNI(JNIEnv *env, jclass type) {
Java_com_dwyane_jni01_NativeManager_stringFromJNI(JNIEnv *env, jobject /* this */) {

    std::string hello = "Hello from C++  新的模板";
    return env->NewStringUTF(hello.c_str());
}

/////////////////////////////////////////////////////////////////////////



extern "C"
JNIEXPORT jstring JNICALL
Java_com_dwyane_jni01_NativeManager_getStrFromNative(JNIEnv *env, jclass type) {
    __android_log_print(ANDROID_LOG_ERROR, "Native_Tag", "这是新方法测试的本地日志");

    std::string hello = "From C++ 新方法测试";

    testNDK();

    return env->NewStringUTF(hello.c_str());
}








extern "C"
JNIEXPORT void JNICALL
Java_com_dwyane_jni01_NativeManager_init(JNIEnv *env, jclass type, jstring serverIP_,
                                         jstring userName_) {
    const char *serverIP = env->GetStringUTFChars(serverIP_, 0);
    const char *userName = env->GetStringUTFChars(userName_, 0);


    __android_log_print(ANDROID_LOG_ERROR, "P2P", "开始进行P2P的初始化，"
            "serverIP：%s, userName:%s", serverIP, userName);
    start(serverIP, userName);


    env->ReleaseStringUTFChars(serverIP_, serverIP);
    env->ReleaseStringUTFChars(userName_, userName);
}




extern "C"
JNIEXPORT void JNICALL
Java_com_dwyane_jni01_NativeManager_sendCmd(JNIEnv *env, jclass type, jstring cmdStr_) {
    const char *cmdStr = env->GetStringUTFChars(cmdStr_, 0);

    __android_log_print(ANDROID_LOG_ERROR, "P2P", "JNI接收命令：%s", cmdStr);


    ParseCommand(cmdStr);


    env->ReleaseStringUTFChars(cmdStr_, cmdStr);
}