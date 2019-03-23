#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

int myscanf(char * str, ...)
{
	int i = 0, k = 0, len = 0, ret = 0, buffsize = 32, a=0;
	char *buff = malloc(buffsize * sizeof(char));
	char *p = (char *) &str + sizeof str;
	while(str[len] != 0) {
		len++;
	}
	while(1) {
		read(0,&buff[a],sizeof(char));
		if(buff[a] == '\n') {
			break;
		}
		a++;	
		if(a >= buffsize) {
			buffsize += 32;
			buff = realloc(buff,buffsize);
		}
	}
	a=0;
	while (str && str[i]) {
		if(str[i] == '%') {
			i++;
			if(str[i] == 'd') {
				int l = 0;
				while(buff[k] != ' ') {
					if(buff[k] == '\n') {
						k--;	
						break;
					}
					l *= 10;
					l += buff[k] - 48;
					k++;
				}
				*(int *) *((int *)p) = l;
				p += sizeof(int *);
			}
			else if (str[i] == 's') {
				int l = 0;
				char *tab = (char *) *((char **)p);
				while(buff[k] != ' ') {
					if(buff[k] == '\n') {
						k--;	
						break;
					}
					tab[l] = buff[k];
					l++;
					k++;
				}	
				tab[l] = '\0';
				p += sizeof(char *);
			} else if(str[i] == 'x') {
				int l = 0;
				int begin = k;
				int end = k;
				int base = 1;
				while(buff[k] != ' ') {
					if(buff[k] == '\n') {
						k--;	
						break;
					}
					end++;
					k++;
				}
				for(int q=end-1;q>=begin;q--) {
					int dec = buff[q] - 48;
					if(dec >= 10) {
						dec -= 7;
					}
					l += dec * base;
					base = base * 16;
				}
				*(int *) *((int *)p) = l;
				p += sizeof(int *);
			} else if(str[i] == 'b') {
				int l=0;
				int begin = k;
				int end = k;
				int base = 1;
				while(buff[k]!=' ') {
					if(buff[k] == '\n') {
						k--;	
						break;
					}
					end++;
					k++;
				}
				for(int q=end-1;q>=begin;q--) {
					int dec = buff[q] - 48;
					l += dec * base;
					base = base * 2;
				}
				*(int *) *((int *)p) = l;
				p += sizeof(int *);
			}
			k++;		
		}
		ret++;
		i++;
		if(i < len && buff[k] == '\n') {
			while(1) {
				read(0,&buff[a],sizeof(char));
				if(buff[a] == '\n') {
					break;
				}
				a++;
				if(a >= buffsize) {
					buffsize += 32;
					buff = realloc(buff,buffsize);
				}	
			}
			k = 0;
			a = 0;
		}
	}	
	return ret;
}

int myprintf (char * str, ...)
{
	int i = 0, j = 0, size = 16, buffsize = 64;
	char *buff = malloc(buffsize * sizeof(char));
	char *p = (char *) &str + sizeof str;
	while (str && str[i]) {
		if(str[i] == '%') {
			i++;
			if(str[i] == 's') {
				int len = 0;
				while((*((char **)p))[len] != '\0') {
					len++;
				}
				char *tab = malloc(len * sizeof(char));
				for(int l=0;l<len;l++) {
					tab[l] = (*((char **)p))[l];
					buff[j] = tab[l];
					j++;
					if(j >= buffsize) {
						buffsize += 64;
						buff = realloc(buff,buffsize);
					}
     				}		
				p += sizeof(len * sizeof(char));
			} else if(str[i] == 'd') {
				int number = *((int *)p);
				int len = 0;
				char *tab = malloc(size * sizeof(char));
				while(number != 0) {
					tab[len] = number%10 + 48;
					number /= 10;
					len++;
					if(len >= size) {
						size += 16;
						tab = realloc(tab,size);
					}
				}
				for(int k=len-1;k>=0;k--) {
					buff[j] = tab[k];
					j++;
					if(j >= buffsize) {
						buffsize += 64;
					buff = realloc(buff,buffsize);
					}
				}
				p += sizeof(int);
			} else if(str[i] == 'x') {			
				int number = *((int *)p);
				int len = 0;
				char *tab = malloc(size * sizeof(char));
				while(number != 0) {
					int hex = number % 16;
					if(hex >= 10) {
						tab[len] = hex + 55;
					} else {
						tab[len] = hex + 48;
					}
					number /= 16;
					len++;
					if(len >= size) {
						size += 16;
						tab = realloc(tab,size);
					}
				}
				for(int k=len-1;k>=0;k--) {
					buff[j] = tab[k];
					j++;
					if(j >= buffsize) {
						buffsize += 64;
					buff = realloc(buff,buffsize);
					}
				}
				p += sizeof(int);
			} else if(str[i] == 'b') {
				int number = *((int *)p);
				int len = 0;
				char *tab = malloc(size * sizeof(char));
				while(number != 0) {
					tab[len] = number%2 + 48;
					number /= 2;
					len++;
					if(len >= size) {
						size += 16;
						tab = realloc(tab,size);
					}
				}
				for(int k=len-1;k>=0;k--) {
					buff[j] = tab[k];
					j++;
					if(j >= buffsize) {
						buffsize += 64;
						buff = realloc(buff,buffsize);
					}
				}
				p += sizeof(int);
			}
		} else {
			buff[j] = str[i];
			j++;
			if(j >= buffsize) {
				buffsize += 64;
				buff = realloc(buff,buffsize);
			}
		}
		i++;
	} 
	write(1,buff,j);
	return j;
}


int main() {
	char tab[10];
	int dec;
	int hex;
	int bin;
	
	myprintf("Tablica: ");
	myscanf("%s",tab);
	myprintf("Bin: ");
	myscanf("%b",&bin);
	myprintf("Dec: ");
	myscanf("%d %d",&dec);
	myprintf("Hex: ");
	myscanf("%x",&hex);

	myprintf("%s\n",tab);
	myprintf("%x\n",bin);
	myprintf("%d\n",dec);
	myprintf("%b\n",hex);
	
 	return 0;
}

// gcc -m32 -fno-stack-protector zad1.c
