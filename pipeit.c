#include "pipeit.h"

int main(void) {
    

    return 0;
}

pid_t safe_fork(void) {
    pid_t child_pid;
    if( (child_pid = fork()) < 0 ) {
        perror("negative child_pid");
        exit(EXIT_CHILD);
    }
    return child_pid;
}

int safe_dup2(int oldfd, int newfd) {
    if( (dup2(oldfd, newfd)) == -1) {
        perror("dup2");
        exit(EXIT_CHILD);
    }
    return newfd;
}

void safe_pipe(int pipefd[2]) {
    if(pipe(pipefd) == -1) {
        perror("pipe create failed");
        exit(EXIT_CHILD);
    }
}

struct child_pid_node* make_child_pid_node(pid_t child_pid) {
    struct child_pid_node* cp = malloc(sizeof(struct child_pid_node));
    cp->pid = child_pid;
    cp->next = NULL;
    return cp;
}

struct child_pid_node* add_child_pid(struct child_pid_node* tail, 
                                     pid_t child_pid) {
    struct child_pid_node* cp = make_child_pid_node(child_pid);
    if(cp == NULL) {
        exit(EXIT_CHILD);
    }
    if(tail != NULL) {
        tail->next = cp;
    }
    return cp;
}

void safe_close(int fd) {
    if((fd == STDIN_FILENO) || (fd == STDOUT_FILENO)) {
        return;
    }
    close(fd);
}

void wait_all_children(struct child_pid_node* list) {
    while(list != NULL) {
        if(waitpid(list->pid, NULL, 0) == -1) {
            perror("waitpid");
        }
        list = list->next;
    }
}