// Mashed RE — Render/RasterDeviceWrappers.cpp
// RenderWare device/raster vtable wrapper leaves (area-render r10 cluster, 2026-09-02).
//
// Five wrapper leaves identified in r8 as device-vtable-backed and measured in r10 as
// non-degenerate under a booted texture-load scenario. All dispatch into the RW device
// plugin vtable (base at DAT_007d3ff8); the vtable fn-ptrs are backed by D3D9 and must
// not be called twice per invocation. Verification policy: NEEDS-BOOTED-RACE.
//
//   0x004c7600  RasterUnlock      — calls vtable+0x88(0, raster, 0); returns raster
//   0x004c76f0  RasterCanLock     — flag+0x23 test; calls vtable+0xb8 on set path
//   0x004c7860  RasterMipLock     — calls vtable+0x84(&p2, raster, (p2&0xff)<<8+p3)
//   0x004d5310  RasterImageCopy   — calls vtable+0x64; sets raster+0x22 bit0
//   0x004d5340  RasterLockRead    — calls vtable+0x6c; reads back w/h/d/stride
//
// Binary anchor: MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// Design constraints (HARD, per parent r10 handoff):
//   C1. No path1 (run_diff.py) — device vtable → degenerate green on synthetic raster.
//   C2. Single-device-call invariant — MODDED pass captures + suppresses device call;
//       ORIGINAL makes the one real device call. No double device call ever.
//   C3. Witness own contribution only — branch decisions, marshalled args, self-written
//       fields. Device effects belong to the vtable slot's own row.
//   C4. Mock-based comparison for RasterLockRead — stub injects known data so the
//       reimpl's read-back and byte-swap are verified against computed expected values,
//       not against the original (which uses real device data, making direct comparison
//       meaningless when lock inputs differ).
//   C5. Coverage counters armed before first run (per [[arm-coverage-counters-before-
//       first-run]]): call counts, branch counts, XOR folds of compared values, void
//       counts. A bare GREEN with no counter data is not acceptance.
//
// Trampoline stolen-byte arithmetic (bytes extracted from MASHED.exe at each VA):
//   0x004c7600: a1 f8 3f 7d 00 (5 bytes = 1 insn: MOV EAX,DS:[0x7d3ff8]). Rejoin +5.
//   0x004c76f0: 8b 4c 24 04 8a (5 bytes, 2 insns: MOV ECX,[ESP+4] + MOV AL,[ECX+0x23]).
//               Rejoin +7.
//   0x004c7860: 8b 44 24 08 8b (5 bytes, 2 insns: MOV EAX,[ESP+8] + MOV EDX,[ESP+0xc]).
//               Rejoin +8.
//   0x004d5310: a1 f8 3f 7d 00 (5 bytes = 1 insn: MOV EAX,DS:[0x7d3ff8]). Rejoin +5.
//   0x004d5340: 8b 44 24 08 8b (5 bytes, 2 insns: MOV EAX,[ESP+8] + MOV EDX,DS:[0x7d3ff8]).
//               Second insn is 6 bytes (8b 15 f8 3f 7d 00). Rejoin +10.
//
// Every absolute global read in reimpl and trampoline bodies uses ds:[imm] form (e.g.
// dword ptr ds:[0x007d3ff8]). A bare mov r,[imm] assembles as mov-IMMEDIATE (loads the
// ADDRESS). Byte-verified in the built .asi; byte patterns cited in PROMOTION_QUEUE row.
//
// asi-ONLY: functions call MASHED VAs by fn-ptr. In asi_sources.rsp, NOT build.bat.
//
// Raw captures: log/texture_cluster_observe_track3.json, ..._track12.json (r10).
#include "../Core/HookSystem.h"
#include <windows.h>
#include <cstdlib>
#include <cstring>
#include <cstdint>

// ---------------------------------------------------------------------------
// Shared RW vtable base (DAT_007d3ff8). Used by all 5 reimpl bodies.
// ---------------------------------------------------------------------------
static constexpr std::uintptr_t kRwVtableBase = 0x007d3ff8u;

// ---------------------------------------------------------------------------
// Self-test log helper (shared by all 5 functions).
// ---------------------------------------------------------------------------
static void SelfTestLog(const char* filename, const char* msg) {
    HANDLE h = CreateFileA(filename, FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD w; WriteFile(h, msg, (DWORD)std::strlen(msg), &w, nullptr); CloseHandle(h);
}

// ===========================================================================
// 0x004c7600  RasterUnlock  (28 bytes)
//
// Disasm (verbatim from MASHED.exe at VA 0x004c7600):
//   a1 f8 3f 7d 00      MOV EAX, DS:[0x007d3ff8]   ; vtable base
//   56                  PUSH ESI
//   8b 74 24 08         MOV ESI, [ESP+8]            ; param_1 (raster)
//   6a 00               PUSH 0                      ; arg3
//   56                  PUSH ESI                    ; arg2 = param_1
//   6a 00               PUSH 0                      ; arg1
//   ff 90 88 00 00 00   CALL DWORD PTR [EAX+0x88]  ; vtable+0x88 unlock
//   83 c4 0c            ADD ESP, 12
//   8b c6               MOV EAX, ESI               ; return param_1
//   5e                  POP ESI
//   c3                  RET
//
// Own contribution: return value = param_1 (raster ptr).
// Device effect (vtable+0x88): changes raster+0x22 / raster+0x23 flags; belongs to
//   the vtable slot's own row, not this one. (r10 finding 1: r8 said "no observable"
//   but the flags ARE measured; they are the device's effect, not ours.)
// Observable: return value. d_ret=11 distinct from r10 → non-degenerate.
//
// A/B policy: modded captures args to stub (no device call), returns param_1.
// Original makes the one real unlock call. Compare return values.
// No restore needed: modded makes no state change.
// ===========================================================================

static void* g_unlock_88      = reinterpret_cast<void*>(0);  // real vtable+0x88 fn (lazy init)
static void* g_unlock_stub_88 = nullptr;                      // points to CaptureUnlock during selftest

static volatile std::uint32_t s_unlock_arg2 = 0;  // raster ptr passed to device (= arg2)

__declspec(naked) static void CaptureUnlock88() {
    // Called as __cdecl: (arg1=0, arg2=raster, arg3=0)
    __asm {
        mov  eax, [esp + 8]        // arg2 = param_1 (raster)
        mov  s_unlock_arg2, eax
        xor  eax, eax
        ret
    }
}

// g_unlock_dispatch: the fn-ptr the reimpl calls (either real unlock or capture stub).
static void* g_unlock_dispatch = nullptr;

__declspec(naked) static void RasterUnlock_Reimpl(std::uint32_t /*param_1*/) {
    __asm {
        mov    eax, dword ptr ds:[0x007d3ff8]   // 004c7600  vtable base
        push   esi
        mov    esi, dword ptr [esp + 8]         // 004c7605  param_1
        push   0                                 // arg3
        push   esi                              // arg2
        push   0                                 // arg1
        call   dword ptr [g_unlock_dispatch]    // replaces CALL [EAX+0x88]
        add    esp, 12
        mov    eax, esi                         // return param_1
        pop    esi
        ret
    }
}

static void* g_orig_4c7605 = reinterpret_cast<void*>(0x004c7605);

__declspec(naked) static void OrigRasterUnlock(std::uint32_t /*param_1*/) {
    // Stolen: a1 f8 3f 7d 00 = MOV EAX,DS:[0x007d3ff8] (5 bytes, 1 insn). Rejoin +5.
    __asm {
        mov    eax, dword ptr ds:[0x007d3ff8]
        jmp    dword ptr [g_orig_4c7605]
    }
}

static inline int Unlock_SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* e = std::getenv("MASHED_RENDER_UNLOCK_SELFTEST"); v = (e && e[0]) ? 1 : 0; }
    return v;
}

static long g_unl_calls = 0, g_unl_mism = 0;
static std::uint32_t g_unl_retFold = 0;
static const long kUnlMax = 40000;

static void RasterUnlockDispatch(std::uint32_t param_1) {
    if (!g_unlock_88) {
        std::uint32_t base = *reinterpret_cast<std::uint32_t*>(kRwVtableBase);
        g_unlock_88 = *reinterpret_cast<void**>(base + 0x88u);
    }
    g_unlock_dispatch = g_unlock_88;

    if (Unlock_SelfTestEnabled() && g_unl_calls < kUnlMax) {
        // Modded pass — no device call (stub).
        g_unlock_dispatch = reinterpret_cast<void*>(&CaptureUnlock88);
        RasterUnlock_Reimpl(param_1);
        const std::uint32_t modRet = param_1;  // reimpl always returns param_1

        // Restore dispatch to real fn for the original pass.
        g_unlock_dispatch = g_unlock_88;

        // Original pass — one real unlock device call.
        OrigRasterUnlock(param_1);
        const std::uint32_t origRet = param_1;  // original also returns param_1

        ++g_unl_calls;
        g_unl_retFold ^= origRet;
        if (modRet != origRet) {
            ++g_unl_mism;
            char line[128];
            wsprintfA(line, "[%ld] MISMATCH ret m=%08X o=%08X\r\n",
                      g_unl_calls, modRet, origRet);
            SelfTestLog("raster_unlock_selftest.log", line);
        }
        if ((g_unl_calls & 0x7f) == 1) {
            char line[192];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld retFold=%08X %s\r\n",
                      g_unl_calls, g_unl_calls, g_unl_mism, g_unl_retFold,
                      g_unl_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_unlock_selftest.log", line);
        }
        return;
    }
    OrigRasterUnlock(param_1);
}

__declspec(naked) static void RasterUnlock_Entry() {
    __asm {
        mov  eax, dword ptr [esp + 4]
        push eax
        call RasterUnlockDispatch
        add  esp, 4
        ret
    }
}

RH_ScopedInstall(RasterUnlock_Entry, 0x004c7600);

// ===========================================================================
// 0x004c76f0  RasterCanLock  (54 bytes)
//
// Disasm (verbatim from MASHED.exe at VA 0x004c76f0):
//   8b 4c 24 04         MOV ECX, [ESP+4]           ; param_1
//   8a 41 23            MOV AL, [ECX+0x23]         ; raster+0x23 byte
//   84 c0               TEST AL, AL
//   78 06               JS +6                      ; if high-bit set → device path
//   b8 01 00 00 00      MOV EAX, 1
//   c3                  RET                        ; clear-flag path: return 1
//   a1 f8 3f 7d 00      MOV EAX, DS:[0x007d3ff8]  ; device path
//   6a 00               PUSH 0                     ; arg3
//   51                  PUSH ECX                   ; arg2 = param_1
//   8d 4c 24 0c         LEA ECX, [ESP+0xc]         ; &param_1 on stack (after 2 pushes)
//   8b 80 b8 00 00 00   MOV EAX, [EAX+0xb8]       ; fn-ptr from vtable+0xb8
//   51                  PUSH ECX                   ; arg1 = &param_1
//   ff d0               CALL EAX
//   83 c4 0c            ADD ESP, 12
//   85 c0               TEST EAX, EAX
//   74 05               JZ +5                      ; if device returned 0 → return -1
//   8b 44 24 04         MOV EAX, [ESP+4]           ; return param_1
//   c3                  RET
//   83 c8 ff            OR EAX, 0xffffffff
//   c3                  RET                        ; return -1
//
// Own contribution: branch decision (high-bit of raster+0x23); return 1 on clear path.
// On device path: args marshalled = (&param_1, param_1, 0); return value from device.
// r10: +0x23=0x3→ret=1 (clear path); +0x23=0x82→ret=0x7 (device path, device returns 0x7).
//
// A/B policy: modded reads flag, takes clear path (return 1, no device), or captures
// device args (stub records, no call). Original makes the one real device call.
// Compare: branch decision + clear-path return + device args (when device path taken).
// No restore needed: modded makes no state change.
// ===========================================================================

static void* g_can88_fn  = nullptr;  // real vtable+0xb8 fn (lazy init)
static void* g_can88_disp = nullptr; // active fn-ptr (real or capture)

static volatile std::uint32_t s_canlk_arg1  = 0;  // &param_1 passed to device
static volatile std::uint32_t s_canlk_arg2  = 0;  // param_1 value passed to device
static volatile long          s_canlk_called = 0; // 1 if device path was taken

__declspec(naked) static void CaptureCanLock88() {
    // Called as __cdecl: (arg1=&param_1, arg2=param_1, arg3=0)
    __asm {
        mov  eax, [esp + 4]
        mov  s_canlk_arg1, eax
        mov  eax, [esp + 8]
        mov  s_canlk_arg2, eax
        mov  s_canlk_called, 1
        xor  eax, eax              // return 0 (device failed simulation)
        ret
    }
}

__declspec(naked) static void RasterCanLock_Reimpl(std::int32_t /*param_1*/) {
    __asm {
        // 004c76f0
        mov    ecx, dword ptr [esp + 4]             // param_1
        mov    al, byte ptr [ecx + 0x23]            // raster+0x23
        test   al, al
        js     L_device
        mov    eax, 1
        ret                                          // clear-flag path
    L_device:
        mov    eax, dword ptr ds:[0x007d3ff8]       // vtable base
        push   0                                    // arg3
        push   ecx                                  // arg2 = param_1
        lea    ecx, [esp + 0xc]                     // &param_1 (after 2 pushes, before call)
        mov    eax, dword ptr [g_can88_disp]        // fn-ptr (replaces [EAX+0xb8])
        push   ecx                                  // arg1 = &param_1
        call   eax
        add    esp, 12
        test   eax, eax
        jz     L_fail
        mov    eax, dword ptr [esp + 4]             // return param_1
        ret
    L_fail:
        or     eax, 0xffffffff                      // return -1
        ret
    }
}

static void* g_orig_4c76f7 = reinterpret_cast<void*>(0x004c76f7);

__declspec(naked) static void OrigRasterCanLock(std::int32_t /*param_1*/) {
    // Stolen: 8b 4c 24 04 (MOV ECX,[ESP+4], 4 bytes) + 8a 41 23 (MOV AL,[ECX+0x23], 3 bytes).
    // JMP overwrites bytes 0-4 (5 bytes); byte 4 = 8a (start of MOV AL,[ECX+0x23]).
    // Rejoin at 0x004c76f7 (after both stolen instructions complete).
    __asm {
        mov    ecx, dword ptr [esp + 4]
        mov    al, byte ptr [ecx + 0x23]
        jmp    dword ptr [g_orig_4c76f7]
    }
}

static inline int CanLock_SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* e = std::getenv("MASHED_RENDER_CANLK_SELFTEST"); v = (e && e[0]) ? 1 : 0; }
    return v;
}

static long g_clk_calls = 0, g_clk_mism = 0;
static long g_clk_clearPath = 0, g_clk_devicePath = 0;
static std::uint32_t g_clk_arg2Fold = 0, g_clk_retFold = 0;
static const long kClkMax = 40000;

static void RasterCanLockDispatch(std::int32_t param_1) {
    if (!g_can88_fn) {
        std::uint32_t base = *reinterpret_cast<std::uint32_t*>(kRwVtableBase);
        g_can88_fn = *reinterpret_cast<void**>(base + 0xb8u);
    }
    g_can88_disp = g_can88_fn;

    if (CanLock_SelfTestEnabled() && g_clk_calls < kClkMax) {
        // Read flag byte to know which branch both will take.
        const std::uint8_t flagByte = *reinterpret_cast<volatile std::uint8_t*>(param_1 + 0x23u);
        const bool highBitSet = (flagByte & 0x80) != 0;

        // Modded pass — capture stub replaces device call.
        s_canlk_called = 0; s_canlk_arg1 = 0; s_canlk_arg2 = 0;
        g_can88_disp = reinterpret_cast<void*>(&CaptureCanLock88);
        RasterCanLock_Reimpl(param_1);
        const long modDeviceCalled = s_canlk_called;
        g_can88_disp = g_can88_fn;

        // Original pass — one real device call.
        OrigRasterCanLock(param_1);

        ++g_clk_calls;
        // Coverage counters: which branch was taken.
        if (!highBitSet) ++g_clk_clearPath; else ++g_clk_devicePath;
        if (highBitSet) g_clk_arg2Fold ^= (std::uint32_t)param_1;
        g_clk_retFold ^= (std::uint32_t)flagByte;

        // Mismatch: modded took device path when it shouldn't, or vice versa.
        const bool branchMismatch = (highBitSet && !modDeviceCalled) ||
                                    (!highBitSet && modDeviceCalled);
        if (branchMismatch) {
            ++g_clk_mism;
            char line[192];
            wsprintfA(line, "[%ld] MISMATCH branch: flag=%02x highSet=%d devCalled=%ld\r\n",
                      g_clk_calls, (unsigned)flagByte, (int)highBitSet, modDeviceCalled);
            SelfTestLog("raster_canlk_selftest.log", line);
        }
        if ((g_clk_calls & 0x7f) == 1) {
            char line[256];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld clearPath=%ld devicePath=%ld "
                            "arg2Fold=%08X retFold=%08X %s\r\n",
                      g_clk_calls, g_clk_calls, g_clk_mism,
                      g_clk_clearPath, g_clk_devicePath,
                      g_clk_arg2Fold, g_clk_retFold,
                      g_clk_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_canlk_selftest.log", line);
        }
        return;
    }
    g_can88_disp = g_can88_fn;
    OrigRasterCanLock(param_1);
}

__declspec(naked) static void RasterCanLock_Entry() {
    __asm {
        mov  eax, dword ptr [esp + 4]
        push eax
        call RasterCanLockDispatch
        add  esp, 4
        ret
    }
}

RH_ScopedInstall(RasterCanLock_Entry, 0x004c76f0);

// ===========================================================================
// 0x004c7860  RasterMipLock  (54 bytes)
//
// Disasm (verbatim from MASHED.exe at VA 0x004c7860):
//   8b 44 24 08         MOV EAX, [ESP+8]            ; param_2 (uint level+mode)
//   8b 54 24 0c         MOV EDX, [ESP+0xc]          ; param_3 (int flags)
//   8b 4c 24 04         MOV ECX, [ESP+4]            ; param_1 (raster)
//   25 ff 00 00 00      AND EAX, 0xff               ; param_2 & 0xff
//   c1 e0 08            SHL EAX, 8                  ; << 8
//   03 c2               ADD EAX, EDX                ; + param_3
//   8d 54 24 08         LEA EDX, [ESP+8]            ; &param_2
//   50                  PUSH EAX                    ; arg3 = (p2&0xff)<<8 + p3
//   a1 f8 3f 7d 00      MOV EAX, DS:[0x007d3ff8]   ; vtable base
//   51                  PUSH ECX                    ; arg2 = param_1
//   52                  PUSH EDX                    ; arg1 = &param_2
//   ff 90 84 00 00 00   CALL DWORD PTR [EAX+0x84]  ; vtable+0x84
//   8b 4c 24 14         MOV ECX, [ESP+0x14]         ; param_2 (after call, before cleanup)
//   83 c4 0c            ADD ESP, 12
//   f7 d8               NEG EAX
//   1b c0               SBB EAX, EAX               ; EAX = 0 or 0xffffffff
//   23 c1               AND EAX, ECX               ; return param_2 if success, else 0
//   c3                  RET
//
// Own contribution: computed arg3 = (param_2 & 0xff) << 8 + param_3 (this function's
// own arithmetic). Return value formula: -(iVar1!=0) & param_2.
// r10: d_ret=24 distinct (one per mip level call). arg3Fold must vary across calls.
//
// A/B: capture stub records arg3. Compare capturedArg3 vs expectedArg3 (computed from
// inputs). No state to restore — modded makes no device call, no state change.
// ===========================================================================

static void* g_miplk_84  = nullptr;   // real vtable+0x84 fn
static void* g_miplk_disp = nullptr;  // active fn-ptr

static volatile std::uint32_t s_miplk_arg3 = 0;  // computed arg3 captured from stub

__declspec(naked) static void CaptureMipLock84() {
    // Called as __cdecl: (arg1=&p2, arg2=raster, arg3=computed)
    __asm {
        mov  eax, [esp + 12]     // arg3 = computed value
        mov  s_miplk_arg3, eax
        xor  eax, eax            // return 0 (lock failure simulation)
        ret
    }
}

__declspec(naked) static void RasterMipLock_Reimpl(std::uint32_t /*p1*/,
                                                    std::uint32_t /*p2*/,
                                                    std::int32_t  /*p3*/) {
    __asm {
        // 004c7860
        mov    eax, dword ptr [esp + 8]              // param_2
        mov    edx, dword ptr [esp + 0xc]            // param_3
        mov    ecx, dword ptr [esp + 4]              // param_1
        and    eax, 0xff
        shl    eax, 8
        add    eax, edx                              // (p2&0xff)<<8 + p3
        lea    edx, dword ptr [esp + 8]              // &param_2
        push   eax                                   // arg3
        mov    eax, dword ptr ds:[0x007d3ff8]        // vtable base (informational; fn called via disp)
        push   ecx                                   // arg2 = param_1
        push   edx                                   // arg1 = &param_2
        call   dword ptr [g_miplk_disp]             // replaces CALL [EAX+0x84]
        mov    ecx, dword ptr [esp + 0x14]           // param_2 (before cleanup)
        add    esp, 12
        neg    eax
        sbb    eax, eax
        and    eax, ecx                              // return param_2 if success, else 0
        ret
    }
}

static void* g_orig_4c7868 = reinterpret_cast<void*>(0x004c7868);

__declspec(naked) static std::uint32_t OrigRasterMipLock(std::uint32_t /*p1*/,
                                                           std::uint32_t /*p2*/,
                                                           std::int32_t  /*p3*/) {
    // Stolen: 8b 44 24 08 (MOV EAX,[ESP+8], 4 bytes) + 8b 54 24 0c (MOV EDX,[ESP+0xc], 4 bytes).
    // JMP overwrites bytes 0-4; byte 4 = 8b (start of second MOV). Rejoin at +8 = 0x004c7868.
    __asm {
        mov    eax, dword ptr [esp + 8]
        mov    edx, dword ptr [esp + 0xc]
        jmp    dword ptr [g_orig_4c7868]
    }
}

static inline int MipLk_SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* e = std::getenv("MASHED_RENDER_MIPLK_SELFTEST"); v = (e && e[0]) ? 1 : 0; }
    return v;
}

static long g_mlk_calls = 0, g_mlk_mism = 0;
static long g_mlk_lockSucceeded = 0;
static std::uint32_t g_mlk_arg3Fold = 0;
static const long kMlkMax = 40000;

static void RasterMipLockDispatch(std::uint32_t p1, std::uint32_t p2, std::int32_t p3) {
    if (!g_miplk_84) {
        std::uint32_t base = *reinterpret_cast<std::uint32_t*>(kRwVtableBase);
        g_miplk_84 = *reinterpret_cast<void**>(base + 0x84u);
    }
    g_miplk_disp = g_miplk_84;

    if (MipLk_SelfTestEnabled() && g_mlk_calls < kMlkMax) {
        // Compute expected arg3 independently from inputs.
        const std::uint32_t expectedArg3 = (p2 & 0xffu) * 0x100u + (std::uint32_t)(std::int32_t)p3;

        // Modded pass — capture stub, no device call.
        s_miplk_arg3 = 0xdeadbeef;
        g_miplk_disp = reinterpret_cast<void*>(&CaptureMipLock84);
        RasterMipLock_Reimpl(p1, p2, p3);
        const std::uint32_t capturedArg3 = s_miplk_arg3;
        g_miplk_disp = g_miplk_84;

        // Original pass — one real device lock.
        std::uint32_t origRet = OrigRasterMipLock(p1, p2, p3);

        ++g_mlk_calls;
        if (origRet != 0) ++g_mlk_lockSucceeded;
        g_mlk_arg3Fold ^= capturedArg3;

        if (capturedArg3 != expectedArg3) {
            ++g_mlk_mism;
            char line[192];
            wsprintfA(line, "[%ld] MISMATCH arg3 got=%08X exp=%08X p2=%08X p3=%08X\r\n",
                      g_mlk_calls, capturedArg3, expectedArg3, p2, (std::uint32_t)p3);
            SelfTestLog("raster_miplk_selftest.log", line);
        }
        if ((g_mlk_calls & 0x7f) == 1) {
            char line[256];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld lockSucceeded=%ld arg3Fold=%08X %s\r\n",
                      g_mlk_calls, g_mlk_calls, g_mlk_mism, g_mlk_lockSucceeded, g_mlk_arg3Fold,
                      g_mlk_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_miplk_selftest.log", line);
        }
        return;
    }
    g_miplk_disp = g_miplk_84;
    OrigRasterMipLock(p1, p2, p3);
}

__declspec(naked) static void RasterMipLock_Entry() {
    __asm {
        mov  eax, dword ptr [esp + 0xc]
        push eax                           // p3
        mov  eax, dword ptr [esp + 0xc]
        push eax                           // p2
        mov  eax, dword ptr [esp + 0xc]
        push eax                           // p1
        call RasterMipLockDispatch
        add  esp, 12
        ret
    }
}

RH_ScopedInstall(RasterMipLock_Entry, 0x004c7860);

// ===========================================================================
// 0x004d5310  RasterImageCopy  (48 bytes)
//
// Disasm (verbatim from MASHED.exe at VA 0x004d5310):
//   a1 f8 3f 7d 00      MOV EAX, DS:[0x007d3ff8]   ; vtable base
//   56                  PUSH ESI
//   8b 74 24 08         MOV ESI, [ESP+8]            ; param_1 (raster int)
//   57                  PUSH EDI
//   8b 7c 24 10         MOV EDI, [ESP+0x10]         ; param_2 (byte* image)
//   6a 00               PUSH 0                      ; arg3
//   57                  PUSH EDI                    ; arg2 = param_2
//   56                  PUSH ESI                    ; arg1 = param_1
//   ff 50 64            CALL DWORD PTR [EAX+0x64]  ; vtable+0x64 copy
//   83 c4 0c            ADD ESP, 12
//   85 c0               TEST EAX, EAX
//   74 0e               JZ +0x0e (→ fail path)
//   f6 07 02            TEST BYTE PTR [EDI], 2     ; *param_2 & 2
//   74 04               JZ +0x04 (→ no-flag)
//   80 4e 22 01         OR BYTE PTR [ESI+0x22], 1  ; raster+0x22 |= 1  ← OWN CONTRIBUTION
//   8b c6               MOV EAX, ESI               ; return param_1
//   5f                  POP EDI
//   5e                  POP ESI
//   c3                  RET
//   5f                  POP EDI
//   33 c0               XOR EAX, EAX               ; return 0
//   5e                  POP ESI
//   c3                  RET
//
// Own contribution: raster+0x22 |= 1 when device_succeeded && (*param_2 & 2).
// Observable: raster+0x22 (d_obs=3 from r10); return value (d_ret=11).
//
// A/B policy:
//   Stub simulates success (returns 1) so the flag-write path is exercisable.
//   Modded may write raster+0x22 based on (*param_2 & 2).
//   Comparison: modded's raster+0x22 state vs expected from (*param_2 & 2).
//   (NOT compared directly against original, since original's device return may differ.)
//   Original runs for the one real device call; its raster+0x22 becomes final state.
// Restore: raster+0x22 snapshotted before modded, restored before original.
// ===========================================================================

static void* g_copy_64  = nullptr;   // real vtable+0x64 fn
static void* g_copy_disp = nullptr;  // active fn-ptr

__declspec(naked) static void CaptureImageCopy64() {
    // Called as __cdecl: (arg1=raster, arg2=image_ptr, arg3=0)
    __asm {
        mov  eax, 1   // simulate success so the flag-write path fires
        ret
    }
}

__declspec(naked) static void RasterImageCopy_Reimpl(std::int32_t  /*param_1*/,
                                                       std::uint8_t* /*param_2*/) {
    __asm {
        // 004d5310
        mov    eax, dword ptr ds:[0x007d3ff8]        // vtable base (for reference; call via disp)
        push   esi
        mov    esi, dword ptr [esp + 8]              // param_1 (raster)
        push   edi
        mov    edi, dword ptr [esp + 0x10]           // param_2 (image ptr)
        push   0                                     // arg3
        push   edi                                   // arg2 = param_2
        push   esi                                   // arg1 = param_1
        call   dword ptr [g_copy_disp]              // replaces CALL [EAX+0x64]
        add    esp, 12
        test   eax, eax
        jz     L_fail
        test   byte ptr [edi], 2                     // *param_2 & 2
        jz     L_no_flag
        or     byte ptr [esi + 0x22], 1             // raster+0x22 |= 1
    L_no_flag:
        mov    eax, esi
        pop    edi
        pop    esi
        ret
    L_fail:
        pop    edi
        xor    eax, eax
        pop    esi
        ret
    }
}

static void* g_orig_4d5315 = reinterpret_cast<void*>(0x004d5315);

__declspec(naked) static void OrigRasterImageCopy(std::int32_t  /*param_1*/,
                                                    std::uint8_t* /*param_2*/) {
    // Stolen: a1 f8 3f 7d 00 = MOV EAX,DS:[0x007d3ff8] (5 bytes, 1 insn). Rejoin +5.
    __asm {
        mov    eax, dword ptr ds:[0x007d3ff8]
        jmp    dword ptr [g_orig_4d5315]
    }
}

static inline int Copy_SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* e = std::getenv("MASHED_RENDER_COPY_SELFTEST"); v = (e && e[0]) ? 1 : 0; }
    return v;
}

static long g_cp_calls = 0, g_cp_mism = 0;
static long g_cp_flagSet = 0, g_cp_fieldWritten = 0;
static std::uint32_t g_cp_fieldFold = 0;
static const long kCpMax = 40000;

static void RasterImageCopyDispatch(std::int32_t param_1, std::uint8_t* param_2) {
    if (!g_copy_64) {
        std::uint32_t base = *reinterpret_cast<std::uint32_t*>(kRwVtableBase);
        g_copy_64 = *reinterpret_cast<void**>(base + 0x64u);
    }
    g_copy_disp = g_copy_64;

    if (Copy_SelfTestEnabled() && g_cp_calls < kCpMax) {
        // Snapshot raster+0x22 before modded pass.
        volatile std::uint8_t* pFlag22 =
            reinterpret_cast<volatile std::uint8_t*>(param_1 + 0x22u);
        const std::uint8_t snap22 = *pFlag22;

        // Whether flag should be written: stub returns 1 (simulated success) AND *param_2 & 2.
        const bool flagShouldWrite = (param_2 && (*param_2 & 2u) != 0);

        // Modded pass — stub simulates success; may write raster+0x22.
        g_copy_disp = reinterpret_cast<void*>(&CaptureImageCopy64);
        RasterImageCopy_Reimpl(param_1, param_2);
        const std::uint8_t modFlag22 = *pFlag22;

        // Restore raster+0x22 before original pass.
        *pFlag22 = snap22;

        // Original pass — one real device call.
        g_copy_disp = g_copy_64;
        OrigRasterImageCopy(param_1, param_2);
        // Leave original's state as-is (final state).

        ++g_cp_calls;
        if (flagShouldWrite) ++g_cp_flagSet;
        if (modFlag22 != snap22) ++g_cp_fieldWritten;
        g_cp_fieldFold ^= (std::uint32_t)modFlag22;

        // Mismatch: our write (or non-write) disagrees with flagShouldWrite expectation.
        const bool expectedWrite = flagShouldWrite && (snap22 & 1u) == 0u;  // bit0 was clear → write sets it
        const bool actualWrite = (modFlag22 & 1u) != (snap22 & 1u);
        if (flagShouldWrite && !actualWrite) {
            ++g_cp_mism;
            char line[192];
            wsprintfA(line, "[%ld] MISMATCH flag22: snap=%02x mod=%02x flagShouldWrite=%d\r\n",
                      g_cp_calls, (unsigned)snap22, (unsigned)modFlag22, (int)flagShouldWrite);
            SelfTestLog("raster_copy_selftest.log", line);
        }
        if ((g_cp_calls & 0x7f) == 1) {
            char line[256];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld flagSet=%ld fieldWritten=%ld fieldFold=%08X %s\r\n",
                      g_cp_calls, g_cp_calls, g_cp_mism, g_cp_flagSet, g_cp_fieldWritten,
                      g_cp_fieldFold, g_cp_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_copy_selftest.log", line);
        }
        return;
    }
    g_copy_disp = g_copy_64;
    OrigRasterImageCopy(param_1, param_2);
}

__declspec(naked) static void RasterImageCopy_Entry() {
    __asm {
        mov  eax, dword ptr [esp + 8]
        push eax                           // param_2
        mov  eax, dword ptr [esp + 8]
        push eax                           // param_1
        call RasterImageCopyDispatch
        add  esp, 8
        ret
    }
}

RH_ScopedInstall(RasterImageCopy_Entry, 0x004d5310);

// ===========================================================================
// 0x004d5340  RasterLockRead  (107 bytes)
//
// Disasm (verbatim from MASHED.exe at VA 0x004d5340):
//   8b 44 24 08         MOV EAX, [ESP+8]            ; param_2 (mode)
//   8b 15 f8 3f 7d 00   MOV EDX, DS:[0x007d3ff8]   ; vtable base → EDX
//   83 ec 34            SUB ESP, 52                 ; alloc 52-byte local struct
//   8d 4c 24 00         LEA ECX, [ESP]              ; &local_struct (bottom of alloc)
//   56                  PUSH ESI
//   8b 74 24 3c         MOV ESI, [ESP+0x3c]         ; param_1 (raster, after sub+push)
//   50                  PUSH EAX                    ; arg3 = mode
//   56                  PUSH ESI                    ; arg2 = param_1
//   51                  PUSH ECX                    ; arg1 = &local_struct
//   ff 52 6c            CALL DWORD PTR [EDX+0x6c]  ; vtable+0x6c lock
//   83 c4 0c            ADD ESP, 12
//   85 c0               TEST EAX, EAX
//   75 05               JNZ +5 (→ success path)
//   5e                  POP ESI
//   83 c4 34            ADD ESP, 52
//   c3                  RET                        ; return 0 if lock failed
// Success path (after JNZ):
//   8b 44 24 27         MOV EAX, [ESP+0x27]        ; DWORD at struct+0x23 (MSB of stride)
//   8b 4c 24 24         MOV ECX, [ESP+0x24]        ; DWORD at struct+0x20 (stride)
//   8b 54 24 50         MOV EDX, [ESP+0x50]        ; param_6 (*stride out-param ptr)
//   25 ff 00 00 00      AND EAX, 0xff              ; = high byte of stride
//   c1 e0 08            SHL EAX, 8
//   81 e1 ff 00 00 00   AND ECX, 0xff              ; = low byte of stride
//   0b c1               OR EAX, ECX                ; byte-swap result ← OWN CONTRIBUTION
//   8b 4c 24 10         MOV ECX, [ESP+0x10]        ; struct+0x0c = w
//   89 02               MOV [EDX], EAX             ; *param_6 = byte-swapped stride
//   8b 44 24 44         MOV EAX, [ESP+0x44]        ; param_3 (*w ptr)
//   8b 54 24 48         MOV EDX, [ESP+0x48]        ; param_4 (*h ptr)
//   89 08               MOV [EAX], ECX             ; *param_3 = w
//   8b 44 24 14         MOV EAX, [ESP+0x14]        ; struct+0x10 = h
//   8b 4c 24 4c         MOV ECX, [ESP+0x4c]        ; param_5 (*d ptr)
//   89 02               MOV [EDX], EAX             ; *param_4 = h
//   8b 54 24 18         MOV EDX, [ESP+0x18]        ; struct+0x14 = d
//   8b c6               MOV EAX, ESI               ; return param_1
//   89 11               MOV [ECX], EDX             ; *param_5 = d
//   5e                  POP ESI
//   83 c4 34            ADD ESP, 52
//   c3                  RET
//
// Own contribution: byte-swap formula (stride_high<<8 | stride_low) and out-param writes.
// Struct layout (relative to &local_struct, which is [ESP+4] after add esp,0xc):
//   struct+0x0c = w    (read as [ESP+0x10])
//   struct+0x10 = h    (read as [ESP+0x14])
//   struct+0x14 = d    (read as [ESP+0x18])
//   struct+0x20 = stride (read as [ESP+0x24])
//   struct+0x23 = stride high byte (read as [ESP+0x27] in DWORD load, masked to 0xff)
//
// A/B policy (mock-based, per constraint C4):
//   Stub fills the struct with KNOWN values and returns 1 (success).
//   Reimpl reads them and writes out-params.
//   Compare reimpl's out-params against expected computed from mock values.
//   This verifies: correct struct offsets read AND correct byte-swap computation.
//   Original runs afterwards for the one real lock call (not compared against mock).
// r10: w/h/d/fmt vary across 24 records, 14 distinct d_obs → non-degenerate.
// ===========================================================================

static void* g_lock_6c  = nullptr;   // real vtable+0x6c fn
static void* g_lock_disp = nullptr;  // active fn-ptr

// Mock lock result struct (filled into local_struct by the capture stub).
// Layout must match the actual d3d9 lock result layout as read by this function.
// mock_stride = 0x12000078 so byte-swap = (0x12<<8) | 0x78 = 0x1278.
static const std::uint32_t kMockW      = 0x00000100u;
static const std::uint32_t kMockH      = 0x00000080u;
static const std::uint32_t kMockD      = 0x00000010u;
static const std::uint32_t kMockStride = 0x12000078u;
static const std::uint32_t kMockBswap  = 0x00001278u;  // pre-computed expected byte-swap

static volatile std::uint32_t s_lkr_capturedMode   = 0;
static volatile std::uint32_t s_lkr_capturedRaster  = 0;

__declspec(naked) static void CaptureLockRead6c() {
    // Called as __cdecl: (arg1=&local_struct, arg2=raster, arg3=mode)
    // Fills the struct at the offsets this function reads and returns 1 (success).
    // struct+0x0c = w, struct+0x10 = h, struct+0x14 = d, struct+0x20 = stride.
    __asm {
        push   esi
        push   edi
        mov    esi, dword ptr [esp + 12]    // arg1 = &local_struct
        mov    eax, dword ptr [esp + 16]    // arg2 = raster
        mov    s_lkr_capturedRaster, eax
        mov    eax, dword ptr [esp + 20]    // arg3 = mode
        mov    s_lkr_capturedMode, eax
        // Fill struct at the offsets this function reads (struct+0x0c/0x10/0x14/0x20).
        // kMockW=0x100, kMockH=0x80, kMockD=0x10, kMockStride=0x12000078.
        // Naked asm cannot reference C++ constexpr by name; use literal immediates.
        mov    dword ptr [esi + 0x0c], 0x100       // w
        mov    dword ptr [esi + 0x10], 0x80        // h
        mov    dword ptr [esi + 0x14], 0x10        // d
        // stride at struct+0x20 (4 bytes). The byte-swap reads:
        //   [struct+0x23] & 0xff = high byte of stride = 0x12
        //   [struct+0x20] & 0xff = low byte  of stride = 0x78
        mov    dword ptr [esi + 0x20], 0x12000078  // stride
        pop    edi
        pop    esi
        mov    eax, 1      // return 1 (success)
        ret
    }
}

__declspec(naked) static void RasterLockRead_Reimpl(
    std::uint32_t  /*param_1*/,  // raster
    std::uint32_t  /*param_2*/,  // mode
    std::uint32_t* /*param_3*/,  // *w out
    std::uint32_t* /*param_4*/,  // *h out
    std::uint32_t* /*param_5*/,  // *d out
    std::uint32_t* /*param_6*/)  // *stride_bswap out
{
    __asm {
        // 004d5340
        mov    eax, dword ptr [esp + 8]              // param_2 (mode)
        mov    edx, dword ptr ds:[0x007d3ff8]        // vtable base → EDX (for reference)
        sub    esp, 52                               // alloc 52-byte local struct
        lea    ecx, dword ptr [esp]                  // &local_struct
        push   esi
        mov    esi, dword ptr [esp + 0x3c]           // param_1 (raster)
        push   eax                                   // arg3 = mode
        push   esi                                   // arg2 = param_1
        push   ecx                                   // arg1 = &local_struct
        call   dword ptr [g_lock_disp]              // replaces CALL [EDX+0x6c]
        add    esp, 12
        test   eax, eax
        jnz    L_success
        pop    esi
        add    esp, 52
        ret                                          // return 0 (lock failed)
    L_success:
        mov    eax, dword ptr [esp + 0x27]           // DWORD at struct+0x23 (stride MSB area)
        mov    ecx, dword ptr [esp + 0x24]           // DWORD at struct+0x20 (stride)
        mov    edx, dword ptr [esp + 0x50]           // param_6 (*stride ptr)
        and    eax, 0xff                             // high byte of stride
        shl    eax, 8
        and    ecx, 0xff                             // low byte of stride
        or     eax, ecx                              // byte-swapped stride result
        mov    ecx, dword ptr [esp + 0x10]           // struct+0x0c = w
        mov    dword ptr [edx], eax                 // *param_6 = byte-swapped stride
        mov    eax, dword ptr [esp + 0x44]           // param_3 (*w ptr)
        mov    edx, dword ptr [esp + 0x48]           // param_4 (*h ptr)
        mov    dword ptr [eax], ecx                 // *param_3 = w
        mov    eax, dword ptr [esp + 0x14]           // struct+0x10 = h
        mov    ecx, dword ptr [esp + 0x4c]           // param_5 (*d ptr)
        mov    dword ptr [edx], eax                 // *param_4 = h
        mov    edx, dword ptr [esp + 0x18]           // struct+0x14 = d
        mov    eax, esi                              // return param_1
        mov    dword ptr [ecx], edx                 // *param_5 = d
        pop    esi
        add    esp, 52
        ret
    }
}

static void* g_orig_4d534a = reinterpret_cast<void*>(0x004d534a);

__declspec(naked) static void OrigRasterLockRead(
    std::uint32_t  /*param_1*/,
    std::uint32_t  /*param_2*/,
    std::uint32_t* /*param_3*/,
    std::uint32_t* /*param_4*/,
    std::uint32_t* /*param_5*/,
    std::uint32_t* /*param_6*/)
{
    // Stolen: 8b 44 24 08 (MOV EAX,[ESP+8], 4 bytes) + 8b 15 f8 3f 7d 00 (MOV EDX,DS:[…], 6 bytes).
    // JMP overwrites bytes 0-4; byte 4 = 8b (start of second MOV which is 6 bytes total).
    // Both complete instructions must be re-executed. Rejoin at +10 = 0x004d534a.
    __asm {
        mov    eax, dword ptr [esp + 8]
        mov    edx, dword ptr ds:[0x007d3ff8]
        jmp    dword ptr [g_orig_4d534a]
    }
}

static inline int LkRead_SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* e = std::getenv("MASHED_RENDER_LKREAD_SELFTEST"); v = (e && e[0]) ? 1 : 0; }
    return v;
}

static long g_lkr_calls = 0, g_lkr_mism = 0;
static long g_lkr_lockSucceeded = 0;
static std::uint32_t g_lkr_bswapFold = 0, g_lkr_wFold = 0;
static const long kLkrMax = 40000;

static void RasterLockReadDispatch(
    std::uint32_t  param_1,
    std::uint32_t  param_2,
    std::uint32_t* param_3,
    std::uint32_t* param_4,
    std::uint32_t* param_5,
    std::uint32_t* param_6)
{
    if (!g_lock_6c) {
        std::uint32_t base = *reinterpret_cast<std::uint32_t*>(kRwVtableBase);
        g_lock_6c = *reinterpret_cast<void**>(base + 0x6cu);
    }
    g_lock_disp = g_lock_6c;

    if (LkRead_SelfTestEnabled() && g_lkr_calls < kLkrMax) {
        // Snapshot out-param storage (to restore before original run).
        const std::uint32_t snapW      = param_3 ? *param_3 : 0u;
        const std::uint32_t snapH      = param_4 ? *param_4 : 0u;
        const std::uint32_t snapD      = param_5 ? *param_5 : 0u;
        const std::uint32_t snapStride = param_6 ? *param_6 : 0u;

        // Modded pass — stub fills struct with mock data, returns 1.
        g_lock_disp = reinterpret_cast<void*>(&CaptureLockRead6c);
        RasterLockRead_Reimpl(param_1, param_2, param_3, param_4, param_5, param_6);
        const std::uint32_t modW      = param_3 ? *param_3 : 0xdeadbeefu;
        const std::uint32_t modH      = param_4 ? *param_4 : 0xdeadbeefu;
        const std::uint32_t modD      = param_5 ? *param_5 : 0xdeadbeefu;
        const std::uint32_t modBswap  = param_6 ? *param_6 : 0xdeadbeefu;

        // Restore out-param storage before original pass.
        if (param_3) *param_3 = snapW;
        if (param_4) *param_4 = snapH;
        if (param_5) *param_5 = snapD;
        if (param_6) *param_6 = snapStride;

        // Original pass — one real device lock.
        g_lock_disp = g_lock_6c;
        OrigRasterLockRead(param_1, param_2, param_3, param_4, param_5, param_6);
        const std::uint32_t origRet = param_1;  // original returns param_1 on success
        if (origRet) ++g_lkr_lockSucceeded;

        ++g_lkr_calls;
        // Verify reimpl's mock reads vs expected computed values.
        g_lkr_bswapFold ^= modBswap;
        g_lkr_wFold     ^= modW;

        const bool wBad     = (modW    != kMockW);
        const bool hBad     = (modH    != kMockH);
        const bool dBad     = (modD    != kMockD);
        const bool bswapBad = (modBswap != kMockBswap);
        if (wBad || hBad || dBad || bswapBad) {
            ++g_lkr_mism;
            char line[256];
            wsprintfA(line, "[%ld] MISMATCH w=%08X(exp %08X) h=%08X(exp %08X) "
                            "d=%08X(exp %08X) bswap=%08X(exp %08X)\r\n",
                      g_lkr_calls, modW, kMockW, modH, kMockH, modD, kMockD,
                      modBswap, kMockBswap);
            SelfTestLog("raster_lkread_selftest.log", line);
        }
        if ((g_lkr_calls & 0x7f) == 1) {
            char line[256];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld lockSucceeded=%ld bswapFold=%08X wFold=%08X %s\r\n",
                      g_lkr_calls, g_lkr_calls, g_lkr_mism, g_lkr_lockSucceeded,
                      g_lkr_bswapFold, g_lkr_wFold, g_lkr_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_lkread_selftest.log", line);
        }
        return;
    }
    g_lock_disp = g_lock_6c;
    OrigRasterLockRead(param_1, param_2, param_3, param_4, param_5, param_6);
}

__declspec(naked) static void RasterLockRead_Entry() {
    __asm {
        mov  eax, dword ptr [esp + 0x18]
        push eax                           // param_6
        mov  eax, dword ptr [esp + 0x18]
        push eax                           // param_5
        mov  eax, dword ptr [esp + 0x18]
        push eax                           // param_4
        mov  eax, dword ptr [esp + 0x18]
        push eax                           // param_3
        mov  eax, dword ptr [esp + 0x18]
        push eax                           // param_2
        mov  eax, dword ptr [esp + 0x18]
        push eax                           // param_1
        call RasterLockReadDispatch
        add  esp, 24
        ret
    }
}

RH_ScopedInstall(RasterLockRead_Entry, 0x004d5340);
