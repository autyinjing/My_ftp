/*
 * =====================================================================================
 *
 *       Filename:  ftp_work.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 19时33分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#ifndef FTP_WORK_H
#define FTP_WORK_H

/* head file */

  #include <sys/types.h>
  #include <sys/socket.h>
  #include <stdio.h>
  #include <pthread.h>
  #include <errno.h>

/* macro */

//  #define MAX_EVENT_NUM 64      /* epoll最大事件个数 */

/* data structure */

typedef struct user
{
        struct sockaddr_in ctl_addr;    /* 控制连接的地址 */
        struct sockaddr_in data_addr;   /* 数据连接的地址 */
        int ctl_fd;     /* 控制连接的socket描述符 */
        int data_fd;    /* 数据连接的socket描述符 */
}user_t;        /* 保存已连接用户信息的数据结构 */

/* global var */

  extern user_t *g_users; /* 保存已建立连接的用户信息 */
  extern int g_user_count;/* 已建立连接的用户数量 */
  extern pthread_mutex_t g_user_mutex;   /* 用于互斥user_count的写操作 */

/* function */

  extern void *work(void *arg);
  extern int do_cmd(const char *buf, int no);
  extern void list(const user_t *users, int no, const char *pathname);
  extern void retr(const user_t *users, int no, const char *filename);
  extern void stor(const user_t *users, int no, const char *filename);
  extern void quit();

#endif  /* end of FTP_WORK_H */
