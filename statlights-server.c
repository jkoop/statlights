#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>

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

void *strinsert(char *haystack, const char *needle, const int offset) {
	int length = strlen(needle) + strlen(haystack);
	char result[length+1];
	bzero(result, length);

	strncpy(result, haystack, offset);
	strcat(result, needle);
	strcat(result, haystack + offset);

	strcpy(haystack, result);
}

void serve(const int newsockfd) {
	int n, insert;
	int l = 1;
	int get = 0;
	char buffer[256], sendBuff[256], httpok[2048];
	bzero(httpok, 2047);

	while (l) {
		bzero(buffer, 256);
		n = read(newsockfd, buffer, 255);
		if (n < 0)
			error("ERROR reading from socket");
		if (n == 0) {
			close(newsockfd);
			close(sockfd);
			return;
		}
		if (strstart("GET ", buffer)) {
			get = 1;
		}
		if (strstr(buffer, "\r\n\r\n") != NULL) {
			l = 0;
		}
		printf("%s", buffer);
	}
	bzero(sendBuff, 256);
	if (get) {
		// hostname
		char hostname[64];
		bzero(hostname, 63);
		gethostname(hostname, 63);

		// ram
		char filebuffer[128];
		FILE *stream;
		stream = fopen("/proc/meminfo", "r");
		fread(&filebuffer, sizeof(char), 128, stream);
		fclose(stream);
		char delim[] = "\n";
		char *ramtotal = strtok(filebuffer, delim);
		strtok(NULL, delim);
		char *ramavail = strtok(NULL, delim);
		char *string = ramtotal, *ptr = string;
		int ramtotali;
		while (*ptr) {
			if (isdigit(*ptr)) {
				long val = strtol(ptr, &ptr, 10);
				ramtotali = val;
			} else {
				ptr++;
			}
		}
		string = ramavail;
		ptr = string;
		int ramavaili;
		while (*ptr) {
			if (isdigit(*ptr)) {
				long val = strtol(ptr, &ptr, 10);
				ramavaili = val;
			} else {
				ptr++;
			}
		}
		char ram[16];
		snprintf(ram, sizeof ram, "%f", ((float)(ramtotali - ramavaili) / ramtotali));

		//swap
		struct sysinfo si;
		sysinfo (&si);
		char swap[16];
		snprintf(swap, sizeof swap, "%f", ((float)(si.totalswap - si.freeswap) / si.totalswap));
		if (si.totalswap == 0)
			strcpy(swap, "null");

		strcpy(httpok, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"name\":\"\",\"ram\":,\"swap\":}\n");
		insert = 60;

		//hostname
		strinsert(httpok, hostname, insert);
		insert = insert + strlen(hostname) + 8;

		//ram
		strinsert(httpok, ram, insert);
		insert = insert + strlen(hostname) + 6;

		//swap
		strinsert(httpok, swap, insert);

		n = netsend(httpok);
	} else {
		n = netsend("HTTP/1.1 400 Bad Request\n");
	}
	if (n < 0)
		error("ERROR writing to socket");
	close(newsockfd);
	printf("\n");
}

int main(int argc, char *argv[]){
	signal(SIGINT, intHandler);

	int portno;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	if (argc == 2) {
		portno = atoi(argv[1]);
	} else {
		fprintf(stderr,"Note: no port provided, using default port 2364\n");
		portno = 2364;
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int));
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
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
		serve(newsockfd);
	}
}
