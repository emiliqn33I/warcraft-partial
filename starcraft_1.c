// warcraft game 
// where gold is taken from a mine at the same time,
// the operation being performed by several workers,
// and the amount of gold gathered is left to the farm. With enough gold, a new farm is created.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#define NUM_THREADS 2

pthread_mutex_t lock;
unsigned int mina = 2000;
unsigned int baza = 0;
unsigned int fermi = 0;

void *mine_gold(void *thread_id)
{
	int m_id;
	m_id = (int)((unsigned)thread_id) + 1;
	int gold_v_miner = 0;
	
	for( ; ; )
	{
		if(mina != 0)
		{
		
			pthread_mutex_lock(&lock);
			
			printf("Worker %d entered mine1\n", m_id);
			
			if(mina >= 10)
			{
				mina -= 10;
				gold_v_miner = 10;
			}
			else
			{
				mina -= mina;
				gold_v_miner = mina;
			}
			
			
			
			pthread_mutex_unlock(&lock);
			sleep(1);
			
			pthread_mutex_lock(&lock);
			
			printf("Worker %d depositing \n", m_id);
			
			if(mina >= 10)
			{
				baza += 10;
				gold_v_miner -= 10;
			}
			else
			{
				baza += mina;
				gold_v_miner -= mina;
			}
			
			pthread_mutex_unlock(&lock);
		}
		else{
			printf("Worker %d exiting\n", m_id);
			break;
		}
	}
	
	pthread_exit(NULL);
}

void *ferma(void *thread_id) 
{
	int t_id;
	t_id = (int)((unsigned)thread_id) + 1;
	
	int i = 1;
	while(1)
	{
		if(baza >= 50)
		{
			pthread_mutex_lock(&lock);
				
			printf("Farm %d created!\n", i);
			
			fermi += 1;
			mina -= 50;
			i++;
			
			pthread_mutex_unlock(&lock);
			sleep(2);
		}
		if(mina == 0)
		{
			break;
		}
		sleep(1);
	}
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if(pthread_mutex_init(&lock, NULL) != 0) 
    {
        printf("\n error in mutex\n");
        return -1;
    }
    
    	int miners_count = 0;
    	if(argc > 1)
    	{
		miners_count = atoi(argv[1]);
	}

	pthread_t miner_threads[NUM_THREADS];
	pthread_t farm_thread[1];
	if(miners_count == 0)
	{
		for(int i = 0; i < NUM_THREADS; i++)
		{
			int rc = pthread_create(&miner_threads[i], NULL, mine_gold, (void *)((unsigned)i));
			if(rc == -1)
			{
				printf("ERROR: pthread_create() return %d\n", rc);
				exit(-1);
			}
		}
	}
	else
	{
		for(int i = 0; i < miners_count; i++)
		{
			int rc = pthread_create(&miner_threads[i], NULL, mine_gold, (void *)((unsigned)i));
			if(rc == -1)
			{
				printf("ERROR: pthread_create() return %d\n", rc);
				exit(-1);
			}
		}
	}
	

	int rc = pthread_create(&farm_thread[0], NULL, ferma, (void *)((unsigned)0));
	if(rc == -1)
	{
		printf("ERROR: pthread_create() return %d\n", rc);
		exit(-1);
	}

	if(miners_count == 0)
	{
		for(int i = 0; i < NUM_THREADS ; i++)
		{
			void * result;
			int rc = pthread_join(miner_threads, (void *)&result);
			if(rc == -1)
			{
				printf("Error: pthread_join() return %d\n", rc);
			}
		}
	}
	else
	{
		for(int i = 0; i < miners_count ; i++)
		{
			void * result;
			int rc = pthread_join(miner_threads, (void *)&result);
			if(rc == -1)
			{
				printf("Error: pthread_join() return %d\n", rc);
			}
		}
	}

	
	
	printf("Gold int mine: %d Collected gold: %d Player gold: %d Farms:\n", mina, baza, baza - fermi, fermi);
	pthread_mutex_destroy(&lock);
	pthread_exit(NULL);
	
	return 0;
}
