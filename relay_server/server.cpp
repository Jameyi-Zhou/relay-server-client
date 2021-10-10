#include "server_events.h"

using namespace std;

int main(int argc, char **argv){ 
	RelayServer server1;
	server1.listenToClients();
    while(1){
		if(!server1.handleEvents()){
			cout << "erorr ocurrs when handle epoll events!\n";
			exit(1);
		}
	}
	exit(0);
}