// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00555360 (h2): 94.4% (regs 94.4%), 89/90 instructions. Font-system init: two FUN_004cc9e0
   free lists (0x144 and 0xc byte entries), an 0x51-byte RwMalloc path buffer grown with RwRealloc
   (RwGlobals+0x110) to strlen("fonts\\")+1 and filled with rwstrcpy (+0xcc), the 64-quad index table at
   0x00912700 ({v, v+2, v+3, v, v+3, v+1}), then the globals at 0x00912a18..0x00913260. Raw converter did not
   compile (string escape). Storing index 2 before index 1 took the loop from 88.9% to 94.4% (permuter).
   Residuals: (1) the original ends each of the three failed checks with its own `jne next; ret` (eax already
   0), ours merges them into one `xor eax,eax; ret`. Tried: returning the tested global, a result variable,
   else-chains, pointer return type, game flags (/O2 /GS /Ob1, /O1, /O1 /Oy- all lower). (2) one 16-bit store
   of the loop ([eax-2], si) is placed one instruction later. Store-order sweep over 60 orders and permuter
   (260 compiles) found nothing better. Class (b): return-block form not found. */
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
typedef void *(*rwmalloc_t)(unsigned int size, unsigned int hint);
typedef void *(*rwrealloc_t)(void *p, unsigned int size, unsigned int hint);
typedef unsigned int (*rwstrlen_t)(const char *s);
typedef char *(*rwstrcpy_t)(char *d, const char *s);
MGLOBAL(int, DAT_00623e94, 0x00623e94);
MGLOBAL(int, DAT_00623e98, 0x00623e98);
MGLOBAL(int, DAT_00623e9c, 0x00623e9c);
MGLOBAL(int, DAT_00623ea0, 0x00623ea0);
MGLOBALA(char, s_fonts__00623ea4, 7, 0x00623ea4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(char, DAT_007dc7a0, 0x007dc7a0);
MGLOBAL(char, DAT_007dc7c4, 0x007dc7c4);
MGLOBALA(short, DAT_00912700, 0x180, 0x00912700);
MGLOBAL(void *, DAT_00912a08, 0x00912a08);
MGLOBAL(char *, DAT_00912a0c, 0x00912a0c);
MGLOBAL(unsigned int, DAT_00912a10, 0x00912a10);
MGLOBAL(void *, DAT_00912a14, 0x00912a14);
MGLOBAL(int, DAT_00912a18, 0x00912a18);
MGLOBAL(int, DAT_00912a1c, 0x00912a1c);
MGLOBAL(int, DAT_00912a20, 0x00912a20);
MGLOBAL(void *, DAT_00913100, 0x00913100);
MGLOBAL(int, DAT_0091325c, 0x0091325c);
MGLOBAL(int, DAT_00913260, 0x00913260);
void *FUN_004cc9e0(int size, int p1, int p2, int p3, void *p4, int hint);
int FUN_004c5890(void);
int FUN_00555910();
// MATCH: 0x00555360 FUN_00555360
int FUN_00555360(void)
{
  unsigned int len;
  char *p;
  int i;

  DAT_00912a08 = FUN_004cc9e0(0x144, DAT_00623e94, 4, DAT_00623e98, &DAT_007dc7a0, 0x40199);
  if (DAT_00912a08 == 0) {
    return 0;
  }
  DAT_00912a14 = FUN_004cc9e0(0xc, DAT_00623e9c, 4, DAT_00623ea0, &DAT_007dc7c4, 0x40190);
  if (DAT_00912a14 == 0) {
    return 0;
  }
  DAT_00912a0c = (char *)(*(rwmalloc_t *)(DAT_007d3ff8 + 0x108))(0x51, 0x1040190);
  if (DAT_00912a0c == 0) {
    return 0;
  }
  DAT_00912a10 = 0x51;
  len = (*(rwstrlen_t *)(DAT_007d3ff8 + 0xf4))(s_fonts__00623ea4) + 1;
  if (len > DAT_00912a10) {
    p = (char *)(*(rwrealloc_t *)(DAT_007d3ff8 + 0x110))(DAT_00912a0c, len, 0x1040190);
    if (p == 0) {
      goto skip;
    }
    DAT_00912a0c = p;
    DAT_00912a10 = len;
  }
  (*(rwstrcpy_t *)(DAT_007d3ff8 + 0xcc))(DAT_00912a0c, s_fonts__00623ea4);
skip:
  {
    short v = 0;
    for (i = 0; i < 0x40; i++) {
      DAT_00912700[i * 6 + 0] = v;
      DAT_00912700[i * 6 + 2] = v + 3;
      DAT_00912700[i * 6 + 1] = v + 2;
      DAT_00912700[i * 6 + 3] = v;
      DAT_00912700[i * 6 + 4] = v + 3;
      DAT_00912700[i * 6 + 5] = v + 1;
      v += 4;
    }
  }
  DAT_00912a18 = 0;
  DAT_00912a1c = FUN_004c5890();
  DAT_00912a20 = DAT_00912a1c;
  DAT_0091325c = 0;
  DAT_00913260 = 0;
  DAT_00913100 = (void *)FUN_00555910;
  return 1;
}
