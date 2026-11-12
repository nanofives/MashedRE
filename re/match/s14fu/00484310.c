// FLAGS: /O2 /GS /Ob1
/* MATCH-S14-FU 0x00484310: copy of re/match/racepath/004844a0.c with 0x00484310 as a second same-TU static (world in ESI). */
/* MATCH-RACEPATH-FRESH 0x004844a0 + 0x00484580 (first hand source; raw g2s 44.3% / compile-fail).
   0x004844a0: 213 B whole symbol BYTE-IDENTICAL (Ghidra body 212 B stops before the ret). It is a `static`
   function: the caller 0x00484580 passes the world in EAX (mov eax,esi at 0x004846f0) and pushes a second
   argument the callee never reads. That is the register convention cl 13.00.9176 gives a same-TU static
   callee (as 0x00419090 in re/match/pilot/a6_00419090.c). The colour {0xff,0,0,0x40} is a byte struct
   stored inside the loop (0x00484538..). 0x00484580: 584 B whole symbol BYTE-IDENTICAL (includes the two
   jump tables at 0x00484778 / 0x004847ac). Closed by reading the old world pointer into a local before the
   +0xc store (0x00484580 loads DAT_006cfc94 first), each read case carrying its own FUN_004292d0 +
   FUN_004292c0 pair (cross-jumped by the compiler), and the else-first order of the 0x00484310 call.
   Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct { unsigned char r, g, b, a; } RwRGBA;
typedef int (*AtomicCB)(int atomic);
MGLOBAL(int, DAT_006cfc90, 0x006cfc90);
MGLOBAL(AtomicCB, DAT_006cf070, 0x006cf070);
MGLOBAL(int, DAT_006cfca4, 0x006cfca4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(char, DAT_0066d728, 0x0066d728);
MGLOBALA(int, DAT_006cf888, 1, 0x006cf888);
MGLOBAL(int, DAT_006cfc94, 0x006cfc94);
MGLOBAL(int, DAT_006cf478, 0x006cf478);
MGLOBALA(int, DAT_006cf480, 1, 0x006cf480);
MGLOBAL(int, DAT_006cf880, 0x006cf880);
MGLOBAL(int, DAT_006cfca8, 0x006cfca8);
MGLOBAL(int, DAT_006cfcac, 0x006cfcac);
MGLOBAL(int, DAT_006cfcb0, 0x006cfcb0);
MGLOBAL(int, DAT_006cfcb4, 0x006cfcb4);
MGLOBAL(int, DAT_006cfc98, 0x006cfc98);
MGLOBAL(int, DAT_006cfc9c, 0x006cfc9c);
AtomicCB FUN_004e5840(int w);
void FUN_004e5820(int w, AtomicCB cb);
void FUN_004e5660(int w);
int  FUN_0055dec0(void *L);
void FUN_004292d0(void *L, void *out);
void FUN_004292c0(void *L, int v);
void FUN_004b61c0(int geom, RwRGBA *color);
int  FUN_00484360(int atomic);
int  FUN_004b41b0(int w, int *out);
void FUN_004842b0(void);
void FUN_004842d0(void);
MGLOBAL(int, DAT_006cfc88, 0x006cfc88);
MGLOBAL(int, DAT_006cf06c, 0x006cf06c);
MGLOBAL(int, DAT_006cf068, 0x006cf068);
MGLOBAL(int, DAT_006cfc8c, 0x006cfc8c);

// MATCH: 0x00484310 _FUN_00484310
static void FUN_00484310(int world, int unused)
{
    DAT_006cf070 = FUN_004e5840(world);
    FUN_004e5820(world, (AtomicCB)FUN_004842d0);
    FUN_004e5660(world);
    FUN_004e5820(world, DAT_006cf070);
    DAT_006cf068 = DAT_006cfc88;
    DAT_006cfc8c = DAT_006cf06c;
}
void FUN_004b60c0(int p, int v);

// MATCH: 0x004844a0 _FUN_004844a0
static void FUN_004844a0(int world, int unused)
{
    int i;
    RwRGBA col;

    DAT_006cfc90 = 0;
    DAT_006cf070 = FUN_004e5840(world);
    FUN_004e5820(world, FUN_00484360);
    FUN_004e5660(world);
    FUN_004e5820(world, DAT_006cf070);
    if (FUN_0055dec0(&DAT_0066d728) == 2) {
        FUN_004292d0(&DAT_0066d728, &DAT_006cfca4);
        FUN_004292c0(&DAT_0066d728, 0);
    }
    if (DAT_006cfca4 != 0) {
        (*(void (**)(int, int))(DAT_007d3ff8 + 0x20))(6, 0);
        for (i = 0; i < DAT_006cfc90; i++) {
            col.r = 0xff; col.g = 0; col.b = 0; col.a = 0x40;
            FUN_004b61c0(DAT_006cf888[i], &col);
        }
        (*(void (**)(int, int))(DAT_007d3ff8 + 0x20))(6, 1);
    }
}

// MATCH: 0x00484580 _FUN_00484580
void FUN_00484580(int world, int s)
{
    int i;
    int prev;

    prev = DAT_006cfc94;
    *(int *)(world + 0xc) = 1;
    if (world != prev) {
        DAT_006cfc94 = world;
        DAT_006cf478 = FUN_004b41b0(world, DAT_006cf480);
    }
    FUN_004842b0();
    DAT_006cf880 = 0;
    switch (FUN_0055dec0(&DAT_0066d728)) {
    case 3:
        FUN_004292d0(&DAT_0066d728, &DAT_006cfcac);
        FUN_004292c0(&DAT_0066d728, 0);
        goto draw;
    case 0x14: DAT_006cfca8 = 0; break;
    case 0x15: DAT_006cfca8 = 1; break;
    case 0x16: DAT_006cfca8 = 2; break;
    case 0x17:
        if (DAT_006cfcb0 == 0) DAT_006cfcb0 = DAT_006cf478 - 1;
        else DAT_006cfcb0--;
        break;
    case 0x18: DAT_006cfcb0 = (DAT_006cfcb0 + 1) % DAT_006cf478; break;
    case 0x19:
        FUN_004292d0(&DAT_0066d728, &DAT_006cfc98);
        FUN_004292c0(&DAT_0066d728, 0);
        goto draw;
    case 0x1a:
        FUN_004292d0(&DAT_0066d728, &DAT_006cfc9c);
        FUN_004292c0(&DAT_0066d728, 0);
        goto draw;
    case 0x1b:
        if (DAT_006cfcb4 == 0) DAT_006cfcb4 = DAT_006cf478 - 1;
        else DAT_006cfcb4--;
        break;
    case 0x1c: DAT_006cfcb4 = (DAT_006cfcb4 + 1) % DAT_006cf478; break;
    case 0x1d: DAT_006cfcb0 = DAT_006cfcb4; break;
    case 0x1e: DAT_006cfcb4 = DAT_006cfcb0; break;
    default:
        goto draw;
    }
    FUN_004292c0(&DAT_0066d728, 0);
draw:
    if (DAT_006cfcac != 0)
        FUN_00484310(world, s);
    else
        FUN_004844a0(world, s);
    if (DAT_006cfc9c != 0)
        DAT_006cf070(DAT_006cf480[DAT_006cfcb4]);
    if (DAT_006cfca8 == 1) {
        for (i = 0; i < DAT_006cf478; i++)
            FUN_004b60c0(DAT_006cf480[i] + 0x60, 0);
    } else if (DAT_006cfca8 == 2) {
        FUN_004b60c0(DAT_006cf480[DAT_006cfcb0] + 0x60, 0);
    }
}

