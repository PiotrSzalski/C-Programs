#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
int redout, redin, rederr;
char *output, *input, *errout;
int ctr;
int in, out, err;
int waits;
void ctrlc(int number) {
	ctr = 1;
	printf("\n");
}
int execute(char **args, int background) {
        if(strcmp(args[0],"cd") == 0) {
                chdir(args[1]);
                return 1;
        } else if (strcmp(args[0],"exit") == 0) {
                return 0;
        }
        int status,i,j;
	out = open(output,O_WRONLY|O_CREAT,0655);
	in = open(input,O_RDONLY);
	err = open(errout,O_WRONLY|O_CREAT,0655);
        i = fork();
        if(i == 0) {
		if(background == 1) {
			setpgid(0,0);
		}
		if(redout == 1) {
			close(1);
			dup2(out,1);
		}
		if(redin == 1) {
			close(0);
			dup2(in,0);
		}
		if(rederr == 1) {
			close(2);
			dup2(err,2);
		}
		if(execvp(args[0],args) == -1)
		{
			exit(0);
		}
        } else {	
		close(out);
		close(in);
		close(err);
		redin = 0;
		redout = 0;
		rederr = 0;
		if(background == 1) {	
			waits = 1;
		} else {
			ctr = 1;
			do {
      			j = waitpid(i, &status, WUNTRACED);
    			} while (!WIFEXITED(status) && !WIFSIGNALED(status));			
		}	
        }
	free(args);
        return 1;
}
void closefd(int *fd,int amount) {
	for(int i=0;i<amount;i++) {
		close(fd[i]);
	}
}	
void waitf(int amount) {
	for(int i=0;i<amount;i++) {
		wait(NULL);
	}
}
int executePipe(char ***words, int pipeLen, int background) {
	out = open(output,O_WRONLY|O_CREAT,0655);
	in = open(input,O_RDONLY);
	err = open(errout,O_WRONLY|O_CREAT,0655);
	int pipes[2*pipeLen-2];
	for(int i=0;i<pipeLen-1;i++) {
		pipe(pipes + 2*i);
	}
	for(int i=0;i<pipeLen;i++) {
		if(fork() == 0) {
			if(background == 1) {
				setpgid(0,0);
			}
			if(2*i-2>=0) {
				dup2(pipes[2*i-2],0);
			}
			if(2*i+1<2*pipeLen-2) {
				dup2(pipes[2*i+1],1);
			}
			if(i == pipeLen-1 && redout == 1) {
				close(1);
				dup2(out,1);
			}
			if(i == 0 && redin == 1) {
				close(0);
				dup2(in,0);
			}
			if(rederr == 1) {
				close(2);
				dup2(err,2);
			}
			closefd(pipes,2*pipeLen-2);
			execvp(words[i][0],words[i]);
		}
	}
	redout = 0;
	redin = 0;
	rederr = 0;
	closefd(pipes,2*pipeLen-2);
	if(background != 1) {
		waitf(pipeLen);
	}
	else {
		waits = pipeLen;
	}
	free(words);
	return 1;
}
int analizeLine(char **words, int wordNumber) {
	int background = 0;
	char **pipe = malloc(64 * sizeof(char*));
	char ***pipes = malloc(64 * sizeof(char**));
	int pipeNumber = 0;
	int pipesNumber = 0;
	int isPipe = 0;
	if(strcmp(words[wordNumber-1],"&") == 0) {
		background = 1;
		words[wordNumber-1] = NULL;
		wordNumber--;
	}
	for(int i=0;i<wordNumber;i++) {
		if(strcmp(words[i],">") == 0) {
			redout = 1;
			output = words[i+1];
			for(int k=0;k<2;k++) {
				for (int j=i;j<wordNumber;j++) {
					words[j]=words[j+1];
		
				}
			wordNumber--;
			}
			i--;
			continue;
		}
		else if(strcmp(words[i],"<") == 0) {
			redin = 1;
			input = words[i+1];
			for(int k=0;k<2;k++) {
				for (int j=i;j<wordNumber;j++) {
					words[j]=words[j+1];
				}
			wordNumber--;
			}
			i--;
			continue;
		}
		else if(strcmp(words[i],"2>") == 0) {
			rederr = 1;
			errout = words[i+1];
			for(int k=0;k<2;k++) {
				for (int j=i;j<wordNumber;j++) {
					words[j]=words[j+1];
				}
			wordNumber--;
			}
			i--;
			continue;
		} else if(strcmp(words[i],"|") == 0) {
			isPipe = 1;
			pipes[pipesNumber] = pipe;
			pipe = malloc(64 * sizeof(char*));
			pipeNumber = 0;
			pipesNumber++;
		}
		else {
			pipe[pipeNumber] = words[i];
			pipeNumber++;
		}
	} 
	if(isPipe == 1) {
		pipe[pipeNumber] = NULL;
		pipes[pipesNumber] = pipe;
		pipesNumber++;
		return executePipe(pipes,pipesNumber,background);
	} else {
        	return execute(words,background);
	}
}
int splitLine(char *line) {
	const char *delim =" ";
        char *word;
        char **words = malloc(64 * sizeof(char*));
        int wordNumber = 0;
        word = strtok(line,delim);
        while (word != NULL) {
                words[wordNumber] = word;
                wordNumber++;
                word = strtok(NULL,delim);
        }
        words[wordNumber]=NULL;
        return analizeLine(words, wordNumber);
} 
int readLine() {
        char *line = NULL;
        size_t len = 0;
        int length;
        length = getline(&line,&len,stdin);
	if(length == 1) {
		return 1;
	} 
	else if(length > 1) {
                line[length-1] = '\0';
        }
	else {
		exit(0);
	} 
   	return splitLine(line);
}
void handler(int number) {
	if(ctr != 1) {
		waitf(waits);
		ctr = 0;
	} 
	else {
		ctr = 0;
	}
}
int main() {
	int status=1;
	signal(17,handler);
	signal(2,ctrlc);
	while (status) {
		printf("$ ");
		status = readLine();
	}
	return 0;
}
