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
//   C4. RasterLockRead uses ORIGINAL-FIRST real A/B (parent review 2026-09-02):
//       run original first to capture real out-params; restore; configure dynamic stub
//       with reconstructed stride; run modded; compare modded vs original out-params.
//       Mock-only (circular) approach REFUSED by parent — see note below.
//   C5. Coverage counters armed before first run (per [[arm-coverage-counters-before-
//       first-run]]): call counts, branch counts, XOR folds, void counts. A bare GREEN
//       with no counter data is not acceptance.
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
// v2 corrections (parent booted-race run, 2026-09-02):
//   - All dispatch functions now return the same type as their originals (void dispatches
//     caused callers to receive wrong return values — root cause of CanLock under-firing
//     where the caller gates further calls on the return value).
//   - RasterUnlock: observable changed from retFold to arg verification (capturedArg1==0,
//     capturedArg2==param_1, capturedArg3==0) + post-flag fold (non-degenerate measure).
//   - RasterImageCopy: mismatch condition fixed to compare against expectedPost22=(snap|1)
//     when flagShouldWrite; alreadySet counter added for idempotent-write calls;
//     void counter added for incomplete-restore detection.
//   - RasterLockRead: redesigned to original-first real A/B. Dynamic stub seeded with
//     captured original out-params. Void counter for restore validation.
//   - RasterMipLock: code unchanged; queue note updated to state oracle-based.
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
    DWORD written; WriteFile(h, msg, (DWORD)std::strlen(msg), &written, nullptr); CloseHandle(h);
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
// Own contribution: args marshalled to device = (0, param_1, 0); return = param_1.
// Observable (v2): verify capturedArg1==0, capturedArg2==param_1, capturedArg3==0.
//   retFold-on-return alone was too weak (it just folds the INPUT pointer).
//   Post-call flag fold (XOR of +0x22|+0x23<<8 after original) gives non-degenerate
//   measure that the device call actually ran and changed state.
// No restore needed: modded makes no state change.
// ===========================================================================

static void* g_unlock_88   = nullptr;  // real vtable+0x88 fn (lazy init)
static void* g_unlock_disp = nullptr;  // active fn-ptr (real or capture stub)

static volatile std::uint32_t s_unlock_arg1 = 0xdeadbeef;
static volatile std::uint32_t s_unlock_arg2 = 0xdeadbeef;
static volatile std::uint32_t s_unlock_arg3 = 0xdeadbeef;

__declspec(naked) static void CaptureUnlock88() {
    // Called as __cdecl: (arg1=0, arg2=raster, arg3=0)
    __asm {
        mov  eax, [esp + 4]
        mov  s_unlock_arg1, eax
        mov  eax, [esp + 8]
        mov  s_unlock_arg2, eax
        mov  eax, [esp + 12]
        mov  s_unlock_arg3, eax
        xor  eax, eax
        ret
    }
}

__declspec(naked) static void RasterUnlock_Reimpl(std::uint32_t /*param_1*/) {
    __asm {
        mov    eax, dword ptr ds:[0x007d3ff8]   // 004c7600  vtable base
        push   esi
        mov    esi, dword ptr [esp + 8]         // 004c7605  param_1
        push   0                                 // arg3
        push   esi                              // arg2
        push   0                                 // arg1
        call   dword ptr [g_unlock_disp]        // replaces CALL [EAX+0x88]
        add    esp, 12
        mov    eax, esi                         // return param_1
        pop    esi
        ret
    }
}

static void* g_orig_4c7605 = reinterpret_cast<void*>(0x004c7605);

__declspec(naked) static std::uint32_t OrigRasterUnlock(std::uint32_t /*param_1*/) {
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
static std::uint32_t g_unl_flagFold = 0;  // XOR of post-call (flag22 | flag23<<8) from original
static const long kUnlMax = 40000;

static std::uint32_t RasterUnlockDispatch(std::uint32_t param_1) {
    if (!g_unlock_88) {
        std::uint32_t base = *reinterpret_cast<std::uint32_t*>(kRwVtableBase);
        g_unlock_88 = *reinterpret_cast<void**>(base + 0x88u);
    }
    if (Unlock_SelfTestEnabled() && g_unl_calls < kUnlMax) {
        // Modded pass — capture stub (no device call).
        s_unlock_arg1 = 0xdeadbeef; s_unlock_arg2 = 0xdeadbeef; s_unlock_arg3 = 0xdeadbeef;
        g_unlock_disp = reinterpret_cast<void*>(&CaptureUnlock88);
        RasterUnlock_Reimpl(param_1);

        // Original pass — one real unlock device call.
        g_unlock_disp = g_unlock_88;
        std::uint32_t origRet = OrigRasterUnlock(param_1);

        // Post-call flag bytes (device effect — non-degenerate measure that device ran).
        const std::uint8_t post22 = *reinterpret_cast<volatile std::uint8_t*>(param_1 + 0x22u);
        const std::uint8_t post23 = *reinterpret_cast<volatile std::uint8_t*>(param_1 + 0x23u);
        g_unl_flagFold ^= (std::uint32_t)post22 | ((std::uint32_t)post23 << 8);

        ++g_unl_calls;
        // Mismatch: args marshalled incorrectly.
        const bool argBad = (s_unlock_arg1 != 0u ||
                             s_unlock_arg2 != param_1 ||
                             s_unlock_arg3 != 0u);
        if (argBad) {
            ++g_unl_mism;
            char line[192];
            wsprintfA(line, "[%ld] MISMATCH args: a1=%08X(exp 0) a2=%08X(exp %08X) a3=%08X(exp 0)\r\n",
                      g_unl_calls, s_unlock_arg1, s_unlock_arg2, param_1, s_unlock_arg3);
            SelfTestLog("raster_unlock_selftest.log", line);
        }
        if ((g_unl_calls & 0x7f) == 1) {
            char line[256];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld flagFold=%08X %s\r\n",
                      g_unl_calls, g_unl_calls, g_unl_mism, g_unl_flagFold,
                      g_unl_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_unlock_selftest.log", line);
        }
        return origRet;
    }
    g_unlock_disp = g_unlock_88;
    return OrigRasterUnlock(param_1);
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
//   8d 4c 24 0c         LEA ECX, [ESP+0xc]         ; &param_1 on stack
//   8b 80 b8 00 00 00   MOV EAX, [EAX+0xb8]       ; fn-ptr from vtable+0xb8
//   51                  PUSH ECX                   ; arg1 = &param_1
//   ff d0               CALL EAX
//   83 c4 0c            ADD ESP, 12
//   85 c0               TEST EAX, EAX
//   74 05               JZ +5
//   8b 44 24 04         MOV EAX, [ESP+4]           ; return param_1 if device succeeded
//   c3                  RET
//   83 c8 ff            OR EAX, 0xffffffff
//   c3                  RET                        ; return -1 if device failed
//
// Own contribution: branch decision (high-bit of raster+0x23); return 1 on clear path.
// On device path: args marshalled = (&param_1, param_1, 0); return from device.
// r10: +0x23=0x3→ret=1 (clear path, d_obs measured); +0x23=0x82→ret=0x7 (device path).
//
// v2 NOTE: dispatch was void (bug). Caller gates further calls on return value, so
//   returning garbage after the first call caused only 1 of 197-251 calls to compare.
//   Fixed: dispatch now returns int.
// ===========================================================================

static void* g_can88_fn   = nullptr;   // real vtable+0xb8 fn (lazy init)
static void* g_can88_disp = nullptr;   // active fn-ptr

static volatile std::uint32_t s_canlk_arg1   = 0;  // &param_1 passed to device
static volatile std::uint32_t s_canlk_arg2   = 0;  // param_1 value passed to device
static volatile long          s_canlk_called = 0;  // 1 if device path was taken

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
        mov    ecx, dword ptr [esp + 4]
        mov    al, byte ptr [ecx + 0x23]
        test   al, al
        js     L_device
        mov    eax, 1
        ret
    L_device:
        mov    eax, dword ptr ds:[0x007d3ff8]
        push   0
        push   ecx
        lea    ecx, [esp + 0xc]
        mov    eax, dword ptr [g_can88_disp]
        push   ecx
        call   eax
        add    esp, 12
        test   eax, eax
        jz     L_fail
        mov    eax, dword ptr [esp + 4]
        ret
    L_fail:
        or     eax, 0xffffffff
        ret
    }
}

static void* g_orig_4c76f7 = reinterpret_cast<void*>(0x004c76f7);

__declspec(naked) static std::int32_t OrigRasterCanLock(std::int32_t /*param_1*/) {
    // Stolen: 8b 4c 24 04 (MOV ECX,[ESP+4], 4B) + 8a 41 23 (MOV AL,[ECX+0x23], 3B).
    // Rejoin at 0x004c76f7 (+7).
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
static std::uint32_t g_clk_arg2Fold = 0, g_clk_flagFold = 0;
static const long kClkMax = 40000;

static std::int32_t RasterCanLockDispatch(std::int32_t param_1) {
    if (!g_can88_fn) {
        std::uint32_t base = *reinterpret_cast<std::uint32_t*>(kRwVtableBase);
        g_can88_fn = *reinterpret_cast<void**>(base + 0xb8u);
    }
    if (CanLock_SelfTestEnabled() && g_clk_calls < kClkMax) {
        const std::uint8_t flagByte = *reinterpret_cast<volatile std::uint8_t*>(param_1 + 0x23u);
        const bool highBitSet = (flagByte & 0x80u) != 0;

        // Modded pass — capture stub.
        s_canlk_called = 0; s_canlk_arg1 = 0; s_canlk_arg2 = 0;
        g_can88_disp = reinterpret_cast<void*>(&CaptureCanLock88);
        RasterCanLock_Reimpl(param_1);
        const long modDeviceCalled = s_canlk_called;

        // Original pass — one real device call.
        g_can88_disp = g_can88_fn;
        std::int32_t origRet = OrigRasterCanLock(param_1);

        ++g_clk_calls;
        if (!highBitSet) ++g_clk_clearPath; else ++g_clk_devicePath;
        if (highBitSet) g_clk_arg2Fold ^= (std::uint32_t)param_1;
        g_clk_flagFold ^= (std::uint32_t)flagByte;

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
                            "arg2Fold=%08X flagFold=%08X %s\r\n",
                      g_clk_calls, g_clk_calls, g_clk_mism,
                      g_clk_clearPath, g_clk_devicePath,
                      g_clk_arg2Fold, g_clk_flagFold,
                      g_clk_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_canlk_selftest.log", line);
        }
        return origRet;
    }
    g_can88_disp = g_can88_fn;
    return OrigRasterCanLock(param_1);
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
//   8b 44 24 08         MOV EAX, [ESP+8]            ; param_2
//   8b 54 24 0c         MOV EDX, [ESP+0xc]          ; param_3
//   8b 4c 24 04         MOV ECX, [ESP+4]            ; param_1
//   25 ff 00 00 00      AND EAX, 0xff
//   c1 e0 08            SHL EAX, 8
//   03 c2               ADD EAX, EDX                ; arg3 = (p2&0xff)<<8 + p3
//   8d 54 24 08         LEA EDX, [ESP+8]            ; &param_2
//   50                  PUSH EAX                    ; arg3
//   a1 f8 3f 7d 00      MOV EAX, DS:[0x007d3ff8]
//   51                  PUSH ECX                    ; arg2 = param_1
//   52                  PUSH EDX                    ; arg1 = &param_2
//   ff 90 84 00 00 00   CALL DWORD PTR [EAX+0x84]
//   8b 4c 24 14         MOV ECX, [ESP+0x14]         ; param_2 (after call, before cleanup)
//   83 c4 0c            ADD ESP, 12
//   f7 d8               NEG EAX
//   1b c0               SBB EAX, EAX
//   23 c1               AND EAX, ECX               ; return param_2 if success, else 0
//   c3                  RET
//
// Own contribution (oracle-based): arg3 = (param_2 & 0xff) << 8 + param_3. This is our
//   own arithmetic; we verify it by comparing capturedArg3 (from stub) against
//   expectedArg3 (computed inline from the same inputs). This is an oracle — the oracle
//   encodes our reading of how arg3 is formed (same shape as slider2 pattern; accepted
//   by parent as long as it is stated as such). Return formula: -(iVar1!=0) & param_2.
// r10: d_ret=24 distinct, lockSucceeded observed, arg3Fold varies → non-degenerate.
// ===========================================================================

static void* g_miplk_84   = nullptr;
static void* g_miplk_disp = nullptr;

static volatile std::uint32_t s_miplk_arg3 = 0;

__declspec(naked) static void CaptureMipLock84() {
    // Called as __cdecl: (arg1=&p2, arg2=raster, arg3=computed)
    __asm {
        mov  eax, [esp + 12]     // arg3 = computed value
        mov  s_miplk_arg3, eax
        xor  eax, eax
        ret
    }
}

__declspec(naked) static void RasterMipLock_Reimpl(std::uint32_t /*p1*/,
                                                    std::uint32_t /*p2*/,
                                                    std::int32_t  /*p3*/) {
    __asm {
        // 004c7860
        mov    eax, dword ptr [esp + 8]
        mov    edx, dword ptr [esp + 0xc]
        mov    ecx, dword ptr [esp + 4]
        and    eax, 0xff
        shl    eax, 8
        add    eax, edx
        lea    edx, dword ptr [esp + 8]
        push   eax
        mov    eax, dword ptr ds:[0x007d3ff8]
        push   ecx
        push   edx
        call   dword ptr [g_miplk_disp]
        mov    ecx, dword ptr [esp + 0x14]
        add    esp, 12
        neg    eax
        sbb    eax, eax
        and    eax, ecx
        ret
    }
}

static void* g_orig_4c7868 = reinterpret_cast<void*>(0x004c7868);

__declspec(naked) static std::uint32_t OrigRasterMipLock(std::uint32_t /*p1*/,
                                                           std::uint32_t /*p2*/,
                                                           std::int32_t  /*p3*/) {
    // Stolen: 8b 44 24 08 (4B) + 8b 54 24 0c (4B). Rejoin +8 = 0x004c7868.
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

static std::uint32_t RasterMipLockDispatch(std::uint32_t p1, std::uint32_t p2, std::int32_t p3) {
    if (!g_miplk_84) {
        std::uint32_t base = *reinterpret_cast<std::uint32_t*>(kRwVtableBase);
        g_miplk_84 = *reinterpret_cast<void**>(base + 0x84u);
    }
    if (MipLk_SelfTestEnabled() && g_mlk_calls < kMlkMax) {
        const std::uint32_t expectedArg3 = (p2 & 0xffu) * 0x100u + (std::uint32_t)(std::int32_t)p3;

        s_miplk_arg3 = 0xdeadbeef;
        g_miplk_disp = reinterpret_cast<void*>(&CaptureMipLock84);
        RasterMipLock_Reimpl(p1, p2, p3);
        const std::uint32_t capturedArg3 = s_miplk_arg3;

        g_miplk_disp = g_miplk_84;
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
                      g_mlk_calls, g_mlk_calls, g_mlk_mism, g_mlk_lockSucceeded,
                      g_mlk_arg3Fold, g_mlk_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_miplk_selftest.log", line);
        }
        return origRet;
    }
    g_miplk_disp = g_miplk_84;
    return OrigRasterMipLock(p1, p2, p3);
}

__declspec(naked) static void RasterMipLock_Entry() {
    __asm {
        mov  eax, dword ptr [esp + 0xc]
        push eax
        mov  eax, dword ptr [esp + 0xc]
        push eax
        mov  eax, dword ptr [esp + 0xc]
        push eax
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
//   a1 f8 3f 7d 00      MOV EAX, DS:[0x007d3ff8]
//   56                  PUSH ESI
//   8b 74 24 08         MOV ESI, [ESP+8]            ; param_1 (raster)
//   57                  PUSH EDI
//   8b 7c 24 10         MOV EDI, [ESP+0x10]         ; param_2 (byte* image)
//   6a 00               PUSH 0
//   57                  PUSH EDI
//   56                  PUSH ESI
//   ff 50 64            CALL DWORD PTR [EAX+0x64]  ; vtable+0x64
//   83 c4 0c            ADD ESP, 12
//   85 c0               TEST EAX, EAX
//   74 0e               JZ +0x0e
//   f6 07 02            TEST BYTE PTR [EDI], 2     ; *param_2 & 2
//   74 04               JZ +0x04
//   80 4e 22 01         OR BYTE PTR [ESI+0x22], 1  ; raster+0x22 |= 1  OWN CONTRIBUTION
//   8b c6               MOV EAX, ESI               ; return param_1
//   5f                  POP EDI
//   5e                  POP ESI
//   c3                  RET
//   5f                  POP EDI
//   33 c0               XOR EAX, EAX
//   5e                  POP ESI
//   c3                  RET
//
// Own contribution: raster+0x22 |= 1 when device_succeeded && (*param_2 & 2).
// Observable (v2 fix):
//   Stub simulates success (returns 1). expectedPost22 = flagShouldWrite ? (snap22|1) : snap22.
//   Compare modded's raster+0x22 against expectedPost22 (not against "did it move").
//   Old mismatch condition "flagShouldWrite && !moved" was wrong for the idempotent case
//   where snap22 already has bit0 set — OR into an already-set bit is correct and
//   unobservable, but the old code called it a mismatch. (Parent: "FALSE MISMATCH,
//   harness defect, not a port defect" — snap=01 mod=01 flagShouldWrite=1.)
//   New counters: alreadySet (calls where flagShouldWrite && snap bit0 already 1) and
//   void (calls where restore failed, i.e., *pFlag22 after restore != snap22).
// ===========================================================================

static void* g_copy_64   = nullptr;
static void* g_copy_disp = nullptr;

__declspec(naked) static void CaptureImageCopy64() {
    __asm {
        mov  eax, 1   // simulate success
        ret
    }
}

__declspec(naked) static void RasterImageCopy_Reimpl(std::int32_t  /*param_1*/,
                                                       std::uint8_t* /*param_2*/) {
    __asm {
        // 004d5310
        mov    eax, dword ptr ds:[0x007d3ff8]
        push   esi
        mov    esi, dword ptr [esp + 8]
        push   edi
        mov    edi, dword ptr [esp + 0x10]
        push   0
        push   edi
        push   esi
        call   dword ptr [g_copy_disp]
        add    esp, 12
        test   eax, eax
        jz     L_fail
        test   byte ptr [edi], 2
        jz     L_no_flag
        or     byte ptr [esi + 0x22], 1
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

__declspec(naked) static std::int32_t OrigRasterImageCopy(std::int32_t  /*param_1*/,
                                                            std::uint8_t* /*param_2*/) {
    // Stolen: a1 f8 3f 7d 00 = MOV EAX,DS:[0x007d3ff8] (5B, 1 insn). Rejoin +5.
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
static long g_cp_flagSet = 0, g_cp_alreadySet = 0, g_cp_void = 0;
static std::uint32_t g_cp_postFold = 0;
static const long kCpMax = 40000;

static std::int32_t RasterImageCopyDispatch(std::int32_t param_1, std::uint8_t* param_2) {
    if (!g_copy_64) {
        std::uint32_t base = *reinterpret_cast<std::uint32_t*>(kRwVtableBase);
        g_copy_64 = *reinterpret_cast<void**>(base + 0x64u);
    }
    if (Copy_SelfTestEnabled() && g_cp_calls < kCpMax) {
        volatile std::uint8_t* pFlag22 =
            reinterpret_cast<volatile std::uint8_t*>(param_1 + 0x22u);
        const std::uint8_t snap22 = *pFlag22;
        const bool flagShouldWrite = (param_2 && (*param_2 & 2u) != 0);
        // Expected post-value after modded (stub returns 1 = success):
        const std::uint8_t expectedPost22 = flagShouldWrite ? (snap22 | 1u) : snap22;

        // Modded pass — stub simulates success; may write raster+0x22.
        g_copy_disp = reinterpret_cast<void*>(&CaptureImageCopy64);
        RasterImageCopy_Reimpl(param_1, param_2);
        const std::uint8_t modFlag22 = *pFlag22;

        // Restore raster+0x22 before original pass.
        *pFlag22 = snap22;
        const bool restoreOk = (*pFlag22 == snap22);  // void detection

        // Original pass — one real device call.
        g_copy_disp = g_copy_64;
        std::int32_t origRet = OrigRasterImageCopy(param_1, param_2);
        // Leave original's state as-is (final state).

        ++g_cp_calls;
        if (flagShouldWrite) ++g_cp_flagSet;
        if (flagShouldWrite && (snap22 & 1u)) ++g_cp_alreadySet;
        if (!restoreOk) ++g_cp_void;
        g_cp_postFold ^= (std::uint32_t)modFlag22;

        // Mismatch: modded's post-flag != expected.
        if (modFlag22 != expectedPost22) {
            ++g_cp_mism;
            char line[256];
            wsprintfA(line, "[%ld] MISMATCH flag22: snap=%02x mod=%02x exp=%02x "
                            "flagShouldWrite=%d alreadySet=%ld\r\n",
                      g_cp_calls, (unsigned)snap22, (unsigned)modFlag22,
                      (unsigned)expectedPost22, (int)flagShouldWrite, g_cp_alreadySet);
            SelfTestLog("raster_copy_selftest.log", line);
        }
        if ((g_cp_calls & 0x7f) == 1) {
            char line[256];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld flagSet=%ld alreadySet=%ld "
                            "void=%ld postFold=%08X %s\r\n",
                      g_cp_calls, g_cp_calls, g_cp_mism, g_cp_flagSet, g_cp_alreadySet,
                      g_cp_void, g_cp_postFold, g_cp_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_copy_selftest.log", line);
        }
        return origRet;
    }
    g_copy_disp = g_copy_64;
    return OrigRasterImageCopy(param_1, param_2);
}

__declspec(naked) static void RasterImageCopy_Entry() {
    __asm {
        mov  eax, dword ptr [esp + 8]
        push eax
        mov  eax, dword ptr [esp + 8]
        push eax
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
//   8b 15 f8 3f 7d 00   MOV EDX, DS:[0x007d3ff8]   ; vtable base
//   83 ec 34            SUB ESP, 52
//   8d 4c 24 00         LEA ECX, [ESP]              ; &local_struct
//   56                  PUSH ESI
//   8b 74 24 3c         MOV ESI, [ESP+0x3c]         ; param_1 (raster)
//   50                  PUSH EAX                    ; arg3 = mode
//   56                  PUSH ESI                    ; arg2 = param_1
//   51                  PUSH ECX                    ; arg1 = &local_struct
//   ff 52 6c            CALL DWORD PTR [EDX+0x6c]  ; vtable+0x6c
//   83 c4 0c            ADD ESP, 12
//   85 c0               TEST EAX, EAX
//   75 05               JNZ +5
//   5e                  POP ESI
//   83 c4 34            ADD ESP, 52
//   c3                  RET
//   8b 44 24 27         MOV EAX, [ESP+0x27]        ; DWORD at struct+0x23 (stride MSB area)
//   8b 4c 24 24         MOV ECX, [ESP+0x24]        ; DWORD at struct+0x20 (stride)
//   8b 54 24 50         MOV EDX, [ESP+0x50]        ; param_6
//   25 ff 00 00 00      AND EAX, 0xff              ; high byte of stride
//   c1 e0 08            SHL EAX, 8
//   81 e1 ff 00 00 00   AND ECX, 0xff              ; low byte of stride
//   0b c1               OR EAX, ECX                ; byte-swap result  OWN CONTRIBUTION
//   8b 4c 24 10         MOV ECX, [ESP+0x10]        ; struct+0x0c = w
//   89 02               MOV [EDX], EAX             ; *param_6 = bswap
//   8b 44 24 44         MOV EAX, [ESP+0x44]        ; param_3
//   8b 54 24 48         MOV EDX, [ESP+0x48]        ; param_4
//   89 08               MOV [EAX], ECX             ; *param_3 = w
//   8b 44 24 14         MOV EAX, [ESP+0x14]        ; struct+0x10 = h
//   8b 4c 24 4c         MOV ECX, [ESP+0x4c]        ; param_5
//   89 02               MOV [EDX], EAX             ; *param_4 = h
//   8b 54 24 18         MOV EDX, [ESP+0x18]        ; struct+0x14 = d
//   8b c6               MOV EAX, ESI              ; return param_1
//   89 11               MOV [ECX], EDX             ; *param_5 = d
//   5e                  POP ESI
//   83 c4 34            ADD ESP, 52
//   c3                  RET
//
// Own contribution: byte-swap formula (stride_high<<8 | stride_low) and out-param writes.
// Struct layout (relative to local_struct start):
//   struct+0x0c = w, struct+0x10 = h, struct+0x14 = d, struct+0x20 = stride,
//   struct+0x23 = stride MSB (read as DWORD at [ESP+0x27], masked to 0xff).
//
// v2 REDESIGN — ORIGINAL-FIRST real A/B (parent refused mock-only as circular):
//   Old design filled the struct with hand-coded kMock* values and compared modded output
//   against hand-computed kMockBswap. If the byte-swap was mis-read, both the port and
//   the expected were wrong together → circular. Parent also noted that the struct fields
//   (w/h/d) are STABLE PER RASTER (not volatile per lock call), making real A/B viable.
//
//   New design (original-first):
//   1. Snapshot *p3..*p6 (snap).
//   2. Run original (one real device lock). Capture origW/origH/origD/origBswap.
//   3. Restore *p3..*p6 = snap. Void counter: check restore complete.
//   4. Configure dynamic stub with orig-derived mock values:
//      g_lkr_mockW/H/D = origW/H/D; g_lkr_mockStride = reconstructed from origBswap
//      such that bswap(reconstructed) = origBswap.
//   5. Run modded (dynamic stub fills struct, reimpl reads it, writes *p3..*p6).
//   6. Compare modded's *p3..*p6 against origW/origH/origD/origBswap.
//   This validates correct struct-offset reads AND correct byte-swap computation
//   against the REAL values produced by the device (not hand-coded expectations).
//   Void count: calls where restore failed (field != snap before modded run).
// r10: w/h/d/fmt vary across 24 records (0x100/0x80/0x10/0x304 and 0x20/0x20/0x10/0x204).
// ===========================================================================

static void* g_lock_6c   = nullptr;
static void* g_lock_disp = nullptr;

// Dynamic stub mock values (set by dispatch from captured original out-params).
static std::uint32_t g_lkr_mockW      = 0;
static std::uint32_t g_lkr_mockH      = 0;
static std::uint32_t g_lkr_mockD      = 0;
static std::uint32_t g_lkr_mockStride = 0;  // reconstructed so bswap(it) = origBswap

static volatile std::uint32_t s_lkr_capturedRaster = 0;
static volatile std::uint32_t s_lkr_capturedMode   = 0;

// Non-naked stub: fills struct at the specific offsets this function reads, then returns 1.
static int __cdecl CaptureLockRead6c_Dynamic(void* structPtr, std::uint32_t raster,
                                               std::uint32_t mode) {
    std::uint8_t* s = reinterpret_cast<std::uint8_t*>(structPtr);
    *reinterpret_cast<std::uint32_t*>(s + 0x0c) = g_lkr_mockW;
    *reinterpret_cast<std::uint32_t*>(s + 0x10) = g_lkr_mockH;
    *reinterpret_cast<std::uint32_t*>(s + 0x14) = g_lkr_mockD;
    *reinterpret_cast<std::uint32_t*>(s + 0x20) = g_lkr_mockStride;
    s_lkr_capturedRaster = raster;
    s_lkr_capturedMode   = mode;
    return 1;  // simulate success so the success path runs
}

__declspec(naked) static void RasterLockRead_Reimpl(
    std::uint32_t  /*param_1*/,
    std::uint32_t  /*param_2*/,
    std::uint32_t* /*param_3*/,
    std::uint32_t* /*param_4*/,
    std::uint32_t* /*param_5*/,
    std::uint32_t* /*param_6*/)
{
    __asm {
        // 004d5340
        mov    eax, dword ptr [esp + 8]
        mov    edx, dword ptr ds:[0x007d3ff8]
        sub    esp, 52
        lea    ecx, dword ptr [esp]
        push   esi
        mov    esi, dword ptr [esp + 0x3c]
        push   eax
        push   esi
        push   ecx
        call   dword ptr [g_lock_disp]
        add    esp, 12
        test   eax, eax
        jnz    L_success
        pop    esi
        add    esp, 52
        ret
    L_success:
        mov    eax, dword ptr [esp + 0x27]
        mov    ecx, dword ptr [esp + 0x24]
        mov    edx, dword ptr [esp + 0x50]
        and    eax, 0xff
        shl    eax, 8
        and    ecx, 0xff
        or     eax, ecx
        mov    ecx, dword ptr [esp + 0x10]
        mov    dword ptr [edx], eax
        mov    eax, dword ptr [esp + 0x44]
        mov    edx, dword ptr [esp + 0x48]
        mov    dword ptr [eax], ecx
        mov    eax, dword ptr [esp + 0x14]
        mov    ecx, dword ptr [esp + 0x4c]
        mov    dword ptr [edx], eax
        mov    edx, dword ptr [esp + 0x18]
        mov    eax, esi
        mov    dword ptr [ecx], edx
        pop    esi
        add    esp, 52
        ret
    }
}

static void* g_orig_4d534a = reinterpret_cast<void*>(0x004d534a);

__declspec(naked) static std::uint32_t OrigRasterLockRead(
    std::uint32_t  /*param_1*/,
    std::uint32_t  /*param_2*/,
    std::uint32_t* /*param_3*/,
    std::uint32_t* /*param_4*/,
    std::uint32_t* /*param_5*/,
    std::uint32_t* /*param_6*/)
{
    // Stolen: 8b 44 24 08 (4B) + 8b 15 f8 3f 7d 00 (6B). Rejoin +10 = 0x004d534a.
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

static long g_lkr_calls = 0, g_lkr_mism = 0, g_lkr_void = 0;
static long g_lkr_lockSucceeded = 0;
static std::uint32_t g_lkr_bswapFold = 0, g_lkr_wFold = 0;
static const long kLkrMax = 40000;

static std::uint32_t RasterLockReadDispatch(
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
    if (LkRead_SelfTestEnabled() && g_lkr_calls < kLkrMax) {
        // Snapshot out-param storage.
        const std::uint32_t snapW = param_3 ? *param_3 : 0u;
        const std::uint32_t snapH = param_4 ? *param_4 : 0u;
        const std::uint32_t snapD = param_5 ? *param_5 : 0u;
        const std::uint32_t snapB = param_6 ? *param_6 : 0u;

        // === ORIGINAL FIRST (one real device lock) ===
        g_lock_disp = g_lock_6c;
        std::uint32_t origRet = OrigRasterLockRead(param_1, param_2, param_3, param_4,
                                                     param_5, param_6);
        const std::uint32_t origW     = param_3 ? *param_3 : 0xdeadbeefu;
        const std::uint32_t origH     = param_4 ? *param_4 : 0xdeadbeefu;
        const std::uint32_t origD     = param_5 ? *param_5 : 0xdeadbeefu;
        const std::uint32_t origBswap = param_6 ? *param_6 : 0xdeadbeefu;
        if (origRet) ++g_lkr_lockSucceeded;

        // Restore *p3..*p6 before modded pass. Void detection: check restore complete.
        if (param_3) *param_3 = snapW;
        if (param_4) *param_4 = snapH;
        if (param_5) *param_5 = snapD;
        if (param_6) *param_6 = snapB;
        const bool restoreOk = (!param_3 || *param_3 == snapW) &&
                               (!param_4 || *param_4 == snapH) &&
                               (!param_5 || *param_5 == snapD) &&
                               (!param_6 || *param_6 == snapB);
        if (!restoreOk) {
            ++g_lkr_void;
            // Still run original's state as the final state (already done above). Count void.
        }

        // Configure dynamic stub from original out-params.
        // Reconstruct mockStride such that bswap(mockStride) = origBswap:
        //   bswap = (stride_byte3 << 8) | stride_byte0
        //   → stride_byte3 = (origBswap >> 8) & 0xff
        //   → stride_byte0 = origBswap & 0xff
        //   → mockStride = (stride_byte3 << 24) | stride_byte0
        g_lkr_mockW      = origW;
        g_lkr_mockH      = origH;
        g_lkr_mockD      = origD;
        g_lkr_mockStride = (((origBswap & 0xff00u) << 16u) | (origBswap & 0xffu));

        // === MODDED (dynamic stub fills struct, no device call) ===
        g_lock_disp = reinterpret_cast<void*>(&CaptureLockRead6c_Dynamic);
        RasterLockRead_Reimpl(param_1, param_2, param_3, param_4, param_5, param_6);
        const std::uint32_t modW     = param_3 ? *param_3 : 0xdeadbeefu;
        const std::uint32_t modH     = param_4 ? *param_4 : 0xdeadbeefu;
        const std::uint32_t modD     = param_5 ? *param_5 : 0xdeadbeefu;
        const std::uint32_t modBswap = param_6 ? *param_6 : 0xdeadbeefu;

        ++g_lkr_calls;
        g_lkr_bswapFold ^= origBswap;
        g_lkr_wFold     ^= origW;

        const bool wBad     = (modW    != origW);
        const bool hBad     = (modH    != origH);
        const bool dBad     = (modD    != origD);
        const bool bswapBad = (modBswap != origBswap);
        if (wBad || hBad || dBad || bswapBad || !restoreOk) {
            if (wBad || hBad || dBad || bswapBad) ++g_lkr_mism;
            char line[256];
            wsprintfA(line, "[%ld] %s w=%08X(o%08X) h=%08X(o%08X) "
                            "d=%08X(o%08X) bswap=%08X(o%08X) void=%ld\r\n",
                      g_lkr_calls, (wBad||hBad||dBad||bswapBad) ? "MISMATCH" : "VOID",
                      modW, origW, modH, origH, modD, origD, modBswap, origBswap,
                      g_lkr_void);
            SelfTestLog("raster_lkread_selftest.log", line);
        }
        if ((g_lkr_calls & 0x7f) == 1) {
            char line[256];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld void=%ld lockSucc=%ld "
                            "bswapFold=%08X wFold=%08X %s\r\n",
                      g_lkr_calls, g_lkr_calls, g_lkr_mism, g_lkr_void,
                      g_lkr_lockSucceeded, g_lkr_bswapFold, g_lkr_wFold,
                      g_lkr_mism ? "" : "ALL-GREEN");
            SelfTestLog("raster_lkread_selftest.log", line);
        }
        // Leave modded's out-param state (modded ran last; original already ran for device effects).
        return origRet;
    }
    g_lock_disp = g_lock_6c;
    return OrigRasterLockRead(param_1, param_2, param_3, param_4, param_5, param_6);
}

__declspec(naked) static void RasterLockRead_Entry() {
    __asm {
        mov  eax, dword ptr [esp + 0x18]
        push eax
        mov  eax, dword ptr [esp + 0x18]
        push eax
        mov  eax, dword ptr [esp + 0x18]
        push eax
        mov  eax, dword ptr [esp + 0x18]
        push eax
        mov  eax, dword ptr [esp + 0x18]
        push eax
        mov  eax, dword ptr [esp + 0x18]
        push eax
        call RasterLockReadDispatch
        add  esp, 24
        ret
    }
}

RH_ScopedInstall(RasterLockRead_Entry, 0x004d5340);
