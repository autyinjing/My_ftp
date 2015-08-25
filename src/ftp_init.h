/*
 * =====================================================================================
 *
 *       Filename:  ftp_init.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 21时56分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#ifndef FTP_INIT_H
#define FTP_INIT_H

/* head file */

  #include <stdlib.h>
  #include <pthread.h>
  #include <unistd.h>

/* macro */

//  #define MAX_USER_NUM          64      /* 最大用户数 */
//  #define MAX_EVENT_NUM         64      /* epoll最大事件数 */

/* global var */

//  int thread_num;       /* 线程数量 */
//  int queue_num;        /* 等待队列的长度 */
//  pthread_mutex_t num_mutex;    /* 互斥上面两个变量的写操作 */

/* function */

  extern void ftp_init();
  extern void del_resource();

#endif  /* end of FTP_INIT_H */
