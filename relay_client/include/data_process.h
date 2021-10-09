#ifndef DATA_PROCESS_H
#define DATA_PROCESS_H

#include <string>

struct data_header
{
    int sender_userID;
    int receiver_userID; 
    int data_len;
};

struct data_info
{
    data_header head;
    std::string content;
};

class DataHandler{
public:
    virtual int getRawdata(int) = 0;
    virtual void unpackData() = 0;
    virtual void packData() = 0;
    virtual void sendPackedData(int) = 0;
};

#endif