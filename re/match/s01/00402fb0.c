// FLAGS: /O2 /GS /Ob1
/* a2 0x00402fb0 (148 B). Best: 100%.
   g2s 36.8%: RGBA structs by value (same shape as 0x00403d30), sin intrinsic, (float)sin(..) * -127.0f
   gives fmul dword (without the cast: fmul qword). _ftol = 0x004a2c48. */
typedef struct { unsigned char r, g, b, a; } RwRGBA;
double sin(double);
#pragma intrinsic(sin)
#ifndef MGLOBAL   /* RWC-FRAME-492E90: port-includable globals (extern T n under C, as before) */
#ifdef __cplusplus
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#else
#define MGLOBAL(T, n, a) extern T n
#endif
#endif
MGLOBAL(float, DAT_00636afc, 0x00636afc);
void FUN_00427e00(int p0, float p1, float p2, RwRGBA p3, float p4, int p5);

// MATCH: 0x00402fb0 FUN_00402fb0
void FUN_00402fb0(void)
{
  RwRGBA black;
  RwRGBA white;
  unsigned char a;
  white.r = 255; white.g = 255; white.b = 255;
  black.r = 0; black.g = 0; black.b = 0;
  a = (unsigned char)(128 - (char)((float)sin(DAT_00636afc) * -127.0f));
  DAT_00636afc = DAT_00636afc + 0.1f;
  black.a = a;
  white.a = a;
  FUN_00427e00(0x2a4, 320.0f, 380.0f, black, 1.2f, 2);
  FUN_00427e00(0x2a4, 316.0f, 376.0f, white, 1.2f, 2);
}
