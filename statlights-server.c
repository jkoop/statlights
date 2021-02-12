/*

✓ Phase 1.2 psuedo code

	open port TCP/arg1 for listening;
	when all received data contains "\r\n\r\n",
		if first received data is "GET ",
			send "HTTP/1.1 200 OK\r\nContent-type: application/json\r\n\r\n{\"name\":\"zeta\"}";
		else
			send "HTTP/1.1 400 Bad Request";
	close connection;
	goto top;

✓ Phase 1.1 psuedo code

	open port TCP/arg1 for listening;
	when all received data contains "\r\n\r\n",
		send "HTTP/1.1 200 OK\r\nContent-type: application/json\r\n\r\n{\"name\":\"zeta\"}";
	close connection;
	goto top;

✓ Phase 1 psuedo code

	open port TCP/arg1 for listening;
	when receive packet ending with "\r\n\r\n",
		send "HTTP/1.1 200 OK\r\nContent-type: application/json\r\n\r\n{\"name\":\"zeta\"}";
	close connection;
	goto top;

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

int sockfd, newsockfd;

void error(const char *msg){
	perror(msg);
	exit(1);
}

void intHandler(int signum) {
	if (newsockfd != 0)
		close(newsockfd);
	if (sockfd != 0)
		close(sockfd);
	exit(0);
}

int strstart(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

int netsend(const char *string) {
	printf("%s", string);
	return write(newsockfd, string, strlen(string));
}

int main(int argc, char *argv[]){
	signal(SIGINT, intHandler);

	int portno;
	socklen_t clilen;
	char buffer[256], sendBuff[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n, l, get;

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int));
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			error("ERROR on accept");
		l = 1;
		get = 0;
		while (l) {
			bzero(buffer, 256);
			n = read(newsockfd, buffer, 255);
			if (n < 0)
				error("ERROR reading from socket");
			if (n == 0) {
				close(newsockfd);
				close(sockfd);
				exit(0);
			}
			if (strstart("GET ", buffer))
				get = 1;
			if (strstr(buffer, "\r\n\r\n") != NULL)
				l = 0;
			printf("%s", buffer);
		}
		bzero(sendBuff, 256);
		if (get) {
			n = netsend("HTTP/1.1 200 OK\r\nContent-type: application/json\r\n\r\n{\"name\":\"zeta\"}\n");
		} else {
			n = netsend("HTTP/1.1 400 Bad Request\n");
		}
		if (n < 0)
			error("ERROR writing to socket");
		close(newsockfd);
	}
}
