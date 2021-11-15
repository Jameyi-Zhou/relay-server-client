#include "client.h"


int main(int argc, char **argv){
    char *addrip;
    char s[] = "127.0.0.1";
    if(argc == 1)
        addrip = s;
    else
        addrip = argv[1];

    Client cli;
    
    if( !cli.init(addrip) ){
        std::cout << "client initialization erorr\n";
        exit(1);
    }
    if( !cli.connecttoServer() )
        exit(1);
    cli.test();
    
    int loop_num = 0;

    while(1) {
        ++loop_num;
        //std::cout << "loop " << loop_num << std::endl;
        
        if(!cli.handleEvents()){
            std::cout << "erorr ocurrs when handle epoll events\n";
            exit(1);
        }       
    } 
    
}