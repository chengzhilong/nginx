
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Maxim Dounin
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_MODULE_H_INCLUDED_
#define _NGX_MODULE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <nginx.h>


#define NGX_MODULE_UNSET_INDEX  (ngx_uint_t) -1


#define NGX_MODULE_SIGNATURE_0                                                \
    ngx_value(NGX_PTR_SIZE) ","                                               \
    ngx_value(NGX_SIG_ATOMIC_T_SIZE) ","                                      \
    ngx_value(NGX_TIME_T_SIZE) ","

#if (NGX_HAVE_KQUEUE)
#define NGX_MODULE_SIGNATURE_1   "1"
#else
#define NGX_MODULE_SIGNATURE_1   "0"
#endif

#if (NGX_HAVE_IOCP)
#define NGX_MODULE_SIGNATURE_2   "1"
#else
#define NGX_MODULE_SIGNATURE_2   "0"
#endif

#if (NGX_HAVE_FILE_AIO || NGX_COMPAT)
#define NGX_MODULE_SIGNATURE_3   "1"
#else
#define NGX_MODULE_SIGNATURE_3   "0"
#endif

#if (NGX_HAVE_AIO_SENDFILE || NGX_COMPAT)
#define NGX_MODULE_SIGNATURE_4   "1"
#else
#define NGX_MODULE_SIGNATURE_4   "0"
#endif

#if (NGX_HAVE_EVENTFD)
#define NGX_MODULE_SIGNATURE_5   "1"
#else
#define NGX_MODULE_SIGNATURE_5   "0"
#endif

#if (NGX_HAVE_EPOLL)
#define NGX_MODULE_SIGNATURE_6   "1"
#else
#define NGX_MODULE_SIGNATURE_6   "0"
#endif

#if (NGX_HAVE_KEEPALIVE_TUNABLE)
#define NGX_MODULE_SIGNATURE_7   "1"
#else
#define NGX_MODULE_SIGNATURE_7   "0"
#endif

#if (NGX_HAVE_INET6)
#define NGX_MODULE_SIGNATURE_8   "1"
#else
#define NGX_MODULE_SIGNATURE_8   "0"
#endif

#define NGX_MODULE_SIGNATURE_9   "1"
#define NGX_MODULE_SIGNATURE_10  "1"

#if (NGX_HAVE_DEFERRED_ACCEPT && defined SO_ACCEPTFILTER)
#define NGX_MODULE_SIGNATURE_11  "1"
#else
#define NGX_MODULE_SIGNATURE_11  "0"
#endif

#define NGX_MODULE_SIGNATURE_12  "1"

#if (NGX_HAVE_SETFIB)
#define NGX_MODULE_SIGNATURE_13  "1"
#else
#define NGX_MODULE_SIGNATURE_13  "0"
#endif

#if (NGX_HAVE_TCP_FASTOPEN)
#define NGX_MODULE_SIGNATURE_14  "1"
#else
#define NGX_MODULE_SIGNATURE_14  "0"
#endif

#if (NGX_HAVE_UNIX_DOMAIN)
#define NGX_MODULE_SIGNATURE_15  "1"
#else
#define NGX_MODULE_SIGNATURE_15  "0"
#endif

#if (NGX_HAVE_VARIADIC_MACROS)
#define NGX_MODULE_SIGNATURE_16  "1"
#else
#define NGX_MODULE_SIGNATURE_16  "0"
#endif

#define NGX_MODULE_SIGNATURE_17  "0"
#define NGX_MODULE_SIGNATURE_18  "0"

#if (NGX_HAVE_OPENAT)
#define NGX_MODULE_SIGNATURE_19  "1"
#else
#define NGX_MODULE_SIGNATURE_19  "0"
#endif

#if (NGX_HAVE_ATOMIC_OPS)
#define NGX_MODULE_SIGNATURE_20  "1"
#else
#define NGX_MODULE_SIGNATURE_20  "0"
#endif

#if (NGX_HAVE_POSIX_SEM)
#define NGX_MODULE_SIGNATURE_21  "1"
#else
#define NGX_MODULE_SIGNATURE_21  "0"
#endif

#if (NGX_THREADS || NGX_COMPAT)
#define NGX_MODULE_SIGNATURE_22  "1"
#else
#define NGX_MODULE_SIGNATURE_22  "0"
#endif

#if (NGX_PCRE)
#define NGX_MODULE_SIGNATURE_23  "1"
#else
#define NGX_MODULE_SIGNATURE_23  "0"
#endif

#if (NGX_HTTP_SSL || NGX_COMPAT)
#define NGX_MODULE_SIGNATURE_24  "1"
#else
#define NGX_MODULE_SIGNATURE_24  "0"
#endif

#define NGX_MODULE_SIGNATURE_25  "1"

#if (NGX_HTTP_GZIP)
#define NGX_MODULE_SIGNATURE_26  "1"
#else
#define NGX_MODULE_SIGNATURE_26  "0"
#endif

#define NGX_MODULE_SIGNATURE_27  "1"

#if (NGX_HTTP_X_FORWARDED_FOR)
#define NGX_MODULE_SIGNATURE_28  "1"
#else
#define NGX_MODULE_SIGNATURE_28  "0"
#endif

#if (NGX_HTTP_REALIP)
#define NGX_MODULE_SIGNATURE_29  "1"
#else
#define NGX_MODULE_SIGNATURE_29  "0"
#endif

#if (NGX_HTTP_HEADERS)
#define NGX_MODULE_SIGNATURE_30  "1"
#else
#define NGX_MODULE_SIGNATURE_30  "0"
#endif

#if (NGX_HTTP_DAV)
#define NGX_MODULE_SIGNATURE_31  "1"
#else
#define NGX_MODULE_SIGNATURE_31  "0"
#endif

#if (NGX_HTTP_CACHE)
#define NGX_MODULE_SIGNATURE_32  "1"
#else
#define NGX_MODULE_SIGNATURE_32  "0"
#endif

#if (NGX_HTTP_UPSTREAM_ZONE)
#define NGX_MODULE_SIGNATURE_33  "1"
#else
#define NGX_MODULE_SIGNATURE_33  "0"
#endif

#if (NGX_COMPAT)
#define NGX_MODULE_SIGNATURE_34  "1"
#else
#define NGX_MODULE_SIGNATURE_34  "0"
#endif

#define NGX_MODULE_SIGNATURE                                                  \
    NGX_MODULE_SIGNATURE_0 NGX_MODULE_SIGNATURE_1 NGX_MODULE_SIGNATURE_2      \
    NGX_MODULE_SIGNATURE_3 NGX_MODULE_SIGNATURE_4 NGX_MODULE_SIGNATURE_5      \
    NGX_MODULE_SIGNATURE_6 NGX_MODULE_SIGNATURE_7 NGX_MODULE_SIGNATURE_8      \
    NGX_MODULE_SIGNATURE_9 NGX_MODULE_SIGNATURE_10 NGX_MODULE_SIGNATURE_11    \
    NGX_MODULE_SIGNATURE_12 NGX_MODULE_SIGNATURE_13 NGX_MODULE_SIGNATURE_14   \
    NGX_MODULE_SIGNATURE_15 NGX_MODULE_SIGNATURE_16 NGX_MODULE_SIGNATURE_17   \
    NGX_MODULE_SIGNATURE_18 NGX_MODULE_SIGNATURE_19 NGX_MODULE_SIGNATURE_20   \
    NGX_MODULE_SIGNATURE_21 NGX_MODULE_SIGNATURE_22 NGX_MODULE_SIGNATURE_23   \
    NGX_MODULE_SIGNATURE_24 NGX_MODULE_SIGNATURE_25 NGX_MODULE_SIGNATURE_26   \
    NGX_MODULE_SIGNATURE_27 NGX_MODULE_SIGNATURE_28 NGX_MODULE_SIGNATURE_29   \
    NGX_MODULE_SIGNATURE_30 NGX_MODULE_SIGNATURE_31 NGX_MODULE_SIGNATURE_32   \
    NGX_MODULE_SIGNATURE_33 NGX_MODULE_SIGNATURE_34


#define NGX_MODULE_V1                                                         \
    NGX_MODULE_UNSET_INDEX, NGX_MODULE_UNSET_INDEX,                           \
    NULL, 0, 0, nginx_version, NGX_MODULE_SIGNATURE

#define NGX_MODULE_V1_PADDING  0, 0, 0, 0, 0, 0, 0, 0

/* 所有模块的通用接口：只定义了7个回调方法——init_master, init_module, init_process, init_thread,
 * exit_thread, exit_process, exit_master.
 */
struct ngx_module_s {
    /* 对于一类模块（由type成员决定类型）而言，ctx_index表示当前模块在这类模块中的序号 */
    ngx_uint_t            ctx_index;
    ngx_uint_t            index;  /* 当前模块在所有模块中的序号，也即在ngx_modules数组中的序号 */

    char                 *name;

    ngx_uint_t            spare0;
    ngx_uint_t            spare1;

    ngx_uint_t            version;  /* 模块的版本，便于将来的扩展 */
    const char           *signature;

    /* 指向一类模块的上下文结构体. 不同类模块的功能差异大，ctx会指向特定类型模块的公共接口 */
    void                 *ctx;
    /* 处理nginx.conf中的配置项 */
    ngx_command_t        *commands;
    /* 模块类型，与ctx成员紧密相连.官方取值有如下五种值:NGX_HTTP|CORE|CONF|EVENT|MALL_MODULE，也可自定义其他类型 */
    ngx_uint_t            type;

    /* Nginx启停过程中，以下7个函数指针表示有7个执行点会分别调用这7种方法.
     * 对于任一方法而言，若不需要Nginx在某个时刻执行它，那么就简单把它设置为NULL空指针即可.
     */
    ngx_int_t           (*init_master)(ngx_log_t *log);  /* 目前为止，框架代码未使用它 */

    ngx_int_t           (*init_module)(ngx_cycle_t *cycle); /* 初始化所有模块时调用. 在master/worker模式下，在启动worker进程前完成 */

    ngx_int_t           (*init_process)(ngx_cycle_t *cycle); /* 正常服务前被调用. 每个worker进程初始化过程会调用所有模块的init_process函数 */
    ngx_int_t           (*init_thread)(ngx_cycle_t *cycle); /* Nginx暂时不支持多线程模式，因此init_thread暂未被调用. */
    void                (*exit_thread)(ngx_cycle_t *cycle);
    void                (*exit_process)(ngx_cycle_t *cycle); /* worker进程退出前调用它 */

    void                (*exit_master)(ngx_cycle_t *cycle); /* master进程退出前调用它 */

    /* 保留字段，暂时未使用 */
    uintptr_t             spare_hook0;
    uintptr_t             spare_hook1;
    uintptr_t             spare_hook2;
    uintptr_t             spare_hook3;
    uintptr_t             spare_hook4;
    uintptr_t             spare_hook5;
    uintptr_t             spare_hook6;
    uintptr_t             spare_hook7;
};

/* 核心模块接口，其模块类型为NGX_CORE_MODULE，
 * 其作用是可以简化Nginx的设计，使得非模块化的框架代码只关注于如何调用核心模块。
 * 而大部分Nginx模块都是非核心模块
 */
typedef struct {
    ngx_str_t             name;
    void               *(*create_conf)(ngx_cycle_t *cycle); /* 解析配置项前调用的方法，创建存储配置项的数据结构 */
    char               *(*init_conf)(ngx_cycle_t *cycle, void *conf);   /* 解析配置项完成后，Nginx会调用init_conf方法 */
} ngx_core_module_t;


ngx_int_t ngx_preinit_modules(void);
ngx_int_t ngx_cycle_modules(ngx_cycle_t *cycle);
ngx_int_t ngx_init_modules(ngx_cycle_t *cycle);
ngx_int_t ngx_count_modules(ngx_cycle_t *cycle, ngx_uint_t type);


ngx_int_t ngx_add_module(ngx_conf_t *cf, ngx_str_t *file,
    ngx_module_t *module, char **order);


extern ngx_module_t  *ngx_modules[];
extern ngx_uint_t     ngx_max_module;

extern char          *ngx_module_names[];


#endif /* _NGX_MODULE_H_INCLUDED_ */
