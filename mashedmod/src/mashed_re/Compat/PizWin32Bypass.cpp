// Mashed RE — Win32-layer bypass for the .piz reader's CreateFileA/ReadFile/
// CloseHandle trio.
//
// Why this exists
// ---------------
// `FUN_004b6710` (0x004b6710, PizWin32Open), `FUN_004b67e0` (0x004b67e0,
// PizWin32Read), and `FUN_004b6770` (0x004b6770, PizWin32Close) are the only
// direct callers of `CreateFileA` / `ReadFile` / `CloseHandle` for .piz
// archives. The original binary opens the file with
//
//     CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING,
//                 FILE_FLAG_NO_BUFFERING (0x20000000)
//               | FILE_FLAG_OVERLAPPED   (0x40000000),
//                 NULL);                                   // 0x004b672f .. 0x004b6740
//
// and then issues 2 KB (`0x800`-byte) reads from stack buffers
// (0x004b6a0e..0x004b6a14 in the caller `FUN_004b6940`). `FILE_FLAG_NO_BUFFERING`
// requires read size, buffer alignment, AND file offset to all be integer
// multiples of the volume's *physical* sector size. On legacy 512 B-sector
// drives 0x800 satisfies that; on modern 4096 B-sector NVMe/AF drives it does
// not — the `ReadFile` returns `ERROR_INVALID_PARAMETER (0x57)`, the overlapped
// poll loop never sees `ERROR_HANDLE_EOF (0x26)` and the caller proceeds with
// uninitialised header bytes, leading to the past-menu piz-load crashes
// documented in `re/analysis/fun_004b6940_scoping/REPORT.md`.
//
// This file installs three replacement functions that mirror the original
// control flow exactly except for two corrections:
//   1. The `CreateFileA` flags drop `FILE_FLAG_NO_BUFFERING` (kept
//      `FILE_FLAG_OVERLAPPED`).
//   2. `PizWin32Close_Compat` nulls `DAT_007d3e48` on the Win32 branch after
//      `CloseHandle`, matching what the stdio branch already does. The
//      original `FUN_004b6770` leaves a stale HANDLE in `DAT_007d3e48` on the
//      Win32 branch — every current caller goes through the wrapper
//      `FUN_004b67a0` which nulls it explicitly at 0x004b67d1, so the bug is
//      defensive-only today, but the asymmetry is a footgun for any future
//      direct caller of FUN_004b6770. We close the asymmetry here.
// The alt-path branch (`DAT_007d3e50 & 1` → `FUN_004a4541` / `fseek`+`fread` /
// `fclose`) is preserved, and the return-value contracts match the original
// (1/0 on success/failure for open, void with `*outBytes` populated for read,
// void for close).
//
// Calling-convention note
// -----------------------
// `FUN_004b6710` is a register-passing function: the caller `FUN_004b6940`
// loads the piz path into EAX (`MOV EAX, ESI` at 0x004b69ee) and falls
// directly into `CALL 0x004b6710` (0x004b69f6). The original prologue at
// 0x004b6710 reads ESI implicitly via the `TEST` against `[0x007d3e50]` then
// consumes EAX as the first `PUSH` operand for the CreateFileA call. Because
// the inline-JMP hook installer at `Core/HookSystem.cpp` overwrites only the
// first 5 bytes, the JMP lands in our replacement *before* any EAX is
// clobbered — so the replacement must capture EAX on entry. We use
// `__declspec(naked)` plus a hand-rolled prologue to do this in MSVC.
//
// `FUN_004b67e0` is plain `__cdecl(DWORD off, void* buf, size_t size,
// LPDWORD outBytes)` — no register magic; `SUB ESP, 0x14` + `RET` (no imm16).
//
// No CRT (matches PizOpenBypass.cpp style). All Win32 calls go through
// LoadLibrary-resolved imports cached at first use, mirroring the original's
// IAT pattern at 0x005cc140 / 0x005cc1a4 / etc.
//
// References:
//   - re/analysis/fun_004b6940_scoping/REPORT.md     (charter)
//   - re/analysis/piz_fsmanager_handler/REPORT.md    (structural chain)
//   - PizOpenBypass.cpp                               (sibling bypass)
#include "../Core/HookSystem.h"

#include <windows.h>

namespace {

// Globals shared with the original binary. Addresses cited from
// re/analysis/piz_fsmanager_handler/REPORT.md and Ghidra cross-references in
// FUN_004b6710 / FUN_004b67e0.
constexpr uintptr_t k_PizFlagAddr     = 0x007d3e50; // bit 0 selects alt path
constexpr uintptr_t k_PizHandleAddr   = 0x007d3e48; // current piz HANDLE / FILE*
constexpr uintptr_t k_PizProgressAddr = 0x007d3e5c; // optional progress callback
constexpr uintptr_t k_PizProgressArg  = 0x007d3e60; // 3rd arg to that callback
constexpr uintptr_t k_PizNameAddr     = 0x008ad8a0; // current piz name (1st cb arg)
constexpr uintptr_t k_PizProgressBuf  = 0x008ab6e0; // scratch buffer (2nd cb arg)

// Alt-path entries — only invoked when DAT_007d3e50 & 1.
//   FUN_004a4541: alt opener (return value stored at DAT_007d3e48)
//   _fseek      : CRT seek, called by the alt-read branch in FUN_004b67e0 at 0x004b67fa
//   _fread      : CRT read, called by the alt-read branch in FUN_004b67e0 at 0x004b6812
//   _fclose     : CRT close, called by the alt-close branch in FUN_004b6770 at 0x004b677f
// We invoke them via their original RVA so the alt branch behaves byte-for-byte
// like the unhooked binary if DAT_007d3e50 is ever flipped.
//
// The decompilation shows _fseek/_fread/_fclose are reached through trampolines at
// 0x005c1d63 (fseek), 0x004a49cf (fread), and 0x004a4368 (fclose) — those are
// the actual call targets in FUN_004b67e0 and FUN_004b6770, not the libc names
// directly. We mirror that.
using AltOpenFn  = int  (__cdecl*)(void* arg0, void* arg1);
using FseekFn    = int  (__cdecl*)(void* file, long offset, int origin);
using FreadFn    = unsigned (__cdecl*)(void* buf, unsigned elem_size,
                                       unsigned count, void* file);
using FcloseFn   = int  (__cdecl*)(void* file);
using ProgressFn = void (__cdecl*)(void* name, void* scratch, int param);

constexpr uintptr_t k_AltOpen   = 0x004a4541; // FUN_004a4541 (alt opener)
constexpr uintptr_t k_FseekTramp = 0x005c1d63; // 0x004b67fa CALL target
constexpr uintptr_t k_FreadTramp = 0x004a49cf; // 0x004b6812 CALL target
constexpr uintptr_t k_FcloseTramp = 0x004a4368; // 0x004b677f CALL target

// CreateFileA flags. Original passes 0x60000000 = NO_BUFFERING | OVERLAPPED.
// We drop NO_BUFFERING (0x20000000) and keep only OVERLAPPED (0x40000000).
//   Cited at 0x004b672f in FUN_004b6710: `PUSH 0x60000000`.
constexpr DWORD k_FlagsOriginal  = 0x60000000u;
constexpr DWORD k_FlagsFixed     = FILE_FLAG_OVERLAPPED; // 0x40000000

// Convenience accessors. Pointer arithmetic only — no heap, no CRT.
inline volatile uint8_t&  PizFlag()    {
    return *reinterpret_cast<volatile uint8_t*>(k_PizFlagAddr);
}
inline HANDLE&            PizHandle()  {
    return *reinterpret_cast<HANDLE*>(k_PizHandleAddr);
}
inline ProgressFn         PizProgress(){
    return *reinterpret_cast<ProgressFn*>(k_PizProgressAddr);
}
inline int                PizProgressArg() {
    return *reinterpret_cast<int*>(k_PizProgressArg);
}
inline void*              PizName()    {
    return reinterpret_cast<void*>(k_PizNameAddr);
}
inline void*              PizScratch() {
    return reinterpret_cast<void*>(k_PizProgressBuf);
}

} // namespace

// Forward declarations so the naked thunk below can `CALL` them by name.
extern "C" int __cdecl PizWin32Open_CompatImpl(const char* path);

// ─────────────────────────────────────────────────────────────────────────────
// PizWin32Open_Compat — replacement for FUN_004b6710 (0x004b6710).
//
// Original signature (effective): int(EAX path) — path-pointer passed in EAX
// from caller `FUN_004b6940` at 0x004b69ee (`MOV EAX, ESI`). No stack args.
// Returns 1 on success, 0 on failure (cited at 0x004b675e / 0x004b6764).
//
// We use __declspec(naked) to capture EAX before any compiler-generated
// prologue runs. Body matches the original control flow:
//   - if (DAT_007d3e50 & 1)  →  alt-path via FUN_004a4541
//   - else                   →  CreateFileA with FIXED flags (no NO_BUFFERING)
//   - error-check via GetLastError + handle != INVALID_HANDLE_VALUE
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl
PizWin32Open_Compat(void)
{
    // Naked prologue: stash EAX (= path) into a local so the C body can
    // read it via the named variable. We push EAX, set up a minimal frame,
    // then jump into a regular __cdecl helper.
    __asm {
        push    ecx                 ; preserve caller-save (paranoid; original clobbers these too)
        push    edx
        push    eax                 ; 1 arg: the path
        call    PizWin32Open_CompatImpl
        add     esp, 4              ; cdecl cleanup
        pop     edx
        pop     ecx
        ret
    }
}

// Body of the open replacement. Declared file-static so the naked thunk can
// reach it by name. Not exported — only the naked entry is the hook surface.
extern "C" int __cdecl PizWin32Open_CompatImpl(const char* path)
{
    // Alt-path branch: matches FUN_004b6710 at 0x004b6717..0x004b672c.
    //   if ((DAT_007d3e50 & 1) != 0) {
    //       DAT_007d3e48 = (HANDLE)FUN_004a4541(<EAX>, 0x5cf010);
    //       return DAT_007d3e48;
    //   }
    if ((PizFlag() & 1u) != 0u) {
        auto Alt = reinterpret_cast<AltOpenFn>(k_AltOpen);
        // Original pushes 0x5cf010 then EAX then CALLs 0x004a4541; arg order
        // in the call (right-to-left, cdecl) is (EAX, 0x5cf010).
        void* arg1 = reinterpret_cast<void*>(0x005cf010);
        int rv = Alt(const_cast<char*>(path), arg1);
        PizHandle() = reinterpret_cast<HANDLE>(rv);
        return rv;
    }

    // Main branch: CreateFileA without FILE_FLAG_NO_BUFFERING. All other
    // arguments match the original at 0x004b672d..0x004b6740:
    //   PUSH 0      ; hTemplateFile
    //   PUSH flags  ; dwFlagsAndAttributes (was 0x60000000, now 0x40000000)
    //   PUSH 3      ; dwCreationDisposition = OPEN_EXISTING
    //   PUSH 0      ; lpSecurityAttributes
    //   PUSH 0      ; dwShareMode
    //   PUSH GENERIC_READ (0x80000000)
    //   PUSH EAX    ; lpFileName
    //   CALL [0x005cc140]    ; CreateFileA via IAT
    static_cast<void>(k_FlagsOriginal); // documentary; not used
    HANDLE h = CreateFileA(
        path,
        GENERIC_READ,             // 0x80000000
        0,                         // dwShareMode
        nullptr,                   // lpSecurityAttributes
        OPEN_EXISTING,             // 3
        k_FlagsFixed,              // 0x40000000 (was 0x60000000)
        nullptr);                  // hTemplateFile

    // Mirror the GetLastError + handle check at 0x004b674b..0x004b6764:
    //   GetLastError() == 0 && handle != INVALID_HANDLE_VALUE  →  return 1
    //   otherwise                                              →  return 0
    DWORD err = GetLastError();
    PizHandle() = h;
    if (err == 0 && h != INVALID_HANDLE_VALUE) {
        return 1;
    }
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// PizWin32Read_Compat — replacement for FUN_004b67e0 (0x004b67e0).
//
// Original signature: void __cdecl(DWORD offset, void* buf, size_t size,
//                                   LPDWORD outBytes).
// Four stack args, no register magic; matches the call site at
// 0x004b6a0e..0x004b6a14 in FUN_004b6940 (PUSH EBX/0, EAX, 0x800, EDX; CALL;
// ADD ESP,0x10).
//
// Behaviour matches FUN_004b67e0 1:1 — no buffer/alignment tricks needed
// because we already dropped FILE_FLAG_NO_BUFFERING in the Open hook above.
// The overlapped poll loop is preserved (with its progress-callback dispatch
// and ERROR_HANDLE_EOF / success exit conditions) so any timing or callback
// observers see identical behaviour to the original.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl
PizWin32Read_Compat(DWORD offset, void* buf, size_t size, LPDWORD outBytes)
{
    // Alt-path branch: matches FUN_004b67e0 at 0x004b67ea..0x004b6823.
    //   if ((DAT_007d3e50 & 1) != 0) {
    //       _fseek(DAT_007d3e48, offset, SEEK_SET);
    //       *outBytes = _fread(buf, 1, size, DAT_007d3e48);
    //       return;
    //   }
    if ((PizFlag() & 1u) != 0u) {
        auto Fseek = reinterpret_cast<FseekFn>(k_FseekTramp);
        auto Fread = reinterpret_cast<FreadFn>(k_FreadTramp);
        Fseek(PizHandle(), static_cast<long>(offset), 0 /*SEEK_SET*/);
        unsigned nread = Fread(buf,
                               1u,
                               static_cast<unsigned>(size),
                               PizHandle());
        if (outBytes) *outBytes = static_cast<DWORD>(nread);
        return;
    }

    // Async path: build a fresh OVERLAPPED, kick off the read, then spin
    // on GetOverlappedResult exactly like the original. Cited from the
    // listing at 0x004b6824..0x004b68d0.
    OVERLAPPED ov;
    ov.Internal     = 0;
    ov.InternalHigh = 0;
    ov.Offset       = offset;       // u.s.Offset       at 0x004b683a
    ov.OffsetHigh   = 0;             // u.s.OffsetHigh   at 0x004b6836
    ov.hEvent       = nullptr;       // hEvent           at 0x004b683e (LEA EDX, [ESP+0x10])

    // ReadFile via IAT at 0x005cc1a4 (cited at 0x004b685d). lpNumberOfBytesRead
    // is NULL — the original passes 0 too, mirroring the OVERLAPPED-only API
    // contract. The polling loop below writes *outBytes via
    // GetOverlappedResult.
    ReadFile(PizHandle(), buf, static_cast<DWORD>(size), nullptr, &ov);
    (void)GetLastError(); // 0x004b6863 — return value unused

    // Polling loop. Two exit conditions, both cited at 0x004b68c0..0x004b68c7:
    //   - GetLastError() == ERROR_HANDLE_EOF (0x26)
    //   - GetOverlappedResult() returned BVar == 1 (success)
    // Re-enter loop on ERROR_IO_INCOMPLETE (0x3e4) only (0x004b68b0..0x004b68b7).
    bool finished = false;
    for (;;) {
        // Progress callback. The original tests DAT_007d3e5c each spin and
        // invokes it with (&DAT_008ad8a0, &DAT_008ab6e0, DAT_007d3e60) —
        // cited at 0x004b687e..0x004b688f.
        ProgressFn cb = PizProgress();
        if (cb != nullptr) {
            cb(PizName(), PizScratch(), PizProgressArg());
        }

        BOOL  ok    = GetOverlappedResult(PizHandle(), &ov, outBytes, FALSE);
        DWORD err   = GetLastError();

        if (ok == 0) {
            if (err == 0x3e4u /*ERROR_IO_INCOMPLETE*/) {
                continue; // 0x004b68b7: JMP LAB_004b6873 (loop restart)
            }
        } else if (ok == 1) {
            finished = true; // 0x004b68be: MOV EDI, ESI (sets the success flag)
        }

        // 0x004b68c0..0x004b68c7: terminate on EOF or success flag.
        if (err == 0x26u /*ERROR_HANDLE_EOF*/ || finished) {
            return;
        }
        // Otherwise loop again.
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// PizWin32Close_Compat — replacement for FUN_004b6770 (0x004b6770).
//
// Original signature: void __cdecl(void). No args, no return value.
// 47 bytes total (0x004b6770..0x004b679f).
//
// Original control flow, cited from listing 0x004b6770..0x004b679f:
//   0x004b6770  TEST byte ptr [0x007d3e50], 0x1
//   0x004b6777  JZ   0x004b6792                ; → Win32 branch
//   ; stdio branch (DAT_007d3e50 & 1) != 0
//   0x004b6779  MOV  EAX, [0x007d3e48]
//   0x004b677e  PUSH EAX
//   0x004b677f  CALL 0x004a4368                ; _fclose trampoline
//   0x004b6784  ADD  ESP, 0x4
//   0x004b6787  MOV  dword ptr [0x007d3e48], 0x0  ; <-- nulls handle
//   0x004b6791  RET
//   ; Win32 branch
//   0x004b6792  MOV  ECX, [0x007d3e48]
//   0x004b6798  PUSH ECX
//   0x004b6799  CALL [0x005cc088]              ; CloseHandle via IAT
//   0x004b679f  RET                            ; <-- BUG: does NOT null handle
//
// The stdio branch nulls `DAT_007d3e48` after `_fclose`; the Win32 branch
// does NOT null after `CloseHandle`. Discovered by batch_t s1 retry
// (cluster_004b4_first_pass), filed in
// re/analysis/cluster_004b4_first_pass/004b6770.md as U-42.
//
// In the live binary FUN_004b6770 has exactly one caller — FUN_004b67a0 at
// 0x004b67a0, which then nulls `DAT_007d3e48` explicitly at 0x004b67d1. So
// the bug is latent today. We fix it here defensively so any future direct
// call to PizWin32Close observes a self-consistent function: stdio and Win32
// branches both leave `DAT_007d3e48 == NULL` on return.
//
// We do NOT touch `DAT_007d3e4c` (the "piz is open" flag) — that is set/
// cleared by the caller (`FUN_004b67a0` zeros it at 0x004b67c1; `FUN_004b6940`
// sets it to 1 after a successful open). Mirroring the original's scope.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl
PizWin32Close_Compat(void)
{
    if ((PizFlag() & 1u) != 0u) {
        // stdio branch — matches 0x004b6779..0x004b6791 exactly.
        auto Fclose = reinterpret_cast<FcloseFn>(k_FcloseTramp);
        Fclose(PizHandle());
        PizHandle() = nullptr;
        return;
    }
    // Win32 branch — matches 0x004b6792..0x004b679f, PLUS the missing
    // null-out that the stdio branch already performs.
    HANDLE h = PizHandle();
    if (h != nullptr && h != INVALID_HANDLE_VALUE) {
        CloseHandle(h);
    }
    PizHandle() = nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Hook registration. Patched in by Core/HookSystem.cpp via 5-byte inline JMP
// at the original RVAs when MASHED_RE_NO_AUTO_HOOK is unset.
// ─────────────────────────────────────────────────────────────────────────────
RH_ScopedInstall(PizWin32Open_Compat,  0x004b6710);
RH_ScopedInstall(PizWin32Read_Compat,  0x004b67e0);
RH_ScopedInstall(PizWin32Close_Compat, 0x004b6770);
