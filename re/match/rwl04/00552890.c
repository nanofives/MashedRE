// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 91.2%, was 82.4%. match_permute.py moved the v.y store after v.x. Remaining: y/z zero-store order and the dot-product load schedule (48 order/spelling combinations tried). Class (b). */
/* MATCH-RWL04 0x00552890 (h2): 82.4% (regs 82.4, ops 94.1). Float globals, 12-byte local vector (y,z zeroed first), float sum. Residue: x87 sum schedule (original loads v1, add esp, v0, v2 interleaved; ours batches); all 6 term orders give identical bytes. class (a). */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef long double float10;
MGLOBAL(int, DAT_00912bec, 0x00912bec);
MGLOBAL(float, DAT_00912b08, 0x00912b08);
MGLOBAL(float, DAT_00912b10, 0x00912b10);
MGLOBAL(float, DAT_00912be4, 0x00912be4);
MGLOBAL(float, DAT_00912be0, 0x00912be0);
MGLOBAL(float, DAT_00912be8, 0x00912be8);
MGLOBAL(undefined1, DAT_00912b58, 0x00912b58);
undefined4 FUN_004c3dc0(float *p0, float *p1, undefined1 *p2);
void FUN_00552e40(void);
// MATCH: 0x00552890 FUN_00552890
float10 FUN_00552890(void)
{
  struct V {float x,y,z;} v;
  if (DAT_00912bec == 0) {
    FUN_00552e40();
    v.z = 0.0f;
    v.x = (DAT_00912b08 / DAT_00912b10) * DAT_00912be4 * DAT_00912be0;
    v.y = 0.0f;
    FUN_004c3dc0((float*)&v,(float*)&v,&DAT_00912b58);
    DAT_00912bec = 1;
    DAT_00912be8 = v.x * v.x + v.y * v.y + v.z * v.z;
    return DAT_00912be8;
  }
  return DAT_00912be8;
}
