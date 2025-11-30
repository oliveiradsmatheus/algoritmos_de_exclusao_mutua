#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

int *turn;

int main(void) {
    int shmid = shmget(IPC_CREAT, 2 * sizeof(int), IPC_CREAT | 0666);
    turn = (int *) shmat(shmid, NULL, 0);

    turn[0] = turn[1] = FALSE;
    if (fork() > 0) {
        while (TRUE) {
            turn[0] = TRUE;
            while (turn[1]) {
                printf("Estou bloqueado - processo %d\n", getpid());
                turn[0] = FALSE;
                sleep(3);
                turn[0] = TRUE;
            }
            printf("< Sessão crítica %d: >\n", getpid());
            turn[0] = FALSE;
        }
    } else {
        while (TRUE) {
            turn[1] = TRUE;
            while (turn[0]) {
                printf("Estou bloqueado - processo %d\n", getpid());
                turn[1] = FALSE;
                sleep(3);
                turn[1] = TRUE;
            }
            printf("< Sessão crítica %d: >\n", getpid());
            turn[1] = FALSE;
        }
    }
    shmdt(turn);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}
