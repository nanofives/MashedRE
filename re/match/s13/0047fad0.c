// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047fad0 (h8): 100% (regs 100%, ops 100%), 105/105 ins.
   Hand pass from the raw converter (38.3%): real callee arities from the call sites (0x004e4380 takes 2 args,
   the first is FUN_00426060()'s result, 0x00426060 takes none; 0x0055dec0 takes 1 and 0x00559c40 takes 4),
   0x0055b940 returns float (fstp st(0)), zero RwV3d local struct-copied into the object, 16-dword struct copy
   (rep movsd), `param_3 * 0.1f` literal (0x005cc56c = 0.1f; the global form loaded the constant first). */
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
typedef struct { float x, y, z; } RwV3d;
MGLOBAL(int, DAT_006c6eb0, 0x006c6eb0);
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBALA(int, DAT_006c6b90, 1, 0x006c6b90);
MGLOBALA(int, DAT_006c71d8, 1, 0x006c71d8);
MGLOBALA(int, DAT_006c9758, 1, 0x006c9758);
MGLOBALA(int, DAT_006cb070, 1, 0x006cb070);
typedef struct { int a[16]; } Mat16;
int FUN_00426060(void);
void FUN_004e4380(int p0, int p1);
int FUN_004e69a0(int p0);
int FUN_004e7e30(int p0, int p1);
int FUN_004c0b30(void);
int FUN_00559c40(int p0, int p1, int p2, int p3);
float FUN_0055b940(int *p0, int p1, int p2);
void FUN_0055bab0(int p0, float p1);
void FUN_0055c4a0(int p0, int p1);
void FUN_0055c4f0(int p0, int p1);
int FUN_0055c810(int p0, int p1);
int FUN_0055dec0(int p0);
int FUN_0057c220(int p0, int p1);
int FUN_0057c300(int *p0);
int *FUN_0057c500(void);
void FUN_0057c550(int *p0);
// MATCH: 0x0047fad0 FUN_0047fad0
int FUN_0047fad0(int param_1,int param_2,float param_3,int param_4,int param_5,
                int param_6,int param_7)
{
  RwV3d v;
  int *p;
  int h;
  int r;
  int n;

  v.x = 0; v.y = 0; v.z = 0;
  p = FUN_0057c500();
  h = *p;
  *(RwV3d *)(p + 1) = v;
  FUN_0055c810(h,param_2);
  FUN_0055c4f0(h,param_4);
  FUN_0055c4a0(h,param_5);
  FUN_0055bab0(h,param_3 * 0.1f);
  FUN_0055b940(p,param_6,0);
  r = FUN_0057c300(p);
  n = DAT_006c6eb0;
  DAT_006c6b90[n] = param_7;
  DAT_006c9758[n] = 0;
  *(Mat16 *)(DAT_006cb070 + n * 0x10) = *(Mat16 *)(*(int *)(param_1 + 4) + 0x10);
  DAT_006c71d8[DAT_006c6eb0] = FUN_004e69a0(param_1);
  FUN_004e7e30(DAT_006c71d8[DAT_006c6eb0],FUN_004c0b30());
  FUN_0057c220(DAT_006c71d8[DAT_006c6eb0],r);
  FUN_004e4380(FUN_00426060(),DAT_006c71d8[DAT_006c6eb0]);
  *(unsigned int *)(r + 8) &= 0xfffffff3;
  FUN_00559c40(FUN_0055dec0(DAT_006ce274),r,-1,2);
  n = DAT_006c6eb0++;
  FUN_0057c550(p);
  return n;
}
