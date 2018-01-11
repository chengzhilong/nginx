
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CONNECTION_H_INCLUDED_
#define _NGX_CONNECTION_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_listening_s  ngx_listening_t;

struct ngx_listening_s {
    ngx_socket_t        fd;

    struct sockaddr    *sockaddr;
    socklen_t           socklen;    /* size of sockaddr */
    size_t              addr_text_max_len;
    ngx_str_t           addr_text;

    int                 type;

    int                 backlog;
    int                 rcvbuf;
    int                 sndbuf;
#if (NGX_HAVE_KEEPALIVE_TUNABLE)
    int                 keepidle;
    int                 keepintvl;
    int                 keepcnt;
#endif

    /* handler of accepted connection */
    ngx_connection_handler_pt   handler;

    void               *servers;  /* array of ngx_http_in_addr_t, for example */

    ngx_log_t           log;
    ngx_log_t          *logp;

    size_t              pool_size;
    /* should be here because of the AcceptEx() preread */
    size_t              post_accept_buffer_size;
    /* should be here because of the deferred accept */
    ngx_msec_t          post_accept_timeout;

    ngx_listening_t    *previous;
    ngx_connection_t   *connection;

    ngx_uint_t          worker;

    unsigned            open:1;
    unsigned            remain:1;
    unsigned            ignore:1;

    unsigned            bound:1;       /* already bound */
    unsigned            inherited:1;   /* inherited from previous process */
    unsigned            nonblocking_accept:1;
    unsigned            listen:1;
    unsigned            nonblocking:1;
    unsigned            shared:1;    /* shared between threads or processes */
    unsigned            addr_ntop:1;
    unsigned            wildcard:1;

#if (NGX_HAVE_INET6)
    unsigned            ipv6only:1;
#endif
    unsigned            reuseport:1;
    unsigned            add_reuseport:1;
    unsigned            keepalive:2;

    unsigned            deferred_accept:1;
    unsigned            delete_deferred:1;
    unsigned            add_deferred:1;
#if (NGX_HAVE_DEFERRED_ACCEPT && defined SO_ACCEPTFILTER)
    char               *accept_filter;
#endif
#if (NGX_HAVE_SETFIB)
    int                 setfib;
#endif

#if (NGX_HAVE_TCP_FASTOPEN)
    int                 fastopen;
#endif

};


typedef enum {
    NGX_ERROR_ALERT = 0,
    NGX_ERROR_ERR,
    NGX_ERROR_INFO,
    NGX_ERROR_IGNORE_ECONNRESET,
    NGX_ERROR_IGNORE_EINVAL
} ngx_connection_log_error_e;


typedef enum {
    NGX_TCP_NODELAY_UNSET = 0,
    NGX_TCP_NODELAY_SET,
    NGX_TCP_NODELAY_DISABLED
} ngx_connection_tcp_nodelay_e;


typedef enum {
    NGX_TCP_NOPUSH_UNSET = 0,
    NGX_TCP_NOPUSH_SET,
    NGX_TCP_NOPUSH_DISABLED
} ngx_connection_tcp_nopush_e;


#define NGX_LOWLEVEL_BUFFERED  0x0f
#define NGX_SSL_BUFFERED       0x01
#define NGX_HTTP_V2_BUFFERED   0x02


struct ngx_connection_s {
	/* 连接未使用时，data用于充当连接池中空闲链表中的next指针。
	 * 连接使用时由模块而定，HTTP中，data指向ngx_http_request_t
	 */
    void               *data;
    ngx_event_t        *read;			/* 连接对应的读事件 */
    ngx_event_t        *write;			/* 连接对应的写事件 */

    ngx_socket_t        fd;				/* 套接字对应的句柄 */

    ngx_recv_pt         recv;			/* 直接接收网络字符流的方法 */
    ngx_send_pt         send;			/* 直接发送网络字符流的方法 */
    ngx_recv_chain_pt   recv_chain;		/* 以链表来接收网络字符流的方法 */
    ngx_send_chain_pt   send_chain;		/* 以链表来发送网络字符流的方法 */

    ngx_listening_t    *listening;		/* 对应ngx_listening_t监听对象，此连接由listening监听端口的事件建立 */

    off_t               sent;			/* 这个连接上已发送的字节数 */

    ngx_log_t          *log;			/* 日志对象 */

	/* 内存池。一般在accept一个新的连接时，会创建一个内存池，而在这个连接结束
	 * 时会销毁内存池。内存池大小是由上面listening成员的pool_size决定的。
	 */
    ngx_pool_t         *pool;

    int                 type;

    struct sockaddr    *sockaddr;		/* 连接客户端的sockaddr */
    socklen_t           socklen;		/* sockaddr结构体的长度 */
    ngx_str_t           addr_text;		/* 连接客户端字符串形成的IP地址 */

    ngx_str_t           proxy_protocol_addr;
    in_port_t           proxy_protocol_port;

#if (NGX_SSL || NGX_COMPAT)
    ngx_ssl_connection_t  *ssl;
#endif

	/* 本机监听端口对应的sockaddr结构体，实际上就是listening监听对象的sockaddr成员 */
    struct sockaddr    *local_sockaddr;
    socklen_t           local_socklen;

	/* 用户接受、缓存客户端发来的字符流，buffer是由连接内存池分配的，大小自由决定。
	 * 用来将当前连接以双向链表元素的形式添加到ngx_cycle_t核心结构体的reuseable_connection_queue
	 * 双向链表中，表示可以重用的连接。
	 */
    ngx_buf_t          *buffer;

    ngx_queue_t         queue;

	/* 连接使用次数。ngx_connection_t结构体每次建立一条来自客户端的连接，或者主动向后端服务器
	 * 发起连接时，number都会加1
	 */
    ngx_atomic_uint_t   number;

    ngx_uint_t          requests;		/* 处理的请求次数 */

    unsigned            buffered:8;		/* 缓存中的业务类型 */

	/* 本连接的日志级别 */
    unsigned            log_error:3;     /* ngx_connection_log_error_e */

    unsigned            timedout:1;		/* 为1表示连接已超时 */
    unsigned            error:1;		/* 为1表示连接处理过程中出现错误 */
    unsigned            destroyed:1;	/* 为1表示连接已经销毁 */

    unsigned            idle:1;			/* 为1表示连接处于空闲状态，如keepalive两次请求中间的状态 */
    unsigned            reusable:1;		/* 为1表示连接可重用，与上面的queue字段对应使用 */
    unsigned            close:1;		/* 为1表示连接关闭 */
    unsigned            shared:1;

    unsigned            sendfile:1;		/* 为1表示正在将文件中的数据发往连接的另一端 */
	/* 为1表示只有连接套接字对应的发送缓冲区必须满足最低设置的大小阈值时，
	 * 事件驱动模块才会分发该事件。这与ngx_handle_write_event方法中的lowat
	 * 参数是对应的。
	 */
    unsigned            sndlowat:1;	
    unsigned            tcp_nodelay:2;   /* ngx_connection_tcp_nodelay_e */
    unsigned            tcp_nopush:2;    /* ngx_connection_tcp_nopush_e */

    unsigned            need_last_buf:1;

#if (NGX_HAVE_AIO_SENDFILE || NGX_COMPAT)
    unsigned            busy_count:2;
#endif

#if (NGX_THREADS || NGX_COMPAT)
    ngx_thread_task_t  *sendfile_task;
#endif
};


#define ngx_set_connection_log(c, l)                                         \
                                                                             \
    c->log->file = l->file;                                                  \
    c->log->next = l->next;                                                  \
    c->log->writer = l->writer;                                              \
    c->log->wdata = l->wdata;                                                \
    if (!(c->log->log_level & NGX_LOG_DEBUG_CONNECTION)) {                   \
        c->log->log_level = l->log_level;                                    \
    }


ngx_listening_t *ngx_create_listening(ngx_conf_t *cf, struct sockaddr *sockaddr,
    socklen_t socklen);
ngx_int_t ngx_clone_listening(ngx_conf_t *cf, ngx_listening_t *ls);
ngx_int_t ngx_set_inherited_sockets(ngx_cycle_t *cycle);
ngx_int_t ngx_open_listening_sockets(ngx_cycle_t *cycle);
void ngx_configure_listening_sockets(ngx_cycle_t *cycle);
void ngx_close_listening_sockets(ngx_cycle_t *cycle);
void ngx_close_connection(ngx_connection_t *c);
void ngx_close_idle_connections(ngx_cycle_t *cycle);
ngx_int_t ngx_connection_local_sockaddr(ngx_connection_t *c, ngx_str_t *s,
    ngx_uint_t port);
ngx_int_t ngx_tcp_nodelay(ngx_connection_t *c);
ngx_int_t ngx_connection_error(ngx_connection_t *c, ngx_err_t err, char *text);

ngx_connection_t *ngx_get_connection(ngx_socket_t s, ngx_log_t *log);
void ngx_free_connection(ngx_connection_t *c);

void ngx_reusable_connection(ngx_connection_t *c, ngx_uint_t reusable);

#endif /* _NGX_CONNECTION_H_INCLUDED_ */
