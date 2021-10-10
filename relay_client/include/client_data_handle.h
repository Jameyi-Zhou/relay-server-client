#ifndef C_D_H
#define C_D_H

#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "data_process.h"

#define BUFFER_SIZE 4096

class ClientDataHandler: DataHandler{
private:
    bool first_msg;
    int my_userID, receiver_userID;
    std::string inputdata, packeddata; //加上头部的数据
    std::string rawdata;
    data_info processeddata;
public:
    ClientDataHandler();
    ~ClientDataHandler(){}
    int getMyID();
    void setSessionID(int receiver_userID);
    int getInputData(int inputfd);
    void packData();
    void sendPackedData(int socketfd);
    int getRawdata(int socketfd);
    void unpackData();
};

#endif