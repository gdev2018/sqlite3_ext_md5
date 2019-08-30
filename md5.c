/*
Maintained by Alexey Pechnikov (pechnikov@mobigroup.ru) and tested on linux only.
The code is public domain. 

gcc -lm -fPIC -shared md5.c -o libsqlitemd5.so

.load './libsqlitemd5.so'
create table t1(x);
insert into t1 values (1);
insert into t1 values (2);
insert into t1 values (3);

select hex(group_md5(x)) from t1;
    => 202CB962AC59075B964B07152D234B70

select hex(md5(''));
    => D41D8CD98F00B204E9800998ECF8427E

select hex(md5_utf16('a'));
    => 4144E195F46DE78A3623DA7364D04F11

select hex(md5file('/tmp/agent_callbuilder.tcl'));
    => 6a85de907c7b44c3758e610853618f5b
*/
/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */

#if !defined(SQLITE_CORE) || defined(SQLITE_ENABLE_MD5)

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#ifndef SQLITE_CORE
  #include "sqlite3ext.h"
  SQLITE_EXTENSION_INIT1
#else
  #include "sqlite3.h"
#endif

/*
 * If compiled on a machine that doesn't have a 32-bit integer,
 * you just set "uint32" to the appropriate datatype for an
 * unsigned 32-bit integer.  For example:
 *
 *       cc -Duint32='unsigned long' md5.c
 *
 */
#ifndef uint32
#  define uint32 unsigned int
#endif

// identifier Context is used in SQLite code
struct Context2 {
    uint32 buf[4];
    uint32 bits[2];
    unsigned char in[64];
};
typedef char MD5Context[88];

/*
 * Note: this code is harmless on little-endian machines.
 */
static void byteReverse (unsigned char *buf, unsigned longs){
    uint32 t;
    do {
        t = (uint32)((unsigned)buf[3]<<8 | buf[2]) << 16 |
            ((unsigned)buf[1]<<8 | buf[0]);
        *(uint32 *)buf = t;
        buf += 4;
    } while (--longs);
}
/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
        ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void MD5Transform(uint32 buf[4], const uint32 in[16]){
    register uint32 a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[ 0]+0xd76aa478,  7);
    MD5STEP(F1, d, a, b, c, in[ 1]+0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[ 2]+0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[ 3]+0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[ 4]+0xf57c0faf,  7);
    MD5STEP(F1, d, a, b, c, in[ 5]+0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[ 6]+0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[ 7]+0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[ 8]+0x698098d8,  7);
    MD5STEP(F1, d, a, b, c, in[ 9]+0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10]+0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11]+0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12]+0x6b901122,  7);
    MD5STEP(F1, d, a, b, c, in[13]+0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14]+0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15]+0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[ 1]+0xf61e2562,  5);
    MD5STEP(F2, d, a, b, c, in[ 6]+0xc040b340,  9);
    MD5STEP(F2, c, d, a, b, in[11]+0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[ 0]+0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[ 5]+0xd62f105d,  5);
    MD5STEP(F2, d, a, b, c, in[10]+0x02441453,  9);
    MD5STEP(F2, c, d, a, b, in[15]+0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[ 4]+0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[ 9]+0x21e1cde6,  5);
    MD5STEP(F2, d, a, b, c, in[14]+0xc33707d6,  9);
    MD5STEP(F2, c, d, a, b, in[ 3]+0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[ 8]+0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13]+0xa9e3e905,  5);
    MD5STEP(F2, d, a, b, c, in[ 2]+0xfcefa3f8,  9);
    MD5STEP(F2, c, d, a, b, in[ 7]+0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12]+0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[ 5]+0xfffa3942,  4);
    MD5STEP(F3, d, a, b, c, in[ 8]+0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11]+0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14]+0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[ 1]+0xa4beea44,  4);
    MD5STEP(F3, d, a, b, c, in[ 4]+0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[ 7]+0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10]+0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13]+0x289b7ec6,  4);
    MD5STEP(F3, d, a, b, c, in[ 0]+0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[ 3]+0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[ 6]+0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[ 9]+0xd9d4d039,  4);
    MD5STEP(F3, d, a, b, c, in[12]+0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15]+0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[ 2]+0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[ 0]+0xf4292244,  6);
    MD5STEP(F4, d, a, b, c, in[ 7]+0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14]+0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[ 5]+0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12]+0x655b59c3,  6);
    MD5STEP(F4, d, a, b, c, in[ 3]+0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10]+0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[ 1]+0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[ 8]+0x6fa87e4f,  6);
    MD5STEP(F4, d, a, b, c, in[15]+0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[ 6]+0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13]+0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[ 4]+0xf7537e82,  6);
    MD5STEP(F4, d, a, b, c, in[11]+0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[ 2]+0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[ 9]+0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
static void MD5Init(MD5Context *pCtx){
    struct Context2 *ctx = (struct Context2 *)pCtx;
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;
    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static void MD5Update(MD5Context *pCtx, const unsigned char *buf, unsigned int len){
    struct Context2 *ctx = (struct Context2 *)pCtx;
    uint32 t;

    /* Update bitcount */

    t = ctx->bits[0];
    if ((ctx->bits[0] = t + ((uint32)len << 3)) < t)
        ctx->bits[1]++; /* Carry from low to high */
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;    /* Bytes already in shsInfo->data */

    /* Handle any leading odd-sized chunks */

    if ( t ) {
        unsigned char *p = (unsigned char *)ctx->in + t;

        t = 64-t;
        if (len < t) {
            memcpy(p, buf, len);
            return;
        }
        memcpy(p, buf, t);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (uint32 *)ctx->in);
        buf += t;
        len -= t;
    }

    /* Process data in 64-byte chunks */

    while (len >= 64) {
        memcpy(ctx->in, buf, 64);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (uint32 *)ctx->in);
        buf += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */

    memcpy(ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static void MD5Final(unsigned char digest[16], MD5Context *pCtx){
    struct Context2 *ctx = (struct Context2 *)pCtx;
    unsigned count;
    unsigned char *p;

    /* Compute number of bytes mod 64 */
    count = (ctx->bits[0] >> 3) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */
    p = ctx->in + count;
    *p++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if (count < 8) {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        memset(p, 0, count);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (uint32 *)ctx->in);

        /* Now fill the next block with 56 bytes */
        memset(ctx->in, 0, 56);
    } else {
        /* Pad block to 56 bytes */
        memset(p, 0, count-8);
    }
    byteReverse(ctx->in, 14);

    /* Append length in bits and transform */
    ((uint32 *)ctx->in)[ 14 ] = ctx->bits[0];
    ((uint32 *)ctx->in)[ 15 ] = ctx->bits[1];

    MD5Transform(ctx->buf, (uint32 *)ctx->in);
    byteReverse((unsigned char *)ctx->buf, 4);
    memcpy(digest, ctx->buf, 16);
    memset(ctx, 0, sizeof(ctx));    /* In case it's sensitive */
}

/*
** During testing, the special md5sum() aggregate function is available.
** inside SQLite.  The following routines implement that function.
*/
static void md5step(sqlite3_context *context, int argc, sqlite3_value **argv){
    MD5Context *p;
    int i;
    if( argc<1 ) return;
    p = sqlite3_aggregate_context(context, sizeof(*p));
    if( p==0 ) return;
    if( sqlite3_aggregate_count(context)==1 ){
        MD5Init(p);
    }
    for(i=0; i<argc; i++){
        const char *zData = (char*)sqlite3_value_blob(argv[i]);
        if( zData ){
//      MD5Update(p, (unsigned char*)zData, strlen(zData));
            MD5Update(p, (unsigned char*)zData, sqlite3_value_bytes(argv[i]));
        }
    }
}
static void md5finalize(sqlite3_context *context){
    MD5Context *p;
    unsigned char digest[16];
    p = sqlite3_aggregate_context(context, sizeof(*p));
    MD5Final(digest,p);
    sqlite3_result_blob(context, digest, sizeof(digest), SQLITE_TRANSIENT);
}

static void md5(sqlite3_context *context, int argc, sqlite3_value **argv){
    MD5Context ctx;
    unsigned char digest[16];
    int i;

    if( argc<1 ) return;
    if( sqlite3_value_type(argv[0]) == SQLITE_NULL ){
        sqlite3_result_null(context);
        return;
    }
    MD5Init(&ctx);
//  MD5Update(&ctx, (unsigned char*)sqlite3_value_blob(argv[0]), sqlite3_value_bytes(argv[0]));
    for(i=0; i<argc; i++){
        const char *zData = (char*)sqlite3_value_blob(argv[i]);
        if( zData ){
            MD5Update(&ctx, (unsigned char*)zData, sqlite3_value_bytes(argv[i]));
        }
    }
    MD5Final(digest,&ctx);
    sqlite3_result_blob(context, digest, sizeof(digest), SQLITE_TRANSIENT);
}



//long long getLongOffset(unsigned char array[], int offset) {
long long getLongOffset(const unsigned char *hex, int offset) {
    long long value = 0;
    for (int i = 0; i < 8; i++) {
        value = ((value << 8) | (hex[offset+i] & 0xFF));
    }
    return value;
}

char *bin2hex(const unsigned char *bin, size_t len)
{
    char   *out;
    size_t  i;

    if (bin == NULL || len == 0)
        return NULL;

    out = malloc(len*2+1);
    for (i=0; i<len; i++) {
//        out[i*2]   = "0123456789ABCDEF"[bin[i] >> 4];
//        out[i*2+1] = "0123456789ABCDEF"[bin[i] & 0x0F];
        out[i*2]   = "0123456789abcdef"[bin[i] >> 4];
        out[i*2+1] = "0123456789abcdef"[bin[i] & 0x0F];
    }
    out[len*2] = '\0';

    return out;
}

int hexchr2bin(const char hex, char *out)
{
    if (out == NULL)
        return 0;

    if (hex >= '0' && hex <= '9') {
        *out = hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        *out = hex - 'A' + 10;
    } else if (hex >= 'a' && hex <= 'f') {
        *out = hex - 'a' + 10;
    } else {
        return 0;
    }

    return 1;
}

size_t hexs2bin(const char *hex, unsigned char **out)
{
    size_t len;
    char   b1;
    char   b2;
    size_t i;

    if (hex == NULL || *hex == '\0' || out == NULL)
        return 0;

    len = strlen(hex);
    if (len % 2 != 0)
        return 0;
    len /= 2;

    *out = malloc(len);
    memset(*out, 'A', len);
    for (i=0; i<len; i++) {
        if (!hexchr2bin(hex[i*2], &b1) || !hexchr2bin(hex[i*2+1], &b2)) {
            return 0;
        }
        (*out)[i] = (b1 << 4) | b2;
    }
    return len;
}


static void md5long(sqlite3_context *context, int argc, sqlite3_value **argv){
    MD5Context ctx;
    unsigned char digest[16];
    int i;

    if( argc<1 ) return;
    if( sqlite3_value_type(argv[0]) == SQLITE_NULL ){
        sqlite3_result_null(context);
        return;
    }
    MD5Init(&ctx);

    for(i=0; i<argc; i++){
        const char *zData = (char*)sqlite3_value_blob(argv[i]);
        if( zData ){
            MD5Update(&ctx, (unsigned char*)zData, sqlite3_value_bytes(argv[i]));
        }
    }
    MD5Final(digest,&ctx);

    const char *hex;
    hex = bin2hex(digest, strlen(digest));
    printf("bin2hex(digest): %s\n", hex);
    printf("strlen(digest): %d\n", strlen(digest));

    long long md5long;
    md5long = (getLongOffset((unsigned char *)hex, 0) ^ getLongOffset((unsigned char *)hex, 8));
    printf("md5long: %lld\n", md5long);

    unsigned char digest_ethalon[] = "1bc29b36f623ba82aaf6724fd3b16718";
    long long md5long_ethalon;
    md5long_ethalon = (getLongOffset(digest_ethalon, 0) ^ getLongOffset(digest_ethalon, 8));
    printf("md5long_ethalon: %lld\n", md5long_ethalon);

    sqlite3_result_int64(context, md5long);
//    sqlite3_result_blob(context, digest, sizeof(digest), SQLITE_TRANSIENT);
}







// Returns the md5 hash of an UTF-16 representation of a string
static void md5_utf16(sqlite3_context *context, int argc, sqlite3_value **argv) {
    MD5Context ctx;
    unsigned char digest[16];
    int i;

    if (argc<1) return;
    if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
        sqlite3_result_null(context);
        return;
    }
    MD5Init(&ctx);
    for (i = 0; i<argc; i++) {
        const char *zData = (char*)sqlite3_value_text16(argv[i]);
        if (zData) {
            MD5Update(&ctx, (unsigned char*)zData, sqlite3_value_bytes16(argv[i]));
        }
    }
    MD5Final(digest, &ctx);
    sqlite3_result_blob(context, digest, sizeof(digest), SQLITE_TRANSIENT);
}

/*
** A command to take the md5 hash of a file.  The argument is the
** name of the file.
*/
static void md5file(sqlite3_context *context, int argc, sqlite3_value **argv){
    FILE *in;
    MD5Context ctx;
    unsigned char digest[16];
    char zBuf[10240];

    if( sqlite3_value_type(argv[0]) == SQLITE_NULL ){
        // file name not defined
        sqlite3_result_null(context);
        return;
    }
    in = fopen((unsigned char *)sqlite3_value_text(argv[0]),"rb");
    if( in==0 ){
        //unable to open file for reading
        sqlite3_result_null(context);
        return;
    }
    MD5Init(&ctx);
    for(;;){
        int n;
        n = fread(zBuf, 1, sizeof(zBuf), in);
        if( n<=0 ) break;
        MD5Update(&ctx, (unsigned char*)zBuf, (unsigned)n);
    }
    fclose(in);
    MD5Final(digest, &ctx);
    sqlite3_result_blob(context, digest, sizeof(digest), SQLITE_TRANSIENT);
}

/* SQLite invokes this routine once when it loads the extension.
** Create new functions, collating sequences, and virtual table
** modules here.  This is usually the only exported symbol in
** the shared library.
*/

int sqlite3Md5Init(sqlite3 *db){
    sqlite3_create_function(db, "group_md5", -1, SQLITE_UTF8, 0, 0, md5step, md5finalize);
    sqlite3_create_function(db, "md5",      -1, SQLITE_UTF8,  0, md5,     0, 0);
    sqlite3_create_function(db, "md5long",      -1, SQLITE_UTF8,  0, md5long,     0, 0);
    sqlite3_create_function(db, "md5_utf16", -1, SQLITE_UTF16, 0, md5_utf16, 0, 0);
    sqlite3_create_function(db, "md5file",   1, SQLITE_UTF8,  0, md5file, 0, 0);
    return 0;
}

#ifdef _WIN32
__declspec(dllexport)
#endif
#if !SQLITE_CORE
// sqlitemd, not sqlitemd5
int sqlite3_extension_init(
        sqlite3 *db,
        char **pzErrMsg,
        const sqlite3_api_routines *pApi
){
    SQLITE_EXTENSION_INIT2(pApi)
    return sqlite3Md5Init(db);
}
#endif


#endif
