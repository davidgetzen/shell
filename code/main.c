#include "title.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
//TODO: add ls functionality (look into open() to accomplish this)
//TODO: add rm for directories, add piping functionality, add error-checking for rm
//Little note: in C, the keyword 'void' must actually be a parameter of the function header. Otherwise, it can accept an arbitrary number of arguments. 
extern void title(void);
//Below: function declarations for builtin commands that will be utilized
int mysh_cd(char **args);
int mysh_help(char **args);
int mysh_exit(char **args);
int mysh_execute(char **args);
int mysh_rm(char **args);
int mysh_touch(char **args);
int mysh_ls(char **args);
//List of builtin commands followed by functions they allign with
char *builtin_str[] = {
	"cd",
	"help",
	"exit",
    "rm",
    "touch",
    "ls"
};

int (*builtin_func[]) (char**) = {
	&mysh_cd,
	&mysh_help,
	&mysh_exit,
    &mysh_rm,
    &mysh_touch,
    &mysh_ls,
};

int mysh_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

int mysh_cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "mysh expected argument to \"cd\" \n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("mysh");
		}
	}
	return 1;
}
int mysh_touch(char **args){
    if (args[1] == NULL){
        fprintf(stderr, "mysh expected argument to touch\n");
    } else {
        struct stat usefile;
        if (stat(args[1], &usefile) == 0){
            fprintf(stderr, "file already exists!\n");
        } else {
                if (open(args[1], O_CREAT) <= 0){
                perror("mysh");
            }
        }
    }
    return 1;
}

int mysh_rm (char **args){
    if (args[1] == NULL){
        fprintf(stderr, "mysh expected argument to rm \n");
    } else {
        if (unlink(args[1]) != 0){
                perror("mysh");
        }
    }
    return 1;
    }
int mysh_help (char **args) {
	int i;
	printf("Welcome to my shell! This is a program which leverages system calls to communicate with the kernel.\n");
	printf("\nTo use the shell, type program names and arguments. The following are built-in:\n");
	for (i = 0; i < mysh_num_builtins(); i++){
		printf("%s\n", builtin_str[i]);
	
	}
	printf("Use the man command for information on other programs\n");
	return 1;
}

int mysh_exit(char **args){
	return 0;
}
int mysh_ls(char **args){
    //struct dirent *de;
    //DIR *dr = opendir('.');
    long size;
    char *buf;
    char *ptr;
    char *buf_sec;
    struct dirent *use;
    size = pathconf(".", _PC_PATH_MAX);
    if ((buf = (char *)malloc((size_t)size)) != NULL){
        ptr = getcwd(buf, (size_t)size);
    }
    int fd = open(ptr, O_DIRECTORY);
    if (args[1] != NULL){
        fprintf(stderr, "mysh expected no arguments to ls \n");
    } else {
        if (fd < 0){
            perror("mysh"); 
        }
        ssize_t test = getdirentries(fd, buf_sec, (size_t) size, (off_t) ptr);
        printf("%d\n", fd);
        printf("%d\n", test);

    }
    return 1;
}
char **mysh_split_line(char *line){
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens){
		fprintf(stderr, "mysh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	token = strtok(line, LSH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;
		if (position >= bufsize){
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens){
				fprintf(stderr, "failure to allocate space\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}
int mysh_launch(char **args){
	//Below: pid_t is an unsigned int type capable of representing a process ID
	pid_t pid, wpid;
	int status;
	pid = fork();
	if (pid == 0){
		//Child process
		if (execvp(args[0], args) == -1){
			perror("mysh");
		}
		exit(EXIT_FAILURE);

	} else if (pid < 0){
		//Error forking
		perror("mysh");
	} else {
		//Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);

		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	}
	return 1;

}
char *mysh_read_line(void){
	
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c; 

	if(!buffer){
		fprintf(stderr, "mysh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	while(1){
		//Reading a character
		c = getchar();

		//If we hit EOF, replace it with a null character and return
		//Note: we store c as an int rather than a char, as EOF is an int. If we stored c as a char, we would not be able to check for EOF 
		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		}else{
			buffer[position] = c;
		}
		position++; 

	
		//If position is out of bounds, reallocate the block with twice the memory space 
		if (position >= bufsize){
			bufsize += LSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer){
				fprintf(stderr, "mysh: allocation error\n");
				exit(EXIT_FAILURE);
				}
			}

		}


}


void mysh_loop(void){

	char *line;
	char **args;
	int status;
	do {
		printf("> ");
		line = mysh_read_line();
		args = mysh_split_line(line);
		status = mysh_execute(args);

		free(line);
		free(args);

	} while(status);
}
//Check if the command is within the builtin function set; if it is, run it; if it isn't, call mysh_launch to run it 
int mysh_execute(char **args){
	int i;
	if (args[0] == NULL) {
		return 1;
	}
	for (i = 0; i < mysh_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0){
			return (*builtin_func[i])(args);
		}

	}
	return mysh_launch(args);
}



int main(int argc, char **argv){	
	
	//Load config files
	
	//Run command loop
    title();
	mysh_loop();

	//Perform shutdown/cleanup


	return 0;
}
