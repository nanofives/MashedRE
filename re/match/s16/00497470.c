// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00497470 (62 B), h4. Register-passed args (pilot finding 1): static, id in EAX, hDlg in ECX,
   player in EDX (parameter order id, hDlg, player picks them). The real caller is not in this file: a stand-in
   caller keeps the static alive and its convention. */
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
__declspec(dllimport) HWND __stdcall GetDlgItem(HWND hDlg, int id);
__declspec(dllimport) long __stdcall SendMessageA(HWND h, unsigned int m, unsigned int w, long l);
MGLOBALA(int, DAT_007e96fc, 0x200, 0x007e96fc);
// MATCH: 0x00497470 FUN_00497470
static int FUN_00497470(int id, HWND hDlg, int player)
{
    HWND h = GetDlgItem(hDlg, id);
    unsigned int sel = SendMessageA(h, 0x147, 0, 0);
    DAT_007e96fc[player * 0x80] = SendMessageA(h, 0x150, sel, 0);
    return 1;
}

/* stand-in caller (not the real caller): keeps the static and its register convention */
int h4_standin_00497470(int id, HWND hDlg, int player)
{
    return FUN_00497470(id, hDlg, player);
}
