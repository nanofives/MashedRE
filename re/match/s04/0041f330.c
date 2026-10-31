// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041f330 by hand (h9): 100% (regs 100%). Raw g2s 56.5%. The id arrives in ESI (Ghidra unaff_ESI): FUN_0041f330 is a file-static function, and the file holds a stand-in caller (Caller_00420230, shaped like the real caller 0x00420230, NOT matched) so cl picks the register convention (pilot finding 1). Table: 0x84-byte records at 0x005f37a8 (name[0x80], id at +0x80 = 0x005f3828), count DAT_005f5fe0; array length 78 = (0x005f5fe0-0x005f37a8)/0x84, a declaration bound only. Bytes of 0x0041f330 identical; the stand-in caller is not the real TU. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct { char name[0x80]; int id; } NamedEntry;
MGLOBAL(int, DAT_005f5fe0, 0x005f5fe0);
MGLOBALA(NamedEntry, s_Shuriken_005f37a8, 78, 0x005f37a8);
typedef struct { char pad[0xcc]; void (*strcpy_)(char *, const char *); int pad2; void (*strcat_)(char *, const char *); } RwGlobalsLike;
MGLOBAL(RwGlobalsLike *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, DAT_0063d7f0, 0x40, 0x0063d7f0);
// MATCH: 0x0041f330 FUN_0041f330
static char *FUN_0041f330(int id)
{
  int i;
  for (i = 0; i < DAT_005f5fe0; i++) {
    if (s_Shuriken_005f37a8[i].id == id)
      return s_Shuriken_005f37a8[i].name;
  }
  return 0;
}
/* stand-in caller shaped like 0x00420230 (not matched here) */
char *Caller_00420230(int id)
{
  char *name = FUN_0041f330(id);
  DAT_007d3ff8->strcpy_(DAT_0063d7f0, "a");
  DAT_007d3ff8->strcat_(DAT_0063d7f0, name);
  DAT_007d3ff8->strcat_(DAT_0063d7f0, "b");
  return DAT_0063d7f0;
}
