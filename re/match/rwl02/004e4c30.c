// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 71.3% (regs 88.9%, ops 94.4%). Locals grouped in one Frame struct {version, err{code,data}, chunk} so the slots do not overlap (frame 0x28 vs original 0x24, one extra dword); err.code reused as the float temp like the original. Residual: frame size, stream pointer load order, ternary at the end. Class (b).
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

typedef struct { undefined4 code; undefined4 data; } Err;
typedef struct { undefined4 v[4]; float angle; uchar b0; uchar b1; uchar type; uchar b3; } Chunk;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined4, DAT_0061862c, 0x0061862c);
float FUN_004c3b30(float p0);
uint FUN_004cbd30(int p0, Chunk *p1, uint p2);
int FUN_004cc5e0(int p0, int p1, int *p2, uint *p3);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 *FUN_004d8480(Err *p0);
int FUN_004e1b60(undefined4 *p0, int p1, int p2);
int FUN_004e4dd0(uchar p0);
// MATCH: 0x004e4c30 RpLightStreamRead
typedef struct { uint version; Err err; Chunk ck; } Frame;
int RpLightStreamRead(int param_1)
{
  int iVar1;
  int uVar2;
  int iVar3;
  undefined4 *puVar4;
  Frame L;

  iVar3 = param_1;
  iVar1 = FUN_004cc5e0(param_1, 1, &param_1, &L.version);
  if (iVar1 != 0) {
    if ((L.version < 0x35000) || (0x37002 < L.version)) {
      L.err.code = 2;
      L.err.data = FUN_004d7ff0(0x80000004);
      FUN_004d8480(&L.err);
    }
    else {
      puVar4 = (undefined4 *)&L.ck;
      for (iVar1 = 6; iVar1 != 0; iVar1 = iVar1 + -1) {
        *puVar4 = 0;
        puVar4 = puVar4 + 1;
      }
      iVar1 = FUN_004cbd30(iVar3, &L.ck, param_1);
      if (iVar1 == param_1) {
        uVar2 = FUN_004e4dd0(L.ck.type);
        if (uVar2 != 0) {
          *(undefined4 *)(uVar2 + 0x14) = L.ck.v[0];
          *(undefined4 *)(uVar2 + 0x18) = L.ck.v[1];
          *(undefined4 *)(uVar2 + 0x1c) = L.ck.v[2];
          *(undefined4 *)(uVar2 + 0x20) = L.ck.v[3];
          if (L.version < 0x30300) {
            *(float *)&L.err.code = DAT_005cc320 / (L.ck.angle * L.ck.angle + DAT_005cc320);
            *(float *)(uVar2 + 0x28) = -FUN_004c3b30(*(float *)&L.err.code);
          }
          else {
            *(float *)(uVar2 + 0x28) = L.ck.angle;
          }
          if ((*(float *)(uVar2 + 0x18) == *(float *)(uVar2 + 0x1c)) &&
             (*(float *)(uVar2 + 0x18) == *(float *)(uVar2 + 0x20))) {
            *(uchar *)(uVar2 + 3) = 1;
          }
          else {
            *(uchar *)(uVar2 + 3) = 0;
          }
          *(uchar *)(uVar2 + 2) = L.ck.b0;
          iVar3 = FUN_004e1b60(&DAT_0061862c, iVar3, uVar2);
          return (iVar3 != 0) ? uVar2 : 0;
        }
      }
    }
  }
  return 0;
}
