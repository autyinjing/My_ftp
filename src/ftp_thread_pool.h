/*
 * =====================================================================================
 *
 *       Filename:  ftp_thread_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 16时53分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#ifndef FTP_THREAD_POOL_H
#define FTP_THREAD_POOL_H

/* head file */

  #include <pthread.h>
  #include <stdbool.h>
  #include <stdlib.h>

/* data structure */

typedef struct task
{
        void *(*callback)(void *);      /* 任务回调函数 */
        void *arg;                      /* 回调函数的参数 */
        struct task *next;              /* 指向下一个任务 */
}task_t;        /* 任务数据结构 */

typedef struct thread_pool
{
        int thread_num;         /* 已创建的线程总个数 */
        int free_thread_num;    /* 空闲线程个数 */
        int queue_max_num;      /* 最大任务个数 */
        int queue_curr_num;     /* 当前任务个数 */

        task_t *head;           /* 任务头指针 */
        task_t *tail;           /* 任务尾指针 */
        pthread_t *threads;     /* 保存线程ID */
        pthread_mutex_t mutex;  /* 互斥信号量 */
        pthread_cond_t queue_empty;     /* 队列为空的条件变量 */
        pthread_cond_t queue_not_empty; /* 任务非空的条件变量 */
        pthread_cond_t queue_not_full;  /* 任务队列不再满的条件变量 */
        
        bool resize_now;        /* 标记是否刚调整过大小 */
        bool queue_close;       /* 任务队列是否关闭 */
        bool pool_close;        /* 线程池是否关闭 */
}thread_pool_t;         /* 线程池数据结构 */

/* global var */

  thread_pool_t *thread_pool = NULL;    /* 全局线程池指针 */

/* function */

  extern thread_pool_t *thread_pool_init(int thread_num, int queue_max_num);
  extern int thread_pool_resize(thread_pool_t *thread_pool, int thread_num, int queue_max_num);
  extern int thread_pool_add_task(thread_pool_t *thread_pool, void *(*callback)(void *), void *arg, bool is_block);
  extern void *thread_function(void *arg);
  extern int thread_pool_destroy(thread_pool_t *thread_pool);

#endif  /* end of FTP_THREAD_POOL_H */
