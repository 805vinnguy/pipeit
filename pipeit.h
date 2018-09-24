#ifndef PIPEIT_H
#define PIPEIT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>

/* >>> STRUCTS <<< */
struct child_pid_node {
    pid_t pid;
    struct child_pid_node* next;
};

/* >>> CONSTANTS <<< */
#define EXIT_CHILD -1
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

/* >>> FUNCTIONS <<< */
pid_t safe_fork(void);

int safe_dup2(int oldfd, int newfd);

void safe_pipe(int pipefd[2]);

struct child_pid_node* make_child_pid_node(pid_t child_pid);

struct child_pid_node* add_child_pid(struct child_pid_node* tail,
                                     pid_t child_pid);

void safe_close(int fd);

void wait_all_children(struct child_pid_node* list);

#endif