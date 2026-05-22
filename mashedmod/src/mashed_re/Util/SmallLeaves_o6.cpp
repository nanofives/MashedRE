// Mashed RE — Util small leaf functions, c3-batch-o session 6.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file:
//   0x00430790  FUN_00430790  — 5-byte global getter: returns DAT_0067f17c.
//
// Source analysis: re/analysis/promote_c1_low_ab1/0x00430790.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// FUN_00430790  --  0x00430790
//
// Original: FUN_00430790 (5 bytes, 0x00430790..0x00430794)
// Signature: undefined4 FUN_00430790(void) — no parameters; returns a 4-byte value.
// Body:
//   MOV EAX, [DAT_0067f17c]   ; 0x00430790 — loads DAT_0067f17c into EAX
//   RETN                       ; 0x00430794
//
// Pure global-read getter. Returns the 4-byte value at DAT_0067f17c with no
// branches, writes, or callees.
//
// 7 callers confirmed by Ghidra MCP:
//   0x0040e590  FUN_0040e590   (util C1)
//   0x00413fa0  FUN_00413fa0   (gameplay C1)
//   0x004299d0  FUN_004299d0   (util C2)
//   0x00429a30  FUN_00429a30   (frontend C3 — MenuMenusBE)
//   0x00429aa0  FUN_00429aa0   (util/frontend)
//   0x00462520  FUN_00462520   (audio C1)
//   0x0046b540  FUN_0046b540
//
// Constants (cited from function body at 0x00430790):
//   0x0067f17c  — DAT_0067f17c: global holding the returned 4-byte value
//
// Uncertainties (non-blocking for C3):
//   U-3894: DAT_0067f17c content type not mechanically confirmed (writes not traced).
//   U-3895: Subsystem assignment — hooks.csv says "util" but 3 callers in 0x004299xx
//           range; reclassification may be warranted after callers are analysed.
//
// ref: re/analysis/promote_c1_low_ab1/0x00430790.md
// ---------------------------------------------------------------------------

// 0x00430790
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat0067f17c() {
    // Return DAT_0067f17c.
    // 0x0067f17c cited at 0x00430790 body.
    return *reinterpret_cast<const std::uint32_t*>(0x0067f17cu);
}

RH_ScopedInstall(GetDat0067f17c, 0x00430790);
