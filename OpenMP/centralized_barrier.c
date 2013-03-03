#include <stdio.h>
#include <omp.h>


int main(){
int count = 10;
int global_sense = 1;
int local_sense;
int i;
#pragma omp parallel shared(global_sense, count) firstprivate(local_sense) num_threads(10)
{
// Parallel Section begins
	
	//A thread can do some work
	for(i=0;i<1000; i++){}
	local_sense = 0;
	
	int threadId = omp_get_thread_num();
	printf("Thread ID Before : %d\n", threadId);

		#pragma omp critical(lastThreadWins)
		{
		  int fetched = count;
		  count --;
			if(fetched==1){
			 count = 2;
			 global_sense = local_sense;
			}
		//	else{		}
		}
		while(global_sense!=local_sense);
	printf("Thread ID After : %d\n", threadId);
// Parallel section ends
}
	return 0;
}
