// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c7330: 29.3% (regs 43.1%). Typed rewrite with bit-punned float sqrt-table step and MGLOBALA short table (raw was 29.3% too). Orig round-trips floats through fixed stack dwords (esp+0x10/0x18) and converts with inline fnstcw/or ah,0xc/fldcw/fistp where this compiler emits call __ftol; the fp temporaries/inline ROUND shape not reproduced (no inline __asm tried within budget). Class: b. */
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
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cd04c, 0x005cd04c);
MGLOBALA(short, DAT_00633b48, 256, 0x00633b48);
void FUN_005c6b60(int *p0, int p1, int p2);
// MATCH: 0x005c7330 FUN_005c7330
void FUN_005c7330(int param_1)
{
  float fVar1;
  float fVar3;
  float fVar4;
  unsigned int u;
  int e;
  int ci;

  fVar1 = *(float *)(*(int *)(param_1 + 0x9c) + 8) * *(float *)(param_1 + 0x44) * *(float *)(param_1 + 0x3c);
  fVar4 = *(float *)(param_1 + 0x48) * DAT_005cc32c + DAT_005cc32c;
  fVar3 = DAT_005cc320 - fVar4;
  if (fVar3 != DAT_005d757c) {
    u = *(unsigned int *)&fVar3 & 0x7fffff;
    e = (int)(*(unsigned int *)&fVar3 >> 0x17) - 0x7f;
    if ((e & 1) != 0) {
      u = u | 0x800000;
    }
    u = ((unsigned int)(int)DAT_00633b48[(u >> 16) & 0xffff] << 16) | (unsigned int)(((short)((short)e >> 1) + 0x7f) << 0x17);
    fVar3 = *(float *)&u;
  }
  if (fVar4 != DAT_005d757c) {
    u = *(unsigned int *)&fVar4 & 0x7fffff;
    e = (int)(*(unsigned int *)&fVar4 >> 0x17) - 0x7f;
    if ((e & 1) != 0) {
      u = u | 0x800000;
    }
    u = ((unsigned int)(int)DAT_00633b48[(u >> 16) & 0xffff] << 16) | (unsigned int)(((short)((short)e >> 1) + 0x7f) << 0x17);
    fVar4 = *(float *)&u;
  }
  *(int *)(param_1 + 0x6c) = (int)(fVar3 * fVar1 * DAT_005cd04c);
  fVar3 = *(float *)(param_1 + 0x40) * DAT_005cd04c;
  *(int *)(param_1 + 0x70) = (int)(fVar4 * fVar1 * DAT_005cd04c);
  *(int *)(param_1 + 0x74) = (int)fVar3;
  FUN_005c6b60((int *)(param_1 + 0xa4), *(int *)(param_1 + 0x6c), *(int *)(param_1 + 0x70));
  FUN_005c6b60((int *)(param_1 + 0xb8), *(int *)(param_1 + 0x74), *(int *)(param_1 + 0x74));
}
