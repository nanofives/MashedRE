// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x0048ff20 (h3): 100% (regs 100%), relocheck OK (unchecked: GS cookie helpers).
   Raw 91.1% (callback passed as a byte global). Callback FUN_0048fed0 now a real function pointer, 30-entry
   0x50-byte slot array at 0x0086a4a0 via an indexed loop, "Tri.dff" as MGLOBALA. No residual. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct Slot_48ff20 {
    int count;         /* +0x00 */
    int clump;         /* +0x04 */
    int atomic;        /* +0x08 */
    int rest[17];      /* +0x0c */
} Slot_48ff20;
MGLOBALA(Slot_48ff20, DAT_0086a4a0, 30, 0x0086a4a0);
#ifndef MATCH_PORT
MGLOBALA(char, s_Tri_dff_005cf4f8, 8, 0x005cf4f8);
#else
/* port: the exe does not load the original .rdata (its image pad reads 0), so the string is a copy of the 8 bytes
   at 0x005cf4f8 in MASHED.exe.unpatched (PORT-OT-TRIPOOL) */
static char s_Tri_dff_005cf4f8[8] = "Tri.dff";
#endif
void FUN_004b6520(char *buf, int n);
void FUN_004b65a0(void);
int FUN_004b65b0(char *name);
void FUN_004b3b70(char *name, char *buf, int a);
int FUN_004b3e40(char *buf, int n, int a, int b);
void *FUN_0048fed0(void *atomic, void *data);
int FUN_004e66d0(int clump, void *(*cb)(void *, void *), void *data);
int FUN_0048fef0(int param_1, void *param_2);
// MATCH: 0x0048ff20 FUN_0048ff20
void FUN_0048ff20(void)
{
    int i;
    char buf[1024];
    FUN_004b6520(buf, 0x400);
    FUN_004b65a0();
    FUN_004b3b70(s_Tri_dff_005cf4f8, buf, FUN_004b65b0(s_Tri_dff_005cf4f8));
    for (i = 0; i < 30; i++) {
        Slot_48ff20 *s = &DAT_0086a4a0[i];
        s->clump = FUN_004b3e40(buf, 0x400, 0, 0);
        s->count = 0;
        FUN_004e66d0(s->clump, FUN_0048fed0, s);
        FUN_0048fef0(s->atomic, s);
    }
}
