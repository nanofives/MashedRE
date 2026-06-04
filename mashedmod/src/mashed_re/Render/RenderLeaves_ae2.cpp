// Mashed RE — Render C2->C3 leaf batch (c3-batch-ae session 2).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// Two genuine render leaves harvested from the ae2 candidate set (the other 9
// candidates were SKIPPED: D3D vtable-dispatch draw 0x004430e0 needs live state;
// the trail-pool allocator 0x00477b60 mutates a global slot-bitfield so an A/B
// pair diverges; the global-index setter 0x00477e00 has no existing arg_type;
// and the six 0x0047axxx/0x0047bxxx "script handlers" fetch their args from a
// live Lua-5.0 stack via FUN_004b6fc0/FUN_004b7090 -> needs-live-state).
//
// Both reimpls match the original's __cdecl convention (verified RET with no
// immediate in the disassembly) so the inline-JMP install is ABI-correct, and
// are bit-identity verified via re/frida/run_diff_warm.py against the original.
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x004b40c0  FUN_004b40c0  RenderElemArrayCopy
// 2-arg __cdecl. Copies `count` dwords from the array pointed to by the
// descriptor field (param_1+0x20) into the caller buffer param_2, where the
// element count is read from (param_1+0x24). Guarded by param_2 != 0 and a
// SIGNED count > 0. Pure leaf (no callees). The "collect" companion to the
// FUN_004e89a0 RenderWare scene-graph element-pointer "forall".
//
// ASM (0x004b40c0..0x004b40e6):
//   MOV  EDX,[ESP+4]        ; param_1
//   MOV  EAX,[EDX+0x24]     ; count = *(param_1+0x24)
//   PUSH ESI
//   MOV  ESI,[ESP+0xc]      ; param_2 (dst)   (post-push: orig ESP+8)
//   TEST ESI,ESI
//   JZ   end                ; param_2 == 0 -> return
//   XOR  ECX,ECX            ; i = 0
//   TEST EAX,EAX
//   JLE  end                ; (signed) count <= 0 -> return
//   PUSH EDI
//  loop:
//   MOV  EDI,[EDX+0x20]     ; src base reloaded each iter (JL target)
//   MOV  EDI,[EDI+ECX*4]    ; src[i]
//   MOV  [ESI+ECX*4],EDI    ; dst[i] = src[i]
//   INC  ECX
//   CMP  ECX,EAX
//   JL   loop
//   POP  EDI
//  end:
//   POP  ESI
//   RET                     ; no immediate -> __cdecl
// ref: re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x004b40c0.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport)
void __cdecl RenderElemArrayCopy(int param_1, int param_2) {
    const int count = *reinterpret_cast<const int*>(param_1 + 0x24);
    if (param_2 != 0) {
        for (int i = 0; i < count; ++i) {  // signed; count <= 0 -> no copy
            *reinterpret_cast<int*>(param_2 + i * 4) =
                *reinterpret_cast<const int*>(
                    *reinterpret_cast<const int*>(param_1 + 0x20) + i * 4);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 0x00478cc0  FUN_00478cc0  RenderWorldStateZeroFill
// 1-arg __cdecl. Zero-fills exactly 0x418f (16783) dwords (67132 bytes) starting
// at param_1 — an inline REP STOSD, equivalent to memset(param_1, 0, 0x418f*4).
// Called once from FUN_00479330 (the render world-state initializer).
//
// ASM (0x00478cc0..0x00478ccf):
//   PUSH EDI
//   MOV  EDI,[ESP+8]        ; param_1  (post-push: orig ESP+4)
//   MOV  ECX,0x418f         ; dword count
//   XOR  EAX,EAX            ; fill value 0
//   REP STOSD               ; [EDI..] = 0 x 0x418f dwords
//   POP  EDI
//   RET                     ; no immediate -> __cdecl
// ref: re/analysis/render_2_c1_to_c2_s2/FUN_00478cc0.md  (U-4528)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport)
void __cdecl RenderWorldStateZeroFill(uint32_t* param_1) {
    for (int n = 0x418f; n != 0; --n) {
        *param_1 = 0u;
        ++param_1;
    }
}

// ─── Hook registration ───────────────────────────────────────────────────────
// Both bit-identity verified leaves (run_diff_warm.py, c3-batch-ae-s2).
RH_ScopedInstall(RenderElemArrayCopy,      0x004b40c0);
RH_ScopedInstall(RenderWorldStateZeroFill, 0x00478cc0);
