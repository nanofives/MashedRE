// Mashed RE — Settings/video-config cluster reimplementations.
// Session: c3-batch-e-s4   Branch: c3/batch-e-s4
//
// Four functions from the CONFIG subsystem that handle logging to the game's
// log-file handle and loading/naming the video config ("videocfg.bin").
//
// Global addresses cited here:
//   0x00772fbc  — global FILE* log handle (written by window-create handler)
//   0x00616038  — MSVC __security_cookie (used by ConfigLogDebug stack guard)
//   0x007731e8  — mutable copy buffer for the filename "videocfg.bin"
//   0x00773208  — 512-byte global settings buffer (filled by ConfigLoad)
//
// Calling convention for all four: __cdecl (C ABI).  ConfigLogDebug is variadic.
//
// CRT note: the game uses a statically-linked MSVC CRT from ~2003.  Cross-CRT
// FILE* sharing crashes because our MSVC 2022 CRT and the game's CRT have
// different FILE struct layouts.  We call the game's own CRT thunks by their
// absolute RVAs.
//
// OPTIMIZER NOTE: MSVC /O2 with LTCG folds away ALL our attempts to call
// the game's fputs by address — function pointers (const/volatile), inline
// __asm, and naked __declspec(naked)+asm are all overridden.  We therefore
// disable whole-file optimization with #pragma optimize("", off).

// Disable whole-file optimization so MSVC cannot substitute its own CRT
// fputs/vsprintf/etc. symbols for our address-based game CRT calls.
#pragma optimize("", off)

#include "../Core/HookSystem.h"

#include <cstdarg>
#include <cstdint>
#include <share.h>   // _SH_DENYNO

// ---------------------------------------------------------------------------
// Naked thunks to game CRT functions.
//
// __declspec(naked) forces MSVC to emit ONLY the __asm body — no prologue,
// no epilogue, no inlining, no substitution.  Each thunk pushes the args
// already on the caller's stack (they're in the right positions for __cdecl),
// jumps to the game function, which returns to the original caller.
//
// Calling convention: all are __cdecl.
//
// game_fputs(const char* s, void* stream) -> int
//   at 0x004a4254  [MASHED.exe]
// ---------------------------------------------------------------------------

// 0x004a4254  _fputs(str, stream) -> int
// Uses push+ret to jump to the absolute game VA, bypassing MSVC LTCG
// constant-folding which would otherwise substitute the CRT fputs symbol.
__declspec(naked) static int __cdecl game_fputs(const char* /*s*/, void* /*stream*/)
{
    __asm {
        push 0x004a4254
        ret
    }
}

// 0x004a42c5  FUN_004a42c5 vsprintf variant: vsprintf(buf, fmt, ap) -> int
__declspec(naked) static int __cdecl game_vsprintf(char* /*buf*/, const char* /*fmt*/, va_list /*ap*/)
{
    __asm {
        push 0x004a42c5
        ret
    }
}

// 0x004a4541  FUN_004a4541 _fsopen wrapper: _fsopen(path, mode, shflag) -> void*
__declspec(naked) static void* __cdecl game_fsopen(const char* /*path*/, const char* /*mode*/, int /*shflag*/)
{
    __asm {
        push 0x004a4541
        ret
    }
}

// 0x004a49cf  _fread(buf, esz, cnt, stream) -> unsigned int
__declspec(naked) static unsigned int __cdecl game_fread(void* /*buf*/, unsigned int /*esz*/, unsigned int /*cnt*/, void* /*stream*/)
{
    __asm {
        push 0x004a49cf
        ret
    }
}

// 0x004a4368  _fclose(stream) -> int
__declspec(naked) static int __cdecl game_fclose(void* /*stream*/)
{
    __asm {
        push 0x004a4368
        ret
    }
}

// ---------------------------------------------------------------------------
// Global address constants
// ---------------------------------------------------------------------------

// FILE* log handle written by the window-create handler (FUN_00496490).
// NULL until the log file is successfully opened.
// Cited in: 0x004963e0, 0x00496400
static constexpr std::uintptr_t kLogHandleAddr     = 0x00772fbc;

// Mutable 13-byte filename buffer in writable data.
// ConfigFilenameGet writes "videocfg.bin\0" here and returns a pointer to it.
// Cited in: 0x00498910
static constexpr std::uintptr_t kFilenameBufAddr   = 0x007731e8;

// Source string "videocfg.bin\0" in read-only data.
// Cited at: 0x00498910  as  s_videocfg_bin_005d012c
static constexpr std::uintptr_t kFilenameStrAddr   = 0x005d012c;

// 512-byte global video-config settings buffer populated by fread.
// Cited at fread call inside 0x00498950.
static constexpr std::uintptr_t kSettingsBufAddr   = 0x00773208;

// ---------------------------------------------------------------------------
// 0x004963e0  ConfigLogError  — void(char*)
// ---------------------------------------------------------------------------
// Writes param_1 directly to the game log via _fputs.
// If the log handle is NULL (file not opened yet), silently returns.
// Pure leaf; 23 bytes in original.
//
// Called from: ConfigLoad (0x00498950) with literal "\tFAILED\n".
//
extern "C" __declspec(dllexport) void __cdecl ConfigLogError(const char* msg) {
    void* log_handle = *reinterpret_cast<void**>(kLogHandleAddr);
    if (log_handle == nullptr) {
        return;
    }
    game_fputs(msg, log_handle);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ConfigLogError, 0x004963e0);

// ---------------------------------------------------------------------------
// 0x00496400  ConfigLogDebug  — void(char*, ...)
// ---------------------------------------------------------------------------
// Variadic log writer: formats into a 512-byte stack buffer via vsprintf,
// then writes to the log file via _fputs.
// If the log handle is NULL, silently returns.
// Stack-cookie guarded in original (MSVC __security_cookie at 0x00616038).
//
// Analysis note 00496400.md: FUN_004a42c5(local_204, param_1, &stack0x00000008)
// matches vsprintf(dst, fmt, va_list) — we call the game's vsprintf variant at
// 0x004a42c5 to maintain identical formatting behaviour.
//
extern "C" __declspec(dllexport) void __cdecl ConfigLogDebug(const char* fmt, ...) {
    void* log_handle = *reinterpret_cast<void**>(kLogHandleAddr);
    if (log_handle == nullptr) {
        return;
    }
    char buf[512];
    va_list args;
    va_start(args, fmt);
    game_vsprintf(buf, fmt, args);
    va_end(args);
    game_fputs(buf, log_handle);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ConfigLogDebug, 0x00496400);

// ---------------------------------------------------------------------------
// 0x00498910  ConfigFilenameGet  — char*(void)
// ---------------------------------------------------------------------------
// Copies the 13-byte string "videocfg.bin\0" from the read-only source at
// 0x005d012c into the writable buffer at 0x007731e8, four bytes at a time:
//
//   buf[0..3]  <- src[0..3]   ("vide")
//   buf[4..7]  <- src[4..7]   ("ocfg")
//   buf[8..11] <- src[8..11]  (".bin")
//   buf[12]    <- src[12]     ('\0')
//
// Returns pointer to buf (0x007731e8).
// Pure leaf; 49 bytes in original. Two callers: ConfigLoad, CONFIG_SAVE_FN.
//
extern "C" __declspec(dllexport) char* __cdecl ConfigFilenameGet() {
    const char* src = reinterpret_cast<const char*>(kFilenameStrAddr);
    char*       dst = reinterpret_cast<char*>(kFilenameBufAddr);

    // Four-byte writes matching the original's DWORD-at-a-time copy pattern.
    // bytes 0..3
    *reinterpret_cast<std::uint32_t*>(dst + 0) =
        *reinterpret_cast<const std::uint32_t*>(src + 0);
    // bytes 4..7
    *reinterpret_cast<std::uint32_t*>(dst + 4) =
        *reinterpret_cast<const std::uint32_t*>(src + 4);
    // bytes 8..11
    *reinterpret_cast<std::uint32_t*>(dst + 8) =
        *reinterpret_cast<const std::uint32_t*>(src + 8);
    // byte 12 (null terminator)
    dst[12] = src[12];

    return dst;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ConfigFilenameGet, 0x00498910);

// ---------------------------------------------------------------------------
// 0x00498950  ConfigLoad  — int(void)
// ---------------------------------------------------------------------------
// Loads 512 bytes from "videocfg.bin" into the global settings buffer.
//
// Flow (from analysis note 00498950.md):
//   1. Call ConfigFilenameGet() -> pointer to "videocfg.bin" in mutable buf.
//   2. Call ConfigLogDebug("Loading video cfg from %s\n", filename).
//   3. _fsopen(filename, "rb", _SH_DENYNO) -> FILE*.
//   4. If FILE* != NULL:
//        fread(&DAT_00773208, 1, 0x200, file_ptr);
//        fclose(file_ptr);
//        return 1;
//   5. Else:
//        ConfigLogError("\tFAILED\n");
//        return 0;
//
// _SH_DENYNO = 0x40 (allow other processes to read and write).
// fread element size = 1, count = 0x200 (512 bytes).
// Mode string "rb\0" sourced from game read-only data at 0x005cf010.
//
extern "C" __declspec(dllexport) int __cdecl ConfigLoad() {
    char* filename = ConfigFilenameGet();
    ConfigLogDebug("Loading video cfg from %s\n", filename);

    // Mode string "rb" at 0x005cf010 (cited in analysis note).
    const char* mode = reinterpret_cast<const char*>(0x005cf010);
    void* fh = game_fsopen(filename, mode, _SH_DENYNO);
    if (fh != nullptr) {
        game_fread(reinterpret_cast<void*>(kSettingsBufAddr), 1, 0x200, fh);
        game_fclose(fh);
        return 1;
    } else {
        ConfigLogError("\tFAILED\n");
        return 0;
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ConfigLoad, 0x00498950);
