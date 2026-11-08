// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052b820: 45.5% (regs 57.0%, ops 87.8%). zlib 1.1.3 infcodes.c inflate_codes, official source with the
   infutil.h macros (inflate_flush = FUN_0052c660, inflate_fast = FUN_0052c7a0, inflate_mask 0x00623bb8); the
   official inflate_codes_state / inflate_blocks_state layouts match (c = s->sub.decode.codes at s+4). One pass
   (function > 1000 B). The prologue and the START fast path match instruction for instruction; the residual is
   register / frame-slot assignment after that (the original keeps m in a frame slot and opens 0x10 bytes, ours
   keeps m in a register and opens 0x14) and the code that follows from it. Tried argument copies (8 subsets,
   no gain), /O2 /GS /Ob1 (39.5), permuter 331 compiles (no gain). Class (b). */
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
typedef unsigned char Byte;
typedef unsigned char Bytef;
typedef unsigned int uInt;
typedef unsigned long uLong;
typedef unsigned int uIntf;
typedef struct inflate_huft_s inflate_huft;
struct inflate_huft_s {
  union {
    struct {
      Byte Exop;
      Byte Bits;
    } what;
    uInt pad;
  } word;
  uInt base;
};
#define exop word.what.Exop
#define bits word.what.Bits
typedef struct {
    int mode;                   /* 0x00 */
    char sub[0x18];             /* 0x04 */
    uInt bitk;                  /* 0x1c */
    uLong bitb;                 /* 0x20 */
    inflate_huft *hufts;        /* 0x24 */
    Bytef *window;              /* 0x28 */
    Bytef *end;                 /* 0x2c */
    Bytef *read;                /* 0x30 */
    Bytef *write;               /* 0x34 */
    check_func checkfn;         /* 0x38 */
    uLong check;                /* 0x3c */
} inflate_blocks_statef;
typedef z_stream *z_streamp;

#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEXTBYTE (n--,*p++)
#define DUMPBITS(j) {b>>=(j);k-=(j);}
#define WAVAIL (uInt)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(uInt)WAVAIL;}
#define LOAD {LOADIN LOADOUT}
#define NEEDBYTE {if(n)r=0;else LEAVE}
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)WAVAIL;}}
#define FLUSH {UPDOUT r=FUN_0052c660(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=0;}
#define OUTBYTE(a) {*q++=(Byte)(a);m--;}
#define LEAVE {UPDATE return FUN_0052c660(s,z,r);}
typedef enum {        /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
      START,    /* x: set up for LEN */
      LEN,      /* i: get length/literal/eob next */
      LENEXT,   /* i: getting length extra (have base) */
      DIST,     /* i: get distance next */
      DISTEXT,  /* i: getting distance extra */
      COPY,     /* o: copying bytes in window, waiting for space */
      LIT,      /* o: got literal, waiting for output space */
      WASH,     /* o: got eob, possibly still output waiting */
      END,      /* x: got eob and all data flushed */
      BADCODE}  /* x: got error */
inflate_codes_mode;

/* inflate codes private state */
struct inflate_codes_state {

  /* mode */
  inflate_codes_mode mode;      /* current inflate_codes mode */

  /* mode dependent information */
  uInt len;
  union {
    struct {
      inflate_huft *tree;       /* pointer into tree */
      uInt need;                /* bits needed */
    } code;             /* if LEN or DIST, where in tree */
    uInt lit;           /* if LIT, literal */
    struct {
      uInt get;                 /* bits to get for extra */
      uInt dist;                /* distance back to copy from */
    } copy;             /* if EXT or COPY, where and how much */
  } sub;                /* submode */

  /* mode independent information */
  Byte lbits;           /* ltree bits decoded per branch */
  Byte dbits;           /* dtree bits decoder per branch */
  inflate_huft *ltree;          /* literal/length/eob tree */
  inflate_huft *dtree;          /* distance tree */

};
typedef struct inflate_codes_state inflate_codes_statef;

MGLOBALA(char, s_invalid_literal_length_code_00622988, 28, 0x00622988);
MGLOBALA(char, s_invalid_distance_code_006229a4, 22, 0x006229a4);
MGLOBALA(uInt, DAT_00623bb8, 17, 0x00623bb8);
#define inflate_mask DAT_00623bb8

int FUN_0052c660(inflate_blocks_statef *s, z_streamp z, int r);
int FUN_0052c7a0(uInt bl, uInt bd, inflate_huft *tl, inflate_huft *td, inflate_blocks_statef *s, z_streamp z);

// MATCH: 0x0052b820 FUN_0052b820
int FUN_0052b820(inflate_blocks_statef *s, z_streamp z, int r)
{
  uInt j;               /* temporary storage */
  inflate_huft *t;      /* temporary pointer */
  uInt e;               /* extra bits or operation */
  uLong b;              /* bit buffer */
  uInt k;               /* bits in bit buffer */
  Bytef *p;             /* input data pointer */
  uInt n;               /* bytes available there */
  Bytef *q;             /* output window write pointer */
  uInt m;               /* bytes to end of window or read pointer */
  Bytef *f;             /* pointer to copy strings from */
  inflate_codes_statef *c = *(inflate_codes_statef **)s->sub;  /* codes state */

  /* copy input/output information to locals (UPDATE macro restores) */
  LOAD

  /* process input and output based on current state */
  while (1) switch (c->mode)
  {             /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
    case START:         /* x: set up for LEN */
      if (m >= 258 && n >= 10)
      {
        UPDATE
        r = FUN_0052c7a0(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
        LOAD
        if (r != 0)
        {
          c->mode = r == 1 ? WASH : BADCODE;
          break;
        }
      }
      c->sub.code.need = c->lbits;
      c->sub.code.tree = c->ltree;
      c->mode = LEN;
    case LEN:           /* i: get length/literal/eob next */
      j = c->sub.code.need;
      NEEDBITS(j)
      t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
      DUMPBITS(t->bits)
      e = (uInt)(t->exop);
      if (e == 0)               /* literal */
      {
        c->sub.lit = t->base;
        c->mode = LIT;
        break;
      }
      if (e & 16)               /* length */
      {
        c->sub.copy.get = e & 15;
        c->len = t->base;
        c->mode = LENEXT;
        break;
      }
      if ((e & 64) == 0)        /* next table */
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      if (e & 32)               /* end of block */
      {
        c->mode = WASH;
        break;
      }
      c->mode = BADCODE;        /* invalid code */
      z->msg = s_invalid_literal_length_code_00622988;
      r = (-3);
      LEAVE
    case LENEXT:        /* i: getting length extra (have base) */
      j = c->sub.copy.get;
      NEEDBITS(j)
      c->len += (uInt)b & inflate_mask[j];
      DUMPBITS(j)
      c->sub.code.need = c->dbits;
      c->sub.code.tree = c->dtree;
      c->mode = DIST;
    case DIST:          /* i: get distance next */
      j = c->sub.code.need;
      NEEDBITS(j)
      t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
      DUMPBITS(t->bits)
      e = (uInt)(t->exop);
      if (e & 16)               /* distance */
      {
        c->sub.copy.get = e & 15;
        c->sub.copy.dist = t->base;
        c->mode = DISTEXT;
        break;
      }
      if ((e & 64) == 0)        /* next table */
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      c->mode = BADCODE;        /* invalid code */
      z->msg = s_invalid_distance_code_006229a4;
      r = (-3);
      LEAVE
    case DISTEXT:       /* i: getting distance extra */
      j = c->sub.copy.get;
      NEEDBITS(j)
      c->sub.copy.dist += (uInt)b & inflate_mask[j];
      DUMPBITS(j)
      c->mode = COPY;
    case COPY:          /* o: copying bytes in window, waiting for space */
      f = (uInt)(q - s->window) < c->sub.copy.dist ?
          s->end - (c->sub.copy.dist - (q - s->window)) :
          q - c->sub.copy.dist;
      while (c->len)
      {
        NEEDOUT
        OUTBYTE(*f++)
        if (f == s->end)
          f = s->window;
        c->len--;
      }
      c->mode = START;
      break;
    case LIT:           /* o: got literal, waiting for output space */
      NEEDOUT
      OUTBYTE(c->sub.lit)
      c->mode = START;
      break;
    case WASH:          /* o: got eob, possibly more output */
      if (k > 7)        /* return unused byte, if any */
      {
        k -= 8;
        n++;
        p--;            /* can always return one */
      }
      FLUSH
      if (s->read != s->write)
        LEAVE
      c->mode = END;
    case END:
      r = 1;
      LEAVE
    case BADCODE:       /* x: got error */
      r = (-3);
      LEAVE
    default:
      r = (-2);
      LEAVE
  }
}
#undef inflate_mask
#undef NEEDBYTE
#undef NEEDBITS
#undef WRAP
#undef FLUSH
#undef NEEDOUT
#undef OUTBYTE
#undef LEAVE
#undef exop
#undef bits
#undef UPDBITS
#undef UPDIN
#undef UPDOUT
#undef UPDATE
#undef LOADIN
#undef NEXTBYTE
#undef DUMPBITS
#undef WAVAIL
#undef LOADOUT
#undef LOAD
