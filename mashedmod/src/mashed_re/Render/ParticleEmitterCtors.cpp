// Mashed RE — particle-emitter constructors (EBX-implicit-this family).
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// Port-capture evidence: re/analysis/callers_c2_unblock/portcap_0x0041ad60.md
// (full literal listing + callee ABI). Verified via the reg_this_callee_stub
// A/B handler (orch-iter6) which stubs the live-RW callees so the ctor runs
// cold and deterministically. C3 not C4 (hook-bypassed synthetic A/B).
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x0041ad60  ParticleEmitterCtorA(this=EBX, clump=EAX)  — 17-atomic Class A.
//
// Verbatim 0x0041ad60..0x0041ada7 (portcap_0x0041ad60.md):
//   0041ad60 SUB ESP,0x44                 ; 17-entry (0x44-byte) stack buffer
//   0041ad63 PUSH ESI ; 0041ad64 MOV ESI,EAX   ; ESI = clump (entry-EAX)
//   0041ad66 PUSH EDI
//   0041ad67 LEA EAX,[ESP+8] ; PUSH EAX   ; buffer ptr  (arg2, deeper)
//   0041ad6c PUSH ESI                     ; clump       (arg1, topmost)
//   0041ad6d CALL 0x004b3fc0              ; fill buffer with 17 atomic handles
//   0041ad72 MOV [EBX+0x5c],ESI           ; this->clump = clump
//   0041ad75 MOV ECX,[ESI+4] ; 0041ad7b MOV [EBX+0x60],ECX  ; this->frame = *(clump+4)
//   0041ad78 PUSH 0x50 ; 0041ad7a PUSH EBX ; 0041ad7e CALL 0x004b6520  ; zero-init region
//   0041ad83 ADD ESP,0x10                 ; batched cleanup for BOTH calls above
//   loop i=0..0x10 (17):
//     0041ad88 MOV EDI,[ESP+ESI*4+8]      ; handle = buffer[i]
//     PUSH 0,0,EDI ; CALL 0x004b5190      ; idx = handle->in-clump index
//     ADD ESP,0xc ; INC ESI ; CMP ESI,0x11
//     0041ad9d MOV [EBX+EAX*4],EDI         ; this[idx*4] = handle
//     JL loop
//   POP EDI ; POP ESI ; ADD ESP,0x44 ; RET
//
// FUN_004b3fc0's own EAX return is discarded (the decomp's `in_EAX` is the
// caller-supplied clump preserved in ESI, NOT a return) — see plate.
// ─────────────────────────────────────────────────────────────────────────────

// Original sub-functions, called by absolute RVA (this .asi is injected into
// MASHED.exe so these addresses are live). During A/B verification all three
// are Interceptor.replace'd by the harness; in production they run for real.
namespace {
using fn_fill_t  = void(__cdecl*)(std::uint32_t clump, std::uint32_t* buf);
using fn_index_t = int (__cdecl*)(std::uint32_t handle, int a, int b);
using fn_zero_t  = void(__cdecl*)(void* self, int len);

// __cdecl impl reached by the naked shim below (which delivers EBX/EAX as
// ordinary stack args). Faithful transcription of the listing above.
void __cdecl ParticleEmitterCtorA_impl(std::uint8_t* self, std::uint32_t clump) {
    std::uint32_t buf[17];                                        // [ESP+8] buffer
    reinterpret_cast<fn_fill_t>(0x004b3fc0)(clump, buf);          // CALL 0x004b3fc0
    *reinterpret_cast<std::uint32_t*>(self + 0x5c) = clump;       // [EBX+0x5c] = clump
    *reinterpret_cast<std::uint32_t*>(self + 0x60) =
        reinterpret_cast<std::uint32_t*>(clump)[1];               // [EBX+0x60] = *(clump+4)
    reinterpret_cast<fn_zero_t>(0x004b6520)(self, 0x50);          // CALL 0x004b6520
    for (int i = 0; i < 0x11; ++i) {                             // 17 iterations
        std::uint32_t handle = buf[i];                           // MOV EDI,[ESP+ESI*4+8]
        int idx = reinterpret_cast<fn_index_t>(0x004b5190)(handle, 0, 0);  // CALL 0x004b5190
        *reinterpret_cast<std::uint32_t*>(self + idx * 4) = handle;        // [EBX+EAX*4] = EDI
    }
}
}  // namespace

// Register-implicit-this entry: `this` in EBX, `clump` in EAX. The naked shim
// repackages the two register inputs as __cdecl stack args (clump deeper, self
// topmost — matching impl(self, clump)) and forwards to the C++ body. EBX is
// callee-saved and untouched here.
extern "C" __declspec(dllexport) __declspec(naked) void ParticleEmitterCtorA() {
    __asm {
        push eax                     // clump  (arg2, pushed first  = deeper)
        push ebx                     // this   (arg1, pushed second = topmost)
        call ParticleEmitterCtorA_impl
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(ParticleEmitterCtorA, 0x0041ad60);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0041c320  ParticleEmitterCtorC(this=EBX, clump=EAX) — 24-atomic Class C.
//
// Same template as 0x0041ad60 with shifted offsets (portcap_0x0041c320.md):
//   handle table at this+0x80 (store this[0x80+idx*4]); clump at this+0x100;
//   frame *(clump+4) at this+0x104; loop count 24 (CMP ESI,0x18);
//   FUN_004b6520(this+0x80, 0x80) zero-fills the table region.
// ─────────────────────────────────────────────────────────────────────────────
namespace {
void __cdecl ParticleEmitterCtorC_impl(std::uint8_t* self, std::uint32_t clump) {
    std::uint32_t buf[24];                                       // [ESP+8] buffer
    reinterpret_cast<fn_fill_t>(0x004b3fc0)(clump, buf);         // CALL 0x004b3fc0
    *reinterpret_cast<std::uint32_t*>(self + 0x100) = clump;     // [EBX+0x100] = clump
    *reinterpret_cast<std::uint32_t*>(self + 0x104) =
        reinterpret_cast<std::uint32_t*>(clump)[1];              // [EBX+0x104] = *(clump+4)
    reinterpret_cast<fn_zero_t>(0x004b6520)(self + 0x80, 0x80);  // CALL 0x004b6520(this+0x80,0x80)
    for (int i = 0; i < 0x18; ++i) {                            // 24 iterations
        std::uint32_t handle = buf[i];                          // MOV EDI,[ESP+ESI*4+8]
        int idx = reinterpret_cast<fn_index_t>(0x004b5190)(handle, 0, 0);      // CALL 0x004b5190
        *reinterpret_cast<std::uint32_t*>(self + 0x80 + idx * 4) = handle;     // [EBX+EAX*4+0x80] = EDI
    }
}
}  // namespace
extern "C" __declspec(dllexport) __declspec(naked) void ParticleEmitterCtorC() {
    __asm {
        push eax
        push ebx
        call ParticleEmitterCtorC_impl
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(ParticleEmitterCtorC, 0x0041c320);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0041cd20  ParticleEmitterCtorD(this=EBX, clump=EAX) — 34-atomic Class D.
//
// Same template plus a colour pass (portcap_0x0041cd20.md):
//   handle table at this+0xb0 (store this[0xb0+idx*4]); clump at this+0x150;
//   frame at this+0x154; loop count 34 (CMP ESI,0x22);
//   FUN_004b6520(this+0xb0, 0xa0) zero-fills; after the loop,
//   FUN_004b5260(*(this+0xb8), &color[4]) with color = {0x32,0x32,0x32,0xff}
//   applies grey to the master atomic.
// ─────────────────────────────────────────────────────────────────────────────
namespace {
using fn_color_t = void(__cdecl*)(std::uint32_t atomic, const void* colorPtr);
void __cdecl ParticleEmitterCtorD_impl(std::uint8_t* self, std::uint32_t clump) {
    std::uint8_t  color[4] = { 0x32, 0x32, 0x32, 0xff };         // R+4..R+7 color buffer
    std::uint32_t buf[34];                                       // [ESP+0xc] buffer
    reinterpret_cast<fn_fill_t>(0x004b3fc0)(clump, buf);         // CALL 0x004b3fc0
    *reinterpret_cast<std::uint32_t*>(self + 0x150) = clump;     // [EBX+0x150] = clump
    *reinterpret_cast<std::uint32_t*>(self + 0x154) =
        reinterpret_cast<std::uint32_t*>(clump)[1];              // [EBX+0x154] = *(clump+4)
    reinterpret_cast<fn_zero_t>(0x004b6520)(self + 0xb0, 0xa0);  // CALL 0x004b6520(this+0xb0,0xa0)
    for (int i = 0; i < 0x22; ++i) {                            // 34 iterations
        std::uint32_t handle = buf[i];
        int idx = reinterpret_cast<fn_index_t>(0x004b5190)(handle, 0, 0);      // CALL 0x004b5190
        *reinterpret_cast<std::uint32_t*>(self + 0xb0 + idx * 4) = handle;     // [EBX+EAX*4+0xb0] = EDI
    }
    reinterpret_cast<fn_color_t>(0x004b5260)(                    // CALL 0x004b5260
        *reinterpret_cast<std::uint32_t*>(self + 0xb8), color);  //   (*(this+0xb8), &color)
}
}  // namespace
extern "C" __declspec(dllexport) __declspec(naked) void ParticleEmitterCtorD() {
    __asm {
        push eax
        push ebx
        call ParticleEmitterCtorD_impl
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(ParticleEmitterCtorD, 0x0041cd20);

// ─────────────────────────────────────────────────────────────────────────────
// RpClumpDestroy destructors (this=EAX). Each reads a clump handle from a fixed
// offset and tail-calls RpClumpDestroy (FUN_004e6e00). Verbatim 11-byte tail
// calls per re/analysis/callers_c2_unblock/portcap_dtor_rpclumpdestroy.md:
//   0x0041b440  MOV ECX,[EAX+0x5c];  PUSH ECX; CALL 0x004e6e00; POP ECX; RET
//   0x0041beb0  MOV ECX,[EAX+0x15c]; PUSH ECX; CALL 0x004e6e00; POP ECX; RET
//   0x0041cb00  MOV ECX,[EAX+0x100]; PUSH ECX; CALL 0x004e6e00; POP ECX; RET
// Verified via reg_this_call_observe (records RpClumpDestroy's arg). C3 not C4.
// ─────────────────────────────────────────────────────────────────────────────
namespace {
using fn_destroy_t = void(__cdecl*)(std::uint32_t clump);
void __cdecl EmitterDtor_0x5c_impl(std::uint8_t* self) {
    reinterpret_cast<fn_destroy_t>(0x004e6e00)(*reinterpret_cast<std::uint32_t*>(self + 0x5c));
}
void __cdecl EmitterDtor_0x15c_impl(std::uint8_t* self) {
    reinterpret_cast<fn_destroy_t>(0x004e6e00)(*reinterpret_cast<std::uint32_t*>(self + 0x15c));
}
void __cdecl EmitterDtor_0x100_impl(std::uint8_t* self) {
    reinterpret_cast<fn_destroy_t>(0x004e6e00)(*reinterpret_cast<std::uint32_t*>(self + 0x100));
}
}  // namespace

// this in EAX → naked shim repackages as a single __cdecl stack arg.
extern "C" __declspec(dllexport) __declspec(naked) void EmitterDtorClumpAt5c() {
    __asm {
        push eax
        call EmitterDtor_0x5c_impl
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(EmitterDtorClumpAt5c, 0x0041b440);

extern "C" __declspec(dllexport) __declspec(naked) void EmitterDtorClumpAt15c() {
    __asm {
        push eax
        call EmitterDtor_0x15c_impl
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(EmitterDtorClumpAt15c, 0x0041beb0);

extern "C" __declspec(dllexport) __declspec(naked) void EmitterDtorClumpAt100() {
    __asm {
        push eax
        call EmitterDtor_0x100_impl
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(EmitterDtorClumpAt100, 0x0041cb00);
