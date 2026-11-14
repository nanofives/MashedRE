// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
// MATCH-RWT-S7: 100% byte-identical under rw9044 (L-131), relocheck OK. Rewritten from the listing: atomic fields read off the
// atomic pointer (cur - 0x40, lea esi,[eax-0x40] at 0x004e6e28), the light and camera unlink loops in one order (stores +8, +4, +0 at
// 0x004e6ed9..0x004e6edf and 0x004e6f27..0x004e6f2d), the exit on a NULL object after the destroy call (cmp esi,ebp / je at
// 0x004e6ee9 and 0x004e6f37), and the object pointer computed before next is loaded (match_permute swap).
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(int, DAT_007d7230, 0x007d7230);
MGLOBAL(int, DAT_007d722c, 0x007d722c);
MGLOBAL(undefined4, DAT_00618664, 0x00618664);
MGLOBAL(undefined4, DAT_0061867c, 0x0061867c);
void FUN_004c0790(void *p0);
undefined4 FUN_004c0de0(int p0);
void FUN_004c0e50(int p0);
undefined4 FUN_004c1cf0(undefined4 p0);
int FUN_004d8060(void *p0, void *p1);
undefined4 FUN_004d8bd0(int * p0);
undefined4 FUN_004e4440(void *p0);
undefined4 FUN_004e4d90(undefined4 p0);
undefined4 FUN_004e8ea0(int p0);
// MATCH: 0x004e6e00 RpClumpDestroy
undefined4 RpClumpDestroy(int clump)
{
  int *cur;
  int *next;
  int atomic;
  int frame;
  int obj;
  int *lnk;

  FUN_004d8060(&DAT_0061867c, (void *)clump);
  cur = *(int **)(clump + 8);
  while (cur != (int *)(clump + 8)) {
    next = (int *)*cur;
    atomic = (int)cur - 0x40;
    FUN_004d8060(&DAT_00618664, (void *)atomic);
    if (*(int *)(atomic + 0x14) != 0) {
      FUN_004d8bd0(*(int **)(atomic + 0x14));
    }
    if (*(int *)(atomic + 0x18) != 0) {
      FUN_004e8ea0(*(int *)(atomic + 0x18));
      frame = *(int *)(atomic + 4);
      *(int *)(atomic + 0x18) = 0;
      if (frame != 0 && FUN_004e4440((void *)atomic) != 0) {
        FUN_004c0e50(frame);
      }
    }
    FUN_004c0790((void *)atomic);
    (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7200 + DAT_007d3ff8), atomic);
    cur = next;
  }
  cur = *(int **)(clump + 0x10);
  while (cur != (int *)(clump + 0x10)) {
    obj = (int)cur - DAT_007d7230 - 4;
    next = (int *)*cur;
    lnk = (int *)(DAT_007d7230 + obj);
    *(int *)lnk[2] = lnk[1];
    *(int *)(lnk[1] + 4) = lnk[2];
    lnk[2] = 0;
    lnk[1] = 0;
    lnk[0] = 0;
    FUN_004e4d90(obj);
    if (obj == 0) break;
    cur = next;
  }
  cur = *(int **)(clump + 0x18);
  while (cur != (int *)(clump + 0x18)) {
    obj = (int)cur - DAT_007d722c - 4;
    next = (int *)*cur;
    lnk = (int *)(DAT_007d722c + obj);
    *(int *)lnk[2] = lnk[1];
    *(int *)(lnk[1] + 4) = lnk[2];
    lnk[2] = 0;
    lnk[1] = 0;
    lnk[0] = 0;
    FUN_004c1cf0(obj);
    if (obj == 0) break;
    cur = next;
  }
  if (*(int *)(clump + 4) != 0) {
    FUN_004c0de0(*(int *)(clump + 4));
  }
  (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7200 + 4 + DAT_007d3ff8), clump);
  return 1;
}
