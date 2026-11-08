// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00551ad0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef char *(*PFN_alloc2)(undefined4,int);
typedef unsigned int size_t;
MGLOBAL(undefined4, DAT_00623e24, 0x00623e24);
MGLOBAL(undefined4, DAT_007dc770, 0x007dc770);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 * FUN_0052da20(undefined4 p0, int p1, undefined4 p2, undefined4 p3);
undefined4 FUN_005522e0(int *p0, int p1, undefined4 p2, undefined4 p3, void *p4);
char * _strncpy(char * p0, char * p1, size_t p2);
// MATCH: 0x00551ad0 FUN_00551ad0
int * FUN_00551ad0(char *param_1)
{
  int iVar1;
  int iVar2;
  int *piVar3;
  char *_Dest;
  float m[16];
  piVar3 = (int *)FUN_0052da20(DAT_00623e24,2,0,0x3f800000);
  if (piVar3 != (int *)0x0) {
    _Dest = (char *)(**(PFN_alloc2 *)(DAT_007d3ff8 + 0x118))(DAT_007dc770,0x30135);
    if (_Dest != (char *)0x0) {
      _strncpy(_Dest,param_1,0x20);
      _Dest[0x1f] = '\0';
      *(undefined4 *)(_Dest + 0x20) = 0;
      *(undefined4 *)(_Dest + 0x40) = 1;
      piVar3[5] = (int)_Dest;
      iVar1 = piVar3[4];
      iVar2 = *(int *)(*piVar3 + 8);
      *(uint *)&m[3] = *(uint *)&m[3] | 0x20003;
      m[10] = 1.0f;
      m[5] = 1.0f;
      m[0] = 1.0f;
      m[4] = 0.0f;
      m[2] = 0.0f;
      m[1] = 0.0f;
      m[9] = 0.0f;
      m[8] = 0.0f;
      m[6] = 0.0f;
      m[14] = 0.0f;
      m[13] = 0.0f;
      m[12] = 0.0f;
      FUN_005522e0(piVar3,iVar1,0,0,m);
      FUN_005522e0(piVar3,iVar2 + iVar1,iVar1,0x3f800000,m);
      return piVar3;
    }
  }
  return (int *)0x0;
}
