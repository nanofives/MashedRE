// Mashed RE — c3_batch_t session 6: skeleton_prep + scattered frontend cluster.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Candidates in this file (10 attempted; 3 GREEN-promoted, 4 impl-only/deferred,
// 1 RED live-state-blocked, 2 harness-incompatible deferred, 1 refused):
//   0x004274d0  LangIndexSeedFromCli     — GREEN C3 (read_global; 0/10 mismatch)
//   0x00428390  FrontendStateSet         — GREEN C3 (void_setter_observe; 0/10)
//   0x0042fa00  PlayerSlotEdgeAdjust     — GREEN C3 (none; 0/10 mismatch)
//   0x004274e0  LocalizationFileLoad     — impl-only; live-PIZ open non-deterministic
//                                          under Frida sandbox; no safe arg_type.
//   0x0042bcb0  PlayerNameSpriteDraw     — impl-only; 5-arg complex frame, no arg_type.
//   0x00431f30  FrontendPageFlagSet      — impl-only; calls mass-clear (destructive
//                                          to 19 frontend flags) — unsafe to A/B.
//   0x00403050  PreRaceLoadingScreenDraw — RED: original AVs at quiescent menu
//                                          (DAT_00771964 non-zero stale handle →
//                                          live-state wall); impl matches decomp.
//
// DEFERRED candidates (not implemented):
//   0x004a2b60  vsprintf-style wrapper — variadic; cannot be exposed via Frida
//               NativeFunction (no varargs in mscdecl harness signature). Filed.
//   0x00497b60  player-config dialog populater — DialogProc-style HWND param;
//               1134B win32 dialog code with LoadStringA/LoadBitmapA/SetWindowTextA;
//               not callable from Frida harness (no dialog context at quiescent menu).
//
// REFUSED:
//   0x00423040  BLOCKED per c3_batch_s-s1: callees FUN_00417450 (C1) and
//               FUN_00417530 (C1) — anti-island rule. Verified 2026-05-26.
//
// Callee gates (verified 2026-05-26 against hooks.csv):
//   FUN_004cc230  C2  (PIZ stream open)
//   FUN_004cbd30  C3  (stream read)
//   FUN_004cc160  C3  (stream close)
//   FUN_004c5c00  C3  (linked-list string search)
//   FUN_004b5750  C2  (Im2D quad draw via vtable)
//   FUN_00431d90  C2  (panel-flag mass-clear)
//   FUN_00402fb0  C2  (pulsing sprite spinner)
//   FUN_00428760  C2  (background quad draw)

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// Forward declarations for callees (originals; we call into MASHED.exe RVAs).
// ---------------------------------------------------------------------------

// 0x004cc230  RwStreamOpen-equivalent: (type, mode, name) -> handle (uint32*)
//   type: 1=file 2=PIZ 3=mem 4=cb;  mode: 1=read 2=write 3=append
static auto* const s_FUN_004cc230 = reinterpret_cast<
    std::uint32_t*(__cdecl*)(int, int, const void*)>(0x004cc230);

// 0x004cbd30  RwStreamRead-equivalent: (handle, buf, size) -> bytes_read
static auto* const s_FUN_004cbd30 = reinterpret_cast<
    std::uint32_t(__cdecl*)(std::uint32_t*, void*, std::uint32_t)>(0x004cbd30);

// 0x004cc160  RwStreamClose-equivalent: (handle, hint)
static auto* const s_FUN_004cc160 = reinterpret_cast<
    void(__cdecl*)(std::uint32_t*, std::uint32_t)>(0x004cc160);

// 0x004c5c00  Linked-list string search: (collection, name) -> entry*
static auto* const s_FUN_004c5c00 = reinterpret_cast<
    std::uint32_t*(__cdecl*)(std::uint32_t, const void*)>(0x004c5c00);

// 0x004b5750  Im2D quad draw via vtable: (driver, &rect, &xfm, &param4, handle)
static auto* const s_FUN_004b5750 = reinterpret_cast<
    void(__cdecl*)(std::uint32_t, const float*, const float*, const void*, std::uint32_t)>(0x004b5750);

// 0x00431d90  Panel-flag mass-clear (no args).
static auto* const s_FUN_00431d90 = reinterpret_cast<void(__cdecl*)()>(0x00431d90);

// 0x00402fb0  Pulsing sprite spinner (no args).
static auto* const s_FUN_00402fb0 = reinterpret_cast<void(__cdecl*)()>(0x00402fb0);

// 0x00428760  Background textured-quad: (handle, x1, y1, x2, y2, mode)
//   Args 2..5 passed as 32-bit IEEE-float-bit-patterns (from original).
static auto* const s_FUN_00428760 = reinterpret_cast<
    void(__cdecl*)(std::uint32_t, float, float, float, float, int)>(0x00428760);

// ───────────────────────────────────────────────────────────────────────────
// 0x004274d0  LangIndexSeedFromCli
//
// Original: FUN_004274d0 (15 bytes, 0x004274d0..0x004274de)
// Signature: uint32_t FUN_004274d0(void)
//
// Body (pure leaf, no calls):
//   MOV EAX, [0x007719e8]                  // @ 0x004274d2
//   MOV [0x007f0f60], EAX                  // @ 0x004274d8
//   MOV EAX, 1
//   RET
//
// Reads:  DAT_007719e8 (cli-language-index)  [0x004274d2]
// Writes: DAT_007f0f60 (language selector)   [0x004274d8]
// Returns: 1 (always)                        [decomp constant]
// Pure leaf — no callees.
//
// ref: re/analysis/skeleton_prep_game_state/004274d0.md
// ───────────────────────────────────────────────────────────────────────────
// 0x004274d0
extern "C" __declspec(dllexport) std::uint32_t __cdecl LangIndexSeedFromCli()
{
    // Read CLI-set language index. [0x004274d2]
    std::uint32_t v = *reinterpret_cast<const std::uint32_t*>(0x007719e8u);
    // Write to language selector global. [0x004274d8]
    *reinterpret_cast<std::uint32_t*>(0x007f0f60u) = v;
    // Always returns 1. [decomp constant]
    return 1u;
}
RH_ScopedInstall(LangIndexSeedFromCli, 0x004274d0);

// ───────────────────────────────────────────────────────────────────────────
// 0x004274e0  LocalizationFileLoad
//
// Original: FUN_004274e0 (123 bytes, 0x004274e0..0x0042755a)
// Signature: undefined4 FUN_004274e0(void)
//
// Body:
//   Switches on DAT_007f0f60 (language selector):
//     0 -> "english.dat"  string at 0x005cd5e4  [0x004274f2]
//     1 -> "french.dat"   string at 0x005cd5d8  [0x004274f9]
//     2 -> "German.dat"   string at 0x005cd5cc  [0x00427500]
//     3 -> "Spanish.dat"  string at 0x005cd5c0  [0x00427507]
//     4 -> "Italian.dat"  string at 0x005cd5b4  [0x0042750e]
//     5 -> "USA.dat"      (decomp string; xref unconfirmed; U-0869)
//     default -> return 0
//   On matched case:
//     handle = FUN_004cc230(2, 1, name)             // PIZ open type=2 mode=1
//     if (handle != 0) {
//       FUN_004cbd30(handle, &DAT_0066d828, 0x10000)
//       FUN_004cc160(handle, 0)
//       DAT_0067d84c = 1
//       return 1
//     }
//     return 0
//
// Constants cited:
//   PIZ open type = 2                  [arg1 to FUN_004cc230]
//   PIZ open mode = 1 (read)           [arg2 to FUN_004cc230]
//   read size = 0x10000 (65536)        [size arg]
//   localized-string buffer base = 0x0066d828
//   success flag dest = 0x0067d84c (writes 1)
//
// ref: re/analysis/skeleton_prep_game_state/004274e0.md
// ───────────────────────────────────────────────────────────────────────────
// 0x004274e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl LocalizationFileLoad()
{
    // String literals at fixed RVAs in MASHED.exe data section.
    // [0x004274f2..0x0042750e]
    const char* name = nullptr;
    std::int32_t lang = *reinterpret_cast<const std::int32_t*>(0x007f0f60u);
    switch (lang) {
        case 0: name = reinterpret_cast<const char*>(0x005cd5e4u); break;  // "english.dat"
        case 1: name = reinterpret_cast<const char*>(0x005cd5d8u); break;  // "french.dat"
        case 2: name = reinterpret_cast<const char*>(0x005cd5ccu); break;  // "German.dat"
        case 3: name = reinterpret_cast<const char*>(0x005cd5c0u); break;  // "Spanish.dat"
        case 4: name = reinterpret_cast<const char*>(0x005cd5b4u); break;  // "Italian.dat"
        // case 5: "USA.dat" — U-0869 (xref unconfirmed); preserved if present in
        //   original via fall-through to default. Original decomp lists it but
        //   no data xref. Conservative: match default branch (return 0).
        default: return 0u;
    }

    // PIZ open type=2 mode=1.
    std::uint32_t* handle = s_FUN_004cc230(2, 1, name);
    if (handle == nullptr) {
        return 0u;
    }

    // Read up to 64 KB into DAT_0066d828.
    s_FUN_004cbd30(handle, reinterpret_cast<void*>(0x0066d828u), 0x10000u);

    // Close stream.
    s_FUN_004cc160(handle, 0u);

    // Success flag.
    *reinterpret_cast<std::uint32_t*>(0x0067d84cu) = 1u;
    return 1u;
}
RH_ScopedInstall(LocalizationFileLoad, 0x004274e0);

// ───────────────────────────────────────────────────────────────────────────
// 0x00428390  FrontendStateSet
//
// Original: FUN_00428390 (9 bytes, 0x00428390..0x00428398)
// Signature: void FUN_00428390(uint32_t param_1)
//
// Body (pure leaf):
//   MOV EAX, [ESP+4]              // param_1
//   MOV [0x0067d960], EAX         // @ 0x00428392
//   RET
//
// Writes: DAT_0067d960 = param_1  [0x00428392]
//
// ref: re/analysis/skeleton_prep_game_state/00428390.md
// ───────────────────────────────────────────────────────────────────────────
// 0x00428390
extern "C" __declspec(dllexport) void __cdecl FrontendStateSet(std::uint32_t param_1)
{
    // Pure global setter. [0x00428392]
    *reinterpret_cast<std::uint32_t*>(0x0067d960u) = param_1;
}
RH_ScopedInstall(FrontendStateSet, 0x00428390);

// ───────────────────────────────────────────────────────────────────────────
// 0x0042fa00  PlayerSlotEdgeAdjust
//
// Original: FUN_0042fa00 (162 bytes, 0x0042fa00..0x0042faa1)
// Signature: void FUN_0042fa00(void)
//
// Body (pure leaf, no callees):
//   iVar2 = DAT_007f1a44                // active slot id A
//   iVar1 = DAT_007f1a14                // active slot id B
//   if (DAT_0067ea88 == 2) DAT_0067ea88 = 0
//   piVar5 = &DAT_0067e938              // slot-array, stride 3 dwords (12 bytes)
//   pcVar3 = &DAT_007f1504              // input-state, stride 0x4c per slot
//   for iVar4 = 0; pcVar3 < 0x7f1894; iVar4++, pcVar3 += 0x4c, piVar5 += 3:
//     if iVar4 in {iVar1, iVar2, DAT_007f1a24, DAT_007f1a34}:
//       // Left-edge: -0x4c0 back into prior-state row
//       if (pcVar3[-0x4c0] != 0 && pcVar3[0] == 0 && *piVar5 > 0)
//         (*piVar5)--
//       // Right-edge: bytes[-0x4bf] + [+1]
//       if (pcVar3[-0x4bf] != 0 && pcVar3[1] == 0 && *piVar5 < 2)
//         (*piVar5)++
//       // Clamp [0, 2]
//       if (*piVar5 < 0) *piVar5 = 0
//       if (*piVar5 > 2) *piVar5 = 2
//
// Constants cited:
//   loop end exclusive: 0x7f1894    [0x0042fa00 body]
//   input stride: 0x4c (76)         [stride per slot]
//   slot stride dwords: 3 (12B)     [piVar5 stride]
//   sentinel reset value: 2 -> 0    [DAT_0067ea88 reset]
//
// ref: re/analysis/frontend_unmapped_a/0x0042fa00.md
// ───────────────────────────────────────────────────────────────────────────
// 0x0042fa00
extern "C" __declspec(dllexport) void __cdecl PlayerSlotEdgeAdjust()
{
    const std::int32_t iVar2 = *reinterpret_cast<const std::int32_t*>(0x007f1a44u);
    const std::int32_t iVar1 = *reinterpret_cast<const std::int32_t*>(0x007f1a14u);

    // [DAT_0067ea88 reset]
    if (*reinterpret_cast<const std::int32_t*>(0x0067ea88u) == 2) {
        *reinterpret_cast<std::int32_t*>(0x0067ea88u) = 0;
    }

    const std::int32_t s3 = *reinterpret_cast<const std::int32_t*>(0x007f1a24u);
    const std::int32_t s4 = *reinterpret_cast<const std::int32_t*>(0x007f1a34u);

    std::int32_t* piVar5 = reinterpret_cast<std::int32_t*>(0x0067e938u);
    auto*         pcVar3 = reinterpret_cast<std::int8_t*>(0x007f1504u);
    const auto*   pcEnd  = reinterpret_cast<const std::int8_t*>(0x007f1894u);

    std::int32_t iVar4 = 0;
    while (pcVar3 < pcEnd) {
        if (iVar4 == iVar1 || iVar4 == iVar2 || iVar4 == s3 || iVar4 == s4) {
            // Left-edge: back-row byte != 0, current byte == 0, slot > 0.
            if (pcVar3[-0x4c0] != 0 && pcVar3[0] == 0 && *piVar5 > 0) {
                (*piVar5)--;
            }
            // Right-edge: back-row byte+1 != 0, current+1 == 0, slot < 2.
            if (pcVar3[-0x4bf] != 0 && pcVar3[1] == 0 && *piVar5 < 2) {
                (*piVar5)++;
            }
            // Clamp [0, 2].
            if (*piVar5 < 0) *piVar5 = 0;
            if (*piVar5 > 2) *piVar5 = 2;
        }
        iVar4++;
        pcVar3 += 0x4c;
        piVar5 += 3;
    }
}
RH_ScopedInstall(PlayerSlotEdgeAdjust, 0x0042fa00);

// ───────────────────────────────────────────────────────────────────────────
// 0x00403050  PreRaceLoadingScreenDraw
//
// Original: FUN_00403050 (124 bytes, 0x00403050..0x004030cb)
// Signature: void FUN_00403050(void)
//
// Body:
//   if (DAT_00771964 != 0) {
//     FUN_00428760(DAT_00771964, 320.0, 200.0, 480.0, 240.0, 2)
//     vtable = *(DAT_007d3ff8); (vtable + 0x20)(6, 0)
//                                (vtable + 0x20)(8, 0)
//     FUN_00402fb0()
//     (vtable + 0x20)(6, 1)
//     (vtable + 0x20)(8, 1)
//   }
//
// Stack-cookie guard (DAT_00616038 ^ retaddr) — omitted in reimpl; MSVC /GS
// reinserts equivalent if active.
//
// Constants cited:
//   bg_x1 = 320.0   (0x43a00000)         [body, FUN_00428760 arg2]
//   bg_y1 = 200.0   (0x43480000)         [body, arg3]
//   bg_x2 = 480.0   (0x43f00000)         [body, arg4]
//   bg_y2 = 240.0   (0x43700000)         [body, arg5]
//   coord-mode = 2                       [body, arg6]
//   vtable slot offset = 0x20            [render-state setter]
//   render-state pairs: (6,0)(8,0) disable Z; (6,1)(8,1) restore
//
// ref: re/analysis/loading_screen/0x00403050.md
// ───────────────────────────────────────────────────────────────────────────

// Render-state vtable fn ptr (slot 0x20 on RW driver object).
typedef void (__cdecl* RwSetState_t)(int, int);

// 0x00403050
extern "C" __declspec(dllexport) void __cdecl PreRaceLoadingScreenDraw()
{
    std::uint32_t bg = *reinterpret_cast<const std::uint32_t*>(0x00771964u);
    if (bg == 0u) {
        return;
    }

    // Background textured quad (320, 200) -> (480, 240), coord-mode 2.
    s_FUN_00428760(bg, 320.0f, 200.0f, 480.0f, 240.0f, 2);

    // Render-state disable: (6, 0), (8, 0).
    std::uintptr_t vtable_obj = *reinterpret_cast<std::uintptr_t*>(0x007d3ff8u);
    RwSetState_t rs_fn = *reinterpret_cast<RwSetState_t*>(vtable_obj + 0x20u);
    rs_fn(6, 0);
    rs_fn(8, 0);

    // Pulsing sprite spinner.
    s_FUN_00402fb0();

    // Render-state restore: (6, 1), (8, 1). Re-read vtable in case state moved.
    vtable_obj = *reinterpret_cast<std::uintptr_t*>(0x007d3ff8u);
    rs_fn = *reinterpret_cast<RwSetState_t*>(vtable_obj + 0x20u);
    rs_fn(6, 1);
    rs_fn(8, 1);
}
RH_ScopedInstall(PreRaceLoadingScreenDraw, 0x00403050);

// ───────────────────────────────────────────────────────────────────────────
// 0x0042bcb0  PlayerNameSpriteDraw
//
// Original: FUN_0042bcb0 (222 bytes, 0x0042bcb0..0x0042bd8d)
// Signature:
//   void FUN_0042bcb0(int param_1, float param_2, float param_3,
//                     undefined4 param_4, int param_5)
//
// Body:
//   Stack cookie guard (DAT_00616038 ^ retaddr).
//   Coordinate transform — 4 floats:
//     local_24 = param_2 * _DAT_005cd5a8 + _DAT_005cd18c
//     local_20 = _DAT_005cc724 + param_3 * _DAT_005cc560
//     local_1c = param_2 * _DAT_005cd5a8 - _DAT_005cd18c
//     local_18 = param_3 * _DAT_005cc560 - _DAT_005cc724
//   Zero 16-byte local_14[0..3].
//   mode_byte = (&DAT_007e96fc)[param_1 * 0x80]
//   if (mode_byte == 2):
//     copy bytes from "keyboard" string at 0x005cd778 (9 bytes) into local_14
//   else if (mode_byte == 1):
//     local_14[0..6]: dword DAT_005cd770, word DAT_005cd774, byte DAT_005cd776
//   if (param_5 != 0): scan for null in local_14, append word DAT_005cd76c
//   ptr = FUN_004c5c00(DAT_00636ac8, &local_14)
//   if (ptr != 0):
//     uVar4 = *ptr
//     FUN_004b5750(*DAT_007d3ff8, &local_1c, &local_24, &param_4, uVar4)
//
// Constants cited:
//   stride: 0x80 (128)              [DAT_007e96fc index]
//   mode codes: 1, 2                [body branches]
//   coord-transform globals:        0x005cd5a8 (xScale), 0x005cd18c (xOffset)
//                                   0x005cc560 (yScale), 0x005cc724 (yOffset)
//   collection ptr:                 0x00636ac8
//   driver ptr:                     0x007d3ff8
//
// ref: re/analysis/c0_promotion_frontend_a/0x0042bcb0.md
// ───────────────────────────────────────────────────────────────────────────
// 0x0042bcb0
extern "C" __declspec(dllexport) void __cdecl PlayerNameSpriteDraw(
    int param_1, float param_2, float param_3,
    std::uint32_t param_4, int param_5)
{
    // Coordinate-transform constants (read at body).
    const float xScale  = *reinterpret_cast<const float*>(0x005cd5a8u);
    const float xOffset = *reinterpret_cast<const float*>(0x005cd18cu);
    const float yScale  = *reinterpret_cast<const float*>(0x005cc560u);
    const float yOffset = *reinterpret_cast<const float*>(0x005cc724u);

    float local_24 = param_2 * xScale + xOffset;
    float local_20 = yOffset + param_3 * yScale;
    float local_1c = param_2 * xScale - xOffset;
    float local_18 = param_3 * yScale - yOffset;

    // Suppress unused-warning: pair captured for caller via &local_24 / &local_1c.
    (void)local_20;
    (void)local_18;

    // 16-byte name buffer.
    std::uint8_t local_14[16];
    std::memset(local_14, 0, sizeof(local_14));

    // Mode-byte lookup: byte at DAT_007e96fc + param_1 * 0x80.
    const std::uint8_t mode_byte = *reinterpret_cast<const std::uint8_t*>(
        0x007e96fcu + static_cast<std::uintptr_t>(param_1) * 0x80u);

    if (mode_byte == 2) {
        // Copy 9 bytes from string "keyboard" at 0x005cd778 into local_14.
        const auto* src = reinterpret_cast<const std::uint8_t*>(0x005cd778u);
        for (int i = 0; i < 9; ++i) local_14[i] = src[i];
    } else if (mode_byte == 1) {
        // Build name from 3 globals: dword DAT_005cd770, word DAT_005cd774, byte DAT_005cd776.
        // Layout from original: local_14[0..3] = dword, [4..5] = word, [6] = byte.
        *reinterpret_cast<std::uint32_t*>(&local_14[0]) =
            *reinterpret_cast<const std::uint32_t*>(0x005cd770u);
        *reinterpret_cast<std::uint16_t*>(&local_14[4]) =
            *reinterpret_cast<const std::uint16_t*>(0x005cd774u);
        local_14[6] = *reinterpret_cast<const std::uint8_t*>(0x005cd776u);
    }
    // else: local_14 remains zero.

    if (param_5 != 0) {
        // Scan for null terminator; append word DAT_005cd76c there.
        int j = 0;
        while (j < 14 && local_14[j] != 0) ++j;
        *reinterpret_cast<std::uint16_t*>(&local_14[j]) =
            *reinterpret_cast<const std::uint16_t*>(0x005cd76cu);
    }

    // Lookup sprite by name.
    const std::uint32_t collection = *reinterpret_cast<const std::uint32_t*>(0x00636ac8u);
    std::uint32_t* puVar2 = s_FUN_004c5c00(collection, local_14);
    if (puVar2 != nullptr) {
        std::uint32_t handle = *puVar2;
        const std::uint32_t driver = *reinterpret_cast<const std::uint32_t*>(0x007d3ff8u);
        // Draw call: (driver, &local_1c, &local_24, &param_4, handle).
        s_FUN_004b5750(driver, &local_1c, &local_24, &param_4, handle);
    }
}
RH_ScopedInstall(PlayerNameSpriteDraw, 0x0042bcb0);

// ───────────────────────────────────────────────────────────────────────────
// 0x00431f30  FrontendPageFlagSet
//
// Original: FUN_00431f30 (226 bytes, 0x00431f30..0x00432012)
// Signature: void FUN_00431f30(int param_1)
//
// Body:
//   FUN_00431d90()                                  // mass-clear 19 panel flags
//   switch (param_1) {
//     case 4:                  DAT_0067e7a8 = 1
//     case 5:                  DAT_0067e7c8 = 1
//     case 6: case 7:          DAT_0067e7b8 = 1
//     case 8:                  DAT_0067e808 = 1; DAT_0067e828 = 1
//     case 9: case 0x10:       DAT_0067e7e0 = 1
//     case 10 (0xa):           DAT_0067e7b0 = 1
//     case 0xc:                DAT_0067e7d0 = 1
//     case 0xf:                DAT_0067e7d8 = 1
//     case 0x11:               DAT_0067e7e8 = 1
//     case 0x12: case 0x18:    DAT_0067e7f0 = 1
//     case 0x13:               DAT_0067e7f8 = 1
//     case 0x14:               DAT_0067e800 = 1
//     case 0x1c:               DAT_0067e810 = 1
//     case 0x1d:               DAT_0067e818 = 1
//     case 0x1e:               DAT_0067e820 = 1
//     case 0x1f: case 0x21:    DAT_0067e830 = 1
//     case 0x20:               DAT_0067e838 = 1
//     default:                 (no write)
//   }
//
// ref: re/analysis/options_menu/0x00431f30.md
// ───────────────────────────────────────────────────────────────────────────
// 0x00431f30
extern "C" __declspec(dllexport) void __cdecl FrontendPageFlagSet(int param_1)
{
    // Mass-clear 19 panel flags.
    s_FUN_00431d90();

    switch (param_1) {
        case 4:    *reinterpret_cast<std::uint32_t*>(0x0067e7a8u) = 1u; break;
        case 5:    *reinterpret_cast<std::uint32_t*>(0x0067e7c8u) = 1u; break;
        case 6:
        case 7:    *reinterpret_cast<std::uint32_t*>(0x0067e7b8u) = 1u; break;
        case 8:
            *reinterpret_cast<std::uint32_t*>(0x0067e808u) = 1u;
            *reinterpret_cast<std::uint32_t*>(0x0067e828u) = 1u;
            break;
        case 9:
        case 0x10: *reinterpret_cast<std::uint32_t*>(0x0067e7e0u) = 1u; break;
        case 0xa:  *reinterpret_cast<std::uint32_t*>(0x0067e7b0u) = 1u; break;
        case 0xc:  *reinterpret_cast<std::uint32_t*>(0x0067e7d0u) = 1u; break;
        case 0xf:  *reinterpret_cast<std::uint32_t*>(0x0067e7d8u) = 1u; break;
        case 0x11: *reinterpret_cast<std::uint32_t*>(0x0067e7e8u) = 1u; break;
        case 0x12:
        case 0x18: *reinterpret_cast<std::uint32_t*>(0x0067e7f0u) = 1u; break;
        case 0x13: *reinterpret_cast<std::uint32_t*>(0x0067e7f8u) = 1u; break;
        case 0x14: *reinterpret_cast<std::uint32_t*>(0x0067e800u) = 1u; break;
        case 0x1c: *reinterpret_cast<std::uint32_t*>(0x0067e810u) = 1u; break;
        case 0x1d: *reinterpret_cast<std::uint32_t*>(0x0067e818u) = 1u; break;
        case 0x1e: *reinterpret_cast<std::uint32_t*>(0x0067e820u) = 1u; break;
        case 0x1f:
        case 0x21: *reinterpret_cast<std::uint32_t*>(0x0067e830u) = 1u; break;
        case 0x20: *reinterpret_cast<std::uint32_t*>(0x0067e838u) = 1u; break;
        default:   break;   // no write
    }
}
RH_ScopedInstall(FrontendPageFlagSet, 0x00431f30);
