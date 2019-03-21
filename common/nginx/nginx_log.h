#ifndef NGINX_LIG_H
#define NGINX_LIG_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

typedef pid_t       ngx_pid_t;
typedef int32_t                     ngx_atomic_int_t;
typedef uint32_t                    ngx_atomic_uint_t;
typedef volatile ngx_atomic_uint_t  ngx_atomic_t;
#define NGX_ATOMIC_T_LEN            (sizeof("-2147483648") - 1)

#define NGX_INVALID_PID  -1

// 跨平台的 换行符号
#define LF     (u_char) '\n'
#define CR     (u_char) '\r'
#define CRLF   "\r\n"

#define NGX_INT32_LEN   (sizeof("-2147483648") - 1)
#define NGX_INT64_LEN   (sizeof("-9223372036854775808") - 1)

#define ngx_abs(value)       (((value) >= 0) ? (value) : - (value))
#define ngx_max(val1, val2)  ((val1 < val2) ? (val2) : (val1))
#define ngx_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))

#define NGX_MAX_UINT32_VALUE  (uint32_t) 0xffffffff
#define NGX_MAX_INT32_VALUE   (uint32_t) 0x7fffffff

#define ngx_memcpy(dst, src, n)   (void) memcpy(dst, src, n)
#define ngx_cpymem(dst, src, n)   (((u_char *) memcpy(dst, src, n)) + (n))

// 指针的大小为 8 
#define NGX_INT_T_LEN   NGX_INT64_LEN
#define NGX_MAX_INT_T_VALUE  9223372036854775807

// 位域
typedef struct {
    unsigned    len:28;

    unsigned    valid:1;
    unsigned    no_cacheable:1;
    unsigned    not_found:1;
    unsigned    escape:1;
    // 28+1+1+1+1 = 32
    u_char     *data;
} ngx_variable_value_t;


typedef struct {
    size_t      len;
    u_char     *data;
} ngx_str_t;

typedef uintptr_t           ngx_uint_t;
typedef ngx_uint_t          ngx_rbtree_key_t;
typedef ngx_rbtree_key_t    ngx_msec_t;

typedef intptr_t        ngx_int_t;
typedef ngx_int_t   ngx_rbtree_key_int_t;
typedef ngx_rbtree_key_int_t  ngx_msec_int_t;

u_char * 
ngx_snprintf(u_char *buf, size_t max, const char *fmt, ...);

u_char *
ngx_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args);

#endif