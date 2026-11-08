// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052da20: 100% (regs 100%). Image create by format id (no library source): format lookup in the
   0x00912160 table (RW error 0x1b7 / FUN_004d7ff0(2) when not found) written as a static __forceinline helper,
   then RwMalloc (RwEngineInstance 0x007d3ff8 +0x108, hint 0x3001b) of rowBytes*rows + 0x18 + extraBytes.
   What moved it: the store order of the three header fields (rows, b, a: the other orders gave 94.4-98.6%).
   Residual: none. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct { int pluginID; int errorCode; } RwError;
typedef struct {
    int id;                     /* 0x00 */
    int pad4;
    int rowBytes;               /* 0x08 */
    int pad0c[8];
    int extraBytes;             /* 0x2c */
} fmt_handler_t;                /* 0x30 bytes */
typedef struct {
    fmt_handler_t *fmt;         /* 0x00 */
    int rows;                   /* 0x04 */
    int a;                      /* 0x08 */
    int b;                      /* 0x0c */
    unsigned char *pixels;      /* 0x10 */
    unsigned char *extra;       /* 0x14 */
} img_t;
typedef void *(*rwmalloc_t)(unsigned size, unsigned hint);

MGLOBAL(int, DAT_007dbcd8, 0x007dbcd8);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(fmt_handler_t, DAT_00912160, 16, 0x00912160);

int FUN_004d7ff0(int code);
RwError *FUN_004d8480(RwError *err);

static __forceinline fmt_handler_t *find_fmt(int id)
{
    int i;
    RwError err;

    for (i = 0; i < DAT_007dbcd8; i++) {
        if (DAT_00912160[i].id == id)
            return &DAT_00912160[i];
    }
    err.pluginID = 0x1b7;
    err.errorCode = FUN_004d7ff0(2);
    FUN_004d8480(&err);
    return 0;
}

// MATCH: 0x0052da20 FUN_0052da20
img_t *FUN_0052da20(int id, int rows, int a, int b)
{
    fmt_handler_t *fmt;
    img_t *img;

    fmt = find_fmt(id);
    if (fmt == 0)
        return 0;
    img = (img_t *)(*(rwmalloc_t *)(DAT_007d3ff8 + 0x108))(fmt->rowBytes * rows + sizeof(img_t) + fmt->extraBytes, 0x3001b);
    img->rows = rows;
    img->b = b;
    img->a = a;
    img->fmt = fmt;
    img->pixels = (unsigned char *)(img + 1);
    if (fmt->extraBytes > 0)
        img->extra = img->pixels + fmt->rowBytes * rows;
    else
        img->extra = 0;
    return img;
}
