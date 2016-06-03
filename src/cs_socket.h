#ifndef __CS_SOCKET_H__
    #define __CS_SOCKET_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>

    #define handle_error(msg) { perror(msg); exit(EXIT_FAILURE); }

    /**
     * 客户端socket
     * 参数说明：
     *      1.ip：服务器ip地址，点分十进制的ip字符串
     *      2.port：服务器端口号，字串的形式
     *      3.srvaddr：连接到服务器端的sockaddr_in
     */
    int socketClient(char* ip, char* port, struct sockaddr_in* srvaddr);
    /**
     * 服务器端socket
     * 参数说明：
     *      1.port：监听端口号，是字串的形式
     *      2.srvaddr：服务器的sockaddr_in
     *
     *
     *      服务器端socket默认能够同时监听30个连接
     */
    int socketServer(char* port, struct sockaddr_in* srvaddr);

#endif
