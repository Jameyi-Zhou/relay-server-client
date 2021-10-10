#include "client_data_handle.h"

std::string intToString8(int x){
    std::string temp = std::to_string(x);
    for(int len = temp.length(); len < 8; ++len)
        temp = "#" + temp;
    return temp;
}

ClientDataHandler::ClientDataHandler(){
    first_msg = 1;
    rawdata = packeddata = processeddata.content = inputdata = "";
    my_userID = receiver_userID \
    = processeddata.head.sender_userID \
    = processeddata.head.receiver_userID  = -1;
    processeddata.head.data_len = 0;
}

int ClientDataHandler::getMyID(){
    return my_userID;
}

void ClientDataHandler::setSessionID(int receiver_userID){
    this->receiver_userID = receiver_userID;
}

int ClientDataHandler::getInputData(int inputfd){ //得到用户输入的数据
    char buff[BUFFER_SIZE] = {0};
    int res = read(inputfd, buff, BUFFER_SIZE);
    inputdata = buff;
    return res;
}

void ClientDataHandler::packData(){ //打包上头部
    std::string header;
    header = intToString8(my_userID)\
     + intToString8(receiver_userID) \
     + intToString8(inputdata.length());
    packeddata = header + inputdata;
    //std::cout << packeddata << std::endl;
}

void ClientDataHandler::sendPackedData(int socketfd){
    char buff[BUFFER_SIZE] = {0};
    for(int i = 0; i < packeddata.length(); ++i)
        buff[i] = packeddata[i];
    buff[packeddata.length()] = '\0';
    write(socketfd, buff, BUFFER_SIZE);
}

int ClientDataHandler::getRawdata(int socketfd){
    char buff[BUFFER_SIZE] = {0};
    int res = read(socketfd, buff, BUFFER_SIZE);
    rawdata = buff;
    if(first_msg){
        my_userID = std::atoi(rawdata.c_str());
        first_msg = 0;
    }
    return res;
}

void ClientDataHandler::unpackData(){ //unpack the rawdata
}

