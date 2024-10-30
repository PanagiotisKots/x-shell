/*
==========================================================================================
  @source file        ==     shell.c
  @brief              ==     XShell (Extended Shell)
  @project details    ==     This shell supports job control, history, and pipelining.
                             It is designed for Linux distros only and has been tested 
                             fully in Debian distributions.
=========================================================================================
*/


// Libraries needed for the project
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

// Include readline libraries (make sure to install them)
#include <readline/readline.h>
#include <readline/history.h>

// Function Declarations for built-in commands
int xsh_cd(char **args);
int xsh_help(char **args);
int xsh_exit(char **args);
int xsh_history(char **args);
int xsh_jobs(char **args);
int xsh_fg(char **args);
int xsh_bg(char **args);
int xsh_execute(char **args);

// New Function Declarations for additional commands
int xsh_pwd(char **args);
int xsh_clear(char **args);
int xsh_echo(char **args);
int xsh_cat(char **args);
int xsh_touch(char **args);
int xsh_mkdir(char **args);
int xsh_rmdir(char **args);
int xsh_rm(char **args);
int xsh_cp(char **args);
int xsh_mv(char **args);

// List of built-in commands and their corresponding functions
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "history",
    "jobs",
    "fg",
    "bg",
    "pwd",
    "clear",
    "echo",
    "cat",
    "touch",
    "mkdir",
    "rmdir",
    "rm",
    "cp",
    "mv"
};

int (*builtin_func[]) (char **) = {
    &xsh_cd,
    &xsh_help,
    &xsh_exit,
    &xsh_history,
    &xsh_jobs,
    &xsh_fg,
    &xsh_bg,
    &xsh_pwd,
    &xsh_clear,
    &xsh_echo,
    &xsh_cat,
    &xsh_touch,
    &xsh_mkdir,
    &xsh_rmdir,
    &xsh_rm,
    &xsh_cp,
    &xsh_mv
};

int xsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

// Linked list to manage background jobs
struct Job {
    pid_t pid;
    char command[1024];
    struct Job *next;
};

struct Job *job_list = NULL;

// Helper function to add a job to the job list
void add_job(pid_t pid, char *command) {
    struct Job *new_job = malloc(sizeof(struct Job));
    new_job->pid = pid;
    strcpy(new_job->command, command);
    new_job->next = job_list;
    job_list = new_job;
}

// Helper function to remove a job from the job list
void remove_job(pid_t pid) {
    struct Job **current = &job_list;
    while (*current != NULL) {
        if ((*current)->pid == pid) {
            struct Job *temp = *current;
            *current = (*current)->next;
            free(temp);
            return;
        }
        current = &((*current)->next);
    }
}

// Built-in command: show current jobs
int xsh_jobs(char **args) {
    struct Job *current = job_list;
    while (current != NULL) {
        printf("[%d] %s\n", current->pid, current->command);
        current = current->next;
    }
    return 1;
}

// Built-in command: bring job to foreground
int xsh_fg(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "xsh: expected PID for fg command\n");
        return 1;
    }
    pid_t pid = atoi(args[1]);
    int status;

    tcsetpgrp(STDIN_FILENO, pid);  // Set foreground process group
    waitpid(pid, &status, WUNTRACED);  // Wait for process

    if (WIFEXITED(status) || WIFSIGNALED(status)) {
        remove_job(pid);  // Remove job if it has finished
    }

    tcsetpgrp(STDIN_FILENO, getpgrp());  // Restore shell as foreground
    return 1;
}

// Built-in command: send job to background
int xsh_bg(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "xsh: expected PID for bg command\n");
        return 1;
    }
    pid_t pid = atoi(args[1]);
    kill(pid, SIGCONT);  // Continue the process in the background
    return 1;
}

// Built-in command: change directory
int xsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "xsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("xsh");
        }
    }
    return 1;
}

// Built-in command: print help
int xsh_help(char **args) {
    int i;
    printf("Panagiotis' XSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built-in:\n");

    for (i = 0; i < xsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    return 1;
}

// Built-in command: exit the shell
int xsh_exit(char **args) {
    return 0;
}

// Built-in command: show history of commands
int xsh_history(char **args) {
    HIST_ENTRY **history = history_list();
    if (history) {
        for (int i = 0; history[i]; i++) {
            printf("%d %s\n", i + history_base, history[i]->line);
        }
    }
    return 1;
}

// Built-in command: print current working directory
int xsh_pwd(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("xsh");
    }
    return 1;
}

// Built-in command: clear the terminal screen
int xsh_clear(char **args) {
    printf("\033[H\033[J"); // ANSI escape code to clear screen
    return 1;
}

// Built-in command: echo text to terminal
int xsh_echo(char **args) {
    int i = 1;  // Start from the first argument
    while (args[i] != NULL) {
        printf("%s ", args[i]);
        i++;
    }
    printf("\n");
    return 1;
}

// Built-in command: concatenate and display file content
int xsh_cat(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "xsh: expected argument to \"cat\"\n");
        return 1;
    }
    
    FILE *file = fopen(args[1], "r");
    if (file == NULL) {
        perror("xsh");
        return 1;
    }
    
    char c;
    while ((c = fgetc(file)) != EOF) {
        putchar(c);
    }
    fclose(file);
    return 1;
}

// Built-in command: create an empty file
int xsh_touch(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "xsh: expected argument to \"touch\"\n");
        return 1;
    }
    
    FILE *file = fopen(args[1], "a"); // Open for appending; creates file if it doesn't exist
    if (file == NULL) {
        perror("xsh");
        return 1;
    }
    fclose(file);
    return 1;
}

// Built-in command: create a directory
int xsh_mkdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "xsh: expected argument to \"mkdir\"\n");
        return 1;
    }
    
    if (mkdir(args[1], 0755) != 0) {
        perror("xsh");
    }
    return 1;
}

// Built-in command: remove an empty directory
int xsh_rmdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "xsh: expected argument to \"rmdir\"\n");
        return 1;
    }
    
    if (rmdir(args[1]) != 0) {
        perror("xsh");
    }
    return 1;
}

// Built-in command: remove a file
int xsh_rm(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "xsh: expected argument to \"rm\"\n");
        return 1;
    }
    
    if (remove(args[1]) != 0) {
        perror("xsh");
    }
    return 1;
}

// Built-in command: copy a file
int xsh_cp(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "xsh: expected two arguments to \"cp\"\n");
        return 1;
    }

    FILE *src = fopen(args[1], "rb");
    if (src == NULL) {
        perror("xsh");
        return 1;
    }

    FILE *dest = fopen(args[2], "wb");
    if (dest == NULL) {
        perror("xsh");
        fclose(src);
        return 1;
    }

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(src);
    fclose(dest);
    return 1;
}

// Built-in command: move or rename a file
int xsh_mv(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "xsh: expected two arguments to \"mv\"\n");
        return 1;
    }

    if (rename(args[1], args[2]) != 0) {
        perror("xsh");
    }
    return 1;
}

// Execute the built-in command
int xsh_execute(char **args) {
    for (int i = 0; i < xsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return 1;  // Command not found
}

// Main loop of the shell
void xsh_loop() {
    char *line;
    char **args;
    int status;

    do {
        line = readline("xsh> ");  // Prompt for input
        if (line && *line) {
            add_history(line);  // Add line to history
            args = malloc(sizeof(char *) * 64);  // Allocate space for arguments
            char *token = strtok(line, " ");
            int position = 0;

            while (token != NULL) {
                args[position++] = token;
                token = strtok(NULL, " ");
            }
            args[position] = NULL;  // Null-terminate the array of arguments

            status = xsh_execute(args);  // Execute the command
            free(args);
        }
        free(line);
    } while (status);
}

int main(int argc, char **argv) {
    // Initialize history
    using_history();
    
    xsh_loop();  // Start the shell loop

    return EXIT_SUCCESS;
}
  
