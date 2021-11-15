#include "relay_server.h"

RelayServer::RelayServer(){
	bzero(&servaddr, sizeof(servaddr));
	bzero(&cliaddr, sizeof(cliaddr));
	for(int i = 0; i < MAXFD; ++i)
		idtofd_table[i] = fdtoid_table[i] = -1;
	fd_count = 0;
	listenfd = -1;
}

RelayServer::~RelayServer(){
	close(listenfd);
}

bool RelayServer::listenToClients(){
	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	epoller.create();
	if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ //创建监听描述符
		std::cout << "create listenfd socket error\n";
		return 0;
	}
	if( bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){ //绑定监听套接字
		std::cout << "bind listenfd erorr\n";
		return 0;
	}
	if( listen(listenfd, 64) < 0){ //开始监听
		std::cout << "listen erorr\n";
		return 0;
	}
	if( !epoller.addSocketfd(listenfd)){ //在epoll中注册listenfd
		std::cout << "add listenfd to epoll erorr\n";
		return 0;
	}
	std::cout << "start the server sucessfully! wait for users to login:\n";
	return 1;
}

bool RelayServer::clientLink() { //获取一个客户端连接，将其connfd加入epollfd的监听中
	socklen_t len = sizeof(sockaddr_in);
	int connfd;
	if( (connfd = accept(listenfd, (struct sockaddr *) &cliaddr[fd_count], &len)) < 0){
		std::cout << "accept error\n";
		return 0;
	}
	//在epoll中注册connfd
	if( !epoller.addSocketfd(connfd)){
		std::cout << "add connfd" << connfd << " to epoll erorr\n";
		return 0;
	}
	idtofd_table[fd_count] = connfd;
	fdtoid_table[connfd] = fd_count;
	//设置IO处理器号
	if(fd_count % 2 == 0)
		sih[fd_count / 2].setConnfd0(connfd);
	else
		sih[fd_count / 2].setConnfd1(connfd);
	
	std::cout << "user " << fd_count <<  " connect successfully\n";
	++fd_count; 

	return 1;
}

bool RelayServer::clientUnlink(int disconnfd) { //断开与fd对应的客户的连接
	if(!epoller.delSocketfd(disconnfd)){
		std::cout << "delete user " << fdtoid_table[disconnfd] << " from epoll erorr\n";
		return 0;
	}
	close(disconnfd);
	std::cout << "user " << fdtoid_table[disconnfd] << " disconnect successfully\n";
	--fd_count;
	return 1;
}

int RelayServer::getReceiverfd(int senderfd){
	int sid = fdtoid_table[senderfd], rid;
	if(sid % 2) //如果是奇数id，那么发给对应的偶数id客户
		rid = sid - 1;
	else  //如果是偶数id，那么发给对应的奇数id客户
		rid = sid + 1;
	return idtofd_table[rid];
}

int RelayServer::dealRecv(int senderfd){ //接收来自任意已连接客户的数据
	int handlerid = fdtoid_table[senderfd] / 2;
	//接收数据
	//std::cout << "receiving from user" << fdtoid_table[senderfd] << ": ";
	int ret = sih[handlerid].getData(senderfd);
	if(ret == 0){  //客户断开连接
		clientUnlink(senderfd);
		return 0;
	}
	else if(ret == -1){
		std::cout << "receiving data...\n";
		return -1;
	}
	else if(ret == -2){
		std::cout << "recv erorr\n";
		return -2;
	}
	else
		std::cout << "received\n"; 
	
	return ret;
}

int RelayServer::dealSend(int receiverfd) { //发送数据给匹配的另外一个客户
	int handlerid = fdtoid_table[receiverfd] / 2;
	//发送数据
	//std::cout << "sending data to user" << fdtoid_table[receiverfd] << ": ";
	int ret = sih[handlerid].sendData(receiverfd);
	if(ret == 0){
		//???未知情况
		return 0;
	}
	if(ret == -1){
		std::cout << "sending data...\n";
		return -1;
	}
	else if(ret == -2){
		std::cout << "send erorr\n";
		return -2;
	}
	else
		std::cout << "sended\n";

	return ret;
}

int RelayServer::handleEvents(){
	int n = epoller.waitEvents();
	if(n < 0){
		std::cout << "epoll error\n";
		return 0;
	}
	for(int i = 0; i < n; ++i) {
		int fd = epoller.event_active[i].data.fd;
		if(fd == listenfd) 
			clientLink();
		else if(epoller.event_active[i].events & EPOLLIN){
			if(dealRecv(fd) > 0){
				int recvfd = getReceiverfd(fd);
				if(dealSend(recvfd) == -1) //数据还未发送完但是发送缓冲区已满
					epoller.addfdOut(recvfd); //将发送缓冲区可写的事件添加到epoll中
			}
		}
		else if(epoller.event_active[i].events & EPOLLOUT){
			if(dealSend(fd) > 0)   //如果发送数据完毕
				epoller.delfdOut(fd); //将发送缓冲区可写事件移出epoll，否则会一直触发				
		}
		else{
			std::cout << "fd error\n";
			return 0;
		}
	}
	return 1;
}