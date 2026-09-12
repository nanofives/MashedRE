// Mashed RE — promote-round round 249 (RenderWare plugin-registry forwarder).
//
// Function in this file:
//   0x004c2d90  RwEngineRegisterPlugin — 36-byte forwarder into the plugin-registry
//               core at 0x004d7de0, binding the engine registry &DAT_00617fe0 as the
//               first argument and a hard-coded 0 as the sixth.
//
// Analysis note: re/analysis/render_promote_c2_rw_plugin/0x004c2d90.md
// Semantics of the two literals recovered 2026-09-12 while closing U-4388.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RwEngineRegisterPlugin  --  0x004c2d90
//
// Original: FUN_004c2d90 (36 bytes, 0x004c2d90..0x004c2db3)
//
// Full disassembly (read from original\MASHED.exe.unpatched, 2026-09-12):
//   0x004c2d90  mov eax, [esp+0x10]     ; param_4
//   0x004c2d94  mov ecx, [esp+0x0c]     ; param_3
//   0x004c2d98  mov edx, [esp+0x08]     ; param_2
//   0x004c2d9c  push 0                  ; callee arg 6  <- HARD-CODED LITERAL
//   0x004c2d9e  push eax                ; callee arg 5  = param_4
//   0x004c2d9f  mov eax, [esp+0x0c]     ; param_1 (ESP is entry-8 after 2 pushes)
//   0x004c2da3  push ecx                ; callee arg 4  = param_3
//   0x004c2da4  push edx                ; callee arg 3  = param_2
//   0x004c2da5  push eax                ; callee arg 2  = param_1
//   0x004c2da6  push 0x617fe0           ; callee arg 1  <- HARD-CODED REGISTRY
//   0x004c2dab  call 0x4d7de0
//   0x004c2db0  add esp, 0x18           ; __cdecl, 6 args
//   0x004c2db3  ret
//
// So the call is exactly:
//   FUN_004d7de0(&DAT_00617fe0, param_1, param_2, param_3, param_4, 0)
//
// RETURN VALUE — Ghidra types this `void`, and that is WRONG. There is no
// instruction between `call` and `ret` that touches EAX, so the callee's EAX
// falls straight through. The sole in-tree caller relies on it:
// FUN_00472380 does `iVar1 = FUN_004c2d90(0,4,...); if (iVar1 < 0) return false;`
// A void port would drop the registry offset the caller tests. [0x00472380]
//
// ARGUMENT MEANING — recovered 2026-09-12 from the callee's own prototype and
// field stores (U-4388), not guessed:
//   FUN_004d7de0(int* registry, int size, int id, code* cb1, code* cb2, code* cb3)
//     size (param_1 here) is a BYTE SIZE added to the registry's running block
//       offset:  `iVar1 = (param_2 + 3U & 0xfffffffc) + *param_1;`
//       then `*piVar3 = *param_1; *param_1 = iVar1; piVar3[1] = param_2;`
//     id   (param_2 here) is the identifier KEY, duplicate-checked by
//       `if (piVar3[2] == param_3)` and stored as `piVar3[2] = param_3;`
//     cb1/cb2/cb3 are stored at piVar3[8]/[9]/[10], each defaulting to the
//       identity stub FUN_004d7ff0 when NULL.
//   That is the RenderWare RwEngineRegisterPlugin(size, pluginID, initCB, termCB)
//   shape: this shim binds the engine registry and passes NULL for the third
//   callback slot.
//
// Constants (both cited from the body above):
//   0x00617fe0 — the engine plugin registry                      [0x004c2da6]
//   0                — the third callback slot, always NULL      [0x004c2d9c]
//
// Uncertainties: none open against this function. U-4388 (the meaning of the
// leading 0 and the 4 at the FUN_00472380 call site) was RESOLVED 2026-09-12.
//
// ref: re/analysis/render_promote_c2_rw_plugin/0x004c2d90.md
// ---------------------------------------------------------------------------

// The plugin-registry core. Called at its original absolute address: it is not
// ported, and both the original and this reimplementation must reach the SAME
// callee so the A/B diff compares the forwarding, not two different registries.
typedef int(__cdecl* RwPluginRegistryAddPlugin_t)(std::uint32_t /*registry*/,
                                                  std::uint32_t /*size*/,
                                                  std::uint32_t /*id*/,
                                                  std::uint32_t /*cb1*/,
                                                  std::uint32_t /*cb2*/,
                                                  std::uint32_t /*cb3*/);

// 0x004c2d90
extern "C" __declspec(dllexport) int __cdecl RwEngineRegisterPlugin(
    std::uint32_t size, std::uint32_t id, std::uint32_t initCB, std::uint32_t termCB)
{
    const RwPluginRegistryAddPlugin_t addPlugin =
        reinterpret_cast<RwPluginRegistryAddPlugin_t>(0x004d7de0u);

    // FUN_004d7de0(&DAT_00617fe0, param_1, param_2, param_3, param_4, 0)
    // [0x004c2d9c .. 0x004c2dab]; EAX falls through to our return. [0x004c2db3]
    return addPlugin(0x00617fe0u, size, id, initCB, termCB, 0u);
}

RH_ScopedInstall(RwEngineRegisterPlugin, 0x004c2d90);
