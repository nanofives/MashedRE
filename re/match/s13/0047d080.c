// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047d080 (h4): 100% (regs 100%). By hand: if (param_2 != 0) {..} else {..} order, int array MGLOBALA, int-typed callees. Relocheck OK. */
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
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBALA(int, DAT_006c71d8, 200, 0x006c71d8);
int FUN_00559c40(int p0, int p1, int p2, int p3);
int FUN_00559ee0(int p0, int p1);
int FUN_0055dec0(int p0);
int FUN_0057c210(int p0);
// MATCH: 0x0047d080 FUN_0047d080
void FUN_0047d080(int param_1, int param_2)
{
  int obj;
  int w;

  if (param_1 >= 0 && param_1 < 200 && DAT_006c71d8[param_1] != 0) {
    obj = FUN_0057c210(DAT_006c71d8[param_1]);
    w = FUN_0055dec0(DAT_006ce274);
    if (param_2 != 0) {
      if (*(int *)(obj + 0x24) == 0) {
        *(unsigned int *)(obj + 8) &= 0xfffffff3;
        FUN_00559c40(w, obj, -1, 7);
      }
    } else {
      if (*(int *)(obj + 0x24) != 0) {
        FUN_00559ee0(w, obj);
      }
    }
  }
}
