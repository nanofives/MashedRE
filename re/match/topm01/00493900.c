// FLAGS: /O2 /GS /Ob1
/* MATCH-TOPM01 0x00493900: 100.0% (122/122 ins), --bytes BYTE-IDENTICAL (448 B, relocations masked),
   relocheck OK (checked=36, unchecked=2: the /GS cookie pair). Flags unchanged: /O2 /GS /Ob1.
   Command-line switch parser: inline strcpy of FUN_00499730()'s string into char[256], then a FUN_004a3ac9
   (strtok-shaped) loop with a 2-byte " " separator local and a FUN_004b302f (_stricmp-shaped) if/else chain.
   What closed it (from the s15 66.9% source): the loop is a do { tok = FUN_004a3ac9(s, seps); s = 0;
   if (tok != 0) { chain } } while (tok != 0). A do-while is not rotated, so the single call site stays at the
   loop head (0x00493942), the string pointer s stays in ebp (buf, then 0 via xor ebp,ebp at 0x00493952), and
   every chain branch's back-edge threads straight to 0x00493942. The s15 for(;;)/while forms were rotated by
   9176 (peeled first call with buf, second call site at the bottom with a constant 0). Behaviour unchanged.
   Dead ends this round: /G5 removes the strcpy-loop alignment nops of the s15 form (68.8%) but schedules the
   cookie xor after push ebx (98.4% with the do-while); /O1 uses an ebp frame (52.2%). The empty-then form
   (if (tok == 0) {} else if ...) also scores 100%. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBALA(char, s__vs0_005cf8cc, 5, 0x005cf8cc);
MGLOBALA(char, s__vs1_005cf8c4, 5, 0x005cf8c4);
MGLOBALA(char, s__cs0_005cf8bc, 5, 0x005cf8bc);
MGLOBALA(char, s__cs1_005cf8b4, 5, 0x005cf8b4);
MGLOBALA(char, s__l0_005cf8b0, 4, 0x005cf8b0);
MGLOBALA(char, s__l1_005cf8ac, 4, 0x005cf8ac);
MGLOBALA(char, s__l2_005cf8a8, 4, 0x005cf8a8);
MGLOBALA(char, s__l3_005cf8a4, 4, 0x005cf8a4);
MGLOBALA(char, s__l4_005cf8a0, 4, 0x005cf8a0);
MGLOBALA(char, s__l5_005cf89c, 4, 0x005cf89c);
MGLOBAL(int, DAT_006147bc, 0x006147bc);
MGLOBAL(int, DAT_007719e0, 0x007719e0);
MGLOBAL(int, DAT_006147c0, 0x006147c0);
MGLOBAL(int, DAT_007719e4, 0x007719e4);
MGLOBAL(int, DAT_007719e8, 0x007719e8);
char *strcpy(char *dst, const char *src);
#pragma intrinsic(strcpy)
char *FUN_00499730(void);
char *FUN_004a3ac9(char *s, const char *delim);
int FUN_004b302f(const char *a, const char *b);
// MATCH: 0x00493900 FUN_00493900
int FUN_00493900(void)
{
  char seps[2];
  char buf[256];
  char *s;
  char *tok;

  s = buf;
  *(short *)seps = 0x20;
  strcpy(buf, FUN_00499730());
  do {
    tok = FUN_004a3ac9(s, seps);
    s = 0;
    if (tok != 0) {
      if (FUN_004b302f(tok, s__vs0_005cf8cc) == 0) {
        DAT_006147bc = 0;
        DAT_007719e0 = 0;
      }
      else if (FUN_004b302f(tok, s__vs1_005cf8c4) == 0) {
        DAT_006147bc = 1;
        DAT_007719e0 = 1;
      }
      else if (FUN_004b302f(tok, s__cs0_005cf8bc) == 0) {
        DAT_006147c0 = 0;
        DAT_007719e4 = 0;
      }
      else if (FUN_004b302f(tok, s__cs1_005cf8b4) == 0) {
        DAT_006147c0 = 1;
        DAT_007719e4 = 1;
      }
      else if (FUN_004b302f(tok, s__l0_005cf8b0) == 0) {
        DAT_007719e8 = 0;
      }
      else if (FUN_004b302f(tok, s__l1_005cf8ac) == 0) {
        DAT_007719e8 = 1;
      }
      else if (FUN_004b302f(tok, s__l2_005cf8a8) == 0) {
        DAT_007719e8 = 2;
      }
      else if (FUN_004b302f(tok, s__l3_005cf8a4) == 0) {
        DAT_007719e8 = 3;
      }
      else if (FUN_004b302f(tok, s__l4_005cf8a0) == 0) {
        DAT_007719e8 = 4;
      }
      else if (FUN_004b302f(tok, s__l5_005cf89c) == 0) {
        DAT_007719e8 = 5;
      }
    }
  } while (tok != 0);
  return 1;
}
