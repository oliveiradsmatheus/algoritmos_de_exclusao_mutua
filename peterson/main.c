#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1
#define N 2

int *turn;
int *interested;

void leave_region(int process) {
    interested[process] = FALSE;
}

void enter_region(int process) {
    int other = 1 - process;

    interested[process] = TRUE;
    *turn = process;
    while (*turn == process && interested[other] == TRUE) {
        printf("Estou bloqueado - processo %d\n", getpid());
        sleep(3);
    }
    printf("< Sessão crítica %d: >\n", getpid());
    leave_region(process);
}

int main(void) {
    int shmidF = shmget (1, N * sizeof(int), IPC_CREAT | 0666);
    int shmidT = shmget (1 + N, sizeof(int), IPC_CREAT | 0666);
    interested = (int*)shmat(shmidF, NULL, 0);
    turn = (int*)shmat(shmidT, NULL, 0);
    *turn = 0;
    interested[0] = interested[1] = 0;

    if (fork() > 0)
        while(TRUE)
            enter_region(0);
    else
        while(TRUE)
            enter_region(1);
    shmdt(turn);
    shmctl(shmidT, IPC_RMID, NULL);
    shmdt(interested);
    shmctl(shmidF, IPC_RMID, NULL);
    exit(0);
}
