#include "client.h"

void setNonblocking(int sockfd){
	int opts = fcntl(sockfd, F_GETFL);
    if (opts < 0){
        std::cout << "fcntl failed " << strerror(errno) << std::endl;
        return;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, opts) < 0)
        std::cout << "fcntl failed " << strerror(errno) << std::endl;
}

EpollController::EpollController(){
	epollfd = -1;
	bzero(&event_active, sizeof(event_active));
}

bool EpollController::create(){
	if( (epollfd = epoll_create(MAXFD)) < 0){
		std::cout << "create epollfd error\n";
        return 0;
    }
	return 1;
}

bool EpollController::destroy(){
	close(epollfd);
}

bool EpollController::addSocketfd(int socketfd){
	epoll_event ev;
    ev.data.fd = socketfd;
    ev.events = EPOLLIN;
	setNonblocking(socketfd);
    if( epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &ev) < 0)
		return 0;
	return 1;
}

bool EpollController::delSocketfd(int socektfd){
	if( epoll_ctl(epollfd, EPOLL_CTL_DEL, socektfd, NULL) < 0)
		return 0;
	return 1;
}

bool EpollController::addfdOut(int socketfd){
	epoll_event ev;
    ev.data.fd = socketfd;
    ev.events = EPOLLIN | EPOLLOUT;
    if( epoll_ctl(epollfd, EPOLL_CTL_MOD, socketfd, &ev) < 0)
		return 0;
	return 1;
}

bool EpollController::delfdOut(int socketfd){
	epoll_event ev;
    ev.data.fd = socketfd;
    ev.events = EPOLLIN;
    if( epoll_ctl(epollfd, EPOLL_CTL_MOD, socketfd, &ev) < 0)
		return 0;
	return 1;
}

int EpollController::waitEvents(){
	int n = epoll_wait(epollfd, event_active, CLIENT_NUM, -1);
	return n;
}