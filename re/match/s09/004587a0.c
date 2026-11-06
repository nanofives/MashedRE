// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004587a0: 100% by hand (raw 61.0%). The 25-entry loop is a count-down index loop
   (for (i = 25; i-- != 0;)) over a 0x50-byte record array at 0x0068b198, pointer form gives a counter register.
   Callee arities from their ret/stack use. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct Icon004587a0 {
  undefined4 clump;
  undefined4 f4;
  float f8;
  float fc;
  float f10;
  float f14;
  undefined4 rest[14];
} Icon004587a0;
MGLOBALA(Icon004587a0, DAT_0068b198, 25, 0x0068b198);
MGLOBALA(undefined1, DAT_0068b968, 8, 0x0068b968);
MGLOBAL(undefined4, DAT_0068b9ac, 0x0068b9ac);
undefined4 FUN_0040bb30(const char *p0);
void FUN_004770c0(void *p0, uint p1, uint p2, undefined4 p3);
undefined4 FUN_004b3b70(const char *p0, void *p1, undefined4 p2);
undefined4 FUN_004b3d80(const char *p0, undefined4 p1, undefined4 p2);
undefined4 FUN_004b3e40(void *p0, undefined4 p1, undefined4 p2, undefined4 p3);
void FUN_004b6520(void *p0, undefined4 p1);
void FUN_004c5c80(undefined4 p0);
undefined4 FUN_004b65a0(void);
undefined4 FUN_004b65b0(const char *p0);
// MATCH: 0x004587a0 FUN_004587a0
void FUN_004587a0(void)
{
  int i;
  undefined1 buf[2048];

  FUN_004770c0(DAT_0068b968, 0x817, 0x19, FUN_0040bb30("puglow"));
  DAT_0068b9ac = FUN_004b3d80("PowerUpIcons.txd", 0, 0);
  FUN_004c5c80(0);
  FUN_004b6520(buf, 0x800);
  FUN_004b65a0();
  FUN_004b3b70("IconCube.dff", buf, FUN_004b65b0("IconCube.dff"));
  for (i = 25; i-- != 0; ) {
    DAT_0068b198[i].clump = FUN_004b3e40(buf, 0x800, 0, 0);
    DAT_0068b198[i].f8 = 1.0f;
    DAT_0068b198[i].fc = 0.0f;
    DAT_0068b198[i].f10 = 1.0f;
    DAT_0068b198[i].f14 = 0.0f;
  }
}
