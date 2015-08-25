/*
 * =====================================================================================
 *
 *       Filename:  ftp_work.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 19时51分02秒
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
user_t *g_users = NULL;
int g_user_count = 0;
pthread_mutex_t g_user_mutex;

/* 工作线程的工作函数，即每个用户的控制连接 */
void *work(void *arg)
{
        int no = *(int *)arg;
        bool stop_thread = false;

        int epoll_fd = epoll_create(5);
        if (epoll_fd < 0)
        {
                printf("epoll failure\n");
                exit(1);
        }

        struct epoll_event th_events[MAX_EVENT_NUM];
        add_event(epoll_fd, g_users[no].ctl_fd);

        while ( !stop_thread )
        {
                int number = epoll_wait(epoll_fd, th_events, MAX_EVENT_NUM, -1);
                int i;
                for (i = 0; i < number; ++i)
                {
                        int fd = th_events[i].data.fd;
                        if ((fd == g_users[no].ctl_fd) && (EPOLLIN & th_events[i].events)) {
                                char recv_buf[64];
                                memset(recv_buf, '\0', sizeof(recv_buf));
                                int ret = recv(fd, recv_buf, 63, 0);
                                if (ret < 0) {
                                        if (errno != EAGAIN)
                                        {
                                                stop_thread = true;
                                                break;
                                        }
                                } else if (ret == 0) {
                                        stop_thread = true;
                                        break;
                                } else {
                                        ret = do_cmd(recv_buf, no);
                                        if (ret == 1) {         /* 客户端请求断开 */
                                                stop_thread = true;
                                                break;
                                        }
                                }
                        }
                }
        }

        close(g_users[no].ctl_fd);
        pthread_mutex_lock(&g_user_mutex);
        --g_user_count;
        pthread_mutex_unlock(&g_user_mutex);

        return NULL;
}

/* 执行命令 */
int do_cmd(const char *buf, int no)
{
        if (strcmp(buf, "list") == 0) {
                list(g_users, no, NULL);
        } else if (strcmp(buf, "retr") == 0) {
                retr(g_users, no, NULL);
        } else if (strcmp(buf, "stor") == 0) {
                stor(g_users, no, NULL);
        } else if (strcmp(buf, "quit") == 0) {
                return 1;
        }

        return 0;
}

/* list命令 */
void list(const user_t *users, int no, const char *pathname)
{
        printf("In function 'list()'\n");
}

/* retr命令 */
void retr(const user_t *users, int no, const char *filename)
{
        printf("In function 'retr()'\n");
}

/* stor命令 */
void stor(const user_t *users, int no, const char *filename)
{
        printf("In function 'stor()'\n");
}

/* quit命令 */
void quit()
{
        printf("In function 'quit'\n");
}
