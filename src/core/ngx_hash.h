
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HASH_H_INCLUDED_
#define _NGX_HASH_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/* 用于存储散列表中的元素 */
typedef struct {
    void             *value;    /* 指向用户自定义元素数据的指针，如果当前ngx_hash_elt_t槽为空，则value值为0 */
    u_short           len;      /* 元素关键字的长度 */
    u_char            name[1];  /* 元素关键字的首地址 */
} ngx_hash_elt_t;

/* 基本散列表 */
typedef struct {
    ngx_hash_elt_t  **buckets;
    ngx_uint_t        size;
} ngx_hash_t;


/* 为处理带有通配符的域名的匹配问题，支持通配符在前和在后两种类型的域名，如*.abc.com和mail.xxx.* */
/* 专用于表示前置或后置通配符的散列表 */
typedef struct {
    ngx_hash_t        hash;     /* 基本散列表 */
    void             *value;    /* 当使用该通配符散列表作为某容器的元素时，可以使用这个value指针指向用户数据 */
} ngx_hash_wildcard_t;  /* 相应两个函数：ngx_hash_find_wc_head, ngx_hash_find_wc_tail */


typedef struct {
    ngx_str_t         key;
    ngx_uint_t        key_hash;			/* 对key使用hash函数计算出来的值 */
    void             *value;
} ngx_hash_key_t;


/* 散列方法指针: data是元素关键字的首地址，len是元素关键字的长度
 */
typedef ngx_uint_t (*ngx_hash_key_pt) (u_char *data, size_t len);


typedef struct {
    ngx_hash_t            hash;			/* 普通hash表: 用于精确匹配的基本散列表 */
    ngx_hash_wildcard_t  *wc_head;		/* 包含前向通配符的hash表 */
    ngx_hash_wildcard_t  *wc_tail;		/* 包含后向通配符的hash表 */
} ngx_hash_combined_t;


typedef struct {    /* 用于初始化散列表 */
    ngx_hash_t       *hash;         /* 指向普通的完全匹配散列表 */
    ngx_hash_key_pt   key;			/* 指向从字符串生成hash值的hash函数 */
	
    ngx_uint_t        max_size;		/* hash表中的桶的个数。该字段越大，元素存储时冲突可能性越小，每个桶存储元素会更少，查询速度更快 */
    ngx_uint_t        bucket_size;	/* 散列表一个槽的空间大小，它限制了每个散列表元素关键字的最大长度 */

    char             *name;			/* 该hash表的名字 */
    ngx_pool_t       *pool;			/* 内存池，它分配散列表（最多3个：普通散列表，前置、后置通配符散列表）中的所有槽 */
    ngx_pool_t       *temp_pool;	/* 临时内存池，它仅存于初始化散列表之前。主要用于分配一些临时的动态数组，带通配符的元素在初始化时需要用到这些数组 */
} ngx_hash_init_t;


#define NGX_HASH_SMALL            1
#define NGX_HASH_LARGE            2

#define NGX_HASH_LARGE_ASIZE      16384
#define NGX_HASH_LARGE_HSIZE      10007

#define NGX_HASH_WILDCARD_KEY     1
#define NGX_HASH_READONLY_KEY     2


typedef struct {
    ngx_uint_t        hsize;				/* 将要构建的hash表的桶的个数 */

    ngx_pool_t       *pool;					/* 构建这些hash表使用的pool */
    ngx_pool_t       *temp_pool;			/* 在构建这各类型以及最终的三个hash表过程中可能用到临时pool。该temp_pool可以在构建完成以后，被销毁 */

    ngx_array_t       keys;					/* 存放所有非通配符key的数组 */
    ngx_array_t      *keys_hash;			/* 这是个二维数组，第一维度代表bucket编号，则keys_hash[i]中存放是所有key计算出来的hash值对hsize取模以后的值为i的key */

    ngx_array_t       dns_wc_head;			/* 存放前向通配符key被处理完成以后的值，如*.abc.com被处理完成以后，变成com.abc.被存放在此数组中 */
    ngx_array_t      *dns_wc_head_hash;		/* 该值在调用过程中用来保存和检测是否有冲突的前向通配符的key值，即是否有重复 */

    ngx_array_t       dns_wc_tail;			/* 存放后向通配符key被处理完成以后的值，如mail.xxx.*被处理完成以后，变成mail.xxx.被存放此数组中 */
    ngx_array_t      *dns_wc_tail_hash;		/* 该值在调用过程中用来保存和检测是否有冲突的后向通配符的key值，即是否有重复 */
} ngx_hash_keys_arrays_t;


typedef struct {
    ngx_uint_t        hash;         /* 用于快速检索头部 */
    ngx_str_t         key;          /* 可存储HTTP头部名称，比如Content-Length */
    ngx_str_t         value;        /* 存储对应的值 */
    u_char           *lowcase_key;  /* 为了忽略HTTP头部名称的大小写 */
} ngx_table_elt_t;


void *ngx_hash_find(ngx_hash_t *hash, ngx_uint_t key, u_char *name, size_t len);
void *ngx_hash_find_wc_head(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_wc_tail(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_combined(ngx_hash_combined_t *hash, ngx_uint_t key,
    u_char *name, size_t len);

ngx_int_t ngx_hash_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,
    ngx_uint_t nelts);
ngx_int_t ngx_hash_wildcard_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,
    ngx_uint_t nelts);

#define ngx_hash(key, c)   ((ngx_uint_t) key * 31 + c)  /* 使用BKDR算法将任意长度的字符串映射为整型 */
ngx_uint_t ngx_hash_key(u_char *data, size_t len);
ngx_uint_t ngx_hash_key_lc(u_char *data, size_t len);
ngx_uint_t ngx_hash_strlow(u_char *dst, u_char *src, size_t n);


ngx_int_t ngx_hash_keys_array_init(ngx_hash_keys_arrays_t *ha, ngx_uint_t type);
ngx_int_t ngx_hash_add_key(ngx_hash_keys_arrays_t *ha, ngx_str_t *key,
    void *value, ngx_uint_t flags);


#endif /* _NGX_HASH_H_INCLUDED_ */
