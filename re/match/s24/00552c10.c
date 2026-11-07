// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x00552c10: 100% (regs 100%). Floats typed float (immediate 1.0f/0.5f stores), the 0x00912a44 block as an RwMatrix-shaped struct, and the globals 0x00912a84..0x00913208 as ONE struct St at 0x00912a84 (stack[32], depth, fields up to +0x784): with separate globals the compiler hoisted the c0c/bf8/3204 stores above the rep stosd (97.9%), as fields of one object it keeps them after it. The stack[1..31] clear is a plain loop (memset intrinsic gave the same code). */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct Mat {
  V3 right; unsigned int flags;
  V3 up; unsigned int pad1;
  V3 at; unsigned int pad2;
  V3 pos; unsigned int pad3;
} Mat;
MGLOBALA(short, DAT_00912500, 256, 0x00912500);
MGLOBAL(Mat, DAT_00912a44, 0x00912a44);
typedef struct St {
  int stack[32];
  int depth;
  char pad0[0xd0];
  int f_bd8;
  char pad1[0x4];
  float f_be0;
  float f_be4;
  char pad2[0x4];
  int f_bec;
  int f_bf0;
  char pad3[0x4];
  float f_bf8;
  float f_bfc;
  float f_c00;
  float f_c04;
  char pad4[0x4];
  int f_c0c;
  char pad5[0x5f4];
  int f_3204;
  int f_3208;
} St;
MGLOBAL(St, DAT_00912a84, 0x00912a84);
int __cdecl FUN_004c57a0(void);
int __cdecl FUN_00552750(void);
// MATCH: 0x00552c10 FUN_00552c10
int FUN_00552c10(void)
{
  int i;
  DAT_00912a84.depth = 0;
  DAT_00912a84.stack[0] = FUN_004c57a0();
  FUN_00552750();
  DAT_00912a84.f_be4 = 0.5f;
  DAT_00912a44.flags |= 0x20003;
  DAT_00912a84.f_bec = 0;
  DAT_00912a84.f_be0 = 1.0f;
  DAT_00912a84.f_bd8 = 0;
  DAT_00912a84.f_bf0 = 7;
  DAT_00912a44.at.z = 1.0f;
  DAT_00912a44.up.y = 1.0f;
  DAT_00912a44.right.x = 1.0f;
  DAT_00912a44.up.x = 0;
  DAT_00912a44.right.z = 0;
  DAT_00912a44.right.y = 0;
  DAT_00912a44.at.y = 0;
  DAT_00912a44.at.x = 0;
  DAT_00912a44.up.z = 0;
  DAT_00912a44.pos.z = 0;
  DAT_00912a44.pos.y = 0;
  DAT_00912a44.pos.x = 0;
  for (i = 0; i < 256; i++) DAT_00912500[i] = (short)i;
  for (i = 1; i < 32; i++) DAT_00912a84.stack[i] = 0;
  DAT_00912a84.f_c0c = 0;
  DAT_00912a84.f_bf8 = 0.0f;
  DAT_00912a84.f_bfc = 0.0f;
  DAT_00912a84.f_3204 = 0;
  DAT_00912a84.f_3208 = 0;
  DAT_00912a84.f_c00 = 1.0f;
  DAT_00912a84.f_c04 = 1.0f;
  return 1;
}
