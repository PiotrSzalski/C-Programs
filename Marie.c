#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int main (int argc, char **argv) {
	if(argc < 2) {
		printf("Nie podano nazwy programu\n");
	}
	else {
		FILE *fp;
		int size = 10;
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		const char *delim = " ";
		int n, p = 0, done;
		char *word;
		char **words = malloc(4 * sizeof(char*));
		char *instruction[] = {"JnS","Load","Store","Add","Subt","Jump","AddI","JumpI","HEX","DEC","Input\n","Output\n","Halt\n","Skipkond\n","Clear\n"};
		char hex[] = {'0','1','2','3','4','9','B','C',' ',' ','5','6','7','8','A'};
		char **label = malloc(size * sizeof(char*));
		char **address = malloc(size * sizeof(char*));
	   	if((fp=fopen(argv[1], "r"))==NULL) {
	   	  	printf ("Nie odnaleziono pliku!\n");
	    	exit(1);
	   	}
		while ((read = getline(&line, &len, fp)) != -1) {
			n = 0;
			word = strtok(line,delim);
			while(word != NULL) {
				if(n > 3) {
					printf("Błąd asemblacji\n");
					return 1;
				} 
				words[n] = word;
				word = strtok(NULL,delim);
				n++;
			}
			if(words[0][strlen(words[0])-1] == '.' &&  n > 1) {
				words[0][strlen(words[0])-1] = '\n';
				char *w = malloc(strlen(words[0]) * sizeof(char));
				char *a = malloc(3 * sizeof(char));
				for(int i=0;i<strlen(words[0]);i++) {
					w[i] = words[0][i];
				}
				for(int i=0;i<3;i++) {
					a[i] = words[1][i];
				}
				label[p] = w;
				address[p] = a;
				p++;
				if(p > size) {
					size += 10;
					address = realloc(address,size);
					label = realloc(label,size);
				}
			}
		}
		fclose (fp);
		fp=fopen(argv[1], "r");
		while ((read = getline(&line, &len, fp)) != -1) {
			done = 0;
			n = 0;
			word = strtok(line,delim);
			while(word != NULL) {
				words[n] = word;
				word = strtok(NULL,delim);
				n++;
			}
			if(words[0][strlen(words[0])-1]=='.' && n>2) {
				for(int i=0;i<n-1;i++) {
					words[i] = words[i+1];
				}
				n--;
			}
			if(n > 1) {
				for(int i=0;i<14;i++) {
						if(strcmp(words[1],instruction[i]) == 0) {
							if(i<=7 && n == 3) {
								printf("%c",hex[i]);
								if(p == 0) {
									printf("%s",words[2]);
								} else {
									for(int j=0;j<p;j++) {
										if(strcmp(label[j],words[2]) == 0) {
											printf("%s\n",address[j]);
											break;
										} else if(j == p-1) {
											printf("%s",words[2]);
										}
									}
								}
							} else if(i==8 && n == 3) {
								printf("%s",words[2]);
							} else if(i==9 && n == 3) {
								int dec = 0;
								for(int j=0;j<strlen(words[2])-1;j++) {						
									dec *= 10;
									dec += (words[2][j] - '0');
								}
								int len = 0;
								char tab[4];
								while(dec != 0) {
									int r = dec % 16;
									if(r >= 10) {
										tab[len] = r + 55;
									} else {
										tab[len] = r + 48;
									}
									dec /= 16;
									len++;
								}
								for(int j=0;j<4-len;j++) {		
									printf("0");
								}
								for(int j=len-1;j>=0;j--) {
									printf("%c",tab[j]);
								}	
								printf("\n");
							} else if(i>9 && n == 2) {
								printf("%c",hex[i]);
								printf("000\n");
							} else {
								printf("Błąd asemblacji\n");
								return 1;
							}
							done = 1;
							break;
						}
					}
					if(done == 0) {
						printf("Błąd asemblacji\n");
						return 1;
					}
			} else {
				printf("Błąd asemblacji\n");
				return 1;
			}
		}
		fclose (fp);
	}
	return 0;
}
