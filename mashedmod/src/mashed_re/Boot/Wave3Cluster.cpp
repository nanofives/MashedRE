// Mashed RE — Boot/Wave3Cluster.cpp
// Wave 3 Session 2 skeleton cluster: 8 mixed reimpls.
//
// Covers:
//   0x004274d0  LangIndexCopy        — copies DAT_007719e8 -> DAT_007f0f60; returns 1
//   0x004274e0  LangFileLoad         — switch lang-index -> open .dat from PIZ; returns 0/1
//   0x004c30b0  RwEngineOpen         — RwEngineOpen wrapper; vtable alloc + cmd 0/4/0xb
//   0x004c32b0  RwEngineInit         — RwEngineInit wrapper; 14x FUN_004d7de0 + state[0x49]=1
//   0x00496370  DInputKeyboardRelease — Unacquire + COM Release + zero DAT_00772fb8
//   0x00496970  HandleRelease         — if DAT_0077307c: FUN_004c7650 + zero; returns 1
//   0x00492440  RenderStatsAccumulate — 60-frame rolling avg + max tracker; pure leaf
//   0x004927c0  RacePairIndexFSM      — 12-state gate-pair progression FSM
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis refs:
//   re/analysis/skeleton_prep_game_state/004274d0.md
//   re/analysis/skeleton_prep_game_state/004274e0.md
//   re/analysis/skeleton_prep_high_leverage/004c30b0.md
//   re/analysis/skeleton_prep_high_leverage/004c32b0.md
//   re/analysis/skeleton_prep_boot_winmain_b/00496370.md
//   re/analysis/skeleton_prep_boot_winmain_b/00496970.md
//   re/analysis/skeleton_prep_boot_winmain_a/00492440.md
//   re/analysis/skeleton_prep_boot_winmain_a/004927c0.md

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>


// ============================================================================
// Callee function-pointer declarations
// (File-static to avoid link-time collisions across TUs.)
// ============================================================================

// 0x004cc230 — PIZ archive file-open: int(int type, int mode, const char* name)
// Cited in FUN_004274e0 body as arg1=2, arg2=1.
// S-0860 depth-2 via FUN_004274e0.
using FUN_004cc230_t = int (__cdecl*)(int, int, const char*);
static FUN_004cc230_t const s_FUN_004cc230 =
    reinterpret_cast<FUN_004cc230_t>(0x004cc230u);

// 0x004cbd30 — PIZ stream read: int(int handle, void* buf, int max_bytes)
// Cited in FUN_004274e0 body with max_bytes=0x10000 into DAT_0066d828.
// S-0861 depth-2.
using FUN_004cbd30_t = int (__cdecl*)(int, void*, int);
static FUN_004cbd30_t const s_FUN_004cbd30 =
    reinterpret_cast<FUN_004cbd30_t>(0x004cbd30u);

// 0x004cc160 — PIZ stream close: void(int handle, int mode)
// Cited in FUN_004274e0 body with mode=0.
// S-0862 depth-2.
using FUN_004cc160_t = void (__cdecl*)(int, int);
static FUN_004cc160_t const s_FUN_004cc160 =
    reinterpret_cast<FUN_004cc160_t>(0x004cc160u);

// 0x004c2c90 — RW command dispatcher: int(int dev, int cmd, void* a, void* b, int c)
// Cited in FUN_004c30b0 body; three calls with cmd 4, 0, 0xb.
// S-0081 pre-existing.
using FUN_004c2c90_t = int (__cdecl*)(int, int, void*, void*, int);
static FUN_004c2c90_t const s_FUN_004c2c90 =
    reinterpret_cast<FUN_004c2c90_t>(0x004c2c90u);

// 0x004cae90 — device open query: int(void)
// Cited in FUN_004c30b0 body; if returns 0, RwEngineOpen returns 0.
// S pre-existing depth-2.
using FUN_004cae90_t = int (__cdecl*)();
static FUN_004cae90_t const s_FUN_004cae90 =
    reinterpret_cast<FUN_004cae90_t>(0x004cae90u);

// 0x004d7ff0 — error emit (write error code): void(unsigned int code)
// Cited in FUN_004c30b0 body, wrong-state + alloc-fail paths.
using FUN_004d7ff0_t = void (__cdecl*)(std::uint32_t);
static FUN_004d7ff0_t const s_FUN_004d7ff0 =
    reinterpret_cast<FUN_004d7ff0_t>(0x004d7ff0u);

// 0x004d8480 — error emit (flush/dispatch): void(void)
// Cited in FUN_004c30b0 body, after FUN_004d7ff0.
using FUN_004d8480_t = void (__cdecl*)();
static FUN_004d8480_t const s_FUN_004d8480 =
    reinterpret_cast<FUN_004d8480_t>(0x004d8480u);

// 0x004cc7e0 — RW code-path selector: void(int flag)
// Cited in FUN_004c32b0 body, called with bVar17 derived from param_2&1.
using FUN_004cc7e0_t = void (__cdecl*)(int);
static FUN_004cc7e0_t const s_FUN_004cc7e0 =
    reinterpret_cast<FUN_004cc7e0_t>(0x004cc7e0u);

// 0x004cce20 — RW init step: int(int param_1)
// Cited in FUN_004c32b0 body; if returns 0, calls FUN_0045b350 and returns 0.
using FUN_004cce20_t = int (__cdecl*)(int);
static FUN_004cce20_t const s_FUN_004cce20 =
    reinterpret_cast<FUN_004cce20_t>(0x004cce20u);

// 0x004ccf20 — RW cleanup (partial): void(void)
// Cited in FUN_004c32b0 failure paths.
using FUN_004ccf20_t = void (__cdecl*)();
static FUN_004ccf20_t const s_FUN_004ccf20 =
    reinterpret_cast<FUN_004ccf20_t>(0x004ccf20u);

// 0x0045b350 — subsystem abort: void(void)
// Cited in FUN_004c32b0 failure paths; also present in BootLowRvaCluster.
using FUN_0045b350_t = void (__cdecl*)();
static FUN_0045b350_t const s_FUN_0045b350 =
    reinterpret_cast<FUN_0045b350_t>(0x0045b350u);

// 0x004d7c60 — RW init step 2: int(void)
// Cited in FUN_004c32b0 body; if returns 0, calls FUN_004ccf20+FUN_0045b350.
using FUN_004d7c60_t = int (__cdecl*)();
static FUN_004d7c60_t const s_FUN_004d7c60 =
    reinterpret_cast<FUN_004d7c60_t>(0x004d7c60u);

// 0x004d7ca0 — RW cleanup step: void(void)
// Cited in FUN_004c32b0 failure path after FUN_004d7de0 OR-combine.
using FUN_004d7ca0_t = void (__cdecl*)();
static FUN_004d7ca0_t const s_FUN_004d7ca0 =
    reinterpret_cast<FUN_004d7ca0_t>(0x004d7ca0u);

// 0x004d7de0 — RW subsystem register: int(void* globals, int size, int id, const char* l1, const char* l2, int z)
// Cited in FUN_004c32b0; called 14 times with ids 0x401..0x40b, 0x40d, 0x40f, 0x412.
using FUN_004d7de0_t = int (__cdecl*)(void*, int, int, const char*, const char*, int);
static FUN_004d7de0_t const s_FUN_004d7de0 =
    reinterpret_cast<FUN_004d7de0_t>(0x004d7de0u);

// 0x004d8560 — RW init step 3: int(void)
// Cited in FUN_004c32b0 body; result OR-combined with FUN_004d7de0 results.
using FUN_004d8560_t = int (__cdecl*)();
static FUN_004d8560_t const s_FUN_004d8560 =
    reinterpret_cast<FUN_004d8560_t>(0x004d8560u);

// 0x004d8570 — RW pre-check: int(void)
// Cited in FUN_004c32b0 guard at 0x004c32dc; if returns 0, returns 0 immediately.
using FUN_004d8570_t = int (__cdecl*)();
static FUN_004d8570_t const s_FUN_004d8570 =
    reinterpret_cast<FUN_004d8570_t>(0x004d8570u);

// 0x004cfa00 (thunk) — final RW init call: int(void)
// Cited in FUN_004c32b0 body; if returns 0, failure path.
using thunk_FUN_004cfa00_t = int (__cdecl*)();
static thunk_FUN_004cfa00_t const s_thunk_FUN_004cfa00 =
    reinterpret_cast<thunk_FUN_004cfa00_t>(0x004cfa00u);

// 0x004987b0 — debug/log print: void(const char* msg)
// Cited in FUN_00496370 body with "Releasing _lpDIDKeyboard\n".
using FUN_004987b0_t = void (__cdecl*)(const char*);
static FUN_004987b0_t const s_FUN_004987b0 =
    reinterpret_cast<FUN_004987b0_t>(0x004987b0u);

// 0x004c7650 — heap/RW free helper: void(void* handle)
// Cited in FUN_00496970 body; releases/frees DAT_0077307c.
using FUN_004c7650_t = void (__cdecl*)(void*);
static FUN_004c7650_t const s_FUN_004c7650 =
    reinterpret_cast<FUN_004c7650_t>(0x004c7650u);


// ============================================================================
// 1. LangIndexCopy  —  0x004274d0
// ============================================================================
//
// Original: FUN_004274d0 (15 bytes, 0x004274d0–0x004274de).
// Decompiled body (verbatim from analysis note 004274d0.md):
//   MOV EAX, [0x007719e8]
//   MOV [0x007f0f60], EAX
//   MOV EAX, 1
//   RET
//
// Copies the command-line language index from DAT_007719e8 to the language
// selector global DAT_007f0f60, then returns 1 unconditionally.
// No branches, no callees.
//
// Constants cited (from analysis table):
//   0x004274d2  0x007719e8  — source: CLI language index (written by -l arg handler)
//   0x004274d8  0x007f0f60  — destination: language selector global
//   return val  0x00000001  — always returns 1
//
// Uncertainties:
//   [UNCERTAIN U-0867] Default value of DAT_007719e8 when no -l arg is present.
//                       Blocks=none.
//
// Caller: 0x00402750.
// Callees: none (pure leaf).

// 0x004274d0
extern "C" __declspec(dllexport) int __cdecl LangIndexCopy() {
    // 0x004274d2: read source language index from DAT_007719e8.
    std::uint32_t val = *reinterpret_cast<const std::uint32_t*>(0x007719e8u);
    // 0x004274d8: write to language selector global DAT_007f0f60.
    *reinterpret_cast<std::uint32_t*>(0x007f0f60u) = val;
    // return 1 unconditionally.
    return 1;
}

RH_ScopedInstall(LangIndexCopy, 0x004274d0);


// ============================================================================
// 2. LangFileLoad  —  0x004274e0
// ============================================================================
//
// Original: FUN_004274e0 (123 bytes, 0x004274e0–0x0042755a).
// Decompiled body (verbatim from analysis note 004274e0.md):
//   Reads DAT_007f0f60 as integer switch discriminant.
//   0 -> "english.dat", 1 -> "french.dat", 2 -> "German.dat",
//   3 -> "Spanish.dat", 4 -> "Italian.dat", 5 -> "USA.dat",
//   default -> return 0.
//   On match: FUN_004cc230(2, 1, filename_ptr) -> handle.
//   If handle != 0: FUN_004cbd30(handle, &DAT_0066d828, 0x10000);
//                   FUN_004cc160(handle, 0);
//                   DAT_0067d84c = 1; return 1.
//   If handle == 0 or default: return 0.
//
// Constants cited (from analysis table):
//   0x004274eb  0x007f0f60  — switch discriminant: language selector global
//   arg1 to FUN_004cc230: 0x00000002 (PIZ archive open type=2)
//   arg2 to FUN_004cc230: 0x00000001 (PIZ open mode=1, read)
//   read size arg: 0x00010000 (65536 bytes max)
//   0x0066d828              — localized-string buffer destination
//   0x0042753a 0x0067d84c   — success flag global (written 1)
//
// Uncertainties:
//   [UNCERTAIN U-0869] Case 5 "USA.dat": string in decomp but no data xref confirmed;
//                       not in MashedRunner language codes; file existence unknown.
//                       Blocks=none.
//
// Stubs:
//   S-0860 FUN_005507b0 (depth-2 via FUN_004cc230 type=2 branch)
//   S-0861 FUN_00550bc0 (depth-2 via FUN_004cc230 type=1 branch)
//   S-0862 FUN_00550910 (depth-2 via FUN_004cc160)
//
// Caller: 0x00402750.

// 0x004274e0
extern "C" __declspec(dllexport) int __cdecl LangFileLoad() {
    // 0x004274eb: read language selector index from DAT_007f0f60.
    int lang = *reinterpret_cast<const int*>(0x007f0f60u);

    // Switch on language index -> filename string literal.
    const char* filename = nullptr;
    switch (lang) {
        case 0: filename = "english.dat";  break;  // 0x004274f2 -> string at 0x005cd5e4
        case 1: filename = "french.dat";   break;  // 0x004274f9 -> string at 0x005cd5d8
        case 2: filename = "German.dat";   break;  // 0x00427500 -> string at 0x005cd5cc
        case 3: filename = "Spanish.dat";  break;  // 0x00427507 -> string at 0x005cd5c0
        case 4: filename = "Italian.dat";  break;  // 0x0042750e -> string at 0x005cd5b4
        case 5: filename = "USA.dat";      break;  // [UNCERTAIN U-0869] case 5; may not exist
        default: return 0;
    }

    // Call FUN_004cc230(2, 1, filename) -> file handle.
    int handle = s_FUN_004cc230(2, 1, filename);
    if (handle == 0) {
        return 0;
    }

    // FUN_004cbd30(handle, &DAT_0066d828, 0x10000) — read up to 65536 bytes.
    s_FUN_004cbd30(handle, reinterpret_cast<void*>(0x0066d828u), 0x10000);

    // FUN_004cc160(handle, 0) — close stream.
    s_FUN_004cc160(handle, 0);

    // 0x0042753a: DAT_0067d84c = 1 (success flag).
    *reinterpret_cast<std::uint32_t*>(0x0067d84cu) = 1u;

    return 1;
}

RH_ScopedInstall(LangFileLoad, 0x004274e0);


// ============================================================================
// 3. RwEngineOpen  —  0x004c30b0
// ============================================================================
//
// Original: FUN_004c30b0 (446 bytes, 0x004c30b0–0x004c326e).
// Decompiled body (verbatim from analysis note 004c30b0.md):
//
//   Null-guard at 0x004c30b0: if DAT_007d3ff8 == NULL, DAT_007d3ff8 = &DAT_007d3ec8.
//   State check at 0x004c30bc: if DAT_007d3ff8[0x49] != 1 -> error 0x80000001; return 0.
//   param_1 null-guard at 0x004c30c6: if param_1 == 0 -> error 0x80000016 (local_8=1); return 0.
//   iVar1 = FUN_004cae90() at 0x004c30d3; if 0 -> return 0.
//   puVar2 = vtable[0x42](DAT_00617fe0, 0x40000) alloc at 0x004c30e1; if NULL -> error 0x80000013; return 0.
//   Copy 0x4b dwords from &DAT_007d3ec8 to puVar2; DAT_007d3ff8 = puVar2.
//   FUN_004c2c90(iVar1, 4, DAT_007d3ff8+4, DAT_007d3ff8+0x42, 0) — cmd 4.
//   iVar3 = FUN_004c2c90(iVar1, 0, 0, param_1, 0) — cmd 0; if 0: rollback; return 0.
//   FUN_004c2c90(iVar1, 0xb, DAT_007d3ff8+0x12, 0, 0x1d) — cmd 0xb.
//   DAT_007d3ff4 += 1 (refcount).
//   DAT_007d3ff8[0x49] = 2 (state = opened).
//   return 1.
//
// Constants cited (from analysis table):
//   0x007d3ff8  — DAT_007d3ff8: active RwGlobals pointer
//   0x007d3ec8  — DAT_007d3ec8: static globals template (75 dwords = 300 bytes)
//   0x00000049  — index 0x49 (offset 0x124): state field
//   0x00000042  — vtable index [0x42]: alloc method
//   0x00617fe0  — DAT_00617fe0: first alloc arg
//   0x00040000  — alloc size hint (262144)
//   0x0000004b  — copy count (75 dwords)
//   0x007d3ff4  — DAT_007d3ff4: refcount
//   0x80000016  — null-param error ID
//   0x80000001  — wrong-state error ID
//   0x80000013  — alloc-fail error ID
//   0x007d3fd4  — free-function pointer (rollback path)
//
// Caller: 0x00493710.

// 0x004c30b0
extern "C" __declspec(dllexport) int __cdecl RwEngineOpen(int param_1) {
    // Null-guard at 0x004c30b0.
    std::uint32_t** ppGlobals = reinterpret_cast<std::uint32_t**>(0x007d3ff8u);
    if (*ppGlobals == nullptr) {
        *ppGlobals = reinterpret_cast<std::uint32_t*>(0x007d3ec8u);
    }

    // State check at 0x004c30bc: state field = (*ppGlobals)[0x49]; must be 1 (init-not-opened).
    std::uint32_t* pGlobals = *ppGlobals;
    if (pGlobals[0x49] != 1u) {
        // 0x80000001: wrong-state error.
        s_FUN_004d7ff0(0x80000001u);
        s_FUN_004d8480();
        return 0;
    }

    // param_1 null-guard at 0x004c30c6.
    if (param_1 == 0) {
        // 0x80000016: null-param error; local_8=1 passed implicitly via error emit.
        s_FUN_004d7ff0(0x80000016u);
        s_FUN_004d8480();
        return 0;
    }

    // 0x004c30d3: device open query.
    int iVar1 = s_FUN_004cae90();
    if (iVar1 == 0) {
        return 0;
    }

    // 0x004c30e1: alloc via vtable[0x42](DAT_00617fe0, 0x40000).
    // vtable is at (*ppGlobals)[0x42].
    using AllocFn_t = std::uint32_t* (__cdecl*)(void*, std::uint32_t);
    void* allocFnPtr = reinterpret_cast<void**>(pGlobals)[0x42];
    AllocFn_t allocFn = reinterpret_cast<AllocFn_t>(allocFnPtr);
    std::uint32_t* puVar2 = allocFn(reinterpret_cast<void*>(0x00617fe0u), 0x40000u);
    if (puVar2 == nullptr) {
        // 0x80000013: alloc-fail error.
        s_FUN_004d7ff0(0x80000013u);
        s_FUN_004d8480();
        return 0;
    }

    // 0x004c30f2: copy 0x4b dwords from &DAT_007d3ec8 to puVar2.
    const std::uint32_t* tmpl = reinterpret_cast<const std::uint32_t*>(0x007d3ec8u);
    for (int i = 0; i < 0x4b; ++i) {
        puVar2[i] = tmpl[i];
    }
    // DAT_007d3ff8 = puVar2.
    *ppGlobals = puVar2;
    pGlobals = puVar2;

    // FUN_004c2c90 call 1: cmd 4.
    s_FUN_004c2c90(iVar1, 4,
        reinterpret_cast<void*>(&pGlobals[4]),
        reinterpret_cast<void*>(&pGlobals[0x42]),
        0);

    // FUN_004c2c90 call 2: cmd 0; if returns 0 -> rollback.
    int iVar3 = s_FUN_004c2c90(iVar1, 0, nullptr, reinterpret_cast<void*>(param_1), 0);
    if (iVar3 == 0) {
        // Rollback: copy puVar2 back to &DAT_007d3ec8; free puVar2.
        std::uint32_t* staticTmpl = reinterpret_cast<std::uint32_t*>(0x007d3ec8u);
        for (int i = 0; i < 0x4b; ++i) {
            staticTmpl[i] = puVar2[i];
        }
        using FreeFn_t = void (__cdecl*)(void*);
        FreeFn_t freeFn = *reinterpret_cast<FreeFn_t*>(0x007d3fd4u);
        freeFn(puVar2);
        return 0;
    }

    // FUN_004c2c90 call 3: cmd 0xb.
    s_FUN_004c2c90(iVar1, 0xb,
        reinterpret_cast<void*>(&pGlobals[0x12]),
        nullptr,
        0x1d);

    // 0x004c3126: DAT_007d3ff4 += 1 (refcount).
    *reinterpret_cast<std::uint32_t*>(0x007d3ff4u) += 1u;

    // 0x004c312c: state = opened (2).
    pGlobals[0x49] = 2u;

    return 1;
}

RH_ScopedInstall(RwEngineOpen, 0x004c30b0);


// ============================================================================
// 4. RwEngineInit  —  0x004c32b0
// ============================================================================
//
// Original: FUN_004c32b0 (767 bytes, 0x004c32b0–0x004c35af).
// Decompiled body (verbatim from analysis note 004c32b0.md):
//
//   Entry: DAT_007d3ff8 = &DAT_007d3ec8 (unconditional).
//   param_2 & 1 check:
//     If bit 0 clear (bVar17=true):  _DAT_007d3fe0=&LAB_004cca80; _DAT_007d3fe4=FUN_004ccba0.
//     If bit 0 set  (bVar17=false):  _DAT_007d3fe0=&LAB_004c35b0; _DAT_007d3fe4=&LAB_004c35d0.
//   FUN_004cc7e0(bVar17).
//   DAT_007d3ff8[0x4a] = param_3.
//   Guard: if DAT_007d3ff8[0x49] != 0 OR FUN_004d8570()==0: return 0.
//   FUN_004cce20(param_1) -> iVar1; if 0: FUN_0045b350(); return 0.
//   FUN_004d7c60() -> iVar1; if 0: FUN_004ccf20()+FUN_0045b350(); return 0.
//   14× FUN_004d7de0(&DAT_00617fe0, size, id, label1, label2, 0):
//     ids: 0x40f(sz=8), 0x401(sz=0x18), 0x40d(sz=0), 0x402(sz=0x18),
//          0x403(sz=4), 0x404(sz=4), 0x405(sz=4), 0x406(sz=0x220),
//          0x407(sz=100), 0x408(sz=0x34), 0x409(sz=0x60), 0x40a(sz=4),
//          0x40b(sz=0x74), 0x412(sz=0x28).
//   FUN_004d8560() -> uVar14.
//   OR-combine all 15 results; if any negative: FUN_004d7ca0+FUN_004ccf20+FUN_0045b350; return 0.
//   thunk_FUN_004cfa00() -> iVar1; if 0: failure path; return 0.
//   DAT_007d3ff8[0x49] = 1; return iVar1.
//
// Constants cited (from analysis table):
//   0x007d3ff8 / 0x007d3ec8  — engine globals pointer / static template
//   0x00000049               — index 0x49 = offset 0x124: state field
//   0x0000004a               — index 0x4a = offset 0x128: param_3 store slot
//   0x00617fe0               — DAT_00617fe0 — first arg to all FUN_004d7de0 calls
//   0x007d3fe0 / 0x007d3fe4  — code-path pointers A/B
//
// Uncertainties:
//   [UNCERTAIN U-0071] Three-parameter signature vs two-arg call site; possible Ghidra
//                       misread. Pre-existing; non-blocking for C2.
//
// Caller: 0x00493710.

// 0x004c32b0
extern "C" __declspec(dllexport) int __cdecl RwEngineInit(int param_1, unsigned int param_2, int param_3) {
    // Entry: unconditionally set DAT_007d3ff8 = &DAT_007d3ec8.
    std::uint32_t** ppGlobals = reinterpret_cast<std::uint32_t**>(0x007d3ff8u);
    *ppGlobals = reinterpret_cast<std::uint32_t*>(0x007d3ec8u);

    // param_2 & 1 check: bit 0 set -> bVar17=false (inverted sense in decomp).
    int bVar17 = ((param_2 & 1u) == 0u) ? 1 : 0;
    if (bVar17) {
        // bit 0 clear: _DAT_007d3fe0 = &LAB_004cca80; _DAT_007d3fe4 = FUN_004ccba0.
        *reinterpret_cast<void**>(0x007d3fe0u) = reinterpret_cast<void*>(0x004cca80u);
        *reinterpret_cast<void**>(0x007d3fe4u) = reinterpret_cast<void*>(0x004ccba0u);
    } else {
        // bit 0 set: _DAT_007d3fe0 = &LAB_004c35b0; _DAT_007d3fe4 = &LAB_004c35d0.
        *reinterpret_cast<void**>(0x007d3fe0u) = reinterpret_cast<void*>(0x004c35b0u);
        *reinterpret_cast<void**>(0x007d3fe4u) = reinterpret_cast<void*>(0x004c35d0u);
    }

    // FUN_004cc7e0(bVar17) at 0x004c32ce.
    s_FUN_004cc7e0(bVar17);

    // 0x004c32d8: DAT_007d3ff8[0x4a] = param_3.
    (*ppGlobals)[0x4a] = static_cast<std::uint32_t>(param_3);

    // Guard at 0x004c32dc.
    std::uint32_t* pGlobals = *ppGlobals;
    if (pGlobals[0x49] != 0u || s_FUN_004d8570() == 0) {
        return 0;
    }

    // FUN_004cce20(param_1).
    int iVar1 = s_FUN_004cce20(param_1);
    if (iVar1 == 0) {
        s_FUN_0045b350();
        return 0;
    }

    // FUN_004d7c60().
    iVar1 = s_FUN_004d7c60();
    if (iVar1 == 0) {
        s_FUN_004ccf20();
        s_FUN_0045b350();
        return 0;
    }

    // 14 FUN_004d7de0 calls with ids 0x401..0x40b, 0x40d, 0x40f, 0x412.
    // DAT_00617fe0 is first arg to all calls; label args are nullptr here (null labels in decomp).
    void* pBase = reinterpret_cast<void*>(0x00617fe0u);
    int r01 = s_FUN_004d7de0(pBase, 0x08,   0x40f, nullptr, nullptr, 0);
    int r02 = s_FUN_004d7de0(pBase, 0x18,   0x401, nullptr, nullptr, 0);
    int r03 = s_FUN_004d7de0(pBase, 0x00,   0x40d, nullptr, nullptr, 0);
    int r04 = s_FUN_004d7de0(pBase, 0x18,   0x402, nullptr, nullptr, 0);
    int r05 = s_FUN_004d7de0(pBase, 0x04,   0x403, nullptr, nullptr, 0);
    int r06 = s_FUN_004d7de0(pBase, 0x04,   0x404, nullptr, nullptr, 0);
    int r07 = s_FUN_004d7de0(pBase, 0x04,   0x405, nullptr, nullptr, 0);
    int r08 = s_FUN_004d7de0(pBase, 0x220,  0x406, nullptr, nullptr, 0);
    int r09 = s_FUN_004d7de0(pBase, 100,    0x407, nullptr, nullptr, 0);
    int r10 = s_FUN_004d7de0(pBase, 0x34,   0x408, nullptr, nullptr, 0);
    int r11 = s_FUN_004d7de0(pBase, 0x60,   0x409, nullptr, nullptr, 0);
    int r12 = s_FUN_004d7de0(pBase, 0x04,   0x40a, nullptr, nullptr, 0);
    int r13 = s_FUN_004d7de0(pBase, 0x74,   0x40b, nullptr, nullptr, 0);
    int r14 = s_FUN_004d7de0(pBase, 0x28,   0x412, nullptr, nullptr, 0);

    // FUN_004d8560() -> uVar14 (15th result).
    unsigned int uVar14 = static_cast<unsigned int>(s_FUN_004d8560());

    // OR-combine all 15 results; if any has high bit set (negative) -> failure.
    int combined = r01 | r02 | r03 | r04 | r05 | r06 | r07 | r08 |
                   r09 | r10 | r11 | r12 | r13 | r14 | static_cast<int>(uVar14);
    if (combined < 0) {
        s_FUN_004d7ca0();
        s_FUN_004ccf20();
        s_FUN_0045b350();
        return 0;
    }

    // thunk_FUN_004cfa00().
    iVar1 = s_thunk_FUN_004cfa00();
    if (iVar1 == 0) {
        s_FUN_004d7ca0();
        s_FUN_004ccf20();
        s_FUN_0045b350();
        return 0;
    }

    // On success: DAT_007d3ff8[0x49] = 1 (init state).
    (*ppGlobals)[0x49] = 1u;

    return iVar1;
}

RH_ScopedInstall(RwEngineInit, 0x004c32b0);


// ============================================================================
// 5. DInputKeyboardRelease  —  0x00496370
// ============================================================================
//
// Original: FUN_00496370 (54 bytes, 0x00496370–0x004963a5).
// Decompiled body (verbatim from analysis note 00496370.md):
//   (**(code **)(*DAT_00772fb8 + 0x20))(DAT_00772fb8) — vtable[0x20] Unacquire (no null check).
//   if (DAT_00772fb8 != NULL):
//     FUN_004987b0("Releasing _lpDIDKeyboard\n")
//     (**(code **)(*DAT_00772fb8 + 8))(DAT_00772fb8) — vtable[0x08] COM Release.
//     DAT_00772fb8 = NULL
//   return void.
//
// Constants cited (from analysis table):
//   ~0x00496372  0x00772fb8  — DAT_00772fb8: DirectInput keyboard device pointer global
//   ~0x00496374  0x20        — vtable offset for Unacquire
//   ~0x0049638a  0x08        — vtable offset for COM Release
//
// Caller: 0x00493560 (HardwareExit).

// 0x00496370
extern "C" __declspec(dllexport) void __cdecl DInputKeyboardRelease() {
    // ~0x00496372: DAT_00772fb8 = IDirectInputDevice COM pointer.
    void** ppDevice = reinterpret_cast<void**>(0x00772fb8u);
    void* pDevice = *ppDevice;

    // vtable[0x20] = Unacquire; called unconditionally (no null check before).
    // vtable is at *pDevice (double-indirect).
    using VtableFn_t = void (__cdecl*)(void*);
    void** vtable = *reinterpret_cast<void***>(pDevice);
    VtableFn_t unacquire = reinterpret_cast<VtableFn_t>(vtable[0x20 / sizeof(void*)]);
    unacquire(pDevice);

    // Null check before log + Release.
    if (pDevice != nullptr) {
        // FUN_004987b0("Releasing _lpDIDKeyboard\n").
        s_FUN_004987b0("Releasing _lpDIDKeyboard\n");

        // vtable[0x08] = COM Release.
        VtableFn_t comRelease = reinterpret_cast<VtableFn_t>(vtable[0x08 / sizeof(void*)]);
        comRelease(pDevice);

        // DAT_00772fb8 = NULL.
        *ppDevice = nullptr;
    }
}

RH_ScopedInstall(DInputKeyboardRelease, 0x00496370);


// ============================================================================
// 6. HandleRelease  —  0x00496970
// ============================================================================
//
// Original: FUN_00496970 (33 bytes, 0x00496970–0x00496990).
// Decompiled body (verbatim from analysis note 00496970.md):
//   Reads DAT_0077307c; if == 0: skip body.
//   If != 0:
//     FUN_004c7650(DAT_0077307c)
//     DAT_0077307c = 0
//   return 1 (undefined4).
//
// Constants cited (from analysis table):
//   ~0x00496972  0x0077307c  — handle/resource pointer global; freed and zeroed
//
// Caller: 0x00402a40 (AppDestroy).

// 0x00496970
extern "C" __declspec(dllexport) int __cdecl HandleRelease() {
    // ~0x00496972: read DAT_0077307c.
    void** ppHandle = reinterpret_cast<void**>(0x0077307cu);
    void* pHandle = *ppHandle;

    if (pHandle != nullptr) {
        // FUN_004c7650(DAT_0077307c) — releases/frees the handle.
        s_FUN_004c7650(pHandle);
        // Write 0 to DAT_0077307c.
        *ppHandle = nullptr;
    }

    return 1;
}

RH_ScopedInstall(HandleRelease, 0x00496970);


// ============================================================================
// 7. RenderStatsAccumulate  —  0x00492440
// ============================================================================
//
// Original: FUN_00492440 (124 bytes, 0x00492440–0x004924bb).
// Decompiled body (verbatim from analysis note 00492440.md):
//   param_1 = pointer to stats record; accesses via relative offsets.
//   iVar1 = *(param_1 + 0x20) (A-value sample)
//   iVar2 = *(param_1 + 0x24) (B-value sample)
//   *(param_1 + 0x2c) += iVar1  (A running sum)
//   *(param_1 + 0x30) += iVar2  (B running sum)
//   if (*(param_1 + 0x28) > 0x3b):  [i.e., frame count > 59]
//     *(param_1 + 0x34) = *(param_1 + 0x2c) / 0x3c  (A avg)
//     *(param_1 + 0x2c) = 0
//     *(param_1 + 0x38) = *(param_1 + 0x30) / 0x3c  (B avg)
//     *(param_1 + 0x30) = 0
//     *(param_1 + 0x28) = 0
//   *(param_1 + 0x28) += 1  (frame counter increment, unconditional)
//   if (*(param_1 + 0x3c) < iVar1): *(param_1 + 0x3c) = iVar1  (A max)
//   if (*(param_1 + 0x40) < iVar2): *(param_1 + 0x40) = iVar2  (B max)
//
// Constants cited (from analysis table):
//   ~0x00492466  0x3b (59)  — threshold: >59 frames triggers 60-frame rollup
//   ~0x00492480  0x3c (60)  — divisor: rolling average over 60 frames
//   stores       0x00000000 — reset value for accumulators and counter
//
// Caller: 0x00492e90 (FrameDispatch).
// Pure leaf function — no callees.

// 0x00492440
extern "C" __declspec(dllexport) void __cdecl RenderStatsAccumulate(int param_1) {
    // param_1 is a pointer to the stats record base.
    int* rec = reinterpret_cast<int*>(param_1);

    // iVar1 = A-value sample at offset 0x20 (= 8 ints).
    int iVar1 = rec[0x20 / 4];   // offset 0x20
    // iVar2 = B-value sample at offset 0x24.
    int iVar2 = rec[0x24 / 4];   // offset 0x24

    // A running sum += iVar1.
    rec[0x2c / 4] += iVar1;      // offset 0x2c
    // B running sum += iVar2.
    rec[0x30 / 4] += iVar2;      // offset 0x30

    // 60-frame rollup: if frame counter (offset 0x28) > 59 (0x3b).
    if (rec[0x28 / 4] > 0x3b) {  // ~0x00492466
        // A avg = A_sum / 60.
        rec[0x34 / 4] = rec[0x2c / 4] / 0x3c;   // ~0x00492480
        rec[0x2c / 4] = 0;
        // B avg = B_sum / 60.
        rec[0x38 / 4] = rec[0x30 / 4] / 0x3c;
        rec[0x30 / 4] = 0;
        // Reset frame counter.
        rec[0x28 / 4] = 0;
    }

    // Increment frame counter unconditionally.
    rec[0x28 / 4] += 1;

    // A-value maximum tracker at offset 0x3c.
    if (rec[0x3c / 4] < iVar1) {
        rec[0x3c / 4] = iVar1;
    }
    // B-value maximum tracker at offset 0x40.
    if (rec[0x40 / 4] < iVar2) {
        rec[0x40 / 4] = iVar2;
    }
}

RH_ScopedInstall(RenderStatsAccumulate, 0x00492440);


// ============================================================================
// 8. RacePairIndexFSM  —  0x004927c0
// ============================================================================
//
// Original: FUN_004927c0 (499 bytes, 0x004927c0–0x004929b2).
// Decompiled body (verbatim from analysis note 004927c0.md):
//   Stack local: local_30[12] = {0,2,1,3,0,3,1,2,0,1,0,1} (pair-index mapping table).
//   Reads DAT_00771980 (global signed int, FSM state index).
//   If < 0 or > 11: clamps to 0.
//   iVar3 = 0 (progression flag: 0=idle, 1=advance, 2=reset).
//   8 gate-pair globals checked: if (DAT_A != '\0') && (DAT_B == '\0') -> iVar3 = 2.
//   Dispatches on local_30[DAT_00771980] (maps state to case 0..3):
//     Case 0: reads 007f159e/007f10de; tests 007f1046/007f1506 pair -> if cond: iVar3=1.
//     Case 1: reads 007f159f/007f10df; tests 007f1047/007f1507 pair -> iVar3=1.
//     Case 2: reads 007f159d/007f10dd; tests 007f1045/007f1505 pair -> iVar3=1.
//     Case 3: reads 007f159c/007f10dc; tests 007f1044/007f1504 pair -> iVar3=1.
//     Default: falls to reset/idle check.
//   After switch: if cVar2=='\0' OR cVar1!='\0' -> default path.
//   Default: if iVar3==2 -> DAT_00771980=0; return. If iVar3!=1 -> fall to LAB_00492977.
//   Advance: DAT_00771980 += 1.
//   LAB_00492977: if DAT_00771980 == 12:
//     Write 2 to 0x9c (156) consecutive uint32 at DAT_007f0a40 (624-byte block).
//     DAT_007f0f2c = 1.
//
// Constants cited (from analysis table):
//   FSM lower clamp  0x00 (0)
//   FSM upper clamp  0x0b (11)
//   FSM terminal     0x0c (12)
//   block-fill count 0x9c (156)
//   block-fill value 0x00000002 (2)
//
// Uncertainties:
//   [UNCERTAIN U-3921] Terminal clear: semantic meaning of write-2 to 156 × uint32
//                       at DAT_007f0a40..0x007f0d0f not derivable from decomp alone.
//                       Blocks=none.
//
// Caller: 0x004929d0 (StateDispatch).
// Pure leaf function — no callees.

// 0x004927c0
extern "C" __declspec(dllexport) void __cdecl RacePairIndexFSM() {
    // Pair-index mapping table (0-based, 12 entries).
    // Values from analysis note: {0,2,1,3,0,3,1,2,0,1,0,1}.
    int local_30[12] = { 0, 2, 1, 3, 0, 3, 1, 2, 0, 1, 0, 1 };

    // Read FSM state index from DAT_00771980.
    int* pFsmState = reinterpret_cast<int*>(0x00771980u);
    int fsmState = *pFsmState;

    // Clamp: if < 0 or > 11, clamp to 0.
    if (fsmState < 0 || fsmState > 0x0b) {
        fsmState = 0;
    }

    // Progression flag: 0=idle, 1=advance, 2=reset.
    int iVar3 = 0;

    // 8 gate-pair checks: if (A != 0 && B == 0) -> iVar3 = 2.
    // Gate A addresses: 007f1046, 007f10de, 007f1047, 007f10df,
    //                   007f1044, 007f10dc, 007f1045, 007f10dd.
    // Gate B addresses: 007f1506, 007f159e, 007f1507, 007f159f,
    //                   007f1504, 007f159c, 007f1505, 007f159d.
    static const uintptr_t gateA[8] = {
        0x007f1046u, 0x007f10deu, 0x007f1047u, 0x007f10dfu,
        0x007f1044u, 0x007f10dcu, 0x007f1045u, 0x007f10ddu
    };
    static const uintptr_t gateB[8] = {
        0x007f1506u, 0x007f159eu, 0x007f1507u, 0x007f159fu,
        0x007f1504u, 0x007f159cu, 0x007f1505u, 0x007f159du
    };
    for (int i = 0; i < 8; ++i) {
        char a = *reinterpret_cast<const char*>(gateA[i]);
        char b = *reinterpret_cast<const char*>(gateB[i]);
        if (a != '\0' && b == '\0') {
            iVar3 = 2;
        }
    }

    // Dispatch on pair-index mapped from FSM state.
    int mappedCase = local_30[fsmState];

    char cVar1 = '\0';  // "complete" flag from state case
    char cVar2 = '\0';  // "ready" flag from state case

    switch (mappedCase) {
        case 0:
            cVar2 = *reinterpret_cast<const char*>(0x007f159eu);
            cVar1 = *reinterpret_cast<const char*>(0x007f10deu);
            // Test pair 007f1046 / 007f1506.
            if (*reinterpret_cast<const char*>(0x007f1046u) != '\0' &&
                *reinterpret_cast<const char*>(0x007f1506u) != '\0') {
                iVar3 = 1;
            }
            break;
        case 1:
            cVar2 = *reinterpret_cast<const char*>(0x007f159fu);
            cVar1 = *reinterpret_cast<const char*>(0x007f10dfu);
            // Test pair 007f1047 / 007f1507.
            if (*reinterpret_cast<const char*>(0x007f1047u) != '\0' &&
                *reinterpret_cast<const char*>(0x007f1507u) != '\0') {
                iVar3 = 1;
            }
            break;
        case 2:
            cVar2 = *reinterpret_cast<const char*>(0x007f159du);
            cVar1 = *reinterpret_cast<const char*>(0x007f10ddu);
            // Test pair 007f1045 / 007f1505.
            if (*reinterpret_cast<const char*>(0x007f1045u) != '\0' &&
                *reinterpret_cast<const char*>(0x007f1505u) != '\0') {
                iVar3 = 1;
            }
            break;
        case 3:
            cVar2 = *reinterpret_cast<const char*>(0x007f159cu);
            cVar1 = *reinterpret_cast<const char*>(0x007f10dcu);
            // Test pair 007f1044 / 007f1504.
            if (*reinterpret_cast<const char*>(0x007f1044u) != '\0' &&
                *reinterpret_cast<const char*>(0x007f1504u) != '\0') {
                iVar3 = 1;
            }
            break;
        default:
            // Default: fall to reset/idle check path.
            goto default_path;
    }

    // After switch: if cVar2 == '\0' OR cVar1 != '\0' -> default path.
    if (cVar2 == '\0' || cVar1 != '\0') {
        goto default_path;
    }

    // Advance path: iVar3==1 already set above for matching case.
    if (iVar3 == 1) {
        *pFsmState = fsmState + 1;
    }
    goto terminal_check;

default_path:
    if (iVar3 == 2) {
        // Reset: DAT_00771980 = 0.
        *pFsmState = 0;
        return;
    }
    if (iVar3 != 1) {
        goto terminal_check;
    }
    // iVar3==1 on default path: advance.
    *pFsmState = fsmState + 1;

terminal_check:
    // LAB_00492977: if FSM state == 12 (all pairs exhausted).
    if (*pFsmState == 0x0c) {
        // Write 2 to 156 consecutive uint32 slots at DAT_007f0a40.
        // [UNCERTAIN U-3921] Semantic meaning unknown; block fill value=2, count=156 (624 bytes).
        std::uint32_t* pBlock = reinterpret_cast<std::uint32_t*>(0x007f0a40u);
        for (int i = 0; i < 0x9c; ++i) {
            pBlock[i] = 2u;
        }
        // DAT_007f0f2c = 1.
        *reinterpret_cast<std::uint32_t*>(0x007f0f2cu) = 1u;
    }
}

RH_ScopedInstall(RacePairIndexFSM, 0x004927c0);
