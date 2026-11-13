// TOOLCHAIN rw9044
// FLAGS: /O2 /Os /Ob1 /G5
/* MATCH-RWL04 0x00551460 (h2): 37.9% (regs 43.1). Hand form with intrinsic strlen x2 + for loop; flags /O2 /Os won (others 27-36). Residue: original strlen is repne scasb (ecx) which DDK 9176 emits as a byte loop (same as S23 png_check_keyword finding). class (a). Chars written as 47/92 decimals. */
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
typedef unsigned int uint;
typedef unsigned char undefined1;
typedef int (__cdecl *FN_t)(int,char*);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
uint __cdecl strlen(const char *);
#pragma intrinsic(strlen)
// MATCH: 0x00551460 FUN_00551460
int FUN_00551460(int param_1,int param_2,char *param_3,char *param_4,undefined1 param_5)
{
  int iVar3;
  int len3, len4;
  char cVar2;

  len3 = strlen(param_3);
  len4 = strlen(param_4);
  if (len4 + len3 <= param_2 - 1) {
    (*(FN_t *)(DAT_007d3ff8 + 0xcc))(param_1, param_3);
    for (iVar3 = 0; iVar3 <= len4; iVar3++) {
      cVar2 = param_4[iVar3];
      if (cVar2 == 47 || cVar2 == 92)
        *(char *)(param_1 + len3 + iVar3) = param_5;
      else
        *(char *)(param_1 + len3 + iVar3) = cVar2;
    }
    return param_1;
  }
  return 0;
}
