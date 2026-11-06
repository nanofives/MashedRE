// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 (h1) 0x00484280: 100% (regs 100%). Register-arg function (item in ECX): written `static` with its
   real caller 0x004842d0 below it (0x3a bytes at 0x004842d0..0x00484309, no Ghidra function there, so no
   MATCH line: match.py has no size for it; transcribed from the disassembly, not scored). The other caller
   is at 0x00484474 (also outside any Ghidra function). No residual. */
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
typedef void (*ItemFn)(void *item);
MGLOBAL(int, DAT_006cf880, 0x006cf880);
MGLOBALA(void *, DAT_006cf078, 0x202, 0x006cf078);
MGLOBAL(int, DAT_006cfc98, 0x006cfc98);
MGLOBAL(int, DAT_006cfc9c, 0x006cfc9c);
MGLOBAL(ItemFn, DAT_006cf070, 0x006cf070);
MGLOBAL(int, DAT_006cfc88, 0x006cfc88);
MGLOBAL(int, DAT_006cf06c, 0x006cf06c);

// MATCH: 0x00484280 FUN_00484280
static void FUN_00484280(void *item)
{
  DAT_006cf078[DAT_006cf880] = item;
  DAT_006cf880++;
  if (DAT_006cfc98 == 0 && DAT_006cfc9c == 0) {
    DAT_006cf070(item);
  }
}

/* 0x004842d0 (Ghidra has no function there): the real caller, here so cl picks the ECX convention */
void *FUN_004842d0(void *item)
{
  unsigned short n = *(unsigned short *)((char *)item + 0x84);
  DAT_006cfc88 += n;
  DAT_006cf06c++;
  if (n > 0) {
    FUN_00484280(item);
  }
  return item;
}
