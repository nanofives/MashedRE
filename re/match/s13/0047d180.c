// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047d180 (h4): 100% (regs 100%). By hand: the 32-byte local is float[8] (a char array adds the /GS cookie), int-typed callees, the bit test inline. Relocheck OK. */
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
MGLOBALA(int, DAT_006c71d8, 200, 0x006c71d8);
void FUN_004c0e50(int p0);
void FUN_004c45f0(int p0);
int FUN_004c52f0(int p0, int p1, int p2);
int FUN_0055ab30(int p0, int p1, int p2);
void FUN_0055ac00(int p0, int p1, int p2);
int FUN_0055ade0(int p0, int p1);
void FUN_0055dff0(int p0, int p1);
int FUN_0057c210(int p0);
int FUN_0057c420(int p0, void *p1);
// MATCH: 0x0047d180 FUN_0047d180
void FUN_0047d180(int param_1, int param_2)
{
  int obj;
  int w;
  int clump;
  float buf[8];

  obj = FUN_0057c210(DAT_006c71d8[param_1]);
  w = *(int *)(obj + 0x24);
  if (w != 0 && (((unsigned int *)*(int *)(w + 0x60))[*(unsigned short *)(obj + 0x20) >> 5] & (1 << (*(unsigned short *)(obj + 0x20) & 0x1f)))) {
    FUN_0055ade0(w, obj);
  }
  clump = *(int *)(*(int *)(obj + 0x10) + 8);
  FUN_004c52f0(clump, param_2, 0);
  FUN_004c45f0(clump);
  FUN_004c0e50(*(int *)(DAT_006c71d8[param_1] + 4));
  FUN_0055dff0(obj, 0);
  if (*(int *)(obj + 0x24) != 0) {
    FUN_0055ab30(*(int *)(obj + 0x24), obj, FUN_0057c420(obj, buf));
    FUN_0055ac00(*(int *)(obj + 0x24), obj, 0);
  }
}
