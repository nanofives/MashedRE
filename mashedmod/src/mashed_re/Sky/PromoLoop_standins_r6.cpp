// Mashed RE: stand-in promotion round 6 (plan item #3 step 2, 2026-10-11): 0x004880a0 and its caller 0x004881d0.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// 0x004880a0  Obj7030acCreate  (FUN_004880a0, 295 B, cdecl(tex) -> *(0x007dc57c); caller 0x004881d0)
// Frame F = esp after `sub esp, 0x10; push esi`; locals F+4..F+0x10 = (0, 0, 0, 0x41200000) (0x004880b7..0x004880cf);
// tex = F+0x18 (0x00488183). p(obj) below is *(obj + *(0x007dc57c)), re-read from both globals at every store as the
// original does.
//   0x004880a4  frame = 0x004c0b30()
//   0x004880d7  obj = 0x00534b60(0x2000, 0x10000087, 0);  0x004880e2 [0x007030ac] = obj
//   0x004880ea  [p+0xa4] = 5;  0x00488102 [p+0xa8] = 2;  0x0048811a [p+0x40] |= 0x10000000;  0x0048812f [p+0x40] |= 0x800000;
//   0x00488145  [p+4] = 0x200
//   0x00488154  0x004e7e30(obj, frame)
//   0x00488159..0x00488180  [[[obj]+0x18]+0x5c]+4 .. +0x10 = the four locals   (obj re-read from 0x007030ac)
//   0x0048818a  tex != 0: byte [tex+0x50] = 2; 0x004881a3 0x004e8090(*[[obj+0x18]+0x20], tex)
//   0x004881b9  [p+0xac] = 1;  EAX on return = *(0x007dc57c) (0x004881ab)

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {
using Fn0 = void*(__cdecl*)();
using Fn2 = void*(__cdecl*)(void*, void*);
using Fn3u = void*(__cdecl*)(std::uint32_t, std::uint32_t, std::uint32_t);

std::uint32_t PlugOff() { return *reinterpret_cast<const std::uint32_t*>(0x007dc57cu); }
template <typename T> T& At(void* base, std::uint32_t off) {
    return *reinterpret_cast<T*>(static_cast<std::uint8_t*>(base) + off);
}

// The shared tail of 0x004880a0 (slot 0x007030ac, count 0x2000) and 0x004881d0 (slot 0x007030a8, count 0x200): every
// store re-reads the slot global and *(0x007dc57c) as both originals do.
void BuildInto(std::uint8_t** slot, std::uint32_t count, void* frame, std::uint8_t* tex) {
    auto plug = [slot]() { return *reinterpret_cast<std::uint8_t**>(*slot + PlugOff()); };
    *slot = static_cast<std::uint8_t*>(reinterpret_cast<Fn3u>(0x00534b60u)(count, 0x10000087u, 0));
    At<std::uint32_t>(plug(), 0xa4) = 5;
    At<std::uint32_t>(plug(), 0xa8) = 2;
    At<std::uint32_t>(plug(), 0x40) |= 0x10000000u;
    At<std::uint32_t>(plug(), 0x40) |= 0x800000u;
    At<std::uint32_t>(plug(), 4) = 0x200;
    reinterpret_cast<Fn2>(0x004e7e30u)(*slot, frame);
    std::uint8_t* morph = At<std::uint8_t*>(At<std::uint8_t*>(*slot, 0x18), 0x5c);
    At<std::uint32_t>(morph, 4) = 0;
    At<std::uint32_t>(morph, 8) = 0;
    At<std::uint32_t>(morph, 0xc) = 0;
    At<std::uint32_t>(morph, 0x10) = 0x41200000u;
    if (tex) {
        tex[0x50] = 2;
        void* mat = *At<void**>(At<std::uint8_t*>(*slot, 0x18), 0x20);
        reinterpret_cast<Fn2>(0x004e8090u)(mat, tex);
    }
    At<std::uint32_t>(plug(), 0xac) = 1;
}
}  // namespace

// 0x004880a0  (store addresses in the header above)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Obj7030acCreate(std::uint8_t* tex) {
    void* frame = reinterpret_cast<Fn0>(0x004c0b30u)();                                  // 0x004880a4
    BuildInto(reinterpret_cast<std::uint8_t**>(0x007030acu), 0x2000u, frame, tex);       // 0x004880d7..0x004881b9
    return PlugOff();                                                                    // 0x004881ab
}
RH_ScopedInstall(Obj7030acCreate, 0x004880a0);

// 0x004881d0  Obj7030a8Create  (FUN_004881d0, 323 B, cdecl(tex))
//   0x004881d4  frame = 0x004c0b30()  (before the guard: the guarded path does not use or release it)
//   0x004881db  [0x007030b0] != 0 -> return [0x007030b0] (EAX, 0x0048830e)
//   0x00488214..0x004882fa  as 0x004880a0 with count 0x200 and slot 0x007030a8 (0x0048821f), the same plugin stores,
//               0x004e7e30, sphere (0,0,0,10.0f), texture mark + 0x004e8090, [p+0xac] = 1
//   0x00488301  [0x007030b0] = 1;  0x00488306 call 0x004880a0(tex); return its EAX
// 0x004881d0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Obj7030a8Create(std::uint8_t* tex) {
    void* frame = reinterpret_cast<Fn0>(0x004c0b30u)();                                  // 0x004881d4
    std::uint32_t& guard = *reinterpret_cast<std::uint32_t*>(0x007030b0u);
    if (guard != 0) return guard;                                                        // 0x004881e2
    BuildInto(reinterpret_cast<std::uint8_t**>(0x007030a8u), 0x200u, frame, tex);        // 0x00488214..0x004882fa
    guard = 1;                                                                           // 0x00488301
    return Obj7030acCreate(tex);                                                         // 0x00488306
}
RH_ScopedInstall(Obj7030a8Create, 0x004881d0);
