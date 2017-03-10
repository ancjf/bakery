// bakery.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <windows.h>
#include<stdlib.h>
#include <assert.h>
#include <process.h>
#include <time.h>
//#include <windows.h>

#define N 10
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

bool choosing[N];
int number[N];

int max_number(int *num, unsigned len)
{
	int ret = 0;
	for(unsigned i = 0; i < len; i++){
		if(num[i] > ret)
			ret = num[i];
	}

	return ret;
}

int g_process_number = 0;
volatile int g_rand;

void process(void* arg)
{
	srand(GetTickCount());

	unsigned i = (unsigned)arg;

	do{
		choosing[i] = true;
		number[i] = 1 + max_number(number, ARRAY_SIZE(number));
		choosing[i] = false;
		for (int j=0; j < N; ++j) {
			if(i == j)
				continue;

			while (choosing[j]) {//保证编号较小的进程先进入临界区
				// wait until process j receives its number
				Sleep(1);
			}

			while ((number[j]!=0) && !(number[j] > number[i] || j > i)) { //进程Pj是其他线程
				Sleep(1);
				// wait until processes with smaller numbers
				// or with the same number, but with higher
				// priority, finish their work
			}
	     }

		int r = rand();
		g_rand = r;

		for(int i = 0; i < 100; i++){
			assert(g_rand == r);
			if(g_rand != r)
				printf("test err i = %u, r = %d, g_rand = %d\r\n", i, r, g_rand);
			Sleep(1);
		}

		printf("i = %u, r = %d, g_rand = %d\r\n", i, r, g_rand);
		g_process_number++;
	     // critical section...
		number[i] = 0;
	     // non-critical section...
	}while (false);
}

int _tmain(int argc, _TCHAR* argv[])
{
	for(unsigned i = 0; i < N; i++){
		_beginthread(process, 0, (void*)i);
	}

	while(g_process_number < N)
		Sleep(100);

	system("pause");
	return 0;
}


