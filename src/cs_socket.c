#include "cs_socket.h"

int socketClient(char* ip, char* port, struct sockaddr_in* srvaddr){

	int sfd;

    // create a socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        handle_error("can not create socketClient.");
	}

	// set the server socket address
	srvaddr->sin_family = AF_INET;
	srvaddr->sin_port = htons(atoi(port));
	inet_aton(ip, &(srvaddr->sin_addr));

    // connect
	if (connect(sfd, (struct sockaddr *)srvaddr, sizeof(struct sockaddr)) == -1){
        handle_error("can not connect to server.");
	}

    return sfd;
}

int socketServer(char* port, struct sockaddr_in* srvaddr){

	int srv_fd;
    //the default listen count
    int listenCount = 30;

    // create a socket
	if ((srv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        handle_error("can not create socketServer.");
	}

    /**
     * reuse the socket address
     */
    int sockopt = 1;
    setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));

	// set the socket address
	srvaddr->sin_family = AF_INET;
	srvaddr->sin_port = htons(atoi(port));
    srvaddr->sin_addr.s_addr = htonl(INADDR_ANY);

	// bind the socket with the address
	if(bind(srv_fd, (struct sockaddr *)srvaddr, sizeof(struct sockaddr)) == -1){
        handle_error("can not bind the address.");
	}

    // listening 
    // default was 30
	if (listen(srv_fd, listenCount) == -1){
        handle_error("can not listening to the port.");
	}

    return srv_fd;
}
