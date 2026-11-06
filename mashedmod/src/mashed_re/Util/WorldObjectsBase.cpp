// Mashed RE — world-object count/base getter: ONE body for both build targets (D4-DC-INC-ai, 2026-11-01).
//
// Before this file the RVA had two bodies, the dual-copy class (re/analysis/DUAL_COPY_AUDIT_2026-09-29.md,
// ROADMAP D4):
//   Util/PromoLoop_round20.cpp           .asi-only, installed, the body hooks.csv's C3 evidence covers
//   TG_484c70 (Ai/AiModeSeven_naked.inc) exe-only, private verbatim naked copy called by FUN_00414c30
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp. The body moved here unchanged; the exe's naked
// caller now `call`s it (re/tools/naked_specs/mode7_m4.json). RH_ScopedInstall installs it in the .asi and is
// a no-op in the exe. Both forms read 0x006e70d8 and return the constant 0x006dccb8, nothing else.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis: re/analysis/bucket_ai_00452eb0_004c3df0/00484c70.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// WorldObjectsBaseGet  --  0x00484c70   (subsystem: ai)
//
// Original: FUN_00484c70 (18 bytes, 0x00484c70..0x00484c81)
// Bytes: 8B 44 24 04 / 8B 0D D8 70 6E 00 / 89 08 / B8 B8 CC 6D 00 / C3
//   (mov eax,[esp+4]; mov ecx,[0x006e70d8]; mov [eax],ecx;
//    mov eax,0x006dccb8; ret)
// Signature: undefined4* FUN_00484c70(undefined4* param_1)
//
// Writes the world-object count to *param_1 and RETURNS the array base. The
// diff folds the return (fold_ret) so both the out (count) and the returned
// base are verified.
//
// Constants (cited from function body at 0x00484c70):
//   0x006e70d8 — world-object count (written to *param_1)
//   0x006dccb8 — world-objects array base (returned; stride 0x8c per caller)
//
// Caller: FUN_00414c30 (ai obstacle-avoidance, C2). Leaf.
// ---------------------------------------------------------------------------

// MATCH-S14: the body of 0x00484c70 (WorldObjectsBaseGet) moved to Match/MatchS14_00484c70.cpp,
// which compiles the byte-matched source re/match/s14/00484c70.c.
