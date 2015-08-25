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

#include <stdio.h>
#include <stdlib.h>

#include "ftp_net.h"

int main(int argc, char *argv[])
{
	int sockfd = bind_socket(argv[1], atoi(argv[2]));
        int ret = listen(sockfd, 5);
        if (ret < 0)
        {
                fprintf(stderr, "listen error!\n");
                exit(1);
        }
        int connfd = handle_accept(sockfd);
        printf("%d\n", connfd);

	return EXIT_SUCCESS;
}

