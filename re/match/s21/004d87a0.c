// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d87a0 (h1, by hand): 100.0% exact, regs 100.0%. String stream write: NULL -> "" (0x005d8d70), size =
   (strlen via RwGlobals+0xf4 + 4) & ~3, chunk header (type 2, 0x37002, 10) via 0x004cc580, body via 0x004cbe80.
   The duplicated NULL test is in the original (kept as a second variable). Raw form was 95.6% on the
   `-(r != 0) & p` return; two early `return 0` give the single neg/sbb/and. relocheck OK, C++ ok. */
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
typedef int (*FUN_004d87a0_strlen)(const char *s);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, DAT_005d8d70, 1, 0x005d8d70);
int FUN_004cbe80(int stream, const char *buf, uint size);
int FUN_004cc580(int stream, int type, uint size, uint version, uint build);
// MATCH: 0x004d87a0 FUN_004d87a0
const char *FUN_004d87a0(const char *param_1,int param_2)

{
  const char *s;
  uint size;

  if (param_1 == 0) {
    param_1 = DAT_005d8d70;
  }
  s = param_1;
  if (param_1 == 0) {
    s = DAT_005d8d70;
  }
  size = ((*(FUN_004d87a0_strlen *)(DAT_007d3ff8 + 0xf4))(s) + 4) & ~3u;
  if (FUN_004cc580(param_2,2,size,0x37002,10) == 0) {
    return 0;
  }
  if (FUN_004cbe80(param_2,param_1,size) == 0) {
    return 0;
  }
  return param_1;
}
