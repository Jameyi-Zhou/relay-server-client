#include "server_events.h"

using namespace std;

RelayServer::RelayServer(){
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	listenfd = -1;
	ev = nullptr;
	cliaddr = nullptr;
	first_unused_ID = 0;
	//初始化epoll
	if( (epollfd = epoll_create(MAXFD)) < 0)
		cout << "create epollfd error!";
	//初始化登录表
	for(int i = 0; i < MAXFD; ++i)
		idtofd_table[i] = fdtoid_table[i] =  -1;	
}

RelayServer::~RelayServer(){
	delete cliaddr, ev;
	close(listenfd);
	close(epollfd);
}

void RelayServer::listenToClients(){
	if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		cout << "create listenfd socket error!";

	if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) 
		cout << "bind listenfd erorr!";
	
	if(listen(listenfd, 64) < 0)
		cout << "listen erorr!";

	//在内核事件中注册listenfd
	addInputfdtoEpoll(listenfd);

	cout << "start the server sucessfully! wait for users to login: \n";
}

void RelayServer::clientLink() { //获取一个客户端连接，将其connfd加入epollfd的监听中，修改用户登录表
	cliaddr = new sockaddr_in;
	socklen_t len = sizeof(sockaddr_in);
	
	int connfd;
	if( (connfd = accept(listenfd, (struct sockaddr *)cliaddr, &len)) < 0)
		cout << "accept error!";

	//在内核事件中注册connfd
	addInputfdtoEpoll(connfd);

	//修改登录表
	idtofd_table[first_unused_ID] = connfd;
	fdtoid_table[connfd] = first_unused_ID;
	int next_unused_ID  = first_unused_ID + 1;
	for(; idtofd_table[next_unused_ID] != -1; ++next_unused_ID);
	first_unused_ID = next_unused_ID;

    cout << "user " << fdtoid_table[connfd] <<  " connect successfully!\n";
}

void RelayServer::clientUnlink(int disconnfd) { //断开与fd对应的客户的连接
	if(epoll_ctl(epollfd, EPOLL_CTL_DEL, disconnfd, NULL) < 0)
		cout << "delete disconnfd from epoll erorr!";
	close(disconnfd);

	//修改登录表
	int duser_ID = fdtoid_table[disconnfd];
	fdtoid_table[disconnfd] = -1;
	idtofd_table[duser_ID] = -1;
	if(first_unused_ID > duser_ID)
		first_unused_ID = duser_ID;

	cout << "user " << duser_ID << " disconnect successfully!\n";
}

void RelayServer::addInputfdtoEpoll(int inputfd){
	ev = new epoll_event;
    (*ev).data.fd = inputfd;
    (*ev).events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, inputfd, ev);
}

void RelayServer::dealClientData(int senderfd) { //处理客户端发送过来的数据，并转发给另外一客户端
	ServerDataHandler s_data_handler;
	int res = s_data_handler.getRawdata(senderfd);
	if(res == 0){
		clientUnlink(senderfd);
		return;
	}
	else if(res == 24){
		std::string s = std::to_string(fdtoid_table[senderfd]);
	}
	/*
	s_data_handler.unpackData();
	int receiverfd = idtofd_table[s_data_handler.getReceiverID()];
	s_data_handler.packData();
	s_data_handler.sendPackedData(receiverfd);
	*/
}

bool RelayServer::handleEvents(){
	int n = epoll_wait(epollfd, events, MAXFD, -1);
	if(n < 0){
		cout << "epoll error\n";
		return 0;
	}
	for(int i = 0; i < n; ++i) {
		int fd = events[i].data.fd;
		if(fd == listenfd) 
			clientLink();
		else if(events[i].events & EPOLLHUP)
			clientUnlink(fd);
		else if(events[i].events & EPOLLIN)
			dealClientData(fd);
		else
			cout << "fd error\n";
	}
	return 1;
}



