#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



int lsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // child process
        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("lsh");
    } else {
        //parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;

}






int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

char* builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int lsh_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}


int lsh_help(char **args)
{
    int i;
    printf("Matt Curschmans' LSH (Based on Stephen Brennan) \n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (int i = 0; i < lsh_num_builtins(); i ++) {
        printf("  %s\n", builtin_str[i]);
    }
    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

int lsh_execute(char **args)
{
    int i;
    if (args[0] == NULL) {
        //empty command
        return 1;
    }

    for (i=0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return lsh_launch(args);
}



#define LSH_RL_BUFFER_SIZE 1024
char *lsh_read_line()
{
    int bufsize = LSH_RL_BUFFER_SIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize); //allocate enough memory to store bufsize chars
    int c;

    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        //get the next character
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0'; // mark an end character
            return buffer;
        } else {
            buffer[position] = c;
        }
        position ++;

        //reallocate the buffer if we go over size
        if (position >= bufsize) {
            bufsize += LSH_RL_BUFFER_SIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr,"lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }

}

//we know a char* is just a string
// it points to a memory address, where the first val is a char, and increasing will give you another mem to store a k, effectively a str
// therefore, a char** must be an array of strings, which makes sense. its a memory address that stores the memory address of a char*
// each increasing index of the char** is just the memory address of another char* (i think)

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a" //delimiters, were using space
char **lsh_split_line(char *line) 
{
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char* token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM); //use the delims to tokenize 
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*)); //whats the size of a char* -> how do we know when to do this?
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}




void lsh_loop()
{
    char *line; 
    char **args; 
    int status;

    do {
        printf("> ");
        //read in lines
        line = lsh_read_line();
        //split the lines into arguments
        args = lsh_split_line(line);

        //execute commands with those arguments
        status = lsh_execute(args);

        free(line);
        free(args);

    } while (status);
}



int main(int argc, char **argv)
{
    //load config giles

    //run command loop
    lsh_loop();

    //cleanup and exit
    return EXIT_SUCCESS;
}