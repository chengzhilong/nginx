
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_PROCESS_H_INCLUDED_
#define _NGX_PROCESS_H_INCLUDED_


#include <ngx_setaffinity.h>
#include <ngx_setproctitle.h>


typedef pid_t       ngx_pid_t;

#define NGX_INVALID_PID  -1

typedef void (*ngx_spawn_proc_pt) (ngx_cycle_t *cycle, void *data);

/* master每创建一个worker都会把一个设置好的ngx_process_t结构变量放入ngx_processes中，进程表
 * 长度为1024，刚创建的进程存放在ngx_process_slot位置，ngx_last_process是进程表中最后一个存
 * 量进程的下一个位置，ngx_process_t是进程在nginx中的抽象。
 */
typedef struct {
    ngx_pid_t           pid;				/* 进程ID */
    int                 status;				/* 进程状态 */
    ngx_socket_t        channel[2];			/* socketpair创建的socket句柄 */

    ngx_spawn_proc_pt   proc;				/* 进程执行函数 */
    void               *data;				/* 执行函数的参数 */
    char               *name;				/* 名称 */

    unsigned            respawn:1;			/* 重新创建 */
    unsigned            just_spawn:1;		/* 第一次创建的 */
    unsigned            detached:1;			/* 分离的 */
    unsigned            exiting:1;			/* 正在退出的 */
    unsigned            exited:1;			/* 退出过的 */
} ngx_process_t;


typedef struct {
    char         *path;
    char         *name;
    char *const  *argv;
    char *const  *envp;
} ngx_exec_ctx_t;


#define NGX_MAX_PROCESSES         1024

#define NGX_PROCESS_NORESPAWN     -1
#define NGX_PROCESS_JUST_SPAWN    -2
#define NGX_PROCESS_RESPAWN       -3
#define NGX_PROCESS_JUST_RESPAWN  -4
#define NGX_PROCESS_DETACHED      -5


#define ngx_getpid   getpid

#ifndef ngx_log_pid
#define ngx_log_pid  ngx_pid
#endif


ngx_pid_t ngx_spawn_process(ngx_cycle_t *cycle,
    ngx_spawn_proc_pt proc, void *data, char *name, ngx_int_t respawn);
ngx_pid_t ngx_execute(ngx_cycle_t *cycle, ngx_exec_ctx_t *ctx);
ngx_int_t ngx_init_signals(ngx_log_t *log);
void ngx_debug_point(void);


#if (NGX_HAVE_SCHED_YIELD)
#define ngx_sched_yield()  sched_yield()
#else
#define ngx_sched_yield()  usleep(1)
#endif


extern int            ngx_argc;
extern char         **ngx_argv;
extern char         **ngx_os_argv;

extern ngx_pid_t      ngx_pid;
extern ngx_socket_t   ngx_channel;
extern ngx_int_t      ngx_process_slot;
extern ngx_int_t      ngx_last_process;
extern ngx_process_t  ngx_processes[NGX_MAX_PROCESSES];


#endif /* _NGX_PROCESS_H_INCLUDED_ */
