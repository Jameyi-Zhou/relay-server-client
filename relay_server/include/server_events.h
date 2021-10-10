#ifndef SERVER_EVENTS_H
#define SERVER_EVENTS_H


#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "server_data_handle.h"

#define MAXFD 65536
#define SERV_PORT 10101


class RelayServer{
private:
    struct sockaddr_in servaddr, *cliaddr;
    struct epoll_event events[MAXFD], *ev; //*ev 和 *cliaddr都是之后动态分配的，每连接一个用户new一个
    int idtofd_table[MAXFD], fdtoid_table[MAXFD], first_unused_ID; //该行定义的变量记录用户的登录信息
    int listenfd, epollfd; 
public:
    RelayServer();
    ~RelayServer();
    void listenToClients();
    void clientLink();
    void clientUnlink(int disconnfd);
    void addInputfdtoEpoll(int fd);
    void dealClientData(int senderfd);
    bool handleEvents();
};

#endif