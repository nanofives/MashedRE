// FLAGS: /O2 /GS
// TOOLCHAIN ddk
/* TL-MATCH-REST 0x004b4eb0, rewritten by RACE-LOAD-004E99B0 (run 57, 2026-11-27) from the disassembly of 0x004b4eb0..0x004b502f
   (Ghidra pool slot Mashed_pool0, read-only). The earlier raw g2s body (70.9%) left out the count update and the early
   return at the limit (0x004b4ffd..0x004b500a) and its preamble redefined bool / wchar_t (not C++). Per-sector callback of
   0x004b5030 (passed to 0x004e5c70 at 0x004b5058). Args: sector, ctx = {int *key, int *out, int *count, int limit}
   (0x004b4eb7..0x004b4ec1). r = FUN_004e55d0(sector) (0x004b4edf); for each triangle i < (ushort)sector +0x84: when
   [[r +0x10][tri.w3 + (ushort)sector +0x80] +4] == *key (0x004b4f26), and out != 0, the 12 dwords at out + count * 0x30 get
   the three vertices (sector +0x08, 12 bytes each), the sector, and the triangle's 8 bytes; then ++*count, and the walk
   returns 0 once *count == limit (0x004b500a). Returns the sector otherwise. */
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
int FUN_004e55d0(int sector);

// MATCH: 0x004b4eb0 FUN_004b4eb0
int FUN_004b4eb0(int sector, int *ctx)
{
    int *key = (int *)ctx[0];
    int *out = (int *)ctx[1];
    int *count = (int *)ctx[2];
    int limit = ctx[3];
    int *o = out + *count * 12;
    int r = FUN_004e55d0(sector);
    int i;

    for (i = 0; i < (int)*(unsigned short *)(sector + 0x84); i++) {
        unsigned short *t = (unsigned short *)(*(int *)(sector + 4) + i * 8);
        int mat = *(int *)(*(int *)(*(int *)(r + 0x10) + ((unsigned int)t[3] + *(unsigned short *)(sector + 0x80)) * 4) + 4);
        if (mat != *key) {
            continue;
        }
        if (out != 0) {
            int *v = (int *)*(int *)(sector + 8);
            o[0] = v[t[0] * 3]; o[1] = v[t[0] * 3 + 1]; o[2] = v[t[0] * 3 + 2];
            o[3] = v[t[1] * 3]; o[4] = v[t[1] * 3 + 1]; o[5] = v[t[1] * 3 + 2];
            o[6] = v[t[2] * 3]; o[7] = v[t[2] * 3 + 1]; o[8] = v[t[2] * 3 + 2];
            o[9] = sector;
            o[10] = *(int *)t;
            o[11] = *(int *)(t + 2);
            o += 12;
        }
        *count = *count + 1;
        if (*count == limit) {
            return 0;
        }
    }
    return sector;
}
