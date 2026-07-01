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
// ── TWO bodies, and why ──────────────────────────────────────────────────────
// FUN_00443300 keeps t, t², t³ AND every polynomial coefficient in 80-bit x87 ST
// registers across the WHOLE function, rounding to float32 ONLY at the two final
// FSTP stores (X @0x004433cf, Z @0x00443427). There are NO intermediate float32
// stores. A portable-C transcription CANNOT reproduce that: MSVC `float` locals
// are 32-bit and `long double`==`double`==64-bit, so any C form inserts float32/64
// roundings the original never does AND lets the compiler pick its own association
// order. Measured (this session, MASHED_AI_SPLINE_SELFTEST live in a 4-car race):
// the C form `AiSplineEval` below diverges from the original by 1–4 ULP in the low
// mantissa on ~all calls (e.g. Z: orig=0xc0c0b256 vs C=0xc0c0b255) — the exact
// x87-association residual class of [[project_phys_chain_float10_methodology]].
// Same category as AiTargeting.cpp's note that x87-FPU functions "need a naked-asm
// shim" — not C-expressible with bit-identity.
//
//   AiSplineEval      — greenfield C++ reimpl; exe-portable (splineBase is a param,
//                       no hardcoded globals); BEHAVIORALLY faithful (1–4 ULP,
//                       gameplay-inert for an AI-lookahead geometry point). For a
//                       future standalone AiStandalone wiring. NOT the installed body.
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
// (all six are exactly representable in float32; the naked asm reads them from the
// live .rdata at their original addresses.)

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <windows.h>

namespace {

// exact float32 constants (all exactly representable → bit-identical to .rdata).
// Non-const so storage is always emitted (the naked __asm below references them by
// symbol as `fmul dword ptr [k5_0]` etc. — MSVC __asm rejects bare absolute-address
// FPU operands `[05cc358h]` with C2415; the symbol form is portable + identical).
float k3_0 = 3.0f;   // _DAT_005cc31c (cubic P1*3 / P2*3)
float k1_0 = 1.0f;   // _DAT_005cc320 (t clamp upper)
float k0_5 = 0.5f;   // _DAT_005cc32c (Catmull-Rom final factor)
float k5_0 = 5.0f;   // _DAT_005cc358 (quadratic P1*5)
float k4_0 = 4.0f;   // _DAT_005cc35c (quadratic P2*4)
float k0_0 = 0.0f;   // DAT_005d757c  (t clamp lower)

}  // namespace

// ===========================================================================
// 0x00443300  AiSplineEval(splineBase, segIdx, t, outXZ)  [void, __cdecl]
// GREENFIELD C++ reimpl (reference / exe-portable; NOT the installed body).
//
// point[i] = float2 at splineBase + i*8 (X@+0, Z@+4); count = *(int*)(splineBase
// + 0x200). Wrapped points P0..P3 = indices [segIdx-1..segIdx+2] mod count. t
// clamped to [0,1]; per component
//   out = 0.5 * ( 2*P1 + (P2-P0)*t + ((3*P1-P0-3*P2)+P3)*t^3 + ((4*P2+2*P0-5*P1)-P3)*t^2 ).
// Behaviorally faithful; carries a 1–4 ULP x87-association residual vs original
// (see file header) — inconsequential for an AI-lookahead geometry point.
// ===========================================================================
extern "C" __declspec(dllexport)
void AiSplineEval(int param_1, int param_2, float param_3, float* param_4)
{
    float* pfVar1; float* pfVar2; float* pfVar3; float* pfVar4;
    int iVar5; float fVar6; float fVar7;

    param_2 = param_2 + -1;                                     // 0x0044330a
    if (param_2 < 0) param_2 = param_2 + *(int*)(param_1 + 0x200);   // 0x00443311 P0 wrap
    iVar5  = *(int*)(param_1 + 0x200);                          // 0x00443317 count
    pfVar1 = (float*)(param_1 + param_2 * 8);                   // 0x0044331d P0
    param_2 = param_2 + 1;                                      // P1 index
    if (iVar5 <= param_2) param_2 = param_2 - iVar5;
    pfVar2 = (float*)(param_1 + param_2 * 8);                   // 0x00443327 P1
    param_2 = param_2 + 1;                                      // P2 index
    if (iVar5 <= param_2) param_2 = param_2 - iVar5;
    pfVar3 = (float*)(param_1 + param_2 * 8);                   // 0x00443331 P2
    param_2 = param_2 + 1;                                      // P3 index
    if (iVar5 <= param_2) param_2 = param_2 - iVar5;
    fVar6 = k1_0;                                               // t clamp
    if ((param_3 <= k1_0) && (fVar6 = param_3, param_3 < k0_0)) fVar6 = k0_0;
    fVar7 = fVar6 * fVar6;                                      // t^2
    pfVar4 = (float*)(param_1 + 4 + param_2 * 8);               // P3.z addr

    *param_4 = (*pfVar2 + *pfVar2 +
                (*pfVar3 - *pfVar1) * fVar6 +
                (((*pfVar2 * k3_0 - *pfVar1) - *pfVar3 * k3_0) +
                 *(float*)(param_1 + param_2 * 8)) * fVar7 * fVar6 +
                ((*pfVar3 * k4_0 + ((*pfVar1 + *pfVar1) - *pfVar2 * k5_0)) -
                 *(float*)(param_1 + param_2 * 8)) * fVar7) * k0_5;
    param_4[1] = (pfVar2[1] + pfVar2[1] +
                  (pfVar3[1] - pfVar1[1]) * fVar6 +
                  (((pfVar2[1] * k3_0 - pfVar1[1]) - pfVar3[1] * k3_0) + *pfVar4) *
                  fVar7 * fVar6 +
                  ((pfVar3[1] * k4_0 + ((pfVar1[1] + pfVar1[1]) - pfVar2[1] * k5_0))
                   - *pfVar4) * fVar7) * k0_5;
}

namespace {

// ===========================================================================
// AiSplineEval_x87 — NAKED VERBATIM transcription of 0x00443300..0x00443432.
// The INSTALLED .asi body: exact 80-bit x87 ST chain → bit-identical. Callable as
// __cdecl (args at [esp+4..]) — layout matches the original entry, so the
// verbatim [esp+8]/[esp+1ch]/[esp+20h] offsets are correct whether reached by the
// installed inline-JMP or a normal call. Reads its 6 float consts from the live
// .rdata at their original addresses (valid inside the injected MASHED). Each ST
// index is annotated with the RVA it transcribes.
//   EBX=splineBase  ECX=index  EDI=&P0  ESI=&P1  EDX=&P2  EBP=outXZ
//   [ebx+ecx*8]=P3.x  [eax]=P3.z ; x87 keeps t^3(ST0),t^2(ST1),t(ST2) at bottom.
// ===========================================================================
__declspec(naked) void AiSplineEval_x87(int /*splineBase*/, int /*segIdx*/, float /*t*/, float* /*outXZ*/)
{
    __asm {
        mov     ecx, dword ptr [esp+8]        // 00443300  segIdx
        push    ebx                           // 00443304
        mov     ebx, dword ptr [esp+8]        // 00443305  splineBase
        push    ebp                           // 00443309
        dec     ecx                           // 0044330a
        test    ecx, ecx                      // 0044330b
        push    esi                           // 0044330d
        push    edi                           // 0044330e
        jge     L317                          // 0044330f
        add     ecx, dword ptr [ebx+200h]     // 00443311  P0 wrap +count
    L317:
        mov     eax, dword ptr [ebx+200h]     // 00443317  count
        lea     edi, [ebx+ecx*8]              // 0044331d  &P0
        inc     ecx                           // 00443320
        cmp     ecx, eax                      // 00443321
        jl      L327                          // 00443323
        sub     ecx, eax                      // 00443325
    L327:
        lea     esi, [ebx+ecx*8]              // 00443327  &P1
        inc     ecx                           // 0044332a
        cmp     ecx, eax                      // 0044332b
        jl      L331                          // 0044332d
        sub     ecx, eax                      // 0044332f
    L331:
        lea     edx, [ebx+ecx*8]              // 00443331  &P2
        inc     ecx                           // 00443334
        cmp     ecx, eax                      // 00443335
        jl      L33b                          // 00443337
        sub     ecx, eax                      // 00443339  ECX = P3 index
    L33b:
        fld     dword ptr [esp+1ch]           // 0044333b  t
        fld     dword ptr [esp+1ch]           // 0044333f  t
        fcomp   dword ptr [k1_0]          // 00443343  vs 1.0
        fnstsw  ax                            // 00443349
        test    ah, 41h                       // 0044334b
        jnz     L35a                          // 0044334e
        fstp    st(0)                         // 00443350
        fld     dword ptr [k1_0]          // 00443352  -> 1.0
        jmp     L373                          // 00443358
    L35a:
        fld     dword ptr [esp+1ch]           // 0044335a  t
        fcomp   dword ptr [k0_0]          // 0044335e  vs 0.0
        fnstsw  ax                            // 00443364
        test    ah, 5                         // 00443366
        jp      L373                          // 00443369
        fstp    st(0)                         // 0044336b
        fld     dword ptr [k0_0]          // 0044336d  -> 0.0
    L373:
        fld     st(0)                         // 00443373  ST: t, t
        mov     ebp, dword ptr [esp+20h]      // 00443375  outXZ
        fmul    st(0), st(1)                  // 00443379  ST0 = t*t = t^2
        lea     eax, [ebx+ecx*8+4]            // 0044337b  &P3.z
        fld     st(0)                         // 0044337f  ST: t^2, t^2, t
        fmul    st(0), st(2)                  // 00443381  ST0 = t^2*t = t^3
        // ---- X channel (P*.x = [reg]) ----
        fld     dword ptr [edi]               // 00443383  P0x
        fadd    st(0), st(0)                  // 00443385  2*P0x
        fld     dword ptr [esi]               // 00443387  P1x
        fmul    dword ptr [k5_0]          // 00443389  5*P1x
        fsubp   st(1), st(0)                  // 0044338f  2P0x-5P1x
        fld     dword ptr [edx]               // 00443391  P2x
        fmul    dword ptr [k4_0]          // 00443393  4*P2x
        faddp   st(1), st(0)                  // 00443399  +4P2x
        fsub    dword ptr [ebx+ecx*8]         // 0044339b  -P3x  => Qx
        fmul    st(0), st(2)                  // 0044339e  Qx * t^2
        fld     dword ptr [esi]               // 004433a0  P1x
        fmul    dword ptr [k3_0]          // 004433a2  3*P1x
        fsub    dword ptr [edi]               // 004433a8  3P1x-P0x
        fld     dword ptr [edx]               // 004433aa  P2x
        fmul    dword ptr [k3_0]          // 004433ac  3*P2x
        fsubp   st(1), st(0)                  // 004433b2  (3P1x-P0x)-3P2x
        fadd    dword ptr [ebx+ecx*8]         // 004433b4  +P3x  => Cx
        fmul    st(0), st(2)                  // 004433b7  Cx * t^3
        faddp   st(1), st(0)                  // 004433b9  Cx*t^3 + Qx*t^2
        fld     dword ptr [edx]               // 004433bb  P2x
        fsub    dword ptr [edi]               // 004433bd  P2x-P0x
        fmul    st(0), st(4)                  // 004433bf  (P2x-P0x)*t
        faddp   st(1), st(0)                  // 004433c1  + ...
        fld     dword ptr [esi]               // 004433c3  P1x
        fadd    st(0), st(0)                  // 004433c5  2*P1x
        faddp   st(1), st(0)                  // 004433c7  + 2P1x
        fmul    dword ptr [k0_5]          // 004433c9  * 0.5
        fstp    dword ptr [ebp]               // 004433cf  store X
        // ---- Z channel (P*.z = [reg+4]; P3.z = [eax]) ----
        fld     dword ptr [edi+4]             // 004433d2  P0z
        fadd    st(0), st(0)                  // 004433d5  2*P0z
        fld     dword ptr [esi+4]             // 004433d7  P1z
        fmul    dword ptr [k5_0]          // 004433da  5*P1z
        fsubp   st(1), st(0)                  // 004433e0  2P0z-5P1z
        fld     dword ptr [edx+4]             // 004433e2  P2z
        fmul    dword ptr [k4_0]          // 004433e5  4*P2z
        faddp   st(1), st(0)                  // 004433eb  +4P2z
        fsub    dword ptr [eax]               // 004433ed  -P3z  => Qz
        fmul    st(0), st(2)                  // 004433ef  Qz * t^2
        fld     dword ptr [esi+4]             // 004433f1  P1z
        fmul    dword ptr [k3_0]          // 004433f4  3*P1z
        fsub    dword ptr [edi+4]             // 004433fa  3P1z-P0z
        fld     dword ptr [edx+4]             // 004433fd  P2z
        fmul    dword ptr [k3_0]          // 00443400  3*P2z
        fsubp   st(1), st(0)                  // 00443406  (3P1z-P0z)-3P2z
        fadd    dword ptr [eax]               // 00443408  +P3z  => Cz
        fmul    st(0), st(2)                  // 0044340a  Cz * t^3
        faddp   st(1), st(0)                  // 0044340c  Cz*t^3 + Qz*t^2
        fld     dword ptr [edx+4]             // 0044340e  P2z
        fsub    dword ptr [edi+4]             // 00443411  P2z-P0z
        pop     edi                           // 00443414
        fmul    st(0), st(4)                  // 00443415  (P2z-P0z)*t
        faddp   st(1), st(0)                  // 00443417  + ...
        fld     dword ptr [esi+4]             // 00443419  P1z
        pop     esi                           // 0044341c
        fadd    st(0), st(0)                  // 0044341d  2*P1z
        faddp   st(1), st(0)                  // 0044341f  + 2P1z
        fmul    dword ptr [k0_5]          // 00443421  * 0.5
        fstp    dword ptr [ebp+4]             // 00443427  store Z
        pop     ebp                           // 0044342a
        pop     ebx                           // 0044342b
        fstp    st(0)                         // 0044342c  clear t^3
        fstp    st(0)                         // 0044342e  clear t^2
        fstp    st(0)                         // 00443430  clear t
        ret                                   // 00443432  (caller-cleans)
    }
}

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
