// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 h5 0x0047ea60 (204 B): 100% (regs 100%), relocheck OK unchecked=0, C++ ok.
   Raw g2s 33.8%. What moved it: 16-dword copy as a struct assignment (rep movsd), callee arities from the
   push counts (4c3df0 4 args, 55dff0 2), and DAT_005cc33c is the .rdata literal -1.0f (written as x * -1.0f;
   a global float operand loads first and scores 90.8%). */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct { undefined4 m[16]; } Mat16;
MGLOBALA(int, DAT_006c9a78, 8, 0x006c9a78);
MGLOBAL(float, DAT_008816d8, 0x008816d8);
MGLOBAL(float, DAT_008816dc, 0x008816dc);
MGLOBAL(float, DAT_008816e0, 0x008816e0);
void FUN_004c0e50(int p0);
int FUN_004c3df0(float *p0, float *p1, int p2, void *p3);
void FUN_004c51a0(void *p0, float *p1, int p2);
void FUN_0055ac00(int p0, int p1, int p2);
void FUN_0055dff0(int p0, int p1);
int FUN_0057c210(int p0);
// MATCH: 0x0047ea60 FUN_0047ea60
void FUN_0047ea60(int param_1, Mat16 *param_2)
{
  int iVar2;
  Mat16 *puVar1;
  float local_c[3];

  if (DAT_006c9a78[param_1] != 0 &&
      (iVar2 = FUN_0057c210(DAT_006c9a78[param_1]), iVar2 != 0)) {
    puVar1 = *(Mat16 **)(*(int *)(iVar2 + 0x10) + 8);
    *puVar1 = *param_2;
    local_c[0] = (&DAT_008816d8)[param_1 * 0x341] * -1.0f;
    local_c[1] = (&DAT_008816dc)[param_1 * 0x341] * -1.0f;
    local_c[2] = (&DAT_008816e0)[param_1 * 0x341] * -1.0f;
    FUN_004c3df0(local_c, local_c, 1, param_2);
    FUN_004c51a0(puVar1, local_c, 1);
    FUN_004c0e50(*(int *)(DAT_006c9a78[param_1] + 4));
    FUN_0055dff0(iVar2, 0);
    FUN_0055ac00(*(int *)(iVar2 + 0x24), iVar2, 1);
  }
}
