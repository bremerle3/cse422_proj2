#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include <string>

//#define MIN_TIME 2
//#define MAX_THINK 10
//#define MAX_EAT 5 
#define MIN_TIME 1
#define MAX_THINK 2
#define MAX_EAT 2

static int MAX_ITERS = 3;
int DEBUG = 0; //  Toggle for debug messages. Don't enable with OUTPUT or it will be messy!
int OUTPUT;
int NUM_FORKS;
int NUM_DINERS;
sem_t* forkArray; // array of fork semaphore pointers
sem_t access_activity;  // Binary semaphore to access the output function

void print_header(int num_philosophers)
{
    std::cout << "Eating Activity" << std::endl;
    std::string tens("          "); //Ten blank spaces
    int num_digits = NUM_DINERS - 10;
    //Display tens place digits
    if(NUM_DINERS > 10)
    {
        //std::cout << space;
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

void print_activity(int thread_id)
{

}

void *perform_work(void *argument)
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
        if(DEBUG)
            printf("%d Eating\n", passed_in_value);
        int eatTime = MIN_TIME + ( rand() % (MAX_EAT - MIN_TIME));
        if(DEBUG)
            printf("\tEat for %d secs\n", eatTime);
        sleep(eatTime);
        sem_post(&forkArray[passed_in_value % NUM_FORKS]);
        sem_post(&forkArray[(passed_in_value + 1) % NUM_FORKS]);
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
    print_header(NUM_DINERS);
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
    for(int idx=0; idx<NUM_FORKS; idx++)
    {

        sem_init(&forkArray[idx], 0, 1);
    }
    //Set up the threads
    pthread_t threads[NUM_DINERS];
    int thread_args[NUM_DINERS];
    int result_code, index;
    // create all threads one by one
    for (index = 0; index < NUM_DINERS; ++index) {
        thread_args[index] = index;
        if(DEBUG)
            printf("In main: creating thread %d\n", index);
        result_code = pthread_create(&threads[index], NULL, perform_work, (void *) &thread_args[index]);
        assert(0 == result_code);
    }
    // wait for each thread to complete
    for (index = 0; index < NUM_DINERS; ++index) {
        // block until thread 'index' completes
        result_code = pthread_join(threads[index], NULL);
        if(DEBUG)
            printf("In main: thread %d has completed\n", index);
        assert(0 == result_code);
    } 
    std::cout << "In main: All threads completed successfully\n" << std::endl;
    //printf("In main: All threads completed successfully\n");
    exit(EXIT_SUCCESS);
}



