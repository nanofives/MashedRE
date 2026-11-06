// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00498510 (653 B Ghidra size, 665 B obj symbol), h4. Best 95.3% per instruction (the 6 tail lines
   past Ghidra's size count as misses); match.py --bytes: 11 differing bytes, all from ONE instruction placed
   differently: in the restore copy `rec[i] = backup` the original emits lea esi / mov edi,ebx / mov ecx,0x80,
   ours emits mov ecx,0x80 first. Everything else (both loops, inlined strcpy/strcmp intrinsics, the hoisted
   strcpy delta spill, /GS frame with and esp,-8) is identical.
   Tried: pointer `*c = backup`, && condition, memcpy intrinsic, block-scoped temp, do-while loop, continue form,
   a static compare helper (82.9%), permuter 357 compiles (no gain). Residual class (b).
   Default controller setup: 6 language buffer pointers, keyboard / "none" strings, per-record defaults for 4
   records (joystick binds for i < n joysticks, keyboard binds for i == n), then contcfg%d.bin load per record
   keeping the defaults when the loaded name/f0 differ, then the optional dialog 0x67 (proc 0x00498360).
   ContCfg_h4 is a stand-in layout: the __int64 at +0x148 only gives the 8-byte alignment the frame shows.
   The file also holds its real static callees 0x004971b0 (EAX index, call at 0x00498705) and 0x00497190. */
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
typedef int (__stdcall *DLGPROC_h4)(HWND, unsigned int, unsigned int, long);
typedef struct _iobuf_h4 { char *_ptr; int _cnt; char *_base; int _flag; int _file; int _charbuf; int _bufsiz; char *_tmpfname; } FILE_h4;
typedef struct ContCfg_h4 {
    int f0;                  /* +0x000 */
    char name[0x104];        /* +0x004 */
    unsigned int bind[13];   /* +0x108 */
    int type;                /* +0x13c: 1 joystick, 2 keyboard, 0 none */
    int joy;                 /* +0x140 */
    int pad144;
    __int64 q148;            /* stand-in: the record copy needs 8-byte alignment (and esp,-8 frame) */
    unsigned char pad150[0xb0];
} ContCfg_h4;
__declspec(dllimport) int __stdcall DialogBoxParamA(HINSTANCE h, const char *tmpl, HWND parent, DLGPROC_h4 fn, long param);
__declspec(dllimport) int __stdcall LoadStringA(HINSTANCE h, unsigned int id, char *buf, int n);
char *strcpy(char *d, const char *s);
int strcmp(const char *a, const char *b);
#pragma intrinsic(strcpy, strcmp)
MGLOBAL(int, DAT_007719e8, 0x007719e8);
MGLOBALA(char, DAT_007730e4, 0x14, 0x007730e4);
MGLOBALA(char, s_contcfg_d_bin_005d000c, 14, 0x005d000c);
MGLOBALA(ContCfg_h4, DAT_007e95c0, 4, 0x007e95c0);
MGLOBALA(char, s_mode_005cf010, 3, 0x005cf010);
MGLOBALA(char, s_Attempting_to_load_005d0064, 41, 0x005d0064);
MGLOBALA(char, s_Loaded_005d0044, 30, 0x005d0044);
MGLOBALA(char, s_Unable_to_load_005d001c, 38, 0x005d001c);
MGLOBALA(char *, DAT_00773098, 6, 0x00773098);
MGLOBAL(char *, DAT_00773118, 0x00773118);
MGLOBALA(char, DAT_006147e8, 0x400, 0x006147e8);
MGLOBALA(char, DAT_00614be8, 0x400, 0x00614be8);
MGLOBALA(char, DAT_00614fe8, 0x400, 0x00614fe8);
MGLOBALA(char, DAT_006153e8, 0x400, 0x006153e8);
MGLOBALA(char, DAT_006157e8, 0x400, 0x006157e8);
MGLOBALA(char, DAT_00615be8, 0x400, 0x00615be8);
MGLOBALA(char, DAT_007730b4, 0x20, 0x007730b4);
MGLOBALA(char, DAT_007730f8, 0x24, 0x007730f8);
int FUN_004a2b60(char *buf, const char *fmt, ...);
void FUN_00496400(const char *fmt, ...);
FILE_h4 *FUN_004a4541(const char *name, const char *mode);
unsigned int FUN_004a49cf(void *buf, unsigned int size, unsigned int n, FILE_h4 *f);
int FUN_004a4368(FILE_h4 *f);
HWND FUN_00499710(void);
HINSTANCE FUN_00499720(void);
int FUN_00495790(void);
int FUN_00495830(int idx, char *name);
void FUN_004b6520(void *buf, int n);
int __stdcall FUN_00498360(HWND h, unsigned int m, unsigned int w, long l);
// MATCH: 0x00497190 FUN_00497190
static char *FUN_00497190(int idx)
{
    FUN_004a2b60(DAT_007730e4, s_contcfg_d_bin_005d000c, idx);
    return DAT_007730e4;
}

// MATCH: 0x004971b0 FUN_004971b0
static int FUN_004971b0(int idx)
{
    char *name = FUN_00497190(idx);
    FILE_h4 *f;
    FUN_00496400(s_Attempting_to_load_005d0064, name);
    f = FUN_004a4541(name, s_mode_005cf010);
    if (f != 0) {
        FUN_004a49cf(&DAT_007e95c0[idx], 1, 0x200, f);
        FUN_004a4368(f);
        FUN_00496400(s_Loaded_005d0044, name);
        return 1;
    }
    FUN_00496400(s_Unable_to_load_005d001c, name);
    return 0;
}

// MATCH: 0x00498510 FUN_00498510
void FUN_00498510(int show, int lang)
{
    int n;
    int i;
    ContCfg_h4 backup;
    DAT_00773098[0] = DAT_006147e8;
    DAT_00773098[1] = DAT_00614be8;
    DAT_00773098[2] = DAT_00614fe8;
    DAT_00773098[3] = DAT_006153e8;
    DAT_00773098[4] = DAT_006157e8;
    DAT_00773098[5] = DAT_00615be8;
    DAT_00773118 = DAT_00773098[lang];
    LoadStringA(FUN_00499720(), DAT_007719e8 + 0x82a, DAT_007730b4, 0x20);
    LoadStringA(FUN_00499720(), DAT_007719e8 + 0x834, DAT_007730f8, 0x20);
    n = FUN_00495790();
    FUN_004b6520(DAT_007e95c0, 0x800);
    for (i = 0; i < 4; i++) {
        ContCfg_h4 *c = &DAT_007e95c0[i];
        c->f0 = 6;
        if (i < n) {
            c->type = 1;
            c->joy = i;
            FUN_00495830(i, c->name);
            c->bind[0] = 0;
            c->bind[1] = 1;
            c->bind[2] = 2;
            c->bind[3] = 3;
            c->bind[4] = 0;
            c->bind[5] = 1;
            c->bind[6] = 4;
            c->bind[7] = 2;
            c->bind[8] = 5;
        } else if (i == n) {
            c->type = 2;
            strcpy(c->name, DAT_007730b4);
            c->bind[0] = 0x1f;
            c->bind[1] = 0x2d;
            c->bind[2] = 0x1e;
            c->bind[3] = 0x20;
            c->bind[4] = 0x1c;
            c->bind[5] = 1;
            c->bind[6] = 1;
            c->bind[7] = 0x1e;
            c->bind[8] = 0x2e;
            c->bind[9] = 0xcb;
            c->bind[10] = 0xcd;
            c->bind[11] = 0xc8;
            c->bind[12] = 0xd0;
        } else {
            c->type = 0;
        }
    }
    for (i = 0; i < 4; i++) {
        backup = DAT_007e95c0[i];
        if (FUN_004971b0(i)) {
            if (DAT_007e95c0[i].f0 != backup.f0 || strcmp(DAT_007e95c0[i].name, backup.name) != 0)
                DAT_007e95c0[i] = backup;
        }
    }
    if (show)
        DialogBoxParamA(FUN_00499720(), (const char *)0x67, FUN_00499710(), FUN_00498360, 0);
}
