// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004657b0 (h8): 100% by hand (raw 35.3%). What moved it: the record char at +0 read into a local
   before the state test (68.3% -> 99.0%), and the final 0.65f / 1.0f store written as if/else (the store-then-
   overwrite form Ghidra shows schedules pop ebx after the first store, 99.0%). The 0x005a8750 query is a local
   {int, type, float val, flag}. */
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
typedef struct { int a; int type; float val; int flag; } MatchQuery_4657b0;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc340, 0x005cc340);
MGLOBAL(float, DAT_005cc318, 0x005cc318);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005cc99c, 0x005cc99c);
MGLOBAL(float, DAT_006036c4, 0x006036c4);
MGLOBAL(float, DAT_006036cc, 0x006036cc);
MGLOBALA(float, DAT_006130e0, 1, 0x006130e0);
MGLOBALA(int, DAT_0069045c, 1, 0x0069045c);
MGLOBALA(int, DAT_006904c4, 6, 0x006904c4);
MGLOBALA(char *, DAT_0088f680, 6, 0x0088f680);
MGLOBALA(int, DAT_008aa2e0, 6, 0x008aa2e0);
void FUN_0045c850(int a);
void FUN_0045da90(int a, int b, float c, float d);
int FUN_00462760(void);
void FUN_005a8750(int h, MatchQuery_4657b0 *q);
int FUN_005a89a0(int h);
// MATCH: 0x004657b0 FUN_004657b0
void FUN_004657b0(void)
{
  char *rec;
  int ch;
  int slot;
  int hit;
  int i;
  float vol;
  MatchQuery_4657b0 q;

  if (FUN_00462760() == 0) {
    hit = 0;
    for (i = 0; i < 6; i++) {
      rec = DAT_0088f680[i];
      if (rec != 0) {
        slot = *rec;
        ch = *(int *)(rec + 4);
        if (*(int *)(rec + 0xc) == 1) {
          switch (*(int *)(rec + 0x24)) {
          case 1:
            vol = DAT_005cc340;
            break;
          case 2:
            vol = DAT_005cc318;
            break;
          case 3:
            vol = DAT_005cc32c;
            break;
          case 4:
            vol = DAT_005cc99c;
            break;
          default:
            vol = DAT_005d757c;
            break;
          }
          vol = vol * DAT_006036c4;
          DAT_006130e0[slot] = 1.0f;
          if (vol < DAT_005d757c) {
            vol = 0.0f;
          }
          if (FUN_005a89a0(DAT_0069045c[ch]) == 6) {
            FUN_0045da90(ch, 0, vol, vol);
            hit = 1;
          }
          q.flag = 1;
          q.type = 3;
          FUN_005a8750(DAT_0069045c[ch], &q);
          if (FUN_005a89a0(DAT_0069045c[ch]) == 3 && DAT_006904c4[i] == 6) {
            *(int *)(DAT_0088f680[i] + 0xc) = 0;
            DAT_008aa2e0[i] = 0;
            FUN_0045c850(*(int *)(DAT_0088f680[i] + 0x24));
          }
          DAT_006904c4[i] = FUN_005a89a0(DAT_0069045c[ch]);
          *(float *)(DAT_0088f680[i] + 0x1c) = *(float *)(DAT_0088f680[i] + 0x18) - q.val;
        }
      }
    }
    if (hit) {
      DAT_006036cc = 0.65f;
    }
    else {
      DAT_006036cc = 1.0f;
    }
  }
}
