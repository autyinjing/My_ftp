/*
 * =====================================================================================
 *
 *       Filename:  ftp_thread_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 17时12分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#include "my_ftp.h"

/* 全局变量定义 */
thread_pool_t *g_thread_pool = NULL;

/* 初始化线程池 */
thread_pool_t *thread_pool_init(int thread_num, int queue_max_num)
{
        thread_pool_t *th_pool = NULL;

        do {
                /* 申请空间并给各个成员初始化 */
                th_pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
                if (th_pool == NULL)
                {
                        break;
                }

                th_pool->thread_num = thread_num;
                th_pool->free_thread_num = thread_num;
                th_pool->queue_max_num = queue_max_num;
                th_pool->queue_curr_num = 0;

                th_pool->head = th_pool->tail = NULL;
                th_pool->resize_now = false;
                th_pool->queue_close = false;
                th_pool->pool_close = false;

                /* 初始化互斥量和条件变量 */
                if (pthread_mutex_init(&(th_pool->mutex), NULL))
                {
                        break;
                }
                if (pthread_cond_init(&(th_pool->queue_empty), NULL))
                {
                        break;
                }
                if (pthread_cond_init(&(th_pool->queue_not_empty), NULL))
                {
                        break;
                }
                if (pthread_cond_init(&(th_pool->queue_not_full), NULL))
                {
                        break;
                }

                th_pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
                if (th_pool->threads == NULL)
                {
                        break;
                }

                /* 创建线程并加入阻塞队列 */
                int i;
                for (i = 0; i < th_pool->thread_num; ++i)
                {
                        if (pthread_create(&(th_pool->threads[i]), NULL, thread_function, (void *)th_pool))
                        {
                                free(th_pool->threads);
                                break;
                        }
                }
                return th_pool;
        } while ( 0 );

        return NULL;
}

/* 线程处理 */
void *thread_function(void *arg)
{
        thread_pool_t *th_pool = (thread_pool_t *)arg;
        task_t *curr_task = NULL;

        while ( 1 )
        {
                pthread_mutex_lock(&(th_pool->mutex));

                /* 队列为空则等待新任务 */
                while ((th_pool->queue_curr_num == 0) && !(th_pool->pool_close))
                {
                        printf("Thread [%lu] will sleep and wait!\n", pthread_self());
                        pthread_cond_wait(&(th_pool->queue_not_empty), &(th_pool->mutex));
                }

                /* 线程池关闭时，线程退出 */
                if (th_pool->pool_close)
                {
                        pthread_mutex_unlock(&(th_pool->mutex));
                        pthread_exit(NULL);
                }

                /* 取出一个任务 */
                th_pool->queue_curr_num--;
                curr_task = th_pool->head;
                if (th_pool->queue_curr_num == 0) {
                        th_pool->head = th_pool->tail = NULL;
                        pthread_cond_signal(&(th_pool->queue_empty));
                } else {
                        th_pool->head = th_pool->head->next;
                }

                /* 从队满转到非空，通知所有可能阻塞的任务添加函数可以工作了 */
                if (th_pool->queue_curr_num == th_pool->queue_max_num - 1)
                {
                        pthread_cond_broadcast(&(th_pool->queue_not_full));
                }
                th_pool->free_thread_num--;
                pthread_mutex_unlock(&(th_pool->mutex));

                /* 执行任务函数 */
                curr_task->task_callback(curr_task->arg);
                free(curr_task);
                curr_task = NULL;

                pthread_mutex_lock(&(th_pool->mutex));
                th_pool->free_thread_num++;
                pthread_mutex_unlock(&(th_pool->mutex));
        }

        return NULL;
}

/* 添加任务
 * @return 0    表示正常 
 * @return 1    表示队列已满或者所有线程忙碌
 * @return -1   表示不可抗拒的错误 
 * */
int thread_pool_add_task(thread_pool_t *th_pool, void *(task_callback)(void *), void *arg, bool is_block)
{
        pthread_mutex_lock(&(th_pool->mutex));

        /* 队列已满 */
        while ((th_pool->queue_curr_num == th_pool->queue_max_num) && !(th_pool->queue_close || th_pool->pool_close))
        {
                if (is_block) {
                        pthread_cond_wait(&(th_pool->queue_not_full), &(th_pool->mutex));
                } else {
                        pthread_mutex_unlock(&(th_pool->mutex));
                        return 1;
                }
        }

        /* 所有线程都在忙的时候，不再添加任务 */
        if (th_pool->free_thread_num == 0 && !is_block)
        {
                pthread_mutex_unlock(&(th_pool->mutex));
                return 1;
        }

        /* 队列关闭或者线程池关闭则退出 */
        if (th_pool->queue_close || th_pool->pool_close)
        {
                pthread_mutex_unlock(&(th_pool->mutex));
                return -1;
        }

        /* 创建新的任务 */
        task_t *new_task = (task_t *)malloc(sizeof(task_t));
        if (new_task == NULL)
        {
                pthread_mutex_unlock(&(th_pool->mutex));
                return -1;
        }
        new_task->task_callback = task_callback;
        new_task->arg = arg;
        new_task->next = NULL;

        /* 将新任务添加到队列中 */
        th_pool->queue_curr_num++;
        if (th_pool->head == NULL) {
                th_pool->head = th_pool->tail = new_task;
        } else {
                th_pool->tail->next = new_task;
                th_pool->tail = new_task;
        }

        /* 如果刚刚调整过大小，重置标记 */
        if (th_pool->resize_now)
        {
                th_pool->resize_now = false;
        }

        /* 通知阻塞的线程开始工作 */
        pthread_cond_signal(&(th_pool->queue_not_empty));
        pthread_mutex_unlock(&(th_pool->mutex));

        return 0;
}

/* 调整线程池的大小
 * @return -1   参数错误
 * @return -2   申请内存错误
 * @return -3   线程创建错误
 * */
int thread_pool_resize(thread_pool_t *th_pool, int thread_num, int queue_max_num)
{
        /* 不允许减小线程池规模 */
        if (thread_num <= th_pool->thread_num || queue_max_num <= th_pool->queue_max_num)
        {
                return -1;
        }

        pthread_mutex_lock(&(th_pool->mutex));
        th_pool->queue_max_num = queue_max_num;

        /* 调整threads的大小 */
        th_pool->threads = (pthread_t *)realloc(th_pool->threads, sizeof(pthread_t) * thread_num);
        if (th_pool->threads == NULL)
        {
                return -2;
        }

        /* 创建新线程 */
        int i;
        for (i = th_pool->thread_num; i< thread_num; ++i)
        {
                if (pthread_create(&(th_pool->threads[i]), NULL, thread_function, (void *)th_pool))
                {
                        return -3;
                }
        }

        th_pool->free_thread_num += (thread_num - th_pool->thread_num);
        th_pool->thread_num = thread_num;
        th_pool->resize_now = true;
        pthread_mutex_unlock(&(th_pool->mutex));

        return 0;
}

/* 销毁线程池 */
int thread_pool_destroy(thread_pool_t *th_pool)
{
        pthread_mutex_lock(&(th_pool->mutex));

        /* 线程池已经退出，则直接退出 */
        if (th_pool->queue_close || th_pool->pool_close)
        {
                pthread_mutex_unlock(&(th_pool->mutex));
                return -1;
        }

        /* 重置队列关闭标记 */
        th_pool->queue_close = true;

        /* 等待队列为空 */
        while (th_pool->queue_curr_num != 0)
        {
                pthread_cond_wait(&(th_pool->queue_empty), &(th_pool->mutex));

        }

        /* 置线程关闭标记 */
        th_pool->pool_close = true;

        pthread_mutex_unlock(&(th_pool->mutex));

        /* 唤醒线程池中正在阻塞的线程，都退出吧 */
        pthread_cond_broadcast(&(th_pool->queue_not_empty));

        /* 唤醒阻塞的添加任务函数，都退出吧 */
        pthread_cond_broadcast(&(th_pool->queue_not_full));

        /* 等待所有线程退出 */
        int i;
        for (i = 0; i < th_pool->thread_num; ++i)
        {
                pthread_join(th_pool->threads[i], NULL);
        }

        /* 清理资源 */
        pthread_mutex_destroy(&(th_pool->mutex));
        pthread_cond_destroy(&(th_pool->queue_empty));
        pthread_cond_destroy(&(th_pool->queue_not_empty));
        pthread_cond_destroy(&(th_pool->queue_not_full));
        free(th_pool->threads);

        /* 清空等待队列 */
        task_t *free_task;
        while (th_pool->head != NULL)
        {
                free_task = th_pool->head;
                th_pool->head = th_pool->head->next;
                free(free_task);
        }

        free(th_pool);

        return 0;
}

#if          0

/* 线程池测试函数 */
void *work(void *arg)
{
        char *p = (char *)arg;
        printf("thread_pool callback function: %s.\n", p);
        sleep(1);
        return NULL;
}

int main(int argc, char *argv[])
{
        char *args[] = 
        {
                "1", "2", "3", "4", "5",
                "6", "7", "8", "9", "10"
        };

        thread_pool_t *thread_pool = thread_pool_init(1, 2);

        int i, tag, value;
        for (i = 0; i < 10; ++i)
        {
                do {
                        tag = thread_pool_add_task(thread_pool, work, args[i], true);

                        if (tag == 1)
                        {
                                value = thread_pool_resize(thread_pool, thread_pool->thread_num * 2, thread_pool->queue_max_num * 2);
                                if (value == -1) {
                                        printf("参数错误\n");
                                        exit(-1);
                                } else if (value == -2) {
                                        printf("申请内存错误\n");
                                        exit(-1);
                                } else if (value == -3) {
                                        printf("线程创建错误\n");
                                        exit(-1);
                                }
                        }
                } while (tag != 0);
        }

        sleep(2);
        thread_pool_destroy(thread_pool);

        return EXIT_SUCCESS;
}

#endif
