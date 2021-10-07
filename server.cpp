#include "server_header.h"

RelayServer::RelayServer(){
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	epollfd = epoll_create(MAXFD);

	//初始化登录表
	for(int i = 0; i < MAXFD; ++i)
		idtofd_table[i] = fdtoid_table[i] =  -1;
}

void RelayServer::listenToClients(){
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); 
	
	//在内核事件中注册listenfd
	ev = new epoll_event;
    (*ev).data.fd = listenfd;
	(*ev).events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, ev); 
	listen(listenfd, 64);
}

void RelayServer::clientLink() { //获取一个客户端连接，将其connfd加入epollfd的监听中，修改用户登录表
	cliaddr = new sockaddr_in;
	socklen_t len = sizeof(sockaddr_in);
	int connfd = accept(listenfd, (struct sockaddr *)cliaddr, &len);

	//在内核事件中注册connfd
	ev = new epoll_event;
	(*ev).events = EPOLLIN | EPOLLOUT;
	(*ev).data.fd = connfd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, ev);

	//修改登录表
	idtofd_table[first_unused_ID] = connfd;
	fdtoid_table[connfd] = first_unused_ID;
	int next_unused_ID  = first_unused_ID + 1;
	for(; idtofd_table[next_unused_ID] != -1; ++next_unused_ID);
	first_unused_ID = next_unused_ID;

    printf("user %d connect successfully\n", fdtoid_table[connfd]);
}

void RelayServer::clientUnlink(int disconnfd) { //断开与fd对应的客户的连接
    close(disconnfd);
	epoll_ctl(epollfd, EPOLL_CTL_DEL, disconnfd, NULL);

	//修改登录表
	int duser_ID = fdtoid_table[disconnfd];
	fdtoid_table[disconnfd] = -1;
	idtofd_table[duser_ID] = -1;
	if(first_unused_ID > duser_ID)
		first_unused_ID = duser_ID;
	printf("user %d disconnect successfully\n", duser_ID);
}

void RelayServer::recvClientData(int recvfd) { //接收客户端数据
	char buff[BUFFER_SIZE] = {0};
	int res = read(recvfd, buff, BUFFER_SIZE);
    if(res == 0){
		clientUnlink(recvfd);
		return;
	}
	printf("user %d send the data: %s", fdtoid_table[recvfd], buff);
}

void RelayServer::sendClientData(int sendfd){ //发送数据到对端
	char buff[64] = "a client send you a message"; 
	
	write(sendfd, buff, 64);
}

bool RelayServer::handleEvents(){
	int n = epoll_wait(epollfd, events, MAXFD, -1);
	if(n < 0){
		printf("epoll error\n");
		return 0;
	}
	for(int i = 0; i < n; ++i) {
		int fd = events[i].data.fd;
		if(fd == listenfd) 
			clientLink();
		else if(events[i].events & EPOLLHUP)
			clientUnlink(fd);
		else if(events[i].events & EPOLLIN)
			recvClientData(fd);
		else if(events[i].events & EPOLLOUT)
			sendClientData(fd);
		else
			printf("fd error\n");
	}
	return 1;
}

int main(int argc, char **argv){ 
	RelayServer server1;
	server1.listenToClients();
    while(1){
		server1.handleEvents();
	}
	exit(0);
}