/*
 * =====================================================================================
 *
 *       Filename:  cli_head.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月26日 09时26分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#ifndef CLI_HEAD_H
#define CLI_HEAD_H

/* head file */

  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <string.h>
  #include <sys/epoll.h>
  #include <errno.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <stdbool.h>

/* macro */

  #define BUFFER_SIZE   1024
  #define MAX_EVENT_NUM 1024

#endif
