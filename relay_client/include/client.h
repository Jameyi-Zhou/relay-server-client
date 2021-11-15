/*客户端事件处理*/

#ifndef CLIENT_H
#define CLIENT_H

#include "myheader.h"
#include "client_IOhandler.h"

#define SERV_PORT 10101
#define MAXFD 8192

#define CLIENT_NUM 100
#define RELAY_COUNT 10000
#define TEST_DATA_LEN 1024

class EpollController{
private:
    int epollfd;
public:
    epoll_event event_active[CLIENT_NUM];
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

class Client{
private:
    sockaddr_in servaddr; //服务器套接字地址
    EpollController epoller;
    ClientIOHandler cih[CLIENT_NUM]; //客户端的IO处理器
    int fdtohandler[MAXFD]; //存储sockfd到处理器号的映射
    int sockfd[CLIENT_NUM]; //客户端的socketfd
    std::string data; //测试数据
public:
    Client();
    ~Client();
    bool init(char *addrip);
    bool connecttoServer();
  
    int dealSend(int sockfd);
    int dealRecv(int sockfd);
    bool handleEvents();

    void test();
};

#endif