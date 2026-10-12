// MenuMenusBB.cpp -- 0x00427ad0, moved out of Frontend/MenuMenusB.cpp as an x87 TU (drawstream 2026-10-12)
//
// Original: FUN_00427ad0 (body 0x00427ad0..0x00427bdd, stack cookie at 0x00427ad6 / 0x00427bc4).
// Signature: void __cdecl (uint32 id, float x, float y, float w, float h, uint32 argb, float size).
// Read with `py -3.12 re/tools/disasm_fn.py 0x00427ad0 0x00427be0` on original\MASHED.exe.unpatched.
// Callees by Ghidra function boundary: 0x00427780, 0x004277a0, 0x00552d10, 0x00556e90, 0x005555b0, 0x00552d70,
// plus __security_check_cookie 0x004a2be9.
//
// WHY ITS OWN TU. The rect is an x87 chain: 0x00427b31 fld w; 0x00427b38 fmul [0x5cd5a8]; 0x00427b47 fmul [0x67d830];
// 0x00427b54 fstp. The old body (SSE2) rounded w*xs to float before the second multiply, and the draw-stream diff
// (re/frida/negctl_fixtures/z_fix_drawstream.py) read it RED 5/7 at CW 0x27f, e.g. h 768.0000610351562 vs 768.
// This TU is in mashedmod/x87_tus.txt; every value the listing keeps in ST is a `double` and is rounded to `float`
// only where the bytes `fstp dword`, so the body follows the caller's precision control as the original does.

#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

using Void_t  = void (__cdecl*)();
using Color_t = void (__cdecl*)(void*, void*, void*, void*, void*);
using Text_t  = void (__cdecl*)(void*, std::uint8_t*, float, void*, int, void*);

constexpr std::uintptr_t kStrTableBase = 0x0066d828u;   // 0x00427784 / 0x0042778b
constexpr std::uintptr_t kXScale    = 0x005cd5a8u;      // 0x00427b38, 0x00427b80
constexpr std::uintptr_t kYScale    = 0x005cc560u;      // 0x00427b64, 0x00427b91
constexpr std::uintptr_t kYBase     = 0x005cc320u;      // 0x00427b97 fsubr
constexpr std::uintptr_t kSizeScale = 0x005cd5fcu;      // 0x00427bac
constexpr std::uintptr_t kViewW     = 0x0067d830u;      // 0x00427b47
constexpr std::uintptr_t kViewH     = 0x0067d834u;      // 0x00427b6f
constexpr std::uintptr_t kFontCtx   = 0x0067d838u;      // 0x00427b4d
constexpr std::uintptr_t kColorCtx  = 0x0067d83cu;      // 0x00427b15, 0x00427b3e

inline double Rd(std::uintptr_t a) { return static_cast<double>(*reinterpret_cast<const float*>(a)); }

}  // namespace

// 0x00427ad0
extern "C" __declspec(dllexport) void __cdecl MenuMenusBB(
    std::uint32_t id, float x, float y, float w, float h, std::uint32_t argb, float size)
{
    // local_204: the 0x200-byte UTF-16 buffer 0x004277a0 fills through EBX (0x00427af7 lea ebx,[esp+0x18]).
    std::uint8_t buf[0x200];

    // 0x00427780 (string id -> base-relative table entry) and 0x004277a0 (copy with control-code remap) pass the
    // string in EAX and the buffer in EBX, so they are transcribed inline: 0x00427784 / 0x0042778b, then
    // 0x004277a1..0x00427826.
    {
        const std::uint16_t* src = reinterpret_cast<const std::uint16_t*>(
            *reinterpret_cast<const std::uint32_t*>(kStrTableBase + id * 4u) + kStrTableBase);
        const short len = static_cast<short>(src[0]);
        short* dst = reinterpret_cast<short*>(buf);
        for (short i = 0; i < len; ++i) {
            short c = static_cast<short>(src[1 + i]);
            switch (c) {
                case 8:    c = 0x81; break;
                case 9:    c = 0x7f; break;
                case 10:   c = 0x81; break;
                case 0x0b: c = 0x8d; break;
                case 0x0c: c = 0x80; break;
                case 0x0d: c = 0x87; break;
                case 0x0e: c = 0x8f; break;
            }
            dst[i] = c;
        }
        dst[len] = 0;
    }

    reinterpret_cast<Void_t>(0x00552d10u)();                                         // 0x00427b00

    // 0x00427b05..0x00427b2c: colour context, all four colour pointers are &argb.
    void* colorCtx = *reinterpret_cast<void* const*>(kColorCtx);
    reinterpret_cast<Color_t>(0x00556e90u)(colorCtx, &argb, &argb, &argb, &argb);

    // 0x00427b31..0x00427bb2, rect = {x, y, w, h} at local_214.
    float rect[4];
    rect[2] = static_cast<float>(static_cast<double>(w) * Rd(kXScale) * Rd(kViewW));  // 0x00427b54 fstp
    rect[3] = static_cast<float>(static_cast<double>(h) * Rd(kYScale) * Rd(kViewH));  // 0x00427b75 fstp
    rect[0] = static_cast<float>(static_cast<double>(x) * Rd(kXScale));                // 0x00427b86 fstp
    rect[1] = static_cast<float>((Rd(kYBase) - static_cast<double>(y) * Rd(kYScale))  // 0x00427b97 fsubr
                                 - static_cast<double>(rect[3]));                      // 0x00427b9d fsub [h], 0x00427ba1 fstp
    const float sz = static_cast<float>(static_cast<double>(size) * Rd(kSizeScale));  // 0x00427bb2 fstp

    void* fontCtx = *reinterpret_cast<void* const*>(kFontCtx);
    reinterpret_cast<Text_t>(0x005555b0u)(fontCtx, buf, sz, rect, 1, colorCtx);       // 0x00427bb7

    reinterpret_cast<Void_t>(0x00552d70u)();                                         // 0x00427bbf
}

RH_ScopedInstall(MenuMenusBB, 0x00427ad0);
