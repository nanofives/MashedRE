// Mashed RE - the posted-modal renderer.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// ---------------------------------------------------------------------------
// THIS TU IS ASI-ONLY (asi_sources.rsp). Do not add it to the exe list.
//
// Every callee below is reached at its original .text RVA and the body reads a
// dozen globals in the 0x0067ea..0x0067ec range plus the live RW device at
// DAT_007d3ff8. Calling any of this from mashed_re.exe would tunnel into
// unmapped code, which is the shape that AV'd CarSlotAssign. Same rule as
// Frontend/SetupScreenRenderers.cpp.
// ---------------------------------------------------------------------------
//
// THE PAIR. Two functions implement the modal, and only one of them draws:
//
//   0x0042bf30  the POSTER. Already C3 (Post0042bf30). Writes its six arguments
//               into the request block at 0x0067eab4..0x0067ead0 and raises the
//               flag DAT_0067eab0. It does not touch the screen.
//   0x00433f40  THIS ONE, the RENDERER. Draws whatever is currently posted,
//               once the alpha ramp DAT_0067eab8 has reached 0x28.
//
// DAT_0067eab0 is also the global FUN_0042f7b0 (FrontendCursorUpdate) early-outs
// on, so a posted modal freezes setup-screen input while it is up: one flag doing
// both jobs. See re/analysis/race_hud_capture_20260902.md Finding 32.
//
// NAME. hooks.csv carried this RVA as "RaceEndFadeOverlay", a 2026-05-23
// skeleton-prep guess taken from the fade guards at the top. It is not a race-end
// overlay: it is the generic modal renderer, and its four best-evidenced callers
// are the team-split rejection arms at 0x0043f3e2 / 0x0043f3ff / 0x0043f41c /
// 0x0043f43a, which post body ids 0xd6..0xd9. Renamed here.
//
// Decompilation and disassembly: Ghidra headless against a pool slot plus
// capstone over MASHED.exe.unpatched, both 2026-09-05. Body 0x00433f40..0x00434324
// (276 instructions), jump table at 0x00434328.

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee declarations -- original function pointers
// ---------------------------------------------------------------------------

// 0x004282a0  MenuMenusBA -- measure a string by message id. Returns float10 in
// ST0, which THIS CALLER DISCARDS with `fstp st(0)` at 0x00433fde. Declared
// float and sunk into a volatile below so MSVC emits the matching pop; see the
// call site for why the opposite choice is a bug.
static auto* const s_FUN_004282a0 =
    reinterpret_cast<float(__cdecl*)(std::uint32_t, float)>(0x004282a0u);

// 0x0042aae0  resets the 0x0067ec38.. vertex block and re-arms render state.
// Ghidra labels this __fastcall; it is not. The `push ecx` at 0x0042aae0 is
// MSVC's 4-byte local allocation, the vtable call at 0x0042aaea takes two
// arguments (cleaned by `add esp,8` at 0x0042ab05, not three), and the function
// ends in a plain `ret` at 0x0042abfe. Our caller pushes one argument and cleans
// it in the batched `add esp,0x4c`, so __cdecl(uint32) reproduces the stack.
static auto* const s_FUN_0042aae0 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x0042aae0u);

// 0x004671a0  returns a render-target handle for the given index.
static auto* const s_FUN_004671a0 =
    reinterpret_cast<std::uint32_t(__cdecl*)(int)>(0x004671a0u);

// 0x004c19f0 / 0x004c1a00  dispatch through the handle's vtable (+0x1c / +0x18).
static auto* const s_FUN_004c19f0 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x004c19f0u);
static auto* const s_FUN_004c1a00 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x004c1a00u);

// 0x00492e60  no-arg render-target step.
static auto* const s_FUN_00492e60 =
    reinterpret_cast<void(__cdecl*)()>(0x00492e60u);

// 0x0042c010  scaled filled rect: void(x, y, w, h, colour).
static auto* const s_FUN_0042c010 =
    reinterpret_cast<void(__cdecl*)(float, float, float, float, std::uint32_t)>(0x0042c010u);

// 0x0042c090  the modal's six border lines. Ghidra prints this `void(void)`
// because it reads the colour as `&stack0x00000004`, i.e. by ADDRESS off its own
// frame. That address is the caller's pushed argument slot, so a normal __cdecl
// parameter is exactly right.
static auto* const s_FUN_0042c090 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x0042c090u);

// 0x00427e00  positioned text by message id: void(id, x, y, colour, scale, align).
static auto* const s_FUN_00427e00 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, float, float, std::uint32_t, float, int)>(
        0x00427e00u);

// 0x004278d0  centred body text by message id. THREE arguments, not one.
// Ghidra's own decompilation of 0x00433f40 prints `FUN_004278d0(DAT_0067eab4)`,
// but all three call sites push three dwords and clean 0xc (0x00434170 +
// 0x00434175, 0x0043417b + 0x00434180), and the callee's body consumes
// param_2 as the colour and param_3 as the scale. Taking the decompiler's
// arity here would have dropped the colour and the scale on the floor.
static auto* const s_FUN_004278d0 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, std::uint32_t, float)>(0x004278d0u);

// 0x00427be0  same shape as 0x004278d0 but sources the string from a caller
// buffer instead of the message table (FUN_00427840 in place of the
// 0x00427780 + 0x004277a0 pair).
static auto* const s_FUN_00427be0 =
    reinterpret_cast<void(__cdecl*)(void*, std::uint32_t, float)>(0x00427be0u);

// 0x00427990  boxed text with alignment: void(id, x, y, w, h, colour, scale, align).
static auto* const s_FUN_00427990 =
    reinterpret_cast<void(__cdecl*)(std::uint32_t, float, float, float, float,
                                    std::uint32_t, float, int)>(0x00427990u);

// ---------------------------------------------------------------------------
// Globals (every address cited at the instruction that reads or writes it)
// ---------------------------------------------------------------------------

// The posted request block, filled by 0x0042bf30.
static constexpr std::uintptr_t kModal_Flag     = 0x0067eab0u;  // 0x00433f6a  posted?
static constexpr std::uintptr_t kModal_BodyId   = 0x0067eab4u;  // 0x00433fc2  p1
static constexpr std::uintptr_t kModal_Alpha    = 0x0067eab8u;  // 0x00433f8b  ramp 0..0xff
static constexpr std::uintptr_t kModal_Layout   = 0x0067eac0u;  // 0x004341b8  p3, prompt layout
static constexpr std::uintptr_t kModal_Prompt1  = 0x0067eac8u;  // 0x004341ce  p4
static constexpr std::uintptr_t kModal_Prompt2  = 0x0067eaccu;  // 0x004341eb  p5
static constexpr std::uintptr_t kModal_IdBiasOn = 0x0067ead4u;  // 0x00433fb1  bias enable
static constexpr std::uintptr_t kModal_IdBias   = 0x0067eadcu;  // 0x00433fc8  page counter
static constexpr std::uintptr_t kModal_TextTint = 0x0067eca8u;  // 0x00433ff4  saved+restored
static constexpr std::uintptr_t kRwDevice       = 0x007d3ff8u;  // 0x00433f98

// Geometry. Every one is an integer under 2^24 or the correctly-rounded float of
// its decimal, so these literals carry the original's exact bit patterns.
//   130.0f=0x43020000  120.0f=0x42f00000  380.0f=0x43be0000   42.0f=0x42280000
//   162.0f=0x43220000  166.0f=0x43260000  328.0f=0x43a40000   32.0f=0x42000000
//   140.0f=0x430c0000  142.0f=0x430e0000  345.0f=0x43ac8000  270.0f=0x43870000
//   145.0f=0x43110000  267.0f=0x43858000  350.0f=0x43af0000  150.0f=0x43160000
//     0.6f=0x3f19999a    0.8f=0x3f4ccccd   0.55f=0x3f0ccccd

// Message ids that are literal in the instruction stream.
static constexpr std::uint32_t kMsg_Title      = 0x41u;   // 0x0043413d "MASHED"
static constexpr std::uint32_t kMsg_TitleBias  = 0xb8u;   // 0x00434127 when the bias is on
static constexpr std::uint32_t kMsg_BufferBody = 0x27bu;  // 0x00434150 the buffer-sourced body
static constexpr std::uint32_t kMsg_BufferHdr  = 0x27cu;  // 0x004341ab its boxed header

static inline std::uint32_t Rd32(std::uintptr_t a) {
    return *reinterpret_cast<volatile std::uint32_t*>(a);
}
static inline void Wr32(std::uintptr_t a, std::uint32_t v) {
    *reinterpret_cast<volatile std::uint32_t*>(a) = v;
}

// The two-int render-state entry, (**(code**)(DAT_007d3ff8 + 0x20))(a, b).
static inline void RwState(int a, int b) {
    const std::uint32_t dev = Rd32(kRwDevice);
    reinterpret_cast<void(__cdecl*)(int, int)>(*reinterpret_cast<std::uint32_t*>(dev + 0x20))(a, b);
}

// ---------------------------------------------------------------------------
// MenuModalRender  --  0x00433f40
//
// void FUN_00433f40(void)
//
// Draws the posted modal: three filled bands, a six-line border, a title, a body
// and up to two prompts. All five colours are the SAME rgb triples with the live
// ramp alpha in the top byte, built byte-by-byte on the frame at
// 0x00433f5e..0x0043404f:
//
//   0x{a}dcdcdc  body / prompt text     (frame -0x218)
//   0x{a}ffffff  title text             (frame -0x214)
//   0x{a}000000  bands 1 and 3          (frame -0x210)
//   0x{a}ffffff  border lines           (frame -0x20c)
//   0x{a}202020  band 2                 (frame -0x208)
//
// NOT PORTED, deliberately: the /GS stack cookie (0x00433f46 load,
// 0x00434312 check, tail `jmp 0x4a2be9`). It is the original build's stack
// protection, computes nothing, and draws nothing.
//
// ref: re/analysis/skeleton_prep_render/00433f40.md
//      re/analysis/race_hud_capture_20260902.md Findings 32 and 37
// ---------------------------------------------------------------------------

// 0x00433f40
extern "C" __declspec(dllexport) void __cdecl MenuModalRender()
{
    // 0x00433f6a / 0x00433f85: nothing posted.
    if (Rd32(kModal_Flag) == 0) return;
    // 0x00433f8b / 0x00433f92: signed compare, the ramp has not reached 0x28.
    if (static_cast<std::int32_t>(Rd32(kModal_Alpha)) < 0x28) return;

    RwState(6, 0);                                       // 0x00433fa1
    RwState(8, 0);                                       // 0x00433fae

    // 0x00433fb1..0x00433fd8: the body id, optionally biased by the page counter.
    const std::uint32_t body_id = Rd32(kModal_BodyId);
    const std::uint32_t biased_id =
        (Rd32(kModal_IdBiasOn) != 0) ? (Rd32(kModal_IdBias) + body_id) : body_id;

    // 0x00433fd9 measure, 0x00433fde `fstp st(0)` discards the result. The sink
    // is what makes MSVC emit that pop. Do NOT declare 0x004282a0 void to "avoid
    // the unused value": a void declaration leaves ST0 loaded and leaks the x87
    // stack, which is the NaN-freeze failure recorded in the render lane.
    volatile float measured = s_FUN_004282a0(biased_id, 0.6f);
    (void)measured;

    // 0x00433fe0..0x00434004: darken the text tint for the reset, then restore.
    // The `cdq; and edx,3; add; sar 2` at 0x00433fea is a signed /4; the input is
    // a movzx byte times three, so it is non-negative and this is exactly (a*3)/4.
    const std::uint32_t alpha_b = Rd32(kModal_Alpha) & 0xffu;
    const std::uint32_t saved_tint = Rd32(kModal_TextTint);
    Wr32(kModal_TextTint, (alpha_b * 3u) / 4u);          // 0x00433fff
    s_FUN_0042aae0(0);                                   // 0x00434004
    Wr32(kModal_TextTint, saved_tint);                   // 0x0043401e

    // 0x00434009..0x0043404f. One alpha, five rgb triples.
    const std::uint32_t a24       = alpha_b << 24;
    const std::uint32_t col_body   = a24 | 0xdcdcdcu;    // frame -0x218
    const std::uint32_t col_title  = a24 | 0xffffffu;    // frame -0x214
    const std::uint32_t col_band13 = a24;                // frame -0x210
    const std::uint32_t col_border = a24 | 0xffffffu;    // frame -0x20c
    const std::uint32_t col_band2  = a24 | 0x202020u;    // frame -0x208

    // 0x00434053..0x0043406b: bracket the modal in its own render target.
    s_FUN_004c19f0(s_FUN_004671a0(-1));                  // 0x00434059
    s_FUN_00492e60();                                    // 0x0043405e
    s_FUN_004c1a00(s_FUN_004671a0(-1));                  // 0x0043406b

    RwState(6, 0);                                       // 0x0043407a
    RwState(8, 0);                                       // 0x00434086

    // Three bands, 0x004340a2 / 0x004340c0 / 0x004340dd.
    s_FUN_0042c010(130.0f, 120.0f, 380.0f,  42.0f, col_band13);
    s_FUN_0042c010(130.0f, 162.0f, 380.0f, 166.0f, col_band2);
    s_FUN_0042c010(130.0f, 328.0f, 380.0f,  32.0f, col_band13);
    s_FUN_0042c090(col_border);                          // 0x004340e7

    RwState(6, 0);                                       // 0x004340f5
    RwState(8, 0);                                       // 0x00434102

    // Title, 0x0043413f. The id flips with the same bias flag that biases the body.
    const std::uint32_t title_id = (Rd32(kModal_IdBiasOn) == 0) ? kMsg_Title : kMsg_TitleBias;
    s_FUN_00427e00(title_id, 140.0f, 142.0f, col_title, 0.8f, 0);

    // Body. Re-read at 0x00434144, unbiased, because 0x27b is matched against the
    // RAW id while the draw below re-applies the bias.
    const std::uint32_t raw_body_id = Rd32(kModal_BodyId);
    if (raw_body_id == kMsg_BufferBody) {                // 0x00434150
        // 0x00434185: the buffer is the caller's, 512 bytes, and the original
        // passes it UNINITIALISED -- Ghidra's decompilation of 0x00427be0 shows
        // param_1 unread, the string coming from FUN_00427840 instead.
        // [UNCERTAIN U-9084] whether 0x00427be0 reads through the pointer on some path;
        // transcribed as written rather than "fixed".
        std::uint8_t body_buf[512];
        s_FUN_00427be0(body_buf, col_body, 0.6f);        // 0x0043418a
        s_FUN_00427990(kMsg_BufferHdr, 145.0f, 267.0f, 350.0f, 150.0f,
                       col_body, 0.55f, 0);              // 0x004341b0
    } else if (Rd32(kModal_IdBiasOn) != 0) {             // 0x0043415d
        s_FUN_004278d0(Rd32(kModal_IdBias) + raw_body_id, col_body, 0.6f);  // 0x00434170
    } else {
        s_FUN_004278d0(raw_body_id, col_body, 0.6f);     // 0x0043417b
    }

    // Prompts. Jump table at 0x00434328, indexed by DAT_0067eac0 - 1, bounds
    // `ja 0x4342ee` at 0x004341c1, so 0 and >0xc draw nothing. Read out of the
    // image, not inferred: 3, 4, 5, 6, 8 and 0xa all target the default arm too,
    // which is why the arms below are not a contiguous range.
    //   1 -> 0x00434254 one    2 -> 0x00434277 two    7 -> 0x0043422f one
    //   9 -> 0x004342af two    b -> 0x00434209 one    c -> 0x004341ce two
    // The six reachable arms are two distinct bodies repeated three times each;
    // the original does not share them, but the emitted draws are identical.
    switch (Rd32(kModal_Layout)) {
        case 1: case 7: case 0xb:
            s_FUN_00427e00(Rd32(kModal_Prompt1), 140.0f, 345.0f, col_body, 0.55f, 0);
            break;
        case 2: case 9: case 0xc:
            s_FUN_00427e00(Rd32(kModal_Prompt1), 140.0f, 345.0f, col_body, 0.55f, 0);
            s_FUN_00427e00(Rd32(kModal_Prompt2), 270.0f, 345.0f, col_body, 0.55f, 0);
            break;
        default:
            break;                                       // 0x004342ee
    }

    RwState(6, 1);                                       // 0x004342f7
    RwState(8, 1);                                       // 0x00434304
}

RH_ScopedInstall(MenuModalRender, 0x00433f40);
