// FLAGS: /O2 /GS /Ob1
/* TL-MATCH-REST 0x00409710 (119 B): opens led.piz, reads stream chunk 0x13269901 into 0x0063a5f0. */
/* ---- MATCH-TLR preamble (same text in every re/match/tlr source; not part of any function body) ----
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
MGLOBALA(unsigned int, DAT_0063a5f0, 0x480, 0x0063a5f0);
MGLOBAL(int, DAT_0063b7f0, 0x0063b7f0);
void FUN_004095a0(int a);
void FUN_00495280(const char *name);
int FUN_004cc230(int a, int b, void *c);
int FUN_004cc5e0(int s, unsigned int id, unsigned int *len, int *ver);
int FUN_004cbd30(int s, void *dst, unsigned int len);
int FUN_004cc160(int s, int a);
void FUN_004952f0(void);
// MATCH: 0x00409710 FUN_00409710
int FUN_00409710(void)
{
  int s;
  int ret;
  unsigned int len;
  int ver;

  ret = 0;
  FUN_004095a0(1);
  FUN_00495280("d:\\ToastArt\\common\\led.piz");
  s = FUN_004cc230(2, 1, &DAT_0063b7f0);
  if (s != 0) {
    if (FUN_004cc5e0(s, 0x13269901, &len, &ver) != 0) {
      FUN_004cbd30(s, DAT_0063a5f0, len);
      FUN_004cc160(s, 0);
      ret = 1;
    }
  }
  FUN_004952f0();
  return ret;
}
