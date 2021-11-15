#include "client.h"
#include <chrono>
using namespace std::chrono;

int relay_count = 0;
auto start = steady_clock::now();
auto end = steady_clock::now();

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

Client::Client(){
    bzero(&servaddr, sizeof(servaddr));
    bzero(&events, sizeof(events));
    for(int i = 0; i < CLIENT_NUM; ++i)
        sockfd[i] = -1;
    epollfd = -1;
    for(int i = 0; i < TEST_DATA_LEN; ++i)
        data += "a";
}

Client::~Client(){
    for(int i = 0; i < CLIENT_NUM; ++i)
        close(sockfd[i]);
    close(epollfd);
}

bool Client::init(char *addrip){
    servaddr.sin_family = AF_INET;
    if( inet_pton(AF_INET, addrip, &servaddr.sin_addr) <= 0){
        std::cout << "address erorr\n";
        return 0;
    }
    servaddr.sin_port = htons(SERV_PORT);

    if( (epollfd = epoll_create(MAXFD)) < 0){
		std::cout << "create epollfd error\n";
        return 0;
    }	
    for(int i = 0; i < CLIENT_NUM; ++i){
        if( (sockfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            std::cout << "create socket" << i << " error\n";
            return 0;
        }
        //std::cout << "create socket" << sockfd[i] << " successfully\n";
        fdtohandler[sockfd[i]] = i;
        cih[i].setSockfd(sockfd[i]);
    }
    return 1;
}

bool Client::connecttoServer(){
    for(int i = 0; i < CLIENT_NUM; ++i){
        if( connect(sockfd[i], (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
            std::cout << "socket" << i << " connect error\n";
            return 0;
        }
        //将套接字描述符加入到epoll中
        if( !epollAddSocketfd(sockfd[i])){
            std::cout << "add socket" << i << " to epoll erorr\n";
            return 0;
        }
        setNonblocking(sockfd[i]);
    }
    return 1;
}

bool Client::epollAddSocketfd(int socketfd){
	epoll_event ev;
    ev.data.fd = socketfd;
    ev.events = EPOLLIN;
    if( epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &ev) < 0)
		return 0;
	return 1;
}

bool Client::epollDelSocketfd(int socektfd){
	if( epoll_ctl(epollfd, EPOLL_CTL_DEL, socektfd, NULL) < 0)
		return 0;
	return 1;
}

bool Client::epollAddfdOut(int socketfd){
	epoll_event ev;
    ev.data.fd = socketfd;
    ev.events = EPOLLIN | EPOLLOUT;
    if( epoll_ctl(epollfd, EPOLL_CTL_MOD, socketfd, &ev) < 0)
		return 0;
	return 1;
}

bool Client::epollDelfdOut(int socketfd){
	epoll_event ev;
    ev.data.fd = socketfd;
    ev.events = EPOLLIN;
    if( epoll_ctl(epollfd, EPOLL_CTL_MOD, socketfd, &ev) < 0)
		return 0;
	return 1;
}

int Client::dealSend(int sendsockfd){
    int handlerid = fdtohandler[sendsockfd];
    //发送数据
	//std::cout << handlerid << " sending data to socket: ";
	int ret = cih[handlerid].sendSocketData();
	if(ret == 0){
		//?????未知情况
		return 0;
	}
	if(ret == -1){
		//std::cout << "sending data...\n";
		return -1;
	}
	else if(ret == -2){
		std::cout << "send erorr\n";
		return -2;
	}
	else
		//std::cout << "sended\n";

	return ret;
}

int Client::dealRecv(int recvsockfd){
     int handlerid = fdtohandler[recvsockfd];
    //接收数据
	//std::cout << handlerid << " receiving data from socket: ";
	int ret = cih[handlerid].recvSocketData();
	if(ret == 0){  //服务器断开连接
		close(recvsockfd);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, recvsockfd, NULL);
        std::cout << "server ends" << std::endl;
		return 0;
	}
	else if(ret == -1){
		//std::cout << "receiving data...\n";
		return -1;
	}
	else if(ret == -2){
		std::cout << "recv erorr\n";
		return -2;
	}
	else
		//std::cout << "received\n"; 

    //接受完数据，又激活发送缓冲区，等待发送
    cih[handlerid].getData(data);

    if(relay_count == 0)
        start = steady_clock::now();
    ++relay_count;
    if(relay_count == RELAY_COUNT){
        end = steady_clock::now();
        auto tt = duration_cast<milliseconds>(end - start);
        std::cout << tt.count() << std::endl;
        exit(0);
    }

	return ret;
}

bool Client::handleEvents(){
    int n = epoll_wait(epollfd, events, CLIENT_NUM, -1);
	if(n < 0){
		std::cout << "epoll error\n";
        return 0;
    }
    for(int i = 0; i < n; ++i){
        int fd = events[i].data.fd;
        if(events[i].events & EPOLLIN){
            if(dealRecv(fd) > 0){
                if(dealSend(fd) == -1) //数据还未发送完但是发送缓冲区已满
					epollAddfdOut(fd); //将发送缓冲区可写的事件添加到epoll中
			}
        }
        else if(events[i].events & EPOLLOUT){
			if(dealSend(fd) > 0)   //如果发送数据完毕
				epollDelfdOut(fd); //将发送缓冲区可写事件移出epoll，否则会一直触发				
		}
        else{
            std::cout << "fd erorr\n";
            return 0;
        }
    }
    return 1;
}

void Client::test(){
    for(int i = 0; i < CLIENT_NUM; ++i){
        cih[i].getData(data);
        cih[i].sendSocketData();
    }
}