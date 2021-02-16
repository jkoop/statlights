#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>

int explode(char* str, char* delim, char ***r) {
    char **res = (char**) malloc(sizeof(char*) * strlen(str));
    char *p;
    int i = 0;
    while (p = strtok(str, delim)) {
        res[i] = malloc(strlen(p) + 1);
        strcpy(res[i], p);
        ++i;
        str = NULL;
    }
    res = realloc(res, sizeof(char*) * i);
    *r = res;
    return i;
}

void main(){
	char str[] = "test1|test2|test3";
	char** res;
	int count = explode(str, "|", &res);
	int i;
	for (i = 0; i < count; ++i) {
	    printf("%s\n", res[i]);
	    free(res[i]);
	}
	free(res);
}
