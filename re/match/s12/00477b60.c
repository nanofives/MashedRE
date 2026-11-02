// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00477b60 (h4, by hand): 100%. Slot pool of 0x2c0-byte records at 0x00693198 + used-bit array 0x006bf198. Moved: a ret variable initialised to 0 (return 0 comes from the zero register), ret = i + 1 before the bit set, then break */
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
typedef struct {
  unsigned char pad[0x2b0];
  int a2b0;
  int a2b4;
  int a2b8;
  float a2bc;
} Slot2c0;
MGLOBALA(Slot2c0, DAT_00693198, 256, 0x00693198);
MGLOBALA(unsigned char, DAT_006bf198, 32, 0x006bf198);

// MATCH: 0x00477b60 FUN_00477b60
int FUN_00477b60(void)
{
  int i;
  int ret = 0;
  Slot2c0 *s;

  for (i = 0; i < 0x100; i++) {
    if ((DAT_006bf198[i >> 3] & (1 << (i & 7))) == 0) {
      s = &DAT_00693198[i];
      s->a2b0 = 0;
      s->a2b8 = 0;
      s->a2b4 = 1;
      s->a2bc = 30.0f;
      ret = i + 1;
      DAT_006bf198[i >> 3] |= (unsigned char)(1 << (i & 7));
      break;
    }
  }
  return ret;
}


