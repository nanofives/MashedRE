// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00426e10 (723 B): 100.0% exact (regs 100, ops 100), relocheck OK (unchecked: the /GS cookie
   helpers only). Raw converter 69.9%. Ghidra's unaff_retaddr is the /GS cookie (cookie ^ [ebp+4]), not a
   register argument, so no static callee is needed. Written by hand from the disassembly: `char path[256] = ""`
   (byte store + rep stosd/stosw/stosb), RwGlobals @0x007d3ff8 as a struct with the string-copy pointers at +0xcc
   and +0xd4, intrinsic strcpy for the inline copy of FUN_004260e0's result into 0x008991e0, FUN_004c1b10 with
   2 args (pointer, float), FUN_0042f510 with 0 args, FUN_004987b0 as varargs cdecl. What moved the last 4.6%:
   the AI-data message is `if (x) print(A); else print(B);` (tail-merged call); a ternary argument or a msg
   variable gives a `mov eax, A; jne; mov eax, B` select and merges the following `add esp` (95.4%).
   Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
typedef struct RwGlobals {
    char pad0[0xcc];
    char *(*rwstrcpy)(char *, const char *);  /* +0xcc */
    void *pad1;                               /* +0xd0 */
    char *(*rwstrcat)(char *, const char *);  /* +0xd4 */
} RwGlobals;
MGLOBAL(RwGlobals *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00656ed8, 0x00656ed8);
MGLOBAL(int, DAT_00657430, 0x00657430);
MGLOBAL(float, DAT_00657450, 0x00657450);
MGLOBAL(float, DAT_00657454, 0x00657454);
MGLOBAL(unsigned char, DAT_0065744c, 0x0065744c);
MGLOBAL(unsigned char, DAT_0065744d, 0x0065744d);
MGLOBAL(unsigned char, DAT_0065744e, 0x0065744e);
MGLOBAL(int, DAT_0066d704, 0x0066d704);
MGLOBAL(int, DAT_00657448, 0x00657448);
MGLOBALA(char, DAT_005cd280, 4, 0x005cd280);
MGLOBALA(char, DAT_00644158, 4, 0x00644158);
MGLOBALA(char, DAT_00644378, 4, 0x00644378);
MGLOBALA(char, DAT_00646e58, 4, 0x00646e58);
MGLOBALA(char, DAT_00657498, 4, 0x00657498);
MGLOBALA(char, DAT_008991e0, 256, 0x008991e0);
void FUN_004053d0(int p0, int p1);
void FUN_00409680(void);
int FUN_00409710(void);
void FUN_00412050(void);
void FUN_0041e870(int p0);
void FUN_0041e8b0(char *p0);
void FUN_0041e970(char *p0);
char *FUN_0041e980(int p0);
int FUN_0041e9d0(void);
int FUN_0041ea90(void);
int FUN_004235b0(void);
void FUN_00423630(void);
void FUN_00425ab0(void);
char *FUN_004260e0(int p0, char *p1, char *p2, char *p3);
void FUN_004262f0(void);
int FUN_00426cd0(void);
int FUN_0042a530(char *p0, char *p1);
char *FUN_0042a8d0(char *p0);
int FUN_0042f510(void);
void FUN_00462950(int p0);
int FUN_004671a0(int p0);
void FUN_004715a0(void);
void FUN_00478660(char *p0, int p1);
int FUN_00479330(char *p0, char *p1, char *p2);
int FUN_0047a020(char *p0, char *p1, char *p2);
int FUN_0047a0f0(char *p0, char *p1);
void FUN_0047c0b0(void);
void FUN_0047c0f0(int p0);
void FUN_00480340(void);
int FUN_00491780(void);
void FUN_004924c0(unsigned char p0, unsigned char p1, unsigned char p2);
void FUN_00495280(char *p0);
void FUN_004952f0(void);
void FUN_004987b0(const char *fmt, ...);
int FUN_004c1b10(int p0, float p1);
char *strcpy(char *, const char *);
#pragma intrinsic(strcpy)
// MATCH: 0x00426e10 FUN_00426e10
void FUN_00426e10(int track)
{
    char path[256] = "";
    char *name;

    name = FUN_0041e980(track);
    DAT_007d3ff8->rwstrcpy(path, "toastart/tracks/");
    DAT_007d3ff8->rwstrcat(path, name);
    DAT_007d3ff8->rwstrcat(path, DAT_005cd280);
    DAT_007d3ff8->rwstrcpy(DAT_008991e0, FUN_0042a8d0(path));
    FUN_00426cd0();
    FUN_0041e870(track);
    if (FUN_0041ea90())
        FUN_0041e970(DAT_00646e58);
    FUN_004987b0("Opening piz file [%s]\n", DAT_008991e0);
    FUN_00495280(DAT_008991e0);
    strcpy(DAT_008991e0, FUN_004260e0(0, name, "COURSE.LUA", ""));
    FUN_0042a530(path, "COURSE.LUA");
    FUN_0047a020("COURSE.LUA", DAT_00644378, DAT_00644158);
    FUN_00479330(DAT_00646e58, DAT_00644378, DAT_00644158);
    FUN_0042a530(path, "LAPDATA.LUA");
    FUN_0047a0f0("LAPDATA.LUA", DAT_00657498);
    FUN_00478660(DAT_00657498, DAT_00656ed8);
    FUN_00480340();
    FUN_004715a0();
    FUN_00412050();
    FUN_004952f0();
    FUN_00409680();
    if (FUN_00409710()) {
        FUN_004987b0("Loaded existing line exceptions\n");
    } else {
        FUN_004987b0("clearing all line exceptions\n");
        FUN_00409680();
    }
    FUN_00423630();
    if (FUN_004235b0())
        FUN_004987b0("Loaded AI data\n");
    else
        FUN_004987b0("No AI Data loaded\n");
    FUN_00425ab0();
    FUN_004262f0();
    FUN_00462950(track);
    FUN_0047c0b0();
    if (DAT_00657430)
        FUN_0047c0f0(DAT_00657430);
    if (DAT_00657450 != DAT_00657454) {
        FUN_004c1b10(FUN_004671a0(0), DAT_00657454);
        *(float *)(FUN_004671a0(0) + 0x88) = DAT_00657450;
        FUN_004671a0(0);
        FUN_004c1b10(FUN_0042f510(), DAT_00657454);
        *(float *)(FUN_0042f510() + 0x88) = DAT_00657450;
        FUN_0042f510();
        FUN_004924c0(DAT_0065744c, DAT_0065744d, DAT_0065744e);
    }
    if (FUN_0041e9d0())
        FUN_0041e8b0(DAT_00646e58);
    FUN_00491780();
    DAT_0066d704 = 1;
    FUN_004053d0(DAT_00657448, FUN_004671a0(0));
}
