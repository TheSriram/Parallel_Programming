#include <stdio.h>
#include <omp.h>
#include <math.h>

typedef struct node{
	int parentSense;
	int* parentPointer;
	int* childPointers[2];
	int haveChild[4];
	int childNotready[4];
	int dummy;
} node;

node treenode[15];


void populateArray(int noProc){
	int i=0, j=0;
	for(i=0;i<noProc;i++){
		for(j=0;j<4;j++){
			if((4*i+j+1)<noProc)
			treenode[i].haveChild[j]=1;
			else treenode[i].haveChild[j]=0;
		}
		if(i==0)
			treenode[i].parentPointer=&treenode[i].dummy;
		else
			treenode[i].parentPointer = &treenode[(int)floor((i-1)/4)].childNotready[(i-1) % 4];
		if(2*i+1 >= noProc) treenode[i].childPointers[0] = &treenode[i].dummy;
		else{
		 treenode[i].childPointers[0] = &treenode[2*i+1].parentSense;
		}
		if(2*i+2 >= noProc)  treenode[i].childPointers[1] = &treenode[i].dummy;
		else treenode[i].childPointers[1] = &treenode[2*i+2].parentSense;
		
		treenode[i].childNotready[0] = treenode[i].haveChild[0];
		treenode[i].childNotready[1] = treenode[i].haveChild[1];
		treenode[i].childNotready[2] = treenode[i].haveChild[2];
		treenode[i].childNotready[3] = treenode[i].haveChild[3];
		
		treenode[i].parentSense = 0;
	}
	
}

int main(){
	
	populateArray(14);


#pragma omp parallel num_threads(14)
	{
		
		int vpid = omp_get_thread_num();
		int local_sense = 1;
		int spin =1;
		int i=0, finishedChild=0;
		printf("Thread no %d is starting\n",omp_get_thread_num());
		while(spin){
			finishedChild=0;
			for(i=0;i<4;i++){
				if(treenode[vpid].childNotready[i]==0)
					finishedChild++;
			}
			if(finishedChild == 4)
				spin=0;
		}
		
		for(i=0;i<4;i++){
			treenode[vpid].childNotready[i]=treenode[vpid].haveChild[i];
		}
		*(treenode[vpid].parentPointer)=0;
		
		if(vpid != 0){
			while(treenode[vpid].parentSense != local_sense){};
		}
		
		// Wake up cycle
		*(treenode[vpid].childPointers[0]) = local_sense;
		*(treenode[vpid].childPointers[1]) = local_sense;
		local_sense = !local_sense;
		
		printf("ThreadId %d - Barrier is over. Come lets go and reach the next one\n",omp_get_thread_num());
		
	}

return 0;
}