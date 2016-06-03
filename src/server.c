#include "cs_socket.h"
#include "talk.h"

int main(int argc, char** argv){

    int client_fd;                  //保存连接客户端的文件描述符
    char IPdotdec[20];              //保存ip转换成点分十进制的字符串
	struct sockaddr_in srvaddr;     //保存服务器端数据的结构体
	struct sockaddr_in cliaddr;     //保存客户端数据的结构体
    /**
     * addr_len必须初始化为这个值，因为accept()的时候
     * 这个值表示要填充到(struct sockaddr *)&cliaddr
     * 数据结构体中字符数，大小应该是struct sockaddr的大小，
     * 如果不初始化，可能会出现accept:Invalid argument错误
     * 
     *     注意：struct sockaddr的长度和struct sockaddr_in
     * 的长度是一样的, 所以下面的sizeof的传入类型可以自由选择
     */
    //int addr_len = sizeof(struct sockaddr_in);  
    int addr_len = sizeof(struct sockaddr);  

    if(argc < 2){
        printf("please input form like: server <port>\n");
        exit(0);
    }
    
    signal(SIGINT, signal_callback);
    signal(SIGUSR1, signal_callback);

	/**
	 * 本程序服务器端设计为只能看客户端进行了哪些操作，不能输入，所以把反馈显示关闭了
	 */
	system("stty -echo");

    int server_fd = socketServer(argv[ 1 ], &srvaddr);

    // accept an incoming connction
    if ((client_fd = accept(server_fd, (struct sockaddr *)&cliaddr, &addr_len)) == -1){
        handle_error("accept error");
    }

    inet_ntop(AF_INET, (void *)&cliaddr.sin_addr, IPdotdec, 20);
    printf("\033[32mIP: %s has connected with port %d.\033[0m\n", IPdotdec, ntohs(srvaddr.sin_port));

    exit(serverTalk(client_fd));
}
