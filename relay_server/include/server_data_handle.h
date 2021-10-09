#ifndef S_D_H
#define S_D_H

#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "data_process.h"

#define BUFFER_SIZE 4096

class ServerDataHandler: DataHandler{
private:
    std::string rawdata, packeddata;
    data_info processeddata;
public:
    ServerDataHandler();
    ~ServerDataHandler(){}
    int getRawdata(int senderfd);
    void unpackData();
    int getReceiverID();
    void packData();
    void sendPackedData(int receiverfd);
};

#endif