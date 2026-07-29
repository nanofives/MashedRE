// Mashed RE — 4-byte global setters 0x00490f50 / 0x00490f80.
//
// Selected as the cheapest possible proof of the snapshot/restore A/B lane:
// scripts/write_surface.py reports a single-function call tree and a FULLY
// resolved write surface of 4 consecutive bytes, with no indirect dispatch and
// nothing in the human-review bucket. Of 1446 mutators only 142 are that clean.
//
// These are MUTATORS — they exist only to write globals — so the synthetic A/B
// lane cannot judge them (nothing to compare but the writes, and calling twice
// is not calling once). They are verified by GlobalByteQuadAB.cpp instead.
//
// Verbatim 0x00490f50..0x00490f76 (0x00490f80 is the same with +4 targets):
//   00490f50 mov al,[esp+4]        ; a
//   00490f54 mov cl,[esp+8]        ; b
//   00490f58 mov dl,[esp+0x10]     ; d
//   00490f5c mov [0x00616030],al   ; <- a
//   00490f61 mov al,[esp+0xc]      ; c   (al reused after its store)
//   00490f65 mov [0x00616031],cl   ; <- b
//   00490f6b mov [0x00616033],dl   ; <- d
//   00490f71 mov [0x00616032],al   ; <- c
//   00490f76 ret
//
// The STORE ORDER is interleaved (30, 31, 33, 32) and the argument→slot mapping
// is 30=a 31=b 32=c 33=d. Order is transcribed as-is: these are four distinct
// byte addresses, so order is not observable to a reader of the finished state,
// but the A/B compares the whole 4-byte window and there is no reason to
// deviate from the original.
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
constexpr std::uintptr_t kQuad50 = 0x00616030u;
constexpr std::uintptr_t kQuad80 = 0x00616034u;

inline void SetQuad(std::uintptr_t base, int a, int b, int c, int d) {
    volatile std::uint8_t* p = reinterpret_cast<volatile std::uint8_t*>(base);
    p[0] = static_cast<std::uint8_t>(a);
    p[1] = static_cast<std::uint8_t>(b);
    p[3] = static_cast<std::uint8_t>(d);
    p[2] = static_cast<std::uint8_t>(c);
}
} // namespace

// 0x00490f50 — writes 0x00616030..0x00616033
extern "C" __declspec(dllexport) void __cdecl ByteQuadSet50(int a, int b, int c, int d) {
    SetQuad(kQuad50, a, b, c, d);
}
RH_ScopedInstall(ByteQuadSet50, 0x00490f50);

// 0x00490f80 — twin, writes 0x00616034..0x00616037
extern "C" __declspec(dllexport) void __cdecl ByteQuadSet80(int a, int b, int c, int d) {
    SetQuad(kQuad80, a, b, c, d);
}
RH_ScopedInstall(ByteQuadSet80, 0x00490f80);
