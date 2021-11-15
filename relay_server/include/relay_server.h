#ifndef SERVER_H
#define SERVER_H

#include "myheader.h"
#include "server_IOhandler.h"

#define SERV_PORT 10101
#define MAX_HANDLER_COUNT 8192
#define MAXFD 8192

class EpollController{
private:
    int epollfd;
public:
    epoll_event event_active[MAXFD];
    EpollController();
    ~EpollController(){}
    bool create();
    bool destroy();
    bool addSocketfd(int socketfd);
    bool delSocketfd(int socketfd);
    bool addfdOut(int socketfd);
    bool delfdOut(int socketfd);
    int waitEvents();
};

class RelayServer{
private:
    sockaddr_in servaddr, cliaddr[MAXFD];
    EpollController epoller;
    ServerIOHandler sih[MAX_HANDLER_COUNT];
    int idtofd_table[MAXFD], fdtoid_table[MAXFD];
    int listenfd; 
    int fd_count;
public:
    RelayServer();
    ~RelayServer();
    bool listenToClients();
    bool clientLink();
    bool clientUnlink(int disconnfd);
    int getReceiverfd(int senderfd);
    int dealRecv(int senderfd);
    int dealSend(int receiverfd);
    int handleEvents();
};

#endif