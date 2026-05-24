// Mashed RE - Input/DInput_j5 cluster (C2->C3 promotion, session c3-batch-j-s5).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file (1):
//   0x004971b0  ControllerConfigLoad_j5   per-slot controller config loader
//                                          (EAX-implicit slot index)
//
// Analysis note (per CLAUDE.md NO-GUESSING rule):
//   re/analysis/promote_c2_dinput_init/004971b0.md  (C2 plate)
//
// Calling-convention notes:
//   0x004971b0 receives the slot index implicitly via register EAX (Ghidra
//   in_EAX). The function calls FUN_00497190 (filename formatter) which
//   reads the same EAX value via FUN_004a2b60's varargs stack-frame trick.
//   We implement as __declspec(naked) so EAX is preserved across the prologue
//   and the call to 0x00497190 sees the same EAX the caller passed.
//
//   The harness extension arg_type='eax_implicit_int' (commit 656273b) builds
//   an RWX trampoline that seeds EAX with each test integer and JMPs to the
//   target. Frida diff thus exercises the real EAX-implicit ABI.
//
// Uncertainties (catalogued, non-blocking for C3):
//   U-2587  _fread size 0x200 vs slot stride 0x80 — buffer block layout
//   U-2588  in_EAX register convention (resolved by caller decomp)

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdio>


// ============================================================================
// Game function pointers (cited at exact RVA)
// ============================================================================

// 0x00497190  FUN_00497190  filename formatter -> char*
//   Returns pointer to DAT_007730e4 with "contcfgN.bin" formatted in-place.
//   Reads slot index N from EAX (in_EAX) - we leave EAX intact across the
//   call site, see naked entry below.
//   Cited at: 0x004971bb (CALL).
using FilenameFormatFn_t = char* (__cdecl*)();
static FilenameFormatFn_t const g_FormatContcfgName =
    reinterpret_cast<FilenameFormatFn_t>(0x00497190u);

// 0x00496400  ConfigLogDebug  (variadic log writer) — already C3 in
//                              Save/SettingsConfig.cpp. We call its export
//                              directly to avoid double-hooking.
extern "C" __declspec(dllexport) void __cdecl ConfigLogDebug(const char* fmt, ...);

// 0x004a4541  FsopenSafe  (2-arg form: filename, mode) — already C3 in
//                          Save/FsOpen.cpp. Returns FILE* (or NULL on fail).
extern "C" __declspec(dllexport) FILE* __cdecl FsopenSafe(
    char* filename, char* mode);

// CRT _fread and _fclose — game uses static-CRT thunks; we call libc directly,
// which is the same code (statically linked into MASHED.exe).
//   Cited at: 0x004971ef (CALL _fread), 0x004971fb (CALL _fclose).


// ============================================================================
// Constants (cited at exact RVA in re/analysis/promote_c2_dinput_init/004971b0.md)
// ============================================================================

// 0x005cf010 — "rb\0" mode string (confirmed by memory_read this session).
static constexpr std::uintptr_t kModeRb_Addr = 0x005cf010u;

// 0x007e95c0 — controller config buffer base.
static constexpr std::uintptr_t kCtrlCfgBase = 0x007e95c0u;

// Per-slot byte stride. RAW DISASSEMBLY at 0x004971e7 shows `SHL ESI, 9` =
// slot << 9 = slot * 0x200. The C2 analysis note 004971b0.md cites 0x80;
// that was inferred from the C1 plate's Ghidra decomp which split the
// arithmetic differently. The instruction-level evidence is unambiguous:
// stride is 0x200 (matches read size).  U-2587 is RESOLVED to hypothesis (b)
// adjusted: slot stride 0x200 == _fread size — no overlap, no sub-slot.
static constexpr std::uint32_t kCtrlCfgStride = 0x200u;

// 0x200 — _fread size argument (bytes per call).
static constexpr std::uint32_t kCtrlCfgReadSize = 0x200u;

// 1 — _fread element-size argument.
static constexpr std::uint32_t kFreadElemSize = 1u;


// ============================================================================
// 0x004971b0  ControllerConfigLoad_j5
//
// Original 114 bytes (0x004971b0..0x00497222).
// Slot index in EAX (in_EAX). Returns 1 on success, 0 on file-not-found.
//
// Mechanical sequence (cited verbatim from 004971b0.md C2 plate):
//   1. 0x004971bb  CALL 0x00497190                  ; uVar1 = filename
//                                                   ;        ("contcfgN.bin")
//   2. 0x004971c4  CALL ConfigLogDebug("Attempting to load controller "
//                                       "config %s\n", uVar1)
//   3. 0x004971d0  CALL FsopenSafe(uVar1, &DAT_005cf010)   ; _fsopen rb
//   4. branch on FILE* == NULL:
//        NULL  -> CALL ConfigLogDebug("\tUnable to load ... %s\n", uVar1)
//                 RETURN 0
//        valid -> 0x004971ef  CALL _fread(&DAT_007e95c0 + EAX*0x80, 1, 0x200, _File)
//                 0x004971fb  CALL _fclose(_File)
//                 CALL ConfigLogDebug("\tLoaded controller config %s\n", uVar1)
//                 RETURN 1
//
// Naked entry preserves EAX across the prologue so FUN_00497190's EAX-implicit
// read sees the slot the caller passed.
// ============================================================================

static int __stdcall ControllerConfigLoad_Body(int slot_idx)
{
    // FUN_00497190 reads slot from EAX — we restore EAX before calling it.
    // To keep this body compiler-friendly we use an inline-asm island to
    // make the EAX-implicit call, then read its return through EAX into a
    // local.  The naked outer entry has already arranged EAX = slot_idx
    // on entry to *this* body; but a regular C function body would clobber
    // EAX during prologue. We therefore re-seed EAX from `slot_idx` here.
    char* filename = nullptr;
    __asm {
        mov     eax, slot_idx
        call    g_FormatContcfgName
        mov     filename, eax
    }

    ConfigLogDebug("Attempting to load controller config %s\n", filename);

    FILE* fp = FsopenSafe(filename,
                          reinterpret_cast<char*>(kModeRb_Addr));   // 0x005cf010 = "rb"
    if (fp == nullptr) {
        ConfigLogDebug("\tUnable to load controller config %s\n", filename);
        return 0;
    }

    // _fread(&DAT_007e95c0 + slot*0x80, 1, 0x200, fp)   ; 0x004971ef
    void* dst = reinterpret_cast<void*>(
        kCtrlCfgBase + static_cast<std::uintptr_t>(
            static_cast<std::uint32_t>(slot_idx) * kCtrlCfgStride));
    fread(dst, kFreadElemSize, kCtrlCfgReadSize, fp);
    fclose(fp);                                                     // 0x004971fb

    ConfigLogDebug("\tLoaded controller config %s\n", filename);
    return 1;
}

// Naked entry: slot index via EAX, no stack args. Bridge EAX into body's
// __stdcall first slot.
extern "C" __declspec(dllexport) __declspec(naked)
int __cdecl ControllerConfigLoad_j5()
{
    __asm {
        push    ebp
        mov     ebp, esp
        push    eax                  // slot index from EAX
        call    ControllerConfigLoad_Body
        mov     esp, ebp
        pop     ebp
        ret
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ControllerConfigLoad_j5, 0x004971b0);
