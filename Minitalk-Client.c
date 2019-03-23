#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
	
int request_tcp_connection(char *hn, int port)
{
	struct sockaddr_in sn;
	int s, ok;
	struct hostent *he;

	if(!(he = gethostbyname(hn))) {
		fprintf(stderr,"gethostbyname()");
		exit(1);
	}
	ok = 0;
	while(!ok) {
		sn.sin_family = AF_INET;
		sn.sin_port = htons((unsigned short)port);
		sn.sin_addr.s_addr = *(uint32_t *)(he->h_addr_list[0]);
		if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("socket()");
			exit(1);
		}
		ok=connect(s,(struct sockaddr *)&sn, sizeof(sn)) != -1;
		if(!ok) sleep(1);
	}
	return s;
}

int readwrite(int in, int out)
{
	char s[1024];
	int i;
	
	i = 0;
	while(read(in,&s[i],1) != 0) {
		if(s[i] == '\n') {
			write(out,s,i+1);
			return i+1;
		}
		if(s[i] == ' ') {
			s[i] = '\n';
		}
		i++;
	}
	return 0;
}

int main(int argc, char **argv) {
	int port, fd;
	char *hostname, buf[100];
	fd_set readfds, masterfds;
	int n;
	char info[] = {'P','o','d','a','j',' ','n','i','c','k',':','\n'};
	
	hostname = argv[1];
	port = atoi(argv[2]);
	fd = request_tcp_connection(hostname, port);
	FD_ZERO(&masterfds);
	FD_SET(0, &masterfds);
	FD_SET(fd, &masterfds);
	write(1,info,12);
	readwrite(0,fd);
	printf("\nZalogowani uzytkownicy:\n");
	readwrite(fd,1);
	while(1) {
		memcpy(&readfds, &masterfds, sizeof(fd_set));
		select(fd+1,&readfds,NULL,NULL,NULL);
		if(FD_ISSET(0,&readfds)) {
			n = read(0,buf,100);
			write(fd,buf,n);
		} else if (FD_ISSET(fd,&readfds)) {
			n=read(fd, buf, 100);
			write(1,buf,n);
		}
	} 
	return 0;
}
