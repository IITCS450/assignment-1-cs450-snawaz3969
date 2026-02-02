#define _POSIX_C_SOURCE 200809L
#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <cmd> [args]\n",a); exit(1);}
static double d(struct timespec a, struct timespec b){
 return (b.tv_sec-a.tv_sec)+(b.tv_nsec-a.tv_nsec)/1e9;}
int main(int c,char**v){
// TODO : ADD CODE HERE

    if (c < 2) usage(v[0]);

    struct timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0)
        DIE("clock_gettime");

    pid_t pid = fork();
    if (pid < 0) {
        DIE("fork");
    } else if (pid == 0) {
        execvp(v[1], &v[1]);
        DIE("execvp");
    }

    int status;
    if (waitpid(pid, &status, 0) < 0)
        DIE("waitpid");

    if (clock_gettime(CLOCK_MONOTONIC, &end) < 0)
        DIE("clock_gettime");

    printf("pid=%d", pid);
    printf(" elapsed=%.3f seconds", d(start, end));

    if (WIFEXITED(status)) {
        printf(" exit=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf(" signal=%d\n", WTERMSIG(status));
    }

 return 0;
}
