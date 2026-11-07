// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052ab00: 32.2% (regs 50.3%, ops 68.4%). zlib 1.1.3 infblock.c inflate_blocks, official source with
   the official inflate_blocks_state layout and infutil.h macros; callees: inflate_codes_new FUN_0052b7e0,
   inflate_codes FUN_0052b820, inflate_codes_free FUN_0052bf20, inflate_trees_bits FUN_0052bf40,
   inflate_trees_dynamic FUN_0052c4a0, inflate_trees_fixed FUN_0052c630, inflate_flush FUN_0052c660; border at
   0x005e42a0, zmemcpy as the memcpy intrinsic. One pass (function > 1000 B, raw g2s 17.8%). What moved it:
   r copied from an r_arg parameter (29.3 -> 32.2). Residual: register / frame assignment from the prologue on
   (the original loads z and its next_in/avail_in before the bit-buffer loads and tests mode with ja-to-default
   before the jump table) and the code that follows from it. /O2 /GS /Ob1 6.8. Class (b) (budget). */
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
typedef z_stream *z_streamp;
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
typedef struct inflate_codes_state inflate_codes_statef;
#define exop word.what.Exop
#define bits word.what.Bits
typedef enum {
      TYPE,     /* get type bits (3, including end bit) */
      LENS,     /* get lengths for stored */
      STORED,   /* processing stored block */
      TABLE,    /* get table lengths */
      BTREE,    /* get bit lengths tree for a dynamic block */
      DTREE,    /* get length, distance trees for a dynamic block */
      CODES,    /* processing fixed or dynamic block */
      DRY,      /* output remaining window bytes */
      DONE,     /* finished last block, done */
      BAD}      /* got a data error--stuck here */
inflate_block_mode;
struct inflate_blocks_state {

  /* mode */
  inflate_block_mode  mode;     /* current inflate_block mode */

  /* mode dependent information */
  union {
    uInt left;          /* if STORED, bytes left to copy */
    struct {
      uInt table;               /* table lengths (14 bits) */
      uInt index;               /* index into blens (or border) */
      uIntf *blens;             /* bit lengths of codes */
      uInt bb;                  /* bit length tree depth */
      inflate_huft *tb;         /* bit length decoding tree */
    } trees;            /* if DTREE, decoding info for trees */
    struct {
      inflate_codes_statef 
         *codes;
    } decode;           /* if CODES, current state */
  } sub;                /* submode */
  uInt last;            /* true if this block is the last block */

  /* mode independent information */
  uInt bitk;            /* bits in bit buffer */
  uLong bitb;           /* bit buffer */
  inflate_huft *hufts;  /* single malloc for tree space */
  Bytef *window;        /* sliding window */
  Bytef *end;           /* one byte after sliding window */
  Bytef *read;          /* window read pointer */
  Bytef *write;         /* window write pointer */
  check_func checkfn;   /* check function */
  uLong check;          /* check on output */

};
typedef struct inflate_blocks_state inflate_blocks_statef;
#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return FUN_0052c660(s,z,r);}
/*   get bytes and bits */
#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEEDBYTE {if(n)r=0;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(j) {b>>=(j);k-=(j);}
/*   output bytes */
#define WAVAIL (uInt)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(uInt)WAVAIL;}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)WAVAIL;}}
#define FLUSH {UPDOUT r=FUN_0052c660(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=0;}
#define OUTBYTE(a) {*q++=(Byte)(a);m--;}
/*   load local pointers */
#define LOAD {LOADIN LOADOUT}

MGLOBALA(uInt, DAT_005e42a0, 19, 0x005e42a0);  /* border */
MGLOBALA(uInt, DAT_00623bb8, 17, 0x00623bb8);  /* inflate_mask */
MGLOBALA(char, s_invalid_bit_length_repeat_00622914, 26, 0x00622914);
MGLOBALA(char, s_too_many_length_or_distance_symb_00622930, 36, 0x00622930);
MGLOBALA(char, s_invalid_stored_block_lengths_00622954, 29, 0x00622954);
MGLOBALA(char, s_invalid_block_type_00622974, 19, 0x00622974);
#define border DAT_005e42a0
#define inflate_mask DAT_00623bb8

inflate_codes_statef *FUN_0052b7e0(uInt bl, uInt bd, inflate_huft *tl, inflate_huft *td, z_streamp z);
int FUN_0052b820(inflate_blocks_statef *s, z_streamp z, int r);
void FUN_0052bf20(inflate_codes_statef *c, z_streamp z);
int FUN_0052bf40(uIntf *c, uIntf *bb, inflate_huft **tb, inflate_huft *hp, z_streamp z);
int FUN_0052c4a0(uInt nl, uInt nd, uIntf *c, uIntf *bl, uIntf *bd, inflate_huft **tl, inflate_huft **td,
                 inflate_huft *hp, z_streamp z);
int FUN_0052c630(uIntf *bl, uIntf *bd, inflate_huft **tl, inflate_huft **td, z_streamp z);
int FUN_0052c660(inflate_blocks_statef *s, z_streamp z, int r);
void *memcpy(void *dst, const void *src, unsigned int n);
#pragma intrinsic(memcpy)
#define zmemcpy memcpy
#define ZALLOC(strm, items, size) (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (void *)(addr))

// MATCH: 0x0052ab00 FUN_0052ab00
int FUN_0052ab00(inflate_blocks_statef *s, z_streamp z, int r_arg)
{
   int r = r_arg;
  uInt t;               /* temporary storage */
  uLong b;              /* bit buffer */
  uInt k;               /* bits in bit buffer */
  Bytef *p;             /* input data pointer */
  uInt n;               /* bytes available there */
  Bytef *q;             /* output window write pointer */
  uInt m;               /* bytes to end of window or read pointer */

  /* copy input/output information to locals (UPDATE macro restores) */
  LOAD

  /* process input based on current state */
  while (1) switch (s->mode)
  {
    case TYPE:
      NEEDBITS(3)
      t = (uInt)b & 7;
      s->last = t & 1;
      switch (t >> 1)
      {
        case 0:                         /* stored */
          DUMPBITS(3)
          t = k & 7;                    /* go to byte boundary */
          DUMPBITS(t)
          s->mode = LENS;               /* get length of stored block */
          break;
        case 1:                         /* fixed */
          {
            uInt bl, bd;
            inflate_huft *tl, *td;

            FUN_0052c630(&bl, &bd, &tl, &td, z);
            s->sub.decode.codes = FUN_0052b7e0(bl, bd, tl, td, z);
            if (s->sub.decode.codes == 0)
            {
              r = (-4);
              LEAVE
            }
          }
          DUMPBITS(3)
          s->mode = CODES;
          break;
        case 2:                         /* dynamic */
          DUMPBITS(3)
          s->mode = TABLE;
          break;
        case 3:                         /* illegal */
          DUMPBITS(3)
          s->mode = BAD;
          z->msg = s_invalid_block_type_00622974;
          r = (-3);
          LEAVE
      }
      break;
    case LENS:
      NEEDBITS(32)
      if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
      {
        s->mode = BAD;
        z->msg = s_invalid_stored_block_lengths_00622954;
        r = (-3);
        LEAVE
      }
      s->sub.left = (uInt)b & 0xffff;
      b = k = 0;                      /* dump bits */
      s->mode = s->sub.left ? STORED : (s->last ? DRY : TYPE);
      break;
    case STORED:
      if (n == 0)
        LEAVE
      NEEDOUT
      t = s->sub.left;
      if (t > n) t = n;
      if (t > m) t = m;
      zmemcpy(q, p, t);
      p += t;  n -= t;
      q += t;  m -= t;
      if ((s->sub.left -= t) != 0)
        break;
      s->mode = s->last ? DRY : TYPE;
      break;
    case TABLE:
      NEEDBITS(14)
      s->sub.trees.table = t = (uInt)b & 0x3fff;
      if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
      {
        s->mode = BAD;
        z->msg = s_too_many_length_or_distance_symb_00622930;
        r = (-3);
        LEAVE
      }
      t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
      if ((s->sub.trees.blens = (uIntf*)ZALLOC(z, t, sizeof(uInt))) == 0)
      {
        r = (-4);
        LEAVE
      }
      DUMPBITS(14)
      s->sub.trees.index = 0;
      s->mode = BTREE;
    case BTREE:
      while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
      {
        NEEDBITS(3)
        s->sub.trees.blens[border[s->sub.trees.index++]] = (uInt)b & 7;
        DUMPBITS(3)
      }
      while (s->sub.trees.index < 19)
        s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
      s->sub.trees.bb = 7;
      t = FUN_0052bf40(s->sub.trees.blens, &s->sub.trees.bb,
                             &s->sub.trees.tb, s->hufts, z);
      if (t != 0)
      {
        ZFREE(z, s->sub.trees.blens);
        r = t;
        if (r == (-3))
          s->mode = BAD;
        LEAVE
      }
      s->sub.trees.index = 0;
      s->mode = DTREE;
    case DTREE:
      while (t = s->sub.trees.table,
             s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
      {
        inflate_huft *h;
        uInt i, j, c;

        t = s->sub.trees.bb;
        NEEDBITS(t)
        h = s->sub.trees.tb + ((uInt)b & inflate_mask[t]);
        t = h->bits;
        c = h->base;
        if (c < 16)
        {
          DUMPBITS(t)
          s->sub.trees.blens[s->sub.trees.index++] = c;
        }
        else /* c == 16..18 */
        {
          i = c == 18 ? 7 : c - 14;
          j = c == 18 ? 11 : 3;
          NEEDBITS(t + i)
          DUMPBITS(t)
          j += (uInt)b & inflate_mask[i];
          DUMPBITS(i)
          i = s->sub.trees.index;
          t = s->sub.trees.table;
          if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
              (c == 16 && i < 1))
          {
            ZFREE(z, s->sub.trees.blens);
            s->mode = BAD;
            z->msg = s_invalid_bit_length_repeat_00622914;
            r = (-3);
            LEAVE
          }
          c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
          do {
            s->sub.trees.blens[i++] = c;
          } while (--j);
          s->sub.trees.index = i;
        }
      }
      s->sub.trees.tb = 0;
      {
        uInt bl, bd;
        inflate_huft *tl, *td;
        inflate_codes_statef *c;

        bl = 9;         /* must be <= 9 for lookahead assumptions */
        bd = 6;         /* must be <= 9 for lookahead assumptions */
        t = s->sub.trees.table;
        t = FUN_0052c4a0(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
                                  s->sub.trees.blens, &bl, &bd, &tl, &td,
                                  s->hufts, z);
        ZFREE(z, s->sub.trees.blens);
        if (t != 0)
        {
          if (t == (uInt)(-3))
            s->mode = BAD;
          r = t;
          LEAVE
        }
        if ((c = FUN_0052b7e0(bl, bd, tl, td, z)) == 0)
        {
          r = (-4);
          LEAVE
        }
        s->sub.decode.codes = c;
      }
      s->mode = CODES;
    case CODES:
      UPDATE
      if ((r = FUN_0052b820(s, z, r)) != 1)
        return FUN_0052c660(s, z, r);
      r = 0;
      FUN_0052bf20(s->sub.decode.codes, z);
      LOAD
      if (!s->last)
      {
        s->mode = TYPE;
        break;
      }
      s->mode = DRY;
    case DRY:
      FLUSH
      if (s->read != s->write)
        LEAVE
      s->mode = DONE;
    case DONE:
      r = 1;
      LEAVE
    case BAD:
      r = (-3);
      LEAVE
    default:
      r = (-2);
      LEAVE
  }
}
#undef border
#undef inflate_mask
#undef zmemcpy
#undef ZALLOC
#undef ZFREE
#undef UPDBITS
#undef UPDIN
#undef UPDOUT
#undef UPDATE
#undef LEAVE
#undef LOADIN
#undef NEEDBYTE
#undef NEXTBYTE
#undef NEEDBITS
#undef DUMPBITS
#undef WAVAIL
#undef LOADOUT
#undef WRAP
#undef FLUSH
#undef NEEDOUT
#undef OUTBYTE
#undef LOAD
#undef exop
#undef bits
