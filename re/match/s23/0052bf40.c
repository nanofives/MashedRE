// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052bf40: 100% (regs 100%). zlib 1.1.3 inftrees.c inflate_trees_bits, official source as is with
   an explicit z_stream layout; huft_build is FUN_0052bff0 (cdecl, 10 args). Residual: none. */
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
typedef void *(*alloc_func)(void *opaque, unsigned items, unsigned size);
typedef void (*free_func)(void *opaque, void *address);
typedef struct {
    unsigned char *next_in;     /* 0x00 */
    unsigned avail_in;          /* 0x04 */
    unsigned long total_in;     /* 0x08 */
    unsigned char *next_out;    /* 0x0c */
    unsigned avail_out;         /* 0x10 */
    unsigned long total_out;    /* 0x14 */
    char *msg;                  /* 0x18 */
    void *state;                /* 0x1c */
    alloc_func zalloc;          /* 0x20 */
    free_func zfree;            /* 0x24 */
    void *opaque;               /* 0x28 */
    int data_type;              /* 0x2c */
    unsigned long adler;        /* 0x30 */
    unsigned long reserved;     /* 0x34 */
} z_stream;
typedef unsigned long (*check_func)(unsigned long check, const unsigned char *buf, unsigned len);
MGLOBALA(char, s_incomplete_dynamic_bit_lengths_t_00623ac8, 36, 0x00623ac8);
MGLOBALA(char, s_oversubscribed_dynamic_bit_lengt_00623aec, 40, 0x00623aec);

typedef struct inflate_huft_s inflate_huft;
int FUN_0052bff0(unsigned *b, unsigned n, unsigned s, const unsigned *d, const unsigned *e, inflate_huft **t,
                 unsigned *m, inflate_huft *hp, unsigned *hn, unsigned *v);

// MATCH: 0x0052bf40 FUN_0052bf40
int FUN_0052bf40(unsigned *c, unsigned *bb, inflate_huft **tb, inflate_huft *hp, z_stream *z)
{
  int r;
  unsigned hn = 0;
  unsigned *v;

  if ((v = (unsigned *)(*(z->zalloc))(z->opaque, 19, sizeof(unsigned))) == 0)
    return -4;
  r = FUN_0052bff0(c, 19, 19, (unsigned *)0, (unsigned *)0,
                 tb, bb, hp, &hn, v);
  if (r == -3)
    z->msg = s_oversubscribed_dynamic_bit_lengt_00623aec;
  else if (r == -5 || *bb == 0)
  {
    z->msg = s_incomplete_dynamic_bit_lengths_t_00623ac8;
    r = -3;
  }
  (*(z->zfree))(z->opaque, v);
  return r;
}
