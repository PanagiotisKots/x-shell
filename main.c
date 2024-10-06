/*
==========================================================================================
  @source file        ==     shell.c
  @brief              ==     XShell (Extended Shell)
  @project details    ==     This shell supports job control, history, and pipelining.
                             it is designed for linux distros only and it has been tested 
                             fully in debian distributions.
=========================================================================================
*/


//libraries needed for the project
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

/*
the libraries  <readline/readline.h> , <readline/history.h> might need to be installed ti use their functions.
if you have to install them simply do:

1. sudo apt update
2. sudo apt install libreadline-dev (for ubuntu / debian)

*/

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



// List of built-in commands and their corresponding functions
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "history",
  "jobs",
  "fg",
  "bg"
};

int (*builtin_func[]) (char **) = {
  &xsh_cd,
  &xsh_help,
  &xsh_exit,
  &xsh_history,
  &xsh_jobs,
  &xsh_fg,
  &xsh_bg
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

// Builtin command: show current jobs
int xsh_jobs(char **args) {
  struct Job *current = job_list;
  while (current != NULL) {
    printf("[%d] %s\n", current->pid, current->command);
    current = current->next;
  }
  return 1;
}

// Builtin command: bring job to foreground
int xsh_fg(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "xsh: expected PID for fg command\n");
    return 1;
  }
  pid_t pid = atoi(args[1]);
  int status;
  tcsetpgrp(STDIN_FILENO, pid);
  waitpid(pid, &status, WUNTRACED);
  tcsetpgrp(STDIN_FILENO, getpgrp());
  return 1;
}

// Builtin command: send job to background
int xsh_bg(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "xsh: expected PID for bg command\n");
    return 1;
  }
  pid_t pid = atoi(args[1]);
  kill(pid, SIGCONT);
  return 1;
}

// Builtin command: change directory
int xsh_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "xsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

// Builtin command: print help
int xsh_help(char **args) {
  int i;
  printf("Panagiotis' XSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built-in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }
  return 1;
}

// Builtin command: exit the shell
int xsh_exit(char **args) {
  return 0;
}

// Builtin command: show history of commands
int xsh_history(char **args) {
    HIST_ENTRY **history = history_list();
    if (history) {
        for (int i = 0; history[i]; i++) {
            printf("%d %s\n", i + history_base, history[i]->line);
        }
    }
    return 1;
}

// Execute external commands, including background tasks
int xsh_launch(char **args) {
  pid_t pid;
  int status;
  int bg = 0;

  // Check if the last argument is "&" for background execution
  if (strcmp(args[strlen(*args)-1], "&") == 0) {
    bg = 1;
    args[strlen(*args)-1] = NULL;
  }

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Forking error
    perror("lsh");
  } else {
    // Parent process
    if (bg) {
      add_job(pid, args[0]);
      printf("[%d] %s\n", pid, args[0]);
    } else {
      waitpid(pid, &status, 0);
    }
  }
  return 1;
}

// Split the input line into arguments (tokens)
char **xsh_split_line(char *line) {
  int bufsize = 64, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  token = strtok(line, " \t\r\n\a");
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += 64;
      tokens = realloc(tokens, bufsize * sizeof(char*));
    }

    token = strtok(NULL, " \t\r\n\a");
  }
  tokens[position] = NULL;
  return tokens;
}

// Execute built-in or external command
int xsh_execute(char **args) {
  int i;
  if (args[0] == NULL) {
    return 1;  // Empty command
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

// Main loop: reads user input, splits into tokens, and executes commands
void xsh_loop(void) {
  char *line;
  char **args;
  int status;

  using_history();  // Enable history support
  
  do {
    line = readline("lsh> ");  // Display prompt and read input
    if (!line) break;

    // Add command to history
    add_history(line);
    
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

// Main entry point
int main(int argc, char **argv) {
  // Load config files if any

  // Run the main command loop
  xsh_loop();

  return EXIT_SUCCESS;
}
