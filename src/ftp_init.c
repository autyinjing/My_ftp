/*
 * =====================================================================================
 *
 *       Filename:  ftp_init.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 21时58分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#include "my_ftp.h"

/* 宏定义 */
#define MAX_USER_NUM    64

/* 初始化 */
void ftp_init()
{
        //daemon(0, 0);         /* 将服务器程序后台化 */
        
        /* 初始化工作线程模块 */
        g_user_count = 0;
        g_users = (user_t *)malloc(sizeof(user_t) * MAX_USER_NUM);
        if (g_users == NULL)
        {
                printf("malloc error!\n");
                exit(-1);
        }
        if (pthread_mutex_init(&g_user_mutex, NULL))
        {
                printf("pthread_mutex_init error!\n");
                exit(-1);
        }

        /* I/O模块 */
        g_epollfd = epoll_create(5);
        if (g_epollfd < 0)
        {
                printf("epoll_create error!\n");
                exit(-1);
        }

        /* 初始化日志模块 */
        open_log();

        /* 线程池模块 */
        /*  thread_num = 1;
        queue_num = 2;
        if (pthread_mutex_init(&num_mutex))
        {
                printf("pthread_mutex_init error!\n");
                exit(-1);
        }*/
        g_thread_pool = thread_pool_init(1, 2);
        if (g_thread_pool == NULL)
        {
                printf("thread_pool_init error!\n");
                exit(-1);
        }

        /* 网络模块包含返回值，要在主程序中初始化 */
}

/* 释放资源,结束程序 */
void del_resource()
{
        /* 网络模块 */
        close(g_listenfd);

        /* 工作线程模块 */
        free(g_users);
        g_users = NULL;
        pthread_mutex_destroy(&g_user_mutex);

        /* I/O复用模块 */
        close(g_epollfd);

        /* 日志模块 */
        close_log();

        /* 线程池模块 */
        //pthread_mutex_destroy(&num_mutex);
        thread_pool_destroy(g_thread_pool);

        exit(0);
}
