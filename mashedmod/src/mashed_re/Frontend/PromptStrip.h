// FUN_00432b30 (prompt-strip row builder): the jump-table dispatch shared by
// both targets. Body: Frontend/PromptStripTwin.cpp (in both rsp lists).
#pragma once
#include <cstdint>

// The four record stores FUN_00432b30 makes after FUN_0042ad10 initialises the
// row: +0x28 sec_id, +0x2c prim_id, +0x04 type, +0x10 slide.
struct PromptStripRow {
    std::int32_t sec;
    std::int32_t prim;
    std::int32_t type;
    std::int32_t slide;
};

// key -> the row's tag, 0 = no row (key outside 1..10, or keys 3/7/9).
extern "C" std::uint32_t __cdecl PromptStripTag(int key);
// Fills *out for a key whose tag is non-zero.
extern "C" void __cdecl PromptStripResolve(int key, int b920_cmp, int rel,
                                           PromptStripRow* out);
