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
            a5F(self, 0x278) = (float)self[0x278] + A5_005cc320;                        // contact -> grounded++ (FLOAT store)
        piVar10[0] = piVar12[0];
        piVar10[1] = 0;
        piVar10[2] = piVar12[2];
        Live_004c3df0(reinterpret_cast<float*>(piVar12 + 5), reinterpret_cast<float*>(piVar10), 1, xform);
        if ((float)piVar12[0xf] == DAT_005d757c) {
            piVar12[0x2d] = self[0x275];
            piVar12[0x2e] = self[0x276];
            piVar12[0x2f] = self[0x277];
        } else {
            unsigned char m[64];
            Live_004c4d20(m, A5_DAT_006146fc, (float)piVar12[0xf], 0);                  // 0x0046dxx FUN_004c4d20
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
    float fVar4 = (float)self[0x279];
    if ((float)self[0x279] < A5_005cd03c) fVar4 = DAT_005d757c;
    fVar4 = (float)self[0x54] * (float)self[0x55] * (float)self[0x56] * fVar4;
    if (self[0x278] != 0x40800000) fVar4 = fVar4 * A5_005cc32c;
    fVar4 = fVar4 * (float)self[0x15] * dt * A5_005cc948;

    // ── Phase 2: drafting / proximity grip reduction (local_70) ──
    int iVar11 = 0, iVar13 = 0;
    float local_70 = 1.0f;
    do {
        if ((iVar11 != *self) && (*reinterpret_cast<int*>(gb + 4 + iVar13) == 1)) {
            int iVar3 = self[0x26a];
            int iVar8 = *reinterpret_cast<int*>(gb + (0x881f48 - 0x8815a0) + iVar13) * 0x40;
            int iVar9 = iVar8 + iVar13;
            float local_64 = *reinterpret_cast<float*>(gb + (0x881ef8 - 0x8815a0) + iVar13 + iVar8)
                             - (float)self[iVar3 * 0x10 + 0x256];
            float local_60 = *reinterpret_cast<float*>(gb + (0x881efc - 0x8815a0) + iVar9)
                             - (float)self[iVar3 * 0x10 + 599];
            float local_58 = *reinterpret_cast<float*>(gb + (0x881f50 - 0x8815a0) + iVar13);
            float local_5c = *reinterpret_cast<float*>(gb + (0x881f00 - 0x8815a0) + iVar9)
                             - (float)self[iVar3 * 0x10 + 600];
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
    a5F(self, 0x26c) = fVar4 * (float)self[0x26c];
    a5F(self, 0x26d) = fVar4 * (float)self[0x26d];
    a5F(self, 0x26e) = fVar4 * (float)self[0x26e];
    float local_5c = dt * (float)self[0x279] * A5_005cc990;
    if (A5_005cc574 < local_5c) local_5c = A5_005cc574;
    local_5c = local_5c * A5_GravScale();
    float local_64 = A5_GravX() * local_5c;
    float local_60 = A5_GravY() * local_5c;
    local_5c = A5_GravZ() * local_5c;
    a5F(self, 0x26c) = local_64 + (float)self[0x26c];
    a5F(self, 0x26d) = local_60 + (float)self[0x26d];
    a5F(self, 0x26e) = local_5c + (float)self[0x26e];

    // ── Phase 5: steer torque from velocity delta (any wheel grounded) ──
    if ((float)self[0x278] != DAT_005d757c) {
        int hi = self[0x2b4];
        unsigned int lo = (unsigned int)(hi - 1) & 1;
        local_64 = ((float)self[hi * 3 + 0x2b5] - (float)self[lo * 3 + 0x2b5]) * dt;
        local_60 = ((float)self[hi * 3 + 0x2b6] - (float)self[lo * 3 + 0x2b6]) * dt;
        local_5c = ((float)self[hi * 3 + 0x2b7] - (float)self[lo * 3 + 0x2b7]) * dt;
        fVar4 = ((float)self[0x14] / (float)self[0x278]) * A5_SuspDtTerm();
        fVar5 = fVar4 * A5_005ccac8;
        // wheel0  -> self[0x83], scratch[1] (DAT_00881564)
        if (self[0x66] == 0) { self[0x83] = 0; }
        else {
            float f = ((float)self[0x62] * local_5c + (float)self[0x60] * local_64 + (float)self[0x61] * local_60) * A5_005ce018;
            if (fVar5 < f) f = fVar5;
            if (f < -fVar5) f = -fVar5;
            a5F(self, 0x83) = fVar4 + f;
            if (f <= A5_005ccd6c) { if (f < A5_005cd61c) f = A5_005cd61c; a5F(scratch, 1) = f * A5_005cea6c; }
            else { a5F(scratch, 1) = A5_005ccd6c * A5_005cea6c; }
        }
        // wheel1  -> self[0xb4], scratch[4] (DAT_00881570)
        if (self[0x97] == 0) { self[0xb4] = 0; }
        else {
            float f = ((float)self[0x93] * local_5c + (float)self[0x91] * local_64 + (float)self[0x92] * local_60) * A5_005ce018;
            if (fVar5 < f) f = fVar5;
            if (f < -fVar5) f = -fVar5;
            a5F(self, 0xb4) = fVar4 + f;
            if (f <= A5_005ccd6c) { if (f < A5_005cd61c) f = A5_005cd61c; a5F(scratch, 4) = f * A5_005cea6c; }
            else { a5F(scratch, 4) = A5_005ccd6c * A5_005cea6c; }
        }
        // wheel2  -> self[0xe5], scratch[7] (DAT_0088157c)
        if (self[200] == 0) { self[0xe5] = 0; }
        else {
            float f = ((float)self[0xc4] * local_5c + (float)self[0xc2] * local_64 + (float)self[0xc3] * local_60) * A5_005ce018;
            if (fVar5 < f) f = fVar5;
            if (f < -fVar5) f = -fVar5;
            a5F(self, 0xe5) = fVar4 + f;
            if (f <= A5_005ccd6c) { if (f < A5_005cd61c) f = A5_005cd61c; a5F(scratch, 7) = f * A5_005cea6c; }
            else { a5F(scratch, 7) = A5_005ccd6c * A5_005cea6c; }
        }
        // wheel3  -> self[0x116], scratch[10] (DAT_00881588)
        if (self[0xf9] == 0) { self[0x116] = 0; }
        else {
            float f = ((float)self[0xf5] * local_5c + (float)self[0xf3] * local_64 + (float)self[0xf4] * local_60) * A5_005ce018;
            if (fVar5 < f) f = fVar5;
            if (f < -fVar5) f = -fVar5;
            a5F(self, 0x116) = fVar4 + f;
            if (f <= A5_005ccd6c) { if (f < A5_005cd61c) f = A5_005cd61c; a5F(scratch, 10) = f * A5_005cea6c; }
            else { a5F(scratch, 10) = A5_005ccd6c * A5_005cea6c; }
        }
        a5F(self, 0x116) = (float)self[0x116] * A5_005cc32c;
        unsigned int ring = (unsigned int)(self[0x2fb] + 1) & 0xf;
        self[0x2fb] = (int)ring;
        Call_FaceNormal(reinterpret_cast<float*>(self + (ring * 3 + 0x2cb)));   // FUN_0046c5f0 (live)
        int slot = ring * 3 + 0x2cd;
        if ((float)self[slot] < DAT_005d757c) a5F(self, slot) = (float)self[slot] * A5_005cc564;
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
                float rr = (float)self[0x279] * A5_005cea68;
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
            if (self[0xb] != 0) *pfVar14 = (float)self[0xb] * *pfVar14 * A5_005cc328;
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
            if ((float)self[0x278] < A5_005cc31c) {
                self[0x2c8] = 1;
                a5F(self, 0x2c6) = (float)self[0x2c6] - A5_SuspDtTerm() * (float)self[0x14] * A5_005ccd08;
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
        mov  eax, dword ptr [esp+8]   // 8b442408  (original prologue instr 1)
        sub  esp, 0x70                // 83ec70    (original prologue instr 2)
        mov  eax, 0x0046ddb7          // -> first untouched original instruction (PUSH EBX)
        jmp  eax
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

RH_ScopedInstall(A4_Entry, 0x00470670);
RH_ScopedInstall(A5_Entry, 0x0046ddb0);
