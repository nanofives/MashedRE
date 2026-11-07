// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00522ce0: zlib 1.1.3 inflate.c inflate (official text, three independent field stores reordered by the
   permuter: METHOD z->msg after sub.marker, BLOCKS sub.marker before mode, CHECK3 mode before need; same behaviour).
   66.6% (regs 77.1, ops 86.8) (62.0% with the official order). Residual: loop shape. The original reloads
   z->state at the switch head on every pass, including the first (no reuse of the null-check load), and keeps
   each 'mode = BAD; msg; sub.marker = 5; break' tail separate; ours peels the first state load into the entry and
   tail-merges the BAD tails. Tried: for(;;) form, permuter 500 variants. Class (b). */
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
typedef unsigned char Byte;
typedef unsigned int uInt;
typedef unsigned long uLong;
typedef Byte Bytef;
typedef char charf;
typedef int intf;
typedef uInt uIntf;
typedef uLong uLongf;
typedef void *voidpf;
typedef voidpf (*alloc_func) (voidpf opaque, uInt items, uInt size);
typedef void (*free_func) (voidpf opaque, voidpf address);
typedef void *voidp;
struct inflate_blocks_state;
typedef struct inflate_blocks_state inflate_blocks_statef;
typedef uLong (*check_func)(uLong check, const Bytef *buf, uInt len);
struct internal_state { int mode; union { uInt method; struct { uLong was; uLong need; } check; uInt marker; } sub;
  int nowrap; uInt wbits; inflate_blocks_statef *blocks; };
typedef struct z_stream_s { Bytef *next_in; uInt avail_in; uLong total_in; Bytef *next_out; uInt avail_out;
  uLong total_out; char *msg; struct internal_state *state; alloc_func zalloc; free_func zfree; voidpf opaque;
  int data_type; uLong adler; uLong reserved; } z_stream;
typedef z_stream *z_streamp;
typedef enum {
      METHOD, FLAG, DICT4, DICT3, DICT2, DICT1, DICT0, BLOCKS, CHECK4, CHECK3, CHECK2, CHECK1, DONE,
      BAD}
inflate_mode;
MGLOBALA(char, s_unknown_compression_method_00621cac, 27, 0x00621cac);
MGLOBALA(char, s_invalid_window_size_00621c98, 20, 0x00621c98);
MGLOBALA(char, s_incorrect_header_check_00621c80, 23, 0x00621c80);
MGLOBALA(char, s_need_dictionary_00621c58, 16, 0x00621c58);
MGLOBALA(char, s_incorrect_data_check_00621c68, 21, 0x00621c68);
int FUN_0052ab00(inflate_blocks_statef *s, z_streamp z, int r); /* inflate_blocks */
void FUN_0052a9c0(inflate_blocks_statef *s, z_streamp z, uLongf *c); /* inflate_blocks_reset */
#define NEEDBYTE {if(z->avail_in==0)return r;r=f;}
#define NEXTBYTE (z->avail_in--,z->total_in++,*z->next_in++)
// MATCH: 0x00522ce0 FUN_00522ce0
int FUN_00522ce0(z_streamp z, int f)
{
  int r;
  uInt b;

  if (z == 0 || z->state == 0 || z->next_in == 0)
    return -2;
  f = f == 4 ? -5 : 0;
  r = -5;
  while (1) switch (z->state->mode)
  {
    case METHOD:
      NEEDBYTE
      if (((z->state->sub.method = NEXTBYTE) & 0xf) != 8)
      {
        z->state->mode = BAD;
        z->state->sub.marker = 5;       
        z->msg = s_unknown_compression_method_00621cac;
        break;
      }
      if ((z->state->sub.method >> 4) + 8 > z->state->wbits)
      {
        z->state->mode = BAD;
        z->msg = s_invalid_window_size_00621c98;
        z->state->sub.marker = 5;       
        break;
      }
      z->state->mode = FLAG;
    case FLAG:
      NEEDBYTE
      b = NEXTBYTE;
      if (((z->state->sub.method << 8) + b) % 31)
      {
        z->state->mode = BAD;
        z->msg = s_incorrect_header_check_00621c80;
        z->state->sub.marker = 5;       
        break;
      }
      if (!(b & 0x20))
      {
        z->state->mode = BLOCKS;
        break;
      }
      z->state->mode = DICT4;
    case DICT4:
      NEEDBYTE
      z->state->sub.check.need = (uLong)NEXTBYTE << 24;
      z->state->mode = DICT3;
    case DICT3:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE << 16;
      z->state->mode = DICT2;
    case DICT2:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE << 8;
      z->state->mode = DICT1;
    case DICT1:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE;
      z->adler = z->state->sub.check.need;
      z->state->mode = DICT0;
      return 2;
    case DICT0:
      z->state->mode = BAD;
      z->msg = s_need_dictionary_00621c58;
      z->state->sub.marker = 0;       
      return -2;
    case BLOCKS:
      r = FUN_0052ab00(z->state->blocks, z, r);
      if (r == -3)
      {
        z->state->sub.marker = 0;       
        z->state->mode = BAD;
        break;
      }
      if (r == 0)
        r = f;
      if (r != 1)
        return r;
      r = f;
      FUN_0052a9c0(z->state->blocks, z, &z->state->sub.check.was);
      if (z->state->nowrap)
      {
        z->state->mode = DONE;
        break;
      }
      z->state->mode = CHECK4;
    case CHECK4:
      NEEDBYTE
      z->state->sub.check.need = (uLong)NEXTBYTE << 24;
      z->state->mode = CHECK3;
    case CHECK3:
      NEEDBYTE
      z->state->mode = CHECK2;
      z->state->sub.check.need += (uLong)NEXTBYTE << 16;
    case CHECK2:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE << 8;
      z->state->mode = CHECK1;
    case CHECK1:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE;

      if (z->state->sub.check.was != z->state->sub.check.need)
      {
        z->state->mode = BAD;
        z->msg = s_incorrect_data_check_00621c68;
        z->state->sub.marker = 5;       
        break;
      }
      z->state->mode = DONE;
    case DONE:
      return 1;
    case BAD:
      return -3;
    default:
      return -2;
  }
}
