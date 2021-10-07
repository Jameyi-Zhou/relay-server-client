#ifndef SERVER_HEADER_H
#define SERVER_HEADER_H

#include <stdio.h>
#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#define MAXFD 8192
#define SERV_PORT 10101
#define BUFFER_SIZE 4096


class RelayServer{
public:
    struct sockaddr_in servaddr, *cliaddr;
    struct epoll_event *ev, events[MAXFD];
    int idtofd_table[MAXFD], fdtoid_table[MAXFD];
    int first_unused_ID = 0;
    int listenfd, epollfd;
    RelayServer();
    void listenToClients();
    void clientLink();
    void clientUnlink(int disconnfd);
    void recvClientData(int recvfd);
    void sendClientData(int sendfd);
    bool handleEvents();
};

#endif