// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00497230 (115 B), h4. Controller config save (contcfg%d.bin, fwrite of 0x200 bytes).
   Register-passed index (pilot finding 1): static, EAX in. The file also holds its real static callee
   0x00497190 (EAX in). The real callers (0x00498510 / 0x004982a0) are not in this file: a stand-in caller keeps
   the statics alive and their convention. */
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

/* stand-in caller (not the real caller): keeps the statics and their EAX convention */
int h4_standin_00497230(int idx)
{
    return FUN_00497230(idx);
}
