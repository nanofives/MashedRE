// Mashed RE — scenario-attach writer reimplementations (c3-batch-sa2 session 1).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Session-1 candidates were largely NOT cleanly diffable via void_write_observe
// (recorded for the record, NO-GUESSING):
//   0x00486370 — reads in_EAX (hidden register param); a faithful installed
//                reimpl needs __declspec(naked) EAX handling. DEFERRED.
//   0x004864f0 — pointer-arg transform + RNG jitter + min-distance gate.
//                DEFERRED (fiddly seed of the distance state).
//   0x004862d0 — non-idempotent emitter init (loads exp_cloud2/scorch textures
//                via the 800B FUN_004770c0). DEFERRED (calling twice double-loads).
//   0x0041bf20 — zero-inits the record then increments the count, so the sentinel
//                is overwritten -> readback is constant -> DEGENERATE. DEFERRED.
// Only 0x00487d00 is a clean, fully-faithful simple writer.
//
// Verbatim transcription of the Ghidra decompilation; matches the original
// __cdecl frame so the inline-JMP redirect lines up. RVA cited per function.

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Callees (RVAs into MASHED.exe) — each call dispatches to the REAL original
// address so the diff stays bit-identical.
// ---------------------------------------------------------------------------
// 0x00476a10  FUN_00476a10 — write particle position from the transform. [C2+]
static auto* const s_F00476a10 = reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x00476a10);
// 0x004769f0  FUN_004769f0 — write particle color from the ARGB dword (by ptr). [C2+]
static auto* const s_F004769f0 = reinterpret_cast<void(__cdecl*)(void*)>(0x004769f0);
// 0x00476d00  FUN_00476d00 — submit the staged particle to the emitter (by ptr). [C2+]
static auto* const s_F00476d00 = reinterpret_cast<void(__cdecl*)(void*)>(0x00476d00);

// Globals (cited from the decompilation).
static const std::uintptr_t k_00703058 = 0x00703058u; // current particle count (observable)
static const std::uintptr_t k_00703068 = 0x00703068u; // third-emitter base (&DAT_00703068)
static const std::uintptr_t k_00703070 = 0x00703070u; // capacity (count < this gate)

// ===========================================================================
// Particle3rdEmitterSubmit  --  0x00487d00
//
// Original: void FUN_00487d00(undefined4 param_1, byte param_2)  body 82 bytes.
//
// Verbatim decompilation:
//   local_4 = (uint)param_2 << 0x18;                 // alpha -> ARGB high byte
//   if (DAT_00703058 < DAT_00703070) {               // count < capacity
//       FUN_00476a10(param_1);                        // position from transform
//       FUN_004769f0(&local_4);                       // color
//       FUN_00476d00(&DAT_00703068);                  // submit
//       DAT_00703058 = DAT_00703058 + 1;              // bump count
//   }
//
// Observable: DAT_00703058 (count++). The harness SEEDS DAT_00703058=sentinel and
// DAT_00703070=high so the body runs on both sides -> readback = sentinel+1.
// param_1 is passed a READABLE address (FUN_00476a10 only reads the transform;
// its content does not affect the count). The three callees write the particle
// buffer identically on both sides.
// ref: re/analysis/cluster_0048_first_pass/0x00487d00.md
// ===========================================================================
extern "C" __declspec(dllexport) void __cdecl Particle3rdEmitterSubmit(std::uint32_t param_1,
                                                                       std::uint8_t param_2)
{
    std::uint32_t local_4 = static_cast<std::uint32_t>(param_2) << 0x18;
    if (*reinterpret_cast<std::int32_t*>(k_00703058) <
        *reinterpret_cast<std::int32_t*>(k_00703070)) {
        s_F00476a10(param_1);
        s_F004769f0(&local_4);
        s_F00476d00(reinterpret_cast<void*>(k_00703068));
        *reinterpret_cast<std::int32_t*>(k_00703058) =
            *reinterpret_cast<std::int32_t*>(k_00703058) + 1;
    }
}

RH_ScopedInstall(Particle3rdEmitterSubmit, 0x00487d00);
