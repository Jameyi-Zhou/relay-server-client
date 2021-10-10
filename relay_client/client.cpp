#include "client_events.h"

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