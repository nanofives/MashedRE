// FLAGS: /O2 /Ob1 /G5
/* 0x005bac00: 100.0%, BYTE-IDENTICAL (PORT-TOP-AUDIO-5AEF30-KNOB, 2026-11-16), source from the listing. The 0x30-byte block at [esp+4]
   holds the twelve dwords stored at 0x005bac11..0x005bac65; the dwords stored as immediates twice (0x42c80000 at 0x005bac55 /
   0x005bac5d) are float fields (an int pair is held in a register, as 0xffffd8f0 is at 0x005bac08). The call result dword reuses
   the argument slot (lea edx,[esp+0x38] at 0x005bac0d). Previous source (59.0%): D:/MashedWT/PORT-TOP-AUDIO-5AEF30-KNOB_data/005bac00_before.c. */
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
MGLOBAL(undefined1, DAT_005e70e0, 0x005e70e0);
MGLOBAL(undefined1, DAT_005e70c0, 0x005e70c0);
MGLOBAL(undefined1, DAT_005e7080, 0x005e7080);
int FUN_005baa60(int *p0, int p1, uint *p2);
typedef int (__stdcall *PFN4_005bac00)(int *, void *, int, uint *);
// MATCH: 0x005bac00 FUN_005bac00
int FUN_005bac00(int *param_1)
{
  struct { uint d0, d4; float f8, fc, f10; uint d14; float f18; uint d1c; float f20, f24, f28, f2c; } l;
  uint r;
  int *o;

  l.d0 = 0xffffd8f0;
  l.d4 = 0xffffd8f0;
  o = (int *)param_1[2];
  l.f8 = 0.0f;
  l.fc = 1.49f;
  l.f10 = 0.83f;
  l.d14 = 0xfffff5d6;
  l.f18 = 0.007f;
  l.d1c = 0xc8;
  l.f20 = 0.011f;
  l.f24 = 100.0f;
  l.f28 = 100.0f;
  l.f2c = 5000.0f;
  if ((*(PFN4_005bac00 *)(*o + 0x14))(o, &DAT_005e70e0, 0, &r) >= 0 && r == 3 &&
      FUN_005baa60(param_1, 1, (uint *)&l) != 0) {
    return 1;
  }
  o = (int *)param_1[2];
  if ((*(PFN4_005bac00 *)(*o + 0x14))(o, &DAT_005e70c0, 1, &r) >= 0 && r == 3 &&
      FUN_005baa60(param_1, 4, (uint *)&l) != 0) {
    return 4;
  }
  o = (int *)param_1[2];
  if ((*(PFN4_005bac00 *)(*o + 0x14))(o, &DAT_005e7080, 0, &r) >= 0 && r == 3 &&
      FUN_005baa60(param_1, 2, (uint *)&l) != 0) {
    return 2;
  }
  return 0;
}
