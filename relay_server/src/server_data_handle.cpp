#include "server_data_handle.h"

std::string intToString8(int x){
    std::string temp = std::to_string(x);
    for(int len = temp.length(); len < 8; ++len)
        temp = "#" + temp;
    return temp;
}

ServerDataHandler::ServerDataHandler(){
    rawdata = packeddata = processeddata.content = "";
    processeddata.head.receiver_userID = processeddata.head.sender_userID \
      = processeddata.head.data_len = -1;
}

int ServerDataHandler::getRawdata(int senderfd){
    char buff[BUFFER_SIZE] = {0};
	int res = read(senderfd, buff, BUFFER_SIZE);
    rawdata = buff;
    return rawdata.length();
}

void ServerDataHandler::unpackData(){
  int x = 0;
} 

int ServerDataHandler::getReceiverID(){
    return processeddata.head.receiver_userID;
}

void ServerDataHandler::packData(){
    int x = 0;
}

void ServerDataHandler::sendPackedData(int receiverfd){
    //char *buff;

    //write()
    int x = 0;
}