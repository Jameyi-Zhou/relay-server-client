#include "client_header.h"

Client::Client(char *addrip){
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    epollfd = epoll_create(MAXFD);
    inet_pton(AF_INET, addrip, &servaddr.sin_addr);
}

void Client::connectToServer(){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        err_sys("connect error");
    ev = new epoll_event;
    (*ev).data.fd = sockfd;
    (*ev).events = EPOLLIN | EPOLLOUT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, ev);
}

void Client::sendDataTo(int userID){
    char buff[BUFFER_SIZE] = {0};
    write(sockfd, buff, BUFFER_SIZE);
}

void Client::recvDataFrom(int & userID){
    char buff[BUFFER_SIZE] = {0};
    read(sockfd, buff, BUFFER_SIZE);
    
}
void Client::handleEvents(){

}

int main(int argc, char **argv){
    char *addrip;
    char *s = "127.0.0.1";
    if(argc == 1)
        addrip = s;
    else
        addrip = argv[1];
    Client cli(addrip);

    while(1) {
        cli.handleEvents();
        //read(sockfd, buff2, BUFFER_SIZE);
        //fputs(buff2, stdout);
    }
}