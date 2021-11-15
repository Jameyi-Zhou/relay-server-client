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
    epoll_event events[MAXFD]; 
public:
    bool epollAddSocketfd(int socketfd);
    bool epollDelSocketfd(int socketfd);
    bool epollAddfdOut(int socketfd);
    bool epollDelfdOut(int socketfd);
};

class RelayServer{
private:
    sockaddr_in servaddr, cliaddr[MAXFD];
    //
    int epollfd;
    epoll_event events[MAXFD]; 
    //
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

    //
    bool epollAddSocketfd(int socketfd);
    bool epollDelSocketfd(int socketfd);
    bool epollAddfdOut(int socketfd);
    bool epollDelfdOut(int socketfd);
    //
};

#endif