#include "cs_socket.h"
#include "talk.h"

int main(int argc, char** argv){

	struct sockaddr_in srvaddr;     //连接服务器的数据结构体

    if(argc < 3){
        printf("please input form like: client <ip> <port>\n");
        exit(0);
    }

    signal(SIGINT, signal_callback);
    signal(SIGUSR1, signal_callback);

    int socket_fd = socketClient(argv[ 1 ], argv[ 2 ], &srvaddr);

    printf("\033[32mConnected to %s with port %d.\033[0m\n", argv[ 1 ], atoi(argv[ 2 ]));

    exit(clientTalk(socket_fd));
}
