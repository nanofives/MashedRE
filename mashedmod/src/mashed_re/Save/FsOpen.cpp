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
// Cited at: 0x004a4541 (push 0x40).
static constexpr int kShareFlagDenyNo = 0x40;

#ifdef MASHED_STANDALONE
// The standalone has no game CRT; it opens through its own _fsopen.
extern "C" FILE* __cdecl _fsopen(const char* _Filename, const char* _Mode, int _ShFlag);
#else
// 0x004a44e5  game __fsopen(filename, mode, shflag), cdecl (the caller pops 0xc at 0x004a4550).
// The FILE* it returns belongs to the game's CRT stream table, which is what the game's own
// _fread/_fclose expect.
using GameFsopenFn = FILE* (__cdecl*)(const char*, const char*, int);
static const GameFsopenFn s_GameFsopen = reinterpret_cast<GameFsopenFn>(0x004a44e5u);
#endif

// 0x004a4541  FsopenSafe
// Disasm 0x004a4541..0x004a4553: push 0x40; push [esp+0xc] (mode); push [esp+0xc] (filename);
// call 0x004a44e5; add esp,0xc; ret. EAX passes through.
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn, bootio). The old port called the DLL's
// CRT _fsopen, so the dev .asi handed the game a FILE* from a different CRT. The boot patch
// fix_fopen (0x004a4541 -> cave 0x0050f6c4) adds `cmp eax,0x10000; jae; xor eax,eax` after the
// same call; that guard is a workaround on the reference and is not part of this port. The
// HookSystem kBootPatched guard refuses to install over the patch, so this body runs in the
// dev .asi only when called by name.
extern "C" __declspec(dllexport) FILE* __cdecl FsopenSafe(
    char* filename,
    char* mode)
{
#ifdef MASHED_STANDALONE
    return _fsopen(filename, mode, kShareFlagDenyNo);
#else
    return s_GameFsopen(filename, mode, kShareFlagDenyNo);     // 0x004a454b
#endif
}

RH_ScopedInstall(FsopenSafe, 0x004a4541);  // re-enabled 2026-05-24 batch-save-e
