#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// 子进程退出，父进程没有kill掉它，称为僵尸进程
void fork7() {
    if (fork() == 0) {
        /* Child */
        printf("Terminating Child, PID = %d\n", getpid());
        exit(0);
    } else {
        printf("Running Parent, PID = %d\n", getpid());
        while (1)
            ; /* Infinite loop */ 
    }
}

// 父进程退出，子进程一直在执行
void fork8() {
    // 子进程将会一直执行，要去kill掉
    if (fork() == 0) {
        /* Child */
        printf("Running Child, PID = %d\n", getpid());
        while (1)
            ; /* Infinite loop */
    } else {
        printf("Terminating Parent, PID = %d\n", getpid());
        exit(0);
    } 
}

// wait 子进程
void fork9() {
    int child_status; 
    if (fork() == 0) {
        printf("HC: hello from child\n");
        exit(0);
    } else {
        printf("HP: hello from parent\n");
        wait(&child_status);
        printf("CT: child has terminated\n");
    }     
    printf("Bye\n");
}

// wait 多个子进程
void fork10() {
    int N = 100;
    pid_t pid[N];
    int i, child_status;
    for (i = 0; i < N; i++)
        if ((pid[i] = fork()) == 0) {
            exit(100+i); 
            /* Child */
        }
    for (i = 0; i < N; i++) {
        /* Parent */
        pid_t wpid = wait(&child_status);
        if (WIFEXITED(child_status))
            printf("Child %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
        else
            printf("Child %d terminate abnormally\n", wpid);
    }
}

// waitpid 等待指定子进程
void fork11() {
    int N = 100;
    pid_t pid[N];
    int i;
    int child_status;
    for (i = 0; i < N; i++)
        if ((pid[i] = fork()) == 0)
            exit(100+i); /* Child */
    for (i = N-1; i >= 0; i--) {
        pid_t wpid = waitpid(pid[i], &child_status, 0);
        if (WIFEXITED(child_status))
            printf("Child %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
        else
            printf("Child %d terminate abnormally\n", wpid);
    }
}

int main(int argc, char *argv[]) {
    int num = atoi(argv[1]);
    if (num == 7) {
        fork7();
    } else if (num == 8) {
        fork8();
    } else if (num == 9) {
        fork9();
    } else if (num == 10) {
        fork10();
    } else if (num == 11) {
        fork11();
    }
    return 0;
}
