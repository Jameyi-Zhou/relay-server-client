#include "buffer.h"

Buffer::Buffer(){
    data = "";
    total_len = cur_addlen = cur_fetchlen = -1;
}

void Buffer::reset(){ 
    data = "";
    total_len = cur_addlen = cur_fetchlen = -1;
}

void Buffer::activate(int length){
    total_len = length;
    cur_addlen = cur_fetchlen = 0;
}

void Buffer::addData(std::string s){
    //直接设置cur_addlen，因为s是真实收到的数据
    cur_addlen += s.length();
    data += s;
}

std::string Buffer::fetchData(int len){ 
    //直接设置cur_fetchlen，但是可能会回退，需要handler自行处理
    std::string s = data.substr(cur_fetchlen, len);
    cur_fetchlen += len;
    return s;
}

void Buffer::setCurFetchLen(int offset_len){ //一般情况offset_len是负数，即回退
    cur_fetchlen += offset_len;
}