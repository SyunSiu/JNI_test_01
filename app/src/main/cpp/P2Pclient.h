//
// Created by TonyDwyane on 2017/5/26.
//

#ifndef JNI01_P2PCLIENT_H
#define JNI01_P2PCLIENT_H


#ifdef __cplusplus
extern "C" {
#endif

//int start();
int start(const char *serverIP, const char *userName);
void ParseCommand(const char *CommandLine);


#ifdef __cplusplus
}
#endif


#endif //JNI01_P2PCLIENT_H
