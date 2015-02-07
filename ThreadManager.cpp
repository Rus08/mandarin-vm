#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define PTW32_STATIC_LIB
#include <pthread.h>
#include "WebVM.h"
#include "ThreadManager.h"

inline uint32_t InitThreads(struct VirtualMachine* pVM)
{
	pVM->pThreads = (uint8_t*)malloc(sizeof(pthread_t) * MAX_ALLOWED_THREADS);
	if(pVM->pThreads == NULL){
		assert(false);
		return VM_NOT_ENOUGH_MEMORY;
	}
	memset(pVM->pThreads, 0, sizeof(pthread_t) * MAX_ALLOWED_THREADS);
	memset(pVM->ThreadAlive, 0, sizeof(pVM->ThreadAlive));
	pVM->ThreadsNum = 0;
	return VM_OK;
}

uint32_t GetAliveThreadsNum(struct VirtualMachine* pVM)
{
	pthread_t* pThreads = (pthread_t*)pVM->pThreads;
	if(pVM->ThreadsNum == 0){
		return 0;
	}
	// find how many threads alive
	pVM->ThreadsNum = 0;
	for(uint32_t i = 0; i < MAX_ALLOWED_THREADS; i++){
		if(pVM->ThreadAlive[i] != 0){
			pVM->ThreadsNum = pVM->ThreadsNum + 1;
		}
	}

	return pVM->ThreadsNum;
}
// This function should call only by main thread
uint32_t AddThread(struct VirtualMachine* pVM, void* (*ThreadFunc)(void*), void* Arg)
{
	pthread_t* pThreads = (pthread_t*)pVM->pThreads;
	uint32_t id = 0;

	if(GetAliveThreadsNum(pVM) == MAX_ALLOWED_THREADS){
		assert(false);
		return VM_TOO_MANY_THREADS;
	}
	// find vacant place
	for(uint32_t i = 0; i < MAX_ALLOWED_THREADS; i++){
		if(pVM->ThreadAlive[i] == false){
			id = i;
			break;
		}
	}
	if(pthread_create(&pThreads[id], NULL, ThreadFunc, Arg) != 0){
		assert(false);
		return VM_TOO_MANY_THREADS;
	}
	pVM->ThreadAlive[id] = true;
	pVM->ThreadsNum = pVM->ThreadsNum + 1;

	return VM_OK;
}

// This function is called by every thread except main
uint32_t ThreadExit(struct VirtualMachine* pVM, bool NormalExit, void* Arg)
{
	pthread_t* pThreads = (pthread_t*)pVM->pThreads;
	pthread_t thread_id = pthread_self();

	for(uint32_t i = 0; i < MAX_ALLOWED_THREADS; i++){
		if(pthread_equal(pThreads[i], thread_id) != 0){
			pVM->ThreadAlive[i] = false;
			break;
		}
	}
	if(NormalExit == false){
		pthread_exit(Arg);
	}
	return VM_OK;
}

inline uint32_t DeInitThreads(struct VirtualMachine* pVM)
{
	pthread_t* pThreads = (pthread_t*)pVM->pThreads;
	if(pVM->pThreads != NULL){
		for(int i = 0; i < MAX_ALLOWED_THREADS; i++){
			if(pVM->ThreadAlive[i] != false){
				pthread_cancel(pThreads[i]);
				pVM->ThreadAlive[i] = false;
			}
		}
		free(pVM->pThreads);
	}
	return VM_OK;
}
