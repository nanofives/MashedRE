// Mashed RE — HUD text/font cluster, batch-k session 4 (C2->C3 promotions).
// Analysis notes: re/analysis/hud_promote_c2_b/, re/analysis/hud_ingame_promote_c2/
// Subsystem: HUD (font context / text rendering)
//
// Binary anchor:
//   original\MASHED.exe.unpatched  SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// Hooks in this file:
//   0x00552750  FontCtx_ResetTransform  pure leaf; identity RwMatrix on cur ctx
//   0x00428450  HudSpinCoinAnim         (C2 stub; smoke-test for spin_angle_observe arg_type)
//
// Refusals from prior sessions (documented in re/PROMOTION_QUEUE.md):
//   0x004c1c80  callee FUN_004c0e50 is C1 — fails C2->C3 "callee at C2+" rule;
//               batch header described as "pure leaf" but analysis note shows
//               guarded conditional callee — tracker drift, skip rather than promote.
//   0x00427680  repeat refusal from HudBatch.cpp session — non-standard ESI
//               implicit output ptr + U-2127 EDI artifact; no matching arg_type
//               in diff_template.js; deferred pending harness arg_type extension.
//   0x00450b10  PROMOTED C2->C3 in c3-batch-n-s1 (DrawQuadPrimitives.cpp);
//               draw_quad_observe arg_type landed 2026-05-21; Frida GREEN 12/12.
//   0x00428450  spin-angle accumulator side-effect — _DAT_0067d974 is incremented
//               by each call; draw_quad_observe cannot reset it between Orig+Reimpl
//               calls; new harness arg_type spin_angle_observe resets the accumulator
//               before each sub-call; smoke-tested 2026-05-22.

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Font context globals (all addresses Ghidra-cited in analysis notes)
// ---------------------------------------------------------------------------

// DAT_00912b04 = current font context stack depth (uint32_t index)
static std::uint32_t* const g_FontCtxDepth_k4 =
    reinterpret_cast<std::uint32_t*>(0x00912b04u);

// DAT_00912a84 = base of font context pointer array (void*[32])
static void** const g_FontCtxPtrs_k4 =
    reinterpret_cast<void**>(0x00912a84u);

// DAT_00912bd8 = matrix dirty/valid flag (zeroed by ResetTransform)
static std::uint32_t* const g_DirtyBd8_k4 =
    reinterpret_cast<std::uint32_t*>(0x00912bd8u);

// DAT_00912bec = secondary dirty flag (zeroed by ResetTransform)
static std::uint32_t* const g_DirtyBec_k4 =
    reinterpret_cast<std::uint32_t*>(0x00912becu);

// ===========================================================================
// 0x00552750  FontCtx_ResetTransform
//
// Original: FUN_00552750 (0x8e bytes; 0x00552750–0x0055283d).
// Pure leaf (no named direct callees; only global reads/writes).
//
// Resets the current font context's 3×3 rotation matrix to identity and
// clears dirty flags. The "current context" is:
//   (&DAT_00912a84)[DAT_00912b04]  (a pointer loaded from the font context
//   stack array at index given by the depth counter).
//
// RwMatrix layout (offsets relative to ctx pointer, from analysis note):
//   +0x00  right.x = 1.0f
//   +0x04  right.y = 0.0f
//   +0x08  right.z = 0.0f
//   +0x0c  flags   |= 0x20003  (identity marker bits; OR not assignment)
//   +0x10  up.x    = 0.0f
//   +0x14  up.y    = 1.0f
//   +0x18  up.z    = 0.0f
//   +0x20  at.x    = 0.0f
//   +0x24  at.y    = 0.0f
//   +0x28  at.z    = 1.0f
//   +0x30  pos.x   = 0.0f
//   +0x34  pos.y   = 0.0f
//   +0x38  pos.z   = 0.0f
//
// After matrix writes:
//   DAT_00912bd8 = 0  (dirty flag reset, 0x00552820)
//   DAT_00912bec = 0  (secondary dirty flag reset, 0x0055282a)
//
// Returns: 1 (always, uint32_t).
//
// Callers (from C1 notes and call sites): 0x00427ff0, 0x00552c10, 0x00427f00.
// Analysis note: re/analysis/hud_promote_c2_b/0x00552750.md
// Leaf-exemption: no named callees (callees_depth1: []). C2->C3 leaf rule applies.
// ===========================================================================

// 0x00552750
extern "C" __declspec(dllexport)
std::uint32_t __cdecl FontCtx_ResetTransform()
{
    // Load current context pointer: (&DAT_00912a84)[DAT_00912b04]
    float* ctx = reinterpret_cast<float*>(
        g_FontCtxPtrs_k4[*g_FontCtxDepth_k4]);

    // Identity matrix writes (linear sequence, 0x00552750–0x005527fe):
    // +0x00: right.x = 1.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x00u) = 1.0f;  // 0x3f800000
    // +0x04: right.y = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x04u) = 0.0f;
    // +0x08: right.z = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x08u) = 0.0f;
    // +0x0c: flags |= 0x20003  (0x00552804 — OR not assignment)
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x0cu) |= 0x20003u;
    // +0x10: up.x = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x10u) = 0.0f;
    // +0x14: up.y = 1.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x14u) = 1.0f;  // 0x3f800000
    // +0x18: up.z = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x18u) = 0.0f;
    // +0x20: at.x = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x20u) = 0.0f;
    // +0x24: at.y = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x24u) = 0.0f;
    // +0x28: at.z = 1.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x28u) = 1.0f;  // 0x3f800000
    // +0x30: pos.x = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x30u) = 0.0f;
    // +0x34: pos.y = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x34u) = 0.0f;
    // +0x38: pos.z = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x38u) = 0.0f;

    // Dirty flag resets (0x00552820, 0x0055282a):
    *g_DirtyBd8_k4 = 0u;
    *g_DirtyBec_k4 = 0u;

    // Returns 1 always.
    return 1u;
}

// Re-enabled r213 2026-06-15. Prior "AV/AV" (2026-05-24) was a null/garbage ctx
// deref because the harness never seeded the INDIRECT ctx pointer
// (g_FontCtxPtrs[depth]). The early_window seed_indirect_ctx_obs handler now
// allocates a ctx buffer + writes its address into g_FontCtxPtrs[0] + sets
// depth=0 -> no AV; GREEN non-degenerate. C2->C3 promoted on that evidence.
RH_ScopedInstall(FontCtx_ResetTransform, 0x00552750);

// ---------------------------------------------------------------------------
// HudSpinCoinAnim  --  0x00428450
//
// Original: FUN_00428450 (305 bytes, 0x00428450..0x00428580)
// Signature: void FUN_00428450(int param_1, int param_2)
//   param_1: horizontal position offset (screen units)
//   param_2: vertical position offset (screen units)
//
// Computes sin of current spin angle (_DAT_0067d974), scales by screen dims
// and constants, then calls FUN_00450b10 (HudIm2DQuad) with computed x/y/w/h.
// Advances spin angle: _DAT_0067d974 += _DAT_005cc56c.
//
// Constants (cited from 0x00428450 body):
//   0x0067d974 — spin angle accumulator (float32)
//   0x005cc56c — angular velocity constant (float32)
//   0x005d757c — sin threshold for UV horizontal flip (float32)
//   0x00771960 — pointer to texture handle (ptr-to-undefined4)
//   0x005cd5a8 — screen-width scale factor (float32)
//   0x005cd65c — x-position base constant (float32)
//   0x005cd274 — x-position sin scale constant (float32)
//   0x005cc560 — y/height scale constant (float32)
//   0x005cd658 — y-position offset constant (float32)
//   0x005cc730 — size ratio constant (float32)
//
// Smoke-test target for spin_angle_observe harness arg_type (2026-05-22).
// Full C3 promotion is c3_batch_p's job.
// Analysis note: re/analysis/hud_ingame_promote_c2/0x00428450.md
// ---------------------------------------------------------------------------

// 0x00428450
// 2026-05-24 phase-a3: rewritten as inline-asm x87 sequence to match the
// orig's bit-pattern output. Prior C++ reimpl used MSVC's default x86
// SSE2 codegen (/arch:SSE2 since VS2019), producing 32-bit-rounded
// intermediates while the orig uses x87 80-bit extended precision on
// the FPU stack. The diff harness caught this as SILENT-DIVERGE on
// 5/10 inputs (any non-zero param_1 or param_2). The new sequence
// mirrors the orig x87 ops at 0x00428453..0x0042857d exactly: FSIN at
// entry, FILD+FMUL for screenW*scaleW, the (baseCx-sinVal*sinScX)+
// param_1 chain via FIADD on the caller's stack slot for param_1, and
// the (param_2+yOff)*screenH*scaleYH chain matching the orig's stack-
// shuffle pattern. Output args are pushed to DrawQuad in the same
// order as 0x00428550..0x00428562.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl HudSpinCoinAnim(int /*param_1*/, int /*param_2*/) {
    __asm {
        // 0x00428450..0x00428492: prologue + fsin + UV defaults + FSTP sinVal.
        sub  esp, 0x2c
        fld  dword ptr ds:[0x0067d974]     // spinAngle
        mov  al, 0xff
        fsin
        push esi
        mov  byte ptr [esp+0x4], al
        mov  byte ptr [esp+0x5], al
        mov  byte ptr [esp+0x6], al
        mov  byte ptr [esp+0x7], al
        mov  dword ptr [esp+0x20], 0x0       // u0 default
        mov  dword ptr [esp+0x24], 0x0       // v0 default
        mov  dword ptr [esp+0x28], 0x3f000000 // u1 default 0.5
        mov  dword ptr [esp+0x2c], 0x3f800000 // v1 default 1.0
        xor  esi, esi                          // texHandle = 0
        fstp dword ptr [esp+0x8]              // sinVal (32-bit)
        // GetWidth -> AX (int16), MOVSX, store, FILD, FMUL scaleW, FSTP fVar2.
        mov  ecx, 0x0042b8b0
        call ecx
        movsx eax, ax
        mov  dword ptr [esp+0xc], eax
        fild dword ptr [esp+0xc]
        fmul dword ptr ds:[0x005cd5a8]           // * scaleW
        fstp dword ptr [esp+0xc]              // fVar2 (32-bit)
        // GetHeight -> AX, MOVSX, store, FILD, FMUL scaleYH (keep on FPU).
        mov  ecx, 0x0042b8c0
        call ecx
        movsx ecx, ax
        mov  dword ptr [esp+0x10], ecx
        fild dword ptr [esp+0x10]
        fmul dword ptr ds:[0x005cc560]           // ST(0) = screenH * scaleYH
        // (baseCx - sinVal*sinScX) + param_1 via FIADD.
        fld  dword ptr [esp+0x8]              // ST(0)=sinVal, ST(1)=screenH*scaleYH
        fmul dword ptr ds:[0x005cd274]           // ST(0) = sinVal * sinScX
        fsubr dword ptr ds:[0x005cd65c]          // ST(0) = baseCx - sinVal*sinScX
        fiadd dword ptr [esp+0x34]            // ST(0) += float10(param_1)
        // (param_2 + yOff) via FILD+FADD; FSTP to scratch.
        fild dword ptr [esp+0x38]             // ST(0)=float10(param_2), push
        fadd dword ptr ds:[0x005cd658]           // ST(0) = param_2 + yOff
        fstp dword ptr [esp+0x14]             // store drawY' (32-bit), pop
        // drawW = sinVal * ratio * fVar2 (FPU stack order matches orig).
        fld  dword ptr [esp+0x8]              // ST(0)=sinVal
        fmul dword ptr ds:[0x005cc730]           // * ratio
        fmul dword ptr [esp+0xc]              // * fVar2
        fstp dword ptr [esp+0x18]             // drawW
        // drawH = screenH*scaleYH * ratio (dup ST(1) first).
        fld  st(1)                             // ST(0)=ST(1)=screenH*scaleYH
        fmul dword ptr ds:[0x005cc730]
        fstp dword ptr [esp+0x1c]             // drawH
        // drawX = fVar2 * (baseCx-sinVal*sinScX+param_1).
        fld  dword ptr [esp+0xc]              // ST(0)=fVar2
        fmul st(0), st(1)                     // * (baseCx-...+param_1)
        fstp dword ptr [esp+0x10]             // drawX
        fstp st(0)                             // pop (baseCx-...+param_1) — discard
        fmul dword ptr [esp+0x14]             // ST(0) = screenH*scaleYH * drawY'
        fstp dword ptr [esp+0x14]             // drawY (final) — store as 32-bit
        // UV horizontal flip when sinVal < threshold.
        fld  dword ptr [esp+0x8]              // ST(0)=sinVal
        fcomp dword ptr ds:[0x005d757c]          // compare with threshold, pop
        fnstsw ax
        test ah, 0x5
        jp   skip_flip
        mov  dword ptr [esp+0x20], 0x3f800000 // u0 = 1.0
        mov  dword ptr [esp+0x28], 0x3f000000 // u1 = 0.5
skip_flip:
        // texHandle from global pointer (if non-null).
        mov  eax, dword ptr ds:[0x00771960]
        test eax, eax
        jz   skip_tex
        mov  esi, dword ptr [eax]
skip_tex:
        // DrawQuad(texHandle, drawX, drawY, drawW, drawH, 0xffffffff, &uvs).
        // Note: per orig, the FPU stack still has drawY-final at ST(0);
        // but the args are PUSHED as int dwords from the local stack slots.
        mov  eax, dword ptr [esp+0x4]          // 0xffffffff (color)
        mov  ecx, dword ptr [esp+0x1c]         // drawH
        lea  edx, [esp+0x20]                   // &uvs
        push edx
        mov  edx, dword ptr [esp+0x1c]         // drawW (post-push)
        push eax                               // color
        mov  eax, dword ptr [esp+0x1c]         // drawY (post-push)
        push ecx                               // drawH
        mov  ecx, dword ptr [esp+0x1c]         // drawX
        push edx                               // drawW
        push eax                               // drawY
        push ecx                               // drawX
        push esi                               // texHandle
        // Pop the still-pending FPU value before calling out (it was stack
        // garbage — orig doesn't pre-pop because cdecl callee preserves FPU).
        // (Actually orig leaves ST(0) live across the CALL; we mirror that.)
        mov  ecx, 0x00450b10
        call ecx
        // Advance spin angle accumulator.
        fld  dword ptr ds:[0x0067d974]
        fadd dword ptr ds:[0x005cc56c]
        add  esp, 0x1c
        pop  esi
        fstp dword ptr ds:[0x0067d974]
        add  esp, 0x2c
        ret
    }
}

RH_ScopedInstall(HudSpinCoinAnim, 0x00428450);  // re-enabled 2026-05-24 phase-a3 x87-inline-asm rewrite GREEN (10/10)
