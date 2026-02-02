#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}
static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}
int main(int c,char**v){
    if(c!=2||!isnum(v[1])) usage(v[0]);


    char path[256];
    FILE *myFile;
    int pid = atoi(v[1]);

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    myFile = fopen(path, "r");
    if (!myFile) DIE("fopen stat");

    int ppid;
    char state;
    unsigned long utime, stime;

    if (fscanf(myFile, "%*d %*s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &state, &ppid, &utime, &stime) != 4) {
        fclose(myFile);
        DIE_MSG("Failed to parse stat");
    }
    fclose(myFile);


    long tickTime = sysconf(_SC_CLK_TCK);
    long ticks = utime + stime;
    double cpu_time = (double)(ticks) / tickTime;

    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    myFile = fopen(path, "r");
    if (!myFile) DIE("fopen status");

    char line[256];
    long vmrss = -1;

    while (fgets(line, sizeof(line), myFile)) {
        if (sscanf(line, "VmRSS: %ld kB", &vmrss) == 1) break;
    }
    fclose(myFile);


    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
    myFile = fopen(path, "r");
    if (!myFile) DIE("fopen cmdline");

    char cmdline[1024];
    size_t n = fread(cmdline, 1, sizeof(cmdline) - 1, myFile);
    fclose(myFile);


    if (n == 0) {
        strcpy(cmdline, "[empty]");
    } else {
        cmdline[n] = '\0';
        for (size_t i = 0; i < n - 1; i++)
            if (cmdline[i] == '\0') cmdline[i] = ' ';
    }

    printf("PID:%d\n", pid);
    printf("State:%c\n", state);
    printf("PPID:%d\n", ppid);
    printf("Cmd:%s\n", cmdline);
    printf("CPU:%lu %.3f\n", ticks, cpu_time);
    if (vmrss >= 0)
        printf("VmRSS:%ld\n", vmrss);
    else
        printf("VmRSS is unavailable\n");

    return 0;
}
