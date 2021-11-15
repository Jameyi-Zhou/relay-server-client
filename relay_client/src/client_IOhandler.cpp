#include "myheader.h"
#include "client_IOhandler.h"
#include "buffer.h"

ClientIOHandler::ClientIOHandler(){
    sockfd = -1;
    send_data_len = recv_data_len = 0;
    recvstatus = _recv_idle;
    sendstatus = _send_idle;
}

void ClientIOHandler::getData(std::string s){ //获取用户的数据
    send_data_len = s.length();
    buffer_send.activate(send_data_len);
    buffer_send.addData(s);
}

int ClientIOHandler::sendSocketData(){
    if(sendstatus == _send_idle){ //该处理器还处于发送空闲状态，将发送携带了数据长度的报文
        send(sockfd, (char*) &send_data_len, sizeof(int), 0);
        sendstatus = _sending;
    }
    //开始发送真正的数据
    char tempdata[TEMP_BUFFER_SIZE] = {0};
    std::string s;
    while(!buffer_send.finishFetch()){ //判断是否从buffer中取完数据
        int _remain_len = buffer_send.dataNotFetchCount(); //buffer中剩余的字节数
        int _len = _remain_len > TEMP_BUFFER_SIZE ? TEMP_BUFFER_SIZE : _remain_len;    
        s = buffer_send.fetchData(_len); //取出数据
        for(int i = 0; i < s.length(); ++i)
            tempdata[i] = s[i];
        int _real_len = send(sockfd, tempdata, _len, 0);
        if( _real_len != -1){
            buffer_send.setCurFetchLen(_real_len - _len); //可能回退
            bzero(tempdata, TEMP_BUFFER_SIZE);
        }
        else if(_real_len == 0) //???未知情况
            return 0;
        else if(errno == EAGAIN) //socket发送缓冲区已写满，但是还有数据没有发送完，非阻塞返回-1
            return -1;
        else //数据发送出错
            return -2;
    }
    //发送数据完毕，置处理器为发送空闲状态
    sendstatus = _send_idle; 
    buffer_send.reset();
    //std::cout << send_data_len << " bytes ";
    int ret = send_data_len;
    send_data_len = 0;

    return ret;
}

int ClientIOHandler::recvSocketData(){
    /*未仔细考虑服务器断开的情况*/
    if(recvstatus == _recv_idle){ //该处理器还处于接收空闲状态，将接收携带了数据长度的报文
        int *temp = nullptr;
        char dataheader[sizeof(int)] = {0};
        ///???
        if( recv(sockfd, dataheader, sizeof(int), 0) == 0) 
            return 0; //服务器断开连接
        ///???
        temp = (int*) &dataheader;
        recv_data_len = *temp;
        buffer_recv.activate(recv_data_len);
        recvstatus = _recving;
    }
    //开始发送真正的数据
    char tempdata[TEMP_BUFFER_SIZE] = {0};
    std::string s;
    while(!buffer_recv.finishAdd()){ //判断是否存完数据到buffer
        int _remain_len = buffer_recv.dataNotAddCount(); //还没有存到buffer的数据长度
        int _len = _remain_len > TEMP_BUFFER_SIZE ? TEMP_BUFFER_SIZE : _remain_len;    
        int _real_len = recv(sockfd, tempdata, _len, 0); //_real_len真实收到的数据长度
        if(_real_len != -1){
            buffer_recv.addData(tempdata);
            bzero(tempdata, TEMP_BUFFER_SIZE);
        }
        else if(_real_len == 0)
            return 0;
        else if(errno == EAGAIN) //socket接收缓冲区为空但是数据还没有收完，非阻塞返回-1
            return -1;
        else //数据接收出错
            return -2;
    }
    //接收数据完毕，置处理器为接收空闲状态，这里可以处理数据，但是该业务直接清空接收缓冲区就可以了
    recvstatus = _recv_idle; 
    buffer_recv.reset();
    //std::cout << recv_data_len << " bytes ";
    int ret = recv_data_len;
    recv_data_len = 0;

    return ret;
}
