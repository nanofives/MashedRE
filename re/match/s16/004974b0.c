// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004974b0 (488 B), h4. 100%, BYTE-IDENTICAL. Controller-select dialog init: localised captions via
   0x00499740 (string base DAT_007719e8), then four combos filled per player record type (1 joystick name at
   record +4, 2 keyboard string 0x007730b4, always the 0x007730f8 entry, CB_SETCURSEL 0).
   Register-passed hDlg (pilot finding 1): static, EAX in. The real caller is not in this file: a stand-in
   caller keeps the static and its convention. What moved it: an index loop `for (i = 0; i < 4; i++)` over
   records/combos (cl strength-reduces it to the pointer compare against 0x007e9efc); a pointer-walk loop gave
   a down counter (90.5%). Record layout ContCfg_h4 is a stand-in with only the fields read here. */
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
typedef struct ContCfg_h4 {
    int pad0;
    char name[0x138];        /* +0x004 */
    int type;                /* +0x13c: 1 joystick, 2 keyboard */
    unsigned char pad140[0x200 - 0x140];
} ContCfg_h4;
MGLOBAL(int, DAT_007719e8, 0x007719e8);
MGLOBALA(ContCfg_h4, DAT_007e95c0, 4, 0x007e95c0);
MGLOBALA(char, DAT_007730b4, 0x20, 0x007730b4);
MGLOBALA(char, DAT_007730f8, 0x24, 0x007730f8);
void FUN_00499740(HWND h, unsigned int id);
// MATCH: 0x004974b0 FUN_004974b0
static void FUN_004974b0(HWND hDlg)
{
    HWND combo[4];
    HWND h;
    int i;
    unsigned int idx;
    FUN_00499740(hDlg, DAT_007719e8 + 0x7ee);
    FUN_00499740(GetDlgItem(hDlg, 0x411), DAT_007719e8 + 0x7f8);
    FUN_00499740(GetDlgItem(hDlg, 0x412), DAT_007719e8 + 0x802);
    FUN_00499740(GetDlgItem(hDlg, 0x413), DAT_007719e8 + 0x80c);
    FUN_00499740(GetDlgItem(hDlg, 0x414), DAT_007719e8 + 0x816);
    FUN_00499740(GetDlgItem(hDlg, 0x3ef), DAT_007719e8 + 0x820);
    FUN_00499740(GetDlgItem(hDlg, 0x3f0), DAT_007719e8 + 0x820);
    FUN_00499740(GetDlgItem(hDlg, 0x3f1), DAT_007719e8 + 0x820);
    FUN_00499740(GetDlgItem(hDlg, 0x3f2), DAT_007719e8 + 0x820);
    FUN_00499740(GetDlgItem(hDlg, 1), DAT_007719e8 + 2000);
    combo[0] = GetDlgItem(hDlg, 0x3f3);
    combo[1] = GetDlgItem(hDlg, 0x3f4);
    combo[2] = GetDlgItem(hDlg, 0x3ea);
    combo[3] = GetDlgItem(hDlg, 0x3eb);
    for (i = 0; i < 4; i++) {
        if (DAT_007e95c0[i].type == 1) {
            h = combo[i];
            idx = SendMessageA(h, 0x143, 0, (long)DAT_007e95c0[i].name);
            SendMessageA(h, 0x151, idx, 1);
        } else if (DAT_007e95c0[i].type == 2) {
            h = combo[i];
            idx = SendMessageA(h, 0x143, 0, (long)DAT_007730b4);
            SendMessageA(h, 0x151, idx, 2);
        }
        h = combo[i];
        idx = SendMessageA(h, 0x143, 0, (long)DAT_007730f8);
        SendMessageA(h, 0x151, idx, 0);
        SendMessageA(h, 0x14e, 0, 0);
    }
}

/* stand-in caller (not the real caller): keeps the static and its EAX convention */
void h4_standin_004974b0(HWND hDlg)
{
    FUN_004974b0(hDlg);
}
