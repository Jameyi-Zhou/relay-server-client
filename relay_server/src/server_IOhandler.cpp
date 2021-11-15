#include "myheader.h"
#include "server_IOhandler.h"

ServerIOHandler::ServerIOHandler(){
    sector[0].connfd = sector[1].connfd = -1;
    sector[0].recv_data_len = sector[1].recv_data_len = \
    sector[0].send_data_len = sector[1].send_data_len = 0;
    sector[0].recvstatus = sector[1].recvstatus = _recv_idle;
    sector[0].sendstatus = sector[1].sendstatus = _send_idle;
}

int ServerIOHandler::getData(int senderfd){   
    int sno = senderfd == sector[0].connfd ? 0: 1; //senderfd对应的sector号，0或1
    /*未仔细考虑客户断开连接的情况*/
    if(sector[sno].recvstatus == _recv_idle){ //表明是第一个携带了数据长度的报文到达
        int *temp = nullptr;
        char dataheader[sizeof(int)] = {0};
        //////???
        if( recv(senderfd, dataheader, sizeof(int), 0) == 0) 
            return 0; //客户断开连接
        //////???
        temp = (int*) &dataheader; //temp指向的数据是本次应该接收的数据长度
        sector[sno].recv_data_len = *temp; 
        sector[sno].buffer_recv.activate(sector[sno].recv_data_len); //激活buffer_recv
        sector[sno].recvstatus = _recving;
    }

    //开始接收真正的数据
    char tempdata[TEMP_BUFFER_SIZE] = {0};
    while( !sector[sno].buffer_recv.finishAdd()){ //判断buffer是否存完数据
        int _remain_len = sector[sno].buffer_recv.dataNotAddCount(); //还没有存进buffer的数据长度
        int _len = _remain_len > TEMP_BUFFER_SIZE ? TEMP_BUFFER_SIZE : _remain_len; 
        int _real_len = recv(senderfd, tempdata, _len, 0); //_real_len是实际接收到的数据长度
        if(_real_len != -1){  
            sector[sno].buffer_recv.addData(tempdata);
            bzero(tempdata, TEMP_BUFFER_SIZE);
        }
        else if(_real_len == 0)
            return 0;
        else if(errno == EAGAIN) //表明还有数据未到达，由于是非阻塞，所以直接返回-1
            return -1;
        else //表明出错，返回-2
            return -2;  
    }
    //将数据copy到对端sector的发送缓冲区
    std::string s = sector[sno].buffer_recv.fetchData(sector[sno].recv_data_len);
    sector[1 - sno].send_data_len = sector[sno].recv_data_len;
    sector[1 - sno].buffer_send.activate(sector[sno].recv_data_len);
    sector[1 - sno].buffer_send.addData(s);
    //清空当前sector的接收缓冲区，切换接收状态为空闲
    sector[sno].buffer_recv.reset();
    sector[sno].recvstatus = _recv_idle;
    //std::cout << sector[sno].recv_data_len << " bytes ";
    int ret = sector[sno].recv_data_len; 
    sector[sno].recv_data_len = 0;

    return ret;
}

int ServerIOHandler::sendData(int receiverfd){
    int sno = sector[0].connfd == receiverfd ? 0: 1; //receiverfd对应的sector号，0或1
    if(sector[sno].sendstatus == _send_idle){ //表明是第一次发送数据，需要发送带数据长度的报文 
        send(receiverfd, (char*) &sector[sno].send_data_len, sizeof(int), 0);
        sector[sno].sendstatus = _sending;
    }
    //开始发送真正的数据
    char tempdata[TEMP_BUFFER_SIZE] = {0};
    std::string s;
    while(!sector[sno].buffer_send.finishFetch()){ //判断是否从buffer中取完数据
        int _remain_len = sector[sno].buffer_send.dataNotFetchCount(); //还没有从buffer取出的数据长度
        int _len = _remain_len > TEMP_BUFFER_SIZE ? TEMP_BUFFER_SIZE : _remain_len;    
        s = sector[sno].buffer_send.fetchData(_len); //取出数据
        for(int i = 0; i < s.length(); ++i)
            tempdata[i] = s[i];
        int _real_len = send(receiverfd, tempdata, _len, 0);
        if(_real_len != -1){
            //_real_len可能小于_len重新设置buffer内偏移
            sector[sno].buffer_send.setCurFetchLen(_real_len - _len); 
            bzero(tempdata, TEMP_BUFFER_SIZE);
        }
        else if(_real_len == 0) //???未知情况
            return 0;
        else if(errno == EAGAIN) //socket发送缓冲区已写满，但是还有数据没有发送完，非阻塞返回-1
            return -1;
        else //数据发送出错
            return -2;
    }
    sector[sno].buffer_send.reset(); //清空buffer_send
    sector[sno].sendstatus = _send_idle; //发送数据完毕，置为_send_idle状态
    //std::cout << sector[sno].send_data_len << " bytes ";
    int ret = sector[sno].send_data_len; 
    sector[sno].send_data_len = 0;

    return ret;
}