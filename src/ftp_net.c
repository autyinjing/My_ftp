/*
 * =====================================================================================
 *
 *       Filename:  ftp_net.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 13时52分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#include "my_ftp.h"

/* 全局变量定义 */
int g_listenfd;

/* 创建socket并绑定到指定地址，返回绑定以后的socket描述符 */
int bind_socket(const char *ip, int port)
{
        struct sockaddr_in addr;
        
        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &addr.sin_addr);
        addr.sin_port = htons(port);

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
                fprintf(stderr, "file: %s, line: %d, error: socket() error!\n",
                                __FILE__, __LINE__);
                exit(1);
        }

        int ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0)
        {
                fprintf(stderr, "file: %s, line: %d, error: bind() error", __FILE__, __LINE__);
                exit(1);
        }

        return sockfd;
}

/* 接受新的连接 */
void handle_accept(int listenfd)
{
        struct sockaddr_in cli_addr;

        memset(&cli_addr, '\0', sizeof(cli_addr));
        socklen_t cli_len = sizeof(cli_addr);

        int connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &cli_len);
        if (connfd < 0)
        {
                fprintf(stderr, "accept error!\n");
                exit(1);
        }

        pthread_mutex_lock(&g_user_mutex);
        g_users[g_user_count].ctl_addr = cli_addr;
        g_users[g_user_count].ctl_fd = connfd;

        //++user_count;
        pthread_mutex_unlock(&g_user_mutex);
}
