// bakery.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <windows.h>
#include<stdlib.h>
#include <assert.h>
#include <process.h>
#include <time.h>
//#include <windows.h>

#define N 20
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

bool choosing[N];
int number[N];

int max_number(int *num, unsigned len)
{
	int ret = num[0];
	for(unsigned i = 1; i < len; i++){
		if(num[i] > ret)
			ret = num[i];
	}

	return ret;
}

bool number_bigger(int ticket1, int n1, int ticket2, int n2)
{
	if(ticket1 > ticket2)
		return true;

	if(ticket1 == ticket2 && n1 > n2)
		return true;

	return false;
}

#define wait_until(X) do{while(!(X)){Sleep(1);}assert(X);}while(false)

volatile int g_process_number = 0;
volatile int g_rand;

void process(void* arg)
{
	unsigned i = (unsigned)arg;
	srand(GetTickCount()+i);

	do{
		choosing[i] = true;
		number[i] = 1 + max_number(number, ARRAY_SIZE(number));
		choosing[i] = false;
		for (int j=0; j < N; ++j) {
			if(i == j)
				continue;

			wait_until(choosing[j] == false);
			//保证编号较小的进程先进入临界区
			// wait until process j receives its number
			
			wait_until(number[j] == 0 || number_bigger(number[j], j, number[i], i));
			
			//进程Pj是其他线程
			// wait until processes with smaller numbers
			// or with the same number, but with higher
			// priority, finish their work
	     }

		int r = rand();
		//if(g_rand)
		printf("enter i = %u, number[%d] = %d, r = %d, g_rand = %d\r\n", i, i, number[i], r, g_rand);
		assert(!g_rand);

		g_rand = r;

		for(int i = 0; i < 100; i++){	
			if(g_rand != r)
				printf("test err i = %u, r = %d, g_rand = %d\r\n", i, r, g_rand);

			assert(g_rand == r);
			Sleep(1);
		}

		g_process_number++;
	     // critical section...

		printf("leave i = %u, number[%d] = %d, r = %d, g_rand = %d\r\n\r\n", i, i, number[i], r, g_rand);
		g_rand = 0;
		number[i] = 0;
	     // non-critical section...
	}while (false);
}

int _tmain(int argc, _TCHAR* argv[])
{
	for(unsigned i = 0; i < N; i++){
		choosing[i] = false;
		number[i] = 0;
	}

	for(unsigned i = 0; i < N; i++){
		_beginthread(process, 0, (void*)i);
	}

	while(g_process_number < N)
		Sleep(100);

	printf("Test succeed!\r\n");
	system("pause");
	return 0;
}


