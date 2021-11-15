/*客户端事件处理*/

#ifndef CLIENT_H
#define CLIENT_H

#include "myheader.h"
#include "client_IOhandler.h"

#define SERV_PORT 10101
#define MAXFD 8192

#define CLIENT_NUM 2000
#define RELAY_COUNT 10000
#define TEST_DATA_LEN 10240

class EpollController{
private:
    int epollfd;
    epoll_event events[CLIENT_NUM];
public:
    bool epollAddSocketfd(int socketfd);
    bool epollDelSocketfd(int socketfd);
    bool epollAddfdOut(int socketfd);
    bool epollDelfdOut(int socketfd);
};

class Client{
private:
    sockaddr_in servaddr; //服务器套接字地址
    //
    int epollfd;
    epoll_event events[CLIENT_NUM];
    //
    ClientIOHandler cih[CLIENT_NUM]; //客户端的IO处理器
    int fdtohandler[MAXFD]; //存储sockfd到处理器号的映射
    int sockfd[CLIENT_NUM]; //客户端的socketfd
    std::string data; //测试数据
public:
    Client();
    ~Client();
    bool init(char *addrip);
    bool connecttoServer();
    //
    bool epollAddSocketfd(int socketfd);
    bool epollDelSocketfd(int socketfd);
    bool epollAddfdOut(int socketfd);
    bool epollDelfdOut(int socketfd);
    //
    int dealSend(int sockfd);
    int dealRecv(int sockfd);
    bool handleEvents();

    void test();
};

#endif