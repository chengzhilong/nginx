
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_BUF_H_INCLUDED_
#define _NGX_BUF_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef void *            ngx_buf_tag_t;

typedef struct ngx_buf_s  ngx_buf_t;

struct ngx_buf_s {
    u_char          *pos;			/* 当buf指向的数据在内存里时，pos指向该段数据开始位置 */
    u_char          *last;			/* 同上，last指向该段数据结束位置 */
    off_t            file_pos;		/* 当buf指向的数据在文件里时，file_pos指向这段数据的开始位置在文件中偏移量 */
    off_t            file_last;		/* 同上，file_last指向这段数据的结束位置在文件中偏移量 */

	/* 当buf指向的数据在内存里时，这一整块内存包含的内容可能被包含在多个buf中(如某段数据中间插入了其他数据，则该块数据需被拆分开。
	 * 那么这些buf中的start和end都指向这一块内存的开始地址和结束地址，而pos和last指向本buf所书籍包含的数据的开始和结尾
	 */
    u_char          *start;         /* start of buffer */
    u_char          *end;           /* end of buffer */		/* 解释参考start */
    ngx_buf_tag_t    tag;			/* void*类型指针，使用者可以关联任意对象上去，只对使用者有意义 */
    ngx_file_t      *file;			/* 当buf所包含的内容在文件中时，file字段指向对应的文件对象 */
    ngx_buf_t       *shadow;		/* 当这个buf完整copy了另外一个buf所有字段时，则这两个buf指向的实际上是同一块内存或同一文件的同一部分。因此在释放时需特别小心，别多次释放资源 */


    /* the buf's content could be changed */
    unsigned         temporary:1;	/* 为1时表示该buf所包含的内容是在一个用户创建的内存块中，并且可以被filter处理的过程中进行变更，而不会造成问题 */

    /*
     * the buf's content is in a memory cache or in a read only memory
     * and must not be changed
     */
    unsigned         memory:1;		/* 为1时表示该buf所包含内容在内存中，但这些内容不能被进行处理的filter进行变更 */

    /* the buf's content is mmap()ed and must not be changed */
    unsigned         mmap:1;		/* 为1时表示该buf所包含内容在内存中，是通过mmap从文件映射到内存中，这些内容不能被进行处理的filter进行变更 */

    unsigned         recycled:1;		/* 可以回收的。也就是这个buf是可以被释放的，通常配合shadow字段一起使用 */
    unsigned         in_file:1;			/* 为1时表明该buf所包含的内容是在文件中 */
    unsigned         flush:1;			/* 遇到有flush字段被设置为1的buf的chain，则该chain的数据即便不是最后结束的数据，也会进行输出，不会受postpone_output配置限制，但会收到发送速率等其他条件限制 */
    unsigned         sync:1;
    unsigned         last_buf:1;		/* 数据被以多个chain传递给了过滤器，此字段为1表明这是最后一个buf */
    unsigned         last_in_chain:1;	/* 在当前的chain里面，此buf是最后一个。注意last_in_chain的buf不一定是last_buf，但last_buf的buf一定是last_in_chain的 */

    unsigned         last_shadow:1;		/* 在创建一个buf的shadow的时候，通常将新创建的一个buf的last_shadow置为1 */
    unsigned         temp_file:1;		/* 由于收到内存使用的限制，有时一些buf内容需要被写到磁盘上临时文件中，这时就设置此标志 */

    /* STUB */ int   num;
};


/* ngx_chain_t是与ngx_buf_t配合使用的链表数据结构 */
struct ngx_chain_s {
    ngx_buf_t    *buf;			/* 指向实际的数据 */
    ngx_chain_t  *next;
}; /* 在向用户发送HTTP包体时，要传入ngx_chain_t链表对象 */


typedef struct {
    ngx_int_t    num;
    size_t       size;
} ngx_bufs_t;


typedef struct ngx_output_chain_ctx_s  ngx_output_chain_ctx_t;

typedef ngx_int_t (*ngx_output_chain_filter_pt)(void *ctx, ngx_chain_t *in);

typedef void (*ngx_output_chain_aio_pt)(ngx_output_chain_ctx_t *ctx,
    ngx_file_t *file);

struct ngx_output_chain_ctx_s {
    ngx_buf_t                   *buf;
    ngx_chain_t                 *in;
    ngx_chain_t                 *free;
    ngx_chain_t                 *busy;

    unsigned                     sendfile:1;
    unsigned                     directio:1;
    unsigned                     unaligned:1;
    unsigned                     need_in_memory:1;
    unsigned                     need_in_temp:1;
    unsigned                     aio:1;

#if (NGX_HAVE_FILE_AIO || NGX_COMPAT)
    ngx_output_chain_aio_pt      aio_handler;
#if (NGX_HAVE_AIO_SENDFILE || NGX_COMPAT)
    ssize_t                    (*aio_preload)(ngx_buf_t *file);
#endif
#endif

#if (NGX_THREADS || NGX_COMPAT)
    ngx_int_t                  (*thread_handler)(ngx_thread_task_t *task,
                                                 ngx_file_t *file);
    ngx_thread_task_t           *thread_task;
#endif

    off_t                        alignment;

    ngx_pool_t                  *pool;
    ngx_int_t                    allocated;
    ngx_bufs_t                   bufs;
    ngx_buf_tag_t                tag;

    ngx_output_chain_filter_pt   output_filter;
    void                        *filter_ctx;
};


typedef struct {
    ngx_chain_t                 *out;
    ngx_chain_t                **last;
    ngx_connection_t            *connection;
    ngx_pool_t                  *pool;
    off_t                        limit;
} ngx_chain_writer_ctx_t;


#define NGX_CHAIN_ERROR     (ngx_chain_t *) NGX_ERROR

/* 返回这个buf里面内容是否在内存里 */
#define ngx_buf_in_memory(b)        (b->temporary || b->memory || b->mmap)
/* 返回这个buf里面的内容是否仅仅在内存里，并且没有在文件里 */
#define ngx_buf_in_memory_only(b)   (ngx_buf_in_memory(b) && !b->in_file)

/* 返回该buf是否是一个特殊的buf，只含有特殊的标识和没有包含真正的数据 */
#define ngx_buf_special(b)                                                   \
    ((b->flush || b->last_buf || b->sync)                                    \
     && !ngx_buf_in_memory(b) && !b->in_file)

/* 返回该buf是否是一个只包含sync标识而不包含真正数据的特殊buf */
#define ngx_buf_sync_only(b)                                                 \
    (b->sync                                                                 \
     && !ngx_buf_in_memory(b) && !b->in_file && !b->flush && !b->last_buf)

/* 返回该buf所含数据的大小，不管这个数据是在文件里还是在内存里 */
#define ngx_buf_size(b)                                                      \
    (ngx_buf_in_memory(b) ? (off_t) (b->last - b->pos):                      \
                            (b->file_last - b->file_pos))

ngx_buf_t *ngx_create_temp_buf(ngx_pool_t *pool, size_t size);
ngx_chain_t *ngx_create_chain_of_bufs(ngx_pool_t *pool, ngx_bufs_t *bufs);


#define ngx_alloc_buf(pool)  ngx_palloc(pool, sizeof(ngx_buf_t))
#define ngx_calloc_buf(pool) ngx_pcalloc(pool, sizeof(ngx_buf_t))

ngx_chain_t *ngx_alloc_chain_link(ngx_pool_t *pool);
#define ngx_free_chain(pool, cl)                                             \
    cl->next = pool->chain;                                                  \
    pool->chain = cl



ngx_int_t ngx_output_chain(ngx_output_chain_ctx_t *ctx, ngx_chain_t *in);
ngx_int_t ngx_chain_writer(void *ctx, ngx_chain_t *in);

ngx_int_t ngx_chain_add_copy(ngx_pool_t *pool, ngx_chain_t **chain,
    ngx_chain_t *in);
ngx_chain_t *ngx_chain_get_free_buf(ngx_pool_t *p, ngx_chain_t **free);
void ngx_chain_update_chains(ngx_pool_t *p, ngx_chain_t **free,
    ngx_chain_t **busy, ngx_chain_t **out, ngx_buf_tag_t tag);

off_t ngx_chain_coalesce_file(ngx_chain_t **in, off_t limit);

ngx_chain_t *ngx_chain_update_sent(ngx_chain_t *in, off_t sent);

#endif /* _NGX_BUF_H_INCLUDED_ */
