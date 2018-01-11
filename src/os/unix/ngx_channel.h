
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CHANNEL_H_INCLUDED_
#define _NGX_CHANNEL_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


/* 进程间通信是利用socketpair创建的一对socket进行的，通信中传输的是ngx_channel_t结构变量 */
typedef struct {
	/* 要发送的命令，有5种：NGX_CMD_(OPEN_CHANNEL, CLOSE_CHANNEL, QUIT, TERMINATE, REOPEN) */
    ngx_uint_t  command;
    ngx_pid_t   pid;			/* 发送方进程id */
    ngx_int_t   slot;			/* 发送方进程表中偏移 */
    ngx_fd_t    fd;				/* 发送给对方的句柄 */
} ngx_channel_t;


ngx_int_t ngx_write_channel(ngx_socket_t s, ngx_channel_t *ch, size_t size,
    ngx_log_t *log);
ngx_int_t ngx_read_channel(ngx_socket_t s, ngx_channel_t *ch, size_t size,
    ngx_log_t *log);
ngx_int_t ngx_add_channel_event(ngx_cycle_t *cycle, ngx_fd_t fd,
    ngx_int_t event, ngx_event_handler_pt handler);
void ngx_close_channel(ngx_fd_t *fd, ngx_log_t *log);


#endif /* _NGX_CHANNEL_H_INCLUDED_ */
