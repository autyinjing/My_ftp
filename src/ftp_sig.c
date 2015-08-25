/*
 * =====================================================================================
 *
 *       Filename:  ftp_sig.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 15时43分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#include "my_ftp.h"

/* 信号处理函数，仅仅处理SIGTERM信号和SIGINT信号，收到信号后做清理工作并结束进程 */
void handle_sig(int sig)
{
        struct sigaction sa;
        
        memset(&sa, '\0', sizeof(sa));
        sa.sa_handler = del_resource;
        sa.sa_flags |= SA_RESTART;
        sigfillset(&sa.sa_mask);
        if (sigaction(sig, &sa, NULL) < 0)
        {
                printf("sigaction error!\n");
                exit(1);
        }
}
