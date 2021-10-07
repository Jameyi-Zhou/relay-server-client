#ifndef CLIENT_HEADER
#define CLIENT_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "myerr.h"
#include "apue.h"

#define SERV_PORT 10101
#define BUFFER_SIZE 4096
#define MAXFD 16

class Client{
public:
    struct sockaddr_in servaddr;
    struct epoll_event *ev, events[MAXFD];
    int epollfd;
    Client(char *addrip);
    void connectToServer();
    void handleEvents();
};

#endif