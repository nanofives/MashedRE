// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046d570 (helper h4): 100% (regs 100%). Dot product needs (double) on the record operand and z,x,y term order (float terms in any of 96 orders/parenthesisations stuck at 91.2%: fld local / fmul global swapped). Literals -1.0f/1.0f/57.2958 (.rdata 0x005cc33c/0x005cc320/0x005ccae0); acos = CRT stack entry FUN_004a3384. */
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
typedef struct { float x, y, z; } V3_0046d570;
typedef struct { float m[16]; } Mat_0046d570;
typedef struct {
  undefined1 pad0[0x9b0];
  V3_0046d570 v9b0; /* +0x9b0 */
  undefined1 pad9bc[0x18];
  V3_0046d570 pos; /* +0x9d4 */
  undefined1 pad9e0[0x324];
} Rec_0046d570;
MGLOBALA(Rec_0046d570, DAT_008815a0, 16, 0x008815a0);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
float FUN_004c39b0(V3_0046d570 *out, V3_0046d570 *in);
double FUN_004a3384(double x);  /* CRT acos, stack-argument entry */
// MATCH: 0x0046d570 FUN_0046d570
int FUN_0046d570(float *param_1,unsigned int param_2)
{
  V3_0046d570 n;
  float d;
  *param_1 = 0.0f;
  if (param_2 >= 0x10) return 0;
  if (DAT_008815a0[param_2].v9b0.x == DAT_005d757c &&
      DAT_008815a0[param_2].v9b0.y == DAT_005d757c &&
      DAT_008815a0[param_2].v9b0.z == DAT_005d757c) return 0;
  FUN_004c39b0(&n, &DAT_008815a0[param_2].v9b0);
  d = (double)DAT_008815a0[param_2].pos.z * n.z + (double)DAT_008815a0[param_2].pos.x * n.x + (double)DAT_008815a0[param_2].pos.y * n.y;
  if (d < -1.0f) d = -1.0f;
  else if (d > 1.0f) d = 1.0f;
  *param_1 = (float)(FUN_004a3384(d) * 57.295799255371094);
  return 1;
}
