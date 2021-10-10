#include "client_events.h"


Client::Client(){
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    sockfd = my_userID = -1;
    epollfd = epoll_create(MAXFD);

    //将标准输入事件添加到epoll中
    addInputfdtoEpoll(STDIN_FILENO); 
}

void Client::connecttoServer(char *addrip){
    
    inet_pton(AF_INET, addrip, &servaddr.sin_addr);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        std::cout << "connect error";
    
    c_data_handler.packData();
    c_data_handler.sendPackedData(sockfd);
    c_data_handler.getRawdata(sockfd);
    my_userID = c_data_handler.getMyID();
    std::cout << my_userID << std::endl;
    //将套接字描述符加入到epoll中
    addInputfdtoEpoll(sockfd);
}

void Client::addInputfdtoEpoll(int inputfd){ //添加某个输入事件到epoll中去
    ev = new epoll_event;
    (*ev).data.fd = inputfd;
    (*ev).events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, inputfd, ev);
}

void Client::sendData(int inputfd){
    c_data_handler.getInputData(inputfd);
    c_data_handler.packData();
    c_data_handler.sendPackedData(sockfd);
}

void Client::recvData(){
    
}


void Client::handleEvents(){
    int n = epoll_wait(epollfd, events, MAXFD, -1);
	if(n < 0)
		printf("epoll error\n");
    int inputfd = STDIN_FILENO;
    for(int i = 0; i < n; ++i){
        int fd = events[i].data.fd;
        if(fd != sockfd)
            sendData(inputfd);
        else if(events[i].events & EPOLLIN)
            recvData();
        else
            std::cout << "fd erorr\n";
    }
}

