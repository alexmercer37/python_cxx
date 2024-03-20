#ifndef __TCP_H_
#define __TCP_H_
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/prctl.h>
#include <pthread.h>
class Addr
{
public:
    Addr(char *ip, int port);
    const char *getIP();
    int getPort();

private:
    int port;
    const char *ip;
};

class TCP
{
public:
    int connetToServe(Addr &addr, int &clientfd);
    int Init_Socket(Addr &addr);
    int Get_Socket(Addr &addr);
    int lisen(int &listenfd);

private:
};

class TCP_Manage : public TCP
{
public:
    int Accept(int &listenfd);
    int TCP_manage(int &clientfd);
    int Recv(int &socketNum, void *RECV_DATA, size_t size);
    int Send(int clientfd, void *SEND_DATA, size_t size);
    void Close(int &listenfd);

private:
};

#endif
