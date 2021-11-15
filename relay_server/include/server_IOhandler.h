#ifndef S_IO_H
#define S_IO_H

#include <iostream>
#include "buffer.h"

enum SectorRecvStatus //sector的recv状态
{
    _recving, _recv_idle
};

enum SectorSendStatus //sector的send状态
{
    _sending, _send_idle
};

struct HandlerSector{ //存放单个客户端发送来的数据以及状态
    int connfd; //该客户的已连接描述符

    int send_data_len; //发送缓冲区的数据长度
    SectorSendStatus sendstatus; //发送状态
    Buffer buffer_send; //发送缓冲区
    
    int recv_data_len; //接收缓冲区的数据长度
    SectorRecvStatus recvstatus; //接收状态
    Buffer buffer_recv; //接收缓冲区
};

class ServerIOHandler{
private:
    //两个对端客户各自的数据区域
    HandlerSector sector[2];
public:
    ServerIOHandler();
    ~ServerIOHandler(){}
    int getData(int senderfd); //从senderfd获取数据，并将数据存放到对应sector的buffer_recv中
    int sendData(int receiverfd); //从对应的sector的buffer_send中取出数据，将数据数据到receiverfd
    
    void setConnfd0(int fd){ //设置sector[0]的connfd
        sector[0].connfd = fd;
    }
    void setConnfd1(int fd){ //设置sector[1]的connfd
        sector[1].connfd = fd;
    }
};

#endif