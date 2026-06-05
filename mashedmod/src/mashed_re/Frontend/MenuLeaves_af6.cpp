// Mashed RE — Frontend menu leaves, c3-batch-af session 6 (HARVEST).
//
// Candidate authored (viable C2->C3):
//   0x00431f30  FrontendPageIdDispatch  — page-ID -> panel-flag setter
//                                         (arg_type multi_arg_global_write)
//
// Skipped this session (recorded with reasons; not synthetically diffable as
// clean leaves):
//   0x00424270  RankSortDispatcher (14-case race-position rank sort). Callee-gate
//               CLEAR (all 16 callees C3/C4) and the sort_dispatch_out4 arg_type
//               fits its (int* out, int sel, int dir) ABI — I authored + diffed
//               a faithful port and it came back GREEN, BUT the diff is DEGENERATE:
//               at a quiescent menu the 4-player score/active arrays (0x007f1a14)
//               are all inactive, so EVERY sel/dir vector yields the identical
//               output [0,1,2,3] (see log/diff_rank_sort_dispatcher.csv). The
//               14-case dispatch + bubble-sort + team-collapse paths are never
//               exercised -> GREEN proves nothing about the port -> false-GREEN.
//               Defer to a populated-race canonical scenario (C3->C4 track), NOT
//               synthetic menu diff. Verbatim decomp captured in the analysis note.
//   0x004d8c40  list-splice + swap on (DAT_00911ad8 + DAT_007d3ff8 + 0x20/0x24):
//               operates on the render-device's runtime-populated doubly-linked
//               lists; void() with no controllable input -> live engine state.
//   0x0042add0  sparse kv-table walk using IMPLICIT register keys (unaff_EBX,
//               unaff_EDI). Its sibling 0x0042ad90 + caller 0x00432b30 were both
//               REJECTED at curation for register/in_EAX args. No arg_type can
//               express EBX/EDI inputs (fastcall_reg is ECX/EDX only).
//   0x004a2b60  vsprintf-style formatter (fake stack FILE -> FUN_004a504f). Genuine
//               CRT-style leaf, but its signature is sprintf(char*, char*, ...) —
//               needs a varargs/printf observe arg_type that does NOT exist. Plan
//               forbids inventing arg_types here -> needs-new-arg_type.
//   0x004215c0  passthrough that forwards param_2 to __fastcall FUN_00420de0
//               (float accumulator writing *(float*)(edx+ecx*4), clamp 50.0).
//               U-2868: decompiler recovered only 2 stack params vs 3 at call
//               sites -> calling-convention ambiguous; writes into live per-car
//               float array -> false-GREEN/false-RED risk. Not cleanly diffable.
//   0x00497b60  player-config-detail dialog populater. param_1 = HWND; calls
//               LoadStringA/SetWindowTextA/GetDlgItem/LoadBitmapA/SendMessageA.
//               Requires a live dialog window + resource + per-player binding
//               state -> live Win32 state.
//   0x0043a610  race-result scoreboard renderer. void(); calls into the render
//               device via (**(code**)(DAT_007d3ff8 + 0x20)) + FUN_0042f8d0 /
//               FUN_00427f00 etc. -> live render state.
//   0x00431710  per-player progress-bar renderer (set B). void(); same render
//               device vtable + RW draw callees -> live render state.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis note:
//   re/analysis/options_menu/0x00431f30.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ===========================================================================
// FrontendPageIdDispatch  --  0x00431f30
//
// Original: FUN_00431f30 (226 bytes, 0x00431f30..0x00432012)
// Signature: void FUN_00431f30(int param_1)   // param_1 = page/screen ID
//
// Verbatim decomp (Mashed_pool13 clone, RVA 0x00431f30):
//   void FUN_00431f30(undefined4 param_1) {
//     FUN_00431d90();          // mass panel-flag advance/clear (C3, FrontendPanelFlagAdvance)
//     switch(param_1) {
//       case 4:    DAT_0067e7a8 = 1; break;
//       case 5:    DAT_0067e7c8 = 1; return;
//       case 6/7:  DAT_0067e7b8 = 1; return;
//       case 8:    DAT_0067e808 = 1; DAT_0067e828 = 1; return;  // two panels
//       case 9/16: DAT_0067e7e0 = 1; return;
//       case 10:   DAT_0067e7b0 = 1; return;
//       case 0xc:  DAT_0067e7d0 = 1; return;
//       case 0xf:  DAT_0067e7d8 = 1; return;
//       case 0x11: DAT_0067e7e8 = 1; return;
//       case 0x12/0x18: DAT_0067e7f0 = 1; return;
//       case 0x13: DAT_0067e7f8 = 1; return;
//       case 0x14: DAT_0067e800 = 1; return;
//       case 0x1c: DAT_0067e810 = 1; return;
//       case 0x1d: DAT_0067e818 = 1; return;
//       case 0x1e: DAT_0067e820 = 1; return;
//       case 0x1f/0x21: DAT_0067e830 = 1; return;
//       case 0x20: DAT_0067e838 = 1; return;
//     }
//     return;   // default: only FUN_00431d90() ran
//   }
//
// The 19 panel flags are u32 stores (confirmed against the C3 sibling
// FrontendPanelFlagAdvance @ 0x00431d90, which advances the same flag block as
// uint32_t). The reimpl calls the ORIGINAL FUN_00431d90 at its RVA so the
// flag-clear half is bit-identical by construction; only the single (or, for
// page 8, double) flag-set differs per page ID -> the switch is genuinely
// exercised by the diff vectors (log/diff_frontend_page_id_dispatch.csv:
// each page id yields a distinct flag-block readback; page 8 sets two flags).
//
// Callee: FUN_00431d90 (0x00431d90) FrontendPanelFlagAdvance — C3.
// Caller: FUN_0043d2a0 (0x0043d2a0) screen push-pop navigator — C2.
// ref: re/analysis/options_menu/0x00431f30.md
// ===========================================================================

// Original mass panel-flag advance/clear. [0x00431d90, C3]
typedef void(__cdecl* PFN_PanelFlagAdvance)();
static const PFN_PanelFlagAdvance s_PanelFlagAdvance_af6 =
    reinterpret_cast<PFN_PanelFlagAdvance>(0x00431d90u);

static inline void af6_set_flag(std::uintptr_t addr) {
    *reinterpret_cast<std::uint32_t*>(addr) = 1u;
}

// 0x00431f30
extern "C" __declspec(dllexport) void __cdecl FrontendPageIdDispatch(std::int32_t param_1)
{
    s_PanelFlagAdvance_af6();   // FUN_00431d90()

    switch (param_1) {
    case 4:    af6_set_flag(0x0067e7a8u); break;   // original uses `break` here
    case 5:    af6_set_flag(0x0067e7c8u); return;
    case 6:
    case 7:    af6_set_flag(0x0067e7b8u); return;
    case 8:    af6_set_flag(0x0067e808u); af6_set_flag(0x0067e828u); return;
    case 9:
    case 0x10: af6_set_flag(0x0067e7e0u); return;
    case 10:   af6_set_flag(0x0067e7b0u); return;
    case 0xc:  af6_set_flag(0x0067e7d0u); return;
    case 0xf:  af6_set_flag(0x0067e7d8u); return;
    case 0x11: af6_set_flag(0x0067e7e8u); return;
    case 0x12:
    case 0x18: af6_set_flag(0x0067e7f0u); return;
    case 0x13: af6_set_flag(0x0067e7f8u); return;
    case 0x14: af6_set_flag(0x0067e800u); return;
    case 0x1c: af6_set_flag(0x0067e810u); return;
    case 0x1d: af6_set_flag(0x0067e818u); return;
    case 0x1e: af6_set_flag(0x0067e820u); return;
    case 0x1f:
    case 0x21: af6_set_flag(0x0067e830u); return;
    case 0x20: af6_set_flag(0x0067e838u); return;
    default:   break;
    }
    return;
}

RH_ScopedInstall(FrontendPageIdDispatch, 0x00431f30);
