// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005baa60: 83.7% (regs 85.3%, ops 99.2%). Hand: 7-arg __stdcall vtable +0x10 call (VT macro), tmp[4]+b[14] locals, flag tests in source order (&1, &2, &4 as else-if chain), p3 copied in ascending index order in the &1 path, float scale via *(float*)&. Residual (a): x87 operand order (original fld [esi+0x28]; fmul const, ours fld const first; swapping the product did not change it) plus path-3 store scheduling. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
typedef int (__stdcall *vf7)(void *, void *, int, int, int, void *, int);
#define VT(o, off) (*(int *)(*(int *)(o) + (off)))
MGLOBAL(float, DAT_005cc328, 0x005cc328);
MGLOBAL(undefined1, DAT_005e7080, 0x005e7080);
MGLOBAL(undefined1, DAT_005e70c0, 0x005e70c0);
MGLOBAL(undefined1, DAT_005e70e0, 0x005e70e0);
void FUN_005bc190(uint *p0, uint *p1);
// MATCH: 0x005baa60 FUN_005baa60
undefined4 FUN_005baa60(int param_1, byte param_2, uint *param_3)
{
  int iVar1;
  uint tmp[4];
  uint b[14];

  if (param_2 & 1) {
    b[0] = param_3[0];
    b[1] = param_3[1];
    b[2] = param_3[2];
    b[3] = param_3[3];
    b[4] = param_3[4];
    b[5] = param_3[5];
    b[6] = param_3[6];
    b[7] = param_3[7];
    b[8] = param_3[8];
    b[9] = param_3[9];
    b[0xa] = param_3[10];
    b[0xb] = param_3[0xb];
    iVar1 = ((vf7)VT(*(int *)(param_1 + 8), 0x10))((void *)*(int *)(param_1 + 8), &DAT_005e70e0, 0, 0, 0, b, 0x30);
    if (iVar1 < 0) {
      return 0;
    }
  } else if (param_2 & 2) {
    FUN_005bc190(param_3, tmp);
    iVar1 = ((vf7)VT(*(int *)(param_1 + 8), 0x10))((void *)*(int *)(param_1 + 8), &DAT_005e7080, 0, 0, 0, tmp, 0x10);
    if (iVar1 < 0) {
      return 0;
    }
  } else if (param_2 & 4) {
    FUN_005bc190(param_3, tmp);
    b[0] = param_3[0];
    b[1] = param_3[1];
    *(float *)&b[0xa] = *(float *)&param_3[10] * DAT_005cc328;
    b[3] = param_3[3];
    b[4] = param_3[4];
    b[6] = param_3[6];
    b[7] = param_3[7];
    b[9] = tmp[0];
    b[2] = param_3[2];
    *(float *)&b[0xb] = *(float *)&param_3[9] * DAT_005cc328;
    b[5] = param_3[5];
    b[8] = param_3[8];
    b[0xd] = 0;
    b[0xc] = 0xc0a00000;
    iVar1 = ((vf7)VT(*(int *)(param_1 + 8), 0x10))((void *)*(int *)(param_1 + 8), &DAT_005e70c0, 1, 0, 0, b, 0x38);
    if (iVar1 < 0) {
      return 0;
    }
  }
  return 1;
}
