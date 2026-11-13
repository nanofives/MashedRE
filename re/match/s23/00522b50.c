// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00522b50: zlib 1.1.3 inflate.c inflateInit2_, with inflateEnd and inflateReset inlined
   (written here as static __forceinline copies above the function; plain static / __inline are not inlined
   by this compiler even at /Ob2, 51.8%).
   81.8% (regs 81.8, ops 92.7). Residual: block layout only. The original keeps a separate return epilogue for
   each early return (z==NULL -2 and zalloc-fail -4 fall through inline) while ours tail-merges them with the
   inflateEnd paths. Tried: else-nesting, inflateEnd result in a variable, /GS, /Ob2, permuter 111 variants.
   Class (b) (inlining form: the original inlined these with the compiler, not by hand). */
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
MGLOBALA(char, s_1_1_3_006214b0, 6, 0x006214b0); /* ZLIB_VERSION */
voidpf FUN_00528e00(voidpf opaque, unsigned items, unsigned size); /* zcalloc */
void FUN_00528e20(voidpf opaque, voidpf ptr); /* zcfree */
uLong FUN_0052a890(uLong adler, const Bytef *buf, uInt len); /* adler32 */
inflate_blocks_statef *FUN_0052aa40(z_streamp z, check_func c, uInt w); /* inflate_blocks_new */
void FUN_0052a9c0(inflate_blocks_statef *s, z_streamp z, uLongf *c); /* inflate_blocks_reset */
int FUN_0052b750(inflate_blocks_statef *s, z_streamp z); /* inflate_blocks_free */

static __forceinline int inflateReset(z_streamp z)
{
  if (z == 0 || z->state == 0)
    return -2;
  z->total_in = z->total_out = 0;
  z->msg = 0;
  z->state->mode = z->state->nowrap ? 7 : 0;
  FUN_0052a9c0(z->state->blocks, z, 0);
  return 0;
}

static __forceinline int inflateEnd(z_streamp z)
{
  if (z == 0 || z->state == 0 || z->zfree == 0)
    return -2;
  if (z->state->blocks != 0)
    FUN_0052b750(z->state->blocks, z);
  (*(z->zfree))(z->opaque, (voidpf)(z->state));
  z->state = 0;
  return 0;
}

// MATCH: 0x00522b50 FUN_00522b50
int FUN_00522b50(z_streamp z, int w, const char *version, int stream_size)
{
  if (version == 0 || version[0] != s_1_1_3_006214b0[0] ||
      stream_size != sizeof(z_stream))
      return -6;

  if (z == 0)
    return -2;
  z->msg = 0;
  if (z->zalloc == 0)
  {
    z->zalloc = FUN_00528e00;
    z->opaque = (voidpf)0;
  }
  if (z->zfree == 0) z->zfree = FUN_00528e20;
  if ((z->state = (struct internal_state *)
       (*(z->zalloc))(z->opaque, 1, sizeof(struct internal_state))) == 0)
    return -4;
  z->state->blocks = 0;

  z->state->nowrap = 0;
  if (w < 0)
  {
    w = - w;
    z->state->nowrap = 1;
  }

  if (w < 8 || w > 15)
  {
    inflateEnd(z);
    return -2;
  }
  z->state->wbits = (uInt)w;

  if ((z->state->blocks =
      FUN_0052aa40(z, z->state->nowrap ? 0 : FUN_0052a890, (uInt)1 << w))
      == 0)
  {
    inflateEnd(z);
    return -4;
  }

  inflateReset(z);
  return 0;
}
