// Mashed RE - Frontend menu navigation helpers shared by both builds.
//
// MenuNav.cpp is in asi_sources.rsp AND exe_sources.rsp, so a body declared
// here is the single implementation at its RVA for the .asi hook and the
// standalone mashed_re.exe alike.
#ifndef MASHED_RE_FRONTEND_MENUNAV_H
#define MASHED_RE_FRONTEND_MENUNAV_H

#include <cstdint>

namespace mashed_re {
namespace Frontend {

// 0x0042ac90 table walk (the body of MenuEntryGet), with the three globals it
// reads passed in: `table` = *(0x0067ed38 + slot*0x40), `cursor` =
// *(0x0067ed40 + slot*0x40). Cursor -1 skips the item walk, so the final scan
// starts at word 0 and returns item 0's action code. Returns 0xffffffff when the
// final scan stops on 0xff060000. No null check: the original dereferences.
std::uint32_t MenuEntryWalk(const std::uint32_t* table, std::int32_t cursor);

} // namespace Frontend
} // namespace mashed_re

#endif // MASHED_RE_FRONTEND_MENUNAV_H
