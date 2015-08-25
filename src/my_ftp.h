/*
 * =====================================================================================
 *
 *       Filename:  my_ftp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 19时56分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#ifndef MY_FTP_H
#define MY_FTP_H

/* head file */

  #include "ftp_net.h"
  #include "ftp_io.h"
  #include "ftp_log.h"
  #include "ftp_sig.h"
  #include "ftp_thread_pool.h"
  #include "ftp_work.h"
  #include "ftp_init.h"

/* macro */

//  #define MAX_USER_NUM  64
  #define MAX_EVENT_NUM 64

//  #define RUNLOG 0
//  #define ERRLOG 1
//  #define RUN_LOG_PATH "~/.my_ftp.log/ftp_run.log"
//  #define ERR_LOG_PATH "~/.my_ftp.log/ftp_err.log"

/* global var */

/*    int run_log_fd;
  int err_log_fd;
  int g_epollfd;
  int g_listenfd;
  thread_pool_t *g_thread_pool = NULL;
  user_t *g_users = NULL;
  int g_user_count = 0;
  pthread_mutex_t g_user_mutex; */

#endif  /* end of MY_FTP_H */
