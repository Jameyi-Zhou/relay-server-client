/*Buffer类只负责存放数据，不负责socket的收发数据*/

#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>

#define TEMP_BUFFER_SIZE 4096

class Buffer{ //存放用户收发的完整的数据，可以单独定义为接收缓冲区或者发送缓冲区
private:
    int total_len; //需要缓冲数据的长度
    int cur_addlen; //当前已经收到的数据长度
    int cur_fetchlen; //当前已经发送的数据长度
    std::string data; //收发的数据
public:
    Buffer();
    ~Buffer(){}
    void activate(int length); //激活该buffer，表示有数据需要存放，length是需要存储的数据长度
    void reset(); //清空缓冲区并重置缓冲区的信息
    void addData(std::string s); //存入一次可能并不完整的数据到buffer中
    std::string fetchData(int len); //从buffer中取出一次可能并不完整的数据
    void setCurFetchLen(int offset_len); //设置当前已取出的数据长度，需要handler来设置

    int dataNotAddCount(){ //还没有存到buffer的数据的长度
        return total_len - cur_addlen;
    }
    int dataNotFetchCount(){ //还没有从buffer中取出的数据的长度
        return total_len - cur_fetchlen;
    }
    bool finishAdd(){ //是否将全部的数据存进到了buffer中
        return total_len == cur_addlen;
    }
    bool finishFetch(){ //是否从buffer中取出了全部的数据
        return total_len == cur_fetchlen;
    }
    int print(){ //调试信息
        std::cout << "total length: " << total_len << std::endl;
        std::cout << "cur_addlen: " << cur_addlen << std::endl;
        std::cout << "cur_fetchlen: " << cur_fetchlen << std::endl;
    }
};

#endif