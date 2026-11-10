// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-SHIFT 2026-11-09: 91.9% / 265 bad B -> 96.5% / 67 bad B, 173 -> 170 insns, symbol 649 -> 638 B
   (first divergence stays at +0xb2). `v.y = (float)(v.y * (double)DAT_005cc9dc)` (pilot lever: double
   intermediate, the global float operand gives fmul dword, no qword constant) stops 9176 keeping the scale in an
   x87 register; v.z as `(float)((0, v.z) * (0, (double)DAT_005cc9dc))`. Left: v.x scale (ours fld k; fmul st(2),
   original fxch st(1); fmul [k]; 8 spellings tried), v.z still loads k first, and the e0 stack temp (5 forms, a
   named temp is worse). Permuter after these: 671 compiles, no gain. The text below is the earlier state. */
/* MATCH-S13 0x0047d3c0 (h4): best 91.9% (regs 91.9%, ops 96.5%). By hand from decomp (raw 40%): RGBA byte struct, V3 struct copy *dst = pos (gives the interleaved store/reload of the stack vector), scale by 0x005cc9dc as a separate step, unsigned loop index (jb + unsigned fild fixup), 1-arg FUN_0055dec0 / 4-arg FUN_00559c40, float-arg callees. The permuter reached 94.2% by computing v.z before v.y, but that loads the globals in a different order from the original (relocheck BAD), so the natural order is kept. Residual: the 0x005cc9dc scale keeps the constant in an x87 register (fld k; fmul st(i)) where the original uses fmul mem three times and reloads v.z, and the e0 value goes through a stack temp at esp+0x14 for the last 0x00883dec copy in the original. Tried: single-expression components, separate temp vector, forceinline scale function, k*v order, float temp t for e0 (worse), do-while loop. Class (b) source shape. */
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
typedef struct { float x, y, z; } V3_47D3C0;
typedef struct { unsigned char r, g, b, a; } RGBA_47D3C0;
MGLOBAL(float, DAT_00881630, 0x00881630);
MGLOBAL(float, DAT_00881634, 0x00881634);
MGLOBAL(float, DAT_00881638, 0x00881638);
MGLOBAL(float, DAT_0088163c, 0x0088163c);
MGLOBAL(float, DAT_00881650, 0x00881650);
MGLOBAL(float, DAT_00881664, 0x00881664);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005cc9dc, 0x005cc9dc);
MGLOBAL(float, DAT_005cc94c, 0x005cc94c);
MGLOBAL(float, DAT_005cd074, 0x005cd074);
MGLOBAL(float, DAT_008816d8, 0x008816d8);
MGLOBAL(float, DAT_008816dc, 0x008816dc);
MGLOBAL(float, DAT_008816e0, 0x008816e0);
MGLOBAL(float, DAT_008823dc, 0x008823dc);
MGLOBAL(float, DAT_008823e0, 0x008823e0);
MGLOBAL(float, DAT_008823e4, 0x008823e4);
MGLOBAL(float, DAT_008830e0, 0x008830e0);
MGLOBAL(float, DAT_008830e4, 0x008830e4);
MGLOBAL(float, DAT_008830e8, 0x008830e8);
MGLOBAL(float, DAT_00883de4, 0x00883de4);
MGLOBAL(float, DAT_00883de8, 0x00883de8);
MGLOBAL(float, DAT_00883dec, 0x00883dec);
MGLOBALA(int, DAT_006c9a78, 200, 0x006c9a78);
int FUN_00426c00(void);
void FUN_0047d240(int p0, int p1, float p2, float p3, float p4);
int FUN_004826d0(int p0, int p1);
int FUN_00482730(int p0, RGBA_47D3C0 *p1);
int FUN_004c0b30(void);
int FUN_004e7e30(int p0, int p1);
int FUN_00559c40(int p0, int p1, int p2, int p3);
int FUN_0055ae70(int p0, int p1, int p2);
float FUN_0055b940(int *p0, float p1, int p2);
void FUN_0055bab0(int p0, float p1);
void FUN_0055c4a0(int p0, float p1);
void FUN_0055c4f0(int p0, float p1);
int FUN_0055c810(int p0, V3_47D3C0 *p1);
int FUN_0055dec0(int p0);
int FUN_0057c220(int p0, int p1);
int FUN_0057c300(int *p0);
int *FUN_0057c500(void);
int FUN_0057c550(int *p0);
// MATCH: 0x0047d3c0 FUN_0047d3c0
void FUN_0047d3c0(int param_1)
{
  RGBA_47D3C0 col;
  V3_47D3C0 v;
  V3_47D3C0 pos;
  int *cam;
  V3_47D3C0 *dst;
  int w;
  int obj;
  unsigned int i;

  col.r = 0x40;
  col.g = 0x40;
  col.b = 0x60;
  col.a = 0xff;
  v.x = 0.2f;
  v.y = 0.065f;
  v.z = 0.4f;
  cam = FUN_0057c500();
  *(float *)&cam[0xd] = 1.0f;
  *(float *)&cam[0xe] = 2.5f;
  dst = (V3_47D3C0 *)(cam + 1);
  pos.x = 0.0f;
  pos.y = -0.1f;
  pos.z = 0.0f;
  *dst = pos;
  v.x = (DAT_00881630 - DAT_0088163c) * DAT_005cc32c;
  v.y = (DAT_00881664 - DAT_00881634) * DAT_005cc32c;
  v.z = (DAT_00881638 - DAT_00881650) * DAT_005cc32c;
  v.x *= DAT_005cc9dc;
  v.y = (float)(v.y * (double)DAT_005cc9dc);
  v.z = (float)((0, v.z) * (0, (double)DAT_005cc9dc));
  DAT_008816d8 = (-DAT_0088163c - DAT_00881630) * DAT_005cc32c;
  DAT_008816dc = -(DAT_00881634 + v.y);
  DAT_008816e0 = (-DAT_00881650 - DAT_00881638) * DAT_005cc32c;
  DAT_008823dc = DAT_008816d8;
  DAT_008823e0 = DAT_008816dc;
  DAT_008823e4 = DAT_008816e0;
  DAT_008830e0 = DAT_008816d8;
  DAT_008830e4 = DAT_008816dc;
  DAT_008830e8 = DAT_008816e0;
  DAT_00883de4 = DAT_008816d8;
  DAT_00883de8 = DAT_008816dc;
  DAT_00883dec = DAT_008816e0;
  w = *cam;
  FUN_0055c810(w, &v);
  FUN_0055c4f0(w, 0.4f);
  FUN_0055c4a0(w, 0.5f);
  FUN_0055bab0(w, 0.01f);
  FUN_0055b940(cam, 1.5f, 1);
  for (i = 0; i < 4; i++) {
    obj = FUN_0057c300(cam);
    DAT_006c9a78[i] = FUN_004826d0(w, 1);
    FUN_00482730(DAT_006c9a78[i], &col);
    FUN_004e7e30(DAT_006c9a78[i], FUN_004c0b30());
    FUN_0057c220(DAT_006c9a78[i], obj);
    FUN_0047d240(i, 0, 0.5f, -10.0f, (float)i * DAT_005cd074);
    *(unsigned int *)(obj + 8) &= 0xfffffff3;
    FUN_00559c40(FUN_0055dec0(param_1), obj, -1, 2);
    if (FUN_00426c00() == 0x26) {
      FUN_0055ae70(*(int *)(obj + 0x24), obj, i + 2);
    } else {
      FUN_0055ae70(*(int *)(obj + 0x24), obj, 1);
    }
  }
  FUN_0057c550(cam);
}
