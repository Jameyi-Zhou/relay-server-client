#include "client_events.h"

using namespace std;

Client::Client(){
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    epollfd = epoll_create(MAXFD);

    //将标准输入事件添加到epoll中
    addInputfdtoEpoll(STDIN_FILENO); 
}

void Client::connecttoServer(char *addrip){
    
    inet_pton(AF_INET, addrip, &servaddr.sin_addr);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        cout << "connect error";
    
    //将套接字描述符加入到epoll中
    addInputfdtoEpoll(sockfd);
}

void Client::addInputfdtoEpoll(int inputfd){ //添加某个输入事件到epoll中去
    ev = new epoll_event;
    (*ev).data.fd = inputfd;
    (*ev).events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, inputfd, ev);
}

void Client::sendDatato(int inputfd){
    char buff[BUFFER_SIZE] = {0};
    int res = read(inputfd, buff, BUFFER_SIZE);
    write(sockfd, buff, BUFFER_SIZE);
}

void Client::recvDataFrom(){
    char buff[BUFFER_SIZE] = {0};
    read(sockfd, buff, BUFFER_SIZE);
    printf("%s", buff);
}

void Client::handleEvents(){
    int n = epoll_wait(epollfd, events, MAXFD, -1);
	if(n < 0)
		printf("epoll error\n");
    int inputfd = STDIN_FILENO;
    for(int i = 0; i < n; ++i){
        int fd = events[i].data.fd;
        if(fd != sockfd)
            sendDatato(inputfd);
        else if(events[i].events & EPOLLIN)
            recvDataFrom();
        else
            cout << "fd erorr\n";
    }
}

int main(int argc, char **argv){
    char *addrip;
    char s[] = "127.0.0.1";
    if(argc == 1)
        addrip = s;
    else
        addrip = argv[1];

    Client cli;
    cli.connecttoServer(addrip);
    while(1) {
        cli.handleEvents();
    }
}