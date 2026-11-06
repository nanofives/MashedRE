// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00497b60 (1135 B), h4. Best 75.6% (regs 97.1%, ops 98.9%), 348/349 ins. Controller-binding dialog
   init: caption "<string 0x83e> %d  -  %s" (inline strcat intrinsic), localised labels via 0x00499740, 13
   button bitmaps (LoadBitmapA from table 0x00615fe8 for joysticks, DAT_00773118 for keyboard) set with
   BM_SETIMAGE, 8 (+4 for keyboard) controls hidden.
   Residual: register permutation only. Original keeps hDlg in EBP (loaded in the prologue), GetDlgItem in ESI,
   ShowWindow in EDI (shared with the loop index); ours puts hDlg in ESI (loaded after the strcat's movs),
   GetDlgItem in EDI, ShowWindow/index in EBP. Every other instruction lines up (the remaining misses are the
   SetWindowTextA push interleave and one loop-alignment pad, both from that assignment).
   Tried: binding read hoisted before the type test (+2.8, kept), ids[] initializer, switch on type, local copy
   of hDlg, block-scoped loop temps with switch, hInst/rec order, permuter 740 compiles (its only gain, 95.7%,
   moved strcat after SetWindowTextA, which changes behaviour, rejected). Residual class (b).
   ContCfg_h4 is a stand-in layout with only the fields read here. */
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
typedef struct HINSTANCE__h4 *HINSTANCE;
typedef struct HBITMAP__h4 *HBITMAP;
__declspec(dllimport) HWND __stdcall GetDlgItem(HWND hDlg, int id);
__declspec(dllimport) long __stdcall SendMessageA(HWND h, unsigned int m, unsigned int w, long l);
__declspec(dllimport) int __stdcall LoadStringA(HINSTANCE h, unsigned int id, char *buf, int n);
__declspec(dllimport) int __stdcall SetWindowTextA(HWND h, const char *s);
__declspec(dllimport) HBITMAP __stdcall LoadBitmapA(HINSTANCE h, const char *name);
__declspec(dllimport) int __stdcall ShowWindow(HWND h, int cmd);
char *strcat(char *d, const char *s);
#pragma intrinsic(strcat)
typedef struct ContCfg_h4 {
    int f0;                  /* +0x000 */
    char name[0x104];        /* +0x004 */
    unsigned int bind[13];   /* +0x108 */
    int type;                /* +0x13c: 1 joystick, 2 keyboard */
    unsigned char pad140[0x200 - 0x140];
} ContCfg_h4;
MGLOBAL(int, DAT_007719e8, 0x007719e8);
MGLOBAL(int, DAT_007730b0, 0x007730b0);
MGLOBALA(ContCfg_h4, DAT_007e95c0, 4, 0x007e95c0);
MGLOBALA(char, s__d_____s_005d010c, 11, 0x005d010c);
MGLOBALA(int, DAT_00615fe8, 0x100, 0x00615fe8);
MGLOBAL(int *, DAT_00773118, 0x00773118);
HINSTANCE FUN_00499720(void);
void FUN_00499740(HWND h, unsigned int id);
int FUN_004a2b60(char *buf, const char *fmt, ...);
// MATCH: 0x00497b60 FUN_00497b60
void FUN_00497b60(HWND hDlg)
{
    char title[256] = "";
    char line[128] = "";
    int ids[13];
    HINSTANCE hInst;
    ContCfg_h4 *rec;
    int i;
    HWND h;
    HBITMAP bmp;
    unsigned int b;
    ids[0] = 0x3f6;
    ids[1] = 0x3f9;
    ids[2] = 0x3fb;
    ids[3] = 0x3fd;
    ids[4] = 0x3ff;
    ids[5] = 0x401;
    ids[6] = 0x403;
    ids[7] = 0x405;
    ids[8] = 0x407;
    ids[9] = 0x409;
    ids[10] = 0x40b;
    ids[11] = 0x40d;
    ids[12] = 0x40f;
    hInst = FUN_00499720();
    rec = &DAT_007e95c0[DAT_007730b0];
    LoadStringA(FUN_00499720(), DAT_007719e8 + 0x83e, title, 0x100);
    FUN_004a2b60(line, s__d_____s_005d010c, DAT_007730b0 + 1, rec->name);
    strcat(title, line);
    SetWindowTextA(hDlg, title);
    if (rec->type == 1) {
        FUN_00499740(GetDlgItem(hDlg, 0x3f7), DAT_007719e8 + 0x848);
        FUN_00499740(GetDlgItem(hDlg, 0x3f8), DAT_007719e8 + 0x852);
    } else if (rec->type == 2) {
        FUN_00499740(GetDlgItem(hDlg, 0x3f7), DAT_007719e8 + 0x8f2);
        FUN_00499740(GetDlgItem(hDlg, 0x3f8), DAT_007719e8 + 0x8fc);
    }
    FUN_00499740(GetDlgItem(hDlg, 0x3fa), DAT_007719e8 + 0x85c);
    FUN_00499740(GetDlgItem(hDlg, 0x3fc), DAT_007719e8 + 0x866);
    FUN_00499740(GetDlgItem(hDlg, 0x402), DAT_007719e8 + 0x870);
    FUN_00499740(GetDlgItem(hDlg, 0x406), DAT_007719e8 + 0x87a);
    FUN_00499740(GetDlgItem(hDlg, 0x408), DAT_007719e8 + 0x884);
    FUN_00499740(GetDlgItem(hDlg, 0x40a), DAT_007719e8 + 0x88e);
    FUN_00499740(GetDlgItem(hDlg, 0x40c), DAT_007719e8 + 0x898);
    FUN_00499740(GetDlgItem(hDlg, 0x40e), DAT_007719e8 + 0x8a2);
    FUN_00499740(GetDlgItem(hDlg, 0x415), DAT_007719e8 + 0x8ac);
    FUN_00499740(GetDlgItem(hDlg, 0x416), DAT_007719e8 + 0x8b6);
    FUN_00499740(GetDlgItem(hDlg, 0x410), DAT_007719e8 + 2000);
    FUN_00499740(GetDlgItem(hDlg, 0x411), DAT_007719e8 + 0x7da);
    for (i = 0; i < 13; i++) {
        h = GetDlgItem(hDlg, ids[i]);
        b = rec->bind[i];
        bmp = 0;
        if (rec->type == 1)
            bmp = LoadBitmapA(hInst, (const char *)(unsigned short)DAT_00615fe8[b]);
        else if (rec->type == 2)
            bmp = LoadBitmapA(hInst, (const char *)(unsigned short)DAT_00773118[b]);
        SendMessageA(h, 0xf7, 0, (long)bmp);
    }
    ShowWindow(GetDlgItem(hDlg, 0x409), 0);
    ShowWindow(GetDlgItem(hDlg, 0x40b), 0);
    ShowWindow(GetDlgItem(hDlg, 0x40d), 0);
    ShowWindow(GetDlgItem(hDlg, 0x40f), 0);
    ShowWindow(GetDlgItem(hDlg, 0x408), 0);
    ShowWindow(GetDlgItem(hDlg, 0x40a), 0);
    ShowWindow(GetDlgItem(hDlg, 0x40c), 0);
    ShowWindow(GetDlgItem(hDlg, 0x40e), 0);
    if (rec->type == 2) {
        ShowWindow(GetDlgItem(hDlg, 0x403), 0);
        ShowWindow(GetDlgItem(hDlg, 0x402), 0);
        ShowWindow(GetDlgItem(hDlg, 0x41b), 0);
        ShowWindow(GetDlgItem(hDlg, 0x41c), 0);
    }
}
