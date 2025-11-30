#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

int *flag;
int *turn;

int main(void) {
    int shmidT = shmget(3, sizeof(int), IPC_CREAT | 0666);
    int shmidF = shmget(1, 2 * sizeof(int), IPC_CREAT | 0666);
    turn = (int *) shmat(shmidT, NULL, 0);
    flag = (int *) shmat(shmidF, NULL, 0);

    *turn = 0;
    flag[0] = flag[1] = FALSE;

    if (fork() > 0) {
        while (TRUE) {
            flag[0] = TRUE;
            sleep(2);
            while (flag[1]) {
                if (*turn != 0) {
                    flag[0] = FALSE;
                    while (*turn != 0) {
                        printf("Estou bloqueado - processo %d\n", getpid());
                        sleep(3);
                    }
                    flag[0] = TRUE;
                }
            }
            printf("< Sessão crítica %d: >\n", getpid());
            *turn = 1;
            flag[0] = FALSE;
        }
    } else {
        while (TRUE) {
            flag[1] = TRUE;
            sleep(2);
            while (flag[0]) {
                if (*turn != 1) {
                    flag[1] = FALSE;
                    while (*turn != 1) {
                        printf("Estou bloqueado - processo %d\n", getpid());
                        sleep(3);
                    }
                    flag[1] = TRUE;
                }
            }
            printf("< Sessão crítica %d: >\n", getpid());
            *turn = 0;
            flag[1] = FALSE;
        }
    }
    shmdt(turn);
    shmctl(shmidT, IPC_RMID, NULL);
    shmdt(flag);
    shmctl(shmidF, IPC_RMID, NULL);
    exit(0);
}
