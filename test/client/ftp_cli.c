/*
 * =====================================================================================
 *
 *       Filename:  ftp_cli.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月26日 09时25分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#include "cli_head.h"

int setnonblocking(int fd)
{
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);

        return old_option;
}

void addfd(int epollfd, int fd)
{
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
        setnonblocking(fd);
}

char *get_line(char *buf, int size)
{
        int ch, i = 0;

        while ((ch = getchar()) != '\n' && (i < size - 1))
        {
                buf[i++] = ch;
        }
        buf[i] = '\0';

        return buf;
}

int main(int argc, char *argv[])
{
	if (argc <= 2)
        {
                printf("Usage: ...\n");
                exit(-1);
        }

        const char *ip = argv[1];
        int port = atoi(argv[2]);

        struct sockaddr_in ser_addr;
        memset(&ser_addr, '\0', sizeof(ser_addr));
        ser_addr.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &ser_addr.sin_addr);
        ser_addr.sin_port = htons(port);

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
                printf("socket error!\n");
                exit(-1);
        }
        int connfd = connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
        if (connfd != 0) {
                printf("%s\n", strerror(errno));
                exit(1);
        } else {
                printf("connect success!\n");
        }

        struct epoll_event events[MAX_EVENT_NUM];
        int epollfd = epoll_create(5);
        if (epollfd < 0)
        {
                printf("epoll error!\n");
                exit(-1);
        }

        addfd(epollfd, sockfd);
        addfd(epollfd, STDIN_FILENO);

        bool stop_client = false;
        while ( !stop_client )
        {
                printf("enter the while()\n");
                int number = epoll_wait(epollfd, events, MAX_EVENT_NUM, -1);
                if (number < 0)
                {
                        printf("epoll_wait error!\n");
                        exit(-1);
                }
                
                printf("number = %d\n", number);
                int i;
                for (i = 0; i < number; ++i)
                {
                        int fd = events[i].data.fd;
                        printf("fd = %d\n", fd);
                        if ((fd == sockfd) && (events[i].events & EPOLLIN)) {
                                char recv_buf[BUFFER_SIZE];
                                memset(recv_buf, '\0', sizeof(recv_buf));
                                int ret = recv(connfd, recv_buf, BUFFER_SIZE-1, 0);
                                if (ret < 0) {
                                        if (errno == EAGAIN)
                                        {
                                                stop_client = true;
                                                break;
                                        }
                                } else if (ret == 0) {
                                        stop_client = true;
                                        break;
                                } else {
                                        printf("recv: %s\n", recv_buf);
                                }
                        } else if (fd == STDIN_FILENO) {
                                printf("stdin ready!\n");
                                char input[BUFFER_SIZE];
                                memset(input, '\0', BUFFER_SIZE);

                                get_line(input, BUFFER_SIZE);
                                //int ret = recv(fd, input, BUFFER_SIZE - 1, 0);
                                //printf("recv success, ret = %d\n", ret);
                                int ret = send(sockfd, input, strlen(input) + 1, 0);
                                if (ret < 0)
                                {
                                        printf("send error!\n");
                                        exit(1);
                                }
                                printf("send success, ret = %d\n", ret);
                        }
                }
        }

	return EXIT_SUCCESS;
}
