#include "relay_server.h"

int main(int argc, char **argv){ 
	RelayServer server1;
	if( !server1.listenToClients())
		exit(1);
	int loop_num = 0;
    while(1){
		++loop_num;
		//std::cout << "loop" << loop_num << std::endl;
		if(!server1.handleEvents()){
			std::cout << "erorr ocurrs when handle epoll events!\n";
			exit(1);
		}
	}
	exit(0);
}