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
    // A5-chain live callees (forwarded so the running MASHED's RW LUT / PRNG run):
    void* PCH_Fwd_46c5f0 = reinterpret_cast<void*>(0x0046c5f0);  // TriangleFaceNormal (EDI=record)
    // A6a chain: the ST0 smoother + the two game-mode getters + the LUT magnitude
    // (forwarded as a raw addr so float10-preserving naked shims can keep ST0).
    void* PCH_Fwd_40e340 = reinterpret_cast<void*>(0x0040e340);  // player-count getter
    void* PCH_Fwd_4c3ac0 = reinterpret_cast<void*>(0x004c3ac0);  // Vec3 magnitude (ST0 float10)
}

// A5 (FUN_0046ddb0) callees called as plain C function pointers at their live RVAs.
// ABI read from the call sites (Mashed_pool11, read-only, 2026-06-17):
//   FUN_004c3df0(dst,src,count,mtx)  __cdecl 4 stack args (RwV3dTransformPoints, RW device)
//   FUN_004c4d20(outMtx,axis,angle,mode) __cdecl 4 args (RwMatrix from axis+angle)
//   FUN_004c3ac0(float* v) -> ST0    __cdecl 1 arg     (Vec3 magnitude; RW fast-sqrt LUT)
//   FUN_004c39b0(out,in) -> ST0      __cdecl 2 args    (Vec3 normalize; RW fast-inv-sqrt LUT)
//   FUN_00472650(lo,hi) -> ST0       __cdecl 2 args    (random float in [lo,hi); PRNG FUN_00534870)
//   FUN_00442ce0(car,band,grip) -> ST0 __cdecl 3 args  (rubber-band grip multiplier)
//   FUN_00442c80(car) -> int(EAX)    __cdecl 1 arg      (rubber-band activation gate)
typedef void  (__cdecl* Fn_xform_t)(float* dst, const float* src, int count, const void* mtx);
typedef void  (__cdecl* Fn_axang_t)(void* outMtx, const float* axis, float angle, int mode);
typedef float (__cdecl* Fn_mag_t)  (const float* v);
typedef float (__cdecl* Fn_norm_t) (float* out, const float* in);
typedef float (__cdecl* Fn_rand_t) (float lo, float hi);
typedef float (__cdecl* Fn_rbg_t)  (int car, float band, float grip);
typedef int   (__cdecl* Fn_rbgate_t)(int car);
static const Fn_xform_t  Live_004c3df0 = reinterpret_cast<Fn_xform_t> (0x004c3df0);
static const Fn_axang_t  Live_004c4d20 = reinterpret_cast<Fn_axang_t> (0x004c4d20);
static const Fn_mag_t    Live_004c3ac0 = reinterpret_cast<Fn_mag_t>   (0x004c3ac0);
static const Fn_norm_t   Live_004c39b0 = reinterpret_cast<Fn_norm_t>  (0x004c39b0);
static const Fn_rand_t   Live_00472650 = reinterpret_cast<Fn_rand_t>  (0x00472650);
static const Fn_rbg_t    Live_00442ce0 = reinterpret_cast<Fn_rbg_t>   (0x00442ce0);
static const Fn_rbgate_t Live_00442c80 = reinterpret_cast<Fn_rbgate_t>(0x00442c80);

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

// ═══════════════════════════════════════════════════════════════════════════
// A5 (FUN_0046ddb0) — exact-bit constants (memory_read, Mashed_pool11 2026-06-17)
// All from ForceIntegrator.h + this session's reads; cited @ their .rdata address.
// ═══════════════════════════════════════════════════════════════════════════
const float A5_005cc320 = Cf(0x3f800000);  // 1.0
const float A5_005cc32c = Cf(0x3f000000);  // 0.5
const float A5_005cc318 = Cf(0x3f19999a);  // 0.6   suspForce[1] coeff / rubber A
const float A5_005cc328 = Cf(0x3c23d70a);  // 0.01  spin-scale (_DAT_005cc328)
const float A5_005cc564 = Cf(0x3e800000);  // 0.25  steer ring quarter
const float A5_005cc574 = Cf(0x40000000);  // 2.0   gravity-ramp clamp
const float A5_005cc31c = Cf(0x40400000);  // 3.0   airborne grounded-count thr
const float A5_005cc990 = Cf(0x3727c5ac);  // 9.98199e-6 gravity ang-scale (EXACT, read 2026-06-17)
const float A5_005cc948 = Cf(0x39aec33e);  // 3.33e-4  substep ms->s
const float A5_005cc9b4 = Cf(0x3f7d70a4);  // 0.99    draft cos threshold
const float A5_005ccac8 = Cf(0x3eaaaaab);  // 0.333   steer clamp third
const float A5_005ccd08 = Cf(0x453b8000);  // 3000.0  airborne damp k (_DAT_005ccd08) (EXACT, read 2026-06-17)
const float A5_005ccd6c = Cf(0x41a00000);  // 20.0    steer-out hi clamp
const float A5_005cd03c = Cf(0x38d1b717);  // 9.99e-5 speed-min epsilon
const float A5_005cd050 = Cf(0x3e000000);  // 0.125   draft ramp
const float A5_005cd0a0 = Cf(0x40c00000);  // 6.0     draft distance
const float A5_005cd61c = Cf(0xc1a00000);  // -20.0   steer-out lo clamp
const float A5_005ce018 = Cf(0x3b03126f);  // 0.002   steer projection scale
const float A5_005cea64 = Cf(0x47800000);  // 65536.0 airborne vel threshold
const float A5_005cea68 = Cf(0x36a7c5ac);  // 4.99955e-6 rand-impulse speed scale
const float A5_005cea6c = Cf(0x3b5a740e);  // 0.00333 steer-out scale
const float A5_005cea70 = Cf(0xbe19999a);  // -0.15   grip count-0 coeff
const float A5_005cea74 = Cf(0xb80bcf65);  // -3.33e-5 grip-ramp k
const float A5_005cea78 = Cf(0xc6ea6000);  // -30000  grip-ramp lo clamp
const float A5_005cea7c = Cf(0x476a6000);  // 60000   grip-ramp hi
const float A5_00613138 = Cf(0x3d4ccccd);  // 0.05    suspForce[2] coeff (_DAT_00613138)
// surface-jitter coefficients (selected by integer surface key)
const float A5_jit0p25 = Cf(0x3e800000);   // 0.25 default
const float A5_jit0p1  = Cf(0x3dcccccd);   // 0.1
const float A5_jit0p01 = Cf(0x3c23d70a);   // 0.01
const float A5_jit0p2  = Cf(0x3e4ccccd);   // 0.2
// surface keys (CMP EAX,imm — verbatim from the dispatch asm 0x0046e76d..0x0046e817)
const int   A5_keyRandom = (int)0xffff32ff;
const int   A5_key0p1    = (int)0xffa08080;
const int   A5_key0p01   = (int)0xffaa8080;
const int   A5_keySlipA  = (int)0xff961e5a;
const int   A5_keySlipB  = (int)0xff1e80b4;
const int   A5_key0p2    = (int)0xffc81e5a;
// local model axes the vehicle matrix transforms (DAT_006146fc up, DAT_00614708 fwd)
const float* const A5_DAT_006146fc = reinterpret_cast<const float*>(0x006146fc);  // (0,1,0)
const float* const A5_DAT_00614708 = reinterpret_cast<const float*>(0x00614708);  // (0,0,1)

// live runtime globals A5 reads (absolute; populated by the running MASHED) ----
inline int&   A5_PlayerCount() { return *reinterpret_cast<int*>  (0x007f0fd0); }   // DAT_007f0fd0
inline int&   A5_RaceTimer()   { return *reinterpret_cast<int*>  (0x007f0ff8); }   // DAT_007f0ff8
inline float& A5_GravScale()   { return *reinterpret_cast<float*>(0x00803340); }   // _DAT_00803340
inline float& A5_GravX()       { return *reinterpret_cast<float*>(0x00803334); }   // _DAT_00803334
inline float& A5_GravY()       { return *reinterpret_cast<float*>(0x00803338); }   // _DAT_00803338
inline float& A5_GravZ()       { return *reinterpret_cast<float*>(0x0080333c); }   // _DAT_0080333c
inline float& A5_SuspDtTerm()  { return *reinterpret_cast<float*>(0x0088e610); }   // _DAT_0088e610
inline float& A5_SuspScale()   { return *reinterpret_cast<float*>(0x0088e5f0); }   // _DAT_0088e5f0
inline char*  A5_VehBase()     { return reinterpret_cast<char*>(0x008815a0); }     // DAT_008815a0
// shared per-wheel scratch region A5 WRITES (0x00881560..0x00881590, 48 bytes):
//   piVar10 loop [0x881560..0x88158c] + steer scalars 0x881564/570/57c/588
inline float* A5_Scratch()     { return reinterpret_cast<float*>(0x00881560); }    // DAT_00881560
inline int*   A5_ScratchI()    { return reinterpret_cast<int*>  (0x00881560); }

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

// FUN_0046c5f0 (TriangleFaceNormal) — __fastcall(ECX=v1,EDX=v2)+EAX=v0+ESI=out.
// A5 invokes it with the EXACT register setup (asm 0x0046e6f3..0x0046e728):
//   EAX=&scratch[0]  ECX=&scratch[6]  EDX=&scratch[3]  ESI=&record[ring*3+0x2cb].
// We forward to the LIVE original (its inner fast-sqrt FUN_004c3b30 runs the live
// RW LUT) so the steer-feedback normal is bit-identical. `out` = the output ptr;
// the three scratch vertices are the fixed DAT_00881560 region.
extern "C" void* PCH_Fwd_46c5f0;
__declspec(naked) void Call_FaceNormal(float* /*out*/) {
    __asm {
        push esi
        push ebx
        mov  esi, dword ptr [esp+12]   // out -> ESI  ([esp+4]+8 for the 2 pushes)
        mov  eax, 0x00881560           // EAX = &scratch[0]   (v0 / in_EAX)
        mov  ecx, 0x00881578           // ECX = &scratch[6]   (param_1)
        mov  edx, 0x0088156c           // EDX = &scratch[3]   (param_2)
        call dword ptr [PCH_Fwd_46c5f0]
        pop  ebx
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

// ===========================================================================
// A5 — FUN_0046ddb0 the per-wheel force integrator (EDI=record; dt, xform).
// VERBATIM transcription of 0x0046ddb0..0x0046e9d0 (decomp Mashed_pool11 RO
// 2026-06-17). Every callee forwarded to its LIVE original so the running
// MASHED's RW fast-sqrt LUT + PRNG execute → a faithful C transcription is
// bit-identical. Built x87 (.asi TU, no /arch:SSE2): float10 transients map to
// `double`, each store rounds to float32 like the original FSTP. Constants are
// exact .rdata bit patterns (A5_005c... above).
//   self  = the 0xd04 record (int* view; vF() = the decompiler's float reinterp)
//   dt    = param_1; xform = param_2 (the vehicle world matrix).
// ===========================================================================
namespace {

inline float& a5F(int* v, int i) { return *reinterpret_cast<float*>(v + i); }
inline int    a5AsI(float f) { int i; std::memcpy(&i, &f, 4); return i; }

void A5_Body(int* self, float dt, void* xform) {
    char* gb = A5_VehBase();
    int*  scratch = A5_ScratchI();   // DAT_00881560 (shared)

    // ── Phase 0: world forward + grounded count + per-wheel transforms ──
    Live_004c3df0(reinterpret_cast<float*>(self + 0x275), A5_DAT_00614708, 1, xform);  // 0x0046ddc9
    self[0x278] = 0;                                                                   // 0x0046ddd1
    int* piVar10 = scratch;
    int* piVar12 = self + 0x5b;
    do {
        if (piVar12[0xb] != 0)
            a5F(self, 0x278) = a5F(self, 0x278) + A5_005cc320;                        // contact -> grounded++ (FLOAT store)
        piVar10[0] = piVar12[0];
        piVar10[1] = 0;
        piVar10[2] = piVar12[2];
        Live_004c3df0(reinterpret_cast<float*>(piVar12 + 5), reinterpret_cast<float*>(piVar10), 1, xform);
        if (a5F(piVar12, 0xf) == DAT_005d757c) {
            piVar12[0x2d] = self[0x275];
            piVar12[0x2e] = self[0x276];
            piVar12[0x2f] = self[0x277];
        } else {
            unsigned char m[64];
            Live_004c4d20(m, A5_DAT_006146fc, a5F(piVar12, 0xf), 0);                  // 0x0046dxx FUN_004c4d20
            Live_004c3df0(reinterpret_cast<float*>(piVar12 + 0x2d),
                          reinterpret_cast<float*>(self + 0x275), 1, m);
        }
        piVar10 += 3;
        piVar12 += 0x31;
    } while ((int)(std::uintptr_t)piVar10 < 0x881590);

    // ── Phase 1: velocity history ring + drive-torque base ──
    unsigned int uVar7 = (unsigned int)(self[0x2b4] - 1) & 1;
    self[0x2b4] = (int)uVar7;
    self[uVar7 * 3 + 0x2b5] = self[0x26c];
    self[uVar7 * 3 + 0x2b6] = self[0x26d];
    self[uVar7 * 3 + 0x2b7] = self[0x26e];
    float fVar4 = a5F(self, 0x279);
    if (a5F(self, 0x279) < A5_005cd03c) fVar4 = DAT_005d757c;
    fVar4 = a5F(self, 0x54) * a5F(self, 0x55) * a5F(self, 0x56) * fVar4;
    if (self[0x278] != 0x40800000) fVar4 = fVar4 * A5_005cc32c;
    fVar4 = fVar4 * a5F(self, 0x15) * dt * A5_005cc948;

    // ── Phase 2: drafting / proximity grip reduction (local_70) ──
    int iVar11 = 0, iVar13 = 0;
    float local_70 = 1.0f;
    do {
        if ((iVar11 != *self) && (*reinterpret_cast<int*>(gb + 4 + iVar13) == 1)) {
            int iVar3 = self[0x26a];
            int iVar8 = *reinterpret_cast<int*>(gb + (0x881f48 - 0x8815a0) + iVar13) * 0x40;
            int iVar9 = iVar8 + iVar13;
            float local_64 = *reinterpret_cast<float*>(gb + (0x881ef8 - 0x8815a0) + iVar13 + iVar8)
                             - a5F(self, iVar3 * 0x10 + 0x256);
            float local_60 = *reinterpret_cast<float*>(gb + (0x881efc - 0x8815a0) + iVar9)
                             - a5F(self, iVar3 * 0x10 + 599);
            float local_58 = *reinterpret_cast<float*>(gb + (0x881f50 - 0x8815a0) + iVar13);
            float local_5c = *reinterpret_cast<float*>(gb + (0x881f00 - 0x8815a0) + iVar9)
                             - a5F(self, iVar3 * 0x10 + 600);
            float local_54 = *reinterpret_cast<float*>(gb + (0x881f54 - 0x8815a0) + iVar13);
            float local_50 = *reinterpret_cast<float*>(gb + (0x881f58 - 0x8815a0) + iVar13);
            // local_58/54/50 = other car velocity (vec3 starting at &local_58)
            float other[3]  = { local_58, local_54, local_50 };
            float deltaN[3];
            double mag = (double)Live_004c3ac0(other);
            if ((double)A5_005cd03c <= mag) {
                Live_004c39b0(other, other);                  // FUN_004c39b0 normalize in place
                float delta[3] = { local_64, local_60, local_5c };
                Live_004c39b0(deltaN, delta);
                if ((double)A5_005cc9b4 < (double)(deltaN[2] * other[2] + deltaN[0] * other[0] + deltaN[1] * other[1])) {
                    double dd = (double)Live_004c3ac0(delta);
                    if (dd < (double)A5_005cd0a0) {
                        double r = (double)A5_005cc320 - ((double)A5_005cd0a0 - dd) * (double)A5_005cd050;
                        if (r < (double)local_70) local_70 = (float)r;
                    }
                }
            }
        }
        iVar13 += 0xd04;
        iVar11 += 1;
    } while (iVar13 < 0x3410);

    // ── Phase 3: player-count / time-ramp / contact-count / rubber-band grip ──
    if (A5_PlayerCount() == 4)      local_70 = 1.0f;
    else if (A5_PlayerCount() == 9) local_70 = 1.0f;
    else if ((A5_PlayerCount() == 8) && (*self != 0)) local_70 = 0.75f;

    float fVar5 = A5_005cea7c - (float)A5_RaceTimer();
    float local_6c;
    if (DAT_005d757c <= fVar5) {
        local_6c = 1.0f;
    } else {
        if (fVar5 < A5_005cea78) fVar5 = A5_005cea78;
        local_6c = A5_005cc320 - fVar5 * A5_005cea74;
    }

    int* C0a = reinterpret_cast<int*>(gb + 4);           int* C0c = reinterpret_cast<int*>(gb + 8);
    int* C1a = reinterpret_cast<int*>(gb + 4 + 0xd04);   int* C1c = reinterpret_cast<int*>(gb + 8 + 0xd04);
    int* C2a = reinterpret_cast<int*>(gb + 4 + 0x1a08);  int* C2c = reinterpret_cast<int*>(gb + 8 + 0x1a08);
    int* C3a = reinterpret_cast<int*>(gb + 4 + 0x270c);  int* C3c = reinterpret_cast<int*>(gb + 8 + 0x270c);

    iVar11 = 0;
    if ((*C0a != 0) && (0 < *C0c)) iVar11 = *C0c;
    if ((*C1a != 0) && (iVar11 < *C1c)) iVar11 = *C1c;
    if ((*C2a != 0) && (iVar11 < *C2c)) iVar11 = *C2c;
    if ((*C3a != 0) && (iVar11 < *C3c)) iVar11 = *C3c;
    fVar5 = A5_005cea70;
    if ((((iVar11 != 0) && (fVar5 = DAT_005d757c, iVar11 != 1)) &&
         (fVar5 = A5_005cc32c, iVar11 != 2)) && (fVar5 = DAT_005d757c, iVar11 == 3)) {
        fVar5 = A5_005cc320;
    }
    if (local_6c == DAT_005d757c) {
        iVar11 = 3;
        if ((*C0a != 0) && (*C0c < 3)) iVar11 = *C0c;
        if ((*C1a != 0) && (*C1c < iVar11)) iVar11 = *C1c;
        if ((*C2a != 0) && (*C2c < iVar11)) iVar11 = *C2c;
        if ((*C3a != 0) && (*C3c < iVar11)) iVar11 = *C3c;
        if (iVar11 == 0) { local_6c = 1.0f; fVar5 = A5_005cea70; }
    }
    local_70 = (fVar5 * local_6c + A5_005cc320) * local_70;
    local_70 = (float)((double)Live_00442ce0(*self, local_70, local_6c) * (double)local_70);
    if ((DAT_005d757c < local_6c) && (Live_00442c80(*self) != 0)) {
        unsigned char bVar15 = (*C0a != 0) ? 1 : 0;
        if (*C1a != 0) bVar15 = bVar15 + 1;
        if (*C2a != 0) bVar15 = bVar15 + 1;
        if (*C3a != 0) bVar15 = bVar15 + 1;
        if (2 < bVar15) local_70 = local_70 * A5_005cc32c;
    }

    // ── Phase 4: drive-drag + gravity applied to linear velocity (+0x9b0) ──
    fVar4 = A5_005cc320 - local_70 * fVar4;
    if ((fVar4 < DAT_005d757c) || (A5_005cc320 < fVar4)) fVar4 = DAT_005d757c;
    a5F(self, 0x26c) = fVar4 * a5F(self, 0x26c);
    a5F(self, 0x26d) = fVar4 * a5F(self, 0x26d);
    a5F(self, 0x26e) = fVar4 * a5F(self, 0x26e);
    float local_5c = dt * a5F(self, 0x279) * A5_005cc990;
    if (A5_005cc574 < local_5c) local_5c = A5_005cc574;
    local_5c = local_5c * A5_GravScale();
    float local_64 = A5_GravX() * local_5c;
    float local_60 = A5_GravY() * local_5c;
    local_5c = A5_GravZ() * local_5c;
    a5F(self, 0x26c) = local_64 + a5F(self, 0x26c);
    a5F(self, 0x26d) = local_60 + a5F(self, 0x26d);
    a5F(self, 0x26e) = local_5c + a5F(self, 0x26e);

    // ── Phase 5: steer torque from velocity delta (any wheel grounded) ──
    if (a5F(self, 0x278) != DAT_005d757c) {
        int hi = self[0x2b4];
        unsigned int lo = (unsigned int)(hi - 1) & 1;
        local_64 = (a5F(self, hi * 3 + 0x2b5) - a5F(self, lo * 3 + 0x2b5)) * dt;
        local_60 = (a5F(self, hi * 3 + 0x2b6) - a5F(self, lo * 3 + 0x2b6)) * dt;
        local_5c = (a5F(self, hi * 3 + 0x2b7) - a5F(self, lo * 3 + 0x2b7)) * dt;
        fVar4 = (a5F(self, 0x14) / a5F(self, 0x278)) * A5_SuspDtTerm();
        fVar5 = fVar4 * A5_005ccac8;
        // wheel0  -> self[0x83], scratch[1] (DAT_00881564)
        if (self[0x66] == 0) { self[0x83] = 0; }
        else {
            float f = (a5F(self, 0x62) * local_5c + a5F(self, 0x60) * local_64 + a5F(self, 0x61) * local_60) * A5_005ce018;
            if (fVar5 < f) f = fVar5;
            if (f < -fVar5) f = -fVar5;
            a5F(self, 0x83) = fVar4 + f;
            if (f <= A5_005ccd6c) { if (f < A5_005cd61c) f = A5_005cd61c; a5F(scratch, 1) = f * A5_005cea6c; }
            else { a5F(scratch, 1) = A5_005ccd6c * A5_005cea6c; }
        }
        // wheel1  -> self[0xb4], scratch[4] (DAT_00881570)
        if (self[0x97] == 0) { self[0xb4] = 0; }
        else {
            float f = (a5F(self, 0x93) * local_5c + a5F(self, 0x91) * local_64 + a5F(self, 0x92) * local_60) * A5_005ce018;
            if (fVar5 < f) f = fVar5;
            if (f < -fVar5) f = -fVar5;
            a5F(self, 0xb4) = fVar4 + f;
            if (f <= A5_005ccd6c) { if (f < A5_005cd61c) f = A5_005cd61c; a5F(scratch, 4) = f * A5_005cea6c; }
            else { a5F(scratch, 4) = A5_005ccd6c * A5_005cea6c; }
        }
        // wheel2  -> self[0xe5], scratch[7] (DAT_0088157c)
        if (self[200] == 0) { self[0xe5] = 0; }
        else {
            float f = (a5F(self, 0xc4) * local_5c + a5F(self, 0xc2) * local_64 + a5F(self, 0xc3) * local_60) * A5_005ce018;
            if (fVar5 < f) f = fVar5;
            if (f < -fVar5) f = -fVar5;
            a5F(self, 0xe5) = fVar4 + f;
            if (f <= A5_005ccd6c) { if (f < A5_005cd61c) f = A5_005cd61c; a5F(scratch, 7) = f * A5_005cea6c; }
            else { a5F(scratch, 7) = A5_005ccd6c * A5_005cea6c; }
        }
        // wheel3  -> self[0x116], scratch[10] (DAT_00881588)
        if (self[0xf9] == 0) { self[0x116] = 0; }
        else {
            float f = (a5F(self, 0xf5) * local_5c + a5F(self, 0xf3) * local_64 + a5F(self, 0xf4) * local_60) * A5_005ce018;
            if (fVar5 < f) f = fVar5;
            if (f < -fVar5) f = -fVar5;
            a5F(self, 0x116) = fVar4 + f;
            if (f <= A5_005ccd6c) { if (f < A5_005cd61c) f = A5_005cd61c; a5F(scratch, 10) = f * A5_005cea6c; }
            else { a5F(scratch, 10) = A5_005ccd6c * A5_005cea6c; }
        }
        a5F(self, 0x116) = a5F(self, 0x116) * A5_005cc32c;
        unsigned int ring = (unsigned int)(self[0x2fb] + 1) & 0xf;
        self[0x2fb] = (int)ring;
        Call_FaceNormal(reinterpret_cast<float*>(self + (ring * 3 + 0x2cb)));   // FUN_0046c5f0 (live)
        int slot = ring * 3 + 0x2cd;
        if (a5F(self, slot) < DAT_005d757c) a5F(self, slot) = a5F(self, slot) * A5_005cc564;
    }

    // ── Phase 6: per-wheel suspension force + surface jitter (4 wheels) ──
    float* pfVar14 = reinterpret_cast<float*>(self + 0x7d);
    int loopCount = 4;
    do {
        if (pfVar14[-0x17] == 0.0f) {
            pfVar14[10] = 0.0f; pfVar14[9] = 0.0f; pfVar14[8] = 0.0f;
        } else {
            int key = a5AsI(pfVar14[-1]);
            *pfVar14 = 0.25f;
            self[0x2c0] = 0; self[0x2c1] = 0; self[0x2c2] = 0;
            if (key == A5_keyRandom) {
                float rr = a5F(self, 0x279) * A5_005cea68;
                a5F(self, 0x2c0) = Live_00472650(-rr, rr);          // FUN_00472650 (live PRNG)
                self[0x2c1] = 0;
                a5F(self, 0x2c2) = Live_00472650(-rr, rr);
            } else if (key == A5_key0p1) {
                *pfVar14 = A5_jit0p1;
            } else if (key == A5_key0p01) {
                *pfVar14 = A5_jit0p01;
            } else {
                if ((key == A5_keySlipA) || (key == A5_keySlipB))
                    *pfVar14 = (self[0x340] == 0) ? A5_jit0p01 : A5_jit0p2;
                if (key == A5_key0p2)
                    *pfVar14 = A5_jit0p2;
            }
            if (self[0xb] != 0) *pfVar14 = a5F(self, 0xb) * *pfVar14 * A5_005cc328;
            float f = pfVar14[-0x1a] * *pfVar14;
            *pfVar14 = f;
            pfVar14[1] = A5_005cc318 * f;
            pfVar14[2] = A5_00613138 * f;
            if (self[4] == 1) { pfVar14[1] = f; pfVar14[2] = f; }
            pfVar14[7] = pfVar14[6] * pfVar14[4];
            if (pfVar14[6] * pfVar14[4] < DAT_005d757c) pfVar14[7] = 0.0f;
            double fVar17;
            if (self[0x278] == 0x40800000) {
                float fc = A5_SuspScale() * pfVar14[6] * A5_005cc32c;
                pfVar14[8]  = fc * pfVar14[3];
                pfVar14[9]  = (pfVar14[4] - A5_005cc320) * fc;
                pfVar14[10] = fc * pfVar14[5];
                fVar17 = (double)Live_004c3ac0(pfVar14 + 8);
                if ((float)fVar17 != DAT_005d757c) {
                    double inv = (double)A5_005cc320 / fVar17;
                    float a0 = (float)(inv * (double)pfVar14[8]);
                    float a1 = (float)(inv * (double)pfVar14[9]);
                    float a2 = (float)(inv * (double)pfVar14[10]);
                    double proj = ((double)a2 * (double)pfVar14[0xd] + (double)a0 * (double)pfVar14[0xb] +
                                   (double)a1 * (double)pfVar14[0xc]) * fVar17;
                    pfVar14[8]  = (float)(proj * (double)pfVar14[0xb]);
                    pfVar14[9]  = (float)(proj * (double)pfVar14[0xc]);
                    pfVar14[10] = (float)(proj * (double)pfVar14[0xd]);
                }
            } else {
                fVar17 = ((double)pfVar14[6] / (double)dt) * (double)A5_SuspScale();
                pfVar14[8]  = (float)(fVar17 * (double)pfVar14[3]);
                pfVar14[9]  = (float)(fVar17 * (double)pfVar14[4]);
                pfVar14[10] = (float)(fVar17 * (double)pfVar14[5]);
            }
            if ((self[0x2c8] == 0) && ((double)A5_005cea64 < fVar17)) self[0x2c8] = 1;
        }
        pfVar14 += 0x31;
        loopCount -= 1;
        if (loopCount == 0) {
            if (a5F(self, 0x278) < A5_005cc31c) {
                self[0x2c8] = 1;
                a5F(self, 0x2c6) = a5F(self, 0x2c6) - A5_SuspDtTerm() * a5F(self, 0x14) * A5_005ccd08;
            }
            return;
        }
    } while (true);
}

// ═══════════════════════════════════════════════════════════════════════════
// A5 IN-PROCESS BIT-IDENTITY SELF-TEST (env MASHED_PHYS_C4_SELFTEST)
// ───────────────────────────────────────────────────────────────────────────
// A5's WHOLE function is the body (no callee-dispatch tail like A4), so the
// "original-body" run is just a forwarded call to the LIVE A5 (Call_A5). The
// hazards the task flags:
//   (1) SHARED globals: A5 writes the scratch region 0x00881560..0x00881590
//       (48 b). Snapshot + restore it (with the record) so both impls see
//       identical input state.
//   (2) RNG: the random-surface branch (key 0xffff32ff) consumes FUN_00472650
//       twice, writing +0xb00/+0xb08. Running the original first advances the
//       live PRNG; my run would then read a DIFFERENT sequence. Detect this by
//       watching the PRNG read cursor *(DAT_007dc578 + 4 + DAT_007d3ff8); if it
//       advanced, the random branch fired → EXCLUDE +0xb00/+0xb08 from the
//       bit-compare for that call and FLAG it (honest: not replayed).
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
const int kA5MaxTests = 96;

void A5_SelfTest(int* record, float dt, void* xform) {
    if (g_a5Count >= kA5MaxTests) return;
    static std::uint8_t snapRec[0xd04];
    static std::uint8_t snapScr[0x30];   // 0x881560..0x881590
    char* scr = reinterpret_cast<char*>(0x00881560);

    // snapshot input state
    std::memcpy(snapRec, record, 0xd04);
    std::memcpy(snapScr, scr, 0x30);
    std::uintptr_t prngBefore = A5_PrngCursor();

    // (1) ORIGINAL A5 on the live record (Call_OrigA5 re-runs the original body,
    //     bypassing our inline-JMP; forwards dt+xform, EDI=record).
    Call_OrigA5(record, dt, xform);
    std::uintptr_t prngAfter = A5_PrngCursor();
    const bool rngFired = (prngBefore != prngAfter) && (prngBefore != 0);

    // capture original outputs (record + scratch)
    std::uint32_t origRec[32]; int nr = 0;
    for (int off : kA5RecOffs)   origRec[nr++] = *reinterpret_cast<std::uint32_t*>((char*)record + off);
    std::uint32_t origWh[8]; int nw = 0;
    for (int off : kA5WheelOffs) {
        origWh[nw++] = *reinterpret_cast<std::uint32_t*>((char*)record + off);       // susp X (+0x20)
        origWh[nw++] = *reinterpret_cast<std::uint32_t*>((char*)record + off + 8);    // susp Z (+0x28)
    }
    std::uint32_t origScr[12];
    std::memcpy(origScr, scr, 0x30);

    // restore input state, run MY A5_Body
    std::memcpy(record, snapRec, 0xd04);
    std::memcpy(scr, snapScr, 0x30);
    A5_Body(record, dt, xform);

    // compare
    int mism = 0; char line[1024]; int p = 0;
    nr = 0;
    for (int off : kA5RecOffs) {
        std::uint32_t mv = *reinterpret_cast<std::uint32_t*>((char*)record + off);
        // exclude RNG-derived +0xb00/+0xb08 if the original consumed PRNG (not replayed)
        const bool isRng = (off == 0xb00) || (off == 0xb08);
        if (isRng && rngFired) { nr++; continue; }
        if (mv != origRec[nr]) { mism++;
            if (p < 900) p += wsprintfA(line+p, " +0x%x:o=%08x,m=%08x", off, origRec[nr], mv); }
        nr++;
    }
    nw = 0;
    for (int off : kA5WheelOffs) {
        std::uint32_t mx = *reinterpret_cast<std::uint32_t*>((char*)record + off);
        std::uint32_t mz = *reinterpret_cast<std::uint32_t*>((char*)record + off + 8);
        if (mx != origWh[nw]) { mism++; if (p < 900) p += wsprintfA(line+p, " w+0x%x:o=%08x,m=%08x", off, origWh[nw], mx); }
        nw++;
        if (mz != origWh[nw]) { mism++; if (p < 900) p += wsprintfA(line+p, " w+0x%x:o=%08x,m=%08x", off+8, origWh[nw], mz); }
        nw++;
    }
    std::uint32_t mineScr[12];
    std::memcpy(mineScr, scr, 0x30);
    for (int i = 0; i < 12; ++i) {
        if (mineScr[i] != origScr[i]) { mism++;
            if (p < 900) p += wsprintfA(line+p, " scr[%d]:o=%08x,m=%08x", i, origScr[i], mineScr[i]); }
    }
    if (rngFired) g_a5RngHits++;

    char hdr[200];
    int grounded = *reinterpret_cast<int*>((char*)record + 0x9e0);
    wsprintfA(hdr, "[%d] dt=%08x grounded=%08x rng=%d ndiff=%d%s\r\n",
              g_a5Count, *reinterpret_cast<std::uint32_t*>(&dt), grounded, rngFired ? 1 : 0,
              mism, mism ? "" : " OK");
    A5_SelfTestLog(hdr);
    if (mism) { line[p] = 0; A5_SelfTestLog("   "); A5_SelfTestLog(line); A5_SelfTestLog("\r\n"); }
    g_a5Count++;
}

// ── entry trampoline installed at 0x0046ddb0 ────────────────────────────────
// A5 ABI: EDI=record; [ESP+4]=dt; [ESP+8]=xform; caller-cleans (saves EBX/EBP/ESI,
// `ret` no-imm). The trampoline preserves the same callee-saved set + EDI and
// forwards to A5_Hook (a normal C fn). When the self-test runs, A5_Hook runs the
// in-process A/B; either way it then runs A5_Body for real.
void A5_Hook(int* record, float dt, void* xform) {
    if (A5_SelfTestEnabled())
        A5_SelfTest(record, dt, xform);
    else
        A5_Body(record, dt, xform);
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
// VERBATIM transcription of 0x00467650..0x0046897b (decomp + disasm Mashed_pool12
// RO 2026-06-17). Every callee forwarded to its LIVE original (RW fast-sqrt LUT
// FUN_004c3ac0/39b0/3df0/4c4d20 + FUN_0040e350/0040e340), and the 7 implicit-ST0
// FUN_004a2c48 round calls are reproduced by naked shims that load ST0 with the
// EXACT original x87 sequence — resolving [U-A6A-ST0] (the smoother's ST0 input is
// not in the decomp; the disasm pins each one). Built x87 (.asi TU, no /arch:SSE2)
// so float10 transients (the `a*b+c` accumulators l_60/l_d0) round to float32 like
// the original FSTP. EXACT .rdata bit constants (A6_*): a literal-vs-.rdata sweep
// found 15/38 decimal literals mis-round (e.g. _DAT_005cea2c 299488 -> 0x48923c00
// vs the real 0x48927c00) — using Cf() bit patterns avoids the A5-class ULP drift.
// ===========================================================================
namespace {

int Fi_PlayerCount() { return reinterpret_cast<int(__cdecl*)()>(0x0040e340)(); }  // FUN_0040e340

// ── A6a EXACT-bit constants (memory_read, Mashed_pool12 2026-06-17) ──────────
const float A6_005cc320 = Cf(0x3f800000);  // 1.0
const float A6_005cc328 = Cf(0x3c23d70a);  // 0.01   speed-bias scale / damage scale
const float A6_005cc32c = Cf(0x3f000000);  // 0.5
const float A6_005cc31c = Cf(0x40400000);  // 3.0    type-C grip mul
const float A6_005cc348 = Cf(0x3fc00000);  // 1.5    type-A grip mul
const float A6_005cc55c = Cf(0x41200000);  // 10.0   speed bias addend
const float A6_005cc564 = Cf(0x3e800000);  // 0.25   fwd-align floor
const float A6_005cc56c = Cf(0x3dcccccd);  // 0.1    upgrade-stat scale / *0.2 clamp half
const float A6_005cc750 = Cf(0x41800000);  // 16.0   low-speed full-stop thr
const float A6_005cc990 = Cf(0x3727c5ac);  // 1e-5   ang-vel hi bound (EXACT)
const float A6_005cc9b4 = Cf(0x3f7d70a4);  // 0.99   spin-type vel damp (EXACT)
const float A6_005cc9c8 = Cf(0x3f666666);  // 0.9    brake scale
const float A6_005cc9f4 = Cf(0x41000000);  // 8.0    spin floor
const float A6_005ccabc = Cf(0x3f8ccccd);  // 1.1    high-load mul
const float A6_005ccac4 = Cf(0x43b40000);  // 360.0  spin->deg
const float A6_005ccd04 = Cf(0x43fa0000);  // 500.0  speed-delta floor
const float A6_005cd03c = Cf(0x38d1b717);  // 9.99999e-5 speed-min eps (EXACT)
const float A6_005cd0ac = Cf(0x44bb8000);  // 1500.0 speed-delta base
const float A6_005cd120 = Cf(0x42480000);  // 50.0   low-load thr
const float A6_005cd694 = Cf(0x3c888889);  // 0.0166667 spin damp (EXACT)
const float A6_005ce18c = Cf(0x3ca3d70a);  // 0.02   low-load scale
const float A6_005ce1e8 = Cf(0x3a2ec33e);  // 6.6667e-4 gear scale (EXACT)
const float A6_005ce264 = Cf(0x3f59999a);  // 0.85   high-load susp scale
const float A6_005ce9f0 = Cf(0x38000000);  // 3.0518e-5 low-grip slope (EXACT)
const float A6_005ce9f4 = Cf(0x33d6bf95);  // 9.9998e-8 hi-grip slope (EXACT)
const float A6_005ce9f8 = Cf(0x4b189680);  // 1.0e7   hi-grip ceiling
const float A6_005ce9fc = Cf(0x47000000);  // 32768.0 grip clamp thr
const float A6_005cea00 = Cf(0x35788da7);  // 9.2549e-7 ang-inertia scale (EXACT)
const float A6_005cea04 = Cf(0x4248f5c3);  // 50.24   normal-load accum
const float A6_005cea08 = Cf(0x3b000000);  // 0.0019531 high-load susp k (EXACT)
const float A6_005cea0c = Cf(0x3a800000);  // 0.0009766 susp force k (EXACT)
const float A6_005cea10 = Cf(0x44800000);  // 1024.0  speed clamp
const float A6_005cea14 = Cf(0x3c03126f);  // 0.008   spin scale
const float A6_005cea18 = Cf(0x3ca30eac);  // 0.0199045 spin scale 2 (EXACT)
const float A6_005cea1c = Cf(0xb727c5ac);  // -1e-5   ang-vel lo bound (EXACT)
const float A6_005cea2c = Cf(0x48927c00);  // 299488.0 type-C grip thr (EXACT)
const float A6_005cea30 = Cf(0x48435000);  // 200000.0 type-B grip thr
const float A6_005cea34 = Cf(0x48127c00);  // 149744.0 type-A grip thr (EXACT)
const float A6_005cea38 = Cf(0x48aae600);  // 349872.0 default grip thr (EXACT)
const float A6_005cea3c = Cf(0x43200000);  // 160.0   throttle/brake cap
const float A6_005cea40 = Cf(0x437a0000);  // 250.0   gear0 speed addend
const float A6_005cea28 = Cf(0x4af42400);  // 8e6     boost-state-1 player force
// _DAT_005cea20 (256.0) and _DAT_005cea24 (1.74533) are read directly inside the
// ST0 shims (RoundBrakeInput / RoundDriveProj) from the live .rdata.

// live globals A6a reads:
inline float& A6_SuspScale() { return *reinterpret_cast<float*>(0x0088e5f0); }  // _DAT_0088e5f0
inline int&   A6_BoostP1()   { return *reinterpret_cast<int*>  (0x0088e668); }  // DAT_0088e668
inline int&   A6_BoostP2()   { return *reinterpret_cast<int*>  (0x0088e66c); }  // DAT_0088e66c

inline float& a6F(int* v, int i) { return *reinterpret_cast<float*>(v + i); }
inline int    a6AsI(float f) { int i; std::memcpy(&i, &f, 4); return i; }

// ── the 7 implicit-ST0 FUN_004a2c48 (round-ST0-to-i64) shims ────────────────
// Each reproduces the EXACT x87 instruction stream that feeds ST0 at its call site
// (disasm Mashed_pool12 2026-06-17), then forwards to the LIVE FUN_004a2c48 so the
// round is bit-identical. dt is passed as the 2nd cdecl arg (the original FSUBs the
// spilled param_2 from the stack; an FSUB of the identical float32 is bit-exact).
//
// #1 0x004679ca  FILD [timer]; FSUB dt; CALL           (upshift-timer decrement)
// #2 0x004679f8  FILD [timer]; FADD dt; CALL           (downshift-timer increment)
// #3 0x00467cfe  FILD [bf4];   FSUB dt; CALL           (boost ctr, mode==6)
// #4 0x00467dc6  FILD [bf4];   FSUB dt; CALL           (boost-state 1)
// #5 0x00467e25  FILD [bf4];   FSUB dt; CALL           (boost-state 2; bf4 spilled)
// #7 0x00467f93  FILD [brake]; FADD [0x5cea20]; CALL   (wheel>1 brake quantize)
__declspec(naked) int RoundIntSubDt(int /*v*/, float /*dt*/) {
    __asm {
        fild  dword ptr [esp+4]            // ST0 = (float64)(int)v
        fsub  dword ptr [esp+8]            // ST0 -= dt
        call  dword ptr [PCH_Fwd_4a2c48]
        ret
    }
}
__declspec(naked) int RoundIntAddDt(int /*v*/, float /*dt*/) {
    __asm {
        fild  dword ptr [esp+4]
        fadd  dword ptr [esp+8]
        call  dword ptr [PCH_Fwd_4a2c48]
        ret
    }
}
// #7: ST0 = (float)brakeByte + _DAT_005cea20 (=256.0). The original FADDs the live
// .rdata float at 0x005cea20 — load it from there so the bit pattern is identical.
__declspec(naked) int RoundBrakeInput(int /*brakeByte*/) {
    __asm {
        fild  dword ptr [esp+4]
        mov   eax, 0x005cea20
        fadd  dword ptr [eax]
        call  dword ptr [PCH_Fwd_4a2c48]
        ret
    }
}
// #6 0x00467e7e: ST0 = (w[0x1f]*vel.x + w[0x20]*vel.y + w[0x21]*vel.z)
//                      / (w[-0xa] * _DAT_005cea24).
// Reproduce the exact FMUL/FADDP/FDIVP chain (0x467e4e..0x467e7e): EDI=wheel ptr,
// ESI=record. wheel passed as arg1, record as arg2.
__declspec(naked) int RoundDriveProj(float* /*wheel*/, float* /*rec*/) {
    __asm {
        mov   eax, dword ptr [esp+4]       // EAX = wheel (= original EDI)
        mov   edx, dword ptr [esp+8]       // EDX = record (= original ESI)
        fld   dword ptr [eax+0x80]         // w[0x20]
        fmul  dword ptr [edx+0x9b4]        // * vel.y
        fld   dword ptr [eax+0x7c]         // w[0x1f]
        fmul  dword ptr [edx+0x9b0]        // * vel.x
        faddp st(1), st(0)
        fld   dword ptr [edx+0x9b8]        // vel.z
        fmul  dword ptr [eax+0x84]         // * w[0x21]
        faddp st(1), st(0)
        fld   dword ptr [eax-0x28]         // w[-0xa]
        mov   ecx, 0x005cea24
        fmul  dword ptr [ecx]              // * 1.74533
        fdivp st(1), st(0)
        call  dword ptr [PCH_Fwd_4a2c48]
        ret
    }
}

// ── float10-preserving accumulator shims ([U-A6A-FLOAT10] resolution) ─────────
// FUN_004c3ac0 returns the magnitude in ST0 as float10 (the decompiler's
// `(float10)FUN_004c3ac0`); a `float`/`double` C return would round it at the ABI
// boundary, losing the extended precision the original carries into the subsequent
// FMUL/FADD before its single FSTP-to-float32. These shims keep ST0 in the FPU.

// local_60 += mag10(acvec) * le4   (stored back to float32 each wheel; disasm
// 0x468208..: FLD mag; FMUL le4; FADD local_60; FSTP local_60). Returns new f32.
__declspec(naked) float Accum60(float* /*acvec*/, float /*le4*/, float /*prev60*/) {
    __asm {
        mov   eax, dword ptr [esp+4]   // acvec
        push  eax
        call  dword ptr [PCH_Fwd_4c3ac0]   // ST0 = mag10(acvec)
        add   esp, 4
        fmul  dword ptr [esp+8]            // * le4   (float10)
        fadd  dword ptr [esp+12]           // + prev60
        ret                                // ST0 -> float return (rounds to f32)
    }
}
// local_d0 += mag10(evec) * 50.24  AND  *numOut = (float)(mag10(evec)*50.24).
// disasm 0x4684bb..0x4684d3: CALL mag; FMUL [0x5cea04]; FST [num]; FADD local_d0; FSTP.
__declspec(naked) float AccumD0Num(float* /*evec*/, float* /*numOut*/, float /*prevD0*/) {
    __asm {
        mov   eax, dword ptr [esp+4]   // evec
        push  eax
        call  dword ptr [PCH_Fwd_4c3ac0]   // ST0 = mag10(evec)
        add   esp, 4
        mov   ecx, 0x005cea04
        fmul  dword ptr [ecx]              // * 50.24 (float10)
        mov   edx, dword ptr [esp+8]       // numOut
        fst   dword ptr [edx]              // *numOut = round_f32(mag*50.24)  (keeps ST0)
        fadd  dword ptr [esp+12]           // + prevD0  (float10)
        ret                                // -> float32
    }
}
// the friction-torque divide + 3 accumulator updates, all in float10
// (disasm 0x4684d7..0x468534). num=float32 numerator; xvec=&local_84(the cross
// product {l84,l80,l7c}); acc=&local_78 {l78,l74,l70}. Returns void; updates acc[].
//   q       = num / mag10(xvec)                       (float10)
//   acc[0] (l78) = round( l78 - l84 * q )             (l84*q kept float10)
//   acc[1] (l74) = round( l74 - round(l80 * q) )
//   acc[2] (l70) = round( l70 - round(l7c * q) )
__declspec(naked) void FrictionUpdate(float /*num*/, float* /*xvec*/, float* /*acc*/) {
    __asm {
        push  ebx
        sub   esp, 4                       // scratch slot at [esp] for f32 rounding
        // args (post push ebx + sub 4 = +8): num=[esp+12] xvec=[esp+16] acc=[esp+20]
        mov   ebx, dword ptr [esp+16]      // xvec (&local_84)
        push  ebx
        call  dword ptr [PCH_Fwd_4c3ac0]   // ST0 = mag10(xvec)
        add   esp, 4
        fdivr dword ptr [esp+12]           // ST0 = num / mag10(xvec) = q  (float10)
        fld   dword ptr [ebx+4]            // local_80
        fmul  st(0), st(1)                 // local_80 * q  (float10)
        fld   dword ptr [ebx]              // local_84
        fmul  st(0), st(2)                 // local_84 * q  (float10, ST0)
        fld   dword ptr [ebx+8]            // local_7c
        fmul  st(0), st(3)                 // local_7c * q  (float10, ST0)
        // FPU: ST0=l7c*q  ST1=l84*q  ST2=l80*q  ST3=q
        mov   ecx, dword ptr [esp+20]      // acc (&local_78)
        fstp  dword ptr [esp]              // [esp] = round(l7c*q)  (pops ST0)
        fld   dword ptr [ecx+8]            // local_70
        fsub  dword ptr [esp]              // local_70 - round(l7c*q)
        fstp  dword ptr [ecx+8]            // store local_70 (f32)
        // FPU: ST0=l84*q  ST1=l80*q  ST2=q
        fxch  st(1)                        // ST0=l80*q  ST1=l84*q
        fstp  dword ptr [esp]              // [esp] = round(l80*q) (pops)
        fld   dword ptr [ecx+4]            // local_74
        fsub  dword ptr [esp]              // local_74 - round(l80*q)
        fstp  dword ptr [ecx+4]            // store local_74 (f32)
        // FPU: ST0=l84*q  ST1=q
        fld   dword ptr [ecx]              // local_78
        fsub  st(0), st(1)                 // local_78 - (l84*q)  (float10)
        fstp  dword ptr [ecx]              // store local_78 (f32)
        fstp  st(0)                        // pop l84*q
        fstp  st(0)                        // pop q
        add   esp, 4
        pop   ebx
        ret
    }
}
// drive-force accumulate (disasm 0x467ca4..0x467cd7): the drive product
// (local_e4 * local_cc) is kept in ST0 as float10 (ST1) across all 3 axis
// accumulates — NOT rounded to float32 between them. [U-A6A-FLOAT10].
//   prod  = e4 * cc                            (float10)
//   *b14  = (f32)( w[0x1f]*prod + *b14 )       (each w*prod in float10, rounded on store)
//   *b18  = (f32)( w[0x20]*prod + *b18 )
//   *b1c  = (f32)( w[0x21]*prod + *b1c )
// wheel=EDI (reads +0x7c/+0x80/+0x84), rec=ESI (+0xb14/+0xb18/+0xb1c).
__declspec(naked) void DriveForceAccum(float /*e4*/, float /*cc*/, float* /*wheel*/, float* /*rec*/) {
    __asm {
        mov   eax, dword ptr [esp+12]      // wheel (EDI)
        mov   edx, dword ptr [esp+16]      // rec (ESI)
        fld   dword ptr [esp+4]            // e4
        fmul  dword ptr [esp+8]            // * cc  -> prod (float10, ST0)
        fld   dword ptr [eax+0x7c]         // w[0x1f]
        fmul  st(0), st(1)                 // w[0x1f]*prod (float10)
        fadd  dword ptr [edx+0xb14]        // + *b14
        fstp  dword ptr [edx+0xb14]        // store b14 (f32)
        fld   dword ptr [eax+0x80]         // w[0x20]
        fmul  st(0), st(1)                 // w[0x20]*prod
        fadd  dword ptr [edx+0xb18]
        fstp  dword ptr [edx+0xb18]        // store b18 (f32)
        fld   dword ptr [eax+0x84]         // w[0x21]
        fmul  st(0), st(1)                 // w[0x21]*prod
        fadd  dword ptr [edx+0xb1c]
        fstp  dword ptr [edx+0xb1c]        // store b1c (f32)
        fstp  st(0)                        // pop prod
        ret
    }
}
// brake-force accumulate (disasm 0x467f40..0x467f70): bf = -(local_cc*0.9*brake)
// kept float10 (ST0) across the 3 axes; writes wheel+0x70/0x74/0x78 (= piVar12[0x1c..0x1e]).
//   prod = (local_cc * 0.9) * brake  (float10), negated
//   wheel[0x1c] = (f32)( prod * w[0x1f] + wheel[0x1c] )  (and 0x20->0x1d, 0x21->0x1e)
__declspec(naked) void BrakeForceAccum(float /*local_cc*/, float /*brake*/, float* /*wheel*/) {
    __asm {
        mov   eax, dword ptr [esp+12]      // wheel
        fld   dword ptr [esp+4]            // local_cc
        mov   ecx, 0x005cc9c8
        fmul  dword ptr [ecx]              // local_cc * 0.9
        fmul  dword ptr [esp+8]            // * brake  (float10)
        fchs                               // -(...) -> bf (float10, ST0)
        fld   st(0)                        // dup bf
        fmul  dword ptr [eax+0x7c]         // bf * w[0x1f]
        fadd  dword ptr [eax+0x70]         // + wheel[0x1c]
        fstp  dword ptr [eax+0x70]         // store (f32)
        fld   st(0)                        // dup bf
        fmul  dword ptr [eax+0x80]         // bf * w[0x20]
        fadd  dword ptr [eax+0x74]
        fstp  dword ptr [eax+0x74]
        fmul  dword ptr [eax+0x84]         // bf * w[0x21]  (consumes the last bf)
        fadd  dword ptr [eax+0x78]
        fstp  dword ptr [eax+0x78]
        ret
    }
}
// linear-velocity redistribution factor (disasm tail 0x46857c..0x468648).
//   d   = (float10)local_d0 - mag10(lvec)
//   if (d == 0)  -> *b8/*b4 unchanged, lin_b0 = (float10)local_b0
//   else fac = d / local_d0 (float10);
//        *b8 = (f32)(e0*fac + *b8); *b4 = (f32)(dc*fac + *b4);
//        lin_b0 = d8*fac + local_b0 (float10)
//   returns local_b0f = (f32)( (float10)bf1c + lin_b0 ).
__declspec(naked) float LinVelFactor(float* /*lvec*/, float /*local_d0*/, float /*e0*/,
                                     float /*dc*/, float /*d8*/, float* /*b8io*/, float* /*b4io*/,
                                     float /*bf1c*/, float /*b0prev*/) {
    __asm {
        push  ebx
        // args (post push ebx, +4): lvec=[esp+8] d0=[esp+12] e0=[esp+16] dc=[esp+20]
        //   d8=[esp+24] b8io=[esp+28] b4io=[esp+32] bf1c=[esp+36] b0prev=[esp+40]
        mov   ebx, dword ptr [esp+8]       // lvec
        push  ebx
        call  dword ptr [PCH_Fwd_4c3ac0]   // ST0 = mag10(lvec)
        add   esp, 4
        fld   dword ptr [esp+12]           // ST0=local_d0  ST1=mag10
        fsub  st(0), st(1)                 // ST0 = local_d0 - mag10 (float10)  ST1=mag10
        fxch  st(1)                        // ST0=mag10  ST1=(d)
        fstp  st(0)                        // pop mag10; ST0 = d (float10)
        ftst                               // compare d : 0.0
        fnstsw ax
        test  ah, 0x40                     // ZF: d == 0 ?
        jnz   d_is_zero
        // fac = d / local_d0  (float10), keep d? no — recompute fac in ST0
        fdiv  dword ptr [esp+12]           // ST0 = d / local_d0 = fac (float10)
        // *b8 = (f32)(e0*fac + *b8)
        mov   ecx, dword ptr [esp+28]      // b8io
        fld   dword ptr [esp+16]           // e0
        fmul  st(0), st(1)                 // e0*fac
        fadd  dword ptr [ecx]              // + *b8
        fstp  dword ptr [ecx]              // *b8 = f32
        // *b4 = (f32)(dc*fac + *b4)
        mov   edx, dword ptr [esp+32]      // b4io
        fld   dword ptr [esp+20]           // dc
        fmul  st(0), st(1)                 // dc*fac
        fadd  dword ptr [edx]              // + *b4
        fstp  dword ptr [edx]              // *b4 = f32
        // lin_b0 = d8*fac + local_b0  (float10, ST0)
        fld   dword ptr [esp+24]           // d8
        fmulp st(1), st(0)                 // d8*fac (consumes fac); ST0 = d8*fac
        fadd  dword ptr [esp+40]           // + b0prev  (float10)
        jmp   finish
    d_is_zero:
        fstp  st(0)                        // pop d
        fld   dword ptr [esp+40]           // ST0 = (float10)b0prev (= lin_b0)
    finish:
        // local_b0f = (f32)( bf1c + lin_b0 )
        fadd  dword ptr [esp+36]           // + bf1c (float10)
        pop   ebx
        ret                                // ST0 -> float32 return
    }
}

// Gear/CVT speed-cap precompute + 5-candidate loop — VERBATIM x87 (disasm
// 0x46768b..0x467820). fVar5 stays in the FPU as float10 through the whole loop
// (the bottom-of-stack ST2 in each candidate); fVar3 (per-candidate t) stays
// float10 in ST1; only local_54[k] and fVar4 round to f32 (FSTP). [U-A6A-FLOAT10]:
// a plain-C transcription rounds fVar5/fVar3 to f32 and drifts 1-3 ULP in local_cc
// -> the drive force on the significant X/Z axes (the near-zero Y always matched).
// out54[5] = local_54; returns iVar11. Increment iff (fVar5<fVar3 && k<4).
// EAX/ECX/EDX scratch; ESI/EDI/EBX preserved. fVar4 (f32) at [esp] scratch.
__declspec(naked) int GearCvtCompute(int* /*self*/, float* /*out54*/) {
    __asm {
        push  ebx
        push  edi
        push  esi
        sub   esp, 8                       // [esp]=fVar4 (f32 scratch)
        mov   esi, dword ptr [esp+24]      // self
        mov   edi, dword ptr [esp+28]      // out54
        mov   ecx, 0x005cc32c              // &0.5
        fld   dword ptr [esi+0x49c]
        fmul  dword ptr [ecx]              // [0x49c]*0.5  (float10)
        fld   dword ptr [esi+0x498]
        fmul  dword ptr [ecx]              // [0x498]*0.5  ST0; ST1=[0x49c]*0.5
        fstp  dword ptr [esp]              // fVar4tmp = round_f32([0x498]*0.5); ST0=[0x49c]*0.5
        mov   eax, 0x005cd0ac
        fld   dword ptr [eax]              // 1500
        fsub  dword ptr [esi+0xb0c]        // 1500 - slide
        mov   eax, 0x005ccd04
        fcom  dword ptr [eax]              // (1500-slide) : 500
        fnstsw ax
        test  ah, 0x41
        jp    gc_skipclamp
        fstp  st(0)
        fld   dword ptr [eax]              // 500.0
    gc_skipclamp:
        mov   eax, 0x005ce1e8
        fmul  dword ptr [eax]              // fVar5_base*6.6667e-4  ST0; ST1=[0x49c]*0.5
        fld   dword ptr [esp]              // [0x498]*0.5 (f32) ST0; ST1=fb; ST2=[0x49c]*0.5
        fmul  st(0), st(1)                 // ([0x498]*0.5)*fb = fVar4 (float10)
        fstp  dword ptr [esp]              // fVar4 = round_f32(...); ST0=fb; ST1=[0x49c]*0.5
        fmulp st(1), st(0)                 // fb*[0x49c]*0.5 = fVar5 (float10) -> ST0 (kept)
        xor   edx, edx                     // iVar11 = 0
        // From here ST0 = fVar5 (float10) and stays the BOTTOM of the stack: each
        // candidate pushes fVar3 (ST0) so fVar5 becomes ST1, then pushes fVar4 so
        // fVar5 is ST2 for the FDIV. The candidate pops back to leave fVar5 on top.

        // ---- candidate k=0 (col @0x478) ----
        fld   dword ptr [esi+0x478]
        mov   eax, 0x005d757c
        fcomp dword ptr [eax]              // col : 0
        fnstsw ax
        test  ah, 0x44
        jnp   gc_d0
        fld   dword ptr [esi+0x9e4]
        mov   eax, 0x005cea40
        fadd  dword ptr [eax]              // speed+250
        fmul  dword ptr [esi+0x478]        // *(0x478) -> fVar3 (k0 special)
        fcom  st(1)                        // fVar3 : fVar5
        fnstsw ax
        test  ah, 0x41
        jnz   gc_n0
        fstp  st(0)
        fld   st(0)                        // fVar3 = fVar5
        inc   edx                          // k0<4 -> incr
    gc_n0:
        fld   dword ptr [esp]              // fVar4 (f32)
        fdiv  dword ptr [esi+0x478]        // /col
        fdiv  st(0), st(2)                 // /fVar5
        fmul  st(0), st(1)                 // *fVar3
        fadd  dword ptr [esi+0x9e4]        // +speed
        fstp  dword ptr [edi+0]            // local_54[0]
        fstp  st(0)                        // drop fVar3
    gc_d0:
        // ---- candidate k=1 (col @0x47c) ----
        fld   dword ptr [esi+0x47c]
        mov   eax, 0x005d757c
        fcomp dword ptr [eax]
        fnstsw ax
        test  ah, 0x44
        jnp   gc_d1
        fld   dword ptr [esi+0x47c]
        fmul  dword ptr [esi+0x9e4]        // col*speed -> fVar3
        fcom  st(1)
        fnstsw ax
        test  ah, 0x41
        jnz   gc_n1
        fstp  st(0)
        fld   st(0)
        inc   edx
    gc_n1:
        fld   dword ptr [esp]
        fdiv  dword ptr [esi+0x47c]
        fdiv  st(0), st(2)
        fmul  st(0), st(1)
        fadd  dword ptr [esi+0x9e4]
        fstp  dword ptr [edi+4]            // local_54[1]
        fstp  st(0)
    gc_d1:
        // ---- candidate k=2 (col @0x480) ----
        fld   dword ptr [esi+0x480]
        mov   eax, 0x005d757c
        fcomp dword ptr [eax]
        fnstsw ax
        test  ah, 0x44
        jnp   gc_d2
        fld   dword ptr [esi+0x480]
        fmul  dword ptr [esi+0x9e4]
        fcom  st(1)
        fnstsw ax
        test  ah, 0x41
        jnz   gc_n2
        fstp  st(0)
        fld   st(0)
        inc   edx
    gc_n2:
        fld   dword ptr [esp]
        fdiv  dword ptr [esi+0x480]
        fdiv  st(0), st(2)
        fmul  st(0), st(1)
        fadd  dword ptr [esi+0x9e4]
        fstp  dword ptr [edi+8]            // local_54[2]
        fstp  st(0)
    gc_d2:
        // ---- candidate k=3 (col @0x484) ----
        fld   dword ptr [esi+0x484]
        mov   eax, 0x005d757c
        fcomp dword ptr [eax]
        fnstsw ax
        test  ah, 0x44
        jnp   gc_d3
        fld   dword ptr [esi+0x484]
        fmul  dword ptr [esi+0x9e4]
        fcom  st(1)
        fnstsw ax
        test  ah, 0x41
        jnz   gc_n3
        fstp  st(0)
        fld   st(0)
        inc   edx
    gc_n3:
        fld   dword ptr [esp]
        fdiv  dword ptr [esi+0x484]
        fdiv  st(0), st(2)
        fmul  st(0), st(1)
        fadd  dword ptr [esi+0x9e4]
        fstp  dword ptr [edi+12]           // local_54[3]
        fstp  st(0)
    gc_d3:
        // ---- candidate k=4 (col @0x488) — NO increment ----
        fld   dword ptr [esi+0x488]
        mov   eax, 0x005d757c
        fcomp dword ptr [eax]
        fnstsw ax
        test  ah, 0x44
        jnp   gc_d4
        fld   dword ptr [esi+0x488]
        fmul  dword ptr [esi+0x9e4]
        fcom  st(1)
        fnstsw ax
        test  ah, 0x41
        jnz   gc_n4
        fstp  st(0)
        fld   st(0)                        // k4: clamp but NO incr
    gc_n4:
        fld   dword ptr [esp]
        fdiv  dword ptr [esi+0x488]
        fdiv  st(0), st(2)
        fmul  st(0), st(1)
        fadd  dword ptr [esi+0x9e4]
        fstp  dword ptr [edi+16]           // local_54[4]
        fstp  st(0)
    gc_d4:
        fstp  st(0)                        // drop fVar5 (float10) — done
        add   esp, 8
        pop   esi
        pop   edi
        pop   ebx
        mov   eax, edx                     // return iVar11
        ret
    }
}

// 0x00467650 — A6a body. self=ESI record; param_1; dt=param_2; input=param_4.
void A6a_Body(int* self, int param_1, float dt, std::uint8_t* input) {
    void* v = self;
    int iVar8 = Fi_GameMode();
    int local_5c = iVar8;

    a6F(self, 0x9e4 / 4) = Live_004c3ac0(reinterpret_cast<float*>((char*)v + 0x9b0));   // 0x46766e
    a6F(self, 0x9e8 / 4) = Live_004c3ac0(reinterpret_cast<float*>((char*)v + 0x9bc));   // 0x467680

    float local_d0 = 0.0f, local_60 = 0.0f;   // [U-A6A-FLOAT10]: rounded to f32 each wheel
    float local_70 = 0, local_74 = 0, local_78 = 0;
    float local_64 = 0, local_68 = 0, local_6c = 0;
    float local_b0 = 0, local_b4 = 0, local_b8 = 0;

    // gear/CVT speed-cap selection — verbatim x87 (float10 fVar5/fVar3 retained).
    float local_54[5] = {0,0,0,0,0};
    int iVar11 = GearCvtCompute(self, local_54);

    // gear state machine (+0x490 gear, +0x494 timer)
    int gear = Ib(v, 0x490);
    if (iVar11 != gear) {
        if (Ib(v, 0x494) == 0 && gear < iVar11) { Ib(v, 0x494) = 3000; Ib(v, 0x490) = gear + 1; }
        if (iVar11 < Ib(v, 0x490)) { Ib(v, 0x494) = (int)0xfffff448; Ib(v, 0x490) = iVar11; }
    }
    gear = Ib(v, 0x490);
    float local_cc = local_54[(gear >= 0 && gear < 5) ? gear : 0];
    if (Ib(v, 0x494) > 0) {                                   // 0x4679ca call#1
        int r = RoundIntSubDt(Ib(v, 0x494), dt);
        Ib(v, 0x494) = r; if (r < 0) Ib(v, 0x494) = 0;
    }
    if (Ib(v, 0x494) < 0) {                                   // 0x4679f8 call#2
        int r = RoundIntAddDt(Ib(v, 0x494), dt);
        Ib(v, 0x494) = r; if (r > 0) Ib(v, 0x494) = 0;
    }

    // throttle clamp + type-grip threshold selection
    float fVar5b = DAT_005d757c;
    float thr = (float)(int)(unsigned)input[4];
    if (iVar8 != 7) { fVar5b = thr; if (A6_005cea3c < thr) fVar5b = A6_005cea3c; }
    iVar8 = Ib(v, 0x1f0);
    float fVar4b = Fb(v, 0x478 + gear * 4);
    float fVar6 = Fb(v, 0x9e4) * A6_005cc328 + A6_005cc55c;
    float fVar3 = A6_005cea34;
    if (iVar8 != -0x5f7f80 && (fVar3 = A6_005cea30, iVar8 != -0x557f80)) {
        fVar3 = A6_005cea38;
        if ((iVar8 == -0x69e1a6 || iVar8 == -0xe17f4c) && Ib(v, 0xd00) == 0) fVar3 = A6_005cea2c;
        if (iVar8 == -1 || iVar8 == -0x373738) {
            float fVar7 = (A6_005cc320 - dt * A6_005cd694 * A6_005cc328) * A6_005cc9b4;
            Fb(v, 0x9b0) = fVar7 * Fb(v, 0x9b0);
            Fb(v, 0x9b4) = fVar7 * Fb(v, 0x9b4);
            Fb(v, 0x9b8) = fVar7 * Fb(v, 0x9b8);
        }
    }
    bool bVar13 = fVar3 < ((local_cc * fVar4b) / fVar6) * fVar5b;
    if (bVar13) local_cc = local_cc * A6_005cc32c;
    unsigned local_58 = (unsigned)bVar13;

    // per-wheel loop (4 wheels, piVar12 = self + 0x1a4 bytes, stride 0x31 ints)
    int iVar10 = 0;
    int* piVar12 = self + (0x1a4 / 4);
    do {
        bool wheelDrive = false;   // decomp bVar13 (re-used local); start false each iter
        if (piVar12[-0xf] == 2 && piVar12[-3] != 0) {
            float local_e4 = (float)(int)(unsigned)input[4];
            if (local_5c == 7) local_e4 = DAT_005d757c;
            if (Ib(v, 0x28) != 0)
                local_e4 = (float)(100 - Ib(v, 0x28)) * local_e4 * A6_005cc328;
            if ((Ib(v, 0x1f0) == -0x69e1a6 || Ib(v, 0x1f0) == -0xe17f4c) && DAT_005d757c < Fb(v, 0x9e4)) {
                float nrm[3]; Live_004c39b0(nrm, reinterpret_cast<float*>((char*)v + 0x9b0));
                float fv = Fb(v, 0x9dc) * nrm[2] + Fb(v, 0x9d4) * nrm[0] + Fb(v, 0x9d8) * nrm[1];
                fv = fv * fv; fv = fv * fv; fv = fv * fv;
                if (fv < DAT_005d757c) fv = -fv;
                if (fv < A6_005cc564) fv = A6_005cc564;
                if (Ib(v, 0xd00) == 0 && fv < A6_005cc32c) fv = A6_005cc32c;
                local_e4 = fv * local_e4;
            }
            if (local_5c != 7 && local_e4 != DAT_005d757c) {
                wheelDrive = true;
                Ib(v, 0xb20) = 1;
                if (A6_005cea3c < local_e4) local_e4 = Cf(0x43200000);  // 160.0
                // drive product (local_e4*local_cc) kept float10 across the 3 axes
                DriveForceAccum(local_e4, local_cc,
                                reinterpret_cast<float*>(piVar12), reinterpret_cast<float*>(self));
            }
            if (Fi_GameMode() == 6) {                          // 0x467cfe call#3
                int r = RoundIntSubDt(Ib(v, 0xbf4), dt);
                Ib(v, 0xbf4) = r;
                if (3000 < r) Ib(v, 0xbf4) = 3000;
                if (Ib(v, 0xbf4) < 0) Ib(v, 0xbf4) = 0;
            }
            if (Ib(v, 0xbf8) == 1) {
                if (Ib(v, 0xbf4) == 0) { Ib(v, 0xbf8) = 0; }
                else {
                    int pc = Fi_PlayerCount();
                    float ff;
                    if (pc != 4 || (A6_BoostP1() != param_1 && A6_BoostP2() != param_1))
                        ff = Cf(0x4a989680);   // 5e6
                    else ff = A6_005cea28;     // 8e6 (player boost)
                    // boost force keeps ff in ST0 float10 across the 3 axes (disasm
                    // 0x467d7e..0x467db1: FLD ff; FLD w; FMUL ST1; FADD b14; FSTP).
                    // DriveForceAccum(e4=ff, cc=1.0): prod=ff*1.0=ff(exact), w*ff float10.
                    DriveForceAccum(ff, A6_005cc320,
                                    reinterpret_cast<float*>(piVar12), reinterpret_cast<float*>(self));
                    int r = RoundIntSubDt(Ib(v, 0xbf4), dt);     // 0x467dc6 call#4
                    Ib(v, 0xbf4) = r;
                    if (r < 1) { Ib(v, 0xbf8) = 0; Ib(v, 0xbf4) = 0; }
                }
            }
            if (Ib(v, 0xbf8) == 2) {
                if (Fb(v, 0xbf4) == DAT_005d757c) { Ib(v, 0xbf8) = 0; }
                else {
                    int saved = Ib(v, 0xbf4);
                    Ib(v, 0xb1c) = 0; Ib(v, 0xb18) = 0; Ib(v, 0xb14) = 0;
                    int r = RoundIntSubDt(saved, dt);            // 0x467e25 call#5
                    Ib(v, 0xbf4) = r;
                    if (r < 1) { Ib(v, 0xbf8) = 0; Ib(v, 0xbf4) = 0; }
                }
            }
        }
        // *piVar12 = round( drive-proj )                       // 0x467e7e call#6
        piVar12[0] = RoundDriveProj(reinterpret_cast<float*>(piVar12),
                                    reinterpret_cast<float*>(self));
        if (local_58 == 0 || piVar12[-0xf] != 2) {
            piVar12[-1] = 0;
        } else {
            int p1 = piVar12[-1] + 1;
            piVar12[-1] = p1;
            if (p1 < 0x200) piVar12[-1] = 0x200;
            piVar12[0] = piVar12[0] + (piVar12[-1] - 0x200) * 0x40;
        }

        // ===== contact-frame block (wheel active) =====
        if (piVar12[-3] != 0) {
            if (input[5] != 0 && input[4] == 0) {
                Ib(v, 0xb20) = 1;
                if (piVar12[0] < 1) {
                    if (piVar12[-0xf] == 2) {
                        float bf = (float)(int)(unsigned)input[5];
                        if (Ib(v, 0x28) != 0)
                            bf = bf * (float)(100 - Ib(v, 0x28)) * A6_005cc328;
                        if (A6_005cea3c < bf) bf = A6_005cea3c;
                        // bf=-(local_cc*0.9*bf) kept float10 across the 3 axes
                        // (disasm 0x467f40..0x467f70). Writes wheel+0x70/0x74/0x78.
                        BrakeForceAccum(local_cc, bf, reinterpret_cast<float*>(piVar12));
                    }
                } else if (1 < iVar10) {
                    piVar12[-1] = RoundBrakeInput((int)(unsigned)input[5]);   // 0x467f93 call#7
                }
            }
            // orientation/spin check
            float le0, ldc, ld8, le4;
            float avx = Fb(v, 0x9bc), avy = Fb(v, 0x9c0), avz = Fb(v, 0x9c4);
            if (avx < A6_005cea1c || A6_005cc990 < avx || avy < A6_005cea1c || A6_005cc990 < avy ||
                avz < A6_005cea1c || A6_005cc990 < avz) {
                unsigned char m40[64];
                Live_004c4d20(m40, reinterpret_cast<float*>((char*)v + 0x9bc), Cf(0x43870000), 0); // 270.0
                float src[3] = { a6F(piVar12, -9), a6F(piVar12, -8), a6F(piVar12, -7) }, dst[3];
                Live_004c3df0(dst, src, 1, m40);
                le0 = dst[0]; ldc = dst[1]; ld8 = dst[2];
                float f = Fb(v, 0x9e4) * A6_005cea14;
                if (f < A6_005cc9f4) f = A6_005cc9f4;
                f = f * Fb(v, 0x9e8) * A6_005cea18 * A6_005ccac4;
                le0 = le0 * f - Fb(v, 0x9b0);
                ldc = ldc * f - Fb(v, 0x9b4);
                ld8 = ld8 * f - Fb(v, 0x9b8);
                float tmp[3] = { le0, ldc, ld8 };
                le4 = Live_004c3ac0(tmp);
            } else {
                le4 = Fb(v, 0x9e4);
                le0 = -Fb(v, 0x9b0); ldc = -Fb(v, 0x9b4); ld8 = -Fb(v, 0x9b8);
            }
            if (A6_005cd03c < le4 && Ib(v, 0x9e0) == 0x40800000) {
                float fInv = A6_005cc320 / le4;
                float s0 = le0 * fInv, s1 = ldc * fInv;
                float s2 = fInv * ld8;   // pre-computed (ASM ST2); reused by local_c0 & local_a4
                if (A6_005cea10 < le4) le4 = Cf(0x44800000);   // 1024.0
                float local_98 = 0, local_9c = 0, local_a0 = 0;
                float local_bc = a6F(piVar12, 0x15) * a6F(piVar12, 0x1b) * A6_SuspScale() * le4 * A6_005cea0c;
                // dot = fwd.z*s2 + fwd.x*s0 + fwd.y*s1 (ASM 0x4681c1: fwd.z*(inv*ld8))
                float local_c0 = Fb(v, 0x9dc) * s2 + Fb(v, 0x9d4) * s0 + Fb(v, 0x9d8) * s1;
                float local_c8 = local_c0 * a6F(piVar12, 0x1f);
                float local_c4 = local_c0 * a6F(piVar12, 0x20);
                local_c0 = local_c0 * a6F(piVar12, 0x21);
                float local_ac = s0 - local_c8;
                float local_a8 = s1 - local_c4;
                float local_a4 = s2 - local_c0;
                float acvec[3] = { local_ac, local_a8, local_a4 };
                float local_d4 = Live_004c3ac0(acvec);   // f32 magnitude (local_d4 uses)
                unsigned local_94 = (unsigned)piVar12[-1];
                // local_60 += mag10(acvec)*le4 (float10 FMA, rounded f32) [U-A6A-FLOAT10]
                local_60 = Accum60(acvec, le4, local_60);
                if ((local_94 & 0x100) == 0) {
                    float f5 = local_bc;
                    if (local_94 == 0) {
                        local_98 = a6F(piVar12, 0x16) * a6F(piVar12, 0x1b) * A6_SuspScale();
                        local_a0 = local_c8 * local_98;
                        local_9c = local_c4 * local_98;
                        local_98 = local_98 * local_c0;
                        float f4 = local_d4 * Fb(v, 0x9e4);
                        if (f4 < A6_005cd120) f5 = f4 * A6_005ce18c * local_bc;
                    }
                    local_a0 = local_ac * f5 + local_a0;
                    local_9c = local_a8 * f5 + local_9c;
                    f5 = f5 * local_a4 + local_98;
                    Fb(piVar12, 0x70) = local_a0 + Fb(piVar12, 0x70);
                    Fb(piVar12, 0x74) = local_9c + Fb(piVar12, 0x74);
                    Fb(piVar12, 0x78) = f5 + Fb(piVar12, 0x78);
                } else {
                    float f5 = local_bc * A6_005ce264;
                    float f4 = a6F(piVar12, 0x1b) * A6_005cc31c * A6_SuspScale() * (float)(int)local_94 *
                               A6_005cea08 * A6_005ccabc;
                    if (bVar13) f4 = f4 * A6_005cc32c;
                    local_a0 = local_c8 * f4 + local_ac * f5;
                    local_9c = local_c4 * f4 + local_a8 * f5;
                    f5 = local_c0 * f4 + f5 * local_a4;
                    Fb(piVar12, 0x70) = local_a0 + Fb(piVar12, 0x70);
                    Fb(piVar12, 0x74) = local_9c + Fb(piVar12, 0x74);
                    Fb(piVar12, 0x78) = f5 + Fb(piVar12, 0x78);
                }
            }
        }

        // ===== cross-product friction block (per-wheel force -> torque accum) =====
        float fvec[3] = { a6F(piVar12, 0x1c), a6F(piVar12, 0x1d), a6F(piVar12, 0x1e) };
        if ((double)A6_005cd03c < (double)Live_004c3ac0(fvec)) {
            float local_88 = A6_005cc320 / a6F(piVar12, -0xb);
            float local_90 = a6F(piVar12, -9) * local_88;
            float local_8c = a6F(piVar12, -8) * local_88;
            local_88 = a6F(piVar12, -7) * local_88;
            float pf = a6F(piVar12, 0x1c);
            float fv5 = local_88 * a6F(piVar12, 0x1e) + local_8c * a6F(piVar12, 0x1d) + local_90 * pf;
            float local_c8 = local_90 * fv5;
            float local_c4 = local_8c * fv5;
            float local_c0 = local_88 * fv5;
            float local_ac = pf - local_c8;
            float local_a8 = a6F(piVar12, 0x1d) - local_c4;
            float local_a4 = a6F(piVar12, 0x1e) - local_c0;
            float local_84 = local_a4 * local_c4 - local_a8 * local_c0;
            float local_80 = local_ac * local_c0 - local_a4 * local_c8;
            float local_7c = local_a8 * local_c8 - local_ac * local_c4;
            if (DAT_005d757c < fv5) { local_84 = -local_84; local_80 = -local_80; local_7c = -local_7c; }
            local_b8 = local_c8 + local_b8;
            local_b4 = local_c4 + local_b4;
            local_b0 = local_c0 + local_b0;
            local_6c = local_6c + pf;
            local_68 = local_68 + a6F(piVar12, 0x1d);
            local_64 = local_64 + a6F(piVar12, 0x1e);
            float wm = a6F(piVar12, -0xb);
            float evec[3] = { local_ac * wm, local_a8 * wm, local_a4 * wm };
            // local_d0 += mag10(evec)*50.24 (float10, f32-rounded); num = round(mag10*50.24)
            float num;
            local_d0 = AccumD0Num(evec, &num, local_d0);                // [U-A6A-FLOAT10]
            float xvec[3] = { local_84, local_80, local_7c };
            if (A6_005cd03c < Live_004c3ac0(xvec)) {
                // q = num / mag10(xvec) (float10); l78 -= l84*q (float10),
                // l74 -= round(l80*q), l70 -= round(l7c*q). acc = {l78,l74,l70}.
                float acc[3] = { local_78, local_74, local_70 };
                FrictionUpdate(num, xvec, acc);
                local_78 = acc[0]; local_74 = acc[1]; local_70 = acc[2];
            }
        }

        iVar10 = iVar10 + 1;
        piVar12 = piVar12 + 0x31;
    } while (iVar10 < 4);

    // angular-velocity integration (+0x9bc..) when state +0x10 == 0
    float local_d8 = dt * Fb(v, 0x5c) * A6_005cea00;
    float local_e0a = local_78 * local_d8;
    float local_dca = local_74 * local_d8;
    local_d8 = local_d8 * local_70;
    if (Ib(v, 0x10) == 0) {
        Fb(v, 0x9bc) = local_e0a + Fb(v, 0x9bc);
        Fb(v, 0x9c0) = local_dca + Fb(v, 0x9c0);
        Fb(v, 0x9c4) = local_d8 + Fb(v, 0x9c4);
    }
    float lvec[3] = { local_78, local_74, local_70 };
    // float10 chain: fac=(local_d0-mag10(lvec))/local_d0; local_b8/b4 = (f32)(e0*fac+b8)
    // etc.; lin_b0 = d8*fac+local_b0 (float10) -> local_b0f = (f32)(Fb(0xb1c)+lin_b0).
    float e0 = local_6c - local_b8, dc = local_68 - local_b4, d8 = local_64 - local_b0;
    float local_b0f = LinVelFactor(lvec, local_d0, e0, dc, d8,
                                   &local_b8, &local_b4, Fb(v, 0xb1c), local_b0);
    float local_d0f = dt * Fb(v, 0x54) * Cf(0x39aec33e);    // _DAT_005cc948 (3.33e-4)
    float local_90f = local_d0f * (Fb(v, 0xb14) + local_b8);
    Fb(v, 0x9b0) = local_90f + Fb(v, 0x9b0);
    Fb(v, 0x9b4) = local_d0f * (Fb(v, 0xb18) + local_b4) + Fb(v, 0x9b4);
    Fb(v, 0x9b8) = local_d0f * local_b0f + Fb(v, 0x9b8);
    float speed = Live_004c3ac0(reinterpret_cast<float*>((char*)v + 0x9b0));
    int tId = Ib(v, 0x1f0);
    float grip = (float)(local_60 / (double)Fb(v, 0x18c));
    Fb(v, 0x9e4) = speed;
    if (tId == -0x5f7f80) {
        grip = grip * A6_005cc348;
    } else {
        bool doDouble = (tId == -0x557f80);
        if (!doDouble) {
            if (tId == -0x69e1a6 || tId == -0xe17f4c)
                grip = grip * (Ib(v, 0xd00) == 0 ? A6_005cc31c : A6_005cc348);
            if (tId == -0x37e1a6) doDouble = true;
        }
        if (doDouble) grip = grip + grip;
    }
    if (Ib(v, 0x2c) != 0) grip = ((float)Ib(v, 0x2c) * A6_005cc328 + A6_005cc320) * grip;
    if (Ib(v, 0x34) != 0) grip = ((float)Ib(v, 0x34) * A6_005cc56c + A6_005cc320) * grip;

    // ===== trailing speed-limit grip-clamp (all-grounded) =====
    if (speed != DAT_005d757c && Ib(v, 0x9e0) == 0x40800000) {
        float fwdDot = Fb(v, 0x9dc) * Fb(v, 0x9b8) + Fb(v, 0x9d4) * Fb(v, 0x9b0) + Fb(v, 0x9d8) * Fb(v, 0x9b4);
        float fx = Fb(v, 0x9b0) - fwdDot * Fb(v, 0x9d4);
        float fy = Fb(v, 0x9b4) - fwdDot * Fb(v, 0x9d8);
        float fz = Fb(v, 0x9b8) - fwdDot * Fb(v, 0x9dc);
        grip = grip * speed;
        if (A6_005ce9fc < grip) {
            float k = (A6_005ce9f8 - grip) * A6_005ce9f4;
            if (k < DAT_005d757c) k = DAT_005d757c;
            k = k * A6_005cc56c + k * A6_005cc56c;            // *0.2
            Fb(v, 0x9b0) = Fb(v, 0x9b0) - fx * k;
            Fb(v, 0x9b4) = Fb(v, 0x9b4) - fy * k;
            Fb(v, 0x9b8) = Fb(v, 0x9b8) - fz * k;
            k = A6_005cc320 - k;
            Fb(v, 0x9bc) = k * Fb(v, 0x9bc);
            Fb(v, 0x9c0) = k * Fb(v, 0x9c0);
            Fb(v, 0x9c4) = k * Fb(v, 0x9c4);
            return;
        }
        float k = (A6_005ce9fc - grip) * A6_005ce9f0;
        if (k < A6_005cc56c) k = A6_005cc56c;
        Fb(v, 0x9b0) = Fb(v, 0x9b0) - fx * k;
        Fb(v, 0x9b4) = Fb(v, 0x9b4) - fy * k;
        Fb(v, 0x9b8) = Fb(v, 0x9b8) - fz * k;
        if (k < A6_005cc32c) k = A6_005cc32c;
        k = A6_005cc320 - k;
        Fb(v, 0x9bc) = k * Fb(v, 0x9bc);
        Fb(v, 0x9c0) = k * Fb(v, 0x9c0);
        Fb(v, 0x9c4) = k * Fb(v, 0x9c4);
        if (Ib(v, 0xb20) == 0 && speed < A6_005cc750) {
            Fb(v, 0x9b0) = 0; Ib(v, 0x9b8) = 0; Ib(v, 0x9b4) = 0;
            Ib(v, 0x9c4) = 0; Ib(v, 0x9c0) = 0; Fb(v, 0x9bc) = 0;
            return;
        }
    }
    (void)param_1;
}

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
int g_a6aIdle  = 0;
const int kA6aMaxTests = 96;

void A6a_SelfTest(int* record, int param_1, float dt, void* param_3, std::uint8_t* input) {
    if (g_a6aCount >= kA6aMaxTests) return;
    const bool hasInput = (input[4] != 0) || (input[5] != 0);
    if (!hasInput) { if (g_a6aIdle >= 8) return; ++g_a6aIdle; }

    static std::uint8_t snapRec[0xd04];
    std::memcpy(snapRec, record, 0xd04);

    // (1) ORIGINAL A6a on the live record (bypassing our inline-JMP).
    Call_OrigA6a(record, param_1, dt, param_3, input);

    // capture original outputs
    std::uint32_t origRec[24]; int nr = 0;
    for (int off : kA6aRecOffs) origRec[nr++] = *reinterpret_cast<std::uint32_t*>((char*)record + off);
    std::uint32_t origStar[4], origPrev[4], origFx[4], origFz[4];
    for (int i = 0; i < 4; ++i) {
        origStar[i] = *reinterpret_cast<std::uint32_t*>((char*)record + kA6aWheelStar[i]);
        origPrev[i] = *reinterpret_cast<std::uint32_t*>((char*)record + kA6aWheelPrev[i]);
        origFx[i]   = *reinterpret_cast<std::uint32_t*>((char*)record + kA6aWheelFx[i]);
        origFz[i]   = *reinterpret_cast<std::uint32_t*>((char*)record + kA6aWheelFx[i] + 8);
    }

    // restore the record, run MY body
    std::memcpy(record, snapRec, 0xd04);
    A6a_Body(record, param_1, dt, input);

    int mism = 0; char line[1024]; int p = 0;
    nr = 0;
    for (int off : kA6aRecOffs) {
        std::uint32_t mv = *reinterpret_cast<std::uint32_t*>((char*)record + off);
        if (mv != origRec[nr]) { mism++;
            if (p < 900) p += wsprintfA(line+p, " +0x%x:o=%08x,m=%08x", off, origRec[nr], mv); }
        nr++;
    }
    for (int i = 0; i < 4; ++i) {
        std::uint32_t s = *reinterpret_cast<std::uint32_t*>((char*)record + kA6aWheelStar[i]);
        std::uint32_t pv= *reinterpret_cast<std::uint32_t*>((char*)record + kA6aWheelPrev[i]);
        std::uint32_t fx= *reinterpret_cast<std::uint32_t*>((char*)record + kA6aWheelFx[i]);
        std::uint32_t fz= *reinterpret_cast<std::uint32_t*>((char*)record + kA6aWheelFx[i] + 8);
        if (s  != origStar[i]) { mism++; if (p<900) p += wsprintfA(line+p, " w%d*:o=%08x,m=%08x", i, origStar[i], s); }
        if (pv != origPrev[i]) { mism++; if (p<900) p += wsprintfA(line+p, " w%dp:o=%08x,m=%08x", i, origPrev[i], pv); }
        if (fx != origFx[i])   { mism++; if (p<900) p += wsprintfA(line+p, " w%dfx:o=%08x,m=%08x", i, origFx[i], fx); }
        if (fz != origFz[i])   { mism++; if (p<900) p += wsprintfA(line+p, " w%dfz:o=%08x,m=%08x", i, origFz[i], fz); }
    }

    char hdr[220];
    int grounded = *reinterpret_cast<int*>((char*)record + 0x9e0);
    wsprintfA(hdr, "[%d] dt=%08x in4=%u in5=%u gm=%d grounded=%08x ndiff=%d%s\r\n",
              g_a6aCount, *reinterpret_cast<std::uint32_t*>(&dt), input[4], input[5],
              Fi_GameMode(), grounded, mism, mism ? "" : " OK");
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
        A6a_Body(record, param_1, dt, input);
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

RH_ScopedInstall(A4_Entry, 0x00470670);
RH_ScopedInstall(A5_Entry, 0x0046ddb0);
RH_ScopedInstall(A6a_Entry, 0x00467650);
