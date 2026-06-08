// Mashed RE - c3_batch_ah s4: RenderWare plugin-field accessors + format-size lookup.
// Bit-faithful ports of five tiny pure-leaf render functions.  See
//   re/analysis/bucket_004f022d/0x004f86{40,60,70,90}.md   (plugin field read/write)
//   re/analysis/bucket_004f022d/0x004f5020.md              (format-size table lookup)
//
// VERIFICATION NOTE (c3_batch_ah s4):
//   - 0x004f5020 FormatEntryLookup: GREEN (int_scalar). Static .rdata table at
//     0x005dae40 holds varied per-format sizes (4,8,12,16,...) -> non-degenerate.
//   - 0x004f8640/8660/8670/8690 (PluginField*): DEFER. They take TWO args
//     (int offset, u32 value) for the writers / ONE arg (int offset) for the
//     readers and address *(base + offset). The single-arg void_setter_observe /
//     zero-arg read_global handlers do not match that shape (would write/read at
//     base+offset, not the observed base). No matching arg_type exists and the
//     hard rule forbids editing diff_template.js -> author faithfully, defer
//     verification.
#include "../Core/HookSystem.h"
#include <cstdint>

// Plugin-offset bases registered by 0x004f8580.
static uint32_t* const DAT_007d73a8 = reinterpret_cast<uint32_t*>(0x007d73a8);
static uint32_t* const DAT_007d73ac = reinterpret_cast<uint32_t*>(0x007d73ac);

// Format-size lookup table (static .rdata).
static const uint32_t* const DAT_005dae40 = reinterpret_cast<const uint32_t*>(0x005dae40);

// 0x004f8640  FUN_004f8640  (18 bytes)  void(int offset, u32 value)
// *(u32*)(*DAT_007d73a8 + offset) = value;
extern "C" __declspec(dllexport) void __cdecl PluginFieldWriteA8(int offset, uint32_t value) {
    *reinterpret_cast<uint32_t*>(*DAT_007d73a8 + offset) = value;
}

RH_ScopedInstall(PluginFieldWriteA8, 0x004f8640);  // c3_batch_ah s4 (DEFER: arg-shape)

// 0x004f8660  FUN_004f8660  (14 bytes)  u32(int offset)
// return *(u32*)(*DAT_007d73a8 + offset);
extern "C" __declspec(dllexport) uint32_t __cdecl PluginFieldReadA8(int offset) {
    return *reinterpret_cast<uint32_t*>(*DAT_007d73a8 + offset);
}

RH_ScopedInstall(PluginFieldReadA8, 0x004f8660);  // c3_batch_ah s4 (DEFER: arg-shape)

// 0x004f8670  FUN_004f8670  (18 bytes)  void(int offset, u32 value)
// *(u32*)(*DAT_007d73ac + offset) = value;
extern "C" __declspec(dllexport) void __cdecl PluginFieldWriteAC(int offset, uint32_t value) {
    *reinterpret_cast<uint32_t*>(*DAT_007d73ac + offset) = value;
}

RH_ScopedInstall(PluginFieldWriteAC, 0x004f8670);  // c3_batch_ah s4 (DEFER: arg-shape)

// 0x004f8690  FUN_004f8690  (14 bytes)  u32(int offset)
// return *(u32*)(*DAT_007d73ac + offset);
extern "C" __declspec(dllexport) uint32_t __cdecl PluginFieldReadAC(int offset) {
    return *reinterpret_cast<uint32_t*>(*DAT_007d73ac + offset);
}

RH_ScopedInstall(PluginFieldReadAC, 0x004f8690);  // c3_batch_ah s4 (DEFER: arg-shape)

// 0x004f5020  FUN_004f5020  (12 bytes)  u32(int idx)
// return *(u32*)(&DAT_005dae40 + idx*4);  -> indexed read of the format-size table.
extern "C" __declspec(dllexport) uint32_t __cdecl FormatEntryLookup(int idx) {
    return DAT_005dae40[idx];
}

RH_ScopedInstall(FormatEntryLookup, 0x004f5020);  // c3_batch_ah s4
