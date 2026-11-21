// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c7d30: PORT-TOP-AUDIO-5BA1D0-STOPS, see below. */
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
/* PORT-TOP-AUDIO-5BA1D0-STOPS (2026-11-21): new source from the listing of MASHED.exe.unpatched 0x005c7d30..0x005c7edc (no
   Ghidra source existed; no caller, 0x005c7a90 stores its address at +0x28). Field offsets are from s = param_1 + 0x28
   (0x005c7d3d); param_3 is a 4-dword record (0x005c7d55 .. 0x005c7e04). The two code addresses it hands on (0x005aaaa0 to
   0x005aaac0 at 0x005c7da6, 0x005c7ee0 into the record for 0x005bb000 at 0x005c7ebb) are named as functions, so the port
   passes their exe bodies. */
MGLOBAL(undefined1, DAT_005e6e50, 0x005e6e50);
void FUN_004522d0(int p0);
int FUN_005aa060(int p0, void *p1, int p2);
int FUN_005aaaa0(int p0, int p1);
void FUN_005aaac0(int (*p0)(int, int), int *p1);
int FUN_005aea50(int p0, uint p1, int p2);
int FUN_005aeea0(void *p0, int p1, int p2);
int FUN_005b9e30(int p0, int p1, int p2, int p3, int p4);
void FUN_005bb000(int p0, int *p1);
void FUN_005c7ee0(void);
void FUN_005c7f80(int p0);
#define S_005c7d30(o, T) (*(T *)(s + (o)))
// MATCH: 0x005c7d30 FUN_005c7d30
int FUN_005c7d30(int param_1, int param_2, int *param_3, int param_4, int param_5)

{
  char *s;
  int r;
  uint n;
  int mem;
  int rec [3];

  s = (char *)param_1 + 0x28;
  S_005c7d30(0x24, int) = 0;
  S_005c7d30(8, uint) = 0x3f800000;
  S_005c7d30(0xc, int) = 0;
  S_005c7d30(0, int) = param_3[1];
  r = param_3[0];
  S_005c7d30(0x14, int) = r;
  S_005c7d30(0x18, int) = param_3[2];
  S_005c7d30(0x20, char *) = s + 0x1c;
  S_005c7d30(0x10, int) = 1;
  S_005c7d30(0x1c, char *) = s + 0x1c;
  if (r == 0) {                                        /* 0x005c7d74: the value loaded at 0x005c7d5a, not a reload */
    r = FUN_005aa060(0,&DAT_005e6e50,0);                /* 0x005c7d87 */
    if (r == 0) {
      return 0;
    }
    rec[0] = 0;
    rec[1] = r;
    FUN_005aaac0(FUN_005aaaa0,rec);                     /* 0x005c7daf: callback 0x005aaaa0, record [esp+8] */
    S_005c7d30(0x14, int) = rec[0];
    if (rec[0] == 0) {
      r = FUN_005b9e30(0,0,0,param_4,param_5);          /* 0x005c7dd2 */
      S_005c7d30(0x14, int) = r;
      S_005c7d30(0x24, uint) = S_005c7d30(0x24, uint) | 1;
      if (r == 0) {
        return 0;
      }
    }
    else {
      S_005c7d30(0x24, uint) = S_005c7d30(0x24, uint) & 0xfffffffe;
    }
    S_005c7d30(0x18, int) = *(int *)(*(int *)(S_005c7d30(0x14, int) + 0x20) + 4);   /* 0x005c7df8 .. 0x005c7e01 */
  }
  n = ((uint)(param_3[3] * S_005c7d30(0x18, int) * 4) / 1000 + 3) & 0xfffffffc;      /* 0x005c7e04 .. 0x005c7e24 */
  S_005c7d30(0x34, uint) = (n >> 1) << 2;
  S_005c7d30(0x28, uint) = n >> 2;
  mem = FUN_005aea50(1,(S_005c7d30(0x34, uint) + n) * 2,0x3080c);                    /* 0x005c7e3e */
  S_005c7d30(0x30, int) = mem;
  if (mem == 0) {
    FUN_005c7f80(param_1);
    return 0;
  }
  S_005c7d30(0x2c, int) = mem + n;
  S_005c7d30(0x38, int) = mem + n + n;
  S_005c7d30(0x3c, int) = mem + n + n + S_005c7d30(0x34, uint);
  S_005c7d30(0x44, char *) = s + 0x2c;
  S_005c7d30(0x40, int) = 2;
  if (FUN_005aeea0(s + 4,1,1) == 0) {                   /* 0x005c7e88 */
    FUN_004522d0(S_005c7d30(0x30, int));
    FUN_005c7f80(param_1);
    return 0;
  }
  rec[0] = (int)FUN_005c7ee0;                           /* 0x005c7ebb */
  rec[1] = (int)n;
  rec[2] = (int)s;
  FUN_005bb000(S_005c7d30(0x14, int),rec);              /* 0x005c7ecb */
  return param_1;
}
