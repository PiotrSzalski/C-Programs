#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

int server_tcp_socket(char *hn,int port) 
{
	struct sockaddr_in sn;
	int s;
	struct hostent *he;
	
	if(!(he = gethostbyname(hn))) {
		fprintf(stderr,"gethostbyname()");
		exit(1);
	}
	bzero((char *)&sn, sizeof(sn));
	sn.sin_family = AF_INET;
	sn.sin_port = htons((short)port);
	sn.sin_addr = *(struct in_addr *)(he->h_addr_list[0]);

	if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	if(bind(s, (struct sockaddr *)&sn, sizeof(sn)) == -1) {
		perror("bind()");
		exit(1);
	}
	return s;
}

int accept_tcp_connection(int s)
{
	struct sockaddr_in sn;
	socklen_t l = sizeof(sn);
	int x;

	sn.sin_family = AF_INET;
	if(listen(s, 1) == -1) {
		perror("listen()");
		exit(1);
	}
	x = accept(s, (struct sockaddr*)&sn, (socklen_t *)&l);
	if(x == -1) {
		perror("accept()");
		exit(1);
	}
	return x;
}
	
int main(int argc, char **argv) {
	int port, sock, number, n, k, fd[number];
	char *hostname, buf[100], users[5][20], mess[50];
	int i = 0, len = 0;
	fd_set readfds, masterfds;

	hostname = argv[1];
	port = atoi(argv[2]);
	sock = server_tcp_socket(hostname, port);
	FD_ZERO(&masterfds);
	printf("Podaj liczbe uzytkownikow: ");
	scanf("%d",&number);
	printf("Aktualnie zalogowani:\n");
	for(int k=0;k<number;k++) {
		fd[k] = accept_tcp_connection(sock);
		FD_SET(fd[k], &masterfds);
		while(read(fd[k],&users[k][i],1) != 0) {
			if(users[k][i] == '\n') {
				break;
			}
			i++;
		}
		write(1,users[k],i+1);
		users[k][i] = '\0'; 
		i = 0;
	}
	for(int p=0;p<number;p++) {	
		while(users[p][i] != '\0') {
			mess[len] = users[p][i];
			len++;
			i++;
		}
		mess[len] = ' ';
		len++;
		i = 0;
	}
	mess[len] = '\n';
	len++;
	for(int q=0;q<number;q++) {	
		write(fd[q],mess,len);
	}
	while(1) {
		memcpy(&readfds, &masterfds, sizeof(fd_set));
		select(fd[number-1]+1,&readfds,NULL,NULL,NULL);
		for(int i=0;i<number;i++) {	
			if(FD_ISSET(fd[i],&readfds)) {
				n = read(fd[i],buf,100);
				k = 0;
				while(buf[k] != ' ') {
					k++;
				}	
				char nick[k+1];
				for(int p=0;p<k;p++) {
					nick[p] = buf[p];
				} 
				nick[k] = '\0';
				for(int l=0;l<number;l++) {	
					if(strcmp(nick,users[l]) == 0) 						{
						write(fd[l],&buf[k+1],n-k-1);
					}
				} 
			}
		}
	}
	return 0;	
}
