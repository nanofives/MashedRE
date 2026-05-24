// Mashed RE — _fsopen wrapper (settings/config subsystem).
// Analysis session: save_gamesave_d3-20260511 / settings_config-20260502
//
// 0x004a4541  FUN_004a4541  FsopenSafe
//   18-byte thunk: forces share-flag = 0x40 (_SH_DENYNO, deny-no-access /
//   full sharing) so callers only supply (filename, mode).
//   Calls __fsopen(param_1, param_2, 0x40) at 0x004a44e5.
//   Returns FILE* from __fsopen (passed through EAX).
//   Called by CONFIG_LOAD_FN with mode "rb" and CONFIG_SAVE_FN with mode "wb".
//
//   Constants:
//     0x004a4548 (approx)  0x40  _SH_DENYNO sharing flag
//   Uncertainties: none.  One-line thunk, mechanically clear.

#include "../Core/HookSystem.h"

#include <cstdio>
#include <share.h>

// _SH_DENYNO = 0x40 — permits concurrent readers and writers.
// Cited at: 0x004a4548 (approx)
static constexpr int kShareFlagDenyNo = 0x40;

// _fsopen is declared in <share.h> / <stdio.h> on MSVC.
// Prototype: FILE* __cdecl _fsopen(const char* filename, const char* mode, int shflag)
extern "C" FILE* __cdecl _fsopen(const char* _Filename, const char* _Mode, int _ShFlag);

// 0x004a4541  FsopenSafe
// Two-arg wrapper that locks the share flag to _SH_DENYNO (0x40).
extern "C" __declspec(dllexport) FILE* __cdecl FsopenSafe(
    char* filename,
    char* mode)
{
    // 0x004a4548: push 0x40 (_SH_DENYNO) as third arg before calling __fsopen
    return _fsopen(filename, mode, kShareFlagDenyNo);
}

RH_ScopedInstall(FsopenSafe, 0x004a4541);  // re-enabled 2026-05-24 batch-save-e
