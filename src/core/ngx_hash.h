
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HASH_H_INCLUDED_
#define _NGX_HASH_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct {
    void             *value;
    u_short           len;
    u_char            name[1];
} ngx_hash_elt_t;


typedef struct {
    ngx_hash_elt_t  **buckets;
    ngx_uint_t        size;
} ngx_hash_t;


/* 为处理带有通配符的域名的匹配问题，支持通配符在前和在后两种类型的域名，如*.abc.com和mail.xxx.* */
typedef struct {
    ngx_hash_t        hash;
    void             *value;
} ngx_hash_wildcard_t;


typedef struct {
    ngx_str_t         key;
    ngx_uint_t        key_hash;			/* 对key使用hash函数计算出来的值 */
    void             *value;
} ngx_hash_key_t;


typedef ngx_uint_t (*ngx_hash_key_pt) (u_char *data, size_t len);


typedef struct {
    ngx_hash_t            hash;			/* 普通hash表 */
    ngx_hash_wildcard_t  *wc_head;		/* 包含前向通配符的hash表 */
    ngx_hash_wildcard_t  *wc_tail;		/* 包含后向通配符的hash表 */
} ngx_hash_combined_t;


typedef struct {
/* 若该字段为NULL，则调用完初始化函数后，该字段指向新创建的hash表；
 * 若不为NULL，则在初始化时所有数据被插入了这个字段所指的hash表中 
 */
    ngx_hash_t       *hash;	
    ngx_hash_key_pt   key;			/* 指向从字符串生成hash值的hash函数 */
	
    ngx_uint_t        max_size;		/* hash表中的桶的个数。该字段越大，元素存储时冲突可能性越小，每个桶存储元素会更少，查询速度更快 */
    ngx_uint_t        bucket_size;	/* 每个桶的最大限制大小，byte为单位。 */

    char             *name;			/* 该hash表的名字 */
    ngx_pool_t       *pool;			/* 该hash表分配内存使用的pool */
    ngx_pool_t       *temp_pool;	/* 该hash表使用的临时pool，在初始化完成以后，该pool可以被释放和销毁 */
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
    ngx_uint_t        hash;
    ngx_str_t         key;
    ngx_str_t         value;
    u_char           *lowcase_key;
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

#define ngx_hash(key, c)   ((ngx_uint_t) key * 31 + c)
ngx_uint_t ngx_hash_key(u_char *data, size_t len);
ngx_uint_t ngx_hash_key_lc(u_char *data, size_t len);
ngx_uint_t ngx_hash_strlow(u_char *dst, u_char *src, size_t n);


ngx_int_t ngx_hash_keys_array_init(ngx_hash_keys_arrays_t *ha, ngx_uint_t type);
ngx_int_t ngx_hash_add_key(ngx_hash_keys_arrays_t *ha, ngx_str_t *key,
    void *value, ngx_uint_t flags);


#endif /* _NGX_HASH_H_INCLUDED_ */
