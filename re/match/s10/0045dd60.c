// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045dd60 (h3): 78.7% (regs 78.7%, ops 93.3%). Raw converter did not compile. Everything
   matches except one x87 operand order: the original loads the per-voice gain (&DAT_0068f644)[i*2] FIRST, then
   v*scale, then fmulp; 9176 always emits fld v; fmul scale; fmul gain. The one-instruction length change shifts
   every later jump target, which is most of the lost %. Tried: all association/commutation orders, separate
   result local, (float)/(double) casts, *1.0f, __inline mul helper, __inline clamp helper, {int,float} struct
   array, product before the if, permuter 240 s (no gain). Residual class (b): source shape of the gain product
   not found. Tail past Ghidra's size (second FUN_005baf00 call) also compared by --bytes. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0088e674, 0x0088e674);
MGLOBAL(float, DAT_006036c0, 0x006036c0);
MGLOBAL(int, DAT_00605db8, 0x00605db8);
MGLOBAL(int, DAT_00604eb0, 0x00604eb0);
MGLOBAL(int, DAT_00604e1c, 0x00604e1c);
MGLOBAL(int, DAT_00605bfc, 0x00605bfc);
MGLOBAL(int, DAT_00605d24, 0x00605d24);
MGLOBAL(int, DAT_0069049c, 0x0069049c);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
MGLOBAL(float, DAT_0068f644, 0x0068f644);
float FUN_00431b20(void);
int FUN_00432290(void);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
void FUN_005baf00(int p0, float p1);
// MATCH: 0x0045dd60 FUN_0045dd60
void FUN_0045dd60(void)
{
  int i;
  float scale;
  float v;

  for (i = 0; i < DAT_0088e674; i++) {
    scale = DAT_006036c0;
    v = FUN_00431b20();
    if (i != DAT_00605db8 && i != DAT_00604eb0 && i != DAT_00604e1c && i != DAT_00605bfc &&
        i != DAT_00605d24) {
      v = (&DAT_0068f644)[i * 2] * (v * scale);
      if (v > 1.0f) {
        v = 1.0f;
      }
      else if (v < 0.0f) {
        v = 0.0f;
      }
      *(float *)((&DAT_0068f640)[i * 2] + 0x40) = v;
      if (*(int *)((&DAT_0068f640)[i * 2] + 0x30) != 0) {
        FUN_005a6dc0(*(int *)((&DAT_0068f640)[i * 2] + 0x30),5,1,v);
      }
    }
  }
  if (FUN_00432290() != 0) {
    FUN_005baf00(DAT_0069049c,0.0f);
    return;
  }
  FUN_005baf00(DAT_0069049c,1.0f);
}
