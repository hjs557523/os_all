#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define READY     0x01
#define RUNNING   0x02
#define TIMESLICE 0x02
#define MAXSIZE   10000

typedef struct _PCB{
	int pid;
	int state;
	int time;
	struct _PCB *next;
}PCB,*pPCB;

pPCB readyQueue = NULL;
pPCB runningQueue = NULL;


void insert_readyQueue(int pid,int time){
	pPCB temp;
	pPCB proc = (pPCB)malloc(sizeof(PCB));
	proc->pid = pid;
	proc->time = time;
	proc->state = READY;
	proc->next = NULL;

	if (!proc){
		exit(0);
	}else if (readyQueue->next == NULL){
		readyQueue->next = proc;
	}else{
		temp = readyQueue->next;
		while(temp->next!=NULL){
			temp=temp->next;
		}
		temp->next = proc;
	}
}

void insert_runningQueue(int pid,int time){
	pPCB temp;
	pPCB proc = (pPCB)malloc(sizeof(PCB));
	proc->pid = pid;
	proc->time = time;
	proc->state = RUNNING;
	proc->next = NULL;

	if (!proc){
		exit(0);
	}else if (runningQueue->next == NULL){
		runningQueue->next = proc;
	}else{
		temp = runningQueue->next;
		while(temp->next!=NULL){
			temp=temp->next;
		}
		temp->next = proc;
	}
}


void delete_readyQueue(){
	pPCB temp;
	if (readyQueue->next == NULL){
		exit(0);
	}else{
		temp = readyQueue->next;
	    readyQueue->next = temp->next;
	    free(temp);
	}
}


void delete_runningQueue(){
	pPCB temp;
	if (runningQueue->next == NULL){
		exit(0);
	}else{
		temp = runningQueue->next;
		runningQueue->next = temp->next;
		free(temp);
	}
}


void exchangeInfo(pPCB x,pPCB y){
	pPCB temp = (pPCB)malloc(sizeof(pPCB));
	temp->pid = x->pid;
	temp->time = x->time;
	x->pid = y->pid;
	x->time = y->time;
	y->pid = temp->pid;
	y->time = temp->time;
}

void sortBySPF(){
	pPCB p = readyQueue->next;
	pPCB q;
	pPCB temp;
	while(p!=NULL){
		q = p;
		temp = p;
		while(q!=NULL){
			if (q->time < temp->time){
				temp = q;
			}
			q = q->next;
		}
		exchangeInfo(temp,p);
		p = p->next;
	}
}

void modifyPCBState(pPCB p){
	if (readyQueue->next == p){
		printf("P0%d ",p->pid);
		insert_runningQueue(p->pid,p->time);
		delete_readyQueue();
	}
	if (runningQueue->next == p){
		p->time = p->time-TIMESLICE;
		if (p->time <= 0){
			delete_runningQueue();
		}else{
			insert_readyQueue(p->pid,p->time);
			delete_runningQueue();
		}
		
	}
}

int main(){
	int i,num;
	int time[MAXSIZE];
	char s[MAXSIZE];
	char s2[MAXSIZE];
	readyQueue = (pPCB)malloc(sizeof(PCB));
	readyQueue->next = NULL;
	runningQueue = (pPCB)malloc(sizeof(PCB));
	runningQueue->next = NULL;
	scanf("%s",s);
	for(i=0;s[i]!='\0'; i++){
		s2[i] = tolower(s[i]);
    }
    s2[i]='\0';
	scanf("%d",&num);
	for (i = 0; i < num; ++i){
		scanf("%d",&time[i]);
		insert_readyQueue(i,time[i]);
	}
	if (strcmp(s2,"fcfs") == 0){
		printf("%s order:",s);
		while(readyQueue->next != NULL || runningQueue->next != NULL){
			modifyPCBState(readyQueue->next);
			modifyPCBState(runningQueue->next);
		}
	}
	if (strcmp(s2,"spf") == 0){
		printf("%s order:",s);
		sortBySPF();
		while(readyQueue->next != NULL || runningQueue->next != NULL){
			modifyPCBState(readyQueue->next);
			sortBySPF();
			modifyPCBState(runningQueue->next);
		}
	}
	return 0;
}

