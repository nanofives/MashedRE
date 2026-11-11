// FLAGS: /O2 /Ob1 /G5
/* MATCH-RT2D-FONT 2026-11-11 0x00552b50: the camera render callback 0x00552a60 installs at camera+0x1c. Body is one
   `jmp dword ptr [0x00912c58]` (0x00552b50, 6 bytes): a tail call through the callback 0x00552a60 saved there. */
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
typedef int (*RenderCb_00552b50)(int camera);
MGLOBAL(RenderCb_00552b50, DAT_00912c58, 0x00912c58);
// MATCH: 0x00552b50 FUN_00552b50
int FUN_00552b50(int camera)
{
  return DAT_00912c58(camera);
}
