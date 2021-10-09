#include "server_data_handle.h"

using namespace std;

ServerDataHandler::ServerDataHandler(){
    rawdata = packeddata = processeddata.content = "";
    processeddata.head.receiver_userID = processeddata.head.sender_userID \
      = processeddata.head.data_len = -1;
}

int ServerDataHandler::getRawdata(int senderfd){
    char buff[BUFFER_SIZE] = {0};
	int res = read(senderfd, buff, BUFFER_SIZE);
    rawdata = buff;
    //cout << rawdata.length() << endl;
    return res;
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