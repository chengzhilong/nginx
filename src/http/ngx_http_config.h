
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HTTP_CONFIG_H_INCLUDED_
#define _NGX_HTTP_CONFIG_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    void        **main_conf;
    void        **srv_conf;
    void        **loc_conf;
} ngx_http_conf_ctx_t;


typedef struct {
    ngx_int_t   (*preconfiguration)(ngx_conf_t *cf);		/* 解析配置文件之前被调用 */
    ngx_int_t   (*postconfiguration)(ngx_conf_t *cf);		/* 解析配置文件之后被调用 */

    /* 直属于http {...} 块的全局配置项 */
    void       *(*create_main_conf)(ngx_conf_t *cf);		/* 调用该函数创建本模块位于http block的配置信息存储结构 */
    char       *(*init_main_conf)(ngx_conf_t *cf, void *conf);	/* 调用该函数初始化本模块位于http block的配置信息存储结构 */

    /* 直属虚拟主机server {...}块的配置项 */
    void       *(*create_srv_conf)(ngx_conf_t *cf);			/* 创建本模块位于http server        block的配置信息存储结构*/
    char       *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);	/* 若http block中配置与server block中配置信息发生冲突时，需要调用该函数进行合并      */

    /* 直属于location{ ... } 块的配置项 */
    void       *(*create_loc_conf)(ngx_conf_t *cf);			/* 调用该函数创建本模块位于location block的配置信息存储结构 */
    /* 主要用于合并srv级别和loc级别下的同名配置项 */
    char       *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);	/* 与merge_srv_conf类似，也是进行配置值合并的地方 */
} ngx_http_module_t;


#define NGX_HTTP_MODULE           0x50545448   /* "HTTP" */

/* 配置项可以出现在http{}块内 */
#define NGX_HTTP_MAIN_CONF        0x02000000
/* 配置项可以出现在server{}块内，然而该server块必须属于http{}块内 */
#define NGX_HTTP_SRV_CONF         0x04000000
/* 配置项可以出现在location{}块内，然而该location块必须属于http{}块内 */
#define NGX_HTTP_LOC_CONF         0x08000000
/* 配置项可以出现在upstream{}块内，然而该upstream块必须属于http{}块内 */
#define NGX_HTTP_UPS_CONF         0x10000000
/* 配置项可以出现在server块内的if{}块内。
 * 目前仅有rewrite模块会使用，该if块必须属于http{}块
 */
#define NGX_HTTP_SIF_CONF         0x20000000
/* 配置项可以出现在location块内的if{}块中。
 * 目前仅有rewrite模块会使用，该if块必须属于http{}块
 */
#define NGX_HTTP_LIF_CONF         0x40000000
/* 配置项可以出现在limit_except{}块内，然而该limit_except块必须属于http{}块 */
#define NGX_HTTP_LMT_CONF         0x80000000


#define NGX_HTTP_MAIN_CONF_OFFSET  offsetof(ngx_http_conf_ctx_t, main_conf)
#define NGX_HTTP_SRV_CONF_OFFSET   offsetof(ngx_http_conf_ctx_t, srv_conf)
#define NGX_HTTP_LOC_CONF_OFFSET   offsetof(ngx_http_conf_ctx_t, loc_conf)


#define ngx_http_get_module_main_conf(r, module)                             \
    (r)->main_conf[module.ctx_index]
#define ngx_http_get_module_srv_conf(r, module)  (r)->srv_conf[module.ctx_index]
#define ngx_http_get_module_loc_conf(r, module)  (r)->loc_conf[module.ctx_index]


#define ngx_http_conf_get_module_main_conf(cf, module)                        \
    ((ngx_http_conf_ctx_t *) cf->ctx)->main_conf[module.ctx_index]
#define ngx_http_conf_get_module_srv_conf(cf, module)                         \
    ((ngx_http_conf_ctx_t *) cf->ctx)->srv_conf[module.ctx_index]
#define ngx_http_conf_get_module_loc_conf(cf, module)                         \
    ((ngx_http_conf_ctx_t *) cf->ctx)->loc_conf[module.ctx_index]

#define ngx_http_cycle_get_module_main_conf(cycle, module)                    \
    (cycle->conf_ctx[ngx_http_module.index] ?                                 \
        ((ngx_http_conf_ctx_t *) cycle->conf_ctx[ngx_http_module.index])      \
            ->main_conf[module.ctx_index]:                                    \
        NULL)


#endif /* _NGX_HTTP_CONFIG_H_INCLUDED_ */
