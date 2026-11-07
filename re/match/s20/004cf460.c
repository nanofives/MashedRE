// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cf460 (h4): 100% (regs 100%). RwTextureStreamGetSize shape: struct chunk 4+0xc, name (+0x10) and
   mask (+0x30) string sizes + 0xc each, plugin registry (0x00618138) size + 0xc. Plugin registry declared as a char
   array (only its address is used). */
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
MGLOBALA(char, DAT_00618138, 4, 0x00618138);
int FUN_004d8770(const char *string);
int FUN_004e1ce0(void *reg, const void *object);

// MATCH: 0x004cf460 FUN_004cf460
int FUN_004cf460(char *texture)
{
  int size;
  size = 4 + 0xc;
  size += FUN_004d8770(texture + 0x10) + 0xc;
  size += FUN_004d8770(texture + 0x30) + 0xc;
  size += FUN_004e1ce0(DAT_00618138, texture) + 0xc;
  return size;
}
