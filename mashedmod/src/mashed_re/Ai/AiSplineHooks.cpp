// Mashed RE — WS-R6-A: AI Catmull-Rom spline-eval verbatim port (.asi diff lane).
//
// Target FUN_00443300 (0x00443300..0x00443432, 306 B): the AI racing-line
// geometry primitive — a standard 4-point Catmull-Rom cubic (tension 0.5)
// evaluated at parameter t on the segment after `segIdx`, writing the
// interpolated XZ float2 to `outXZ`. Pure-math LEAF: no callees, no PRNG, no
// global writes (only the 2 caller floats) → deterministic, directly bit-diffable
// in isolation. It is the geometry helper the whole AI lookahead (FUN_00443dc0 /
// FUN_00443440) is built on, so porting it first builds the R6 AI diff chain
// outward from the leaf. Scope: re/analysis/R6_HANDLING_PORT_SCOPE_20260701.md.
//
// Like Vehicle/PhysicsChainHooks.cpp this file is .asi-ONLY (asi_sources.rsp; NOT
// in the exe source list). It installs the reimpl live at 0x00443300 via the
// inline-JMP so a canonical race runs MY body, and an in-process A/B self-test
// (env MASHED_AI_SPLINE_SELFTEST) compares MINE vs the ORIGINAL body per call with
// ZERO Frida overhead (Frida Interceptor on this per-frame hot path destabilizes
// MASHED — CLAUDE.md).
//
// ── why the body is naked (D4-DC-INC-ai 2026-11-01: it moved to Ai/AiSplineEval.cpp, both targets) ──
// FUN_00443300 keeps t, t², t³ AND every polynomial coefficient in 80-bit x87 ST
// registers across the WHOLE function, rounding to float32 ONLY at the two final
// FSTP stores (X @0x004433cf, Z @0x00443427). There are NO intermediate float32
// stores. A portable-C transcription CANNOT reproduce that: MSVC `float` locals
// are 32-bit and `long double`==`double`==64-bit, so any C form inserts float32/64
// roundings the original never does AND lets the compiler pick its own association
// order. Measured (this session, MASHED_AI_SPLINE_SELFTEST live in a 4-car race):
// the C form `AiSplineEval` (deleted 2026-11-01, never called) diverged from the original by 1–4 ULP in the low
// mantissa on ~all calls (e.g. Z: orig=0xc0c0b256 vs C=0xc0c0b255) — the exact
// x87-association residual class of [[project_phys_chain_float10_methodology]].
// Same category as AiTargeting.cpp's note that x87-FPU functions "need a naked-asm
// shim" — not C-expressible with bit-identity.
//
//   AiSplineEval      — greenfield C++ reimpl, 1–4 ULP off, never installed or called.
//                       DELETED 2026-11-01 (D4-DC-INC-ai): a second body for the RVA.
//   AiSplineEval_x87  — naked-asm VERBATIM transcription of 0x00443300..0x00443432
//                       (exact 80-bit ST chain, same operation order). This IS the
//                       installed .asi body → bit-identical to the original. The
//                       self-test verifies the transcription is faithful (catches a
//                       mis-transcribed ST index / operand, per
//                       [[feedback_diff_reimpl_asm_vs_original]]).
//
// Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Every constant/offset/RVA below re-confirmed this session (Mashed_pool0,
// read-only, 2026-07-01: decomp_function + listing_code_units_list + memory_read).
// .rdata bit patterns (memory_read) — these CORRECT the stale table in
// re/analysis/bucket_ai_00415d00_00452ea0/0x00443300.md (which mislabeled
// 005cc31c/005cc358/005cc35c):
//   _DAT_005cc31c=0x40400000=3.0  _DAT_005cc320=0x3f800000=1.0
//   _DAT_005cc32c=0x3f000000=0.5  _DAT_005cc358=0x40a00000=5.0
//   _DAT_005cc35c=0x40800000=4.0  DAT_005d757c =0x00000000=0.0
// (all six are exactly representable in float32; Ai/AiSplineEval.cpp holds exact copies.)

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <windows.h>

// The installed body, AiSplineEval_x87, lives in Ai/AiSplineEval.cpp (both targets, D4-DC-INC-ai 2026-11-01).
extern "C" void __cdecl AiSplineEval_x87(int splineBase, int segIdx, float t, float* outXZ);   // FUN_00443300

namespace {

// ── trampoline to the ORIGINAL body (for the self-test) ──────────────────────
// HookSystem overwrites exactly the first 5 bytes at 0x00443300 (8b 4c 24 08 =
// MOV ECX,[ESP+8]; 53 = PUSH EBX) with the E9 rel32 — the boundary lands on the
// instruction boundary at 0x00443305. Re-execute those 2, then JMP into the
// untouched remainder. __cdecl arg layout matches the original entry byte-for-byte.
void* g_orig_443305 = reinterpret_cast<void*>(0x00443305);
__declspec(naked) void OrigSplineEval(int /*splineBase*/, int /*segIdx*/, float /*t*/, float* /*outXZ*/)
{
    __asm {
        mov  ecx, dword ptr [esp+8]      // re-exec 0x00443300
        push ebx                         // re-exec 0x00443304
        jmp  dword ptr [g_orig_443305]   // continue original at 0x00443305
    }
}

// ── in-process A/B self-test (env MASHED_AI_SPLINE_SELFTEST) ─────────────────
inline int SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_SPLINE_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}

void SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_spline_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

long g_calls    = 0;   // total comparisons performed
long g_mismatch = 0;   // total bit-mismatches (installed x87 body vs original)
const long kMaxCompare = 20000;  // after this, pass-through MINE only (avoid overhead)

void SplineSelfTest(int splineBase, int segIdx, float t, float* mine, float* orig) {
    std::uint32_t mx, mz, ox, oz;
    std::memcpy(&mx, &mine[0], 4); std::memcpy(&mz, &mine[1], 4);
    std::memcpy(&ox, &orig[0], 4); std::memcpy(&oz, &orig[1], 4);
    ++g_calls;
    if ((mx != ox) || (mz != oz)) {
        ++g_mismatch;
        std::uint32_t tb; std::memcpy(&tb, &t, 4);
        char line[256];
        wsprintfA(line, "[%ld] MISMATCH base=%08x seg=%d t=%08x  X:o=%08x m=%08x  Z:o=%08x m=%08x\r\n",
                  g_calls, (unsigned)splineBase, segIdx, tb, ox, mx, oz, mz);
        SelfTestLog(line);
    }
    if ((g_calls & 0x1ff) == 1) {          // heartbeat ~every 512 calls
        char line[128];
        wsprintfA(line, "[%ld] calls=%ld mism=%ld %s\r\n",
                  g_calls, g_calls, g_mismatch, g_mismatch ? "" : "ALL-GREEN");
        SelfTestLog(line);
    }
}

// ── C dispatcher: the installed hook body ────────────────────────────────────
void AiSpline_Dispatch(int splineBase, int segIdx, float t, float* outXZ) {
    if (SelfTestEnabled() && g_calls < kMaxCompare) {
        float mine[2], orig[2];
        AiSplineEval_x87(splineBase, segIdx, t, mine);      // installed (bit-identical) body
        OrigSplineEval  (splineBase, segIdx, t, orig);      // original via trampoline
        SplineSelfTest(splineBase, segIdx, t, mine, orig);
        outXZ[0] = mine[0]; outXZ[1] = mine[1];             // faithful: hook writes MINE
    } else {
        AiSplineEval_x87(splineBase, segIdx, t, outXZ);
    }
}

// ── naked entry installed at 0x00443300 (forwards the 4 __cdecl stack args) ──
__declspec(naked) void AiSpline_Entry() {
    __asm {
        // stack: [esp]=ret [esp+4]=splineBase [esp+8]=segIdx [esp+c]=t [esp+10]=outXZ
        push dword ptr [esp+0x10]   // outXZ
        push dword ptr [esp+0x10]   // t          (post-push: original [esp+0xc])
        push dword ptr [esp+0x10]   // segIdx     (original [esp+8])
        push dword ptr [esp+0x10]   // splineBase (original [esp+4])
        call AiSpline_Dispatch
        add  esp, 16                // cdecl clean our 4 forwarded args
        ret                         // original is caller-cleans (ret no imm)
    }
}

}  // namespace

RH_ScopedInstall(AiSpline_Entry, 0x00443300);
