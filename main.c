#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
/* Global Variables shared among threads*/
static FILE* fp;
static int seed = 12;
static int dealerscore;
static int scoreofallplayers[8] = { 0,0,0,0,0,0,0,0 };
/* mutex and semaphore to sync threads*/
static pthread_mutex_t lock;
static pthread_cond_t ok = PTHREAD_COND_INITIALIZER;
/* Throw dice function throws two dice and save their results in scoreofallplayer with respect to player positions*/
void* throwdice(void* n)
{
    int diceroll1, diceroll2;
    char playername;
    pthread_mutex_lock(&lock);
    int player = (int)n;
    time_t t;
    if (player == 0)
    {
        playername = 'A';
    }
    else if (player == 1)
    {
        playername = 'B';
    }
    else if (player == 2)
    {
        playername = 'C';
    }
    else if (player == 3)
    {
        playername = 'D';
    }
    else
    {
        playername = 'K';
        diceroll1 = rand() % seed;
        diceroll2 = rand() % seed;
        dealerscore = diceroll1 + diceroll2;
    }

    if (playername != 'K')
    {
        diceroll1 = rand() % seed;
        diceroll2 = rand() % seed;
        scoreofallplayers[(player * 2)] = diceroll1;
        scoreofallplayers[(player * 2) + 1] = diceroll2;

        printf("Player %c gets %d and %d with a sum %d \n", playername, diceroll1, diceroll2, diceroll1 + diceroll2);

        /* write text into the file stream*/
        fputs("Player ", fp);
        fputc(playername, fp);
        fputs(" gets ", fp);
        char str[50];
        sprintf(str, "%d", diceroll1);
        fputs(str, fp);

        fputs(" and ", fp);
        sprintf(str, "%d", diceroll2);
        fputs(str, fp);
        fputs(" with a sum ", fp);
        sprintf(str, "%d", diceroll1 + diceroll2);
        fputs(str, fp);
        fputs("\n", fp);


    }


    pthread_cond_signal(&ok);
    pthread_mutex_unlock(&lock);

}
/* Driver Program*/
int main(int argc, char* argv[])
{
    pthread_t callThd[5];
    pthread_mutex_init(&lock, NULL);
    time_t t;
    /* Intializes random number generator */
    srand((unsigned)time(&t));

    printf("Program Name Is: %s", argv[0]);
    if (argc == 1)
    {
        printf("\nNo Seed Paramter is Passed.\n");
        return 0;
    }
    else if (argc >= 2)
    {

        printf("\nSeed Value is: %s\n", argv[1]); 
    }

    /* open or create the logfile for writing*/
    fp = fopen("logfile.txt", "w");


    // Let us create three threads 
    for (int i = 0; i < 5; i++)
    {
        pthread_create(&callThd[i], NULL, throwdice, (void*)i);
    }
    for (int j = 0; j < 5; j++)
    {
        pthread_join(callThd[j], NULL);
    }
    char playername;
    printf("Dealer Score is: %d \n", dealerscore);
    printf("Dealer: The winning team is ");
    for (int k = 0; k < 4; k++)
    {
        if (k == 0)
        {
            playername = 'A';
        }
        else if (k == 1)
        {
            playername = 'B';
        }
        else if (k == 2)
        {
            playername = 'C';
        }
        else if (k == 3)
        {
            playername = 'D';
        }
        if (dealerscore == scoreofallplayers[(k * 2)] + scoreofallplayers[(k * 2) + 1])
        {
            printf(" %c ", playername);
        }
    }
    printf("\n");

    printf("***************************************\n");
    printf("Score Board\n");
    printf("****************************************\n");
    for (int k = 0; k < 4; k++)
    {
        if (k == 0)
        {
            playername = 'A';
        }
        else if (k == 1)
        {
            playername = 'B';
        }
        else if (k == 2)
        {
            playername = 'C';
        }
        else if (k == 3)
        {
            playername = 'D';
        }
        printf("Player %c: %d, %d \n", playername, scoreofallplayers[(k * 2)], scoreofallplayers[(k * 2) + 1]);
    }
    /* close the file*/
    fclose(fp);
    /* Closing all the threads*/
    pthread_exit(NULL);
    return 0;
}