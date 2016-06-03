#ifndef TALK_H
    #define TALK_H

    #define TALK_BUF_SIZE 1024

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <signal.h>
    #include <poll.h>
    #include <sys/socket.h>
    /**
     * 信号回调函数
     */
    void signal_callback(int signum);
	/**
	 * 客户端对话运行的程序
	 */
	int clientTalk(int peer_fd);
	/**
	 * 服务器端对话运行的程序
	 */
	int serverTalk(int peer_fd);
	/**
	 * 客户端发送数据到服务器端
	 */
	int clientSpeak(int size, char buf[], int fd);
	/**
	 * 客户端接收从服务器端发送的数据
	 */
	int clientHear(int size, char buf[], int fd);
	/**
	 * 服务器端的程序负责接收客户端输入的命令，也就是hear，经过处理后直接
	 * 发送执行结果给客户端，也就是speak，所以函数名字位：serverHearWithSpeak
	 */
	int serverHearWithSpeak(int size, char buf[], int fd);

#endif
