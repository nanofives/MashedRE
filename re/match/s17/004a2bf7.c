// FLAGS: /O1
/* MATCH-S17 0x004a2bf7: CRT cmiscdat.c/fpinit.c _cfltcvt_init: fills _cfltcvt_tab[6] (0x006160d8) with _cfltcvt
   FUN_004a5d92, _cropzeros FUN_004a5a0c, _fassign FUN_004a5a71, _forcdecpt FUN_004a59d0, _positive FUN_004a5a57,
   _cfltcvt again. */
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
typedef void (__cdecl *PFV)(void);
MGLOBALA(PFV, DAT_006160d8, 6, 0x006160d8);
void FUN_004a5d92(void);
void FUN_004a5a0c(void);
void FUN_004a5a71(void);
void FUN_004a59d0(void);
void FUN_004a5a57(void);
// MATCH: 0x004a2bf7 FUN_004a2bf7
void FUN_004a2bf7(void)
{
    DAT_006160d8[0] = (PFV)FUN_004a5d92;
    DAT_006160d8[1] = (PFV)FUN_004a5a0c;
    DAT_006160d8[2] = (PFV)FUN_004a5a71;
    DAT_006160d8[3] = (PFV)FUN_004a59d0;
    DAT_006160d8[4] = (PFV)FUN_004a5a57;
    DAT_006160d8[5] = (PFV)FUN_004a5d92;
}
