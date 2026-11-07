// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00558100 (h1): 100% (regs 100%). The early `xor edx,edx` is a `result = 0` local set to the object
   only inside the non-NULL branch. The `mov edx,imm; test edx,edx` pair is the RW SetRenderCallBack macro form:
   store the callback, then re-test the field and store the default (FUN_004e5f90) when it is NULL. */
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
typedef int (*RenderCB)(int atomic);
int FUN_00557b70(int atomic);
int FUN_004e5f90(int atomic);
MGLOBAL(int, DAT_00913274, 0x00913274);

typedef struct Ext { int set; RenderCB saved; } Ext;

// MATCH: 0x00558100 FUN_00558100
int FUN_00558100(int param_1)
{
  int result = 0;
  Ext *ext = *(Ext **)(param_1 + DAT_00913274);
  if (ext != 0) {
    if (ext->set == 0) {
      ext->saved = *(RenderCB *)(param_1 + 0x48);
      *(RenderCB *)(param_1 + 0x48) = FUN_00557b70;
      if (*(RenderCB *)(param_1 + 0x48) == 0) {
        *(RenderCB *)(param_1 + 0x48) = FUN_004e5f90;
      }
      ext->set = 1;
    }
    result = param_1;
  }
  return result;
}
