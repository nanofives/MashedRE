// ===========================================================================
// HudStandingsRowUpdate.cpp — 0x0041c410  FUN_0041c410
//
// The STANDINGS PER-ROW UPDATE. One call per standings widget group per frame,
// dispatched by HudSlotUpdateCc50 0x0041cc50 (HudDispatch.cpp) with the group
// pointer in ESI. This is the function that turns the five score-flag arrays
// into a row's enable word, places the row on screen, scales its score bar and
// pulses its crown.
//
// Evidence: re/analysis/race_hud_capture_20260902.md Finding 20 (semantics) and
// re/analysis/bucket_gameplay_0041a980_0041d910/0041c410.md (the C2 plate:
// body 0x0041c410..0x0041c91d, 1293 bytes). Decompilation and disassembly
// re-pulled 2026-09-03 against the anchored binary before writing this.
//
// Calling convention: `this` in ESI, no stack args, no return
// (`void FUN_0041c410(void)` with `unaff_ESI`). The original call site emits
//   MOV ESI,<entry> ; CALL 0x0041c410
// so the installed export must be naked and read ESI. ESI/EDI are callee-saved
// in this ABI; the original pushes EDI at 0x0041c420 and pops it at 0x0041c89f.
//
// Callees, all at their original VAs so they route through our own hooks
// exactly as the game's do:
//   0x0040b420  FUN_0040b420   C4  score-delta classifier, DAT_008a9500[type]
//   0x004c51a0  RwMatrixTranslate      C1
//   0x004c1480  frame-transform apply  C1
//   0x004c13e0  frame-scale apply      C1  (x3 sites, 5 calls)
//   0x0040b6d0  FUN_0040b6d0   C1  score getter, DAT_008a94e0[type]
//   0x0040b890  FUN_0040b890   C2  bar max, returns 8 or 12
// ===========================================================================

#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

// ---- image addresses, all read rather than baked -------------------------
// The five x87 constants were verified byte-for-byte out of the anchored
// MASHED.exe on 2026-09-03 (pefile + struct.unpack, not a decimal gloss --
// see memory plate-hex-gloss-authoritative):
//   0x005cd118  00 00 90 3f  = 1.125f          root scale   (_DAT_005cd118)
//   0x005cc94c  00 00 80 4f  = 4294967296.0f   2^32 wrap    (_DAT_005cc94c)
//   0x005cc9c0  cd cc 4c 3e  = 0.20000000298f  pulse freq   (_DAT_005cc9c0)
//   0x005cc8f0  9a 99 19 3e  = 0.15000000596f  pulse ampl   (_DAT_005cc8f0)
//   0x005cc320  00 00 80 3f  = 1.0f            pulse DC     (_DAT_005cc320)
constexpr std::uintptr_t kRootScale_005cd118 = 0x005cd118u;
constexpr std::uintptr_t kWrap2p32_005cc94c  = 0x005cc94cu;
constexpr std::uintptr_t kPulseFreq_005cc9c0 = 0x005cc9c0u;
constexpr std::uintptr_t kPulseAmpl_005cc8f0 = 0x005cc8f0u;
constexpr std::uintptr_t kPulseDc_005cc320   = 0x005cc320u;

// Per-type flag arrays filled by HudSlotUpdateCc50's five calls, in the order
// FUN_0041c410 folds them into the enable word.
constexpr std::uintptr_t kCrownFlags_0063cde8   = 0x0063cde8u;  // -> bit 0x40
constexpr std::uintptr_t kTiedFlags_0063ce08    = 0x0063ce08u;  // -> bit 0x80
constexpr std::uintptr_t kLowestFlags_0063cdd4  = 0x0063cdd4u;  // -> bit 0x100
constexpr std::uintptr_t kZeroFlags_0063cdc0    = 0x0063cdc0u;  // -> bit 0x200
constexpr std::uintptr_t kRowOrder_0063cdf8     = 0x0063cdf8u;  // 4-entry order
constexpr std::uintptr_t kRowTranslate_005f337c = 0x005f337cu;  // 4 x vec3
constexpr std::uintptr_t kPulseTick_0063d270    = 0x0063d270u;  // signed tick

// ---- callee thunks ------------------------------------------------------
typedef std::int32_t (__cdecl *DeltaClassFn)(std::int32_t type);
typedef void         (__cdecl *MatTranslateFn)(float* m, const float* v, std::int32_t combine);
typedef std::int32_t (__cdecl *FrameApplyFn)(std::int32_t frame, const float* v, std::int32_t mode);
typedef std::int32_t (__cdecl *ScoreGetFn)(std::int32_t type);
typedef std::int32_t (__cdecl *BarMaxFn)(void);

inline float ImageFloat(std::uintptr_t at) {
    return *reinterpret_cast<const float*>(at);
}

}  // namespace

// ---------------------------------------------------------------------------
// The body. Takes the group pointer as a normal __cdecl argument; the naked
// export below converts the original's ESI convention into this call.
//
// External linkage on purpose: it is referenced only from inline asm, and a
// static would be eligible for removal.
// ---------------------------------------------------------------------------
extern "C" void __cdecl HudStandingsRowUpdate_Body(std::uint8_t* self) {
    // Local names track the decompilation: iVar3/iVar4/uVar1/uVar2, and the
    // scale triple local_4c/local_48/local_44 (ascending addresses, so the
    // pointer handed to the apply calls is &scale[0]).
    float scale[3];
    scale[0] = 1.0f;   // local_4c
    scale[1] = 1.0f;   // local_48
    scale[2] = 1.0f;   // local_44

    std::uint32_t* const enable = reinterpret_cast<std::uint32_t*>(self + 0x10c);
    const std::int32_t type = *reinterpret_cast<const std::int32_t*>(self + 0x108);

    // ---- 1. four visibility-bit phases, each a read-modify-write ----------
    // Verbatim: the original stores the enable word back after EVERY phase
    // (0x0041c44d / 0x0041c46b / ... ), it does not accumulate in a register.
    *enable = (*reinterpret_cast<const std::int32_t*>(kCrownFlags_0063cde8 + type * 4) == 0)
                  ? (*enable & 0xffffffbfu) : (*enable | 0x40u);
    *enable = (*reinterpret_cast<const std::int32_t*>(kTiedFlags_0063ce08 + type * 4) == 0)
                  ? (*enable & 0xffffff7fu) : (*enable | 0x80u);
    *enable = (*reinterpret_cast<const std::int32_t*>(kLowestFlags_0063cdd4 + type * 4) == 0)
                  ? (*enable & 0xfffffeffu) : (*enable | 0x100u);
    std::uint32_t uVar1 =
        (*reinterpret_cast<const std::int32_t*>(kZeroFlags_0063cdc0 + type * 4) == 0)
            ? (*enable & 0xfffffdffu) : (*enable | 0x200u);
    *enable = uVar1;

    // ---- 2. two mutual-exclusion cleanups ---------------------------------
    // The first test is `(char)uVar1 < '\0'`, i.e. bit 0x80 of the LOW BYTE,
    // not a sign test on the whole word.
    if (static_cast<std::int8_t>(uVar1) < 0 && (uVar1 & 0x100u) != 0)
        *enable = uVar1 & 0xfffffe7fu;      // clear {0x80, 0x100}
    uVar1 = *enable;
    if ((uVar1 & 0x40u) != 0 && (uVar1 & 0x100u) != 0)
        *enable = uVar1 & 0xfffffeffu;      // clear 0x100

    // ---- 3. score-delta band 0x1f000 --------------------------------------
    const std::int32_t uVar2 =
        reinterpret_cast<DeltaClassFn>(0x0040b420u)(type);
    uVar1 = *enable & 0xfffe0fffu;
    *enable = uVar1;
    switch (uVar2) {
        case  1: uVar1 |= 0x2000u;  break;
        case  2: uVar1 |= 0x1000u;  break;
        case -2: uVar1 |= 0x10000u; break;
        case -1: uVar1 |= 0x8000u;  break;
        default: uVar1 |= 0x4000u;  break;
    }
    *enable = uVar1;

    // ---- 4. 32-slot per-atomic visibility switch --------------------------
    // NOTE the stored value is the MASKED BIT, not a normalised 0/1, for the
    // single-bit cases (1,2,4,5,7..0xc) -- e.g. case 1 stores 0x800 or 0. Only
    // the compound cases store literal 1. Slots 0xd, 0xe and 0x1a..0x1f fall
    // through the switch and store 0. A slot with a null handle at +0x80+i*4
    // is skipped entirely (its result dword is left alone, not zeroed).
    for (std::int32_t iVar3 = 0; iVar3 < 0x20; ++iVar3) {
        std::uint32_t v = 0;
        if (*reinterpret_cast<const std::int32_t*>(self + 0x80 + iVar3 * 4) == 0)
            continue;
        const std::uint32_t e = *enable;
        switch (iVar3) {
            case 0x00: v = 1; break;
            case 0x01:
            case 0x02: v = e & 0x800u; break;
            case 0x03: v = ((e & 0x40u) != 0 && (e & 0x800u) != 0) ? 1u : 0u; break;
            case 0x04:
            case 0x05: v = e & 0x400u; break;
            case 0x06: v = ((e & 0x40u) != 0 && (e & 0x400u) != 0) ? 1u : 0u; break;
            case 0x07: v = e & 0x1u;  break;
            case 0x08: v = e & 0x2u;  break;
            case 0x09: v = e & 0x4u;  break;
            case 0x0a: v = e & 0x8u;  break;
            case 0x0b: v = e & 0x10u; break;
            case 0x0c: v = e & 0x20u; break;
            case 0x0f: v = ((e & 0x4000u)  != 0 && (e & 0x800u) != 0) ? 1u : 0u; break;
            case 0x10: v = ((e & 0x2000u)  != 0 && (e & 0x800u) != 0) ? 1u : 0u; break;
            case 0x11: v = ((e & 0x1000u)  != 0 && (e & 0x800u) != 0) ? 1u : 0u; break;
            // byte-1 sign test: `-1 < (char)(e >> 8)` fails when bit 0x8000 is
            // set, so this is the -1 delta arm expressed as a sign test.
            case 0x12: v = (static_cast<std::int8_t>(e >> 8) < 0 && (e & 0x800u) != 0) ? 1u : 0u; break;
            case 0x13: v = ((e & 0x10000u) != 0 && (e & 0x800u) != 0) ? 1u : 0u; break;
            case 0x14: v = ((e & 0x4000u)  != 0 && (e & 0x400u) != 0) ? 1u : 0u; break;
            case 0x15: v = ((e & 0x2000u)  != 0 && (e & 0x400u) != 0) ? 1u : 0u; break;
            case 0x16: v = ((e & 0x1000u)  != 0 && (e & 0x400u) != 0) ? 1u : 0u; break;
            case 0x17: v = (static_cast<std::int8_t>(e >> 8) < 0 && (e & 0x400u) != 0) ? 1u : 0u; break;
            // case 0x18 FALLS THROUGH into case 0x19 in the original when its
            // test passes (no `break` at 0x0041c710); when it fails it jumps to
            // LAB_0041c6ea with v = 0. Both arms are spelled out here.
            case 0x18: v = ((e & 0x10000u) != 0 && (e & 0x400u) != 0) ? 1u : 0u; break;
            case 0x19: v = 1; break;
            default:   v = 0; break;
        }
        *reinterpret_cast<std::uint32_t*>(self + iVar3 * 4) = v;
    }

    // ---- 5. row lookup in the descending-score order ----------------------
    // EDI is zeroed at 0x0041c421 and only written on a hit, so a type absent
    // from the order array places the row at index 0.
    std::int32_t iVar4 = 0;
    for (std::int32_t i = 0; i < 4; ++i) {
        if (*reinterpret_cast<const std::int32_t*>(self + 0x108) ==
            reinterpret_cast<const std::int32_t*>(kRowOrder_0063cdf8)[i]) {
            iVar4 = i;
            break;
        }
    }

    // ---- 6. identity matrix, translated to the row, applied to the frame ---
    // The matrix is a 16-float RwMatrix on the stack. The original writes only
    // the twelve basis/position floats and the flags word; matrix+0x1c, +0x2c
    // and +0x3c are LEFT UNWRITTEN, and the flags word at +0x0c is a
    // read-modify-write of uninitialised stack (0x0041c756 `mov edx,[esp+0x24]`
    // / 0x0041c75a `or edx,0x20003` / 0x0041c76c stores back to the same slot).
    //
    // [UNCERTAIN U-9081] That OR reads whatever the previous call left on the
    // stack, so the flags word's bits outside 0x20003 are frame-history
    // dependent in the ORIGINAL too -- this is not an artefact of the port. It
    // is also unobservable through this call path: RwMatrixTranslate 0x004c51a0
    // with combine == 0 rewrites all twelve floats and performs the same
    // `| 0x20003` followed by `& 0xfffdffff` itself, so the caller's OR cannot
    // change the value the callee produces. Kept verbatim rather than
    // zero-initialised so the port does not silently define what the original
    // leaves undefined. Any diff of the resulting frame matrix must compare the
    // twelve floats and the low byte of flags, not the whole flags word.
    float m[16];
    reinterpret_cast<std::uint32_t*>(m)[3] |= 0x20003u;   // flags |= 0x20003
    m[0]  = 1.0f;  m[1]  = 0.0f;  m[2]  = 0.0f;           // right
    m[4]  = 0.0f;  m[5]  = 1.0f;  m[6]  = 0.0f;           // up
    m[8]  = 0.0f;  m[9]  = 0.0f;  m[10] = 1.0f;           // at
    m[12] = 0.0f;  m[13] = 0.0f;  m[14] = 0.0f;           // pos

    // `&DAT_005f337c + iVar4 * 3` is float-typed pointer arithmetic == a 12-byte
    // stride (0x0041c760 `lea eax,[edi+edi*2]` / 0x0041c765 `lea ecx,[eax*4+0x5f337c]`).
    const float* const row_v =
        reinterpret_cast<const float*>(kRowTranslate_005f337c) + iVar4 * 3;
    reinterpret_cast<MatTranslateFn>(0x004c51a0u)(m, row_v, 0);
    reinterpret_cast<FrameApplyFn>(0x004c1480u)(
        *reinterpret_cast<const std::int32_t*>(self + 0x104), m, 0);

    // ---- 7. root scale 1.125 on the group frame ---------------------------
    const float root = ImageFloat(kRootScale_005cd118);
    scale[0] *= root;
    scale[1] *= root;
    scale[2] *= root;
    reinterpret_cast<FrameApplyFn>(0x004c13e0u)(
        *reinterpret_cast<const std::int32_t*>(self + 0x104), scale, 1);

    // ---- 8. score-bar fill: X scale = score / max -------------------------
    const std::int32_t score =
        reinterpret_cast<ScoreGetFn>(0x0040b6d0u)(
            *reinterpret_cast<const std::int32_t*>(self + 0x108));
    const float fVar6 = static_cast<float>(score);
    const std::int32_t maxpts = reinterpret_cast<BarMaxFn>(0x0040b890u)();
    scale[0] = fVar6 / static_cast<float>(maxpts);
    scale[2] = 1.0f;
    scale[1] = 1.0f;
    reinterpret_cast<FrameApplyFn>(0x004c13e0u)(
        *reinterpret_cast<const std::int32_t*>(
            *reinterpret_cast<const std::int32_t*>(self + 0x94) + 4),
        scale, 0);
    reinterpret_cast<FrameApplyFn>(0x004c13e0u)(
        *reinterpret_cast<const std::int32_t*>(
            *reinterpret_cast<const std::int32_t*>(self + 0x88) + 4),
        scale, 0);

    // ---- 9. crown pulse ---------------------------------------------------
    // pulse = sin(tick * 0.2) * 0.15 + 1.0, with tick = DAT_0063d270 read as a
    // SIGNED int and wrapped by +2^32 when negative.
    //
    // Done in inline x87 rather than C++ on purpose. The original computes the
    // whole chain in 80-bit ST0 (fild / fadd / fmul / fsin / fmul / fadd) and
    // only narrows on the final store, so evaluating it as float or double
    // would round at different points and can differ in the last bit of the
    // stored float -- which is exactly what a bit-identity diff measures. This
    // also keeps the x87 stack balanced inside one asm block: the
    // ST0-leak-through-a-void-function-pointer failure mode
    // (memory x87-st0-float10-fnptr-void-leak) cost a session before.
    if ((*(self + 0x10c) & 0x40u) == 0) {
        scale[1] = 1.0f;
        scale[0] = 1.0f;
    } else {
        const std::int32_t* const tick =
            reinterpret_cast<const std::int32_t*>(kPulseTick_0063d270);
        const float* const c_wrap = reinterpret_cast<const float*>(kWrap2p32_005cc94c);
        const float* const c_freq = reinterpret_cast<const float*>(kPulseFreq_005cc9c0);
        const float* const c_ampl = reinterpret_cast<const float*>(kPulseAmpl_005cc8f0);
        const float* const c_dc   = reinterpret_cast<const float*>(kPulseDc_005cc320);
        float pulse;
        __asm {
            mov  eax, tick
            mov  edx, dword ptr [eax]
            fild dword ptr [eax]                 // 0x0041c8a8
            test edx, edx                        // 0x0041c8ae
            jge  hsru_no_wrap                    // 0x0041c8b0
            mov  ecx, c_wrap
            fadd dword ptr [ecx]                 // 0x0041c8b2  += 2^32
        hsru_no_wrap:
            mov  ecx, c_freq
            fmul dword ptr [ecx]                 // 0x0041c8b8  *= 0.2
            fsin                                 // 0x0041c8be
            mov  ecx, c_ampl
            fmul dword ptr [ecx]                 // 0x0041c8c0  *= 0.15
            mov  ecx, c_dc
            fadd dword ptr [ecx]                 // 0x0041c8c6  += 1.0
            fstp pulse                           // 0x0041c8cc/d0 (fst + fstp, one value)
        }
        scale[1] = pulse;
        scale[0] = pulse;
    }
    scale[2] = 1.0f;
    reinterpret_cast<FrameApplyFn>(0x004c13e0u)(
        *reinterpret_cast<const std::int32_t*>(
            *reinterpret_cast<const std::int32_t*>(self + 0x98) + 4),
        scale, 0);
    reinterpret_cast<FrameApplyFn>(0x004c13e0u)(
        *reinterpret_cast<const std::int32_t*>(
            *reinterpret_cast<const std::int32_t*>(self + 0x8c) + 4),
        scale, 0);
    // (The decompilation shows two extra trailing args on the +0x98 call --
    // U-7928. The disassembly at 0x0041c8ec..0x0041c8ff pushes exactly three,
    // so that was a vararg/stack artefact and is not reproduced.)
}

// 0x0041c410 — naked entry: converts the original's ESI-this into the body's
// __cdecl argument. No stack args to clean; the original's own `ret` is bare.
extern "C" __declspec(dllexport) __declspec(naked) void HudStandingsRowUpdate() {
    __asm {
        push esi
        call HudStandingsRowUpdate_Body
        add  esp, 4
        ret
    }
}

// C3 2026-09-03. Installed only after path1 came back GREEN 8/8 non-degenerate
// (log/diff_hud_standings_row_update.csv, arg_type esi_row_update_rw): every
// dword this function WRITES is bit-identical to the original across eight
// vectors covering both flag cleanups, all five score-delta arms, all four row
// translations plus the no-match fallback, all 26 slot-switch arms, seven
// distinct bar ratios including 0 and 1, and the crown pulse in all three of
// its arms (positive tick, negative tick through the +2^32 wrap, and crown
// clear). The gate mattered here: this runs every frame of a race via
// HudSlotUpdateCc50 and writes live RW frame matrices for all four rows.
RH_ScopedInstall(HudStandingsRowUpdate, 0x0041c410);
