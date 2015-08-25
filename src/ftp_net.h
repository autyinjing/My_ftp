/*
 * =====================================================================================
 *
 *       Filename:  ftp_net.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 13时43分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */


#ifndef FTP_NET_H
#define FTP_NET_H

/* head file */

  #include <sys/types.h>
  #include <sys/socket.h>
  #include <string.h>
  #include <arpa/inet.h>
  #include <stdlib.h>
  #include <stdio.h>

/* global var */

  extern int g_listenfd; /* 监听新连接的socket描述符 */

/* function */

  extern int bind_socket(const char *ip, int port);
  extern void handle_accept(int listenfd);

#endif  /* end of FP_NET_H */
