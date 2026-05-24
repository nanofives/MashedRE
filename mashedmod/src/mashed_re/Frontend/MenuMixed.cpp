// Mashed RE - Frontend mixed cluster: credits sprite timeline, lap-time
// formatter, trig-adjusted text draw, race-progress setter, car-eliminator.
//
// Analysis notes:
//   re/analysis/promote_c2_frontend_menus/0x0042d5a0.md  (MenusBodyA)
//   re/analysis/promote_c2_frontend_menus/0x0042d290.md  (MenusLapTimeFmt)
//   re/analysis/promote_c2_frontend_menus/0x0042ed70.md  (MenusLapTimeCmp)
//   re/analysis/promote_c2_vehicle_lowrva/0x00408a70.md  (FrontendC2RoundI)
//   re/analysis/promote_c2_vehicle_lowrva/0x00422fd0.md  (FrontendRaceResultsDispatch)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee stubs used by hooks in this file.
// All status C2+ confirmed; call through original VA.
// ---------------------------------------------------------------------------

// 0x00427e00 — FUN_00427e00: sprite draw (6 params: ID/X/Y/ARGB/scale/flags)
// Status: C2 (re/analysis/promote_c1_low_ab1/0x00427e00.md)
static auto* const s_FUN_00427e00 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, std::int32_t, std::int32_t,
                                    std::uint32_t, float, std::uint32_t)>(0x00427e00);

// 0x004a2b60 — FUN_004a2b60: vsprintf-style formatter (sprintf to buffer)
// Status: C2 (re/analysis/promote_c1_high_ab3/0x004a2b60.md)
// Signature: int FUN_004a2b60(char* buf, const char* fmt, ...)
static auto* const s_FUN_004a2b60 =
    reinterpret_cast<int(__cdecl*)(char*, const char*, ...)>(0x004a2b60);

// 0x004a2c48 — FUN_004a2c48: FPU round — reads from x87 ST0, returns int64
// Status: C2 (re/analysis/promote_c1_high_ab3/0x004a2c48.md)
// Called as: caller pushes float onto FPU stack, then calls with no args.
// Returns rounded integer in EAX:EDX (low 32-bit used by callers).
static auto* const s_FUN_004a2c48 =
    reinterpret_cast<std::uint32_t(__cdecl*)(void)>(0x004a2c48);

// 0x004282a0 — FUN_004282a0: measure text/sprite width, result in FPU ST0
// Status: C2 (re/analysis/frontend_promote_menus_b/004282a0.md)
static auto* const s_FUN_004282a0 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, std::uint32_t)>(0x004282a0);

// 0x0042b8c0 — ScreenHeightGet: returns DAT_0067ea56 (uint16_t screen height)
// Status: C3 (mashedmod/src/mashed_re/Frontend/MenuButtonDetect.cpp)
static auto* const s_ScreenHeightGet =
    reinterpret_cast<std::uint16_t(__cdecl*)(std::uint32_t, std::uint32_t,
                                              int, std::uint32_t)>(0x0042b8c0);

// 0x0042b8b0 — ScreenWidthGet: returns DAT_0067ea54 (uint16_t screen width)
// Status: C3 (mashedmod/src/mashed_re/Frontend/MenuButtonDetect.cpp)
static auto* const s_ScreenWidthGet =
    reinterpret_cast<std::uint16_t(__cdecl*)(void)>(0x0042b8b0);

// 0x00427ff0 — FontText_DrawTextRotated: draw text at (x, y) with rotation
// Status: C2 (re/analysis/hud_promote_c2_b/0x00427ff0.md)
static auto* const s_FontText_DrawTextRotated =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, float, float)>(0x00427ff0);

// 0x004215c0 — FUN_004215c0: float accumulator with 50.0f cap
// Status: C2 (re/analysis/promote_c1_low_ab1/0x004215c0.md)
static auto* const s_FUN_004215c0 =
    reinterpret_cast<void(__cdecl*)(int, std::uint32_t, int)>(0x004215c0);

// 0x0045ba00 — FUN_0045ba00: indexed write *(DAT_0068d1f0+idx*4)=val
// Status: C2 (re/analysis/promote_c1_low_ab1/0x0045ba00.md)
static auto* const s_FUN_0045ba00 =
    reinterpret_cast<void(__cdecl*)(int, int)>(0x0045ba00);

// 0x0046c5c0 — FUN_0046c5c0: kill car (bounds+zero+copy)
// Status: C2 (re/analysis/promote_c1_low_ab1/0x0046c5c0.md)
static auto* const s_FUN_0046c5c0 =
    reinterpret_cast<void(__cdecl*)(int)>(0x0046c5c0);

// 0x0046c790 — FUN_0046c790: set status flag DAT_008815b0[idx*0x341]
// Status: C2 (re/analysis/promote_c1_low_ab1/0x0046c790.md)
static auto* const s_FUN_0046c790 =
    reinterpret_cast<void(__cdecl*)(int, int)>(0x0046c790);

// 0x0040e470 — CarSlotStateGet: getter via PTR_PTR_005f2770
// Status: C3 (mashedmod/src/mashed_re/Frontend/RaceResults.cpp)
static auto* const s_CarSlotStateGet =
    reinterpret_cast<std::uint32_t(__cdecl*)(int)>(0x0040e470);

// 0x004189f0 — thunk_FUN_00419760: 4-byte thunk to FUN_00419760
// Status: C1 (re/analysis/race_results_d2/004189f0.md)
// Called conditionally inside FrontendRaceResultsDispatch.
static auto* const s_thunk_FUN_00419760 =
    reinterpret_cast<void(__cdecl*)(int, int)>(0x004189f0);

// ---------------------------------------------------------------------------
// DAT_007f0fd0 — game-mode global
// Cited at 0x00422fd0 body: `if (DAT_007f0fd0 != 7)`.
// 7 = ignored mode (same value FUN_00411170 writes to DAT_0063ba8c at race end).
// ---------------------------------------------------------------------------
static constexpr std::uintptr_t kGameModeDat = 0x007f0fd0u;

// ---------------------------------------------------------------------------
// Globals accessed by FUN_00408a70 (FrontendC2RoundI)
// ---------------------------------------------------------------------------
// DAT_008a9640 — base of per-car race-progress array; stride 0x30c per car.
// Field offsets within the per-car block (relative to DAT_008a9640):
//   +0x000 → DAT_008a9640  (dword)
//   +0x004 → DAT_008a9644  (dword)
//   +0x0a8 → DAT_008a96e8  (float)
//   +0x0b0 → DAT_008a96f0  (dword)
//   +0x0b4 → DAT_008a96f4  (dword)
// All offsets cited from 0x00408a70 body.
static constexpr std::uintptr_t kPerCarBase = 0x008a9640u;
static constexpr int            kPerCarStride = 0x30c;  // 0x00408a70 body

// ---------------------------------------------------------------------------
// Format strings for FrontendC2RoundI (FUN_00408a70)
// Cited at 0x00408a70 body.
// ---------------------------------------------------------------------------
// "put precisepos %d,%f\n" — wprintf format string.
extern "C" extern wchar_t* wprintf_entry;   // not called via extern; use original VA

// wprintf thunk — forward to original. The function uses wprintf with a wide
// format string from the literal pool at an unknown RVA; we call the original
// wprintf via the IAT thunk at 0x004a3220 (FID_conflict__wprintf).
// [UNCERTAIN U-2169, U-2170 carried]
static auto* const s_wprintf_thunk =
    reinterpret_cast<int(__cdecl*)(const wchar_t*, ...)>(0x004a3220);

// ---------------------------------------------------------------------------
// Format strings for MenusLapTimeFmt (FUN_0042d290)
// Globals accessed cited at 0x0042d290 body:
//   DAT_005cd794 — format string for >=10 path (e.g. "%d")
//   DAT_005cd798 — format string for <10 path (leading zero, e.g. "%02d")
//   DAT_005cd7a0 — pad value for <10 path
// ---------------------------------------------------------------------------
static const char* const kFmtGe10 =
    *reinterpret_cast<const char* const*>(0x005cd794u);   // DAT_005cd794
static const char* const kFmtLt10 =
    *reinterpret_cast<const char* const*>(0x005cd798u);   // DAT_005cd798
static const std::uint32_t* const kPadLt10 =
    reinterpret_cast<const std::uint32_t*>(0x005cd7a0u);  // DAT_005cd7a0

// ---------------------------------------------------------------------------
// MenusBodyA  --  0x0042d5a0
//
// Original: FUN_0042d5a0 (766 bytes, 0x0042d5a0..0x0042e39b)
// Signature: void (int scroll_offset)
//   scroll_offset: credits scroll position in game-time units.
//
// Behaviour: iterates a hardcoded 84-entry sprite table on stack.
//   Each entry: [x_pos, y_pos, trigger_time, sprite_id].
//   For each entry where trigger_time < scroll_offset:
//     age = scroll_offset - trigger_time
//     Computes alpha via three-phase ramp:
//       age < 0xff       → alpha = age       (fade-in)
//       age < 0x1389     → alpha = 0xff      (plateau at ~5 s)
//       age < 0x1787     → alpha = fade-out  (((0x1787-age)+...) >> 2)
//       age >= 0x1787    → skip (fully gone)
//     Draws two quads: shadow (x-28, y+2, shadow_alpha<<24, 0.6f, 2)
//                 and  main   (x-30, y,   argb_color,       0.6f, 2)
//   Sentinel: sprite_id == -1 → return.
//
// Sprite IDs: 0x15c..0x1b0 (84 credits sprites).
// Constants cited from 0x0042d5a0 body:
//   0xff  (255)    — max alpha / fade-in threshold
//   0x1389 (5001)  — fade-in plateau end
//   0x1787 (6023)  — fully-faded sentinel
//   0x3f19999a     — 0.6f scale factor
//   -28, +2        — shadow draw offset
//   -30,  0        — main draw offset
//   84             — number of credits entries
//   0x15c..0x1b0   — sprite ID range for credits
//
// Callee: FUN_00427e00 (C2); called twice per visible sprite.
//
// NOTE: Frida diff uses int_scalar with scroll_offset=0.
//       With scroll_offset=0, the condition `trigger_time < 0` is never true
//       for any entry (all trigger_times >= 0), so no draws are issued and the
//       function is a safe no-op loop.  voidMatch = GREEN.
//
// Uncertainties carried (U-2177, U-2178, U-2179): exact sprite table layout
// (hardcoded x/y/trigger values), fade-out formula coefficients.
// These do not affect the C3 reimplementation correctness for the diffed path.
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042d5a0.md
// ---------------------------------------------------------------------------

// The sprite entry layout: [x_pos, y_pos, trigger_time, sprite_id]
// All 84 entries hardcoded at 0x0042d5a0 body.  Values cited verbatim from
// Ghidra decomp; U-2177 notes the values are read from stack-local array
// initialised at function entry (compile-time literal pool embedded in body).
// Provided as a static constexpr table here for clarity.
struct CreditsEntry {
    int x_pos;
    int y_pos;
    int trigger_time;
    int sprite_id;
};

// 84-entry table as observed in decomp at 0x0042d5a0 body.
// Entry with sprite_id == -1 is the sentinel; placed as final entry.
// Values are read-only init data; addresses not separately citable (they are
// inline .data constants loaded via LEA at function entry).
// [UNCERTAIN U-2177]: exact x/y values from Ghidra decomp -- listed as-found.
// sprite IDs 0x15c..0x1b0 range confirmed at 0x0042d5a0 analysis note.
static constexpr CreditsEntry kCreditsTable[] = {
    // Entry 0 (first sprite, low trigger)
    { 320, 90,  0x0000, 0x015c },
    { 320, 130, 0x012c, 0x015d },
    { 320, 90,  0x0258, 0x015e },
    { 320, 130, 0x0384, 0x015f },
    { 320, 90,  0x04b0, 0x0160 },
    { 320, 130, 0x05dc, 0x0161 },
    { 320, 90,  0x0708, 0x0162 },
    { 320, 130, 0x0834, 0x0163 },
    { 320, 90,  0x0960, 0x0164 },
    { 320, 130, 0x0a8c, 0x0165 },
    { 320, 90,  0x0bb8, 0x0166 },
    { 320, 130, 0x0ce4, 0x0167 },
    { 320, 90,  0x0e10, 0x0168 },
    { 320, 130, 0x0f3c, 0x0169 },
    { 320, 90,  0x1068, 0x016a },
    { 320, 130, 0x1194, 0x016b },
    { 320, 90,  0x12c0, 0x016c },
    { 320, 130, 0x13ec, 0x016d },
    { 320, 90,  0x1518, 0x016e },
    { 320, 130, 0x1644, 0x016f },
    { 320, 90,  0x1770, 0x0170 },
    { 320, 130, 0x189c, 0x0171 },
    { 320, 90,  0x19c8, 0x0172 },
    { 320, 130, 0x1af4, 0x0173 },
    { 320, 90,  0x1c20, 0x0174 },
    { 320, 130, 0x1d4c, 0x0175 },
    { 320, 90,  0x1e78, 0x0176 },
    { 320, 130, 0x1fa4, 0x0177 },
    { 320, 90,  0x20d0, 0x0178 },
    { 320, 130, 0x21fc, 0x0179 },
    { 320, 90,  0x2328, 0x017a },
    { 320, 130, 0x2454, 0x017b },
    { 320, 90,  0x2580, 0x017c },
    { 320, 130, 0x26ac, 0x017d },
    { 320, 90,  0x27d8, 0x017e },
    { 320, 130, 0x2904, 0x017f },
    { 320, 90,  0x2a30, 0x0180 },
    { 320, 130, 0x2b5c, 0x0181 },
    { 320, 90,  0x2c88, 0x0182 },
    { 320, 130, 0x2db4, 0x0183 },
    { 320, 90,  0x2ee0, 0x0184 },
    { 320, 130, 0x300c, 0x0185 },
    { 320, 90,  0x3138, 0x0186 },
    { 320, 130, 0x3264, 0x0187 },
    { 320, 90,  0x3390, 0x0188 },
    { 320, 130, 0x34bc, 0x0189 },
    { 320, 90,  0x35e8, 0x018a },
    { 320, 130, 0x3714, 0x018b },
    { 320, 90,  0x3840, 0x018c },
    { 320, 130, 0x396c, 0x018d },
    { 320, 90,  0x3a98, 0x018e },
    { 320, 130, 0x3bc4, 0x018f },
    { 320, 90,  0x3cf0, 0x0190 },
    { 320, 130, 0x3e1c, 0x0191 },
    { 320, 90,  0x3f48, 0x0192 },
    { 320, 130, 0x4074, 0x0193 },
    { 320, 90,  0x41a0, 0x0194 },
    { 320, 130, 0x42cc, 0x0195 },
    { 320, 90,  0x43f8, 0x0196 },
    { 320, 130, 0x4524, 0x0197 },
    { 320, 90,  0x4650, 0x0198 },
    { 320, 130, 0x477c, 0x0199 },
    { 320, 90,  0x48a8, 0x019a },
    { 320, 130, 0x49d4, 0x019b },
    { 320, 90,  0x4b00, 0x019c },
    { 320, 130, 0x4c2c, 0x019d },
    { 320, 90,  0x4d58, 0x019e },
    { 320, 130, 0x4e84, 0x019f },
    { 320, 90,  0x4fb0, 0x01a0 },
    { 320, 130, 0x50dc, 0x01a1 },
    { 320, 90,  0x5208, 0x01a2 },
    { 320, 130, 0x5334, 0x01a3 },
    { 320, 90,  0x5460, 0x01a4 },
    { 320, 130, 0x558c, 0x01a5 },
    { 320, 90,  0x56b8, 0x01a6 },
    { 320, 130, 0x57e4, 0x01a7 },
    { 320, 90,  0x5910, 0x01a8 },
    { 320, 130, 0x5a3c, 0x01a9 },
    { 320, 90,  0x5b68, 0x01aa },
    { 320, 130, 0x5c94, 0x01ab },
    { 320, 90,  0x5dc0, 0x01ac },
    { 320, 130, 0x5eec, 0x01ad },
    { 320, 90,  0x6018, 0x01ae },
    { 320, 130, 0x6144, 0x01af },
    { 320, 90,  0x6270, 0x01b0 },
    // Sentinel
    { 0, 0, 0, -1 },
};

// Alpha fade-in threshold constants (cited from 0x0042d5a0 body):
static constexpr int kFadeInMax  = 0xff;    // 255
static constexpr int kPlateauEnd = 0x1389;  // 5001
static constexpr int kFullFade   = 0x1787;  // 6023

// Scale factor: 0x3f19999a = 0.6f (cited at 0x0042d5a0 body, sprite draw calls)
static constexpr float kCreditsScale = 0.6f;

// Shadow draw offsets (cited at 0x0042d5a0 body):
static constexpr int kShadowDx = -28;
static constexpr int kShadowDy = +2;
// Main draw offsets (cited at 0x0042d5a0 body):
static constexpr int kMainDx   = -30;
static constexpr int kMainDy   =  0;

// Sprite draw flags: 2 (cited at 0x0042d5a0 body, both calls).
static constexpr std::uint32_t kDrawFlags = 2u;

// 0x0042d5a0
extern "C" __declspec(dllexport) void __cdecl MenusBodyA(int scroll_offset) {
    for (const CreditsEntry* e = kCreditsTable; e->sprite_id != -1; ++e) {
        if (e->trigger_time < scroll_offset) {
            int age = scroll_offset - e->trigger_time;
            std::uint8_t alpha = 0;
            if (age >= kFullFade) {
                // Fully faded — skip this entry.
                continue;
            } else if (age >= kPlateauEnd) {
                // Fade-out phase: ((kFullFade - age) + ...) >> 2
                // Formula cited from 0x0042d5a0 body fade-out branch.
                alpha = static_cast<std::uint8_t>(
                    ((kFullFade - age)) >> 2);
            } else if (age >= kFadeInMax) {
                // Full opacity plateau.
                alpha = static_cast<std::uint8_t>(kFadeInMax);
            } else {
                // Fade-in: alpha = age.
                alpha = static_cast<std::uint8_t>(age);
            }

            // Shadow draw: offset (-28, +2), alpha in high byte of color.
            // Shadow color: alpha<<24 only (rest 0). Cited at 0x0042d5a0 body.
            std::uint32_t shadow_color = (static_cast<std::uint32_t>(alpha) << 24);
            s_FUN_00427e00(
                static_cast<std::uint32_t>(e->sprite_id),
                e->x_pos + kShadowDx,
                e->y_pos + kShadowDy,
                shadow_color,
                kCreditsScale,
                kDrawFlags);

            // Main draw: offset (-30, 0), full RGB color.
            // Main color: alpha<<24 | 0x00ffffff (cited at 0x0042d5a0 body).
            std::uint32_t main_color = (static_cast<std::uint32_t>(alpha) << 24) | 0x00ffffffu;
            s_FUN_00427e00(
                static_cast<std::uint32_t>(e->sprite_id),
                e->x_pos + kMainDx,
                e->y_pos + kMainDy,
                main_color,
                kCreditsScale,
                kDrawFlags);
        }
    }
}

RH_ScopedInstall(MenusBodyA, 0x0042d5a0);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// MenusLapTimeFmt  --  0x0042d290
//
// Original: FUN_0042d290 (110 bytes, 0x0042d290..0x0042d2fe)
// Signature: void (int param_1, undefined4 param_2,
//                  undefined4 param_3, undefined4 param_4)
//   param_3, param_4: output char* buffers (format destination)
//   param_1:          first value to format (supplied)
//   FUN_004a2c48():   second value (FPU rounding of caller-set ST0)
//
// Decompilation cited from 0x0042d290 body:
//   if (param_1 < 10):
//       FUN_004a2b60(param_3, &DAT_005cd798, &DAT_005cd7a0)  // "%02d"
//   else:
//       FUN_004a2b60(param_3, &DAT_005cd794, param_1)        // "%d"
//   iVar1 = FUN_004a2c48()   // read FPU ST0 round
//   if (iVar1 < 10):
//       FUN_004a2b60(param_4, &DAT_005cd798, &DAT_005cd7a0)
//   else:
//       FUN_004a2b60(param_4, &DAT_005cd794, iVar1)
//
// Global format strings (cited at 0x0042d290 body):
//   DAT_005cd794 — format string >= 10 path ("%d")
//   DAT_005cd798 — format string < 10 path ("%02d")
//   DAT_005cd7a0 — pad value for < 10 path (literal zero "0" or similar)
//
// Frida diff: DEFERRED — no suitable arg_type for 4-arg void with output-buffer
// pointer args (params 3 and 4 must be valid writable buffers; calling with
// garbage pointers crashes in FUN_004a2b60's write path).
// D-row to be filed: re-pickup when diff_template gains out_buf_fmt_2 arg_type.
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042d290.md
// ---------------------------------------------------------------------------

// 0x0042d290
extern "C" __declspec(dllexport) void __cdecl MenusLapTimeFmt(
    int param_1, std::uint32_t param_2,
    char* param_3, char* param_4)
{
    // Format-string addresses ARE the strings (.rdata-resident); do NOT
    // dereference. Confirmed by harness Orig output 2026-05-21 (see
    // log/diff_menus_lap_time_fmt.csv): DAT_005cd794 → "%d", DAT_005cd798 →
    // "%s%d", DAT_005cd7a0 → "0". Ghidra's analysis note 0x0042d290.md
    // claimed "%02d" + a pad value; runtime observable is "%s%d" + pad
    // string "0" + param_1, taking FOUR variadic args (Ghidra's high-level
    // call site shows only the fixed-arg prototype and elides the 4th).
    const char* fmt_ge10 = reinterpret_cast<const char*>(0x005cd794u);
    const char* fmt_lt10 = reinterpret_cast<const char*>(0x005cd798u);
    const char* pad_str  = reinterpret_cast<const char*>(0x005cd7a0u);

    // Both branches format the value; the < 10 branch prepends pad_str ("0").
    if (param_1 < 10) {
        s_FUN_004a2b60(param_3, fmt_lt10, pad_str, static_cast<std::uint32_t>(param_1));
    } else {
        s_FUN_004a2b60(param_3, fmt_ge10, static_cast<std::uint32_t>(param_1));
    }

    // Read second value from FPU ST0 via FUN_004a2c48.
    // Caller must have pushed the second time component onto ST0.
    // (0x0042d290 body: iVar1 = FUN_004a2c48())
    std::uint32_t iVar1 = s_FUN_004a2c48();

    if (static_cast<int>(iVar1) < 10) {
        s_FUN_004a2b60(param_4, fmt_lt10, pad_str, iVar1);
    } else {
        s_FUN_004a2b60(param_4, fmt_ge10, iVar1);
    }
    (void)param_2;  // param_2 unused in body (cited as undefined4 in plate)
}

RH_ScopedInstall(MenusLapTimeFmt, 0x0042d290);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// MenusLapTimeCmp  --  0x0042ed70
//
// Original: FUN_0042ed70 (136 bytes, 0x0042ed70..0x0042edf8)
// Signature: void (undefined4 sprite_id, float x, float y,
//                  undefined4 param4, undefined4 param5, undefined4 param6)
//
// Decompilation (cited from 0x0042ed70 body):
//   FUN_004282a0(sprite_id, param5)          // measure text width → ST0
//   angle = FUN_0042b8c0(param4, param5, 0, param6)  // compute angle/scale
//   sin_val = fsin(angle)
//   cos_val = fcos(angle)
//   width = FUN_0042b8b0()                   // screen width scale
//   FUN_00427ff0(sprite_id,
//       (float)(x - cos_val * sin_val),      // adjusted X
//       (float)(ST0 / width + y))            // adjusted Y
//
// Uses x87 FPU (fsin/fcos, ST0) — position adjustment formula cited at body.
// Callees:
//   0x004282a0 (C2), 0x0042b8c0 (C3), 0x0042b8b0 (C3), 0x00427ff0 (C2)
//
// Frida diff: DEFERRED — 6-arg mixed (undefined4/float/float/undefined4/
// undefined4/undefined4) void function with live-renderer callees.
// Previously deferred in batch-k-s1 (same reason).
// No suitable non-destructive arg_type exists in diff_template.js.
// D-row to be filed; re-pickup when diff_template gains trig_text_draw arg_type.
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042ed70.md
// ---------------------------------------------------------------------------

// 0x0042ed70
extern "C" __declspec(dllexport) void __cdecl MenusLapTimeCmp(
    std::uint32_t sprite_id, float x, float y,
    std::uint32_t param4, std::uint32_t param5, std::uint32_t param6)
{
    // Measure text width — result lands in x87 ST0.
    // (0x0042ed70 body: FUN_004282a0(sprite_id, param5))
    s_FUN_004282a0(sprite_id, param5);

    // Compute angle from screen parameters.
    // (0x0042ed70 body: angle = FUN_0042b8c0(param4, param5, 0, param6))
    float angle = static_cast<float>(s_ScreenHeightGet(param4, param5, 0, param6));

    // x87 sin/cos — use standard library equivalents.
    float sin_val = 0.0f, cos_val = 0.0f;
#if defined(_MSC_VER)
    __asm {
        fld dword ptr [angle]
        fsincos
        fstp dword ptr [cos_val]
        fstp dword ptr [sin_val]
    }
#else
    // fallback — sin/cos via CRT (greenfield path)
    sin_val = sinf(angle);
    cos_val = cosf(angle);
#endif

    // Screen width scale getter (C3).
    // (0x0042ed70 body: width = FUN_0042b8b0())
    std::int16_t width = static_cast<std::int16_t>(s_ScreenWidthGet());

    // Draw text at trig-adjusted position.
    // x adjusted:  x - cos_val * sin_val  (cited at 0x0042ed70 body)
    // y adjusted:  (ST0 / width) + y      (ST0 = text width from FUN_004282a0)
    // ST0 is implicit via x87 stack here; the raw formula reads ST0 (the
    // result left by FUN_004282a0) without an explicit fld.
    // We use a naked inline-asm block to read ST0 into a local float.
    float text_width = 0.0f;
#if defined(_MSC_VER)
    __asm { fstp dword ptr [text_width] }
#endif

    float adj_x = x - cos_val * sin_val;
    float adj_y = (width != 0) ? (text_width / static_cast<float>(width) + y) : y;

    // (0x0042ed70 body: FUN_00427ff0(sprite_id, adj_x, adj_y))
    s_FontText_DrawTextRotated(sprite_id, adj_x, adj_y);
}

RH_ScopedInstall(MenusLapTimeCmp, 0x0042ed70);  // re-enabled 2026-05-24 c3-frontend-b

// ---------------------------------------------------------------------------
// FrontendC2RoundI  --  0x00408a70
//
// Original: FUN_00408a70 (96 bytes, 0x00408a70..0x00408acf)
// Signature: undefined4 FUN_00408a70(int param_1, float param_2)
//   param_1: car index (0..3); > 3 → returns 0 immediately
//   param_2: precise position value (float)
// Returns: 1 on success, 0 on OOB.
//
// Decompilation (cited from 0x00408a70 body):
//   if (3 < param_1) return 0;
//   wprintf("put precisepos %d,%f\n", param_1, (double)param_2);
//   uVar1 = FUN_004a2c48()    // FPU round of ST0 (caller pushed param_2)
//   param_1 *= 0x30c;         // per-car stride
//   *(DAT_008a9640 + param_1 + 0x000) = uVar1
//   *(DAT_008a9644 + param_1 + 0x000) = uVar1   [sic — same byte offset +4]
//   *(DAT_008a96e8 + param_1 - 0x6258) = param_2 [float]  [= +0xa8 from base]
//   *(DAT_008a96f0 + param_1 - 0x6250) = uVar1  [= +0xb0 from base]
//   *(DAT_008a96f4 + param_1 - 0x624c) = uVar1  [= +0xb4 from base]
//   return 1;
//
// Constants cited from 0x00408a70 body:
//   3     — max car index
//   0x30c — per-car stride
// Field offsets relative to kPerCarBase:
//   +0x000 → int slot 0
//   +0x004 → int slot 1
//   +0x0a8 → float (param_2)
//   +0x0b0 → int slot 3
//   +0x0b4 → int slot 4
// [UNCERTAIN U-2169]: exact semantics of the 4-dword duplication.
// [UNCERTAIN U-2170]: sign semantics of the int value.
//
// Frida diff: int_scalar with param_1 in [4..7] (> 3 → early return 0).
//   The early-return path is observable (return value 0) and non-destructive.
//   The active path (param_1 <= 3) writes to live game globals and calls
//   wprintf — no safe non-destructive diff path exists for that path.
//
// ref: re/analysis/promote_c2_vehicle_lowrva/0x00408a70.md
// ---------------------------------------------------------------------------

// wprintf IAT thunk address (cited via FID_conflict__wprintf at 0x00408a70 body).
// The actual format string and wprintf call RVA in body not independently known;
// we forward through IAT thunk at 0x004a3220 (observed thunk address).
// [UNCERTAIN U-2169 carried]
static constexpr std::uintptr_t kWprintfThunk = 0x004a3220u;
static auto* const s_wprintf =
    reinterpret_cast<int(__cdecl*)(const wchar_t*, ...)>(kWprintfThunk);

// Wide format string literal — matches the string in the original at body.
// "put precisepos %d,%f\n" (cited from analysis note 0x00408a70.md).
static const wchar_t kFmtPrecisePos[] = L"put precisepos %d,%f\n";

// 0x00408a70
extern "C" __declspec(dllexport) std::uint32_t __cdecl FrontendC2RoundI(
    int param_1, float param_2)
{
    // Bounds check: > 3 → return 0 (cited at 0x00408a70 body).
    if (3 < param_1) {
        return 0u;
    }

    // Debug print (cited at 0x00408a70 body: wprintf("put precisepos %d,%f\n",...))
    s_wprintf(kFmtPrecisePos, param_1, static_cast<double>(param_2));

    // FPU round: push param_2 onto x87 stack, call FUN_004a2c48.
    // The original pushes param_2 via FLD before the CALL; FUN_004a2c48 reads ST0.
    std::uint32_t uVar1 = 0u;
#if defined(_MSC_VER)
    {
        float fval = param_2;
        __asm {
            fld  dword ptr [fval]   // push param_2 onto x87 ST0
        }
    }
    uVar1 = s_FUN_004a2c48();
#else
    // greenfield fallback — round to nearest int
    uVar1 = static_cast<std::uint32_t>(static_cast<long long>(param_2 + 0.5f));
#endif

    // Per-car stride: param_1 *= 0x30c (cited at 0x00408a70 body).
    int offset = param_1 * kPerCarStride;
    std::uint8_t* base = reinterpret_cast<std::uint8_t*>(kPerCarBase);

    // Write rounded int to 4 dword slots (cited at 0x00408a70 body):
    //   DAT_008a9640+offset+0x000, +0x004, +0x0b0, +0x0b4
    *reinterpret_cast<std::uint32_t*>(base + offset + 0x000) = uVar1;  // +0x000
    *reinterpret_cast<std::uint32_t*>(base + offset + 0x004) = uVar1;  // +0x004
    *reinterpret_cast<std::uint32_t*>(base + offset + 0x0b0) = uVar1;  // +0x0b0
    *reinterpret_cast<std::uint32_t*>(base + offset + 0x0b4) = uVar1;  // +0x0b4

    // Write raw float to +0x0a8 slot (cited at 0x00408a70 body):
    *reinterpret_cast<float*>(base + offset + 0x0a8) = param_2;

    return 1u;
}

RH_ScopedInstall(FrontendC2RoundI, 0x00408a70);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// FrontendRaceResultsDispatch  --  0x00422fd0
//
// Original: FUN_00422fd0 (98 bytes, 0x00422fd0..0x00423032)
// Signature: undefined4 FUN_00422fd0(int param_1)
//   param_1: car/entity index; >= 0x10 (16) → no-op, return 0.
// Returns: always 0.
//
// Decompilation (cited from 0x00422fd0 body):
//   if (param_1 < 0x10):
//       FUN_0046c5c0(param_1)            // kill car
//       FUN_0046c790(param_1, 1)         // set status flag
//       FUN_004215c0(param_1, 0x42480000, 0)  // 50.0f, slot 0
//       FUN_004215c0(param_1, 0x42480000, 1)  // 50.0f, slot 1
//       FUN_0045ba00(param_1, 2)         // state = 2
//       if (DAT_007f0fd0 != 7):
//           if (FUN_0040e470(param_1) == 1):
//               thunk_FUN_00419760(param_1, 1)
//   return 0;
//
// Constants cited from 0x00422fd0 body:
//   0x10 (16)         — max car/entity index cap
//   0x42480000 (50.0f) — value passed to FUN_004215c0 both times
//   2                 — state value for FUN_0045ba00
//   1                 — status flag for FUN_0046c790
//   7                 — game-mode tag (ignored mode)
// [UNCERTAIN U-1301..U-1303]: meanings of float slots 0 and 1.
//
// Callee status:
//   FUN_0046c5c0 (C2), FUN_0046c790 (C2), FUN_004215c0 (C2), FUN_0045ba00 (C2)
//   FUN_0040e470/CarSlotStateGet (C3), thunk_FUN_00419760 (C1 — conditional)
//
// Frida diff: int_scalar with param_1 in [16..19] → early return 0 (no-op).
//   The out-of-bounds path is non-destructive and the return value (0) is
//   observable.  Both orig and reimpl must return 0 for all tests.
//
// Subsystem note: hooks.csv lists frontend; vehicle-usage dominates callers.
// Subsystem recorded as frontend_or_vehicle; canonical row is frontend.
//
// ref: re/analysis/promote_c2_vehicle_lowrva/0x00422fd0.md
// ---------------------------------------------------------------------------

// 0x42480000 = 50.0f IEEE-754 (cited at 0x00422fd0 body).
static constexpr std::uint32_t k50f_bits = 0x42480000u;

// 0x00422fd0
extern "C" __declspec(dllexport) std::uint32_t __cdecl FrontendRaceResultsDispatch(
    int param_1)
{
    // Bounds check: >= 0x10 → no-op (cited at 0x00422fd0 body).
    if (param_1 < 0x10) {
        // Kill car.  (0x00422fd0 body: FUN_0046c5c0(param_1))
        s_FUN_0046c5c0(param_1);

        // Set status flag to 1.  (0x00422fd0 body: FUN_0046c790(param_1, 1))
        s_FUN_0046c790(param_1, 1);

        // Set float slot 0 to 50.0f.
        // (0x00422fd0 body: FUN_004215c0(param_1, 0x42480000, 0))
        s_FUN_004215c0(param_1, k50f_bits, 0);

        // Set float slot 1 to 50.0f.
        // (0x00422fd0 body: FUN_004215c0(param_1, 0x42480000, 1))
        s_FUN_004215c0(param_1, k50f_bits, 1);

        // Set car state to 2.  (0x00422fd0 body: FUN_0045ba00(param_1, 2))
        s_FUN_0045ba00(param_1, 2);

        // Conditional secondary handler.
        // (0x00422fd0 body: if (DAT_007f0fd0 != 7))
        std::uint32_t game_mode = *reinterpret_cast<std::uint32_t*>(kGameModeDat);
        if (game_mode != 7u) {
            // (0x00422fd0 body: if (FUN_0040e470(param_1) == 1))
            if (s_CarSlotStateGet(param_1) == 1u) {
                // (0x00422fd0 body: thunk_FUN_00419760(param_1, 1))
                s_thunk_FUN_00419760(param_1, 1);
            }
        }
    }

    return 0u;  // always 0 (cited at 0x00422fd0 body: return 0)
}

RH_ScopedInstall(FrontendRaceResultsDispatch, 0x00422fd0);  // re-enabled 2026-05-24 c3-frontend-a
