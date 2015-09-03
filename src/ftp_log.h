/*
 * =====================================================================================
 *
 *       Filename:  ftp_log.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 16时01分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#ifndef FTP_LOG_H
#define FTP_LOG_H

/* head file */

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <stdio.h>
  #include <stdlib.h>

/* macro */

#ifndef LOG_MACRO
  #define RUNLOG 0
  #define ERRLOG 1
  #define RUN_LOG_PATH "/home/honey/.my_ftp.log/ftp_run.log"
  #define ERR_LOG_PATH "/home/honey/.my_ftp.log/ftp_err.log"
#endif

/* global var */

  extern int run_log_fd;       /* 运行日志的文件描述符 */
  extern int err_log_fd;       /* 错误日志的文件描述符 */

/* function */

  extern void open_log();
  extern void ftp_log(const char *log, int logtype);
  extern void close_log();

#endif /* end of FTP_LOG_H */
