#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include <string>

//Randon number generator bounds for thinking and eating (in seconds)
#define MIN_TIME 2
#define MAX_THINK 10
#define MAX_EAT 5 

static int MAX_ITERS = 3; //Number of eat/think cycles for each philosopher
static int EATING = 1; 
static int NOT_EATING = 0;
int DEBUG = 0; //Toggle for debug messages. Don't enable with OUTPUT or it will be messy!
int OUTPUT = 1; //Toggle for status output. Don't enable with DEBUG or it will be messy!
int NUM_FORKS; //Number of forks (semaphores)
int NUM_DINERS; //Number of philosophers (threads)
sem_t* forkArray; //Array of semaphores for the forks (set dynamically by user input)
sem_t access_activity;  //Binary semaphore to protect reads/writes to the activityArray
int* activityArray;  //Global array to keep track of the status of each philosopher

/*Prints the header, as displayed in the assignment documentation.
 */
void print_header(int num_philosophers)
{
    std::cout << "Eating Activity" << std::endl;
    std::string tens("          "); //Ten blank spaces
    int num_digits = NUM_DINERS - 10;
    //Display tens place digits
    if(NUM_DINERS > 10)
    {
        for(int idx=0; idx<num_digits; idx++)
        {
            tens += "1";
        }
        std::cout << tens << std::endl;
    }
    //Display ones place digits
    int one_digit = NUM_DINERS > 10 ? 10 : NUM_DINERS;
    for(int idx=0; idx<one_digit; idx++)
    {
        std::cout << idx;
    }
    if(NUM_DINERS <= 10)
    {
        std::cout << std::endl;
    }
    if(NUM_DINERS > 10)
    {
        for(int idx=0; idx<num_digits; idx++)
        {
            std::cout << idx;
        }
        std::cout << std::endl;
    }
}

/*Prints the status of each philosopher in the activityArray.
 * An asterisk indicates eating, blank space indicates thinking.
 */
void print_activity(int thread_id, int status)
{
    activityArray[thread_id] = status;
    //If OUTPUT is set high, display status
    if(OUTPUT == 1)
    {
        for(int idx=0; idx<NUM_DINERS; idx++)
        {
            if(activityArray[idx] == 0)
            {
                std::cout << " ";
            }
            else if (activityArray[idx] == 1)
            {
                std::cout << "*";
            }
        }
        std::cout << "      ";  
        if(status == EATING)
        {
            std::cout << thread_id << " starts eating";
        }
        else if (status == NOT_EATING)
        {
            std::cout << thread_id << " ends eating";
        }
        std::cout << std::endl;
    }
}

/* The philosophers are either thinking or eating. In order to each they
 * must pick up the two forks that are adjacent to them. I implemented Havender's
 * algorithm to avoid deadlock and starvation. This algorithm imposes an ordering
 * that the philosopher's must obey when they try to pick up forks. In this 
 * implementation, the philosophers always try to pick up the fork with the 
 * lower order, ensuring that at least one philosopher will always have access
 * to two forks.
 */
void *philosophize(void *argument)
{
    int passed_in_value;
    passed_in_value = *((int *) argument);
    int count = 0;
    while(count < MAX_ITERS)
    {
        if(DEBUG)
            printf("%d Thinking\n", passed_in_value);
        int thinkTime = MIN_TIME + ( rand() % (MAX_THINK - MIN_TIME));
        if(DEBUG)
            printf("\tThink for %d secs\n", thinkTime);
        sleep(thinkTime);
        //Access the lower-numbered fork first.
        sem_wait(&forkArray[passed_in_value % NUM_FORKS]);  
        //Then access the higher-numbered fork.
        if(NUM_DINERS > 1)
        {
            sem_wait(&forkArray[(passed_in_value + 1) % NUM_FORKS]); 
        }
        else if (NUM_DINERS == 1)
        {
            sem_wait(&forkArray[1]); 
        } 
        //Protect calls to print_activity with access_activity semaphore
        sem_wait(&access_activity);
        print_activity(passed_in_value, EATING);
        sem_post(&access_activity);
        if(DEBUG)
            printf("%d Eating\n", passed_in_value);
        int eatTime = MIN_TIME + ( rand() % (MAX_EAT - MIN_TIME));
        if(DEBUG)
            printf("\tEat for %d secs\n", eatTime);
        sleep(eatTime);
        sem_post(&forkArray[passed_in_value % NUM_FORKS]);
        sem_post(&forkArray[(passed_in_value + 1) % NUM_FORKS]);
        //Protect calls to print_activity with access_activity semaphore
        sem_wait(&access_activity);
        print_activity(passed_in_value, NOT_EATING);
        sem_post(&access_activity);
        if(DEBUG)
            printf("%d Finished Eating\n",passed_in_value);
        count++;
    }
    return NULL;
}
 
int main(void)
{
    std::cout << "Enter number of philosophers from 1 to 15" << std::endl;
    std::cin >> NUM_DINERS;
    //If we are outputting status to the terminal, print the header now
    if(OUTPUT == 1)
        print_header(NUM_DINERS);
    //Set the status of all philosophers to "NOT_EATING"
    activityArray = new int[NUM_DINERS];
    for(int idx=0; idx<NUM_DINERS; idx++)
    {
        activityArray[idx] = NOT_EATING;
    }
    //Initialize forks as binary semaphores
    if(NUM_DINERS > 1)
    {
        NUM_FORKS = NUM_DINERS;
        forkArray = new sem_t[NUM_FORKS];
    }
    else if(NUM_DINERS == 1) //One diner is degenerate case; still need two forks
    {
        NUM_FORKS = NUM_DINERS + 1;
        forkArray = new sem_t[NUM_FORKS];
    }
    else 
    {
        std::cout << "Error with NUM_DINERS" << std::endl;
        exit(EXIT_FAILURE);
    }
    //Initialize the fork semaphores
    for(int idx=0; idx<NUM_FORKS; idx++)
    {
        sem_init(&forkArray[idx], 0, 1);
    }
    //Set up the array semaphore as a mutex
    sem_init(&access_activity, 0, 1);
    //Set up the threads
    pthread_t philosophers[NUM_DINERS];
    int thread_args[NUM_DINERS];
    int result_code, index;
    for (index = 0; index < NUM_DINERS; index++) {
        thread_args[index] = index;
        if(DEBUG)
            printf("In main: creating thread %d\n", index);
        result_code = pthread_create(&philosophers[index], NULL, philosophize, (void *) &thread_args[index]);
        assert(0 == result_code);
    }
    // wait for each thread to complete
    for (index = 0; index < NUM_DINERS; index++) {
        // block until thread 'index' completes
        result_code = pthread_join(philosophers[index], NULL);
        if(DEBUG)
            printf("In main: thread %d has completed\n", index);
        assert(0 == result_code);
    } 
    std::cout << "In main: All threads completed successfully!\n" << std::endl;
    return 0;
}




