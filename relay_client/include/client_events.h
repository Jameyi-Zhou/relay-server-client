#ifndef CLIENT_EVENTS_H
#define CLIENT_EVENTS_H


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "client_data_handle.h"

#define SERV_PORT 10101
#define BUFFER_SIZE 4096
#define MAXFD 64

class Client{
private:
    int my_userID;
    struct sockaddr_in servaddr;
    struct epoll_event *ev, events[MAXFD];
    int sockfd, epollfd;
public:
    ClientDataHandler c_data_handler;
    Client();
    ~Client(){}
    void connecttoServer(char *addrip);
    void addInputfdtoEpoll(int fd);
    void sendData(int inputfd);
    
    void recvData(); 
    void handleEvents();
};

#endif