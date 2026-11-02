// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00420230 by hand (h2): 100% (regs 100%). Raw g2s 95.0%. The car id arrives on the stack and goes to
   FUN_0041f330 in ESI (Ghidra lost the argument): the file holds FUN_0041f330 as a `static` callee defined before
   its caller (pilot finding 1; same body as re/match/s04/0041f330.c, whose stand-in caller this file replaces with
   the real one). Strings through MGLOBALA: "toastart/vehicles/" 0x005cd288, ".piz" 0x005cd280. RwGlobals-like
   record at DAT_007d3ff8 (+0xcc strcpy, +0xd4 strcat). No residual. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { char pad[0xcc]; void (*strcpy_)(char *, const char *); int pad2; void (*strcat_)(char *, const char *); } RwGlobalsLike;
MGLOBAL(int, DAT_005f5fe0, 0x005f5fe0);
MGLOBALA(NamedEntry, s_Shuriken_005f37a8, 78, 0x005f37a8);
MGLOBAL(RwGlobalsLike *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, DAT_0063d7f0, 0x40, 0x0063d7f0);
MGLOBALA(char, s_toastart_vehicles__005cd288, 19, 0x005cd288);
MGLOBALA(char, DAT_005cd280, 5, 0x005cd280);
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
// MATCH: 0x00420230 FUN_00420230
char *FUN_00420230(int id)
{
  char *name = FUN_0041f330(id);
  DAT_007d3ff8->strcpy_(DAT_0063d7f0, s_toastart_vehicles__005cd288);
  DAT_007d3ff8->strcat_(DAT_0063d7f0, name);
  DAT_007d3ff8->strcat_(DAT_0063d7f0, DAT_005cd280);
  return DAT_0063d7f0;
}
