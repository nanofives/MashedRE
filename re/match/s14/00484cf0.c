// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00484cf0 (h2): world-object registrar. 100% / whole obj symbol BYTE-IDENTICAL (237 B; Ghidra's
   size 231 stops before the `mov eax,1; pop esi; ret` tail at 0x00484dd6). Raw 40.5%. Moved: count cached in a
   local (esi), fields written as DAT_006dccb8[n].f (not through an obj pointer), Vec3 struct copy for +0..+8,
   the f24 clear as Ghidra's pointer form (store [16], then 16 pre-decrement stores; `for (i=16;i>=0;i--)`
   gives a 0x11 count). Residual: none. */
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
typedef struct Vec3 { float x, y, z; } Vec3;
typedef struct WorldObjDesc {
  Vec3 pos;
  int f0c;
  int f10;
  int f14;
  int f18;
  int f1c;
} WorldObjDesc;
typedef struct WorldObj {
  Vec3 pos;
  int f0c;
  int f10;
  int f14;
  int f18;
  int f1c;
  short idx;
  short pad22;
  int f24[17];
  int f68[9];
} WorldObj;
MGLOBALA(WorldObj, DAT_006dccb8, 300, 0x006dccb8);
MGLOBAL(int, DAT_006e70d4, 0x006e70d4);
MGLOBAL(int, DAT_006e70c8, 0x006e70c8);
MGLOBAL(uint, DAT_006e70cc, 0x006e70cc);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
void FUN_004847d0(void);
int FUN_00484ac0(WorldObj *obj);
// MATCH: 0x00484cf0 FUN_00484cf0
int FUN_00484cf0(WorldObjDesc *desc)
{
  uint n;
  int *p;
  int i;
  if (DAT_006e70d4 == 0 && DAT_006e70c8 != 0) {
    FUN_004847d0();
  }
  n = DAT_006e70cc;
  if (n >= 300) {
    return 0;
  }
  DAT_006dccb8[n].pos = desc->pos;
  DAT_006dccb8[n].f0c = desc->f0c;
  DAT_006dccb8[n].f10 = desc->f10;
  DAT_006dccb8[n].f14 = desc->f14;
  DAT_006dccb8[n].f18 = desc->f18;
  DAT_006dccb8[n].f1c = desc->f1c;
  p = &DAT_006dccb8[n].f24[16];
  DAT_006dccb8[n].idx = (short)n;
  *p = 0;
  for (i = 16; i != 0; i--) {
    p--;
    *p = 0;
  }
  memset(DAT_006dccb8[n].f68, 0, sizeof(DAT_006dccb8[n].f68));
  if (DAT_006e70c8 != 0 && FUN_00484ac0(&DAT_006dccb8[n]) == 0) {
    return 0;
  }
  DAT_006e70cc++;
  return 1;
}
