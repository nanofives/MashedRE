// ProgressBarSetA.cpp -- 0x00430b90, re-ported from the listing (drawstream 2026-10-12)
//
// Original: FUN_00430b90, body 0x00430b90..0x0043122d (jump table 0x00431230..0x0043123f).
// Read with `py -3.12 re/tools/disasm_fn.py 0x00430b90 0x00431230` on original\MASHED.exe.unpatched.
// Callees by Ghidra function boundary (re/tools/decomp_pc.py --callees): 0x0040bb50, 0x00427e00, 0x004282a0,
// and 0x0042ac00, 0x0042ac50, 0x0042b8b0, 0x0042b8c0, 0x00472c60, 0x00473870, 0x004739f0, 0x004a2c48, plus the indirect
// `call [*(0x007d3ff8)+0x20]` render-state slot.
//
// The old body (Frontend/SpriteCluster.cpp, transcribed from an analysis note) did not follow the bytes:
//   * it returned on the early-out before issuing (6,0),(8,0) and never issued (6,1),(8,1) there
//     (0x00430b99..0x00430bbe run first, 0x00430bde..0x00430bf8 on the early-out);
//   * 0x0042ac00 takes the token stream in EDX (0x00430c07 / 0x00430c18) and 0x0042ac50 takes the count in EAX
//     and 0x1e in ECX (0x00430c27); the old body called both with no register setup;
//   * it called the in-TU SpriteLookupC body once, the original calls 0x0040bb50 twice per row;
//   * three colour words, not one: 0x00430b9d..0x00430bac build 0x??2080d8, 0x00430c4b..0x00430c64 zero the two others,
//     and only the alpha byte of each is set per row (0x00430c70..0x00430c7a);
//   * all quad / sprite arguments, the fill height (0x41200000 = 10.0, not 18.0), the strict `> 1.0` fill gate
//     (0x00430e5a fcomp + test ah,0x41), the case-3 fallback id (the stale 0x0042ac50 result) and the right-arrow
//     x (ftol of 0x004282a0's ST0 + 0x17e at 0x00431222..0x00430ea1) differed.
//
// FLOAT MODEL. This TU is listed in mashedmod/x87_tus.txt (x87 codegen). Every value the listing keeps in ST between
// instructions is a `double` here and is rounded to `float` only where the bytes `fst`/`fstp dword` it, so the body
// follows whatever precision control the calling thread runs at, as the original does
// (memories st-resident-values-must-be-double, measure-ports-at-the-live-cw).

#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

// .rdata constants, read where the listing reads them.
inline float Rd(std::uintptr_t a) { return *reinterpret_cast<const float*>(a); }

using RenderState_t = void (__cdecl*)(int, int);
// Render-state slot, read at 0x00430b93 / 0x00430bb4 / 0x00430bd8 / ...: mov reg,[0x7d3ff8]; call [reg+0x20]. Re-read per call.
inline void RenderState(int state, int value)
{
    const std::uintptr_t g = *reinterpret_cast<const std::uintptr_t*>(0x007d3ff8u);
    (*reinterpret_cast<RenderState_t*>(g + 0x20u))(state, value);
}

using Short_t  = short (__cdecl*)();
using Lookup_t = void* (__cdecl*)(const char*);
using Quad_t   = void (__cdecl*)(float, float, float, float, std::uint32_t);
using Spr12_t  = void (__cdecl*)(void*, float, float, float, float, std::uint32_t,
                                 std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
using Spr7_t   = void (__cdecl*)(void*, float, float, float, float, std::uint32_t, std::uint32_t);
using Icon_t   = void (__cdecl*)(int, float, float, std::uint32_t, std::uint32_t, std::uint32_t);

inline short  ScreenW()                 { return reinterpret_cast<Short_t>(0x0042b8b0u)(); }   // ax = [0x67ea54]
inline short  ScreenH()                 { return reinterpret_cast<Short_t>(0x0042b8c0u)(); }   // ax = [0x67ea56]
inline void*  ArrowLookup()             { return reinterpret_cast<Lookup_t>(0x0040bb50u)(reinterpret_cast<const char*>(0x005cd828u)); }
inline void   Quad(float x, float y, float w, float h, std::uint32_t c) { reinterpret_cast<Quad_t>(0x00472c60u)(x, y, w, h, c); }

// Call shim (not a body) for 0x0042ac00: count of 0xff060000 tokens before 0xff070000 in the dword stream at EDX.
__declspec(naked) int TokenCount(const void* /*stream*/)
{
    __asm {
        mov  edx, dword ptr [esp + 4]
        mov  eax, 0x0042ac00
        call eax
        ret
    }
}

// Call shim (not a body) for 0x0042ac50: EAX = count, ECX = spacing (0x1e at 0x00430c27), returns the first row's y in EAX.
__declspec(naked) int RowBase(int /*count*/, int /*spacing*/)
{
    __asm {
        mov  eax, dword ptr [esp + 4]
        mov  ecx, dword ptr [esp + 8]
        mov  edx, 0x0042ac50
        call edx
        ret
    }
}

// Call shim (not a body), the caller's 0x00431210..0x00431227: push 0x3f333333; push id; call 0x4282a0; add esp,8; call 0x4a2c48 (ftol of the ST0
// return, untouched in between); mov edx,eax.
__declspec(naked) int TextWidthFtol(int /*id*/, std::uint32_t /*sizeBits*/)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        mov  eax, 0x004282a0
        call eax
        add  esp, 8
        mov  eax, 0x004a2c48
        call eax
        ret
    }
}

// Signed 32-bit product, then the `imul 0x66666667; sar edx,8; + sign bit` quotient, i.e. truncating / 640.
inline int Div640(std::int32_t a, std::int32_t b)
{
    const std::int32_t p = static_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b));
    return p / 640;
}

}  // namespace

// 0x00430b90
extern "C" __declspec(dllexport) void __cdecl ProgressBarSetA()
{
    // 0x00430b99..0x00430bbe: render states before the gate.
    RenderState(6, 0);
    RenderState(8, 0);

    const std::int32_t mode  = *reinterpret_cast<const std::int32_t*>(0x0067e7f8u);   // 0x00430bc1
    std::int32_t       alpha = *reinterpret_cast<const std::int32_t*>(0x0067e7fcu);   // 0x00430bc6 (ebx)

    // 0x00430bcf..0x00430bf8: mode 0 with alpha < 0x60 (signed) restores the states and returns.
    if (mode == 0 && alpha < 0x60) {
        RenderState(6, 1);
        RenderState(8, 1);
        return;
    }

    // 0x00430bf9..0x00430c18: the token stream comes from 0x0067ed38 (mode 1) or 0x0067ed78 (anything else),
    // indexed by DAT_0067e9f8 << 6.
    const std::uint32_t sel = static_cast<std::uint32_t>(*reinterpret_cast<const std::int32_t*>(0x0067e9f8u)) << 6;
    const void* stream = *reinterpret_cast<void* const*>((mode == 1 ? 0x0067ed38u : 0x0067ed78u) + sel);

    const int count = TokenCount(stream);                  // 0x00430c20, edi
    const int base  = RowBase(count, 0x1e);                // 0x00430c30, [B+0x18]; also case 3's fallback id
    float rowY = static_cast<float>(base);                 // 0x00430c39 fild, 0x00430c41 fstp [B+0xc]

    if (count > 0) {
        // Colour words at B+8 (d8 80 20 a), B+0x10 (00 00 00 a>>1), B+0x14 (00 00 00 a).
        std::int32_t iconId = base;                        // [L+0x24] until case 3 writes it

        for (int row = 0; row < count; ++row) {
            const std::uint8_t a = static_cast<std::uint8_t>(alpha);                   // bl
            const std::uint32_t col1 = 0x002080d8u | (static_cast<std::uint32_t>(a) << 24);
            const std::uint32_t col2 = static_cast<std::uint32_t>(a >> 1) << 24;        // shr bl,1 at 0x00430c78
            const std::uint32_t col3 = static_cast<std::uint32_t>(a) << 24;

            // 0x00430c7e..0x00430ca5: x = (short)W * 0x164 / 640.
            const float arrowX = static_cast<float>(Div640(ScreenW(), 0x164));
            // 0x00430ca9..0x00430ccb: ((rowY - [0x5cc9f4]) * (short)H) * [0x5cc560].
            const double hy = static_cast<double>(ScreenH());
            const float arrowY = static_cast<float>(((static_cast<double>(rowY) - Rd(0x005cc9f4u)) * hy) * Rd(0x005cc560u));
            // 0x00430ccf..0x00430ce5: (short)W * [0x5cc9a4].
            const float arrowW = static_cast<float>(static_cast<double>(ScreenW()) * Rd(0x005cc9a4u));
            // 0x00430ce9 / 0x00430d0c..0x00430d1e: (short)H * [0x5cc324].
            const float arrowH = static_cast<float>(static_cast<double>(ScreenH()) * Rd(0x005cc324u));

            // 0x00430cf6..0x00430d3c
            void* spr = ArrowLookup();
            reinterpret_cast<Spr12_t>(0x004739f0u)(spr, arrowX, arrowY, arrowW, arrowH, col3,
                                                   0x3f7ff972u, 0u, 0u, 0x3f800000u, 0u, 0u);

            std::int32_t edxArg = 0x66;                    // 0x00430e94
            if (row <= 2) {
                // Cases 0/1/2 (0x00430d54 / 0x00430f3d / 0x0043107a) differ only in the fill global.
                static const std::uintptr_t kFill[3] = { 0x007f0f04u, 0x007f0f00u, 0x007f0f08u };
                const float yy  = static_cast<float>(static_cast<double>(rowY) - Rd(0x005cc9b8u));   // fst / fstp
                Quad(374.0f, yy, 106.0f, 16.0f, col2);                 // 0x43bb0000, 0x42d40000, 0x41800000
                Quad(374.0f, yy, 106.0f, 3.0f, col3);
                const float yy2 = static_cast<float>(static_cast<double>(yy) + Rd(0x005cd8d8u));
                Quad(374.0f, yy2, 106.0f, 3.0f, col3);
                Quad(374.0f, yy, 3.0f, 16.0f, col3);
                Quad(477.0f, yy, 3.0f, 16.0f, col3);                   // 0x43ee8000
                const float yy3 = static_cast<float>(static_cast<double>(yy) + Rd(0x005cc31cu));
                const double fill = static_cast<double>(Rd(kFill[row])) * Rd(0x005cc568u);
                const float fillW = static_cast<float>(fill);                                    // fst [L+0x34]
                if (fill > static_cast<double>(Rd(0x005cc320u)))                                 // fcomp; test ah,0x41
                    Quad(377.0f, yy3, fillW, 10.0f, col1);       // 0x43bc8000, 0x41200000
            } else if (row == 3) {
                // 0x004311b7..0x00431227
                const std::int32_t s = *reinterpret_cast<const std::int32_t*>(0x007f0f10u);
                if (s == 0)      iconId = 0x59;
                else if (s == 1) iconId = 0x1b2;
                else if (s == 2) iconId = 0x1b1;
                reinterpret_cast<Icon_t>(0x00427e00u)(iconId, 376.0f, rowY, col3, 0x3f333333u, 0u);
                edxArg = TextWidthFtol(iconId, 0x3f333333u);
            }

            // 0x00430e99..0x00430eec: x = (short)W * (edx + 0x17e) / 640, then the right arrow.
            const float arrowX2 = static_cast<float>(Div640(ScreenW(), edxArg + 0x17e));
            void* spr2 = ArrowLookup();
            reinterpret_cast<Spr7_t>(0x00473870u)(spr2, arrowX2, arrowY, arrowW, arrowH, col3, 1u);

            // 0x00430ef1..0x00430f07
            rowY = static_cast<float>(static_cast<double>(rowY) + Rd(0x005cc72cu));
            alpha = *reinterpret_cast<const std::int32_t*>(0x0067e7fcu);                  // 0x00430efb
        }
    }

    // 0x00430f1a..0x00430f30
    RenderState(6, 1);
    RenderState(8, 1);
}

RH_ScopedInstall(ProgressBarSetA, 0x00430b90);
