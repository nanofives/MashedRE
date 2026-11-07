// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aeca0 (h3): 78.4% exact, regs 80.4%. Residual class (b) big-endian writer: switch on size, byte-wise loads; remaining diff is the case-4 instruction order (and the shifted/or tail). */
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
typedef unsigned short undefined2;
typedef unsigned int uint;
typedef unsigned char byte;
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
// MATCH: 0x005aeca0 FUN_005aeca0
void FUN_005aeca0(unsigned char **param_1,unsigned char *param_2,int param_3)
{
  switch (param_3) {
  case 1: {
    unsigned char *d = *param_1;
    *d = *param_2;
    *param_1 = d + 1;
    break; }
  case 2: {
    unsigned short *d = (unsigned short *)*param_1;
    *d = (unsigned short)((param_2[0] << 8) | param_2[1]);
    *param_1 = (unsigned char *)(d + 1);
    break; }
  case 4: {
    uint *d = (uint *)*param_1;
    uint v = *(uint *)param_2; unsigned char *pb = (unsigned char *)&v;
    *d = ((v << 16 | v & 0xff00 | v >> 16 & 0xff) << 8) | param_2[3];
    *param_1 = (unsigned char *)(d + 1);
    break; }
  }
}
