#pragma once
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#define TASK_NUM 10
/*
 * 线程池里所有运行和等待的任务都是一个Thread_worker
 * 由于所有线程都在链表中，所以给出一个链表结构
*/
typedef struct Worker
{
    void *(*process) (void* arg);//回调函数
    void *arg;
    struct Worker *next;
}Thread_Worker;

/*
 *线程池结构
 * */
typedef struct{
    pthread_mutex_t queue_lock;//互斥量
    pthread_cond_t queue_ready;;//条件变量

    /*链表结构，线程池中所有等待任务*/
    Thread_Worker* queue_head;

    /*是否销毁线程*/
    int shutdown;
    pthread_t *threadid;

    //允许线程活动数
    int max_thread_num;

    //当前等待队列的任务数目
    int cur_queue_size;

}Thread_pool;


void pool_init(Thread_pool* pool,int max_thread_num);
int pool_add_work(Thread_pool* pool,void* (*process)(void* arg),void* arg);
int pool_destroy(Thread_pool* pool);
void* thread_routine(void* arg);








