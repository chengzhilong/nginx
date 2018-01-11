
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CHANNEL_H_INCLUDED_
#define _NGX_CHANNEL_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


/* ���̼�ͨ��������socketpair������һ��socket���еģ�ͨ���д������ngx_channel_t�ṹ���� */
typedef struct {
	/* Ҫ���͵������5�֣�NGX_CMD_(OPEN_CHANNEL, CLOSE_CHANNEL, QUIT, TERMINATE, REOPEN) */
    ngx_uint_t  command;
    ngx_pid_t   pid;			/* ���ͷ�����id */
    ngx_int_t   slot;			/* ���ͷ����̱���ƫ�� */
    ngx_fd_t    fd;				/* ���͸��Է��ľ�� */
} ngx_channel_t;


ngx_int_t ngx_write_channel(ngx_socket_t s, ngx_channel_t *ch, size_t size,
    ngx_log_t *log);
ngx_int_t ngx_read_channel(ngx_socket_t s, ngx_channel_t *ch, size_t size,
    ngx_log_t *log);
ngx_int_t ngx_add_channel_event(ngx_cycle_t *cycle, ngx_fd_t fd,
    ngx_int_t event, ngx_event_handler_pt handler);
void ngx_close_channel(ngx_fd_t *fd, ngx_log_t *log);


#endif /* _NGX_CHANNEL_H_INCLUDED_ */
