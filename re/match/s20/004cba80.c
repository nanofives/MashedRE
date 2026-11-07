// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cba80 (h2): 90.3% (regs 90.3%). Raw 47.1%. 12-byte entry table {obj, a, b}, Release through a
   __stdcall slot +8, unsigned return compared <= 0 (gives the original's ja). Tried: break vs return, pointer walk
   (77.4%), while-with-precheck (54.5%), local obj copy, permuter (no gain). Residual (b): the original pushes
   esi/edi before the count test and routes the empty-table case through the shared epilogue; ours returns before
   the pushes. */
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
typedef struct Unk Unk;
typedef unsigned long (__stdcall *relfn)(Unk *);
struct Unk { relfn *vt; };
typedef struct { Unk *obj; int a; int b; } Ent;
MGLOBAL(unsigned int, DAT_007d4144, 0x007d4144);
MGLOBAL(Ent *, DAT_007d414c, 0x007d414c);
// MATCH: 0x004cba80 FUN_004cba80
void FUN_004cba80(Unk *param_1)
{
  unsigned int i;
  for (i = 0; i < DAT_007d4144; i++) {
    if (DAT_007d414c[i].obj == param_1) {
      if (DAT_007d414c[i].obj->vt[2](DAT_007d414c[i].obj) <= 0) {
        DAT_007d414c[i].obj = 0;
      }
      return;
    }
  }
}
