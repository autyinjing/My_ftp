/*
 * =====================================================================================
 *
 *       Filename:  ftp_io.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 14时42分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#ifndef FTP_IO_H
#define FTP_IO_H

/* head file */

  #include <sys/epoll.h>
  #include <unistd.h>
  #include <fcntl.h>

/* function */

  extern void do_epoll(int epollfd, struct epoll_event *events, int maxnum);
  extern void add_event(int epollfd, int fd);
  extern int set_nonblocking(int fd);
  extern void handle_event(int epollfd, struct epoll_event *events, int number);

#endif /* end of FTP_IO_H */
