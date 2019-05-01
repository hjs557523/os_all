#include <stdio.h>
#include <stdlib.h>

void initProcess(int Max[][10],int Allocation[][10],int Need[][10],int sourceNum,int processNum)
{
	int i,j,value;
	for (i = 0; i < processNum; ++i)
	{
		for (j = 0; j < sourceNum; ++j)
		{
			scanf("%d",&value);
			Max[i][j] = value;
		}
	}

	for (i = 0; i < processNum; ++i)
	{
		for (j = 0; j < sourceNum; ++j)
		{
			scanf("%d",&value);
			Allocation[i][j] = value;
		}
	}

	for (i = 0; i < processNum; ++i)
	{
		Need[i][sourceNum] = 0;
		for (j = 0; j < sourceNum; ++j)
		{
			Need[i][j] = Max[i][j] - Allocation[i][j];
			Need[i][sourceNum] = Need[i][sourceNum] + Need[i][j];
		}
	}
}


int check(int proNeed[10],int Available[10],int sourceNum)
{
	int i;
	int flag = 1;
	for (i = 0; i < sourceNum; ++i)
	{
		if (proNeed[i] > Available[i])
		{
			flag = 0;
		}
	}
	return flag;
}


int getMinNeedProcessId(int Need[][10],int processNum,int sourceNum)
{
	int processId = -1;
	int minSource = 9999;
	int Empty = 0;
	int i = 0;
	for (i = 0; i < processNum; ++i)
	{
		if ((Need[i][sourceNum] < minSource) && (Need[i][sourceNum] != Empty))
		{
			minSource = Need[i][sourceNum];
			processId = i;
		}
	}
	return processId;
}


void recycleSource(int MaxElem[10],int Available[10],int proNeed[10],int sourceNum)
{
	int i = 0;
	for (i = 0; i < sourceNum; ++i)
	{
		Available[i] = Available[i] + MaxElem[i];
	}
	proNeed[sourceNum] = 0;
}


void bankerAlgorithm(int Max[][10],int Allocation[][10],int Need[][10],int sourceNum,int processNum)
{
	int i;
	int firstId;
	int nowId;
	int Available[10] = {0};
	int firstRequest[10] = {0};
	int order[10] = {0};
	int flag = 0;
	int safe = 1;
	int value;
	for (i = 0; i < sourceNum; ++i)
	{
		scanf("%d",&value);
		Available[i] = value;
	}


	scanf("%d",&firstId);


	for (i = 0; i < sourceNum; ++i)
	{
		scanf("%d",&value);
		firstRequest[i] = value;
	}

	for (i = 0; i < sourceNum; ++i)
	{
		if ((firstRequest[i] > Need[firstId][i])||(firstRequest[i] > Available[i]))
		{
			safe = 0;
			printf("unsafe");
			return;
		}
	}

	if (safe == 1)
	{
		for (i = 0; i < sourceNum; ++i)
		{
			Available[i] = Available[i] - firstRequest[i];
			Allocation[firstId][i] = Allocation[firstId][i] + firstRequest[i];
			Need[firstId][i] = Need[firstId][i] - firstRequest[i];
			Need[firstId][sourceNum] = Need[firstId][sourceNum] - firstRequest[i];
		}

	}

    if(Need[firstId][sourceNum] == 0)
    {
        order[0] = firstId;
        i = 1;
        recycleSource(Max[firstId],Available,Need[firstId],sourceNum);
    }
    else
    {
        i = 0;
    }

	for (; i < processNum; ++i)
	{
		nowId = getMinNeedProcessId(Need,processNum,sourceNum);
		flag = check(Need[nowId],Available,sourceNum);
		if (flag)
		{
			order[i] = nowId;
			recycleSource(Max[nowId],Available,Need[nowId],sourceNum);
		}
		else
		{
			safe = 0;
			break;
		}
	}

	if (safe)
	{
		printf("safe order:");
		for (i = 0; i < processNum; ++i)
		{
			printf("P0%d ",order[i]);
		}
	}
	else
	{
		printf("unsafe");
	}
}

int main()
{
	int Max[20][10] = {0};
	int Allocation[20][10] = {0};
	int Need[20][10] = {0};
	int processNum = 0;
	int sourceNum = 0;
	scanf("%d%d",&sourceNum,&processNum);
	initProcess(Max,Allocation,Need,sourceNum,processNum);
	bankerAlgorithm(Max,Allocation,Need,sourceNum,processNum);
	system("pause");
	return 0;
}
