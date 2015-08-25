/*
 * =====================================================================================
 *
 *       Filename:  ftp_log.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月25日 16时02分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  aut (yinjing), autyinjing@126.com
 *        Company:  Information and Computing Science 1201
 *
 * =====================================================================================
 */

#include "ftp_log.h"

/* 打开日志 */
void open_log()
{
        /* 打开运行日志文件 */
        run_log_fd = open(run_log_path, O_WRONLY);
        if (run_log_fd < 0)
        {
                printf("open error!\n");
                exit(1);
        }

        /* 打开错误日志文件 */
        err_log_fd = open(err_log_path, O_WRONLY);
        if (err_log_fd < 0)
        {
                printf("open error!\n");
                exit(1);
        }
}

/* 记录日志 */
void ftp_log(const char *log, int logtype)
{
        int ret;
        int fd;
        char write_buf[256];

        memset(write_buf, '\0', sizeof(write_buf));
        switch (logtype)
        {
                case RUNLOG:
                        /* 运行日志格式：日期 时间 事件 */
                        sprintf(write_buf, "%s %s: %s\n", __DATE__, __TIME__, log);
                        fd = run_log_fd;
                        break;

                case ERRLOG:
                        /* 错误日志格式：日期 时间 文件 行数 事件 */
                        sprintf(write_buf, "%s %s %s %d: %s\n", __DATE__, __TIME__, __FILE__, __LINE__, log);
                        fd = err_log_fd;
                        break;

                default:
                        printf("参数错误！\n");
        }

        ret = write(fd, write_buf, strlen(write_buf));
        if (ret < 0)
        {
                printf("write error!\n");
                exit(1);
        }
}

/* 关闭日志 */
void close_log()
{
        close(run_log_fd);
        close(err_log_fd);
}
