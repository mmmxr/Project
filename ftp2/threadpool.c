#include "threadpool.h"
//共享资源
void pool_init(Thread_pool* pool,int max_thread_num)
{
    pthread_mutex_init(&(pool->queue_lock),NULL);
    pthread_cond_init(&(pool->queue_ready),NULL);

    pool->queue_head=NULL;

    pool->max_thread_num=max_thread_num;
    pool->cur_queue_size=0;

    pool->shutdown=0;
    pool->threadid=(pthread_t*)malloc(max_thread_num*sizeof(pthread_t));
    int i=0;
    for(i=0;i<max_thread_num;i++)
    {
        pthread_create(&(pool->threadid[i]),NULL,thread_routine, (void*)pool);
    }
}

//向线程池中添加任务
int pool_add_work(Thread_pool* pool,void* (*process)(void* arg),void* arg)
{
    //构造新任务
    Thread_Worker* new_worker=(Thread_Worker*)malloc(sizeof(Thread_Worker));
    new_worker->process=process;
    new_worker->arg=arg;
    new_worker->next=NULL;

    pthread_mutex_lock(&(pool->queue_lock));
    //添加任务至等待队列中
    Thread_Worker* member=pool->queue_head;
    if(member!=NULL)
    {
        while(member->next!=NULL)
        {
            member=member->next;
        }
        member->next=new_worker;
    }
    else
    {
        pool->queue_head=new_worker;
    }
    assert(pool->queue_head!=NULL);

    pool->cur_queue_size++;
    pthread_mutex_unlock(&(pool->queue_lock));

    //唤醒一个等待线程
    pthread_cond_signal(&(pool->queue_ready));

    return 0;
}

//销毁线程池,等待队列中的任务不会被执行，但是正在执行的线程会一直把任务运行后在退出
int pool_destroy(Thread_pool* pool)
{
    if(pool->shutdown)
    {
        return -1;
    }
    pool->shutdown=1;

    //唤醒所有线程，线程池要销毁了
    pthread_cond_broadcast(&(pool->queue_ready));

    //阻塞等待线程退出，否则就成僵尸进程
    int i=0;
    for(i=0;i<pool->max_thread_num;i++)
    {
        pthread_join(pool->threadid[i],NULL);
    }
    free(pool->threadid);
    
    //销毁等待队列
    Thread_Worker* head=NULL;
    while(pool->queue_head!=NULL)
    {
        head=pool->queue_head;
        pool->queue_head=pool->queue_head->next;
        free(head);
    }

    //销毁互斥和条件变量
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));

    free(pool);
    pool=NULL;
    return 0;
}
void* thread_routine(void* arg)
{
    Thread_pool* pool = (Thread_pool*)arg;
    while(1)
    {
        pthread_mutex_lock(&(pool->queue_lock));
        while(pool->cur_queue_size==0&& !pool->shutdown)
        {
            pthread_cond_wait(&(pool->queue_ready),&(pool->queue_lock));
        }

        //销毁线程池
        if(pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->queue_lock));
            pthread_exit(NULL);
        }
        assert(pool->cur_queue_size!=0);
        assert(pool->queue_head!=NULL);

        pool->cur_queue_size--;
        Thread_Worker*worker=pool->queue_head;
        pool->queue_head=worker->next;
        pthread_mutex_unlock(&(pool->queue_lock));

        //执行任务
        (*(worker->process)) (worker->arg);
        free(worker);
        worker=NULL;
    }
    pthread_exit(NULL);
}

