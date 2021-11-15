/*客户端IO处理器*/

#ifndef C_IO_H
#define C_IO_H

#include <iostream>
#include "buffer.h"

enum HandlerRecvStatus
{
    _recving, _recv_idle
};
enum HandlerSendStatus
{
    _sending, _send_idle
};

class ClientIOHandler{ //IO处理器，一个socket对应一个
private:
    int sockfd;
    /*用户的接收和发送缓冲区*/
    int send_data_len;  //待发送数据的总长度   
    HandlerSendStatus sendstatus;
    Buffer buffer_send; //存放发送的完整数据

    int recv_data_len;  //待接收的数据总长度
    HandlerRecvStatus recvstatus; 
    Buffer buffer_recv; //存放接收的完整数据
public:
    ClientIOHandler();
    ~ClientIOHandler(){}
    void getData(std::string s); //得到用户输入的数据
    int recvSocketData(); //得到socket发送的数据
    int sendSocketData(); //发送数据到socket
    void setSockfd(int fd){ //设置socketfd
        sockfd = fd;
    }
};

#endif