#include <stdio.h>
#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
//Little note: in C, the keyword 'void' must actually be a parameter of the function header. Otherwise, it can accept an arbitrary number of arguments. 
char **lsh_split_line(char *line){
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if(!tokens){
		fprintf(stderr, "mysh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	token = strtok(line, LSH_TOK_DELIM);
	while (token != NULL){
		tokens[position] = token;
		position++;
	}
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
char *mysh_read_line(void){
	
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c; 

	if(!buffer){
		fprint(stderr, "mysh: allocation error\n");
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
			realloc(buffer, bufsize);
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





int main(int argc, char **argv){	
	
	//Load config files
	
	//Run command loop
	mysh_loop();

	//Perform shutdown/cleanup


	return 0;
}
