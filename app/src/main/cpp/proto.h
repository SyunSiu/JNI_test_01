/* P2P 程序传输协议
 * 
 * 日期：2011-05-31
 *
 * 作者 jamesxie(j373553739@163.com)
 *
 */

#ifndef PROTO_H
#define PROTO_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// 定义iMessageType的值
//这个是用户登陆给服务器发送的消息
#define LOGIN               1
//这个是用户下线给服务器发送的消息
#define LOGOUT              2
#define P2PTRANS            3
#define GETALLUSER          4
//这个是一个用户上线给其他在线用户发送的消息,这个是服务端给客户端发送的消息
#define ONE_LOGIN           5
//这个是一个用户下线给其他用户发送的消息，这个是服务端给客户端发送的消息
#define ONE_LOGOUT          6
//这个是用户给服务器发送的心跳包对应的命令
#define TICKONLINE          7
//这个是服务器发送给客户的心跳包的回应
#define TICKONLINEBAK       8

// 服务器端口
#define SERVER_PORT 2280

// Client登录时向服务器发送的消息
typedef struct _stLoginMessage {
    char userName[10];
    char password[10];
} stLoginMessage;

// Client注销时发送的消息
typedef struct _stLogoutMessage {
    char userName[10];
} stLogoutMessage;

// Client向服务器请求另外一个Client(userName)向自己方向发送UDP打洞消息
typedef struct _stP2PTranslate {
    char userName[10];
} stP2PTranslate;

//这是一个心跳包格式
typedef struct _stTickMessage {
    char userName[10];
} stTickMessage;

// Client向服务器发送的消息格式
typedef struct _stMessage {
    int iMessageType;
    union _message {
        stLoginMessage loginmember;
        stLogoutMessage logoutmember;
        stP2PTranslate translatemessage;
        stTickMessage tickmessage;   //心跳包
    } message;
} stMessage;

// 客户节点信息
typedef struct _stUserListNode {
    char userName[10];
    unsigned int ip;
    unsigned short port;
} stUserListNode;

#if 0
// Server向Client发送的消息
typedef struct _stServerToClient
{
    int iMessageType;
    union _message
    {
        stUserListNode user;
    }message;

} stServerToClient ;
#endif

// Server向Client发送的消息
typedef struct _stServerToClient {
    int iMessageType;

    stUserListNode user;

} stServerToClient;


//======================================
// 下面的协议用于客户端之间的通信
//======================================
#define P2PMESSAGE 100               // 发送消息
#define P2PMESSAGEACK 101            // 收到消息的应答
#define P2PSOMEONEWANTTOCALLYOU 102  // 服务器向客户端发送的消息
// 希望此客户端发送一个UDP打洞包
#define P2PTRASH        103          // 客户端发送的打洞包，接收端应该忽略此消息

// 客户端之间发送消息格式
typedef struct _stP2PMessage {
    int iMessageType;
    int iStringLen;         // or IP address
    unsigned short Port;
} stP2PMessage;

typedef struct _UserList {
    stUserListNode *oneUserNode;
    struct _UserList *next;
} UserList;


//====================================
//这个是记录用户登陆信息的结构体链表，
//只有服务器端需要这个链表，因为每个
//客户端只需要记住自己的相关信息就可以
//不需要链表
//logintime不会改变，alivetime每次受到
//客户端的心跳消息，并且给客户端发送
//回应消息成功之后就更新alivetime时间
//====================================

typedef struct _UserLoginImfoLink {
    stUserListNode *userNode;
    time_t logintime;     //登陆的时间，这个可以得到用户总共在线的时长
    time_t alivetime;      //上次的活动时间，用这个时间判断是否断开连接
    struct _UserLoginImfo *next;
} UserLoginImfoLink;

//心跳时间间隔，表示90秒钟向服务器报道一次
#define   TICKTIMESPACE       90

#endif 

