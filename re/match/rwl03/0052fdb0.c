// FLAGS: /O2 /Ob1 /G5
// h3: 96.2% regs 96.2%. Bubble sort of 4 floats with byte companions; float temp for the swap gives fld/fstp. Residual: esi push/xor order at entry, class (a).
/* MATCH-RWL03 0x0052fdb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef struct { int a0; int a4; int a8; int pad[2]; int cnt; } H0052fdb0_t;
MGLOBAL(int, DAT_007dc508, 0x007dc508);
void FUN_0052fd90(void);
void FUN_005c24e0(int p0, int p1, int p2, void *p3);
// MATCH: 0x0052fdb0 FUN_0052fdb0
int FUN_0052fdb0(int param_1)
{
  int iVar5;
  int *piVar9;
  unsigned char *puVar10;
  int iVar11;
  int sw;
  float ft;
  unsigned char b;

  iVar5 = *(int *)(DAT_007dc508 + param_1);
  iVar11 = 0;
  if (iVar5 != 0) {
    piVar9 = *(int **)(iVar5 + 0x18);
    puVar10 = (unsigned char *)(*(int *)(iVar5 + 0x14) + 2);
    if (0 < *(int *)(param_1 + 0x14)) {
      do {
        if (*piVar9 < 0x3f800000) {
          while (1) {
            sw = 0;
            if (piVar9[0] < piVar9[1]) {
              b = puVar10[-2];
              ft = ((float *)piVar9)[0];
              puVar10[-2] = puVar10[-1];
              puVar10[-1] = b;
              piVar9[0] = piVar9[1];
              ((float *)piVar9)[1] = ft;
              sw = 1;
            }
            if (piVar9[1] < piVar9[2]) {
              b = puVar10[-1];
              ft = ((float *)piVar9)[1];
              puVar10[-1] = puVar10[0];
              puVar10[0] = b;
              piVar9[1] = piVar9[2];
              ((float *)piVar9)[2] = ft;
              sw = 1;
            }
            if (piVar9[2] < piVar9[3]) {
              b = puVar10[0];
              ft = ((float *)piVar9)[2];
              puVar10[0] = puVar10[1];
              puVar10[1] = b;
              piVar9[2] = piVar9[3];
              ((float *)piVar9)[3] = ft;
              continue;
            }
            if (!sw) break;
          }
        }
        piVar9 = piVar9 + 4;
        iVar11 = iVar11 + 1;
        puVar10 = puVar10 + 4;
      } while (iVar11 < *(int *)(param_1 + 0x14));
    }
  }
  if (*(int *)(iVar5 + 8) != 0) {
    FUN_005c24e0(*(int *)(iVar5 + 8),*(int *)(iVar5 + 4),1,(void *)FUN_0052fd90);
  }
  return param_1;
}
