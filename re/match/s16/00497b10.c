// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00497b10 (74 B), h4. 100%. Register-passed args (pilot finding 1): static, action in EAX,
   hDlg in EDI, control id in ESI; parameter order (action, hDlg, id) picks them ((action, id, hDlg) swapped
   EDI/ESI, 81%). The real caller is not in this file: a stand-in caller keeps the static and its convention. */
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
typedef struct HWND__h4 *HWND;
typedef void (__stdcall *TIMERPROC_h4)(HWND, unsigned int, unsigned int, unsigned long);
__declspec(dllimport) HWND __stdcall GetDlgItem(HWND hDlg, int id);
__declspec(dllimport) HWND __stdcall SetFocus(HWND h);
__declspec(dllimport) unsigned int __stdcall SetTimer(HWND h, unsigned int id, unsigned int ms, TIMERPROC_h4 fn);
MGLOBAL(int, DAT_007730b0, 0x007730b0);
MGLOBAL(int, DAT_0077315c, 0x0077315c);
MGLOBAL(int, DAT_00773160, 0x00773160);
MGLOBALA(int, DAT_007e96fc, 0x200, 0x007e96fc);
void __stdcall FUN_004976a0(HWND h, unsigned int m, unsigned int id, unsigned long t);
// MATCH: 0x00497b10 FUN_00497b10
static void FUN_00497b10(int action, HWND hDlg, int id)
{
    if (DAT_007e96fc[DAT_007730b0 * 0x80] == 1 && action >= 9 && action <= 12)
        return;
    DAT_00773160 = action;
    DAT_0077315c = id;
    SetTimer(hDlg, 20000, 0x14, FUN_004976a0);
    SetFocus(GetDlgItem(hDlg, id));
}

/* stand-in caller (not the real caller): keeps the static and its register convention */
void h4_standin_00497b10(int action, int id, HWND hDlg)
{
    FUN_00497b10(action, hDlg, id);
}
