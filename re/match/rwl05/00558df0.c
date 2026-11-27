// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00558df0, rewritten by RACE-LOAD-004E99B0 (run 57, 2026-11-27) from the disassembly of
   0x00558df0..0x00559032 (Ghidra pool slot Mashed_pool0, read-only). The earlier raw g2s.py body (39.7%) kept the error
   record as two separate locals passed by address and did not compile as C++. Frame (S = esp after `sub esp,0x10`):
     S+0x00 chunk version, S+0x04 chunk size (0x00558df3 / 0x00558df7, FUN_004cc5e0(stream, 1, &size, &version))
     S+0x08 2-dword error record {0x1bf, FUN_004d7ff0(0x80000013, 8)} (0x00558e7e / 0x00558e8f)
     S+0x18 the stream argument's slot, overwritten with the entry count FUN_004cbd30 reads (0x00558e3c; stream stays in EBP)
   Args: desc (a 0x30+ byte record: +0x04 alloc hint, +0x08 entry chunk type, +0x0c version floor, +0x10 list, +0x14,
   +0x18 / +0x1c / +0x28 / +0x2c code pointers), stream. Version below desc +0x0c: tail call (*(desc+0x2c))(stream)
   (0x00558e24). Allocs through RwGlobals (DAT_007d3ff8) +0x108, frees through +0x10c. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);

typedef struct { int a; int b; } Err_00558df0;
typedef int (*Fn1_00558df0)(int);
typedef int (*Alloc_00558df0)(uint size, uint hint);

int FUN_004cc5e0(int stream, int type, uint *size, uint *version);
uint FUN_004cbd30(int stream, void *buf, uint size);
int FUN_004d7ff0(int code, ...);
void FUN_004d8480(Err_00558df0 *err);
int FUN_0055deb0(int list);
int FUN_0055dec0(int list);
int FUN_005c4ad0(int size, uint hint);
int FUN_005c4bb0(int list, uint hint);
void FUN_005c4d20(int list, int n);
void FUN_005c4d50(int list);
int FUN_005c4df0(int list, int i);
int FUN_005c4e10(int list);

#define F1_00558df0(rec, off) (*(Fn1_00558df0 *)((rec) + (off)))

// MATCH: 0x00558df0 FUN_00558df0
int FUN_00558df0(int desc, int stream)
{
    uint version;
    uint size;
    Err_00558df0 err;
    int count;
    int dict;
    int *slot;
    int entry;
    int list;
    int *p;
    int *end;

    if (!FUN_004cc5e0(stream, 1, &size, &version)) {
        return 0;
    }
    if (version < *(uint *)(desc + 0xc)) {
        return F1_00558df0(desc, 0x2c)(stream);
    }
    if (FUN_004cbd30(stream, &count, size) == 0) {
        return 0;
    }
    dict = (*(Alloc_00558df0 *)(DAT_007d3ff8 + 0x108))(8, *(uint *)(desc + 4) | 0x30000);
    if (dict == 0) {
        err.a = 0x1bf;
        err.b = FUN_004d7ff0(0x80000013, 8);
        FUN_004d8480(&err);
        return 0;
    }
    *(int *)(dict + 4) = FUN_005c4ad0(4, *(uint *)(desc + 4) | 0x30000);
    if (*(int *)(dict + 4) == 0) {
        (*(Fn1_00558df0 *)(DAT_007d3ff8 + 0x10c))(dict);
        return 0;
    }
    *(int *)dict = desc;
    slot = (int *)FUN_005c4bb0(*(int *)(desc + 0x10), *(uint *)(desc + 4) | 0x30000);
    if (slot == 0) {
        FUN_005c4d50(*(int *)(dict + 4));
        (*(Fn1_00558df0 *)(DAT_007d3ff8 + 0x10c))(dict);
        return 0;
    }
    *slot = dict;
    if (count == 0) {
        return dict;
    }
    do {
        count--;
        if (!FUN_004cc5e0(stream, *(int *)(desc + 8), &size, &version)) {
            goto fail;
        }
        entry = F1_00558df0(desc, 0x28)(stream);
        if (entry == 0) {
            goto fail;
        }
        *(int *)FUN_005c4bb0(*(int *)(dict + 4), *(uint *)(*(int *)dict + 4) | 0x30000) = entry;
        F1_00558df0(*(int *)dict, 0x18)(entry);
        F1_00558df0(desc, 0x1c)(entry);
    } while (count != 0);
    return dict;
fail:
    if (dict == *(int *)(*(int *)dict + 0x14)) {
        *(int *)(*(int *)dict + 0x14) = 0;
    }
    list = *(int *)dict;   /* desc; its +0x10 list is re-read at every call, as 0x00558f8a..0x00558fcc do */
    p = (int *)FUN_0055dec0(*(int *)(list + 0x10));
    end = (int *)FUN_005c4e10(*(int *)(list + 0x10));
    for (; p != end; p++) {
        if (*p == dict) {
            *p = *(int *)FUN_005c4df0(*(int *)(list + 0x10), FUN_0055deb0(*(int *)(list + 0x10)) - 1);
            FUN_005c4d20(*(int *)(list + 0x10), 1);
            break;
        }
    }
    end = (int *)FUN_005c4e10(*(int *)(dict + 4));
    for (p = (int *)FUN_0055dec0(*(int *)(dict + 4)); p != end; p++) {
        F1_00558df0(*(int *)dict, 0x1c)(*p);
    }
    FUN_005c4d50(*(int *)(dict + 4));
    (*(Fn1_00558df0 *)(DAT_007d3ff8 + 0x10c))(dict);
    return 0;
}
