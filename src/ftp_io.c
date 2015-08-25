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

#include "ftp_io.h"
#include "ftp_net.h"

/* epoll主循环 */
void do_epoll(int epoll_fd, struct epoll_event *events, int maxnum)
{
        while ( 1 )
        {       /* 虽然主线程仅仅处理一个accept阻塞，但是为了扩展性，还是使用epoll_wait来处理 */
                int i;
                int number = epoll_wait(epollfd, events, maxnum, -1);
                handle_event(epollfd, number);
        }
}

/* 向epoll内核事件表中添加事件 */
void add_event(int epollfd, int fd)
{
        struct epoll_event ev;
        ev.data.fd = fd;
        event.events = EPOLLIN | EPOLLET;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
        setnonblocking(fd);
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
void handle_event(int epollfd, struct epoll_event *events, int number)
{
        int i;
        for (i = 0; i < number; ++i)
        {
                int fd = events[i].data.fd;
                if (fd == listenfd)
                {
                        handle_accept(listenfd);
                        /* 交给工作线程来处理 */
                }
        }
}
