// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b68f0 (h4): 100%, regs 100% (g2s 96.7%). By hand: index loop over the 0x80-byte name table at DAT_0090dac0 with e = &DAT_0090dac0[i] inside the body (a walking pointer gave 96.7%, the param load scheduled after the table base). FUN_004b302f is the stricmp. Table length 0x200 is a placeholder (only the address is used). */
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
typedef struct { char name[0x78]; int value; int pad; } E_0090dac0;
MGLOBAL(unsigned int, DAT_008ad9a8, 0x008ad9a8);
MGLOBALA(E_0090dac0, DAT_0090dac0, 0x200, 0x0090dac0);
int FUN_004b302f(const char *p0, const char *p1);

// MATCH: 0x004b68f0 FUN_004b68f0
int FUN_004b68f0(char *param_1)
{
  unsigned int i;
  E_0090dac0 *e;

  for (i = 0; i < DAT_008ad9a8; i++) {
    e = &DAT_0090dac0[i];
    if (FUN_004b302f(e->name, param_1) == 0) {
      return e->value;
    }
  }
  return 0;
}
