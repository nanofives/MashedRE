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

// ── live-original forward addresses (absolute; MASHED is /FIXED base 0x400000) ─
extern "C" {
    void* PCH_Fwd_4a2c48 = reinterpret_cast<void*>(0x004a2c48);  // round ST0 -> i64
    void* PCH_Fwd_46ddb0 = reinterpret_cast<void*>(0x0046ddb0);  // A5
    void* PCH_Fwd_467650 = reinterpret_cast<void*>(0x00467650);  // A6a
    void* PCH_Fwd_468980 = reinterpret_cast<void*>(0x00468980);  // A6b
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
const float DAT_005d757c  = Cf(0x00000000);  // 0.0
const float _DAT_005cc320 = Cf(0x3f800000);  // 1.0
const float _DAT_005cc32c = Cf(0x3f000000);  // 0.5
const float _DAT_005cd6d4 = Cf(0x42800000);  // 64.0  airborne-flag speed thr
const float _DAT_005ceaa8 = Cf(0x3b800000);  // 1/256 input->force
const float _DAT_005cc950 = Cf(0x3f400000);  // 0.75  boost-gate mul
const float _DAT_005cc9d0 = Cf(0x43000000);  // 128.0 input[5] grip-branch thr
const float _DAT_005ceaa4 = Cf(0x45bb8000);  // 6000.0 filtered-input clamp
const float _DAT_005cea58 = Cf(0x392ec33e);  // ~1/6000 grip scale  (EXACT)
const float _DAT_005cc348 = Cf(0x3fc00000);  // 1.5   high-input branch mul
const float _DAT_005cc9c8 = Cf(0x3f666666);  // 0.9   parked vel damp

// runtime global the torque ring phase indexes (live: DAT_007f101c)
inline int& TorqueRingPhase() { return *reinterpret_cast<int*>(0x007f101c); }

// FUN_0040e350  Fi_GameMode  (__cdecl int(void))
inline int Fi_GameMode() { return reinterpret_cast<int(__cdecl*)()>(0x0040e350)(); }

// ── input smoother: store EAX of FUN_004a2c48(ST0 = (float)*(int*)(rec+off) + dt)
// reproduces A4's exact x87 sequence (FILD dword; FADD float; CALL) bit-for-bit.
__declspec(naked) int SmoothInput(void* /*record*/, int /*off*/, float /*dt*/) {
    __asm {
        mov   eax, dword ptr [esp+4]      // record
        mov   ecx, dword ptr [esp+8]      // off
        fild  dword ptr [eax+ecx]         // ST0 = (float64)(int)record[off]   (A4: FILD [EDI+0xb24])
        fadd  dword ptr [esp+12]          // ST0 += dt                         (A4: FADD [ESP+0x1c])
        call  dword ptr [PCH_Fwd_4a2c48]  // EAX = round(ST0) low32, consumes ST0
        ret
    }
}

// A5 FUN_0046ddb0(dt, wheelBlock, xform) with EDI = record.
// EDI/ESI/EBX/EBP are cdecl callee-saved — preserve across our frame so A4_Body
// (a normal C function calling these) sees them intact.
__declspec(naked) void Call_A5(void* /*record*/, float /*dt*/, int* /*wheelBlock*/, void* /*xform*/) {
    __asm {
        push edi
        mov  edi, dword ptr [esp+8]    // record -> EDI  ([esp+4]+4 for the pushed edi)
        push dword ptr [esp+20]        // xform
        push dword ptr [esp+20]        // wheelBlock
        push dword ptr [esp+20]        // dt
        call dword ptr [PCH_Fwd_46ddb0]
        add  esp, 12
        pop  edi
        ret
    }
}
// A6a FUN_00467650(param_1, dt, wheelBlock, input) with ESI = record.
__declspec(naked) void Call_A6a(void* /*record*/, int /*param_1*/, float /*dt*/, int* /*wheelBlock*/, void* /*input*/) {
    __asm {
        push esi
        mov  esi, dword ptr [esp+8]    // record -> ESI
        push dword ptr [esp+24]        // input
        push dword ptr [esp+24]        // wheelBlock
        push dword ptr [esp+24]        // dt
        push dword ptr [esp+24]        // param_1
        call dword ptr [PCH_Fwd_467650]
        add  esp, 16
        pop  esi
        ret
    }
}
// A6b FUN_00468980(dt, input) with ECX = ESI = record.
__declspec(naked) void Call_A6b(void* /*record*/, float /*dt*/, void* /*input*/) {
    __asm {
        push esi
        mov  ecx, dword ptr [esp+8]    // record -> ECX (A6b reads [ECX+0x9e0])
        mov  esi, ecx                  // record -> ESI (A6b reads [ESI+0x24])
        push dword ptr [esp+16]        // input
        push dword ptr [esp+16]        // dt
        call dword ptr [PCH_Fwd_468980]
        add  esp, 8
        pop  esi
        ret
    }
}

}  // namespace

// ===========================================================================
// A4 body-MATH — the pure-arithmetic half of FUN_00470670 (0x00470670 prologue
// through 0x00470914, i.e. everything BEFORE the A5/A6a/A6b callee dispatch).
//   v        = the 0xd04 vehicle record
//   input    = per-car input descriptor ([0]=accel [1]=brake [5]=gate)
//   dt       = the float dt (FADD slot fed to the input smoother)
//   gameMode = FUN_0040e350() result (==7 special)
//   phase    = DAT_007f101c & 0xf (torque ring index)
// SELF-CONTAINED: every constant is a compiled-in literal (NOT read from MASHED
// memory) and the smoother call is the ONLY external dependency (FUN_004a2c48 at
// a fixed RVA). Exported so the telemetry harness can A/B it against the live
// original's body outputs on a captured record (deterministic, no race noise).
// Built x87 (.asi TU, no /arch:SSE2) → bit-identical float32 stores.
// ===========================================================================
extern "C" __declspec(dllexport)
void A4_BodyMath(void* v, std::uint8_t* input, float dt, int gameMode, unsigned phase) {
    const bool atRest = (Fb(v, 0x9e4) == DAT_005d757c);              // 0x00470680

    Ib(v, 0xb20) = 0; Ib(v, 0xb1c) = 0; Ib(v, 0xb18) = 0; Ib(v, 0xb14) = 0;
    Ib(v, 0x3f4) = 0; Ib(v, 0x330) = 0; Ib(v, 0x26c) = 0; Ib(v, 0x1a8) = 0;

    if (atRest) {
        Ib(v, 0xb0c) = 0;                                            // 0x0047072c
    } else {
        float dot = Fb(v, 0x9dc) * Fb(v, 0x9b8)
                  + Fb(v, 0x9d4) * Fb(v, 0x9b0)
                  + Fb(v, 0x9d8) * Fb(v, 0x9b4);                     // 0x004706db
        if (dot < DAT_005d757c) dot = -dot;                         // 0x00470710 FCHS
        Fb(v, 0xb0c) = (_DAT_005cc320 - dot / Fb(v, 0x9e4)) * Fb(v, 0x9e4);
    }

    // input smoother: A4 does FILD[+0xb24];FADD dt;CALL 0x4a2c48; store EAX → +0xb24
    if (input[0] == 0) { Ib(v, 0xb24) = 0; }                        // 0x0047074e
    else { Ib(v, 0xb24) = SmoothInput(v, 0xb24, dt); }
    if (input[1] == 0) { Ib(v, 0xb28) = 0; }
    else { Ib(v, 0xb28) = SmoothInput(v, 0xb28, dt); }

    float force = DAT_005d757c;

    if (input[0] != 0) {                                            // 0x00470788 accelerate
        if (_DAT_005cd6d4 < Fb(v, 0x9e4)) Ib(v, 0xb20) = 1;
        force = static_cast<float>(input[0]) * Fb(v, 0x190) * _DAT_005ceaa8;
        if (gameMode == 7) force = DAT_005d757c;
        force *= _DAT_005cc32c;
        if (Ib(v, 0xbf0) != 0) force *= _DAT_005cc950;
        if (static_cast<float>(input[5]) <= _DAT_005cc9d0) {
            float f = static_cast<float>(Ib(v, 0xb24));
            if (_DAT_005ceaa4 < static_cast<float>(Ib(v, 0xb24))) f = _DAT_005ceaa4;
            force = (f + _DAT_005ceaa4) * force * _DAT_005cea58;
        } else {
            force *= _DAT_005cc348;
        }
        Fb(v, 0x1a8) = force;
        Fb(v, 0x26c) = force;
    }
    Fb(v, 0x1ac + phase * 4) = force;                               // 0x00470841
    Fb(v, 0x270 + phase * 4) = force;                               // 0x00470848

    if (input[1] != 0) {                                            // 0x0047084f brake/reverse
        if (_DAT_005cd6d4 < Fb(v, 0x9e4)) Ib(v, 0xb20) = 1;
        force = static_cast<float>(input[1]) * Fb(v, 0x190) * _DAT_005ceaa8;
        if (gameMode == 7) force = DAT_005d757c;
        force *= _DAT_005cc32c;
        if (Ib(v, 0xbf0) != 0) force *= _DAT_005cc950;
        if (static_cast<float>(input[5]) <= _DAT_005cc9d0) {
            float f = static_cast<float>(Ib(v, 0xb28));
            if (_DAT_005ceaa4 < static_cast<float>(Ib(v, 0xb28))) f = _DAT_005ceaa4;
            force = (f + _DAT_005ceaa4) * force * _DAT_005cea58;
        } else {
            force *= _DAT_005cc348;
        }
        force = -force;                                             // 0x004708f8 FCHS
        Fb(v, 0x1a8) = force;
        Fb(v, 0x26c) = force;
        Fb(v, 0x1ac + phase * 4) = force;
        Fb(v, 0x270 + phase * 4) = force;
    }
}

namespace {

// ═══════════════════════════════════════════════════════════════════════════
// IN-PROCESS BIT-IDENTITY SELF-TEST (gated by env MASHED_PHYS_C4_SELFTEST)
// ───────────────────────────────────────────────────────────────────────────
// Frida Interceptor on this >1000/s hot path destabilizes MASHED before a single
// call can be captured (CLAUDE.md; verified WS-PHYS-C4-LANE). So the bit-identity
// A/B is done IN-PROCESS with ZERO Frida overhead: run the ORIGINAL A4 body-math
// (the instruction stream 0x00470670..0x00470914) on the live record, snapshot
// its outputs, restore the record, run MY A4_BodyMath, and bit-compare. The
// original body is reached by an early-return trampoline: the original prologue
// (5 bytes saved by HookSystem) is re-executed and then JMP A4+5; 0x00470914
// (first instr after the body) is temporarily patched to a frame-restoring RET
// so the original body math runs WITHOUT its A5/A6a/A6b callee dispatch (which
// would double-step shared globals). Results are written to phys_c4_selftest.log.
// Single-threaded physics -> the temporary patch is safe (restored before return).
// ═══════════════════════════════════════════════════════════════════════════
const int kBodyOutOffs[] = {0x1a8,0x26c,0xb0c,0xb24,0xb28,0xb14,0xb18,0xb1c,0xb20,0x330,0x3f4};

inline int SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_PHYS_C4_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}

// early-return patch site + saved bytes
const std::uintptr_t kA4BodyEnd = 0x00470914;   // MOV ECX,[ESP+0x24] (first post-body instr)
// frame-restoring early return: POP EDI;POP ESI;POP EBP;POP EBX;POP ECX;RET = 6 bytes
const std::uint8_t kEarlyRet[6] = {0x5f,0x5e,0x5d,0x5b,0x59,0xc3};
std::uint8_t g_saveBodyEnd[6];

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

// run the original A4 body-math on `record` (with the same ABI args), capturing
// nothing — the outputs land in the live record. Patches/*restores* kA4BodyEnd.
typedef void (*OrigA4Fn)(void* record_in_eax, int param_1, float dt, void* input, void* xform);
void RunOrigBody(void* record, int param_1, float dt, void* input, void* xform) {
    DWORD op;
    auto* site = reinterpret_cast<std::uint8_t*>(kA4BodyEnd);
    if (!VirtualProtect(site, 6, PAGE_EXECUTE_READWRITE, &op)) return;
    std::memcpy(g_saveBodyEnd, site, 6);
    std::memcpy(site, kEarlyRet, 6);
    DWORD tmp; VirtualProtect(site, 6, op, &tmp);
    // call the trampoline with EAX=record + cdecl args (push xform,input,dt,param_1)
    __asm {
        mov  eax, record
        push xform
        push input
        push dt
        push param_1
        call OrigA4Trampoline
        add  esp, 16
    }
    // restore the patched site
    if (VirtualProtect(site, 6, PAGE_EXECUTE_READWRITE, &op)) {
        std::memcpy(site, g_saveBodyEnd, 6);
        VirtualProtect(site, 6, op, &tmp);
    }
}

void SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("phys_c4_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

// Compare MY A4_BodyMath vs the ORIGINAL body on the SAME live record state.
// Returns mismatch count; logs per-call summary. Runs for the first kMaxTests calls.
int g_selfTestCount = 0;        // calls WITH nonzero accel/brake input (the force path)
int g_selfTestIdle  = 0;        // coast calls (no input) — sampled sparsely
const int kMaxTests = 96;
void A4_SelfTest(void* record, int param_1, float dt, std::uint8_t* input, void* xform,
                 int gameMode, unsigned phase) {
    if (g_selfTestCount >= kMaxTests) return;
    // Prioritize the DRIVE path (input nonzero exercises the force formula / grip
    // branch / boost gate). Keep only a few idle/coast samples so the log isn't
    // flooded by the pre-throttle countdown frames.
    const bool hasInput = (input[0] != 0) || (input[1] != 0);
    if (!hasInput) {
        if (g_selfTestIdle >= 8) return;
        ++g_selfTestIdle;
    }
    // snapshot the full record so each impl runs from identical input state
    static std::uint8_t snap[0xd04], mineOut[0xd04];
    std::memcpy(snap, record, 0xd04);
    // (1) original body on the live record
    RunOrigBody(record, param_1, dt, input, xform);
    // capture original outputs
    std::uint32_t origVals[16]; int nf = 0;
    for (int off : kBodyOutOffs) origVals[nf++] = *reinterpret_cast<std::uint32_t*>((char*)record + off);
    std::uint32_t origRD = *reinterpret_cast<std::uint32_t*>((char*)record + 0x1ac + phase*4);
    std::uint32_t origRA = *reinterpret_cast<std::uint32_t*>((char*)record + 0x270 + phase*4);
    // restore the record, run MY body math
    std::memcpy(record, snap, 0xd04);
    A4_BodyMath(record, input, dt, gameMode, phase);
    // compare
    int mism = 0; char line[512]; int p = 0;
    for (int i = 0; i < nf; ++i) {
        std::uint32_t mv = *reinterpret_cast<std::uint32_t*>((char*)record + kBodyOutOffs[i]);
        if (mv != origVals[i]) { mism++;
            p += wsprintfA(line+p, " +0x%x:o=%08x,m=%08x", kBodyOutOffs[i], origVals[i], mv); }
    }
    std::uint32_t mRD = *reinterpret_cast<std::uint32_t*>((char*)record + 0x1ac + phase*4);
    std::uint32_t mRA = *reinterpret_cast<std::uint32_t*>((char*)record + 0x270 + phase*4);
    if (mRD != origRD) { mism++; p += wsprintfA(line+p, " rd:o=%08x,m=%08x", origRD, mRD); }
    if (mRA != origRA) { mism++; p += wsprintfA(line+p, " ra:o=%08x,m=%08x", origRA, mRA); }
    char hdr[160];
    wsprintfA(hdr, "[%d] in0=%u in1=%u in5=%u gm=%d ph=%u ndiff=%d%s\r\n",
              g_selfTestCount, input[0], input[1], input[5], gameMode, phase, mism,
              mism ? "" : " OK");
    SelfTestLog(hdr);
    if (mism) { line[p]=0; SelfTestLog("   "); SelfTestLog(line); SelfTestLog("\r\n"); }
    g_selfTestCount++;
}

// ===========================================================================
// A4 body — full FUN_00470670: the body-math + the callee dispatch + parked damp.
//   param_1 = stack arg0 (to A6a)
//   xform   = stack arg3 (vehicle world matrix; to A5)
// ===========================================================================
void A4_Body(void* record, int param_1, float dt, std::uint8_t* input, void* xform) {
    void* v = record;
    const int gameMode = Fi_GameMode();                              // 0x0047067b
    int* wheelBlock = reinterpret_cast<int*>(
        reinterpret_cast<char*>(v) + Ib(v, 0x9a8) * 0x40 + 0x928);   // 0x0047068e
    const unsigned phase = static_cast<unsigned>(TorqueRingPhase()) & 0xf;  // 0x00470785

    if (SelfTestEnabled())
        A4_SelfTest(v, param_1, dt, input, xform, gameMode, phase);

    A4_BodyMath(v, input, dt, gameMode, phase);

    Call_A5(v, dt, wheelBlock, xform);                              // 0x00470923 (EDI=record)
    Call_A6a(v, param_1, dt, wheelBlock, input);                   // 0x00470936 (ESI=record)
    Call_A6b(v, dt, input);                                        // 0x00470943 (ECX=ESI=record)

    if (Ib(v, 0x9f0) == 2) {                                        // 0x00470948 parked damp
        Fb(v, 0x9b0) *= _DAT_005cc9c8;
        Fb(v, 0x9b4) *= _DAT_005cc9c8;
        Fb(v, 0x9b8) *= _DAT_005cc9c8;
    }
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

RH_ScopedInstall(A4_Entry, 0x00470670);
