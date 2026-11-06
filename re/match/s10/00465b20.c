// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00465b20 (h8): 100% by hand (raw did not compile: Ghidra thiscall). Same shape as 0x00465940:
   cdecl (V3 *pos, int k), the channel is the int at DAT_0088e670 + k * 0x90 + 0x6b0, 2 iterations. */
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
typedef struct { float x, y, z; } MatchV3_465b20;
typedef struct { char *p; float f; } MatchRec0068f640;
MGLOBAL(char *, DAT_0088e670, 0x0088e670);
MGLOBAL(float, DAT_005cd0b8, 0x005cd0b8);
MGLOBAL(float, DAT_005ce6b8, 0x005ce6b8);
MGLOBALA(MatchRec0068f640, DAT_0068f640, 1, 0x0068f640);
void FUN_0045e0f0(int p0, float p1);
int FUN_00492d10(void);
void FUN_005a66d0(char *p, int a);
void FUN_005a6d60(int h, int a, int b, MatchV3_465b20 *v);
void FUN_005a6dc0(int h, int a, int b, float f);
// MATCH: 0x00465b20 FUN_00465b20
void FUN_00465b20(MatchV3_465b20 *pos, int k)
{
  int idx;
  int n;
  float f;

  if (FUN_00492d10() == 3) {
    idx = *(int *)(DAT_0088e670 + k * 0x90 + 0x6b0);
    n = 2;
    do {
      if ((*(unsigned char *)(DAT_0068f640[idx].p + 0x34) & 2) == 0) {
        *(MatchV3_465b20 *)(DAT_0068f640[idx].p + 0x58) = *pos;
        if (*(int *)(DAT_0068f640[idx].p + 0x30) != 0) {
          FUN_005a6d60(*(int *)(DAT_0068f640[idx].p + 0x30), 0, 1, pos);
        }
        FUN_005a66d0(DAT_0068f640[idx].p, 1);
        f = (float)n * DAT_005cd0b8 + DAT_005ce6b8;
        *(float *)(DAT_0068f640[idx].p + 0x3c) = f;
        if (*(int *)(DAT_0068f640[idx].p + 0x30) != 0) {
          FUN_005a6dc0(*(int *)(DAT_0068f640[idx].p + 0x30), 4, 1, f);
        }
        FUN_0045e0f0(idx, 1.0f);
        n = 0;
      }
      idx++;
      n--;
    } while (0 < n);
  }
}
