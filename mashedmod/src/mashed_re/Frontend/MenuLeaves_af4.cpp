// Mashed RE — Frontend menu-leaf reimplementations (c3-batch-af session 4).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file (HARVEST author+verify):
//   0x0043aee0  MenuSlotFlagSetCurrent  — gated write of 1 to the current-slot flag
//   0x00401ee0  EntityTableSelectUpdate — table-select + RW matrix update glue
//   0x0040d590  RaceRankThreePlayers    — three-way position ranking, score deltas
//   0x00425c00  CopterClumpRegister     — copter clump asset registration
//   0x00428bf0  TitleBuildInfoRender    — MOVED to Match/MatchS06_00428bf0.cpp (MATCH-S06)
//
// Analysis notes:
//   re/analysis/bucket_0041dc30/0x0043aee0.md
//   re/analysis/frontend_c1_to_c2_s1/00401ee0.md
//   re/analysis/frontend_c1_to_c2_s1/0040d590.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00425c00.md
//   re/analysis/frontend_c1_to_c2_s5/FUN_00428bf0.md
//
// Every reimplementation is a verbatim transcription of the Ghidra
// decompilation (master Mashed.gpr, read-only) and matches the original
// __cdecl frames so the inline-JMP redirect lines up. The MSVC /GS stack
// cookie that wraps the two buffer-using functions (0x00425c00, 0x00428bf0)
// is a compiler artifact — the compiler emits its own; it has no observable
// effect, so it is not hand-replicated.

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees (RVAs into MASHED.exe)
// ---------------------------------------------------------------------------

// 0x004307a0  FUN_004307a0 — returns int used as a boolean gate.
static auto* const s_F004307a0 =
    reinterpret_cast<std::int32_t(__cdecl*)(void)>(0x004307a0);

// 0x00401570  FUN_00401570 — table scan; selects an entry pointer into the
//             global at 0x00636ac0. Return value unused by the caller.
static auto* const s_F00401570 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x00401570);

// 0x00401da0  FUN_00401da0 — RenderWare matrix update for the selected entry.
static auto* const s_F00401da0 =
    reinterpret_cast<void(__cdecl*)(void)>(0x00401da0);

// 0x004e6680  FUN_004e6680 — consumes the dereferenced selected-entry value.
static auto* const s_F004e6680 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x004e6680);

// 0x00408ad0  FUN_00408ad0 — car-position getter: returns the float at
//             (&DAT_008a96ec + idx*0x30c). Returned in ST0 (32-bit value).
static auto* const s_F00408ad0 =
    reinterpret_cast<float(__cdecl*)(int)>(0x00408ad0);

// 0x00422fd0  FUN_00422fd0 — per-rank pre-assignment helper (return unused).
static auto* const s_F00422fd0 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x00422fd0);

// 0x0040b290  FUN_0040b290 — applies a signed score delta to a player slot.
static auto* const s_F0040b290 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, int)>(0x0040b290);

// 0x004c5c80  FUN_004c5c80 — render-context push/pop (entry: nonzero, exit: 0).
static auto* const s_F004c5c80 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x004c5c80);

// 0x004781b0  FUN_004781b0 — asset loader (path, int); returns RpClump handle.
static auto* const s_F004781b0 =
    reinterpret_cast<std::uint32_t(__cdecl*)(const char*, int)>(0x004781b0);

// 0x00425b90  FUN_00425b90 — RpClumpForAllAtomics dispatch wrapper.
static auto* const s_F00425b90 =
    reinterpret_cast<void(__cdecl*)(void*)>(0x00425b90);

// 0x004a2b60  FUN_004a2b60 — snprintf-style formatter (dst, fmt, ...).
static auto* const s_F004a2b60 =
    reinterpret_cast<int(__cdecl*)(char*, const char*, ...)>(0x004a2b60);

// ---------------------------------------------------------------------------
// Global addresses (cited from the decompilation)
// ---------------------------------------------------------------------------

// 0x0067ed6c  DAT_0067ed6c — override flag (second gate of MenuSlotFlagSetCurrent).
static const std::uintptr_t k_OverrideFlag = 0x0067ed6cu;
// 0x0067f17c  DAT_0067f17c — current-slot (primary-player / card) index.
static const std::uintptr_t k_CurrentSlot  = 0x0067f17cu;
// 0x007f0a40  DAT_007f0a40 — per-slot int flag array, stride 0xc ints.
static const std::uintptr_t k_SlotFlagBase = 0x007f0a40u;

// 0x00636ac0  DAT_00636ac0 — pointer to the selected table entry.
static const std::uintptr_t k_SelectedEntryPtr = 0x00636ac0u;

// Position wrap constants (cited at 0x0040d5c4/cc/d8; values read from .rdata).
static const std::uintptr_t k_WrapLo  = 0x005cc730u; // float 80.0
static const std::uintptr_t k_WrapHi  = 0x005ccd6cu; // float 20.0
static const std::uintptr_t k_WrapOff = 0x005cc568u; // float 100.0

// 0x00644150  DAT_00644150 — render-context handle for FUN_004c5c80 entry.
static const std::uintptr_t k_RenderCtx = 0x00644150u;
// 0x00899260  DAT_00899260 — copter slot array; stride 0x13 ints; end 0x8994c0.
static const std::uintptr_t k_CopterArrayBase = 0x00899260u;
static const std::uintptr_t k_CopterArrayEnd  = 0x008994c0u;
static const int            k_CopterStride    = 0x13;   // ints per slot

// ---------------------------------------------------------------------------
// MenuSlotFlagSetCurrent  --  0x0043aee0
//
// Original: FUN_0043aee0 (0x0043aee0..0x0043af06)
// Signature: void FUN_0043aee0(void)
//
// Verbatim decompilation:
//   iVar1 = FUN_004307a0();
//   if ((iVar1 != 0) && (DAT_0067ed6c != 0)) {
//     (&DAT_007f0a40)[DAT_0067f17c * 0xc] = 1;
//   }
//
// Disassembly (cited): LEA EAX,[EAX+EAX*2]; SHL EAX,4  ->  EAX*0x30 bytes, i.e.
// slot index * 0xc ints. MOV dword ptr [EAX + 0x7f0a40],1.
//
// ref: re/analysis/bucket_0041dc30/0x0043aee0.md
// ---------------------------------------------------------------------------

// 0x0043aee0
extern "C" __declspec(dllexport) void __cdecl MenuSlotFlagSetCurrent(void)
{
    if (s_F004307a0() != 0 &&
        *reinterpret_cast<std::int32_t*>(k_OverrideFlag) != 0) {
        std::int32_t slot = *reinterpret_cast<std::int32_t*>(k_CurrentSlot);
        // (&DAT_007f0a40)[slot * 0xc] — int array, stride 0xc ints. [0x0043aef7]
        reinterpret_cast<std::int32_t*>(k_SlotFlagBase)[slot * 0xc] = 1;
    }
}

RH_ScopedInstall(MenuSlotFlagSetCurrent, 0x0043aee0);

// ---------------------------------------------------------------------------
// EntityTableSelectUpdate  --  0x00401ee0
//
// Original: FUN_00401ee0 (0x00401ee0..0x00401f06)
// Signature: void FUN_00401ee0(int param_1)
//
// Verbatim decompilation:
//   FUN_00401570(param_1);
//   FUN_00401da0();
//   if (*DAT_00636ac0 != 0) {
//     FUN_004e6680();          // tail-call; arg slot rewritten to the deref'd value
//     return;
//   }
//
// Disassembly (cited 0x00401ef2..0x00401f02): MOV ECX,[0x636ac0]; MOV EAX,[ECX];
// TEST EAX,EAX; JZ ret; MOV [ESP+4],EAX; JMP 0x4e6680. The original tail-jumps
// into FUN_004e6680 after overwriting its own param slot with EAX, so the callee
// receives EAX (= *(*(int**)0x636ac0)) as param_1. A normal __cdecl call with the
// same argument is behaviorally identical (caller cleans the stack either way).
//
// ref: re/analysis/frontend_c1_to_c2_s1/00401ee0.md
// ---------------------------------------------------------------------------

// [moved to Match/MatchS01.cpp: C4-by-match port body] 0x00401ee0 EntityTableSelectUpdate


// ---------------------------------------------------------------------------
// RaceRankThreePlayers  --  0x0040d590
//
// Original: FUN_0040d590 (0x0040d590..0x0040d8e0)
// Signature: void FUN_0040d590(int p1, int p2, int p3, int delta)
//   p1,p2,p3: player indices; delta: score-delta magnitude.
//
// Mechanism (verbatim transcription): three-way comparison of track positions
// returned by FUN_00408ad0(idx). Each pairwise comparison applies a wrap-around
// correction: positions near the lap seam (0x005cc730 = 80.0 lower bound,
// 0x005ccd6c = 20.0 upper bound) are shifted by 0x005cc568 = 100.0 so the
// comparison reflects true race order. Winner gets -delta, runner-up +delta,
// last +delta*2 (FUN_0040b290 applies the delta; FUN_00422fd0 is the per-rank
// pre-assignment helper called immediately before).
//
// The decompiler truncates the first operand of each compare to 32-bit float
// ((float10)(float)) while the second stays at the getter's 32-bit return; both
// are exact 32-bit values, so plain float arithmetic is bit-identical.
//
// Constants cited:
//   0x005cc730 — float 80.0  (wrap lower bound)        [0x0040d5c4]
//   0x005ccd6c — float 20.0  (wrap upper bound)        [0x0040d5cc]
//   0x005cc568 — float 100.0 (wrap subtraction offset) [0x0040d5d8]
//
// ref: re/analysis/frontend_c1_to_c2_s1/0040d590.md
// ---------------------------------------------------------------------------

// Inlined wrap-correction block (appears 6 times in the original). Mutates the
// pair in place exactly as the decompilation does before each comparison.
// [u9779] The bounds are literals now: in the standalone exe the .rdata addresses
// k_WrapLo / k_WrapHi / k_WrapOff read 0 (the RwMatrixRotate.cpp:72 class of defect).
// 80.0 / 20.0 / 100.0 are exact in binary32, so the .asi result is unchanged.
static inline void WrapAdjust(float& fa, float& fb)
{
    const float lo  = 80.0f;    // [0x005cc730] (k_WrapLo)
    const float hi  = 20.0f;    // [0x005ccd6c] (k_WrapHi)
    const float off = 100.0f;   // [0x005cc568] (k_WrapOff)
    (void)k_WrapLo; (void)k_WrapHi; (void)k_WrapOff;   // retained for provenance
    if ((fa <= lo) || (hi <= fb)) {
        if ((lo < fb) && (fa < hi)) {
            fb = fb - off;
        }
    } else {
        fa = fa - off;
    }
}

namespace mashed_re { namespace Frontend {
// [u9779] The one body of FUN_0040d590 for both targets. The callees are passed in:
// the .asi export below hands it the original's FUN_00408ad0 / FUN_00422fd0 /
// FUN_0040b290, the standalone exe's TrackRenderer::Resolve3_40d590 hands it its own
// race_pct read / KillCar / ScoreAward (MASHED_ELIM_RESOLVE, verify/u9779_20261021).
struct Rank3Callees {
    float (*pct)(int);                  // FUN_00408ad0
    void  (*kill)(std::uint32_t);       // FUN_00422fd0
    void  (*add)(std::uint32_t, int);   // FUN_0040b290
};
// 0x0040d590
void RaceRankThree(std::uint32_t param_1, std::uint32_t param_2, std::uint32_t param_3, int param_4,
                   const Rank3Callees& c)
{
    float fVar1, fVar2;

    fVar1 = c.pct(static_cast<int>(param_1));
    fVar2 = c.pct(static_cast<int>(param_2));
    WrapAdjust(fVar1, fVar2);
    if (fVar1 <= fVar2) {
        fVar1 = c.pct(static_cast<int>(param_3));
        fVar2 = c.pct(static_cast<int>(param_1));
        WrapAdjust(fVar1, fVar2);
        if (fVar2 < fVar1) {
            c.kill(param_1);
            c.add(param_1, -param_4);
            fVar1 = c.pct(static_cast<int>(param_2));
            fVar2 = c.pct(static_cast<int>(param_3));
            WrapAdjust(fVar1, fVar2);
            if (fVar2 < fVar1) {
                c.kill(param_3);
                c.add(param_3, param_4);
                c.add(param_2, param_4 * 2);
                return;
            }
            c.kill(param_2);
            c.add(param_2, param_4);
            c.add(param_3, param_4 * 2);
            return;
        }
        c.kill(param_3);
        c.add(param_3, -param_4);
        c.kill(param_1);
        c.add(param_1, param_4);
        c.add(param_2, param_4 * 2);
        return;
    }
    fVar1 = c.pct(static_cast<int>(param_3));
    fVar2 = c.pct(static_cast<int>(param_2));
    WrapAdjust(fVar1, fVar2);
    if (fVar1 <= fVar2) {
        c.kill(param_3);
        c.add(param_3, -param_4);
        c.kill(param_2);
        c.add(param_2, param_4);
        c.add(param_1, param_4 * 2);
        return;
    }
    c.kill(param_2);
    c.add(param_2, -param_4);
    fVar1 = c.pct(static_cast<int>(param_1));
    fVar2 = c.pct(static_cast<int>(param_3));
    WrapAdjust(fVar1, fVar2);
    if (fVar2 < fVar1) {
        c.kill(param_3);
        c.add(param_3, param_4);
        c.add(param_1, param_4 * 2);
        return;
    }
    c.kill(param_1);
    c.add(param_1, param_4);
    c.add(param_3, param_4 * 2);
}
}}  // namespace mashed_re::Frontend

static void Kill_00422fd0(std::uint32_t v) { s_F00422fd0(v); }
static float Pct_00408ad0(int v) { return s_F00408ad0(v); }
static void Add_0040b290(std::uint32_t v, int d) { s_F0040b290(v, d); }

// .asi export of the body above with the original's own callees.
extern "C" __declspec(dllexport) void __cdecl RaceRankThreePlayers(
    std::uint32_t param_1, std::uint32_t param_2, std::uint32_t param_3, int param_4)
{
    const mashed_re::Frontend::Rank3Callees c{Pct_00408ad0, Kill_00422fd0, Add_0040b290};
    mashed_re::Frontend::RaceRankThree(param_1, param_2, param_3, param_4, c);
}

// NOT installed yet: verification-blocked. This is a faithful bit-identical
// transcription (build-clean), but the existing diff harness has no clean,
// safe way to exercise it — it reads positions via FUN_00408ad0 and applies
// non-idempotent score deltas via FUN_0040b290, with no single observable
// scalar. Enable RH_ScopedInstall once a position-seed + score-snapshot
// harness lands (flagged SWEEP-CRITICAL in the c3-batch-af queue fragment).
// RH_ScopedInstall(RaceRankThreePlayers, 0x0040d590);

// ---------------------------------------------------------------------------
// CopterClumpRegister  --  0x00425c00
//
// Original: FUN_00425c00 (0x00425c00..0x00425c9a)
// Signature: undefined4 FUN_00425c00(const char* copterName)  (returns 1)
//
// Verbatim transcription:
//   FUN_004c5c80(DAT_00644150);                       // push render context
//   scan &DAT_00899260 (stride 0x13 ints, end 0x8994c0) for first slot[0]==0;
//   FUN_004a2b60(buf,"%s%s%s%s","ToastArt/common/perm/","Copters/",name,".dff");
//   handle = FUN_004781b0(buf, 0);                     // load RpClump
//   *slot = handle;
//   FUN_00425b90(slot);                                // ForAllAtomics dispatch
//   FUN_004c5c80(0);                                   // pop render context
//   return 1;
//
// Constants cited:
//   0x00899260 — copter slot array base                [0x00425c2c]
//   0x008994c0 — scan end bound (exclusive)            [0x00425c40]
//   0x13 (19)  — dword stride per slot                 [0x00425c34]
//   "ToastArt/common/perm/", "Copters/", ".dff" (0x005cd29c) — path components.
//   0x005cd29c content read = ".dff\0\0\0\0.txd" -> ".dff" extension. (resolves U-4204)
//
// ref: re/analysis/frontend_c1_to_c2_s3/FUN_00425c00.md
// ---------------------------------------------------------------------------

// 0x00425c00
extern "C" __declspec(dllexport) std::uint32_t __cdecl CopterClumpRegister(
    const char* copterName)
{
    s_F004c5c80(*reinterpret_cast<std::uint32_t*>(k_RenderCtx));

    std::int32_t* slot = nullptr;
    std::int32_t* scan = reinterpret_cast<std::int32_t*>(k_CopterArrayBase);
    int idx = 0;
    do {
        if (*scan == 0) {
            slot = reinterpret_cast<std::int32_t*>(k_CopterArrayBase) +
                   idx * k_CopterStride;
            break;
        }
        scan += k_CopterStride;
        ++idx;
    } while (reinterpret_cast<std::uintptr_t>(scan) < k_CopterArrayEnd);

    char buf[64];
    s_F004a2b60(buf, "%s%s%s%s", "ToastArt/common/perm/", "Copters/",
                copterName, ".dff");

    std::uint32_t handle = s_F004781b0(buf, 0);
    *slot = static_cast<std::int32_t>(handle);
    s_F00425b90(slot);
    s_F004c5c80(0);
    return 1;
}

// NOT installed yet: verification-blocked. Faithful transcription, but calling
// it at the diff-attach menu is unsafe — it loads a copter clump off disk
// (FUN_004781b0), consumes a slot non-idempotently, and dereferences a NULL
// slot pointer if the array is full (same as the original). Enable once an
// isolated asset-load harness exists.
// RH_ScopedInstall(CopterClumpRegister, 0x00425c00);

// MATCH-S06: the body of 0x00428bf0 (TitleBuildInfoRender) moved to Match/MatchS06_00428bf0.cpp,
// which compiles the byte-matched source re/match/s06/00428bf0.c.
