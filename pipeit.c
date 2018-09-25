#include "pipeit.h"

int main(void) {
    struct child_pid_node* child_pid_list = NULL;
    struct child_pid_node* child_pid_list_tail = child_pid_list;
    pid_t child_pid;
    int pipe[2];
    char* argv_ls[2];
    char* argv_sort[3];
    int outfile;
    if( (outfile = open("outfile", O_RDWR | O_CREAT | O_TRUNC, 
                                   S_IRUSR | S_IWUSR)) < 0) {
        perror("could not open file");
        exit(EXIT_FAILURE);
    }

    argv_ls[0] = CMD_LS;
    argv_ls[1] = NULL;
    argv_sort[0] = CMD_SORT;
    argv_sort[1] = SORT_ARG;
    argv_sort[2] = NULL;

    /* PIPE */
    safe_pipe(pipe);

    /* FORK FIRST CHILD (LS) */
    child_pid = safe_fork();
    child_pid_list_tail = add_child_pid(child_pid_list_tail, child_pid);
    if(child_pid_list == NULL) {
        child_pid_list = child_pid_list_tail;
    }
    
    /* FIRST CHILD (LS) */
    if(child_pid == 0) {
        /* SET CHILD's STDOUT to PIPE WRITE END */
        safe_dup2(pipe[PIPE_WRITE_END], STDOUT_FILENO);

        /* CLOSE CHILD's PIPE */
        safe_close(pipe[PIPE_READ_END]);
        safe_close(pipe[PIPE_WRITE_END]);

        /* EXEC */
        execvp(argv_ls[0], argv_ls);
        perror(argv_ls[0]);
        exit(EXIT_CHILD);
    }

    /* FORK SECOND CHILD (SORT) */
    child_pid = safe_fork();
    child_pid_list_tail = add_child_pid(child_pid_list_tail, child_pid);

    /* SECOND CHILD (SORT) */
    if(child_pid == 0) {
        /* SET CHILD's STDIN to PIPE READ END */
        safe_dup2(pipe[PIPE_READ_END], STDIN_FILENO);

        /* REDIRECT CHILD's STDOUT to OUTFILE */
        safe_dup2(outfile, STDOUT_FILENO);

        /* CLOSE CHILD's PIPE */
        safe_close(pipe[PIPE_READ_END]);
        safe_close(pipe[PIPE_WRITE_END]);

        /* EXEC */
        execvp(argv_sort[0], argv_sort);
        perror(argv_sort[0]);
        exit(EXIT_CHILD);
    }

    /* CLOSE PARENT'S PIPE AND OUTFILE AND WAIT FOR ALL CHILDREN */
    safe_close(pipe[PIPE_READ_END]);
    safe_close(pipe[PIPE_WRITE_END]);
    wait_all_children(child_pid_list);
    safe_close(outfile);

    return 0;
}/* END */

pid_t safe_fork(void) {
    pid_t child_pid;
    if( (child_pid = fork()) < 0 ) {
        perror("negative child_pid");
        exit(EXIT_CHILD);
    }
    return child_pid;
}

/* Dup2 with appropriate failure check */
int safe_dup2(int oldfd, int newfd) {
    if( (dup2(oldfd, newfd)) == -1) {
        perror("dup2");
        exit(EXIT_CHILD);
    }
    return newfd;
}

/* pipe with appropriate failure check */
void safe_pipe(int pipefd[2]) {
    if(pipe(pipefd) == -1) {
        perror("pipe create failed");
        exit(EXIT_CHILD);
    }
}

/* creates a child_pid_node */
struct child_pid_node* make_child_pid_node(pid_t child_pid) {
    struct child_pid_node* cp = malloc(sizeof(struct child_pid_node));
    cp->pid = child_pid;
    cp->next = NULL;
    return cp;
}

/* adds a new child_pid_node to the list of child's for parent */
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

/* close with appropriate failure check */
void safe_close(int fd) {
    if((fd == STDIN_FILENO) || (fd == STDOUT_FILENO)) {
        return;
    }
    close(fd);
}

/* calls waitpid on the list of child_pid_node's */
void wait_all_children(struct child_pid_node* list) {
    while(list != NULL) {
        if(waitpid(list->pid, NULL, 0) == -1) {
            perror("waitpid");
        }
        list = list->next;
    }
}