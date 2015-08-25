/*
 * =====================================================================================
 *
 *       Filename:  thread_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年06月08日 20时55分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#include "thread_pool.h"

/* 初始化线程池 */
thread_pool_t *thread_pool_init(int thread_num, int queue_max_num)
{
        thread_pool_t *thread_pool = NULL;

        do {
                /* 申请空间并给各个成员初始化 */
                thread_pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
                if (thread_pool == NULL)
                        break;
                thread_pool->thread_num = thread_num;
                thread_pool->free_thread_num = thread_num;
                thread_pool->queue_max_num = queue_max_num;
                thread_pool->resize_now = 0;
                thread_pool->queue_curr_num = 0;
                thread_pool->head = thread_pool->tail = NULL;

                /* 初始化互斥量和条件变量 */
                if (pthread_mutex_init(&(thread_pool->mutex), NULL))
                        break;
                if (pthread_cond_init(&(thread_pool->queue_empty), NULL))
                        break;
                if (pthread_cond_init(&(thread_pool->queue_not_empty), NULL))
                        break;
                if (pthread_cond_init(&(thread_pool->queue_not_full), NULL))
                        break;

                thread_pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
                if (thread_pool->threads == NULL)
                        break;
                thread_pool->queue_close = 0;
                thread_pool->pool_close = 0;

                /* 创建线程并加入阻塞队列 */
                int i;
                for (i = 0; i < thread_pool->thread_num; i++)
                {
                        if (pthread_create(&(thread_pool->threads[i]), NULL,
                                                thread_function, (void *)thread_pool))
                        {
                                free(thread_pool->threads);
                                break;
                        }
                }
                return thread_pool;
        } while (0);

        return NULL;
}

/* 销毁线程池 */
int thread_pool_destroy(thread_pool_t *thread_pool)
{
        pthread_mutex_lock(&(thread_pool->mutex));

        //线程池已经退出，则直接退出
        if (thread_pool->queue_close || thread_pool->pool_close)
        {
                pthread_mutex_unlock(&(thread_pool->mutex));
                return -1;
        }

        /* 置队列关闭标记 */
        thread_pool->queue_close = 1;

        /* 等待队列为空 */
        while (thread_pool->queue_curr_num != 0)
                pthread_cond_wait(&(thread_pool->queue_empty), &(thread_pool->mutex));

        /* 置线程池关闭标记 */
        thread_pool->pool_close = 1;

        pthread_mutex_unlock(&(thread_pool->mutex));

        /* 唤醒线程池中正在阻塞的线程都退出 */
        pthread_cond_broadcast(&(thread_pool->queue_not_empty));

        /* 唤醒阻塞的添加任务函数都退出 */
        pthread_cond_broadcast(&(thread_pool->queue_not_full));

        /* 等待所有线程退出 */
        int i;
        for (i = 0; i < thread_pool->thread_num; i++)
                pthread_join(thread_pool->threads[i], NULL);

        /* 清理资源 */
        pthread_mutex_destroy(&(thread_pool->mutex));
        pthread_cond_destroy(&(thread_pool->queue_empty));
        pthread_cond_destroy(&(thread_pool->queue_not_empty));
        pthread_cond_destroy(&(thread_pool->queue_not_full));
        free(thread_pool->threads);

        /* 清空等待队列 */
        task_t *free_task;
        while (thread_pool->head != NULL)
        {
                free_task = thread_pool->head;
                thread_pool->head = thread_pool->head->next;
                free(free_task);
        }

        free(thread_pool);

        return 0;
}

/* 调整线程池的大小 
 * @return -1   参数错误
 * @return -2   申请内存错误
 * @return -3   线程创建错误
 */
int thread_pool_resize(thread_pool_t *thread_pool, int thread_num, int queue_max_num)
{
        /* 不允许减小线程池规模 */
        if (thread_num < thread_pool->thread_num ||
                        queue_max_num < thread_pool->queue_max_num)
                return -1;
        pthread_mutex_lock(&(thread_pool->mutex));
        thread_pool->queue_max_num = queue_max_num;

        /* 调整threads的大小 */
        thread_pool->threads = (pthread_t *)realloc(thread_pool->threads, sizeof(pthread_t) * thread_num);
        if (thread_pool->threads == NULL)
                return -2;

        /* 创建新线程 */
        int i;
        for (i = thread_pool->thread_num; i < thread_num; i++)
                if (pthread_create(&(thread_pool->threads[i]), NULL, 
                                        thread_function, (void *)thread_pool))
                        return -3;
        thread_pool->free_thread_num += (thread_num - thread_pool->thread_num);
        thread_pool->thread_num = thread_num;
        thread_pool->resize_now = 1;
        pthread_mutex_unlock(&(thread_pool->mutex));

        return 0;
}

/* 向线程池添加任务
 * @return 0    表示正常
 * @return 1    表示队列已满或者所有线程忙碌(非阻塞模式)
 * @return -1   表示不可抗拒的错误
 * */
int thread_pool_add_task(thread_pool_t *thread_pool, void *(task_callback)(void *), void *arg, int is_block)
{
        pthread_mutex_lock(&(thread_pool->mutex));

        /* 队列已满 */
        while ((thread_pool->queue_curr_num == thread_pool->queue_max_num) &&
                        !(thread_pool->queue_close || thread_pool->pool_close))
        {
                if (is_block == 1) {
                        pthread_cond_wait(&(thread_pool->queue_not_full), &(thread_pool->mutex));
                } else {
                        pthread_mutex_unlock(&(thread_pool->mutex));
                        return 1;
                }
        }

        /* 所有线程都在忙的时候，不再添加任务 */
        if (thread_pool->free_thread_num == 0 && is_block == 0)
        {
                pthread_mutex_unlock(&(thread_pool->mutex));
                return 1;
        }

        /* 队列关闭或者线程池关闭则退出 */
        if (thread_pool->queue_close || thread_pool->pool_close)
        {
                pthread_mutex_unlock(&(thread_pool->mutex));
                return -1;
        }

        /* 创建新的任务 */
        task_t *new_task = (task_t *)malloc(sizeof(task_t));
        if (new_task == NULL)
        {
                pthread_mutex_unlock(&(thread_pool->mutex));
                return -1;
        }
        new_task->task_callback = task_callback;
        new_task->arg = arg;
        new_task->next = NULL;

        /* 将新任务添加到队列中 */
        thread_pool->queue_curr_num++;
        if (thread_pool->head == NULL) {
                thread_pool->head = thread_pool->tail = new_task;
        } else {
                thread_pool->tail->next = new_task;
                thread_pool->tail = new_task;
        }

        /* 如果刚刚调整过池大小，重置标记 */
        if (thread_pool->resize_now == 1)
                thread_pool->resize_now = 0;

        /* 通知阻塞的线程开始工作 */
        pthread_cond_signal(&(thread_pool->queue_not_empty));

        pthread_mutex_unlock(&(thread_pool->mutex));

        return 0;
}

/* 线程处理函数 */
void *thread_function(void *arg)
{
        thread_pool_t *thread_pool = (thread_pool_t *)arg;
        task_t *curr_task = NULL;

        while ( 1 )
        {
                pthread_mutex_lock(&(thread_pool->mutex));
                
                /* 队列为空则等待新任务 */
                while ((thread_pool->queue_curr_num == 0) && !thread_pool->pool_close)
                {
                        printf("Thread %lu will sleep and wait!\n", pthread_self());
                        pthread_cond_wait(&(thread_pool->queue_not_empty), &(thread_pool->mutex));
                }
                
                /* 线程池关闭的时候，线程退出 */
                if (thread_pool->pool_close)
                {
                        pthread_mutex_unlock(&(thread_pool->mutex));
                        pthread_exit(NULL);
                }

                /* 取出一个任务 */
                thread_pool->queue_curr_num--;
                curr_task = thread_pool->head;
                if (thread_pool->queue_curr_num == 0) {
                        thread_pool->head = thread_pool->tail = NULL;
                        /* 发出队列为空的信号，用于销毁线程池的函数 */
                        pthread_cond_signal(&(thread_pool->queue_empty));
                } else {
                        thread_pool->head = thread_pool->head->next;
                }

                /* 从队满转到非空，通知所有可能阻塞的任务添加函数可以工作了 */
                if (thread_pool->queue_curr_num == thread_pool->queue_max_num - 1)
                        pthread_cond_broadcast(&(thread_pool->queue_not_full));
                thread_pool->free_thread_num--;
                pthread_mutex_unlock(&(thread_pool->mutex));

                /* 执行任务函数 */
                curr_task->task_callback(curr_task->arg);
                free(curr_task);
                curr_task = NULL;

                pthread_mutex_lock(&(thread_pool->mutex));
                thread_pool->free_thread_num++;
                pthread_mutex_unlock(&(thread_pool->mutex));
        }

        return NULL;
}
