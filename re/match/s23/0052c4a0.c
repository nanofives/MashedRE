// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052c4a0: 84.1% (regs 87.8%). zlib 1.1.3 inftrees.c inflate_trees_dynamic, official source with
   explicit z_stream layout (cplens/cplext/cpdist/cpdext at 0x005e4320/43a0/4420/4498; huft_build = FUN_0052bff0).
   Residual: the original pushes ebp and esi in the prologue; ours shrink-wraps them past the first return
   (every other instruction matches). Same pattern as 0x0052aa40; the sibling inflate_trees_bits 0x0052bf40 does
   shrink-wrap in the original. Permuter 114 compiles no improvement. Class (b). */
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
MGLOBALA(unsigned, DAT_005e4320, 31, 0x005e4320);
MGLOBALA(unsigned, DAT_005e43a0, 31, 0x005e43a0);
MGLOBALA(unsigned, DAT_005e4420, 30, 0x005e4420);
MGLOBALA(unsigned, DAT_005e4498, 30, 0x005e4498);
MGLOBALA(char, s_incomplete_literal_length_tree_00623b14, 31, 0x00623b14);
MGLOBALA(char, s_oversubscribed_literal_length_tr_00623b34, 35, 0x00623b34);
MGLOBALA(char, s_empty_distance_tree_with_lengths_00623b58, 33, 0x00623b58);
MGLOBALA(char, s_incomplete_distance_tree_00623b7c, 25, 0x00623b7c);
MGLOBALA(char, s_oversubscribed_distance_tree_00623b98, 29, 0x00623b98);

typedef struct inflate_huft_s inflate_huft;
int FUN_0052bff0(unsigned *b, unsigned n, unsigned s, const unsigned *d, const unsigned *e, inflate_huft **t,
                 unsigned *m, inflate_huft *hp, unsigned *hn, unsigned *v);

// MATCH: 0x0052c4a0 FUN_0052c4a0
int FUN_0052c4a0(unsigned nl, unsigned nd, unsigned *c, unsigned *bl, unsigned *bd, inflate_huft **tl,
                 inflate_huft **td, inflate_huft *hp, z_stream *z)
{
  int r;
  unsigned hn = 0;
  unsigned *v;

  if ((v = (unsigned *)(*(z->zalloc))(z->opaque, 288, sizeof(unsigned))) == 0)
    return -4;

  r = FUN_0052bff0(c, nl, 257, DAT_005e4320, DAT_005e43a0, tl, bl, hp, &hn, v);
  if (r != 0 || *bl == 0)
  {
    if (r == -3)
      z->msg = s_oversubscribed_literal_length_tr_00623b34;
    else if (r != -4)
    {
      z->msg = s_incomplete_literal_length_tree_00623b14;
      r = -3;
    }
    (*(z->zfree))(z->opaque, v);
    return r;
  }

  r = FUN_0052bff0(c + nl, nd, 0, DAT_005e4420, DAT_005e4498, td, bd, hp, &hn, v);
  if (r != 0 || (*bd == 0 && nl > 257))
  {
    if (r == -3)
      z->msg = s_oversubscribed_distance_tree_00623b98;
    else if (r == -5) {
      z->msg = s_incomplete_distance_tree_00623b7c;
      r = -3;
    }
    else if (r != -4)
    {
      z->msg = s_empty_distance_tree_with_lengths_00623b58;
      r = -3;
    }
    (*(z->zfree))(z->opaque, v);
    return r;
  }

  (*(z->zfree))(z->opaque, v);
  return 0;
}
