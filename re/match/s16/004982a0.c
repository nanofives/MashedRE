// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004982a0 (191 B), h4. 100%, BYTE-IDENTICAL. Controller config dialog for one player: back up the
   0x200 B record, DialogBoxParamA(template 0x68, proc 0x00497fd0), save on IDOK (1), restore on 0.
   What moved it: the backup is a struct copy of a type that (1) needs 8-byte alignment (the and esp,-8 frame)
   and (2) holds a char array (the /GS cookie). ContCfgRec_h4 = { __int64; char[0x1f8] } is a stand-in layout that
   gives both; the real field layout is not known. The file also holds its real static callees 0x00497230
   (EAX player, called at 0x0049832d) and 0x00497190. */
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
typedef struct ContCfgRec_h4 { __int64 q; char s[0x1f8]; } ContCfgRec_h4;
__declspec(dllimport) int __stdcall DialogBoxParamA(HINSTANCE h, const char *tmpl, HWND parent, DLGPROC_h4 fn, long param);
MGLOBAL(int, DAT_007730b0, 0x007730b0);
MGLOBALA(int, DAT_007e96fc, 0x200, 0x007e96fc);
HWND FUN_00499710(void);
HINSTANCE FUN_00499720(void);
int __stdcall FUN_00497fd0(HWND h, unsigned int m, unsigned int w, long l);
typedef struct _iobuf_h4 { char *_ptr; int _cnt; char *_base; int _flag; int _file; int _charbuf; int _bufsiz; char *_tmpfname; } FILE_h4;
MGLOBALA(char, DAT_007730e4, 0x14, 0x007730e4);
MGLOBALA(char, s_contcfg_d_bin_005d000c, 14, 0x005d000c);
MGLOBALA(unsigned char, DAT_007e95c0, 0x800, 0x007e95c0);
MGLOBALA(char, s_mode_005d00d8, 3, 0x005d00d8);
MGLOBALA(char, s_Attempting_to_save_005d00dc, 41, 0x005d00dc);
MGLOBALA(char, s_Saved_005d00b8, 29, 0x005d00b8);
MGLOBALA(char, s_Unable_to_save_005d0090, 38, 0x005d0090);
int FUN_004a2b60(char *buf, const char *fmt, ...);
void FUN_00496400(const char *fmt, ...);
FILE_h4 *FUN_004a4541(const char *name, const char *mode);
unsigned int FUN_004a4b22(void *buf, unsigned int size, unsigned int n, FILE_h4 *f);
int FUN_004a4368(FILE_h4 *f);
// MATCH: 0x00497190 FUN_00497190
static char *FUN_00497190(int idx)
{
    FUN_004a2b60(DAT_007730e4, s_contcfg_d_bin_005d000c, idx);
    return DAT_007730e4;
}

// MATCH: 0x00497230 FUN_00497230
static int FUN_00497230(int idx)
{
    char *name = FUN_00497190(idx);
    FILE_h4 *f;
    FUN_00496400(s_Attempting_to_save_005d00dc, name);
    f = FUN_004a4541(name, s_mode_005d00d8);
    if (f != 0) {
        FUN_004a4b22(&DAT_007e95c0[idx * 0x200], 1, 0x200, f);
        FUN_004a4368(f);
        FUN_00496400(s_Saved_005d00b8, name);
        return 1;
    }
    FUN_00496400(s_Unable_to_save_005d0090, name);
    return 0;
}

// MATCH: 0x004982a0 FUN_004982a0
int FUN_004982a0(int player)
{
    ContCfgRec_h4 *rec;
    ContCfgRec_h4 backup;
    int r;
    if (DAT_007e96fc[player * 0x80] == 0)
        return 1;
    rec = (ContCfgRec_h4 *)&DAT_007e95c0[player * 0x200];
    DAT_007730b0 = player;
    backup = *rec;
    r = DialogBoxParamA(FUN_00499720(), (const char *)0x68, FUN_00499710(), FUN_00497fd0, 0);
    if (r == 1)
        FUN_00497230(player);
    else if (r == 0)
        *rec = backup;
    return r;
}
