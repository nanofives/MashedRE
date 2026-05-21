// Mashed RE — Settings/video-config save function reimplementation.
// Session: c3-batch-i-s4   Branch: c3/batch-i-s4
//
// Single function: 0x004989b0  ConfigSave  — int(void)
// Companion to CONFIG_LOAD_FN at 0x00498950 already in Save/SettingsConfig.cpp.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
//
// Function: CONFIG_SAVE_FN  writes 512 bytes from global settings buffer
// at 0x00773208 to "videocfg.bin" via game CRT _fsopen/_fwrite/_fclose.
//
// Decomp verified via disassembly at 0x004989b0..0x004989fc (76 bytes):
//   PUSH ESI                                            ; 0x004989b0
//   CALL 0x00498910 (ConfigFilenameGet)                 ; 0x004989b1
//   MOV ESI, EAX                                        ; 0x004989b6
//   PUSH ESI; PUSH 0x5d0158                             ; 0x004989b8/b9
//   CALL 0x00496400 (ConfigLogDebug)                    ; 0x004989be
//   PUSH 0x5d00d8 ("wb"); PUSH ESI                      ; 0x004989c3/c8
//   CALL 0x004a4541 (_fsopen wrapper, 2-arg form)       ; 0x004989c9
//   MOV ESI, EAX                                        ; 0x004989ce
//   ADD ESP, 0x10                                       ; 0x004989d0
//   TEST ESI, ESI                                       ; 0x004989d3
//   JZ 0x004989f9                                       ; 0x004989d5
//   PUSH ESI                                            ; 0x004989d7  (file*)
//   PUSH 0x200                                          ; 0x004989d8  (count)
//   PUSH 0x1                                            ; 0x004989dd  (size)
//   PUSH 0x773208                                       ; 0x004989df  (buf)
//   CALL 0x004a4b22 (_fwrite)                           ; 0x004989e4
//   PUSH ESI; CALL 0x004a4368 (_fclose)                 ; 0x004989e9/ea
//   ADD ESP, 0x14; MOV EAX, 1; POP ESI; RET             ; success: return 1
//   XOR EAX, EAX; POP ESI; RET                          ; fail   : return 0
//
// Note: 0x004a4541 is called with TWO args here (filename, mode) — different
// from ConfigLoad's THREE-arg use (filename, mode, _SH_DENYNO). The decomp
// shows just `FUN_004a4541(filename, &DAT_005d00d8)`. The thunk at 0x004a4541
// reads _SH_DENYNO from a fixed source (FidDB identifies it as a wrapper
// that defaults the share flag). Match the original by passing 2 args.
// We declare game_fsopen2 with the 2-arg signature seen at the call site.
//
// Side effect: writes 512 bytes to videocfg.bin on disk every call.
// At quiescent main-menu state, the file exists and is writable; both orig
// and reimpl will hit the success path and return 1.

#pragma optimize("", off)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Naked thunks to game internal functions used by 0x004989b0.
// __declspec(naked) + push+ret bypasses MSVC LTCG constant folding,
// matching the convention from Save/SettingsConfig.cpp.
//
//   0x00498910  ConfigFilenameGet (game-internal) -> char*
//   0x00496400  ConfigLogDebug    (game-internal) -> void (variadic)
//   0x004a4541  _fsopen wrapper   (2-arg form at this call site)
//   0x004a4b22  _fwrite
//   0x004a4368  _fclose
// ---------------------------------------------------------------------------

__declspec(naked) static char* __cdecl game_ConfigFilenameGet()
{
    __asm {
        push 0x00498910
        ret
    }
}

__declspec(naked) static void __cdecl game_ConfigLogDebug(const char* /*fmt*/, ...)
{
    __asm {
        push 0x00496400
        ret
    }
}

// 2-arg form matching the call site at 0x004989c9 — original pushes only
// filename and mode (no _SH_DENYNO). The wrapper at 0x004a4541 supplies
// the share flag internally.
__declspec(naked) static void* __cdecl game_fsopen2(const char* /*path*/, const char* /*mode*/)
{
    __asm {
        push 0x004a4541
        ret
    }
}

__declspec(naked) static unsigned int __cdecl game_fwrite(const void* /*buf*/, unsigned int /*esz*/, unsigned int /*cnt*/, void* /*stream*/)
{
    __asm {
        push 0x004a4b22
        ret
    }
}

__declspec(naked) static int __cdecl game_fclose(void* /*stream*/)
{
    __asm {
        push 0x004a4368
        ret
    }
}

// ---------------------------------------------------------------------------
// Global address constants (cited from 0x004989b0 disassembly).
// ---------------------------------------------------------------------------

// 512-byte global settings buffer (passed to _fwrite at 0x004989df).
static constexpr std::uintptr_t kSettingsBufAddr = 0x00773208;

// Mode string "wb\0" in read-only data (PUSH 0x5d00d8 at 0x004989c3).
static constexpr std::uintptr_t kFopenWriteMode  = 0x005d00d8;

// Format string "Saving video cfg to %s\n" (PUSH 0x5d0158 at 0x004989b9).
static constexpr std::uintptr_t kFmtSavingCfg    = 0x005d0158;

// _fwrite size & count (cited at 0x004989dd / 0x004989d8).
static constexpr unsigned int   kSettingsByteCnt = 0x200u;  // 512 bytes

// ---------------------------------------------------------------------------
// 0x004989b0  ConfigSave  — int(void)
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int __cdecl ConfigSave() {
    char* filename = game_ConfigFilenameGet();
    game_ConfigLogDebug(reinterpret_cast<const char*>(kFmtSavingCfg), filename);

    // _fsopen(filename, "wb") — 2-arg call form per disassembly.
    void* fp = game_fsopen2(filename, reinterpret_cast<const char*>(kFopenWriteMode));
    if (fp != nullptr) {
        // _fwrite(&DAT_00773208, 1, 0x200, fp).
        game_fwrite(reinterpret_cast<const void*>(kSettingsBufAddr), 1u, kSettingsByteCnt, fp);
        // _fclose(fp).
        game_fclose(fp);
        return 1;
    }

    // File open failed — silent return 0 (no FAILED log, unlike ConfigLoad).
    return 0;
}

RH_ScopedInstall(ConfigSave, 0x004989b0);
