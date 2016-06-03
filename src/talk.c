#include "talk.h"

/**
 * 用static修饰变量，改变量只能在本文件使用，
 * 同时另外的文件可以声明定义同样的变量。
 */
static int talkFd = 0;          //这个变量主要是从talk()获取peer_fd变量给signal_callback()使用
static char* feedBack = "\n\033[32m---->Bye!  :-)\033[0m\n\n";
static char* quit = "quit\n";   //退出程序命令字符串

int clientTalk(int peer_fd){

	struct pollfd pfds[2];

    pfds[0].fd = 0;
    pfds[0].events = POLLIN;
    pfds[1].fd = peer_fd;
    pfds[1].events = POLLIN;

    int poll_ret;
    int talk_ret;

	char recv_buf[TALK_BUF_SIZE] = { 0 };
	char send_buf[TALK_BUF_SIZE] = { 0 };

    talkFd = peer_fd;

	/**
	 * 输入提示符，第一次由客户端这边提供，之后所有的输入提示符都是有服务端提供
	 * 主要是因为客户端只是输入命令和接收反馈，至于接收多长的反馈信息无法得知，
	 * 所以有服务器端输出，因为服务器端知道何时pipe的输出结束了。
	 */
	printf(">>>");
	fflush(0);	//记住，一定要记得fflush()，要不然可能看不到效果

    while (1){
        if ((poll_ret = poll(pfds, 2, -1)) == -1){
            perror("poll");
			/**
			 * 加上这句是因为程序退出的时候光标被隐藏了，不知道是为什么，所以加上这句显示光标
			 * 其他地方同理
			 */
			printf("\033[?25h");
            return EXIT_FAILURE;
        }
        else if (poll_ret == 0){
            continue;
        }
        else{
            if (pfds[0].revents & POLLIN){ // from the stdi
				if ((talk_ret = clientSpeak(TALK_BUF_SIZE, send_buf, peer_fd)) == -1){ // ERROR
					printf("\033[?25h");
					return EXIT_FAILURE;
				} else if (talk_ret == 0){ // EOF
					pfds[0].fd = -1;
				}
            }

            if (pfds[1].revents & POLLIN){// from the peer
				if ((talk_ret = clientHear(TALK_BUF_SIZE, recv_buf, peer_fd)) == -1){
					printf("\033[?25h");
					return EXIT_FAILURE;
				}else if (talk_ret == 0){ // EOF
					pfds[1].fd = -1;
				}
            }
            
            if (pfds[0].fd == pfds[1].fd){
                break;
            }
        }
    }
}
int serverTalk(int peer_fd){
	struct pollfd pfds[1];

    pfds[0].fd = peer_fd;
    pfds[0].events = POLLIN;

    int poll_ret;
    int talk_ret;

	char recv_buf[TALK_BUF_SIZE] = { 0 };
	char send_buf[TALK_BUF_SIZE] = { 0 };

    talkFd = peer_fd;

    while (1){
        if ((poll_ret = poll(pfds, 1, -1)) == -1){
            perror("poll");
			/**
			 * 服务器端的不能输入，只能显示进行了哪些操作，
			 * 所以关闭了输入显示，在退出的时候要开启输入显示,
			 * 其他地方同理
			 */
			system("stty echo");
			printf("\033[?25h");
            return EXIT_FAILURE;
        }else if (poll_ret == 0){
            continue;
        }else{
            if (pfds[0].revents & POLLIN){ // from the peer
				if ((talk_ret = serverHearWithSpeak(TALK_BUF_SIZE, recv_buf, peer_fd) == -1)){
					system("stty echo");
					printf("\033[?25h");
					return EXIT_FAILURE;
				}else if (talk_ret == 0){ // EOF
					pfds[1].fd = -1;
				}
            }
        }
    }
}

int clientSpeak(int size, char buf[], int fd){
    int count;
    count = read(0, buf, size);
    if (count == -1){ // error
        perror("read");
        return -1;
    }else{
        /**
         * 实现通过指令关闭通信，和hear函数配合使用，
         * 同时在窗口上显示提示信息。
		 * 当输入quit或者键盘录入ctrl+d的时候退出
         */
        if((strncmp(buf, quit, strlen(quit)) == 0) || (count == 0)){
            write(1, feedBack, strlen(feedBack));
            write(fd, quit, strlen(quit));
            return -1;
        }else{
            return write(fd, buf, count);
        }
    }
}  

int clientHear(int size, char buf[], int fd){
    int count;
    count = read(fd, buf, size);
    if (count == -1){ // error
        perror("read");
        return -1;
    }else{
        if((strncmp(buf, quit, strlen(quit)) == 0)){
            write(1, feedBack, strlen(feedBack));
            return -1;
		}else{
			return write(1, buf, count);
		}
    }
}

int serverHearWithSpeak(int size, char buf[], int fd){
    int count;
    count = read(fd, buf, size);
    if (count == -1){ // error
        perror("read");
        return -1;
    }else{
        /**
         * 实现指令退出，同时在窗口上显示提示信息。
         */
        if((strncmp(buf, quit, strlen(quit)) == 0)){
            write(1, feedBack, strlen(feedBack));
            return -1;
        }else{
			/**
			 * 服务器端显示客户端输入的指令
			 */
			buf[count] = '\0';
			char* clientCmd = "\033[32mclient command: \033[0m";
			write(1, clientCmd, strlen(clientCmd));
			write(1, buf, strlen(buf));
			
			/**
			 * 由于使用的popen来执行所有的命令，但是该函数是通过fork()出子进程来完成
			 * 命令的执行，所以cd命令无效，这里通过调用chdir()来完成这部分的工作
			 *
			 * 当然在完成这部分工作之外，还要给客户端命令输入提示符
			 */
			buf[count - 1] = '\0';	//因为popen需要传入的是一个字符串，将最后的'\n'改成'\0'
			if(strncmp(buf, "cd", strlen("cd")) == 0){
				if(chdir(buf+3) == -1){
					char* errorDirectory = "\033[31mplease input accuracy directory\033[0m\n";
					write(fd, errorDirectory, strlen(errorDirectory));
				}
				write(fd, ">>>", strlen(">>>"));
			}else{

				/**
				 * popen函数的管道本身只能输出标准输出，但是我们很多时候还是会想得到
				 * 标准错误输出，所以通过在命令的后面加上标准错误输出重定向到标准输出上
				 */
				buf[count - 1] = ' ';
				buf[count] = '2';
				buf[count + 1] = '>';
				buf[count + 2] = '&';
				buf[count + 3] = '1';
				buf[count + 4] = '\0';

				FILE* pipeFile = popen(buf, "r");
				if(pipeFile == NULL){
					char* returnWord = "can not popen(cmd)\n";
					write(fd, returnWord, strlen(returnWord));
					return -1;
				}
				
				/**
				 * 输出命令执行完成输出起始标志，面这行标志为起始
				 */
				char* insulateShowBegin =	"\033[32m---------------------Out Begin--------------------\033[0m\n";
				write(fd, insulateShowBegin, strlen(insulateShowBegin));

				/**
				 * 输出命令执行结果
				 */
				if(pipeFile != NULL){
					while(fgets(buf, size, pipeFile) != NULL){
						write(fd, buf, strlen(buf));
					}
				}
				/**
				 * 输出命令执行完成输出结束标志， 这行标志位结束
				 */
				char* insulateShowEnd =		"\033[32m----------------------Out end--------------------\033[0m\n>>>";
				write(fd, insulateShowEnd, strlen(insulateShowEnd));
			}
            return 1;
        }
    }
}


void signal_callback(int signum){
    /**
     * 输出退出提示信息，
     * 同时告诉连接的另一端退出连接。
     */
    write(1, feedBack, strlen(feedBack));
    write(talkFd, quit, strlen(quit));

	system("stty echo");
	printf("\033[?25h");

    switch (signum){
        case SIGUSR1:
            exit(0);
            break;
        case SIGINT:
            exit(signum);
            break;
    }
}

