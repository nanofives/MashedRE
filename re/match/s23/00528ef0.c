// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00528ef0: 100% (regs 100%). zlib 1.1.3 trees.c _tr_stored_block with send_bits, copy_block and
   bi_windup written inline (STORED_BLOCK = 0, header = 1). What moved it: copying the inlined callee's arguments
   into fresh locals (len, b) after send_bits, as the inliner does; with buf used directly edi held 0 instead of
   buf (87.1%). A static __inline copy_block is not inlined at /Ob1 (19%). Residual: none. */
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
typedef struct {
    char pad0[8];
    unsigned char *pending_buf;     /* 0x08 */
    char pad1[8];
    int pending;                    /* 0x14 */
    char pad2[0x16ac - 0x18];
    int last_eob_len;               /* 0x16ac */
    unsigned short bi_buf;          /* 0x16b0 */
    short pad3;
    int bi_valid;                   /* 0x16b4 */
} deflate_state;

#define put_byte(s, c) {s->pending_buf[s->pending++] = (c);}
#define put_short(s, w) { \
    put_byte(s, (unsigned char)((w) & 0xff)); \
    put_byte(s, (unsigned char)((unsigned short)(w) >> 8)); \
}

// MATCH: 0x00528ef0 FUN_00528ef0
void FUN_00528ef0(deflate_state *s, char *buf, unsigned long stored_len, int eof)
{
    unsigned len;
    char *b;
    { int len = 3;
      if (s->bi_valid > (int)16 - len) {
        int val = eof;
        s->bi_buf |= (val << s->bi_valid);
        put_short(s, s->bi_buf);
        s->bi_buf = (unsigned short)val >> (16 - s->bi_valid);
        s->bi_valid += len - 16;
      } else {
        s->bi_buf |= (eof) << s->bi_valid;
        s->bi_valid += len;
      }
    }
    len = (unsigned)stored_len;
    b = buf;
    if (s->bi_valid > 8) {
        put_short(s, s->bi_buf);
    } else if (s->bi_valid > 0) {
        put_byte(s, (unsigned char)s->bi_buf);
    }
    s->bi_buf = 0;
    s->bi_valid = 0;
    s->last_eob_len = 8;
    put_short(s, (unsigned short)len);
    put_short(s, (unsigned short)~len);
    while (len--) {
        put_byte(s, *b++);
    }
}
#undef put_byte
#undef put_short
