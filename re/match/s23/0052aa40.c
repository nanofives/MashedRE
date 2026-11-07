// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052aa40: 89.3% (regs 89.3%; h7 round 2, was 82.1). zlib 1.1.3 infblock.c inflate_blocks_new with
   inflate_blocks_reset (c = Z_NULL) written inline; the reset's mode tests fold away after s->mode = TYPE.
   What moved it: the first test written `if ((s = ZALLOC(...)) != 0) {} else return s;` (same semantics). With
   the official `== 0) return s;` our c2 lays the return out at the fallthrough and shrink-wraps ebx below the
   first two returns; with the else-return form it pushes ebx in the prologue as the original does.
   Residual: our c2 then moves that return to the shared return-0 tail (constant s == 0, je to the epilogue),
   the original keeps it inline at the fallthrough with its own pop/ret; the second return's xor eax,eax is
   scheduled after the pops. In this c2 inline exit <=> shrink-wrapped and moved exit <=> no shrink-wrap, the
   original has inline exits without the shrink-wrap. Tried (unsigned)s < 1 (inline again, 85.7), goto,
   braces, flags (round 1: /O2 /GS /Ob1 72.6, /Ox, /Ob0, /Ob2, /G3-/G6, /Oy-), permuter 147 compiles.
   Class (a). */
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
typedef struct {
    int mode;                   /* 0x00 */
    char sub[0x18];             /* 0x04 */
    unsigned bitk;              /* 0x1c */
    unsigned long bitb;         /* 0x20 */
    void *hufts;                /* 0x24 */
    unsigned char *window;      /* 0x28 */
    unsigned char *end;         /* 0x2c */
    unsigned char *read;        /* 0x30 */
    unsigned char *write;       /* 0x34 */
    check_func checkfn;         /* 0x38 */
    unsigned long check;        /* 0x3c */
} inflate_blocks_state;

// MATCH: 0x0052aa40 FUN_0052aa40
inflate_blocks_state *FUN_0052aa40(z_stream *z, check_func c, unsigned w)
{
  inflate_blocks_state *s;

  if ((s = (inflate_blocks_state *)(*(z->zalloc))(z->opaque, 1, 0x40)) != 0) {} else return s;
  if ((s->hufts = (*(z->zalloc))(z->opaque, 8, 0x5a0)) == 0)
  {
    (*(z->zfree))(z->opaque, s);
    return 0;
  }
  if ((s->window = (unsigned char *)(*(z->zalloc))(z->opaque, 1, w)) == 0)
  {
    (*(z->zfree))(z->opaque, s->hufts);
    (*(z->zfree))(z->opaque, s);
    return 0;
  }
  s->end = s->window + w;
  s->checkfn = c;
  s->mode = 0;
  s->mode = 0;
  s->bitk = 0;
  s->bitb = 0;
  s->read = s->write = s->window;
  if (s->checkfn != 0)
    z->adler = s->check = (*s->checkfn)(0L, 0, 0);
  return s;
}
