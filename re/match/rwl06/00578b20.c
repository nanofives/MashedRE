// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR05 2026-11-08: 90.6% regs 90.6% (was 81.2). The global DAT_005cc33c (-1.0f) written as the literal -1.0f: the compiler then loads the vector element first (fld [esi]; fmul [const]) as the original does; with the global it loads the constant first. Residual: the param_1 copy (ours mov eax,[esp+0xc] before push esi then mov edi,eax; original mov edi,[esp+0x14] after push edi). Real locals (minA..maxB) get ops 100% but a 0xc frame (69.8%); 60 declaration orders did not give the 8-byte frame. Class b. Supersedes the header below. */
/* MATCH-RWL06 h1 0x00578b20: 81.2% (regs 81.2%), class b. edi/eax load order for param_1 copy. Tried: out vars alias param slots, local_8 reuse, d>t compare, permuter. cxx:ok. */
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
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
void FUN_0055c2d0(int param_1, int param_2, float *param_3, float *param_4, float *param_5);
// MATCH: 0x00578b20 FUN_00578b20
float10 FUN_00578b20(int *param_1,float *param_2,float *param_3)
{
  int *piVar1;
  float *pfVar2;
  float local_8;
  float local_4;
  float d;

  pfVar2 = param_2;
  piVar1 = param_1;
  FUN_0055c2d0(*(int *)(*piVar1 + 8),*(int *)(*piVar1 + 0x10),param_2,&local_8,&local_4);
  FUN_0055c2d0(*(int *)(piVar1[1] + 8),*(int *)(piVar1[1] + 0x10),pfVar2,(float *)&param_2,(float *)&param_1);
  d = local_8 - *(float *)&param_1;
  local_8 = *(float *)&param_2 - local_4;
  if (d > local_8) {
    *param_3 = *(float *)&param_1;
    return d;
  }
  *pfVar2 = *pfVar2 * -1.0f;
  pfVar2[1] = pfVar2[1] * -1.0f;
  pfVar2[2] = pfVar2[2] * -1.0f;
  *param_3 = -*(float *)&param_2;
  return local_8;
}
