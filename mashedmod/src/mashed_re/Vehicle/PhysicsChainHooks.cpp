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
} }
// [dcphys 2026-10-21] The .asi's DEFAULT for the shared A6a body = what its former verbatim copy computed (bit-identical
// on every sampled call, verify/dcphys_20261021): the A6a knobs the standalone seeds through Core/FaithDefaults.cpp
// (exe only, so the .asi never sees them) plus the drive arm's LUT normalise, which the verbatim copy called live
// (0x004c39b0). MASHED_<name> already set is left as given. A knob only, never a second body.
namespace {
struct A6aAsiDefaults {
    A6aAsiDefaults() {
        const char* const ks[] = { "MASHED_A6A_LC0", "MASHED_A6A_GEAR_ORDER", "MASHED_A6A_SPIN_ORDER",
                                   "MASHED_A6A_DRIVE_NORM_LUT" };
        for (const char* k : ks) if (!std::getenv(k)) _putenv_s(k, "1");
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
// A6b FUN_00468980(dt, input) with ECX = record and ESI = wheelBlock (the +0x928
// RwMatrix). CORRECTED 2026-09-29 (U-9149): this forwarder set ESI = record, which
// the dispatch site does not support. From the anchored image:
//   0047093b  MOV ESI,[ESP+0x3c]   ; ESP = E-0x30 here -> E+0x0c
//   00470941  MOV ECX,EDI          ; record
//   00470943  CALL 0x468980
// and E+0x0c is the slot A4 filled at 0x004706a2 with LEA [EAX+EDI+0x928]
// (EAX = [EDI+0x9a8]<<6, 0x0047068e/0x00470696) — i.e. wheelBlock, the very same
// pointer Call_A5 gets as arg 2 (MOV EDX,[ESP+0x20] @0x00470918) and Call_A6a as
// arg 3 (MOV EAX,[ESP+0x2c] @0x00470928). A6b passes ESI straight to RwMatrixRotate
// (0x00468a31 / 0x00468aaf / 0x00468b29) and reads [ESI+0x24] / [ESI+4], so it is an
// RwMatrix; 0x40 == sizeof(RwMatrix).
// NOTE the two readings this replaces were BOTH wrong: `mov esi, ecx` (record), and
// the claim at the A6b self-test block below that ESI is A4's param_4 / xform —
// param_4 lives at E+0x10 and A5 never reads it (balanced ESP walk 0x0046ddb0 ->
// RET 0x0046e9d0: caller-frame refs are E5+0x4 and E5+0x8 only).
__declspec(naked) void Call_A6b(void* /*record*/, float /*dt*/, void* /*input*/, int* /*wheelBlock*/) {
    __asm {
        push esi
        mov  ecx, dword ptr [esp+8]    // record     -> ECX (A6b reads [ECX+0x9e0])
        mov  esi, dword ptr [esp+20]   // wheelBlock -> ESI (A6b reads [ESI+0x24])
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
void A4_SelfTest(void* record, int param_1, float dt, std::uint8_t* input, void* xform,
                 int gameMode, unsigned phase) {
    const bool brake  = (input[1] != 0);
    const bool accel  = (input[0] != 0) && !brake;
    const bool hiIn5  = ((input[0] != 0) || brake) && (input[5] > 128);
    const bool idle   = (input[0] == 0) && (input[1] == 0);
    // admit the call only if it advances some branch quota that is not yet full
    bool admit = false;
    if (brake && g_selfTestBrake < kMaxBrake) admit = true;
    if (hiIn5 && g_selfTestHiIn5 < kMaxHiIn5) admit = true;
    if (accel && g_selfTestAccel < kMaxAccel) admit = true;
    if (idle  && g_selfTestIdle  < kMaxIdle ) admit = true;
    if (!admit) return;
    if (idle)  ++g_selfTestIdle;
    if (accel) ++g_selfTestAccel;
    if (brake) ++g_selfTestBrake;
    if (hiIn5) ++g_selfTestHiIn5;
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
    char hdr[200];
    int slot = (int)(((std::uintptr_t)record - 0x008815a0) / 0xd04);
    wsprintfA(hdr, "[%d] slot=%d in0=%u in1=%u in5=%u gm=%d ph=%u ndiff=%d%s\r\n",
              g_selfTestCount, slot, input[0], input[1], input[5], gameMode, phase, mism,
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
    Call_A6b(v, dt, input, wheelBlock);                            // 0x00470943 (ECX=record, ESI=wheelBlock)

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
void A5_SelfTest(int* record, float dt, void* xform) {
    const std::uint32_t groundedBits = *reinterpret_cast<std::uint32_t*>((char*)record + 0x9e0);
    const bool airborne = (groundedBits != 0x40800000u);   // != 4.0f
    if (airborne) { if (g_a5Air >= kA5MaxAir) return; }
    else          { if (g_a5Gnd >= kA5MaxGnd) return; }
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
    if (airborne) g_a5Air++; else g_a5Gnd++;

    char hdr[220];
    int grounded = *reinterpret_cast<int*>((char*)record + 0x9e0);
    int slot = (int)(((std::uintptr_t)record - 0x008815a0) / 0xd04);
    wsprintfA(hdr, "[%d] slot=%d dt=%08x grounded=%08x air=%d rng=%d ndiff=%d%s\r\n",
              g_a5Count, slot, *reinterpret_cast<std::uint32_t*>(&dt), grounded,
              airborne ? 1 : 0, rngFired ? 1 : 0, mism, mism ? "" : " OK");
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
    if (c < 0) { const char* s = std::getenv("MASHED_A6A_SELFTEST_CAP"); c = (s && std::atoi(s) > 0) ? std::atoi(s) : 1; }
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
    if (!admit) return;
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
// Bit-identity strategy: the angle math keeps the acos result in ST0 as float10
// across (*180/pi)(double FMUL), (-90)(double/float FSUB), FCHS, (*dts)(float FMUL)
// before the single FSTP-to-float32 — and routes acos through the LIVE CRT
// (FUN_004a3384) + the matrix rotate through the LIVE RW device builder
// (FUN_004c4d20, internally FUN_004c3b90 inv-sqrt LUT + fsin/fcos) + the live
// normalize (FUN_004c39b0). Reproduced as a NAKED transcription of the exact
// instruction stream so the float10 width + .rdata bit patterns + live calls match.
// ===========================================================================
namespace {

// EXACT-bit .rdata constants A6b reads (memory_read, Mashed_pool12 2026-06-17).
// MSVC inline-asm cannot encode an absolute-address x87 memory operand
// (`fmul dword ptr [0x00614700]` -> C2415), so the constants are materialized as
// file-scope variables with the exact original bit patterns and referenced by name.
const float  A6b_005d757c = Cf(0x00000000);  // 0.0
const float  A6b_005cc558 = Cf(0x3a83126f);  // 0.001   dt scale (rad/deg-ish)
const float  A6b_005cc33c = Cf(0xbf800000);  // -1.0    clamp lo
const float  A6b_005cc320 = Cf(0x3f800000);  // 1.0     clamp hi
const float  A6b_005cc9a0 = Cf(0x3d4ccccd);  // 0.05    vel-align angle scale
const float  A6b_005ccad0 = Cf(0x42b40000);  // 90.0    (FLOAT; roll bias)
const float  A6b_g6fc     = Cf(0x00000000);  // _DAT_006146fc = 0.0  (Y-basis.x)
const float  A6b_g700     = Cf(0x3f800000);  // _DAT_00614700 = 1.0  (Y-basis.y)
const float  A6b_g704     = Cf(0x00000000);  // _DAT_00614704 = 0.0  (Y-basis.z)
// doubles (exact 8-byte patterns):
inline double Cd(std::uint64_t bits) { double d; std::memcpy(&d, &bits, 8); return d; }
const double A6b_005ccae0 = Cd(0x404ca5dcc0000000ULL);  // 57.29578 = 180/pi
const double A6b_005ccad8 = Cd(0x4056800000000000ULL);  // 90.0  (DOUBLE; pitch bias)

// FUN_00468980 body, register ABI ECX=record, ESI=xform, [ESP+4]=dt(param_2).
// VERBATIM bytes 0x00468980..0x00468b34, with the three external CALLs retargeted
// to live absolute addresses via the PCH_Fwd_* pointers.
extern "C" void* PCH_Fwd_4a3384;
extern "C" void* PCH_Fwd_4c4d20;
extern "C" void* PCH_Fwd_4c39b0;
__declspec(naked) void A6b_BodyAsm(/* ECX=record, ESI=xform, [esp+4]=dt */) {
    __asm {
        fld   dword ptr [ecx+0x9e0]          // 0x468980  grounded count
        sub   esp, 0x20                       // 0x468986  -> param_2(dt) at [esp+0x24]
        fcomp A6b_005d757c                    // 0x468989  : 0.0
        fnstsw ax
        test  ah, 0x44
        jp    a6b_end                         // 0x468994  not airborne -> skip
        mov   edx, dword ptr [ecx+0x9f0]      // 0x46899a  motion state
        xor   eax, eax
        cmp   edx, eax
        jnz   a6b_state_nz                    // 0x4689a4  -> state != 0 branch
        // ── state == 0 : auto-level (pitch + roll) ──
        fld   dword ptr [esp+0x24]            // dt
        fmul  A6b_005cc558                    // * 0.001
        fstp  dword ptr [esp+4]               // dts (f32) -> [esp+4]
        // fVar4 = xform[10]*g704 + xform[8]*g6fc + xform[9]*g700  (= at.y)
        fld   dword ptr [esi+0x24]            // xform[9]
        fmul  A6b_g700                        // * 1.0
        fld   dword ptr [esi+0x20]            // xform[8]
        fmul  A6b_g6fc                        // * 0.0
        faddp st(1), st(0)
        fld   dword ptr [esi+0x28]            // xform[10]
        fmul  A6b_g704                        // * 0.0
        faddp st(1), st(0)
        fcom  A6b_005cc33c                    // : -1.0
        fnstsw ax
        test  ah, 5
        jp    a6b_p1_chkhi
        fstp  st(0)
        fld   A6b_005cc33c                    // clamp lo -> -1.0
        jmp   a6b_p1_acos
    a6b_p1_chkhi:
        fcom  A6b_005cc320                    // : 1.0
        fnstsw ax
        test  ah, 0x41
        jnz   a6b_p1_acos
        fstp  st(0)
        fld   A6b_005cc320                    // clamp hi -> 1.0
    a6b_p1_acos:
        sub   esp, 8
        fstp  qword ptr [esp]                 // push clamp as double
        call  dword ptr [PCH_Fwd_4a3384]      // ST0 = acos(clamp)  [live CRT]
        fmul  A6b_005ccae0                    // * 180/pi (double, float10)
        push  1                               // mode arg (early)
        fsub  A6b_005ccad8                    // - 90.0 (double)
        fchs                                  // negate
        fmul  dword ptr [esp+0x10]            // * dts
        fstp  dword ptr [esp+0xc]             // ang1 (f32)
        mov   eax, dword ptr [esp+0xc]
        push  eax                             // angle
        push  0x6146f0                        // axis X = (1,0,0)
        push  esi                             // out = xform
        call  dword ptr [PCH_Fwd_4c4d20]      // RwMatrixRotate(xform,X,ang1,1) [live RW]
        // fVar1 = xform[2]*g704 + g6fc*xform[0] + xform[1]*g700  (= right.y)
        fld   dword ptr [esi+4]               // xform[1]
        fmul  A6b_g700                        // * 1.0
        add   esp, 0x18                        // clean acos double + the 4 c4d20 args
        fld   A6b_g6fc                        // 0.0
        fmul  dword ptr [esi]                 // * xform[0]
        faddp st(1), st(0)
        fld   dword ptr [esi+8]               // xform[2]
        fmul  A6b_g704                        // * 0.0
        faddp st(1), st(0)
        fcom  A6b_005cc33c                    // : -1.0
        fnstsw ax
        test  ah, 5
        jp    a6b_p2_chkhi
        fstp  st(0)
        fld   A6b_005cc33c
        jmp   a6b_p2_acos
    a6b_p2_chkhi:
        fcom  A6b_005cc320                    // : 1.0
        fnstsw ax
        test  ah, 0x41
        jnz   a6b_p2_acos
        fstp  st(0)
        fld   A6b_005cc320
    a6b_p2_acos:
        sub   esp, 8
        fstp  qword ptr [esp]                 // push clamp as double
        call  dword ptr [PCH_Fwd_4a3384]      // ST0 = acos(clamp) [live CRT]
        fmul  A6b_005ccae0                    // * 180/pi (double, float10)
        push  1                               // mode
        fsub  A6b_005ccad0                    // - 90.0 (FLOAT here)
        fmul  dword ptr [esp+0x10]            // * dts   (NO FCHS on roll)
        fstp  dword ptr [esp+0xc]             // ang2 (f32)
        mov   ecx, dword ptr [esp+0xc]
        push  ecx                             // angle
        push  0x614708                        // axis Z = (0,0,1)
        push  esi                             // out = xform
        call  dword ptr [PCH_Fwd_4c4d20]      // RwMatrixRotate(xform,Z,ang2,1) [live RW]
        add   esp, 0x18
        add   esp, 0x20
        ret
    a6b_state_nz:
        // ── state != 0 : zero ang-vel + align xform to velocity dir ──
        mov   dword ptr [ecx+0x9c4], eax      // 0.0
        mov   dword ptr [ecx+0x9c0], eax
        mov   dword ptr [ecx+0x9bc], eax
        add   ecx, 0x9b0                       // &linVel
        mov   edx, dword ptr [ecx]            // velX
        mov   eax, dword ptr [ecx+4]          // velY
        mov   ecx, dword ptr [ecx+8]          // velZ
        mov   dword ptr [esp+8], edx
        lea   edx, [esp+8]                     // &local (in)
        mov   dword ptr [esp+0xc], eax
        push  edx                              // src
        lea   eax, [esp+0xc]                   // dst (= &local after push)
        push  eax                              // dst
        mov   dword ptr [esp+0x18], ecx        // velZ stored
        call  dword ptr [PCH_Fwd_4c39b0]      // FUN_004c39b0 normalize [live]
        fstp  st(0)                            // pop ST0 magnitude
        mov   ecx, dword ptr [esp+0x18]        // velZ (preserved)
        fld   dword ptr [esp+0x2c]            // dt
        fmul  A6b_005cc9a0                    // * 0.05
        mov   edx, dword ptr [esp+0x14]        // normX
        mov   eax, dword ptr [esp+0x10]        // (slot)
        add   esp, 8
        push  1                                // mode
        push  ecx                              // (velZ slot reuse)
        mov   dword ptr [esp+0x1c], ecx
        lea   ecx, [esp+0x1c]
        fstp  dword ptr [esp]                  // angle = dt*0.05 (f32)
        push  ecx                              // axis (&local dir)
        push  esi                              // out = xform
        mov   dword ptr [esp+0x28], edx
        mov   dword ptr [esp+0x2c], eax
        call  dword ptr [PCH_Fwd_4c4d20]      // RwMatrixRotate(xform,dir,dt*0.05,1) [live]
        add   esp, 0x10
    a6b_end:
        add   esp, 0x20
        ret
    }
}

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
// Call MY A6b body with the same ABI.
__declspec(naked) void Call_A6bBody(int* /*record*/, void* /*xform*/, float /*dt*/) {
    __asm {
        push esi
        mov  ecx, dword ptr [esp+8]
        mov  esi, dword ptr [esp+12]
        push dword ptr [esp+16]        // dt
        call A6b_BodyAsm
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
    const bool airborne = (*reinterpret_cast<float*>((char*)record + 0x9e0) == 0.0f);
    const bool haveXform = (xform != nullptr);
    // NON-PERTURBING verification (2026-06-23): the ORIGINAL always runs and is LEFT as the
    // live game state, so the race follows faithful physics even when our body diverges (the
    // old "leave mine" + "skip past quota" perturbed the run — a skipped aero corrupts the
    // rest of the race and flung the car off ramps). MINE is measured on a restored copy.
    // Past the per-branch sample quota we STILL run the original (never skip the function).
    // A6b's only side effects are record + xform (pure aero math, no PRNG), so snapshot/
    // restore of those two makes the two runs independent.
    const bool sample = airborne ? (g_a6bAir < kA6bMaxAir) : (g_a6bGnd < kA6bMaxGnd);
    if (!sample) { Call_OrigA6b(record, xform, dt); return; }   // quota full: run original only

    static std::uint8_t snapRec[0xd04];
    static std::uint8_t snapXf[64];
    std::memcpy(snapRec, record, 0xd04);
    if (haveXform) std::memcpy(snapXf, xform, 64);

    // (1) MINE on the live copy — capture its outputs.
    Call_A6bBody(record, xform, dt);
    std::uint32_t mineRec[4]; int nr = 0;
    for (int off : kA6bRecOffs) mineRec[nr++] = *reinterpret_cast<std::uint32_t*>((char*)record + off);
    std::uint32_t mineXf[16];
    if (haveXform) std::memcpy(mineXf, xform, 64);

    // (2) restore, run the ORIGINAL, and LEAVE its output as the faithful game state.
    std::memcpy(record, snapRec, 0xd04);
    if (haveXform) std::memcpy(xform, snapXf, 64);
    Call_OrigA6b(record, xform, dt);
    std::uint32_t origRec[4]; nr = 0;
    for (int off : kA6bRecOffs) origRec[nr++] = *reinterpret_cast<std::uint32_t*>((char*)record + off);
    std::uint32_t origXf[16];
    if (haveXform) std::memcpy(origXf, xform, 64);

    int mism = 0; char line[1024]; int p = 0;
    for (int i = 0; i < nr; ++i) {
        if (mineRec[i] != origRec[i]) { mism++;
            if (p < 900) p += wsprintfA(line+p, " +0x%x:o=%08x,m=%08x", kA6bRecOffs[i], origRec[i], mineRec[i]); }
    }
    if (haveXform) {
        for (int i = 0; i < 16; ++i) {
            if (mineXf[i] != origXf[i]) { mism++;
                if (p < 900) p += wsprintfA(line+p, " m[%d]:o=%08x,m=%08x", i, origXf[i], mineXf[i]); }
        }
    }
    if (airborne) g_a6bAir++;
    else          g_a6bGnd++;

    char hdr[280];
    int state = *reinterpret_cast<int*>((char*)record + 0x9f0);
    // slot = (record - DAT_008815a0) / 0xd04 : 0=player, 1..3=AI opponents
    int slot = (int)(((std::uintptr_t)record - 0x008815a0) / 0xd04);
    // U-9149 DISPATCH WITNESS (2026-09-29). The self-test compares two runs that share
    // the SAME ESI, so it can go green on a WRONG ESI (memory: inherited-green-may-be-
    // guard-only). Log the pointer we were actually handed against the one the original
    // computes at 0x0047068e/0x00470696/0x00470699 and stores to A4's E+0x0c slot at
    // 0x004706a2 — the slot 0x0047093b reloads into ESI. xfok=1 means the dispatch that
    // reached us matches the original's. With A4 NOT hooked this witnesses the ORIGINAL's
    // dispatch; with A4 hooked it witnesses Call_A6b.
    const void* expectXf = (const void*)((char*)record
                            + *reinterpret_cast<int*>((char*)record + 0x9a8) * 0x40 + 0x928);
    wsprintfA(hdr, "[%d] slot=%d dt=%08x air=%d state=%d xform=%d xf=%08x exp=%08x xfok=%d ndiff=%d%s\r\n",
              g_a6bCount, slot, *reinterpret_cast<std::uint32_t*>(&dt), airborne ? 1 : 0,
              state, haveXform ? 1 : 0, (unsigned)(std::uintptr_t)xform,
              (unsigned)(std::uintptr_t)expectXf, (xform == expectXf) ? 1 : 0,
              mism, mism ? "" : " OK");
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
    else {
        // run the verbatim body for real (ECX=record, ESI=xform, dt on stack)
        Call_A6bBody(record, xform, dt);
    }
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
