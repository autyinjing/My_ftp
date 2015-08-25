/*
 * =====================================================================================
 *
 *       Filename:  my_ftp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 14时28分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#include "my_ftp.h"

int main(int argc, char *argv[])
{
        ftp_init();

        /* 初始化网络模块 */
        if (argc <= 2)
        {
                printf("Usage: my_ftp ip port\n");
                exit(-1);
        }
	int sockfd = bind_socket(argv[1], atoi(argv[2]));
        int ret = listen(sockfd, 5);
        if (ret < 0)
        {
                fprintf(stderr, "listen error!\n");
                exit(1);
        }

        /* 信号模块 */
        handle_sig(SIGINT);
        handle_sig(SIGTERM);

        /* I/O复用模块 */
        struct epoll_event events[MAX_EVENT_NUM];
        add_event(g_epollfd, g_listenfd);

        /* 进入主循环 */
        do_epoll(g_epollfd, events, MAX_EVENT_NUM);

	return EXIT_SUCCESS;
}
