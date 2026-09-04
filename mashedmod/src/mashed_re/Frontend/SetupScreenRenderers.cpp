// Mashed RE - the two player-setup screen renderers.
//
//   0x0043a610  AbilitySelectRender  -- nav screen 15 (Ability Select)
//   0x0043aa30  TeamSelectRender     -- nav screen 16 (Team Select)
//
// Both are driven from FUN_0043bf30's flat flag->renderer list, which
// FUN_0043c000 gates per panel: DAT_0067e7d8 (+ alpha DAT_0067e7dc) for
// Ability Select, DAT_0067e7e0 (+ alpha DAT_0067e7e4) for Team Select, the
// same two flags FUN_00431f30 raises for page ids 0xf and 9/0x10. Each
// renderer reads its own alpha byte at entry, which is how the panels fade.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// ASI-ONLY TU: every draw callee is reached through its original .text RVA, and
// the standalone maps only the 0x00420000 / 0x00470000 granules -- calling
// these from mashed_re.exe would tunnel into unmapped code (the CarSlotAssign
// AV of 2026-09-04). The standalone draws these screens with its own
// exe_main.cpp code, transcribed from the same geometry. Do NOT add this file
// to build.bat's exe list.

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

namespace {

// ---------------------------------------------------------------------------
// Callees, all reached at their original RVAs.
// ---------------------------------------------------------------------------

// 0x0042f8d0  MenuMenusBC -- bordered plate. C2-impl.
//
// FIFTH ARGUMENT IN AL, which no decompilation shows. Its prologue reads the
// panel alpha out of the register before touching any stack slot:
//   0x0042f8d0  sub esp, 0xc
//   0x0042f8d9  mov bl, al        ; <-- the alpha, never a stack argument
//   0x0042f8e7  shr cl, 1         ; plate fill gets alpha >> 1
// and every call site in both renderers reloads it first -- 0x0043a67a
// `mov al, [0x67e7dc]` for the first ability plate, then 0x0043a70a /
// 0x0043a733 / 0x0043a75b / 0x0043a7fc from stack copies of the same byte, and
// 0x0043ab53 / 0x0043ab8a / 0x0043ac91 `mov al, bl` on the team side.
//
// A plain C++ call cannot set AL, so the first cut of this port shipped
// whatever AL happened to hold: the hook-on vs hook-off draw-stream A/B came
// back with byte-identical GEOMETRY and 328 colour-only mismatches (plate
// 0x00 / border 0x90 against the original's 0x7f / 0xff). That is what found
// this argument. The trampoline below passes it explicitly.
inline void PlateAl(float x, float y, float w, float h, std::uint8_t alpha) {
    // NOTE the names: bx / bh / bw are x86 REGISTERS, and MSVC's inline
    // assembler resolves the register before the local, which silently
    // assembles `push bh` as a register push (warning C4409) instead of the
    // argument. Hence the arg_ prefix.
    std::uint32_t arg_x, arg_y, arg_w, arg_h;
    std::memcpy(&arg_x, &x, 4);
    std::memcpy(&arg_y, &y, 4);
    std::memcpy(&arg_w, &w, 4);
    std::memcpy(&arg_h, &h, 4);
    const void* const fn = reinterpret_cast<const void*>(0x0042f8d0);
    __asm {
        push arg_h
        push arg_w
        push arg_y
        push arg_x
        mov  ecx, fn
        mov  al,  alpha
        call ecx
        add  esp, 16
    }
}

// 0x0042fab0  SpriteSlotDispatch -- car sprite handle for a Player Colour.
// C3-impl. Declared as RETURNING the handle: the port's own export is void,
// but the original leaves 0x0040bb90's EAX in place and both renderers use it
// as FUN_004739f0's first argument.
using SpriteSlotFn = int*(__cdecl*)(int);
auto* const oSpriteSlot = reinterpret_cast<SpriteSlotFn>(0x0042fab0);

// 0x004739f0  TextSpriteScaled -- textured quad. C3-impl.
using SpriteFn = void(__cdecl*)(int*, float, float, float, float,
                                std::uint32_t, float, float, float, float,
                                int, int);
auto* const oSprite = reinterpret_cast<SpriteFn>(0x004739f0);

// 0x0042bcb0  device/joypad glyph for a profile (int, float, float, argb, int).
using DeviceGlyphFn = void(__cdecl*)(int, float, float, std::uint32_t, int);
auto* const oDeviceGlyph = reinterpret_cast<DeviceGlyphFn>(0x0042bcb0);

// 0x00427e00  sprite/text draw by message id (id, x, y, argb, scale, flags).
using MsgTextFn = void(__cdecl*)(int, float, float, std::uint32_t, float, int);
auto* const oMsgText = reinterpret_cast<MsgTextFn>(0x00427e00);

// 0x00427f00  literal-string draw (str, x, y, argb, scale, flags).
using StrTextFn = void(__cdecl*)(const char*, float, float, std::uint32_t,
                                 float, int);
auto* const oStrText = reinterpret_cast<StrTextFn>(0x00427f00);

// 0x004282a0  MenuMenusBA -- measures a string; the width is left in x87 ST0.
// DELIBERATELY declared void: the original never pops it, and 0x004a2c48 on
// the very next line is what consumes it. Declaring the real `float` return
// here would make the compiler emit an FSTP and hand 0x004a2c48 an empty
// stack -- the ST0-leak shape recorded for the RW math leaves, inverted.
// Nothing that touches the FPU may be inserted between the two calls.
using MeasureFn = void(__cdecl*)(std::uint32_t, float);
auto* const oMeasure = reinterpret_cast<MeasureFn>(0x004282a0);

// 0x004a2c48  round ST0 -> int (and pop). C3-impl.
using RoundSt0Fn = int(__cdecl*)();
auto* const oRoundSt0 = reinterpret_cast<RoundSt0Fn>(0x004a2c48);

// (**(code **)(DAT_007d3ff8 + 0x20))(a, b) -- the RW render-state setter,
// same accessor MenuDrawLoopTwin / MenuSpriteDispatch already use.
inline void vt20(int a, int b) {
    const std::uintptr_t vptr = *reinterpret_cast<std::uintptr_t*>(0x007d3ff8u);
    (*reinterpret_cast<void(__cdecl**)(int, int)>(vptr + 0x20))(a, b);
}

// ---------------------------------------------------------------------------
// Globals.
// ---------------------------------------------------------------------------
constexpr std::uintptr_t kAbilAlpha   = 0x0067e7dc;  // DAT_0067e7dc
constexpr std::uintptr_t kTeamAlpha   = 0x0067e7e4;  // DAT_0067e7e4
constexpr std::uintptr_t kAbilTable   = 0x0067e850;  // DAT_0067e850, stride 12
constexpr std::uintptr_t kAbilEnd     = 0x0067e8e0;  // outer bound
constexpr std::uintptr_t kTeamTable   = 0x0067e938;  // DAT_0067e938, stride 12
constexpr std::uintptr_t kTeamEnd     = 0x0067e9c7;  // outer bound (inclusive cmp)
constexpr std::uintptr_t kSlotColour  = 0x007f1a1c;  // slot +0x08; [-2] = profile
constexpr std::uintptr_t kSlotEnd     = 0x007f1a5c;
constexpr std::uintptr_t kSecondBank  = 0x007f0fe8;  // DAT_007f0fe8
constexpr std::uintptr_t kAbilNumGate = 0x0067e897;  // profile >= 6 (0x0043a???)
constexpr std::uintptr_t kTeamNumGate = 0x0067e97f;  // profile >= 6
constexpr std::uintptr_t kTeamZero    = 0x0067ea7c;  // DAT_0067ea7c
constexpr std::uintptr_t kTeamOne     = 0x008990dc;  // DAT_008990dc

// Float constants, read out of .rdata of the anchored binary.
constexpr float kBorder2   = 2.0f;    // _DAT_005cc574
constexpr float kGlyphUp   = 15.0f;   // _DAT_005cc9b0
constexpr float kSpriteUp  = 8.0f;    // _DAT_005cc9f4
constexpr float kRowPitch  = 40.0f;   // _DAT_005cd274
constexpr float kAbilX0    = 64.0f;   // _DAT_005cd6d4
constexpr float kTeamX0    = 82.0f;   // _DAT_005cd95c
constexpr float kTextX     = 65.0f;   // _DAT_005cda6c
constexpr float kTeamGlyphX = 70.0f;  // _DAT_005ccd0c
constexpr float kNumNudge  = 4.0f;    // _DAT_005cc35c

template <typename T>
inline T& At(std::uintptr_t a) { return *reinterpret_cast<T*>(a); }

inline std::uint32_t Load4(const std::uint8_t* p) {
    std::uint32_t v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

// The row number char, shared shape: '1' + profile, minus 6 for the second
// bank of six profiles when DAT_007f0fe8 is set. `gate` is the pointer
// threshold the original compares the OUTER table cursor against.
inline void RowNumber(char out[2], int profile, std::uintptr_t cursor,
                      std::uintptr_t gate) {
    char c = static_cast<char>(profile);
    if (At<std::int32_t>(kSecondBank) != 0 &&
        static_cast<int>(gate) < static_cast<int>(cursor)) {
        c = static_cast<char>(profile) + -6;
    }
    out[0] = static_cast<char>(c + '1');
    out[1] = 0;
}

}  // namespace

// ---------------------------------------------------------------------------
// AbilitySelectRender  --  0x0043a610
//
// Original: FUN_0043a610 (1050 bytes). void(void).
//
// Four header plates at x = 65 + c * 130, w 120, y 140, h 28, captioned by
// message ids 0xe3 / 0xd2 / 0xd3 / 0xd4 centred at 125 / 255 / 385 / 515.
// Then ONE ROW PER ASSIGNED CAR SLOT in PROFILE order: the outer loop walks
// the 12-entry ability table (stride 12, bound 0x67e8e0), the inner loop the
// four slot records (stride 0x10, bound 0x7f1a5c), and a row is drawn only
// where the slot's profile field equals the outer profile -- so an unassigned
// slot (-1) contributes nothing and the Y cursor advances only on a drawn row.
//
// RESOLVES the [UNCERTAIN] on the row plate's height. The 4th argument is
// uVar4, which Ghidra shows clobbered by the FUN_0042fab0 call on the next
// line -- but the bottom of every drawn row restores `uVar4 = local_24`, and
// local_24 is set to 0x41e00000 at entry and never written again. So the
// height is 28.0 on every row, read off the listing rather than assumed by
// analogy with the headers.
//
// Per row: plate, car sprite for the slot's Player Colour at
// x = ability * 130 + 64 (the slide constant IS the header pitch, so the
// sprite lands under the chosen column), the profile's device glyph, and the
// player number measured through the ST0 pair.
// ---------------------------------------------------------------------------

// 0x0043a610
extern "C" __declspec(dllexport) void __cdecl AbilitySelectRender() {
    const std::uint8_t bVar2 = At<std::uint8_t>(kAbilAlpha);

    // Six 4-byte colour blocks on the stack; the original's local_18[] is a
    // 4-entry array immediately followed by local_8 and local_4, i.e. a
    // 6-entry palette indexed by the slot's Player Colour, with index 5 also
    // being the `== 5` case the device glyph is told about.
    std::uint8_t c0[4] = { 0x98, 0x3a, 0x3d, bVar2 };
    std::uint8_t c1[4] = { 0x4e, 0x89, 0xae, bVar2 };
    std::uint8_t c2[4] = { 0x61, 0x76, 0x56, bVar2 };
    std::uint8_t c3[4] = { 0xdb, 0xc3, 0x62, bVar2 };
    std::uint8_t c4[4] = { 0xeb, 0xa7, 0xa7, bVar2 };
    std::uint8_t c5[4] = { 0xff, 0xff, 0xff, bVar2 };
    const std::uint8_t* const pal[6] = { c0, c1, c2, c3, c4, c5 };

    float local_24 = 28.0f;                   // 0x41e00000
    float uVar4    = local_24;                // the plate height, see above
    float local_28 = 120.0f;                  // 0x42f00000
    float local_34 = 140.0f;
    float local_38 = 65.0f;                   // 0x42820000

    PlateAl(65.0f,  140.0f, 120.0f, 28.0f, bVar2);
    local_38 = 195.0f;                        // 0x43430000
    PlateAl(195.0f, 140.0f, 120.0f, 28.0f, bVar2);
    local_38 = 325.0f;                        // 0x43a28000
    PlateAl(325.0f, 140.0f, 120.0f, 28.0f, bVar2);
    local_38 = 455.0f;                        // 0x43e38000
    PlateAl(455.0f, 140.0f, 120.0f, 28.0f, bVar2);

    local_38 = 60.0f;                         // 0x42700000  row plate x
    local_34 = 200.0f;                        // first row y
    local_28 = 523.0f;                        // 0x4402c000  row plate w
    const float local_20 = 48.0f;             // sprite w
    const float local_1c = 48.0f;             // sprite h

    const std::uint32_t local_50 =            // CONCAT13(bVar2, 0xffffff)
        (static_cast<std::uint32_t>(bVar2) << 24) | 0x00ffffffu;
    const std::uint32_t local_6c =            // (uint)bVar2 << 0x18
        static_cast<std::uint32_t>(bVar2) << 24;

    vt20(6, 0);
    vt20(8, 0);

    int iStack_70 = 0;                        // profile index
    std::uintptr_t piStack_44 = kAbilTable;   // outer cursor
    do {
        std::uintptr_t piVar6 = kSlotColour;  // slot record +0x08
        std::uintptr_t piVar7 = piStack_44;
        do {
            if (At<std::int32_t>(piVar6 - 8) == iStack_70) {
                PlateAl(local_38, local_34, local_28, uVar4, bVar2);

                int* const sprite = oSpriteSlot(At<std::int32_t>(piVar6));

                const float fStack_40 =
                    static_cast<float>(At<std::int32_t>(piVar7) * 0x82) + kAbilX0;
                const float fStack_3c = local_34 - kBorder2;
                const float fVar1     = fStack_3c + kGlyphUp;
                const float fStack_30 = fStack_40;

                oDeviceGlyph(At<std::int32_t>(piVar6 - 8),
                             fStack_40 + kTextX, fVar1,
                             Load4(pal[At<std::int32_t>(piVar6)]),
                             At<std::int32_t>(piVar6) == 5);

                // ST0 pair -- nothing may go between these two calls.
                oMeasure(0x36, 0.9f);         // 0x3f666666
                const int iVar5 = oRoundSt0();
                const float fStack_48 = static_cast<float>(iVar5 / 2);

                char acStack_74[2];
                RowNumber(acStack_74, iStack_70, piStack_44, kAbilNumGate);

                oStrText(acStack_74,
                         static_cast<float>(static_cast<int>(fStack_48)) +
                             fStack_30 + kTextX,
                         fVar1, local_6c, 0.6f /*0x3f19999a*/, 0);

                const float fStack_2c = fStack_3c - kSpriteUp;
                oSprite(sprite, fStack_40, fStack_2c, local_20, local_1c,
                        local_50, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);

                local_34 = local_34 + kRowPitch;
                uVar4    = local_24;
                piVar7   = piStack_44;
            }
            piVar6 += 0x10;
        } while (static_cast<int>(piVar6) < static_cast<int>(kSlotEnd));
        piStack_44 = piVar7 + 12;
        iStack_70  = iStack_70 + 1;
    } while (static_cast<int>(piStack_44) < static_cast<int>(kAbilEnd));

    // uStack_4c holds only its top byte (CONCAT13 of the alpha over an
    // undefined3) and is then masked with 0xff000000, so the defined value is
    // exactly alpha << 24; the original's undefined low bytes cannot survive
    // the mask.
    const std::uint32_t uStack_4c = static_cast<std::uint32_t>(bVar2) << 24;
    oMsgText(0xe3, 125.0f, 156.0f, uStack_4c, 0.75f, 2);
    oMsgText(0xd2, 255.0f, 156.0f, uStack_4c, 0.75f, 2);
    oMsgText(0xd3, 385.0f, 156.0f, uStack_4c, 0.75f, 2);
    oMsgText(0xd4, 515.0f, 156.0f, uStack_4c, 0.75f, 2);

    vt20(6, 1);
    vt20(8, 1);
}

RH_ScopedInstall(AbilitySelectRender, 0x0043a610);

// ---------------------------------------------------------------------------
// TeamSelectRender  --  0x0043aa30
//
// Original: FUN_0043aa30 (1187 bytes). void(void).
//
// Two header plates at x 250 / 430, w 140, y 140, h 28, captioned 0x9f / 0xa0
// centred at 320 / 500, then the same profile-major row loop as Ability
// Select -- outer over the 12-entry team table (stride 12, bound 0x67e9c7),
// inner over the four slot records -- with row plate x 70 w 510 and the car
// sprite sliding to x = team * 180 + 82.
//
// The roster stacks are the second sprite: a row whose team pick is 1 or 2
// also draws the same car at y 108, x = 250 + n * 34 (team 1) or 430 + n * 34
// (team 2), where n is that team's running count. Both counters are
// re-seeded from iStack_3c / iStack_40 at the top of every OUTER iteration,
// which is transcribed as written.
// ---------------------------------------------------------------------------

// 0x0043aa30
extern "C" __declspec(dllexport) void __cdecl TeamSelectRender() {
    const std::uint8_t bVar2 = At<std::uint8_t>(kTeamAlpha);

    std::uint8_t c0[4] = { 0x98, 0x3a, 0x3d, bVar2 };
    std::uint8_t c1[4] = { 0x4e, 0x89, 0xae, bVar2 };
    std::uint8_t c2[4] = { 0x61, 0x76, 0x56, bVar2 };
    std::uint8_t c3[4] = { 0xdb, 0xc3, 0x62, bVar2 };
    std::uint8_t c4[4] = { 0xeb, 0xa7, 0xa7, bVar2 };
    // Index 5 is local_78 itself -- the text colour dword, not a sixth
    // palette entry. The original's local_4 points at it.
    const std::uint32_t local_78 = static_cast<std::uint32_t>(bVar2) << 24;
    const std::uint8_t* const pal[6] = {
        c0, c1, c2, c3, c4,
        reinterpret_cast<const std::uint8_t*>(&local_78)
    };

    At<std::int32_t>(kTeamZero) = 0;          // DAT_0067ea7c
    At<std::int32_t>(kTeamOne)  = 1;          // DAT_008990dc

    vt20(6, 0);
    vt20(8, 0);

    float uStack_2c = 28.0f;                  // plate h
    float uStack_30 = 140.0f;                 // plate w
    float fStack_70 = 140.0f;                 // plate y
    float uStack_74 = 250.0f;                 // 0x437a0000

    PlateAl(250.0f, 140.0f, 140.0f, 28.0f, bVar2);
    oMsgText(0x9f, 320.0f, 154.0f, local_78, 0.8f /*0x3f4ccccd*/, 2);
    uStack_74 = 430.0f;                       // 0x43d70000
    PlateAl(430.0f, 140.0f, 140.0f, 28.0f, bVar2);
    oMsgText(0xa0, 500.0f, 154.0f, local_78, 0.8f, 2);

    uStack_74 = 70.0f;                        // 0x428c0000  row plate x
    fStack_70 = 200.0f;                       // first row y
    uStack_30 = 510.0f;                       // 0x43ff0000  row plate w
    const float uStack_20 = 48.0f;
    const float uStack_1c = 48.0f;

    const std::uint32_t uStack_6c =
        (static_cast<std::uint32_t>(bVar2) << 24) | 0x00ffffffu;
    const std::uint32_t iStack_68 = static_cast<std::uint32_t>(bVar2) << 24;

    vt20(6, 0);
    vt20(8, 0);

    int iStack_7c = 0;                        // profile index
    int iStack_3c = 0;                        // team-2 roster count
    int iStack_40 = 0;                        // team-1 roster count
    float fStack_28 = 0.0f;                   // roster x, persists across rows
    float uStack_24 = 0.0f;                   // roster y, ditto
    std::uintptr_t piStack_34 = kTeamTable;
    for (;;) {
        int iStack_44 = iStack_3c * 0x22 + 0x1ae;   // team 2: 430 + n*34
        int iStack_48 = iStack_40 * 0x22 + 0xfa;    // team 1: 250 + n*34

        std::uintptr_t piVar5 = kSlotColour;
        std::uintptr_t piVar6 = piStack_34;
        do {
            if (At<std::int32_t>(piVar5 - 8) == iStack_7c) {
                PlateAl(uStack_74, fStack_70, uStack_30, uStack_2c, bVar2);

                int* const sprite = oSpriteSlot(At<std::int32_t>(piVar5));

                int iVar4 = At<std::int32_t>(piVar6);       // team pick
                float fStack_64 =
                    static_cast<float>(iVar4 * 0xb4) + kTeamX0;
                float fStack_80 = fStack_70 - kBorder2;
                float fStack_60 = fStack_80 - kSpriteUp;
                const float fStack_38_row = fStack_64;

                oSprite(sprite, fStack_64, fStack_60, uStack_20, uStack_1c,
                        uStack_6c, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);

                fStack_64 = fStack_38_row;
                fStack_60 = fStack_80;

                if (iVar4 != 0) {
                    if (iVar4 == 1) {
                        iStack_40 = iStack_40 + 1;
                        iVar4     = iStack_48;
                        iStack_48 = iStack_48 + 0x22;
                        fStack_28 = static_cast<float>(iVar4);
                        uStack_24 = 108.0f;                // 0x42d80000
                    } else if (iVar4 == 2) {
                        iStack_3c = iStack_3c + 1;
                        iVar4     = iStack_44;
                        iStack_44 = iStack_44 + 0x22;
                        fStack_28 = static_cast<float>(iVar4);
                        uStack_24 = 108.0f;
                    }
                    oSprite(sprite, fStack_28, uStack_24, uStack_20, uStack_1c,
                            uStack_6c, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
                }

                const float fVar1 = fStack_60 + kGlyphUp;
                fStack_80 = fVar1;

                oDeviceGlyph(At<std::int32_t>(piVar5 - 8),
                             fStack_64 + kTeamGlyphX, fVar1,
                             Load4(pal[At<std::int32_t>(piVar5)]),
                             At<std::int32_t>(piVar5) == 5);

                // ST0 pair -- nothing may go between these two calls.
                oMeasure(0x36, 0.9f);
                const int iRound = oRoundSt0();
                const float fStack_38 = static_cast<float>(iRound / 2);

                char acStack_84[2];
                RowNumber(acStack_84, iStack_7c, piStack_34, kTeamNumGate);

                oStrText(acStack_84,
                         static_cast<float>(static_cast<int>(fStack_38)) +
                             kNumNudge + fStack_64 + kTextX,
                         fVar1, iStack_68, 0.6f, 0);

                fStack_70 = fStack_70 + kRowPitch;
                piVar6    = piStack_34;
            }
            piVar5 += 0x10;
        } while (static_cast<int>(piVar5) < static_cast<int>(kSlotEnd));

        piStack_34 = piVar6 + 12;
        iStack_7c  = iStack_7c + 1;
        if (static_cast<int>(kTeamEnd) < static_cast<int>(piStack_34)) {
            vt20(6, 1);
            vt20(8, 1);
            return;
        }
    }
}

RH_ScopedInstall(TeamSelectRender, 0x0043aa30);
