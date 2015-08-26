/*
 * =====================================================================================
 *
 *       Filename:  ftp_io.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 14时45分48秒
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
//#define MAX_EVENT_NUM   64

/* 全局变量定义 */
int g_epollfd;

/* epoll主循环 */
void do_epoll(int epoll_fd, struct epoll_event *events, int maxnum)
{
        while ( 1 )
        {       /* 虽然主线程仅仅处理一个accept阻塞，但是为了扩展性，还是使用epoll_wait来处理 */
                //printf("Enter do_epoll->while ()\n");
                int number = epoll_wait(epoll_fd, events, maxnum, -1);
                handle_event(epoll_fd, events, number);
                //printf("In do_epoll(), number = %d\n", number);
        }
}

/* 向epoll内核事件表中添加事件 */
void add_event(int epoll_fd, int fd)
{
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
        set_nonblocking(fd);
}

/* 设置描述符为非阻塞 */
int set_nonblocking(int fd)
{
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        
        fcntl(fd, F_SETFL, new_option);
        
        return old_option;
}

/* 处理epoll事件 */
void handle_event(int epoll_fd, struct epoll_event *events, int number)
{
        int i;
        for (i = 0; i < number; ++i)
        {
                int fd = events[i].data.fd;
                if (fd == g_listenfd)
                {
                        handle_accept(g_listenfd);
                        
                        /* 交给工作线程来处理 */
                        int ret, value;
                        do {
                                ret = thread_pool_add_task(g_thread_pool, work, (void *)&g_user_count, false);
                                //应该在工作线程中改变user_count的值
                               // pthread_mutex_lock(&g_user_mutex);
                               // ++g_user_count;
                               // pthread_mutex_unlock(&g_user_mutex);
                                //printf("ret = %d\n", ret);
                                if (ret == 1)
                                {
                                        value = thread_pool_resize(g_thread_pool, g_thread_pool->thread_num * 2, g_thread_pool->queue_max_num * 2);
                                        //printf("value = %d\n", value);
                                        if (value == -1) {
                                                printf("参数错误！\n");
                                                exit(-1);
                                        } else if (value == -2) {
                                                printf("申请内存错误！\n");
                                                exit(-1);
                                        } else if (value == -3) {
                                                printf("线程创建错误！\n");
                                                exit(-1);
                                        }
                                }
                        } while (ret != 0);
                }
        }
}
