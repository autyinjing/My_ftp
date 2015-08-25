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
  
  #define RUNLOG 0
  #define ERRLOG 1

/* global var */

  const char *run_log_path = "~/.my_ftp.log/ftp_run.log";/* 运行日志的文件路径 */
  const char *err_log_path = "~/.my_ftp.log/ftp_err.log";/* 错误文件的文件路径 */
  int run_log_fd;       /* 运行日志的文件描述符 */
  int err_log_fd;       /* 错误日志的文件描述符 */

/* function */

  extern void open_log();
  extern void ftp_log(const char *log, int logtype);
  extern void close_log();

#endif /* end of FTP_LOG_H */
