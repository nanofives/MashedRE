// Mashed RE — WS-PHYS-C4-LANE: installed-hook telemetry C4 lane for the
// per-frame vehicle-physics chain (.asi-ONLY).
//
// These are .asi-resident VERBATIM transcriptions of the register-ABI physics
// functions, installed at their original RVAs via RH_ScopedInstall so the
// inline-JMP runs live during a canonical race. Unlike the standalone bodies in
// Vehicle/{VehicleControl,ForceIntegrator,Integrate2,AeroStabilize}.cpp (which
// substitute std::sqrt for the RW fast-sqrt LUT because the standalone has no RW
// device → null LUT, WS-PHYS-CRASH-FIX), THESE forward every chain callee to its
// ORIGINAL RVA — i.e. inside the injected MASHED.exe the live RW LUT primitives
// (FUN_004c3ac0/39b0/3df0/4c4d20, built by RwEngineOpen) execute, so a faithful
// C transcription CAN be bit-identical to the original (the WS-H2 architecture
// the user ratified; SESSION_VERIFICATION_AUDIT §WS-A-VERIFY-3 / §H2).
//
// REGISTER ABI (the wall run_diff can't cross): these are NOT __cdecl — the
// dispatcher (FUN_00470c70) passes the 0xd04 record in a register and several
// stack args. The contract was read from the disassembly (Mashed_pool12,
// read-only, 2026-06-17):
//   A4 FUN_00470670 : EAX=record; stack [+4]=param_1 [+8]=dt [+c]=input* [+10]=xform
//                     (caller-cleans; 5 reg pushes balanced; `ret` no-imm)
//       dispatches:  FUN_0046ddb0(dt, wheelBlock, xform)        EDI=record  (A5)
//                    FUN_00467650(param_1, dt, wheelBlock, input) ESI=record (A6a)
//                    FUN_00468980(dt, input)                    ECX=ESI=record (A6b)
//       smoother:    FUN_004a2c48 round-ST0-to-i64 (implicit ST0; A4 loads
//                    FILD[+0xb24];FADD dt then CALL, stores EAX → [+0xb24])
//   wheelBlock = record + *(int*)(record+0x9a8)*0x40 + 0x928
//
// A4 is the lane PROOF: its OWN body is pure x87 + integer arithmetic with NO
// transcendental/LUT hazard (the LUT calls all live in the A5/A6a/A6b callees it
// dispatches, which we forward to the live originals). Built x87 (the .asi TU has
// no /arch:SSE2), the C transcription rounds each store to float32 the same way
// the original does → bit-identical field writes.
//
// This file is EXCLUDED from the exe target (asi_sources.rsp only). It is gated by
// MASHED_HOOK_ONLY at install (HookSystem.cpp) so a C4 run installs ONLY the
// physics hook live while every other RVA stays original.
//
// Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// Every constant memory_read / every register-ABI fact disassembled this session.

#include "../Core/HookSystem.h"
#include <cstdint>
#include <windows.h>
#include <cstdlib>
#include <cstring>

// [dcphys 2026-10-21] A6a's ONE body (0x00467650), shared with mashed_re.exe: Vehicle/Integrate2.cpp.
#include "ForceIntegrator.h"   // [dcphys DEV] g_a6aDump / g_a6aFrame branch flags for the three-way self-test
namespace mashed_re { namespace Vehicle {
void Vehicle_Integrate2(int* self, int param_1, float dt, void* wheelBlock, std::uint8_t* input);
int  VehicleInit(int slot, int trackType);   // A3 0x0046b540 (Vehicle/VehicleInit.cpp)
void Vehicle_AeroStabilize(int* self, float* orient, float dt);   // A6b 0x00468980 (Vehicle/AeroStabilize.cpp)
void VehicleWheelForceIntegrate(int* self, float dt, void* xform); // A5 0x0046ddb0 (Vehicle/ForceIntegrator.cpp)
void VehicleControlIntegrate(int* self, float dt, std::uint8_t* input, void* xform, int car);   // A4 0x00470670 (Vehicle/VehicleControl.cpp)
} }
// [dcphys 2026-10-21] The .asi's DEFAULT for the shared A6a body = what its former verbatim copy computed (bit-identical
// on every sampled call, verify/dcphys_20261021): the A6a knobs the standalone seeds through Core/FaithDefaults.cpp
// (exe only, so the .asi never sees them) plus the drive arm's LUT normalise, which the verbatim copy called live
// (0x004c39b0). [dotflip 2026-10-22] plus A6A_DOT_ORDER, default ON in both targets (Mariano's decision), with which the
// shared body is bit-identical to the original. As in Core/FaithDefaults.cpp: MASHED_<name> already set is left as given,
// MASHED_NO_<name>=1 skips that one. A knob only, never a second body.
namespace {
struct A6aAsiDefaults {
    A6aAsiDefaults() {
        const char* const ks[] = { "A6A_LC0", "A6A_GEAR_ORDER", "A6A_SPIN_ORDER", "A6A_DRIVE_NORM_LUT", "A6A_DOT_ORDER",
                                   // [dcphys2] A6b: what the deleted verbatim copy computed (Vehicle/AeroStabilize.cpp)
                                   "A6B_ACOS_CRT", "A6B_NORM_LUT",
                                   // [dcphys2] A5 (Vehicle/ForceIntegrator.cpp): the drafting test in float / LUT and the
                                   // face-normal operands in the original's registers (0x0046e6f3..0x0046e728)
                                   "A5_DRAFT_LUT", "A5_RING_ORDER", "A5_DOT_ORDER", "A5_AIR_F32", "A5_CONTACT_INT", "A5_SPIN_FILD",
                                   // [dcphys2] A4 (Vehicle/VehicleControl.cpp): the slide dot (0x004706db..0x00470701)
                                   "A4_DOT_ORDER" };
        char on[64], no[64];
        for (const char* k : ks) {
            std::strcpy(on, "MASHED_");    std::strcat(on, k);
            std::strcpy(no, "MASHED_NO_"); std::strcat(no, k);
            const char* n = std::getenv(no);
            if (std::getenv(on) || (n && n[0] && n[0] != '0')) continue;
            _putenv_s(on, "1");
        }
    }
};
const A6aAsiDefaults s_a6aAsiDefaults;
}  // namespace

// ── live-original forward addresses (absolute; MASHED is /FIXED base 0x400000) ─
extern "C" {
    void* PCH_Fwd_4a2c48 = reinterpret_cast<void*>(0x004a2c48);  // round ST0 -> i64
    void* PCH_Fwd_46ddb0 = reinterpret_cast<void*>(0x0046ddb0);  // A5
    void* PCH_Fwd_467650 = reinterpret_cast<void*>(0x00467650);  // A6a
    void* PCH_Fwd_468980 = reinterpret_cast<void*>(0x00468980);  // A6b
    // A5-chain live callees (forwarded so the running MASHED's RW LUT / PRNG run):
    void* PCH_Fwd_46c5f0 = reinterpret_cast<void*>(0x0046c5f0);  // TriangleFaceNormal (EDI=record)
    // A6a chain: the ST0 smoother + the two game-mode getters + the LUT magnitude
    // (forwarded as a raw addr so float10-preserving naked shims can keep ST0).
    void* PCH_Fwd_40e340 = reinterpret_cast<void*>(0x0040e340);  // player-count getter
    void* PCH_Fwd_4c3ac0 = reinterpret_cast<void*>(0x004c3ac0);  // Vec3 magnitude (ST0 float10)
}


namespace {

// ── byte-offset field views on the live 0xd04 record ────────────────────────
inline float& Fb(void* b, int off) { return *reinterpret_cast<float*>(reinterpret_cast<char*>(b) + off); }
inline int&   Ib(void* b, int off) { return *reinterpret_cast<int*>  (reinterpret_cast<char*>(b) + off); }

// ── tuning constants: EXACT bit patterns @ address (memory_read, pool12) ─────
// Initialized from the raw 32-bit hex so they bit-match the original's .rdata
// (decimal literals mis-round: e.g. 1.66677e-4f -> 0x392ec604 != 0x392ec33e,
// which caused the WS-PHYS-C4-LANE grip-branch ~1-2 ULP divergence — fixed here).
inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }

// runtime global the torque ring phase indexes (live: DAT_007f101c)
inline int& TorqueRingPhase() { return *reinterpret_cast<int*>(0x007f101c); }

// FUN_0040e350  Fi_GameMode  (__cdecl int(void))
inline int Fi_GameMode() { return reinterpret_cast<int(__cdecl*)()>(0x0040e350)(); }


}  // namespace


namespace {

// IN-PROCESS BIT-IDENTITY SELF-TEST (gated by env MASHED_PHYS_C4_SELFTEST). Frida Interceptor on this >1000/s hot
// path destabilizes MASHED, so the A/B runs in process: the original A4 is reached through OrigA4Trampoline (its
// prologue re-executed, then JMP 0x0047067b). [dcphys2] The test is now the whole function (A4_SelfTest below);
// the early-return patch at 0x00470914 and the 13-dword compare are gone with the old .asi copy.

inline int SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_PHYS_C4_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}


// re-execute the original prologue then jump to A4+5 — i.e. run the ORIGINAL A4.
// 5 saved prologue bytes: 51 53 55 8b 6c  (PUSH ECX;PUSH EBX;PUSH EBP; MOV EBP,..[split])
__declspec(naked) void OrigA4Trampoline() {
    __asm {
        // executed with EAX=record + the 4 cdecl stack args already in place,
        // exactly as the original is entered. Re-run the saved prologue bytes:
        push ecx                              // 0x51
        push ebx                              // 0x53
        push ebp                              // 0x55
        mov  ebp, dword ptr [esp+0x18]        // 8b 6c 24 18 (the full original MOV)
        push esi                              // continue original at A4+0x7 (0x00470677)
        push edi
        mov  edi, eax
        // jump into the original body at 0x0047067b (after MOV EDI,EAX, i.e.
        // the CALL 0x0040e350 — keeps the gameMode call identical)
        mov  eax, 0x0047067b
        jmp  eax
        // control reaches 0x00470914 -> our patched POP../RET unwinds the 5
        // pushes above and returns here's caller.
    }
}


void SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("phys_c4_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

// Per-branch sample counters and quotas for A4_SelfTest (quotas x MASHED_PHYS_SELFTEST_CAP).
int g_selfTestCount = 0;        // total logged calls
int g_selfTestIdle  = 0;        // coast calls (no input) — sampled sparsely
// NAMES CORRECTED 2026-08-24 — these counters were named accel/brake, but A4 reads
// input[0]/[1] which are the STEER signs, not accel/brake (accel/brake are
// input[4]/[5] and are read by A6a; see VehicleControl.cpp's descriptor map). The
// counter VARIABLE names are kept so the existing log format and any archived log
// parsers still line up; the semantics below are the corrected ones.
int g_selfTestAccel = 0;        // steer-sign-A-only calls (input[0]!=0, input[1]==0)
int g_selfTestBrake = 0;        // steer-sign-B calls (input[1]!=0)  — TARGET branch
int g_selfTestHiIn5 = 0;        // input[5]>128 grip-else branch     — TARGET branch
const int kMaxIdle  = 8;
const int kMaxAccel = 40;
const int kMaxBrake = 40;       // dedicated quota for the input[1] (steer sign B) branch
const int kMaxHiIn5 = 16;
// WS-PHYS-COVERAGE-SCENARIO: the input[1]!=0 branch (the FCHS-negated force path) and
// the input[5]>128 grip-else branch were unhit in the straight-line player-car demo.
// The per-frame dispatcher invokes A4 for EVERY car, and the AI opponents (slots 1-3)
// steer through corners on their own, so those calls arrive from AI records during a
// normal race. Independent per-branch quotas keep the (far more numerous) sign-A/coast
// calls from filling the cap first.
//
// CONSEQUENCE OF THE RENAME, recorded because a coverage claim rests on it: A4 has NO
// BRAKE BRANCH. The branch this quota covers is STEER SIGN B (steer left), and the
// rationale "AI opponents brake into corners" was the right prediction for the wrong
// reason — what lands here is AI cars STEERING. The branch was genuinely exercised, so
// the coverage result stands; only its label was wrong.
//
// It also CORROBORATES the steer model rather than undermining it: the FCHS negation
// (force = -force) in the input[1] branch is precisely the mechanism by which the two
// steer signs yield opposite wheel angles, matching VehiclePhysicsRun.h:76
// (+steer -> input[0] -> +angle, -steer -> input[1] -> -angle). Under the old
// "brake/reverse" reading that same negation was explained as reverse thrust; both fit
// the arithmetic, but only the steer reading is consistent with input[0]/[1] being set
// from io.steer at VehiclePhysicsRun.cpp:404-405.
// [dcphys2 2026-10-22] the WHOLE original A4: the same trampoline with no early-return patch, so the original runs its
// own dispatch (0x00470914..0x00470943: A5 / A6a / A6b at their RVAs) and parked damp, and its epilogue
// (0x0047098c..0x00470991) pops the five prologue pushes and returns here.
__declspec(noinline) void RunOrigFull(void* record, int param_1, float dt, void* input, void* xform) {   // [dcphys2] noinline: the __asm CALL runs the original chain, whose RW leaves use XMM; inlined, the compiler kept dt in an XMM register across it (run 2 read dt = 0)
    __asm {
        mov  eax, record
        push xform
        push input
        push dt
        push param_1
        call OrigA4Trampoline
        add  esp, 16
    }
}


inline int A4_SelfTestCap() {   // [dcphys2] MASHED_PHYS_SELFTEST_CAP, else MASHED_A6A_SELFTEST_CAP, else 1
    static int c = -1;
    if (c < 0) {
        const char* e = std::getenv("MASHED_PHYS_SELFTEST_CAP");
        if (!(e && std::atoi(e) > 0)) e = std::getenv("MASHED_A6A_SELFTEST_CAP");
        c = (e && std::atoi(e) > 0) ? std::atoi(e) : 1;
    }
    return c;
}
// PRNG read cursor (FUN_00534870's word; A5's random-surface branch draws through it)
inline std::uintptr_t* A4_PrngCursorPtr() {
    int base = *reinterpret_cast<int*>(0x007dc578);
    int off  = *reinterpret_cast<int*>(0x007d3ff8);
    if (base == 0) return nullptr;
    return reinterpret_cast<std::uintptr_t*>(static_cast<char*>(0) + base + 4 + off);
}

// [dcphys2 2026-10-22] WHOLE-FUNCTION A/B (the old test stopped at 0x00470914 and compared 13 dwords). Per sampled
// call: snapshot (record, the A5 scratch 0x00881560..0x0088158f, the PRNG cursor), the ORIGINAL A4 with its own
// dispatch, the ORIGINAL again from the snapshot (noo), then the SHARED body
// (Vehicle/VehicleControl.cpp), whose output stays live (the old test ran the body math a second time after it, so
// +0xb24 / +0xb28 advanced twice on every sampled call). The compare is the whole 0xd04 record plus the 12 scratch
// dwords. The callees run at their live RVAs in all four runs, so run the A/B with A4's hook alone: then the
// original A5 / A6a / A6b serve every run and the diff is A4's own. Quotas x MASHED_PHYS_SELFTEST_CAP; past quota
// the shared body runs.
void A4_SelfTest(void* record, int param_1, float dt, std::uint8_t* input, void* xform) {
    const bool brake  = (input[1] != 0);
    const bool accel  = (input[0] != 0) && !brake;
    const bool hiIn5  = ((input[0] != 0) || brake) && (input[5] > 128);
    const bool idle   = (input[0] == 0) && (input[1] == 0);
    const int cap = A4_SelfTestCap();
    bool admit = false;
    if (brake && g_selfTestBrake < kMaxBrake * cap) admit = true;
    if (hiIn5 && g_selfTestHiIn5 < kMaxHiIn5 * cap) admit = true;
    if (accel && g_selfTestAccel < kMaxAccel * cap) admit = true;
    if (idle  && g_selfTestIdle  < kMaxIdle  * cap) admit = true;
    if (!admit) {
        mashed_re::Vehicle::VehicleControlIntegrate(static_cast<int*>(record), dt, input, xform, param_1);
        return;
    }
    if (idle)  ++g_selfTestIdle;
    if (accel) ++g_selfTestAccel;
    if (brake) ++g_selfTestBrake;
    if (hiIn5) ++g_selfTestHiIn5;

    static std::uint8_t snapRec[0xd04], origRec[0xd04], o2Rec[0xd04];
    static std::uint8_t snapScr[0x30], origScr[0x30], o2Scr[0x30];
    std::uint8_t* scr = reinterpret_cast<std::uint8_t*>(0x00881560);
    std::uintptr_t* cur = A4_PrngCursorPtr();
    const std::uintptr_t cur0 = cur ? *cur : 0;
    std::memcpy(snapRec, record, 0xd04);
    std::memcpy(snapScr, scr, 0x30);
    auto restore = [&]() { std::memcpy(record, snapRec, 0xd04); std::memcpy(scr, snapScr, 0x30); if (cur) *cur = cur0; };
    auto ndw = [](const std::uint8_t* x, const std::uint8_t* y, int n) {
        int d = 0; for (int off = 0; off < n; off += 4)
            if (*reinterpret_cast<const std::uint32_t*>(x + off) != *reinterpret_cast<const std::uint32_t*>(y + off)) d++;
        return d; };

    RunOrigFull(record, param_1, dt, input, xform);                       // (1) original
    const bool rngFired = cur && (*cur != cur0);
    std::memcpy(origRec, record, 0xd04); std::memcpy(origScr, scr, 0x30);
    restore(); RunOrigFull(record, param_1, dt, input, xform);            // (2) original again: noo
    std::memcpy(o2Rec, record, 0xd04); std::memcpy(o2Scr, scr, 0x30);
    restore(); mashed_re::Vehicle::VehicleControlIntegrate(static_cast<int*>(record), dt, input, xform, param_1);  // (4) shared

    const int noo = ndw(o2Rec, origRec, 0xd04) + ndw(o2Scr, origScr, 0x30);
    // The +0x928 matrix A6b rotates (record + [+0x9a8]*0x40 + 0x928, 0x00470699) keeps three pad words that
    // FUN_004c4d20 fills from its stack, so they follow the caller's stack depth (A6b_SelfTest's note): npad, not ndiff.
    const int padRel = 0x928 + *reinterpret_cast<const int*>(snapRec + 0x9a8) * 0x40;
    int mism = 0, npad = 0; char line[1024]; int p = 0;
    for (int off = 0; off < 0xd04; off += 4) {
        const std::uint32_t ov = *reinterpret_cast<const std::uint32_t*>(origRec + off);
        const std::uint32_t mv = *reinterpret_cast<const std::uint32_t*>((char*)record + off);
        if (mv != ov) {
            if (off == padRel + 0x1c || off == padRel + 0x2c || off == padRel + 0x3c) { npad++; continue; }
            mism++; if (p < 900) p += wsprintfA(line+p, " +0x%x:o=%08x,m=%08x", off, ov, mv); }
    }
    for (int i = 0; i < 12; ++i) {
        const std::uint32_t ov = reinterpret_cast<const std::uint32_t*>(origScr)[i];
        const std::uint32_t mv = reinterpret_cast<const std::uint32_t*>(scr)[i];
        if (mv != ov) { mism++; if (p < 900) p += wsprintfA(line+p, " scr[%d]:o=%08x,m=%08x", i, ov, mv); }
    }
    char hdr[300];
    int slot = (int)(((std::uintptr_t)record - 0x008815a0) / 0xd04);
    const int grounded = *reinterpret_cast<int*>((char*)record + 0x9e0);
    wsprintfA(hdr, "[%d] slot=%d in0=%u in1=%u in5=%u gm=%d grounded=%08x air=%d state=%d rng=%d noo=%d npad=%d ndiff=%d%s\r\n",
              g_selfTestCount, slot, input[0], input[1], input[5], Fi_GameMode(), grounded,
              (grounded != 0x40800000) ? 1 : 0, *reinterpret_cast<int*>((char*)record + 0x9f0), rngFired ? 1 : 0,
              noo, npad, mism, mism ? "" : " OK");
    SelfTestLog(hdr);
    if (mism) { line[p]=0; SelfTestLog("   "); SelfTestLog(line); SelfTestLog("\r\n"); }
    if (noo) {   // which words the original's second run did not reproduce
        char nl2[600]; int q = 0;
        for (int off = 0; off < 0xd04 && q < 500; off += 4) {
            const std::uint32_t a1 = *reinterpret_cast<const std::uint32_t*>(origRec + off);
            const std::uint32_t a2 = *reinterpret_cast<const std::uint32_t*>(o2Rec + off);
            if (a1 != a2) q += wsprintfA(nl2+q, " +0x%x:o1=%08x,o2=%08x", off, a1, a2);
        }
        nl2[q] = 0; SelfTestLog("   noo:"); SelfTestLog(nl2); SelfTestLog("\r\n");
    }
    g_selfTestCount++;
}

// ===========================================================================
// A4 hook — [dcphys2] the shared body (Vehicle/VehicleControl.cpp VehicleControlIntegrate), or the A/B.
//   param_1 = stack arg0 (the car index, to A6a)
//   xform   = stack arg3 (A4's param_4; the original pushes it to A5, which never reads it)
// ===========================================================================
void A4_Body(void* record, int param_1, float dt, std::uint8_t* input, void* xform) {
    if (SelfTestEnabled())
        A4_SelfTest(record, param_1, dt, input, xform);
    else
        mashed_re::Vehicle::VehicleControlIntegrate(static_cast<int*>(record), dt, input, xform, param_1);
}

// ── entry trampoline installed at 0x00470670 ────────────────────────────────
// Captures EAX (record) + forwards the 4 cdecl stack args to A4_Body. The
// original is caller-cleans (ret no-imm), so we restore ESP and `ret` plain.
__declspec(naked) void A4_Entry() {
    __asm {
        // stack on entry: [esp]=retaddr [esp+4]=param_1 [esp+8]=dt
        //                 [esp+0xc]=input [esp+0x10]=xform ; EAX=record
        // Preserve the same callee-saved set the original (FUN_00470670) does,
        // so the MASHED caller sees EBX/ESI/EDI/EBP intact.
        push ebx
        push ebp
        push esi
        push edi                  // 4 saves -> args now at [esp+20.. ]
        push dword ptr [esp+32]   // xform   ([esp+16]+16 for the 4 saves)
        push dword ptr [esp+32]   // input
        push dword ptr [esp+32]   // dt
        push dword ptr [esp+32]   // param_1
        push eax                  // record
        call A4_Body
        add  esp, 20              // cdecl: clean our 5 forwarded args
        pop  edi
        pop  esi
        pop  ebp
        pop  ebx
        ret                       // caller cleans the original 4 stack args
    }
}

}  // namespace

// ===========================================================================
// A5 — FUN_0046ddb0 the per-wheel force integrator (EDI=record; dt, matrix).
// [dcphys2 2026-10-22] ONE body, shared with mashed_re.exe: Vehicle/ForceIntegrator.cpp (VehicleWheelForceIntegrate).
// The C++ transcription that lived here (A5_Body, C4 96/96 on the default quotas) was deleted after the shared body
// read bit-identical to the original on every sampled call of this file's A/B, where that copy missed 18..385 per
// race (verify/dcphys2_20261022). What stays here is the .asi's harness: the trampoline, the A/B and the entry.
// ===========================================================================
namespace {


// ═══════════════════════════════════════════════════════════════════════════
// A5 IN-PROCESS BIT-IDENTITY SELF-TEST (env MASHED_PHYS_C4_SELFTEST)
// ───────────────────────────────────────────────────────────────────────────
// A5's WHOLE function is the body (no callee-dispatch tail like A4); the original is reached through
// OrigA5Trampoline. The hazards:
//   (1) SHARED globals: A5 writes the scratch region 0x00881560..0x00881590
//       (48 b). Snapshot + restore it (with the record) so both impls see
//       identical input state.
//   (2) RNG: the random-surface branch (key 0xffff32ff) consumes FUN_00472650
//       twice, writing +0xb00/+0xb08. Running the original first advances the
//       live PRNG; a second run would read a DIFFERENT sequence. [dcphys2] The PRNG read cursor
//       *(DAT_007dc578 + 4 + DAT_007d3ff8) is saved and put back before every run, so the call is
//       compared in full (rng=1 flags it).
// Single-threaded physics → snapshot/restore is safe.
// ═══════════════════════════════════════════════════════════════════════════
namespace {

inline int A5_SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_PHYS_C4_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}

// PRNG read-cursor (FUN_00534870): *(int*)(*(int*)0x7dc578 + 4 + *(int*)0x7d3ff8).
// Returns 0 if the PRNG hasn't been initialised yet (base ptr null).
inline std::uintptr_t A5_PrngCursor() {
    int base = *reinterpret_cast<int*>(0x007dc578);
    int off  = *reinterpret_cast<int*>(0x007d3ff8);
    if (base == 0) return 0;
    return *reinterpret_cast<std::uintptr_t*>(static_cast<char*>(0) + base + 4 + off);
}

void A5_SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("phys_c4_a5_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

// Re-run the ORIGINAL A5 body, bypassing OUR inline-JMP at 0x0046ddb0. The 5-byte
// JMP RH_ScopedInstall writes overwrites the prologue `MOV EAX,[ESP+8]`(4) + first
// byte of `SUB ESP,0x70`. Re-execute those two whole instructions then JMP to the
// first untouched instruction at 0x0046ddb7 (PUSH EBX). Entered (via Call_OrigA5)
// with the SAME ABI as the original: EDI=record; [ESP]=ret [ESP+4]=dt [ESP+8]=xform.
__declspec(naked) void OrigA5Trampoline() {
    __asm {
        mov  eax, dword ptr [esp+8]   // 8b442408  (instr 1; EAX=xform — MUST reach 0x0046ddb7)
        sub  esp, 0x70                // 83ec70    (instr 2)
        // jump to 0x0046ddb7 (PUSH EBX) WITHOUT clobbering EAX: the original pushes
        // EAX as the transform mtx at 0x0046ddba, so a `mov reg,target;jmp reg` that
        // touched EAX would feed a garbage matrix. push-imm + ret jumps clobber-free.
        push 0x0046ddb7
        ret
        // the original body runs to its own `ADD ESP,0x70; RET` (0x0046e9cd/d0),
        // returning to Call_OrigA5's `add esp,8`.
    }
}
// self-test forwarder: invoke the ORIGINAL A5 (not our hook) with EDI=record.
__declspec(naked) void Call_OrigA5(int* /*record*/, float /*dt*/, void* /*xform*/) {
    __asm {
        push edi
        mov  edi, dword ptr [esp+8]   // record -> EDI
        push dword ptr [esp+16]       // xform  ([esp+12]+4 for pushed edi)
        push dword ptr [esp+16]       // dt
        call OrigA5Trampoline
        add  esp, 8
        pop  edi
        ret
    }
}

// the record fields A5 writes that the telemetry observes + the shared scratch.
// Output offsets (byte) checked per call (record-relative):
const int kA5RecOffs[] = {
    0x9b0,0x9b4,0x9b8,            // linear velocity (drive-drag + gravity)
    0x9d4,0x9d8,0x9dc,            // world forward
    0x9e0,                        // grounded count
    0x20c,0x2d0,0x394,0x458,      // per-wheel steer torque (self[0x83]/0xb4/0xe5/0x116)
    0xb00,0xb08,                  // random impulse (RNG-derived; conditionally compared)
    0xb18,0xb20,                  // airborne damp counter + airborne flag
    0xad0,                        // ping-pong ring index
};
// suspension-force vectors per wheel — write outputs. pfVar14 = (float*)(self+0x7d)
// = byte +0x1f4; pfVar14[8] (susp force X) = +0x214; +0xC4 (0x31 ints) per wheel.
//   w0=+0x214 w1=+0x2d8 w2=+0x39c w3=+0x460  (X; Z = +8). Matches vehicle.md §3 (+0xa8).
const int kA5WheelOffs[] = { 0x214, 0x2d8, 0x39c, 0x460 };

int   g_a5Count = 0;
int   g_a5RngHits = 0;
int   g_a5Gnd = 0;      // all-4-grounded (grounded==4.0) baseline calls
int   g_a5Air = 0;      // airborne (grounded != 4.0) — TARGET branch
const int kA5MaxGnd = 48;
const int kA5MaxAir = 48;   // reserve a dedicated quota for the airborne susp path
// WS-PHYS-COVERAGE-SCENARIO: A5's airborne branch (grounded != 4.0 -> the
// divide-by-dt suspension path + airborne-flag) and the random-surface RNG branch
// (key 0xffff32ff) were unhit when only the all-grounded player car was sampled. The
// per-frame dispatcher invokes A5 for EVERY car; AI opponents (slots 1-3) leave the
// ground on Arctic's terrain, so their records drive the airborne branch. A dedicated
// airborne quota (independent of the grounded baseline) lets a longer race fill it.
// [dcphys2 2026-10-22] PRNG read cursor (the word A5_PrngCursor reads), saved and put back before every run so a
// random-surface call (key 0xffff32ff, FUN_00472650 x2 at 0x0046e7ac / 0x0046e7c3) draws the same two values in each.
inline std::uintptr_t* A5_PrngCursorPtr() {
    int base = *reinterpret_cast<int*>(0x007dc578);
    int off  = *reinterpret_cast<int*>(0x007d3ff8);
    if (base == 0) return nullptr;
    return reinterpret_cast<std::uintptr_t*>(static_cast<char*>(0) + base + 4 + off);
}
inline int A5_SelfTestCap() {   // [dcphys2] MASHED_PHYS_SELFTEST_CAP, else MASHED_A6A_SELFTEST_CAP, else 1
    static int c = -1;
    if (c < 0) {
        const char* s = std::getenv("MASHED_PHYS_SELFTEST_CAP");
        if (!(s && std::atoi(s) > 0)) s = std::getenv("MASHED_A6A_SELFTEST_CAP");
        c = (s && std::atoi(s) > 0) ? std::atoi(s) : 1;
    }
    return c;
}
void A5_SelfTest(int* record, float dt, void* xform) {
    // [dcphys2 2026-10-22] upgraded the way dcphys upgraded A6a: per sampled call, snapshot (record, scratch
    // 0x00881560..0x0088158f, PRNG cursor), the ORIGINAL, the ORIGINAL again from the snapshot (noo), then the SHARED
    // body (Vehicle/ForceIntegrator.cpp), whose output stays live. The compare is the whole 0xd04 record plus the 12 scratch dwords. Quotas x
    // MASHED_PHYS_SELFTEST_CAP. Past quota the shared body runs (the old `return` skipped A5 for that call).
    const std::uint32_t groundedBits = *reinterpret_cast<std::uint32_t*>((char*)record + 0x9e0);
    const bool airborne = (groundedBits != 0x40800000u);   // != 4.0f
    const int cap = A5_SelfTestCap();
    const bool sample = airborne ? (g_a5Air < kA5MaxAir * cap) : (g_a5Gnd < kA5MaxGnd * cap);
    if (!sample) { mashed_re::Vehicle::VehicleWheelForceIntegrate(record, dt, xform); return; }

    static std::uint8_t snapRec[0xd04], origRec[0xd04], o2Rec[0xd04];
    static std::uint8_t snapScr[0x30], origScr[0x30], o2Scr[0x30];
    std::uint8_t* scr = reinterpret_cast<std::uint8_t*>(0x00881560);
    std::uintptr_t* cur = A5_PrngCursorPtr();
    const std::uintptr_t cur0 = cur ? *cur : 0;
    std::memcpy(snapRec, record, 0xd04);
    std::memcpy(snapScr, scr, 0x30);
    auto restore = [&]() { std::memcpy(record, snapRec, 0xd04); std::memcpy(scr, snapScr, 0x30); if (cur) *cur = cur0; };
    auto ndw = [](const std::uint8_t* x, const std::uint8_t* y, int n) {
        int d = 0; for (int off = 0; off < n; off += 4)
            if (*reinterpret_cast<const std::uint32_t*>(x + off) != *reinterpret_cast<const std::uint32_t*>(y + off)) d++;
        return d; };

    Call_OrigA5(record, dt, xform);                                        // (1) original
    const bool rngFired = cur && (*cur != cur0);
    std::memcpy(origRec, record, 0xd04); std::memcpy(origScr, scr, 0x30);
    restore(); Call_OrigA5(record, dt, xform);                             // (2) original again: noo
    std::memcpy(o2Rec, record, 0xd04); std::memcpy(o2Scr, scr, 0x30);
    restore(); mashed_re::Vehicle::VehicleWheelForceIntegrate(record, dt, xform);   // (4) shared, live

    const int noo = ndw(o2Rec, origRec, 0xd04) + ndw(o2Scr, origScr, 0x30);
    int mism = 0; char line[1024]; int p = 0;
    for (int off = 0; off < 0xd04; off += 4) {
        const std::uint32_t ov = *reinterpret_cast<const std::uint32_t*>(origRec + off);
        const std::uint32_t mv = *reinterpret_cast<const std::uint32_t*>((char*)record + off);
        if (mv != ov) { mism++; if (p < 900) p += wsprintfA(line+p, " +0x%x:o=%08x,m=%08x", off, ov, mv); }
    }
    for (int i = 0; i < 12; ++i) {
        const std::uint32_t ov = reinterpret_cast<const std::uint32_t*>(origScr)[i];
        const std::uint32_t mv = reinterpret_cast<const std::uint32_t*>(scr)[i];
        if (mv != ov) { mism++; if (p < 900) p += wsprintfA(line+p, " scr[%d]:o=%08x,m=%08x", i, ov, mv); }
    }
    if (rngFired) g_a5RngHits++;
    if (airborne) g_a5Air++; else g_a5Gnd++;

    char hdr[260];
    int grounded = *reinterpret_cast<int*>((char*)record + 0x9e0);
    int slot = (int)(((std::uintptr_t)record - 0x008815a0) / 0xd04);
    wsprintfA(hdr, "[%d] slot=%d dt=%08x grounded=%08x air=%d rng=%d noo=%d ndiff=%d%s\r\n",
              g_a5Count, slot, *reinterpret_cast<std::uint32_t*>(&dt), grounded,
              airborne ? 1 : 0, rngFired ? 1 : 0, noo, mism, mism ? "" : " OK");
    A5_SelfTestLog(hdr);
    if (mism) { line[p] = 0; A5_SelfTestLog("   "); A5_SelfTestLog(line); A5_SelfTestLog("\r\n"); }
    g_a5Count++;
}

// ── entry trampoline installed at 0x0046ddb0 ────────────────────────────────
// A5 ABI: EDI=record; [ESP+4]=dt; [ESP+8]=xform; caller-cleans (saves EBX/EBP/ESI,
// `ret` no-imm). The trampoline preserves the same callee-saved set + EDI and
// forwards to A5_Hook (a normal C fn), which runs the A/B (self-test on) or the shared body.
void A5_Hook(int* record, float dt, void* xform) {
    if (A5_SelfTestEnabled())
        A5_SelfTest(record, dt, xform);
    else
        mashed_re::Vehicle::VehicleWheelForceIntegrate(record, dt, xform);   // [dcphys2] the shared body
}

__declspec(naked) void A5_Entry() {
    __asm {
        // EDI=record; [esp]=ret [esp+4]=dt [esp+8]=xform
        push ebx
        push ebp
        push esi
        push edi                  // preserve A5's callee-saved set + EDI; 4 saves
        push dword ptr [esp+24]   // xform   ([esp+8]+16 for the 4 saves)
        push dword ptr [esp+24]   // dt
        push edi                  // record (EDI)
        call A5_Hook
        add  esp, 12              // cdecl: clean our 3 forwarded args
        pop  edi
        pop  esi
        pop  ebp
        pop  ebx
        ret                       // caller cleans A5's 2 (effective) stack args
    }
}

}  // namespace

}  // namespace

// ===========================================================================
// A6a — FUN_00467650 the velocity / angular-velocity integration step
// (ESI=record; param_1, dt=param_2, param_3 unused, input=param_4).
// [dcphys 2026-10-21] ONE body, shared with mashed_re.exe: Vehicle/Integrate2.cpp (Vehicle_Integrate2). The
// verbatim naked-x87 transcription that lived here (C4 72/72, with 14 float10 / _ftol2 shims) was deleted after
// the shared body read bit-identical to it on every sampled call (nvs = 0 on 2 x 1376 calls, track 0) and, with
// MASHED_A6A_DOT_ORDER=1, bit-identical to the original on every sampled call where the verbatim copy was not
// (verify/dcphys_20261021). What stays here is the .asi's harness: the trampoline into the original, the A/B
// (full 0xd04 record + a no-op control) and the entry.
// ===========================================================================
namespace {

// ═══════════════════════════════════════════════════════════════════════════
// A6a IN-PROCESS BIT-IDENTITY SELF-TEST (env MASHED_PHYS_C4_SELFTEST)
// ───────────────────────────────────────────────────────────────────────────
// Same architecture as A4/A5: A6a's WHOLE function is the body (no callee-dispatch
// tail), so the "original-body" run is a forwarded call to the LIVE original
// (Call_OrigA6a), bypassing our inline-JMP. The 5-byte JMP at 0x00467650 overwrites
// the first 5 bytes of `SUB ESP,0xe4` (81 ec e4 00 00 | 00). OrigA6a re-executes the
// whole `SUB ESP,0xe4` then `push 0x00467656; ret` (clobber-free; PRESERVES ESI=record
// which the body reads as unaff_ESI). Hazards:
//   - SHARED global _DAT_0088e5f0 (susp scale) is READ-only here (set by FUN_00470c70),
//     so no snapshot needed; the live PRNG is NOT touched by A6a (no FUN_00472650).
//   - A6a calls FUN_004a2c48 (round ST0). Its inner state is a pure rounding-mode
//     read (no global write the second run would see differently) — running it twice
//     on identical ST0 yields identical EAX, so no exclusion needed (verified by the
//     A/B itself reaching 0 mismatches; if any +0x494/+0xbf4/wheel[0] differs it is
//     reported, not masked).
// Single-threaded physics → snapshot/restore the record is sufficient.
// ═══════════════════════════════════════════════════════════════════════════
inline int A6a_SelfTestEnabled() {
    static int vv = -1;
    if (vv < 0) { const char* s = std::getenv("MASHED_PHYS_C4_SELFTEST"); vv = (s && s[0]) ? 1 : 0; }
    return vv;
}
void A6a_SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("phys_c4_a6a_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

// re-run the ORIGINAL A6a body bypassing our inline-JMP at 0x00467650.
__declspec(naked) void OrigA6aTrampoline() {
    __asm {
        sub  esp, 0xe4                 // 81ece4000000 (the full overwritten instr)
        push 0x00467656                // -> PUSH EBX (first untouched byte), ESI preserved
        ret
    }
}
// self-test forwarder: invoke the ORIGINAL A6a with ESI=record + 4 cdecl stack args.
__declspec(naked) void Call_OrigA6a(int* /*record*/, int /*param_1*/, float /*dt*/,
                                    void* /*param_3*/, void* /*input*/) {
    __asm {
        push esi
        mov  esi, dword ptr [esp+8]    // record -> ESI
        push dword ptr [esp+24]        // input   (arg5; [esp+20]+4 for pushed esi)
        push dword ptr [esp+24]        // param_3 (arg4)
        push dword ptr [esp+24]        // dt      (arg3)
        push dword ptr [esp+24]        // param_1 (arg2)
        call OrigA6aTrampoline
        add  esp, 16
        pop  esi
        ret
    }
}

// the record fields A6a writes that the telemetry observes.
const int kA6aRecOffs[] = {
    0x9b0, 0x9b4, 0x9b8,           // linear velocity
    0x9bc, 0x9c0, 0x9c4,           // angular velocity
    0x9e4, 0x9e8,                  // linear/angular speed magnitude
    0x490, 0x494,                  // gear index + shift timer
    0xb14, 0xb18, 0xb1c,           // torque/boost-force accumulator
    0xb20,                         // torque-active flag
    0xbf4, 0xbf8,                  // boost timer + boost state
};
// per-wheel writes: piVar12=self+0x1a4; piVar12[0]=*piVar12 (byte +0x1a4), piVar12[-1]
// (byte +0x1a0), and the per-wheel force vec piVar12[0x1c..0x1e] (byte +0x214..). Wheel
// stride 0xC4. byte bases: *p w0=+0x1a4; p[-1] w0=+0x1a0; force w0=+0x214 (X), +8 (Z).
const int kA6aWheelStar[]  = { 0x1a4, 0x268, 0x32c, 0x3f0 };   // *piVar12 (round result)
const int kA6aWheelPrev[]  = { 0x1a0, 0x264, 0x328, 0x3ec };   // piVar12[-1]
const int kA6aWheelFx[]    = { 0x214, 0x2d8, 0x39c, 0x460 };   // force X (Z=+8)

int g_a6aCount = 0;
int g_a6aIdle  = 0;     // coast (no throttle/brake)
int g_a6aAccel = 0;     // throttle (input[4]!=0)
int g_a6aBrake = 0;     // BRAKE branch (input[5]!=0 && input[4]==0) — TARGET
int g_a6aAir   = 0;     // airborne (grounded != 4.0)               — TARGET
const int kA6aMaxIdle  = 8;
const int kA6aMaxAccel = 32;
const int kA6aMaxBrake = 32;
const int kA6aMaxAir   = 32;
// WS-PHYS-COVERAGE-SCENARIO: A6a's brake branch (input[5]!=0 && input[4]==0 ->
// BrakeForceAccum + the wheel>1 brake-quantize call#7) and the airborne path
// (grounded != 4.0) were unhit in the throttle-only player demo. The per-frame
// dispatcher invokes A6a for EVERY car; the AI opponents brake into corners and
// leave the ground, so their records exercise both. Independent per-branch quotas
// keep the more-numerous accel/coast calls from filling the cap first. (A6a's known
// susp-force float10 residual is on the GROUNDED accel path and is unaffected.)
// [dcphys 2026-10-21] MASHED_A6A_SELFTEST_CAP=<n> multiplies every per-branch quota (default 1). The default
// quotas fill within the first ~100 calls of a race, which is how the 72/72 C4 sample missed the three-dot
// residual a x20 sample shows (verify/dcphys_20261021).
inline int A6a_SelfTestCap() {
    static int c = -1;
    if (c < 0) {   // [dcphys2] MASHED_PHYS_SELFTEST_CAP sets every physics self-test's cap; the A6a name still works
        const char* s = std::getenv("MASHED_PHYS_SELFTEST_CAP");
        if (!(s && std::atoi(s) > 0)) s = std::getenv("MASHED_A6A_SELFTEST_CAP");
        c = (s && std::atoi(s) > 0) ? std::atoi(s) : 1;
    }
    return c;
}
void A6a_SelfTest(int* record, int param_1, float dt, void* param_3, std::uint8_t* input) {
    const bool brake = (input[5] != 0) && (input[4] == 0);
    const bool accel = (input[4] != 0);
    const bool idle  = (input[4] == 0) && (input[5] == 0);
    const std::uint32_t groundedBits = *reinterpret_cast<std::uint32_t*>((char*)record + 0x9e0);
    const bool airborne = (groundedBits != 0x40800000u);
    const int cap = A6a_SelfTestCap();
    bool admit = false;
    if (airborne && g_a6aAir   < kA6aMaxAir   * cap) admit = true;
    if (brake    && g_a6aBrake < kA6aMaxBrake * cap) admit = true;
    if (accel    && g_a6aAccel < kA6aMaxAccel * cap) admit = true;
    if (idle     && g_a6aIdle  < kA6aMaxIdle  * cap) admit = true;
    // [dcphys2] past quota the shared body runs (the old `return` skipped A6a for that call, so every later sample
    // started from a record that had missed A6a on the skipped frames)
    if (!admit) { mashed_re::Vehicle::Vehicle_Integrate2(record, param_1, dt, param_3, input); return; }
    if (airborne) ++g_a6aAir;
    if (brake)    ++g_a6aBrake;
    if (accel)    ++g_a6aAccel;
    if (idle)     ++g_a6aIdle;

    static std::uint8_t snapRec[0xd04];
    std::memcpy(snapRec, record, 0xd04);

    // (1) ORIGINAL A6a on the live record (bypassing our inline-JMP).
    Call_OrigA6a(record, param_1, dt, param_3, input);

    // capture the original's outputs: the WHOLE record ([dcphys] the original A6a stores nothing outside
    // it, so a 0xd04-byte compare covers every output; the kA6a* lists above were a subset)
    static std::uint8_t origFull[0xd04];
    std::memcpy(origFull, record, 0xd04);

    // [dcphys 2026-10-21] no-op control: the ORIGINAL again from the same snapshot (noo). noo != 0 means the
    // original read state the snapshot does not hold, and that call's verdict is void. Then the shared body.
    static std::uint8_t orig2Full[0xd04];
    std::memcpy(record, snapRec, 0xd04);
    Call_OrigA6a(record, param_1, dt, param_3, input);
    std::memcpy(orig2Full, record, 0xd04);
    int noo = 0;
    for (int off = 0; off < 0xd04; off += 4)
        if (*reinterpret_cast<const std::uint32_t*>(orig2Full + off) != *reinterpret_cast<const std::uint32_t*>(origFull + off)) noo++;
    std::memcpy(record, snapRec, 0xd04);
    mashed_re::Vehicle::Vehicle_Integrate2(record, param_1, dt, param_3, input);
    char flags[96];   // which of the shared body's branches ran (its diag sinks, Vehicle/ForceIntegrator.h)
    {   using namespace mashed_re::Vehicle;
        wsprintfA(flags, " fired=%d%d%d%d spin=%d%d%d%d hi=%d%d%d%d arm=%d",
                  g_a6aDump[0].fired, g_a6aDump[1].fired, g_a6aDump[2].fired, g_a6aDump[3].fired,
                  g_a6aDump[0].spin, g_a6aDump[1].spin, g_a6aDump[2].spin, g_a6aDump[3].spin,
                  (g_a6aDump[0].pm1 >> 8) & 1, (g_a6aDump[1].pm1 >> 8) & 1, (g_a6aDump[2].pm1 >> 8) & 1,
                  (g_a6aDump[3].pm1 >> 8) & 1, g_a6aFrame.arm); }

    int mism = 0; char line[1024]; int p = 0;
    for (int off = 0; off < 0xd04; off += 4) {
        const std::uint32_t ov = *reinterpret_cast<const std::uint32_t*>(origFull + off);
        const std::uint32_t mv = *reinterpret_cast<const std::uint32_t*>((char*)record + off);
        if (mv != ov) { mism++;
            if (p < 900) p += wsprintfA(line+p, " +0x%x:o=%08x,m=%08x", off, ov, mv); }
    }

    char hdr[400];
    int grounded = *reinterpret_cast<int*>((char*)record + 0x9e0);
    int slot = (int)(((std::uintptr_t)record - 0x008815a0) / 0xd04);
    wsprintfA(hdr, "[%d] slot=%d dt=%08x in4=%u in5=%u gm=%d grounded=%08x air=%d noo=%d%s ndiff=%d%s\r\n",
              g_a6aCount, slot, *reinterpret_cast<std::uint32_t*>(&dt), input[4], input[5],
              Fi_GameMode(), grounded, airborne ? 1 : 0, noo, flags, mism, mism ? "" : " OK");
    A6a_SelfTestLog(hdr);
    if (mism) { line[p] = 0; A6a_SelfTestLog("   "); A6a_SelfTestLog(line); A6a_SelfTestLog("\r\n"); }
    g_a6aCount++;
}

// ── entry trampoline installed at 0x00467650 ────────────────────────────────
// A6a ABI: ESI=record; [esp+4]=param_1 [esp+8]=dt [esp+0xc]=param_3 [esp+0x10]=input;
// caller-cleans (saves EBX/EBP/EDI, `ret` no-imm). The dispatcher's Call_A6a preserves
// ESI itself, but we preserve it too (and EBX/EBP/EDI) so the caller sees them intact.
void A6a_Hook(int* record, int param_1, float dt, void* param_3, std::uint8_t* input) {
    if (A6a_SelfTestEnabled())
        A6a_SelfTest(record, param_1, dt, param_3, input);
    else
        mashed_re::Vehicle::Vehicle_Integrate2(record, param_1, dt, param_3, input);
}

__declspec(naked) void A6a_Entry() {
    __asm {
        // ESI=record; [esp]=ret [esp+4]=param_1 [esp+8]=dt [esp+0xc]=param_3 [esp+0x10]=input
        push ebx
        push ebp
        push edi
        push esi                  // preserve A6a's callee-saved set + ESI; 4 saves
        push dword ptr [esp+32]   // input    ([esp+0x10]+16 for the 4 saves)
        push dword ptr [esp+32]   // param_3
        push dword ptr [esp+32]   // dt
        push dword ptr [esp+32]   // param_1
        push esi                  // record (ESI)
        call A6a_Hook
        add  esp, 20              // cdecl: clean our 5 forwarded args
        pop  esi
        pop  edi
        pop  ebp
        pop  ebx
        ret                       // caller cleans A6a's 4 stack args
    }
}

}  // namespace

// ===========================================================================
// A6b — FUN_00468980 the aerodynamic-stabilization step (ECX=record, ESI=xform,
// [ESP+4]=dt=param_2). VERBATIM transcription of 0x00468980..0x00468b34 (disasm
// Mashed_pool12 RO 2026-06-17). Runs ONLY when grounded count +0x9e0 == 0.0
// (airborne). Two sub-branches on motion-state +0x9f0:
//   == 0 : auto-level. Two RwMatrixRotate (FUN_004c4d20) passes preconcat onto the
//          world xform: pitch about world X (axis _DAT_006146f0=(1,0,0)) by
//          ang1 = -(acos(clamp(at.y))*180/pi - 90)*dts; roll about world Z
//          (axis _DAT_00614708=(0,0,1)) by ang2 = (acos(clamp(right.y))*180/pi
//          - 90)*dts.  dts = dt*0.001 (_DAT_005cc558). 90 is _DAT_005ccad8 (double)
//          for ang1 and _DAT_005ccad0 (float) for ang2; 180/pi is _DAT_005ccae0
//          (double, 0x404ca5dcc0000000). at.y/right.y are read as a dot of xform
//          row {idx8/9/10} resp. {0/1/2} with the Y basis _DAT_006146fc=(0,1,0).
//   != 0 : zero ang-vel (+0x9bc/0x9c0/0x9c4), normalize linear-vel (FUN_004c39b0)
//          to a dir, then RwMatrixRotate(xform, dir, dt*0.05, 1) (_DAT_005cc9a0).
//
// ABI: read at the A4 dispatch site 0x00470943 — MOV ECX,EDI (record);
// MOV ESI,[ESP+0x3c]; PUSH EBP(input,UNUSED); PUSH EBX(dt). So A6b is effectively
// __thiscall-with-ESI: ECX=record, ESI=<the matrix>, one float stack arg (dt).
//
// WHICH matrix — CORRECTED 2026-09-29 (U-9149). This block used to say ESI is
// "A4's param_4 world matrix". It is not. A4's frame is 5 prologue pushes
// (0x00470670..0x00470678) with NO `sub esp`, so at 0x0047093b — after A5's 3 and
// A6a's 4 pushed args — ESP is E-0x30 and [ESP+0x3c] is E+0x0c. A4's param_4 is at
// E+0x10. E+0x0c is A4's param_3 SLOT, overwritten at 0x004706a2 with
// `LEA ECX,[EAX+EDI+0x928]` (EAX = [EDI+0x9a8]<<6). So ESI = wheelBlock — the same
// pointer A5 receives as arg 2 (MOV EDX,[ESP+0x20] @0x00470918) and A6a as arg 3
// (MOV EAX,[ESP+0x2c] @0x00470928). Cross-check: ADD ESP,0x24 at 0x0047094e
// balances exactly 3+4+2 = 9 pushed dwords, and 5 pops restore the 5 prologue
// pushes, so the frame is fully accounted for with no hidden allocation.
// Corroboration that param_4 is not the matrix: a balanced ESP walk through A5
// (0x0046ddb0 -> RET 0x0046e9d0) finds caller-frame references at E5+0x4 (dt, 6x)
// and E5+0x8 (the matrix, 2x: 0x0046ddb0 and 0x0046de01, both feeding
// RwV3dTransformVectors) and NONE at E5+0xc (param_4).
// A4_Body's Call_A6b set ESI=record; that latent dispatch bug is FIXED (it now
// passes wheelBlock). A4's C4 evidence came from its body-math self-test, which
// stops BEFORE the dispatch tail, so it never covered this.
//
// [dcphys2 2026-10-22] ONE body, shared with mashed_re.exe: Vehicle/AeroStabilize.cpp (Vehicle_AeroStabilize). The
// naked transcription that lived here (A6b_BodyAsm, C4) was deleted after the shared body, with MASHED_A6B_ACOS_CRT and
// MASHED_A6B_NORM_LUT (seeded here), read bit-identical to the original on every sampled call of this file's A/B
// apart from the matrix pad words (verify/dcphys2_20261022). What stays here is the .asi's harness.
// ===========================================================================
namespace {

// ── self-test forwarder: invoke the ORIGINAL A6b bypassing our inline-JMP ────
// The 5-byte JMP at 0x00468980 overwrites bytes 0x00468980..0x00468984 — the first FIVE of
// the six bytes of `FLD float ptr [ECX+0x9e0]` (d9 81 e0 09 00 00). Verified against the
// binary 2026-07-28:
//     00468980  d981e0090000  FLD   float ptr [ECX + 0x9e0]     <- 6 bytes, 0x468980..0x468985
//     00468986  83ec20        SUB   ESP,0x20                    <- first INTACT instruction
// CORRECTION 2026-07-28: this comment previously also claimed the "first byte of
// `SUB ESP,0x20`" was clobbered. That is impossible — a 5-byte JMP at 0x00468980 cannot reach
// 0x00468986. Byte 0x00468985 (the FLD's last byte) is likewise untouched. The CODE was always
// right (re-exec the whole FLD, resume at 0x00468986); only the description over-reported the
// clobbered region, and a wrong plate is what propagates into the next port.
// Re-execute the whole FLD then `push 0x00468986; ret` (clobber-free; preserves ECX=record +
// ESI=xform). The original body runs to its own `ADD ESP,0x20; RET`.
extern "C" void* PCH_Fwd_468980;
__declspec(naked) void OrigA6bTrampoline(/* ECX=record, ESI=xform, [esp+4]=dt */) {
    __asm {
        fld  dword ptr [ecx+0x9e0]     // d981e0090000 (the full overwritten instr)
        push 0x00468986                // -> SUB ESP,0x20 (first untouched byte)
        ret
    }
}
// Call the original A6b with ECX=record, ESI=xform, dt on the stack.
__declspec(naked) void Call_OrigA6b(int* /*record*/, void* /*xform*/, float /*dt*/) {
    __asm {
        push esi
        mov  ecx, dword ptr [esp+8]    // record -> ECX  ([esp+4]+4 for pushed esi)
        mov  esi, dword ptr [esp+12]   // xform  -> ESI
        push dword ptr [esp+16]        // dt (param_2)
        call OrigA6bTrampoline
        add  esp, 4
        pop  esi
        ret
    }
}
inline int A6b_SelfTestEnabled() {
    static int vv = -1;
    if (vv < 0) { const char* s = std::getenv("MASHED_PHYS_C4_SELFTEST"); vv = (s && s[0]) ? 1 : 0; }
    return vv;
}
void A6b_SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("phys_c4_a6b_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

// observed outputs: the angular-velocity it zeroes (state!=0 branch) and the 16-float
// xform matrix that FUN_004c4d20 rotates IN PLACE (the principal observable). The
// xform is the per-vehicle world matrix; A6b preconcats a rotation onto it.
const int kA6bRecOffs[] = { 0x9bc, 0x9c0, 0x9c4 };

int g_a6bCount = 0;
int g_a6bAir   = 0;     // calls that actually entered the airborne body (the TARGET branch)
int g_a6bGnd   = 0;     // grounded no-op calls (gate-only)
const int kA6bMaxAir = 64;   // reserve most slots for the airborne aero body
const int kA6bMaxGnd = 8;    // a few grounded no-op rows only

// WS-PHYS-COVERAGE-SCENARIO: the airborne aero body (+0x9e0==0.0) is the function's
// real work; in a throttle-only player-car demo the player never leaves the ground,
// but the per-frame dispatcher (FUN_00470c70) invokes A6b for EVERY car record, and
// the AI opponents (slots 1-3) brake/steer/jump on their own — so an AI car that
// flies off Arctic's terrain drives this hook airborne. The sampler now keeps a large
// dedicated AIRBORNE quota (independent of the grounded-no-op quota) so a longer race
// fills it from whichever car (player or AI) actually goes airborne.
void A6b_SelfTest(int* record, void* xform, float dt) {
    // [dcphys2 2026-10-22] upgraded the way dcphys upgraded A6a: per sampled call, snapshot, the ORIGINAL, the
    // ORIGINAL again from the snapshot (noo, the no-op control), then the SHARED body (Vehicle/AeroStabilize.cpp),
    // whose output stays live. The compare is the
    // whole 0xd04 record plus the 16 matrix floats (the matrix lies inside the record at +0x928 + ring*0x40, but the
    // pointer is the caller's and is compared on its own). Quotas x MASHED_PHYS_SELFTEST_CAP. Past quota the shared
    // body runs (never skip the function). The original writes nothing outside the record and the matrix
    // (0x00468980..0x00468b34 has no absolute store; its leaves FUN_004a3384 / FUN_004c4d20 / FUN_004c39b0 write
    // the matrix, the dir copy on the stack, and FUN_004c39b0's error path only when |v| <= 0).
    const bool airborne = (*reinterpret_cast<float*>((char*)record + 0x9e0) == 0.0f);
    const bool haveXform = (xform != nullptr);
    const int cap = A6a_SelfTestCap();
    const bool sample = airborne ? (g_a6bAir < kA6bMaxAir * cap) : (g_a6bGnd < kA6bMaxGnd * cap);
    if (!sample) { mashed_re::Vehicle::Vehicle_AeroStabilize(record, static_cast<float*>(xform), dt); return; }

    static std::uint8_t snapRec[0xd04], origRec[0xd04], o2Rec[0xd04];
    static std::uint8_t snapXf[64], origXf[64], o2Xf[64];
    std::memcpy(snapRec, record, 0xd04);
    if (haveXform) std::memcpy(snapXf, xform, 64);
    auto restore = [&]() { std::memcpy(record, snapRec, 0xd04); if (haveXform) std::memcpy(xform, snapXf, 64); };
    auto ndw = [](const std::uint8_t* a, const std::uint8_t* b, int n) {
        int d = 0; for (int off = 0; off < n; off += 4)
            if (*reinterpret_cast<const std::uint32_t*>(a + off) != *reinterpret_cast<const std::uint32_t*>(b + off)) d++;
        return d; };

    Call_OrigA6b(record, xform, dt);                                       // (1) original
    std::memcpy(origRec, record, 0xd04); if (haveXform) std::memcpy(origXf, xform, 64);
    restore(); Call_OrigA6b(record, xform, dt);                            // (2) original again: noo
    std::memcpy(o2Rec, record, 0xd04); if (haveXform) std::memcpy(o2Xf, xform, 64);
    restore(); mashed_re::Vehicle::Vehicle_AeroStabilize(record, static_cast<float*>(xform), dt);   // (4) shared, live

    const int noo = ndw(o2Rec, origRec, 0xd04) + (haveXform ? ndw(o2Xf, origXf, 64) : 0);
    // [dcphys2] the matrix's three pad words (m[7] / m[11] / m[15], RwMatrix pad1..3) are counted as npad, not ndiff.
    // FUN_004c4d20 leaves them holding whatever its stack held (the original's values on the first boosted run:
    // 0x001af78c, a stack address, and 0x004a7070, a return address inside the CRT acos exit path 0x004a7047..), so
    // they follow the caller's stack depth, not a computed value. Their reader FUN_004dc410 (A5's vector transform,
    // device slot +0x14) overwrites lane 3 of every row with lane 2 (SHUFPS 0xa4 at 0x004dc43a / 0x004dc441 /
    // 0x004dc448) and stores three lanes, so it never uses them. noo still counts them.
    const int padRel = haveXform ? (int)((char*)xform - (char*)record) : -1000;
    auto isPadOff = [&](int off) { return off == padRel + 0x1c || off == padRel + 0x2c || off == padRel + 0x3c; };
    int mism = 0, npad = 0; char line[1024]; int p = 0;
    for (int off = 0; off < 0xd04; off += 4) {
        const std::uint32_t ov = *reinterpret_cast<const std::uint32_t*>(origRec + off);
        const std::uint32_t mv = *reinterpret_cast<const std::uint32_t*>((char*)record + off);
        if (mv != ov) {
            if (isPadOff(off)) { npad++; continue; }
            mism++; if (p < 900) p += wsprintfA(line+p, " +0x%x:o=%08x,m=%08x", off, ov, mv); }
    }
    if (haveXform) {
        for (int i = 0; i < 16; ++i) {
            const std::uint32_t ov = reinterpret_cast<const std::uint32_t*>(origXf)[i];
            const std::uint32_t mv = reinterpret_cast<const std::uint32_t*>(xform)[i];
            if (mv != ov) {
                if (i == 7 || i == 11 || i == 15) { npad++; continue; }
                mism++; if (p < 900) p += wsprintfA(line+p, " m[%d]:o=%08x,m=%08x", i, ov, mv); }
        }
    }
    if (airborne) g_a6bAir++;
    else          g_a6bGnd++;

    char hdr[320];
    int state = *reinterpret_cast<int*>((char*)record + 0x9f0);
    // slot = (record - DAT_008815a0) / 0xd04 : 0=player, 1..3=AI opponents
    int slot = (int)(((std::uintptr_t)record - 0x008815a0) / 0xd04);
    // U-9149 DISPATCH WITNESS (2026-09-29). The self-test compares runs that share the SAME ESI, so it can go green
    // on a WRONG ESI (memory: inherited-green-may-be-guard-only). Log the pointer we were handed against the one the
    // original computes at 0x0047068e/0x00470696/0x00470699 and stores to A4's E+0x0c slot at 0x004706a2, the slot
    // 0x0047093b reloads into ESI. With A4 NOT hooked this witnesses the ORIGINAL's dispatch; with A4 hooked, Call_A6b.
    const void* expectXf = (const void*)((char*)record
                            + *reinterpret_cast<int*>((char*)record + 0x9a8) * 0x40 + 0x928);
    wsprintfA(hdr, "[%d] slot=%d dt=%08x air=%d state=%d xform=%d xf=%08x exp=%08x xfok=%d noo=%d npad=%d ndiff=%d%s\r\n",
              g_a6bCount, slot, *reinterpret_cast<std::uint32_t*>(&dt), airborne ? 1 : 0,
              state, haveXform ? 1 : 0, (unsigned)(std::uintptr_t)xform,
              (unsigned)(std::uintptr_t)expectXf, (xform == expectXf) ? 1 : 0,
              noo, npad, mism, mism ? "" : " OK");
    A6b_SelfTestLog(hdr);
    if (mism) { line[p] = 0; A6b_SelfTestLog("   "); A6b_SelfTestLog(line); A6b_SelfTestLog("\r\n"); }
    g_a6bCount++;
}

// ── entry trampoline installed at 0x00468980 ────────────────────────────────
// NAMING (2026-09-29, U-9149): everything in this A6b self-test block calls the
// ESI object `xform`. That name is retained because it is only ever used as an
// opaque 64-byte RwMatrix (snapshot/restore/compare), which is correct — but it is
// NOT A4's param_4. It is A4's wheelBlock, record + [record+0x9a8]*0x40 + 0x928.
// See the U-9149 note above Call_A6b and in the A6b_BodyAsm header.
//
// A6b ABI: ECX=record, ESI=xform, [esp+4]=dt. The original saves only ESI (PUSH ESI
// at 0x468543... actually 0x468985 is SUB; the body PUSHes nothing — it uses ESP-relative
// scratch and `RET` no-imm). We preserve ESI (the caller relies on it) + EBX/EBP/EDI
// for safety and forward to A6b_Hook (a normal C fn) with (record, xform, dt).
void A6b_Hook(int* record, void* xform, float dt) {
    if (A6b_SelfTestEnabled())
        A6b_SelfTest(record, xform, dt);
    else
        mashed_re::Vehicle::Vehicle_AeroStabilize(record, static_cast<float*>(xform), dt);   // [dcphys2] the shared body
}

__declspec(naked) void A6b_Entry() {
    __asm {
        // ECX=record, ESI=xform; [esp]=ret [esp+4]=dt
        push ebx
        push ebp
        push edi
        push esi                  // preserve callee-saved + ESI (xform); 4 saves
        push dword ptr [esp+20]   // dt   ([esp+4]+16 for the 4 saves)
        push esi                  // xform (ESI)
        push ecx                  // record (ECX)
        call A6b_Hook
        add  esp, 12              // cdecl: clean our 3 forwarded args
        pop  esi
        pop  edi
        pop  ebp
        pop  ebx
        ret                       // caller cleans A6b's 1 stack arg (dt)
    }
}

}  // namespace

// ===========================================================================
// A3 — FUN_0046b540 the spawn-time per-vehicle init (__cdecl(int slot)).
// [dcphys 2026-10-21] ONE body, shared with mashed_re.exe: Vehicle/VehicleInit.cpp (VehicleInit). The verbatim
// transcription that lived here (C4 24/24) was deleted after the shared body read bit-identical to the original
// in this file's in-process A/B (full record + the 4 handling globals + return, verify/dcphys_20261021).
// What stays here is the .asi's harness: the trampoline into the original, the A/B, and the entry.
// ===========================================================================
namespace {

inline int&   Gi(std::uintptr_t a) { return *reinterpret_cast<int*>(a); }

// ── self-test forwarder: invoke the ORIGINAL A3 bypassing our inline-JMP ─────
// The 5-byte JMP at 0x0046b540 overwrites `SUB ESP,8`(3, 83ec08) + `PUSH ESI`(1, 56)
// + the FIRST byte of `MOV [0x613108],0x42c80000` (10 bytes, 0x46b544..0x46b54d). So
// the MOV is split by the JMP boundary and CANNOT be resumed mid-instruction:
// re-execute SUB ESP,8 + PUSH ESI + the WHOLE first MOV, then jump to 0x0046b54e
// (the second MOV, first fully-intact instruction). Clobber-free push-imm+ret.
extern "C" void* PCH_Fwd_46b540;
__declspec(naked) void OrigA3Trampoline(/* [esp+4]=slot (after Call_OrigA3's push) */) {
    __asm {
        sub  esp, 8                                  // 83ec08
        push esi                                     // 56
        // re-do the split first MOV [0x613108]=100.0f (EAX dead at 0x46b54e: the
        // next CALL 0x00430790 overwrites it). MSVC can't encode mov [abs],imm.
        mov  eax, 0x00613108
        mov  dword ptr [eax], 0x42c80000
        push 0x0046b54e                              // -> MOV [0x613114],imm (first intact instr)
        ret
    }
}
int Call_OrigA3(int slot) {
    int r;
    __asm {
        push dword ptr slot
        call OrigA3Trampoline
        add  esp, 4
        mov  r, eax
    }
    return r;
}

inline int A3_SelfTestEnabled() {
    static int vv = -1;
    if (vv < 0) { const char* s = std::getenv("MASHED_PHYS_C4_SELFTEST"); vv = (s && s[0]) ? 1 : 0; }
    return vv;
}
void A3_SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("phys_c4_a3_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

int g_a3Count = 0;
const int kA3MaxTests = 64;
// region snapshotted/compared: the full 0xd04 record + the 4 handling globals A3
// writes (0x613108/14/30/3c). The other 0x613110..2c are READ-only (stable),
// excluded. We compare the full record dword-by-dword.
void A3_SelfTest(int slot) {
    if (g_a3Count >= kA3MaxTests) return;
    if ((unsigned)slot >= 0x10) return;     // out-of-range slot returns 0 trivially
    const int o = slot * 0xd04;
    char* rec = reinterpret_cast<char*>(0x008815a0 + o);
    // also snapshot the per-vehicle scalar flags at slot*0x341 stride bases that A3 writes
    static std::uint8_t snapRec[0xd04];
    std::uint32_t snapG[4];
    std::memcpy(snapRec, rec, 0xd04);
    snapG[0]=Gi(0x00613108); snapG[1]=Gi(0x00613114); snapG[2]=Gi(0x00613130); snapG[3]=Gi(0x0061313c);

    // (1) ORIGINAL A3 on the live slot
    int origRet = Call_OrigA3(slot);
    static std::uint8_t origRec[0xd04];
    std::memcpy(origRec, rec, 0xd04);
    std::uint32_t origG[4] = { (std::uint32_t)Gi(0x00613108), (std::uint32_t)Gi(0x00613114),
                               (std::uint32_t)Gi(0x00613130), (std::uint32_t)Gi(0x0061313c) };

    // [dcphys 2026-10-21] no-op control: the ORIGINAL again from the same snapshot (noo). noo != 0 means the
    // original read state the snapshot does not hold, and that call's verdict is void. Then the shared body.
    struct A3State { std::uint8_t rec[0xd04]; std::uint32_t g[4]; int ret; };
    static A3State st[2];   // 0 = orig (taken above), 1 = orig again
    std::memcpy(st[0].rec, origRec, 0xd04); for (int i = 0; i < 4; ++i) st[0].g[i] = origG[i]; st[0].ret = origRet;
    auto restore = [&]() {
        std::memcpy(rec, snapRec, 0xd04);
        Gi(0x00613108)=snapG[0]; Gi(0x00613114)=snapG[1]; Gi(0x00613130)=snapG[2]; Gi(0x0061313c)=snapG[3];
    };
    restore();
    {   const int r = Call_OrigA3(slot);
        std::memcpy(st[1].rec, rec, 0xd04); st[1].ret = r;
        st[1].g[0]=(std::uint32_t)Gi(0x00613108); st[1].g[1]=(std::uint32_t)Gi(0x00613114);
        st[1].g[2]=(std::uint32_t)Gi(0x00613130); st[1].g[3]=(std::uint32_t)Gi(0x0061313c); }
    int noo = 0;
    for (int i = 0; i < 0xd04; i += 4)
        if (*reinterpret_cast<const std::uint32_t*>(st[0].rec + i) != *reinterpret_cast<const std::uint32_t*>(st[1].rec + i)) noo++;
    for (int i = 0; i < 4; ++i) if (st[0].g[i] != st[1].g[i]) noo++;
    if (st[0].ret != st[1].ret) noo++;
    restore();
    const int myRet = mashed_re::Vehicle::VehicleInit(slot, 0);

    // compare full record + the 4 globals + return
    int mism = 0, firstOff = -1; char line[1024]; int p = 0;
    auto* mo = reinterpret_cast<std::uint32_t*>(rec);
    auto* oo = reinterpret_cast<std::uint32_t*>(origRec);
    for (int i = 0; i < 0xd04/4; ++i) {
        if (mo[i] != oo[i]) { mism++; if (firstOff<0) firstOff = i*4;
            if (p < 880) p += wsprintfA(line+p, " +0x%x:o=%08x,m=%08x", i*4, oo[i], mo[i]); }
    }
    std::uint32_t myG[4] = { (std::uint32_t)Gi(0x00613108), (std::uint32_t)Gi(0x00613114),
                             (std::uint32_t)Gi(0x00613130), (std::uint32_t)Gi(0x0061313c) };
    const std::uintptr_t gAddr[4] = {0x613108,0x613114,0x613130,0x61313c};
    for (int i = 0; i < 4; ++i) if (myG[i] != origG[i]) { mism++;
        if (p < 880) p += wsprintfA(line+p, " G%x:o=%08x,m=%08x", (unsigned)gAddr[i], origG[i], myG[i]); }
    if (myRet != origRet) { mism++; if (p < 880) p += wsprintfA(line+p, " ret:o=%d,m=%d", origRet, myRet); }

    char hdr[200];
    wsprintfA(hdr, "[%d] slot=%d noo=%d ndiff=%d firstOff=0x%x%s\r\n",
              g_a3Count, slot, noo, mism, firstOff, mism ? "" : " OK");
    A3_SelfTestLog(hdr);
    if (mism) { line[p]=0; A3_SelfTestLog("   "); A3_SelfTestLog(line); A3_SelfTestLog("\r\n"); }
    g_a3Count++;
}

// ── entry trampoline installed at 0x0046b540 (__cdecl(int slot)) ─────────────
// When the self-test runs it does the in-process A/B (original-vs-mine on the same
// slot) and leaves the record as MINE wrote; the non-self-test path just runs the
// body for real. Either way the body's return is EAX.
int A3_Hook(int slot) {
    if (A3_SelfTestEnabled()) {
        A3_SelfTest(slot);          // runs original then mine; record left as mine
        return mashed_re::Vehicle::VehicleInit(slot, 0);   // once more for the real spawn (deterministic)
    }
    return mashed_re::Vehicle::VehicleInit(slot, 0);
}

__declspec(naked) void A3_Entry() {
    __asm {
        // [esp]=ret [esp+4]=slot ; __cdecl, caller-cleans.
        mov  eax, dword ptr [esp+4]   // slot
        push eax
        call A3_Hook
        add  esp, 4
        ret                            // EAX = VehicleInit's return (1/0)
    }
}

}  // namespace

extern "C" {
    void* PCH_Fwd_4a3384 = reinterpret_cast<void*>(0x004a3384);  // CRT acos(double)->ST0
    void* PCH_Fwd_4c4d20 = reinterpret_cast<void*>(0x004c4d20);  // RwMatrixRotate(out,axis,ang,mode)
    void* PCH_Fwd_4c39b0 = reinterpret_cast<void*>(0x004c39b0);  // Vec3 normalize
    void* PCH_Fwd_4c3b30 = reinterpret_cast<void*>(0x004c3b30);  // RW fast-sqrt LUT (A3 loops)
    void* PCH_Fwd_46b540 = reinterpret_cast<void*>(0x0046b540);  // A3 original (self-test fwd)
}

RH_ScopedInstall(A4_Entry, 0x00470670);
RH_ScopedInstall(A5_Entry, 0x0046ddb0);
RH_ScopedInstall(A6a_Entry, 0x00467650);
RH_ScopedInstall(A6b_Entry, 0x00468980);
RH_ScopedInstall(A3_Entry, 0x0046b540);
