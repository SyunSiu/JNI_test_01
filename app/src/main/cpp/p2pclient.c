/* P2P 程序客户端
 * 
 * 文件名：P2PClient.c
 *
 * 日期：2011-05-31
 *
 * 作者：jamesxie(j373553739@163.com)
 *
 */


#include <stdio.h>
#include "proto.h"
#include "android/log.h"


UserList *head = NULL;


//这个记录用户登陆的时间
time_t logintime = 0;


#define COMMANDMAXC 256
#define MAXRETRY    5

int PrimaryUDP;
char UserName[10];
char ServerIP[20];

// 0 == flase ; 1 == true 
int RecvedACK;

UserList *creat_UserList() {
    UserList *h;

    if ((h = (UserList *) malloc(sizeof(UserList))) == NULL) {
        printf("No Memory to malloc !");
        exit(1);
    }
    h->oneUserNode = NULL;

    h->next = NULL;
    return h;
}


void insertNode(stUserListNode *user) {

    UserList *s, *current, *last;
    if ((s = (UserList *) malloc(sizeof(UserList))) == NULL) {
        printf("No Memory to malloc !");
        exit(0);
    }

    if (user) {
        s->oneUserNode = user;
    }


    if (head) {
        last = head;
        current = (UserList *) (head->next);
        while (current) {
            //if exit , don't insert !
            if (strcmp(current->oneUserNode->userName, user->userName) == 0) {
                if ((current->oneUserNode->ip == user->ip) &&
                    (current->oneUserNode->port == user->port)) {
                    printf("you have login in !");
                    return;
                }

            }
            last = (UserList *) current;
            current = (UserList *) (current->next);
        }
        last->next = s;
        s->next = NULL;
    }

}


void removeNode(stUserListNode *user) {
    UserList *current, *last;
    if (head) {
        last = head;
        current = (UserList *) (head->next);
        while (current) {
            printf("the user->userName is :%s\n", user->userName);
            if (strcmp(current->oneUserNode->userName, user->userName) == 0) {
                //如果哦不是链表的最后一个节点
                if (current->next) {
                    last->next = current->next;

                } else    //如果是链表的最后一个节点
                {
                    last->next = NULL;
                }
                free(current);
                return;

            }
            last = current;
            current = (UserList *) (current->next);
        }
        printf("Client :can not find one to remove!\n");

    }
}


int getNodeNum() {
    int nodenum = 0;
    UserList *current;
    if (head) {
        current = (UserList *) (head->next);
        while (current) {
            nodenum += 1;
            current = (UserList *) (current->next);
        }
        return nodenum;
    }
    return 0;
}


stUserListNode *GetUser(char *username) {
    UserList *current;
    stUserListNode *usernode = NULL;
    if (head) {
        current = (UserList *) (head->next);
        while (current) {
            if (strcmp(current->oneUserNode->userName, username) == 0) {
                usernode = current->oneUserNode;
                return usernode;
            }
            current = (UserList *) (current->next);
        }
        printf("can not find one to remove!\n");

    }
    return usernode;
}


int mksock(int type) {
    int sock = socket(AF_INET, type, 0);
    if (sock < 0) {
        perror("create socket error");
        exit(1);
    }
    return sock;
}

void BindSock(int sock) {
    struct sockaddr_in sin;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_family = AF_INET;
    sin.sin_port = 0;

    if (bind(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("call to bind");
        exit(1);
    }
}

void ConnectToServer(int sock, char *username, char *serverip) {

    struct sockaddr_in remote;
    remote.sin_addr.s_addr = inet_addr(serverip);
    remote.sin_family = AF_INET;
    remote.sin_port = htons(SERVER_PORT);

    stMessage sendbuf;
    sendbuf.iMessageType = LOGIN;
    strncpy(sendbuf.message.loginmember.userName, username, 10);

    sendto(sock, (const char *) &sendbuf, sizeof(sendbuf), 0, (struct sockaddr *) &remote,
           sizeof(remote));

    int usercount, i;
    int fromlen = sizeof(remote);
    int iread = recvfrom(sock, (char *) &usercount, sizeof(int), 0, (struct sockaddr *) &remote,
                         &fromlen);
    if (iread <= 0) {
        perror("recvfrom error !");
        exit(1);
    }

    // 登录到服务端后，接收服务端发来的已经登录的用户的信息
    printf("ConnectToServer :Have %d users logined server:\n", usercount);
    for (i = 0; i < usercount; i++) {
        stUserListNode *node = (stUserListNode *) malloc(sizeof(stUserListNode));
        if (!node) {
            printf("No Memory to malloc !");
            exit(0);
        }
        recvfrom(sock, (char *) node, sizeof(stUserListNode), 0, (struct sockaddr *) &remote,
                 &fromlen);
        //ClientList.push_back(node);
        insertNode(node);

        printf("Username : %s\n", node->userName);
        struct in_addr tmp;
        tmp.s_addr = htonl(node->ip);
        printf("UserIP : %s\n", inet_ntoa(tmp));
        printf("UserPort : %d\n", node->port);
    }
    //这个时候表示登陆成功，记录登陆时间
    time(&logintime);
    printf("the logintime is : %d\n", logintime);
}



/* 这是主要的函数：发送一个消息给某个用户(C)
 *流程：直接向某个用户的外网IP发送消息，如果此前没有联系过
 *      那么此消息将无法发送，发送端等待超时。
 *      超时后，发送端将发送一个请求信息到服务端，
 *      要求服务端发送给客户C一个请求，请求C给本机发送打洞消息
 *      以上流程将重复MAXRETRY次
 */
// 0 == false ; 1 == true 
int SendMessageTo(char *UserName, char *Message) {
    char realmessage[256];
    unsigned int UserIP;
    unsigned short UserPort;

    int FindUser = 0, i, j;

    UserList *current = NULL;
    //printf("the UserName is :%s\n",UserName);
    for (current = (UserList *) (head->next);
         current != NULL; current = (UserList *) (current->next)) {
        printf("the current->oneUserNode->userName is %s\n", current->oneUserNode->userName);
        if (strcmp(current->oneUserNode->userName, UserName) == 0) {
            printf("find the UserName in UserList!\n");
            UserIP = current->oneUserNode->ip;
            UserPort = current->oneUserNode->port;
            FindUser = 1;
            break;
        }
    }

    if (!FindUser) {
        printf("can not find name to SendMessageTo !\n");
        return 0;
    }


    strcpy(realmessage, Message);

    for (i = 0; i < MAXRETRY; i++) {
        RecvedACK = 0;

        struct sockaddr_in remote;
        remote.sin_addr.s_addr = htonl(UserIP);
        remote.sin_family = AF_INET;
        remote.sin_port = htons(UserPort);
        stP2PMessage MessageHead;
        MessageHead.iMessageType = P2PMESSAGE;
        MessageHead.iStringLen = (int) strlen(realmessage) + 1;
        int isend = sendto(PrimaryUDP, (const char *) &MessageHead, sizeof(MessageHead), 0,
                           (struct sockaddr *) &remote, sizeof(remote));
        isend = sendto(PrimaryUDP, (const char *) &realmessage, MessageHead.iStringLen, 0,
                       (struct sockaddr *) &remote, sizeof(remote));

        // 等待接收线程将此标记修改
        for (j = 0; j < 10; j++) {
            if (RecvedACK)
                return 1;
            else
                sleep(1);
        }

        // 没有接收到目标主机的回应，认为目标主机的端口映射没有
        // 打开，那么发送请求信息给服务器，要服务器告诉目标主机
        // 打开映射端口（UDP打洞）
        struct sockaddr_in server;
        server.sin_addr.s_addr = inet_addr(ServerIP);
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT);

        stMessage transMessage;
        transMessage.iMessageType = P2PTRANS;
        strcpy(transMessage.message.translatemessage.userName, UserName);

        sendto(PrimaryUDP, (const char *) &transMessage, sizeof(transMessage), 0,
               (struct sockaddr *) &server, sizeof(server));
        sleep(3);// 等待对方先发送信息。
    }
    return 0;
}


// 解析命令，暂时只有exit和send命令
// 新增getu命令，获取当前服务器的所有用户
void ParseCommand(const char *CommandLine) {
    if (strlen(CommandLine) < 4) {
        return;
    }

    char Command[10];
    strncpy(Command, CommandLine, 4);
    Command[4] = '\0';


    // 退出命令
    if (strcmp(Command, "exit") == 0) {
        stMessage sendbuf;
        sendbuf.iMessageType = LOGOUT;
        strncpy(sendbuf.message.logoutmember.userName, UserName, 10);
        struct sockaddr_in server;
        server.sin_addr.s_addr = inet_addr(ServerIP);
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT);

        sendto(PrimaryUDP, (const char *) &sendbuf, sizeof(sendbuf), 0, (struct sockaddr *) &server,
               sizeof(server));

        if (-1 == close(PrimaryUDP)) {
            perror("call to close.\n");
            exit(1);
        }
        exit(0);


        // 发送命令
    } else if (strcmp(Command, "send") == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "P2P", "发送消息：%s", Command);


        char sendname[20];
        char message[COMMANDMAXC];
        int i;
        for (i = 5;; i++) {
            if (CommandLine[i] != ' ')
                sendname[i - 5] = CommandLine[i];
            else {
                sendname[i - 5] = '\0';
                break;
            }
        }
        strcpy(message, &(CommandLine[i + 1]));
        if (SendMessageTo(sendname, message)) {
            printf("Send OK!\n");
        } else {
            printf("Send Failure!\n");
        }


        // 获取所有用户命令
    } else if (strcmp(Command, "getu") == 0) {
        int command = GETALLUSER;
        struct sockaddr_in server;
        server.sin_addr.s_addr = inet_addr(ServerIP);
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT);

        sendto(PrimaryUDP, (const char *) &command, sizeof(command), 0, (struct sockaddr *) &server,
               sizeof(server));
    }
}


/****************************************************************
 *心跳线程，每隔一个心跳时间间隔，给服务器发送一条消息，表示我还在
 *****************************************************************/
void TickThreadProc(void) {
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ServerIP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    stMessage tickMessage;
    tickMessage.iMessageType = TICKONLINE;
    strcpy(tickMessage.message.tickmessage.userName, UserName);

    for (;;) {
        //logintime == 0 时还么登陆
        if (logintime != 0) {
            time_t nowtime;
            time(&nowtime);
            //printf("nowtime is :%d\n",nowtime);
            //刚好到达下一个心跳的时间
            if (TICKTIMESPACE <= (nowtime - logintime)) {
                printf("send tickMessage to Server !");
                printf("nowtime is :%d\n", nowtime);
                sendto(PrimaryUDP, (const char *) &tickMessage, sizeof(tickMessage), 0,
                       (struct sockaddr *) &server, sizeof(server));
                sleep(1);  //等待发送完成
            }
                //不至于那么快，缓解cpu压力
            else {
                sleep(2);
            }
        } else {
            sleep(1);
        }
    }
}


// 接受消息线程
void RecvThreadProc(void) {
    struct sockaddr_in remote;
    int sinlen = sizeof(remote);
    int i;
    stP2PMessage recvbuf;
    for (;;) {
        int iread = recvfrom(PrimaryUDP, (char *) &recvbuf, sizeof(recvbuf), 0,
                             (struct sockaddr *) &remote, &sinlen);
        if (iread <= 0) {
            printf("recv error\n");
            continue;
        }
        printf("the recvbuf.iMessageType is :%d\n", recvbuf.iMessageType);
        switch (recvbuf.iMessageType) {
            case P2PMESSAGE: {
                // 接收到P2P的消息
                //char *comemessage= new char[recvbuf.iStringLen];
                char *comemessage = (char *) malloc(recvbuf.iStringLen);
                int iread1 = recvfrom(PrimaryUDP, comemessage, 256, 0, (struct sockaddr *) &remote,
                                      &sinlen);
                if (iread1 <= 0) {
                    perror("recv error");
                    exit(1);
                } else {
                    comemessage[iread1 - 1] = '\0';
                    printf("Recv a Message:%s\n", comemessage);
                    __android_log_print(ANDROID_LOG_ERROR, "P2P", "Recv a Message:%s\n",
                                        comemessage);


                    stP2PMessage sendbuf;
                    sendbuf.iMessageType = P2PMESSAGEACK;
                    sendto(PrimaryUDP, (const char *) &sendbuf, sizeof(sendbuf), 0,
                           (struct sockaddr *) &remote, sizeof(remote));
                }

                free(comemessage);
                break;

            }
            case P2PSOMEONEWANTTOCALLYOU: {
                // 接收到打洞命令，向指定的IP地址打洞
                printf("Recv p2someonewanttocallyou data\n");
                __android_log_print(ANDROID_LOG_ERROR, "P2P",
                                    "Recv p2someonewanttocallyou data 收到打洞消息\n");


                struct sockaddr_in remote;
                remote.sin_addr.s_addr = htonl(recvbuf.iStringLen);
                remote.sin_family = AF_INET;
                remote.sin_port = htons(recvbuf.Port);

                // UDP hole punching
                stP2PMessage message;
                message.iMessageType = P2PTRASH;
                sendto(PrimaryUDP, (const char *) &message, sizeof(message), 0,
                       (struct sockaddr *) &remote, sizeof(remote));
                break;
            }
            case P2PMESSAGEACK: {
                // 发送消息的应答
                RecvedACK = 1;
                break;
            }
            case P2PTRASH: {
                // 对方发送的打洞消息，忽略掉。
                //do nothing ...
                printf("Recv p2ptrash data\n");
                __android_log_print(ANDROID_LOG_ERROR, "P2P","Recv p2ptrash data 对方发送的打洞消息\n");
                break;
            }
            case GETALLUSER: {
                int usercount;
                int fromlen = sizeof(remote);
                int iread = recvfrom(PrimaryUDP, (char *) &usercount, sizeof(int), 0,
                                     (struct sockaddr *) &remote, &fromlen);
                if (iread <= 0) {
                    perror("recv error");
                    exit(1);
                }

                printf("Have %d users logined server:\n", usercount);
                __android_log_print(ANDROID_LOG_ERROR, "P2P",
                                    "***** Have %d users logined server:\n", usercount);


                for (i = 0; i < usercount; i++) {
                    stUserListNode *node = (stUserListNode *) malloc(sizeof(stUserListNode));
                    if (!node) {
                        printf("No Memory to malloc !");
                        exit(0);
                    }
                    recvfrom(PrimaryUDP, (char *) node, sizeof(stUserListNode), 0,
                             (struct sockaddr *) &remote, &fromlen);

                    insertNode(node);

                    printf("Username : %s\n", node->userName);
                    __android_log_print(ANDROID_LOG_ERROR, "P2P", "***** Username : %s\n",
                                        node->userName);
                    struct in_addr tmp;
                    tmp.s_addr = htonl(node->ip);
                    printf("UserIP : %s\n", inet_ntoa(tmp));
                    __android_log_print(ANDROID_LOG_ERROR, "P2P", "***** UserIP : %s\n",
                                        inet_ntoa(tmp));
                    printf("UserPort : %d\n", node->port);
                    __android_log_print(ANDROID_LOG_ERROR, "P2P", "***** UserPort : %d\n",
                                        node->port);
                }
                break;
            }
            case ONE_LOGIN: {
                int fromlen = sizeof(remote);
                stUserListNode *node = (stUserListNode *) malloc(sizeof(stUserListNode));
                if (!node) {
                    printf("No Memory to malloc !");
                    exit(0);
                }
                recvfrom(PrimaryUDP, (char *) node, sizeof(stUserListNode), 0,
                         (struct sockaddr *) &remote, &fromlen);
                //把这个刚上线的用户插入进链表中！
                insertNode(node);
                printf("The new client login is :\n");
                printf("Username : %s\n", node->userName);
                struct in_addr tmp;
                tmp.s_addr = htonl(node->ip);
                printf("UserIP : %s\n", inet_ntoa(tmp));
                printf("UserPort : %d\n", node->port);

                break;
            }
            case ONE_LOGOUT: {
                int fromlen = sizeof(remote);
                stUserListNode *node = (stUserListNode *) malloc(sizeof(stUserListNode));
                if (!node) {
                    printf("No Memory to malloc !");
                    exit(0);
                }
                recvfrom(PrimaryUDP, (char *) node, sizeof(stUserListNode), 0,
                         (struct sockaddr *) &remote, &fromlen);
                //把这个刚离线的用户从链表中删除！
                removeNode(node);
                printf("The client logout is :\n");
                printf("Username : %s\n", node->userName);
                struct in_addr tmp;
                tmp.s_addr = htonl(node->ip);
                printf("UserIP : %s\n", inet_ntoa(tmp));
                printf("UserPort : %d\n", node->port);

                break;
            }
            case TICKONLINEBAK: {
                //在这里受到服务端发送回来的消息，所以就表示还在线，网络没有除问题，这个是更新logintime 到目前的时间
                printf("The KONLINEBAK from Server ! ");
                time(&logintime);
                printf("the update time is :%d\n", logintime);
                break;
            }
        }
    }
}


void llog(const char *lsk) {
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "");
}
//        int printf(const char *__restrict, ...)
//        __printflike(1, 2);



void OutputUsage() {
//    printf("You can input you command:\n");
//    printf("Command Type:\"send\",\"exit\",\"getu\"\n");
//    printf("Example : send Username Message\n");
//    printf("          exit\n");
//    printf("          getu\n");
    __android_log_print(ANDROID_LOG_ERROR, "P2P", " \n");
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "||====================================\n");
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "||  You can input you command:\n");
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "||  Command Type:\"send\",\"exit\",\"getu\"\n");
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "||  Example : send Username Message\n");
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "||            exit\n");
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "||            getu\n");
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "||====================================\n\n ");
}


// 方法入口， 可以封装成初始化方法。
int start(const char *serverIP, const char *userName) {

    __android_log_print(ANDROID_LOG_ERROR, "P2P", "开始start方法");


    pthread_t thread_id[2];
    int ret[2];

    head = creat_UserList();

    PrimaryUDP = mksock(SOCK_DGRAM);
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "========= 游标 11111 ==========");
    BindSock(PrimaryUDP);
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "========= 游标 22222 ==========");





    //printf("Please input server ip:\n");
    //scanf("%s",ServerIP) ;
    //printf("Please input your name:\n");
    //scanf("%s",UserName) ;




//    获取 ip地址 和 用户名
    strcpy(ServerIP, serverIP);
    strcpy(UserName, userName);
//    printf("the ServerIP is :%s\n", ServerIP);
//    printf("the UserName is :%s\n", UserName);
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "***** The serverIP is :%s\n", ServerIP);
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "***** The userName is :%s\n", UserName);

    ConnectToServer(PrimaryUDP, UserName, ServerIP);


    ret[0] = pthread_create(&thread_id[0], NULL, (void *) TickThreadProc, NULL);
    if (ret[0] != 0) {
        perror("TickThreadProc Thread creation failed");
        exit(1);
    }

    __android_log_print(ANDROID_LOG_ERROR, "P2P", "========= 游标 33333 ==========");


    ret[1] = pthread_create(&thread_id[1], NULL, (void *) RecvThreadProc, NULL);
    if (ret[1] != 0) {
        perror("RecvThreadProc Thread creation failed");
        exit(1);
    }
    OutputUsage();
    __android_log_print(ANDROID_LOG_ERROR, "P2P", "========= 游标 44444 ==========");






    // 接受输入的
//    for (;;) {
//        char Command[COMMANDMAXC];
//        gets(Command);
//        ParseCommand(Command);
//    }



//    pthread_join(thread_id[0], NULL);
//    __android_log_print(ANDROID_LOG_ERROR, "P2P", "========= 游标 55555 ==========");
//    pthread_join(thread_id[1], NULL);
//    __android_log_print(ANDROID_LOG_ERROR, "P2P", "结束 start方法");

    return 0;
}











//  原有的入口函数
//
//int main(int argc, char *argv[]) {
//
//    pthread_t thread_id[2];
//    int ret[2];
//
//    head = creat_UserList();
//
//    PrimaryUDP = mksock(SOCK_DGRAM);
//    BindSock(PrimaryUDP);
//
//    //printf("Please input server ip:\n");
//    //scanf("%s",ServerIP) ;
//
//    //printf("Please input your name:\n");
//    //scanf("%s",UserName) ;
//    strcpy(ServerIP, argv[1]);
//    strcpy(UserName, argv[2]);
//    printf("the ServerIP is :%s\n", ServerIP);
//    printf("the UserName is :%s\n", UserName);
//    ConnectToServer(PrimaryUDP, UserName, ServerIP);
//
//    ret[0] = pthread_create(&thread_id[0], NULL, (void *) TickThreadProc, NULL);
//    if (ret[0] != 0) {
//        perror("TickThreadProc Thread creation failed");
//        exit(1);
//    }
//    ret[1] = pthread_create(&thread_id[1], NULL, (void *) RecvThreadProc, NULL);
//    if (ret[1] != 0) {
//        perror("RecvThreadProc Thread creation failed");
//        exit(1);
//    }
//    OutputUsage();
//
//    for (;;) {
//        char Command[COMMANDMAXC];
//        gets(Command);
//        ParseCommand(Command);
//    }
//
//
//    pthread_join(thread_id[0], NULL);
//    pthread_join(thread_id[1], NULL);
//
//    return 0;
//}







