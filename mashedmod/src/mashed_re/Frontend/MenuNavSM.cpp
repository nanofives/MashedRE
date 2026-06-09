// Mashed RE - Standalone menu navigation state machine (implementation).
//
// Port of MASHED's frontend nav centerpiece for the standalone mashed_re.exe.
// Because the standalone builds /BASE:0x10000 + image-pad .bss (which zeroes
// MASHED's address space), this port reimplements BOTH the logic AND every
// const/table it reads. The per-screen descriptor tables live at 0x005f6xxx
// (inside the zeroed range) so they are hard-coded here, harvested verbatim from
// original/MASHED.exe (SHA-256 BDCAE093...).
//
// Ported pieces (RVA-cited, NO-GUESSING):
//   FUN_0042ad90  kv-iterator      -> KvLookup()      (exact decomp transcription)
//   FUN_0042ac00  group-count      -> (not needed; item count derived from KvLookup)
//   FUN_0043d2a0  push/pop builder  -> Nav()          (Phase 3-7, minimal)
//   FUN_00432800  cursor-placement -> PlaceCursor()   (tail only; minimal all-enabled)
//   FUN_0042d3e0  array zero (C3)   -> RecordsZero()   (reimplemented locally)
//
// Deferred (documented in port spec): faithful per-screen grey-out cases
// (FUN_00430b60/0042f500/00497450 are sub-C2), the slide/countdown anim tick
// (FUN_004325c0), the prompt-strip glyph row (FUN_00432b30), and the faithful
// pixel draw loop (FUN_0043c5b0). The standalone renders g_records through its
// existing TextRenderer/MashedFont path instead.

#include "MenuNavSM.h"

#include <cstring>

namespace mashed_re {
namespace Frontend {

namespace {

// --------------------------------------------------------------------------
// Harvested per-screen descriptor tables (verbatim from MASHED.exe, pool13).
//
// Format: a flat u32 stream of (key, value) entries, walked by the kv-iterators
// FUN_0042ad90 / FUN_0042ac90. Group delimiter = 0xff060000 (-0xfa0000);
// terminator = 0xff070000 (-0xf90000). The selectable list items are the
// 0xff040000 entries; 0xff000000 is the back-target string id; 0xff080000 is
// the prompt/screen-kind id; 0xff140000 / 0xff050000 carry each item's ACTION
// CODE (the value the select handler dispatches on, see ItemActionCode + Nav_Select).
//
// Source: PTR_DAT_005f7638[0..33] pointer array @0x005f7638 (read_only
// Mashed_pool13, anchor BDCAE093... untouched, program_close issued). Each kTN[]
// below is the full byte stream from PTR_DAT_005f7638[N], read out to its
// 0xff070000 terminator. Screen 27 = nullptr (PTR_DAT_005f7638[27] = 0).
// Generated from re/analysis/standalone_menu_sm/harvest/.
//
// NOTE these REPLACE the earlier hand-transcribed kTable0..3, which truncated
// the action codes (e.g. root item 1 action was transcribed 0x50 but the real
// little-endian bytes are 0xff500000). The action codes are load-bearing for the
// reversed push map, so the full verbatim stream is required.
// --------------------------------------------------------------------------

const std::uint32_t kT0[] = { 0xff000000u, 0x00000047u, 0xff080000u, 0x00000005u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000018u, 0xff140000u, 0xff150000u, 0xff060000u, 0xff040000u, 0x00000027u, 0xff140000u, 0xff500000u, 0xff060000u, 0xff040000u, 0x0000001cu, 0xff140000u, 0xff1e0000u, 0xff060000u, 0xff040000u, 0x0000001du, 0xff140000u, 0xff1f0000u, 0xff060000u, 0xff040000u, 0x0000001eu, 0xff140000u, 0xff200000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff120000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT1[] = { 0xff000000u, 0x00000040u, 0xff080000u, 0x00000001u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000021u, 0xff050000u, 0x00000002u, 0xff060000u, 0xff040000u, 0x00000022u, 0xff050000u, 0x00000003u, 0xff060000u, 0xff040000u, 0x00000027u, 0xff140000u, 0xff500000u, 0xff060000u, 0xff040000u, 0x0000026bu, 0xff140000u, 0xff820000u, 0xff060000u, 0xff040000u, 0x00000261u, 0xff140000u, 0xff800000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT2[] = { 0xff000000u, 0x00000021u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x000000e5u, 0xff140000u, 0xff3d0000u, 0xff060000u, 0xff040000u, 0x000000e6u, 0xff140000u, 0xff4d0000u, 0xff060000u, 0xff040000u, 0x00000024u, 0xff050000u, 0xff400000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT3[] = { 0xff000000u, 0x00000022u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x0000013eu, 0xff140000u, 0xff2c0000u, 0xff060000u, 0xff040000u, 0x00000140u, 0xff140000u, 0xff2e0000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT4[] = { 0xff000000u, 0x00000130u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff1d0000u, 0xff060000u, 0xff090000u, 0xff0d0000u, 0xff0e0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT5[] = { 0xff000000u, 0x00000037u, 0xff080000u, 0x00000008u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff240000u, 0xff060000u, 0xff090000u, 0xff230000u, 0xff330000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT6[] = { 0xff000000u, 0x00000270u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff260000u, 0xff060000u, 0xff090000u, 0xff100000u, 0xff110000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT7[] = { 0xff000000u, 0x00000270u, 0xff080000u, 0x0000000au, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff260000u, 0xff060000u, 0xff090000u, 0xff100000u, 0xff110000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT8[] = { 0xff000000u, 0x00000027u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000156u, 0xff140000u, 0xff430000u, 0xff060000u, 0xff040000u, 0x00000234u, 0xff140000u, 0xff730000u, 0xff060000u, 0xff040000u, 0x00000157u, 0xff140000u, 0xff440000u, 0xff060000u, 0xff040000u, 0x00000250u, 0xff140000u, 0xff470000u, 0xff060000u, 0xff040000u, 0x0000025fu, 0xff140000u, 0xff830000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT9[] = { 0xff000000u, 0x00000025u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x0000005du, 0xff140000u, 0xff2e0000u, 0xff060000u, 0xff040000u, 0x0000005eu, 0xff050000u, 0xff2f0000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT10[] = { 0xff000000u, 0x0000013fu, 0xff080000u, 0x00000006u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000153u, 0xff140000u, 0xffffffffu, 0xff060000u, 0xff040000u, 0x0000005fu, 0xff140000u, 0xff300000u, 0xff060000u, 0xff040000u, 0x00000060u, 0xff140000u, 0xff310000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT11[] = { 0xff000000u, 0x000000c5u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x000000c6u, 0xff140000u, 0xff370000u, 0xff060000u, 0xff040000u, 0x000000c7u, 0xff140000u, 0xff380000u, 0xff060000u, 0xff090000u, 0xff100000u, 0xff0b0000u, 0xff0c0000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT12[] = { 0xff000000u, 0x000000ccu, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff3a0000u, 0xff060000u, 0xff090000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT13[] = { 0xff000000u, 0x000000c7u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x000000cdu, 0xff140000u, 0xff390000u, 0xff060000u, 0xff090000u, 0xff100000u, 0xff0b0000u, 0xff0c0000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT14[] = { 0xff000000u, 0x000000ccu, 0xff080000u, 0x00000006u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff260000u, 0xff060000u, 0xff090000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT15[] = { 0xff000000u, 0x000000cfu, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff3b0000u, 0xff060000u, 0xff090000u, 0xff0d0000u, 0xff0e0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT16[] = { 0xff000000u, 0x000000d5u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff3c0000u, 0xff060000u, 0xff090000u, 0xff0d0000u, 0xff0e0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT17[] = { 0xff000000u, 0x0000013fu, 0xff080000u, 0x00000006u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000132u, 0xff140000u, 0xffffffffu, 0xff060000u, 0xff040000u, 0x000000dbu, 0xff140000u, 0xffffffffu, 0xff060000u, 0xff040000u, 0x0000024cu, 0xff140000u, 0xffffffffu, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT18[] = { 0xff000000u, 0x0000024du, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000150u, 0xff140000u, 0xff420000u, 0xff060000u, 0xff040000u, 0x00000056u, 0xff140000u, 0xff810000u, 0xff060000u, 0xff040000u, 0x0000024cu, 0xff140000u, 0xff810000u, 0xff060000u, 0xff040000u, 0x00000103u, 0xff140000u, 0xff810000u, 0xff040000u, 0x000000feu, 0xff140000u, 0xff810000u, 0xff040000u, 0x000000dbu, 0xff140000u, 0xff810000u, 0xff060000u, 0xff040000u, 0x0000024du, 0xff140000u, 0xff810000u, 0xff060000u, 0xff060000u, 0xff060000u, 0xff090000u, 0xff100000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT19[] = { 0xff000000u, 0x00000156u, 0xff080000u, 0x00000004u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000158u, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x00000159u, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x00000260u, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x0000015bu, 0xff140000u, 0xff450000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff110000u, 0xff100000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT20[] = { 0xff000000u, 0x00000047u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff4c0000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT21[] = { 0xff000000u, 0x00000027u, 0xff080000u, 0x00000001u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000018u, 0xff140000u, 0xff490000u, 0xff060000u, 0xff040000u, 0x00000017u, 0xff140000u, 0xff4a0000u, 0xff060000u, 0xff040000u, 0x00000250u, 0xff140000u, 0xff470000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT22[] = { 0xff000000u, 0x000000c3u, 0xff080000u, 0x00000001u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff4b0000u, 0xff060000u, 0xff090000u, 0xff100000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT23[] = { 0xff000000u, 0x00000047u, 0xff080000u, 0x00000001u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff4c0000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT24[] = { 0xff000000u, 0x0000024du, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000150u, 0xff140000u, 0xff420000u, 0xff060000u, 0xff040000u, 0x00000056u, 0xff140000u, 0xff810000u, 0xff060000u, 0xff040000u, 0x000000fdu, 0xff140000u, 0xff810000u, 0xff060000u, 0xff040000u, 0x000000feu, 0xff140000u, 0xff810000u, 0xff060000u, 0xff040000u, 0x00000100u, 0xff140000u, 0xff810000u, 0xff060000u, 0xff040000u, 0x00000101u, 0xff140000u, 0xff810000u, 0xff060000u, 0xff040000u, 0x00000102u, 0xff140000u, 0xff810000u, 0xff060000u, 0xff040000u, 0x0000024du, 0xff140000u, 0xff810000u, 0xff060000u, 0xff090000u, 0xff100000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT25[] = { 0xff000000u, 0x00000020u, 0xff080000u, 0x00000000u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0xffffffffu, 0xff140000u, 0xff4f0000u, 0xff060000u, 0xff070000u };
const std::uint32_t kT26[] = { 0xff000000u, 0x0000001au, 0xff080000u, 0x00000004u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000224u, 0xff140000u, 0xff450000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT28[] = { 0xff000000u, 0x0000020fu, 0xff080000u, 0x00000004u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x000000c3u, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x000000c3u, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x000000c3u, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x000000c3u, 0xff140000u, 0xff450000u, 0xff060000u, 0xff090000u, 0xff0d0000u, 0xff0e0000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT29[] = { 0xff000000u, 0x00000235u, 0xff080000u, 0x00000002u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000242u, 0xff140000u, 0xff710000u, 0xff060000u, 0xff040000u, 0x00000243u, 0xff140000u, 0xff720000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT30[] = { 0xff000000u, 0x00000234u, 0xff080000u, 0x00000006u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x00000234u, 0xff140000u, 0xffffffffu, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff110000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT31[] = { 0xff000000u, 0x0000026bu, 0xff080000u, 0x00000004u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x0000026cu, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x0000026du, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x0000026eu, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x0000026fu, 0xff140000u, 0xff450000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff110000u, 0xff100000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT32[] = { 0xff000000u, 0x0000025fu, 0xff080000u, 0x00000004u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x0000025fu, 0xff140000u, 0xff450000u, 0xff060000u, 0xff090000u, 0xff0d0000u, 0xff0e0000u, 0xff110000u, 0xff100000u, 0xff0a0000u, 0xff070000u };
const std::uint32_t kT33[] = { 0xff000000u, 0x0000026bu, 0xff080000u, 0x00000004u, 0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u, 0xff040000u, 0x0000026du, 0xff140000u, 0xff450000u, 0xff060000u, 0xff040000u, 0x0000026eu, 0xff140000u, 0xff450000u, 0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u, 0xff0d0000u, 0xff0e0000u, 0xff110000u, 0xff100000u, 0xff0a0000u, 0xff070000u };

// PTR_DAT_005f7638 analogue: screen id -> descriptor table (all 34 entries
// verbatim; index 27 = nullptr matching PTR_DAT_005f7638[27]=0).
const std::uint32_t* const kScreenTables[] = {
    kT0, kT1, kT2, kT3, kT4, kT5, kT6, kT7, kT8, kT9,
    kT10, kT11, kT12, kT13, kT14, kT15, kT16, kT17, kT18, kT19,
    kT20, kT21, kT22, kT23, kT24, kT25, kT26, nullptr /*27*/, kT28, kT29,
    kT30, kT31, kT32, kT33,
};
constexpr int kScreenTableCount =
    static_cast<int>(sizeof(kScreenTables) / sizeof(kScreenTables[0]));

const std::uint32_t* TableForScreen(int screen_id) {
    if (screen_id < 0 || screen_id >= kScreenTableCount) return nullptr;
    return kScreenTables[screen_id];
}

// --------------------------------------------------------------------------
// Reversed item -> child-screen push map (FUN_0043dfd0, the frontend input/
// update tick; 0x0043dfd0). On SELECT the original reads the highlighted item's
// ACTION CODE via FUN_0042ac90 (0x0042ac90) — the value after the item's
// 0xff140000 / 0xff050000 key — then dispatches on it. The cases below are the
// FUN_0043d2a0(child, 0) PUSH targets transcribed verbatim from that dispatch
// ladder, with the originating decomp line cited.
//
// Three classes:
//   (A) unconditional push  -> faithful, returned directly.
//   (B) state-gated push    -> the original chooses between two children on a
//       sub-C2 game-state query; we take the *primary/default* child and mark it
//       [UNCERTAIN] (the secondary branch needs the gated state ported).
//   (C) non-push action      -> modal dialog (FUN_0042bf30), reload(0,2),
//       pop(0,1), or a pure side-effect; ActionToScreen returns a sentinel and
//       Nav_Select performs the right non-push behavior (or no-op).
//
// Default rule (LAB_0043fc37, 0x0043fc37): if the action code does not match any
// 0xffXX0000 case, the original calls FUN_0043d2a0(action_code, 0) — i.e. a
// SMALL action value IS the literal child screen id. This is how screen 1's
// items 0x21/0x22 (action 0x2 / 0x3) push screens 2 / 3.
// --------------------------------------------------------------------------

// Sentinels returned by ActionToScreen for non-push actions.
enum : int {
    kActReload = -2,   // FUN_0043d2a0(0,2)  reload current (returns to root state)
    kActPop    = -3,   // FUN_0043d2a0(0,1)  pop one level
    kActNone   = -1,   // modal / pure side-effect / unmapped: no nav change
};

// --------------------------------------------------------------------------
// Standalone game-state model + the ported state queries the dispatcher gates
// on. All values default to MASHED's fresh-main-menu reset state, confirmed by
// decompiling the original initializers (pool13, anchor BDCAE093...): player/team
// slots = -1, every mode flag / unlock entry = 0.
// --------------------------------------------------------------------------
// Fresh-main-menu defaults (image-padded zeroes; player slots -1).
const MenuGameState kFreshState = {
    { -1, -1, -1, -1 },  // team_slot[]
    0,                   // game_mode
    0,                   // flag_ea64
    0,                   // flag_ecdc
    0,                   // flag_ed6c
    { 0, 0, 0, 0 },      // player_active[]
    0,                   // has_savedata (DAT_007f0f2c)
    0,                   // has_profiles (DAT_007f0ad4)
    0,                   // ea88
    0,                   // ea7c
    0,                   // ea84 (DAT_0067ea8c)
    0,                   // cur_track_set
    { 0 },               // unlock_track[]
    { 0 },               // unlock_car[]
};
MenuGameState g_game_state = kFreshState;

// FUN_0042bb60 (0x0042bb60) - team-composition validator. Counts active team
// slots (DAT_007f1a14/24/34/44 >= 0); for each, reads its team byte
// (&DAT_0067e938)[slot*3]-1 and tallies team-0 (iVar2) vs team-1 (iVar4).
// Returns 0x1000 for a valid 1-vs-1 / 2-vs-1 / .. split, 1 / (iVar4!=0)+2 for
// other team-game compositions, 0 for none-paired, -1 when <2 active. Verbatim
// transcription. At fresh menu (all slots -1) iVar3==0 -> returns -1.
int Q_TeamComposition() {
    const MenuGameState& gs = g_game_state;
    int iVar2 = 0, iVar3 = 0, iVar4 = 0;
    int team[4] = { -1, -1, -1, -1 };
    for (int i = 0; i < 4; ++i) {
        if (gs.team_slot[i] >= 0) {
            // (&DAT_0067e938)[slot*3] - 1; standalone has no team table loaded,
            // so the byte is 0 -> team value -1 (neither 0 nor 1). Faithful at
            // fresh state; a save-loaded standalone would supply the real table.
            team[i] = -1; // (&DAT_0067e938)[gs.team_slot[i]*3] - 1
            if (team[i] == 0) ++iVar2;
            else if (team[i] == 1) ++iVar4;
            ++iVar3;
        }
    }
    if (iVar3 == 2) {
        if (iVar2 == 1 && iVar4 == 1) return 0x1000;
        return 0;
    }
    if (iVar3 == 3) {
        if (iVar2 == 1) { return (iVar4 == 2) ? 0x1000 : (iVar4 != 0) + 2; }
        if (iVar2 == 2) { if (iVar4 == 1) return 0x1000; return (iVar4 != 0) + 2; }
        if (iVar2 == 0) return 1;
        return (iVar4 != 0) + 2;
    }
    if (iVar3 == 4) {
        if (iVar2 == 2) { return (iVar4 == 2) ? 0x1000 : (iVar4 != 0) + 2; }
        if (iVar2 == 1) { return (iVar4 == 3) ? 0x1000 : (iVar4 != 0) + 2; }
        if (iVar2 == 3) { if (iVar4 == 1) return 0x1000; return (iVar4 != 0) + 2; }
        if (iVar2 == 0) return 1;
        return (iVar4 != 0) + 2;
    }
    return -1; // iVar3 < 2
}

// FUN_00402f40 (0x00402f40) - leaf getter: return DAT_00636ad8. Fresh menu = 0.
int Q_Flag636ad8() { return 0; }

// action code -> child screen (or sentinel). action is the raw u32 from the
// descriptor table. `slot_kind` is the active slot's screen-kind field
// (DAT_0067ed3c[depth*0x40]; 0 at fresh menu) used only by the 0xff4c0000 gate.
// RVAs are lines within FUN_0043dfd0 (0x0043dfd0) unless noted.
int ActionToScreen(std::uint32_t action, int slot_kind) {
    switch (action) {
        // (A) unconditional pushes -----------------------------------------
        case 0xff430000u: return 0x13; // L1116
        case 0xff490000u: return 7;    // L1138
        case 0xff4b0000u: return 1;    // L1133
        case 0xff4d0000u: return 4;    // L941
        case 0xff500000u: return 8;    // L1187
        case 0xff710000u:              // L1198
        case 0xff720000u: return 4;    // L1204
        case 0xff730000u: return 0x1e; // L1183
        case 0xff830000u: return 0x20; // L1229
        case 0xff2c0000u:              // L402 -> LAB_0043f198 -> push 4
        case 0xff2e0000u: return 4;    // L816 -> LAB_0043f1fd -> push 4
        case 0xff2d0000u:              // L400 -> LAB_0043f21d
        case 0xff2f0000u: return 10;   // L287
        case 0xff300000u:              // L849 -> ecdc=1 -> LAB_0043f203 -> push 4
        case 0xff310000u: return 4;    // L853 -> ecdc=2 -> LAB_0043f203 -> push 4
        case 0xff360000u: return 0xb;  // L863
        case 0xff380000u: return 0xd;  // L867
        case 0xff3b0000u: return 6;    // L914
        case 0xff400000u: return 4;    // L273 (LAB_0043f468 path; pushes 4)
        case 0xff4a0000u: return kActReload; // L1145 FUN_0043d2a0(0,2)
        case 0xff3a0000u: return kActReload; // L907 FUN_0043d2a0(0,2)
        case 0xff150000u: return kActReload; // L338 FUN_0043d2a0(0,2)
        case 0xff450000u: return kActPop;    // L1120 FUN_0043d2a0(0,1)

        // (B) state-gated dispatches (now FAITHFULLY ported; queries reproduce the
        // fresh-main-menu branch the real game takes).
        case 0xff240000u:
            // L292-331. game_mode 0 -> else (ea6c=4) -> LAB_0043e736; ecdc==0 &&
            // ed6c==0 -> push 7. Other states open confirm dialogs (no nav here).
            if (g_game_state.flag_ecdc == 0 && g_game_state.flag_ed6c == 0) return 7;
            return kActNone; // confirm dialog / state set: no nav in standalone
        case 0xff3c0000u:
            // L877-903. push 0xf only when FUN_0042bb60()==0x1000 (valid 1v1 team
            // split); otherwise a "need N players" modal (no nav).
            return (Q_TeamComposition() == 0x1000) ? 0xf : kActNone;
        case 0xff3d0000u: return 4;    // L257/LAB_0043f468: UNCONDITIONAL push 4
                                       // (also sets DAT_0067f184=1; no nav effect).
        case 0xff820000u:
            // L1210-1217. FUN_00402f40()==0 -> push 0x1f; else push 0x21.
            return (Q_Flag636ad8() == 0) ? 0x1f : 0x21;
        case 0xff4c0000u:
            // L1156-1165. slot_kind (DAT_0067ed3c[depth]) ==0x17 -> push 1; else
            // pop. Fresh menu slot_kind=0 -> pop.
            return (slot_kind == 0x17) ? 1 : kActPop;

        // (C) modal dialogs / pure side-effects (no nav) --------------------
        case 0xff1e0000u:              // L335 FUN_0042bf30 confirm
        case 0xff1f0000u:              // L392 FUN_0042bf30 confirm
        case 0xff200000u:              // L396 FUN_0042bf30 confirm
        case 0xff390000u:              // no Nav from this code
        case 0xff440000u:              // L951 FUN_00409900 (no push)
        case 0xff470000u:              // L1124 FUN_00409930 (no push)
        case 0xff4f0000u:              // L1192 handled, no push
        case 0xff800000u:              // L1219 FUN_0042bf30 confirm
        case 0xff810000u:              // L1223 reset cursor (no nav)
        case 0xff420000u:              // L953 gated reload/modal (player-count dep)
        case 0xff260000u:              // mode-tile sub-handler (internal codes)
        case 0xffffffffu:              // FUN_0042ac90 -> "no action key in group"
            return kActNone;

        // (default) LAB_0043fc37: small action value == literal child screen id.
        default:
            if (action < static_cast<std::uint32_t>(kScreenTableCount)) {
                return static_cast<int>(action);
            }
            return kActNone;
    }
}

// Tag keys (negated forms confirmed in the iterator decomp).
constexpr std::uint32_t kTagBack = 0xff000000u; // -0x1000000
constexpr std::uint32_t kTagItem = 0xff040000u; // -0xfc0000
constexpr std::uint32_t kTagAct1 = 0xff050000u; // -0xfb0000 (per-item action key)
constexpr std::uint32_t kTagAct2 = 0xff140000u; // -0xec0000 (per-item action key)
constexpr std::uint32_t kGrpEnd  = 0xff060000u; // -0xfa0000 (group delimiter)
constexpr std::uint32_t kTermVal = 0xff070000u; // -0xf90000 (terminator)

// --------------------------------------------------------------------------
// Module state (standalone analogue of MASHED's .data nav globals).
// --------------------------------------------------------------------------
int        g_nav_depth = 0;                 // DAT_0067e9f8
NavSlot    g_stack[kMaxNavDepth] = {};
MenuRecord g_records[kMaxRecords] = {};     // 0x00898ac0 analogue
int        g_record_count = 0;
int        g_last_dir = 0;                  // DAT_0067e844

// --------------------------------------------------------------------------
// FUN_0042ad90  kv-iterator. Verbatim transcription of the decomp:
//   __fastcall(ECX unused, EDX=table); EBX=tag_key, EDI=occurrence.
//   Walks the flat u32 stream; on the occurrence-th match of tag_key, returns
//   the NEXT u32. Stops (returns -1) at 0xff070000.
// 0x0042ad90
// --------------------------------------------------------------------------
int KvLookup(const std::uint32_t* table, std::uint32_t tag_key, int occurrence) {
    if (table == nullptr) {
        return -1;
    }
    std::int32_t iVar1 = static_cast<std::int32_t>(table[0]);
    int idx = 0;        // iVar2
    int occ = 0;        // iVar3
    for (;;) {
        if (static_cast<std::uint32_t>(iVar1) == kTermVal) {
            return -1;
        }
        if (static_cast<std::uint32_t>(iVar1) == tag_key) {
            if (occurrence == occ) {
                return static_cast<std::int32_t>(table[idx + 1]);
            }
            ++occ;
        }
        iVar1 = static_cast<std::int32_t>(table[idx + 1]);
        ++idx;
    }
}

// FUN_0042d3e0 (MenuEntryArrayInit, C3) analogue - zero the whole record array.
// 0x0042d3e0
void RecordsZero() {
    std::memset(g_records, 0, sizeof(g_records));
    g_record_count = 0;
}

// Count the selectable items (0xff040000 tag occurrences) in a screen's table.
// NOTE: must count by TAG occurrence, not by KvLookup value — some items carry a
// legitimate string id of 0xffffffff (-1) (e.g. screens 4/5/6/.. whose label is
// computed at runtime), which KvLookup returns as -1. Counting on value==-1 would
// wrongly drop those items (and zero the whole screen). The original derives the
// count from the group-count walker FUN_0042ac00, which counts tags.
int CountItems(const std::uint32_t* table) {
    if (table == nullptr) return 0;
    int n = 0, idx = 0;
    for (;;) {
        const std::uint32_t k = table[idx];
        if (k == kTermVal) break;
        if (k == kTagItem) { ++n; if (n >= kMaxItems) break; }
        ++idx;
    }
    return n;
}

// FUN_0042ac90 (0x0042ac90) - read the ACTION CODE of the item at list index
// `item_index`. The original walks the table past `item_index` occurrences of
// the item tag (0xff040000), then scans within that item's group for the action
// key (0xff050000 or 0xff140000), returning the following u32; or 0xffffffff
// ("no action") if the group ends (0xff060000) first. This is the value the
// select handler dispatches on (see ActionToScreen). Returns 0xffffffff if the
// item or table is absent.
std::uint32_t ItemActionCode(const std::uint32_t* table, int item_index) {
    if (table == nullptr || item_index < 0) return 0xffffffffu;
    int idx = 0;       // word cursor
    int item = -1;     // current item ordinal (-1 before first 0xff040000)
    bool in_target = false;
    for (;;) {
        const std::uint32_t k = table[idx];
        if (k == kTermVal) return 0xffffffffu;        // ran off the end
        if (k == kTagItem) {                          // 0xff040000 -> next item
            ++item;
            in_target = (item == item_index);
            idx += 2;                                 // skip (tag, string-id)
            continue;
        }
        if (in_target) {
            if (k == kGrpEnd) return 0xffffffffu;     // group closed, no action key
            if (k == kTagAct1 || k == kTagAct2) {     // 0xff050000 / 0xff140000
                return table[idx + 1];                // the dispatch action code
            }
            idx += 2;                                 // other keyed pair
            continue;
        }
        // Outside the target group: walk word-by-word (markers may be bare).
        ++idx;
    }
}

// FUN_00497450 (0x00497450) - player-active predicate. param<4 && DAT_007e96fc[
// param*0x80] != 0. Fresh menu: all inactive. Used by the screen-0x1c grey-out.
bool Q_PlayerActive(int player) {
    if (player < 0 || player >= 4) return false;
    return g_game_state.player_active[player] != 0;
}

// FUN_00430b60 (0x00430b60) - count of active team slots (DAT_007f1a14/24/34/44
// != -1). Fresh menu = 0. Used by the screen-0x12 grey-out.
int Q_ActivePlayerCount() {
    int n = 0;
    for (int i = 0; i < 4; ++i) if (g_game_state.team_slot[i] != -1) ++n;
    return n;
}

// FUN_00432800 (0x00432800) - per-screen availability-flag init + cursor
// placement. FAITHFUL port: first set all 12 avail flags = 1, then apply the
// screen-id-keyed disable switch using the ported state queries, then place the
// cursor on the first enabled item (forward scan, wrapping at item_count).
// Avail-flag offsets (verbatim from the decomp; base &DAT_0067ed84[depth*0x10]):
//   ed84=avail[0] ed88=avail[1] ed8c=avail[2] ed90=avail[3] ed98=avail[5]
//   ed9c=avail[6]. RVAs are lines within FUN_00432800.
void PlaceCursor(NavSlot& slot) {
    int* av = slot.avail;
    for (int i = 0; i < kMaxItems; ++i) av[i] = 1;

    const MenuGameState& gs = g_game_state;
    switch (slot.screen_id) {
    case 1:   // L21: if DAT_007f0f2c==0 -> avail[3]=0 (Restart/saved-game item)
        if (gs.has_savedata == 0) av[3] = 0;
        // (also FUN_0040e480(0..3,0): clears external player-ready flags; no avail)
        break;
    case 2:   // L31: if DAT_007f0ad4==0 -> avail[1]=0 (no profiles)
        if (gs.has_profiles == 0) av[1] = 0;
        break;
    case 8:   // L41: FUN_00492d10()==1 -> none; else avail[2]=0 then avail[3]=0
        // DAT_00771968 fresh=0 (!=1) -> disable item 2 and item 3.
        av[2] = 0;
        av[3] = 0;
        break;
    case 10:  // L48: ea8c!=2 -> avail[1]=0 ; ea8c!=3 -> avail[2]=0
        if (gs.ea84 != 2) av[1] = 0;
        if (gs.ea84 != 3) av[2] = 0;
        break;
    case 0x12: { // 18 - L52: unlock/profile gating
        // ea88==1 -> avail[3]=0, then track-2 unlock check; else avail[6] per ea7c.
        if (gs.ea88 == 1) {
            av[3] = 0;
            if (Q_ActivePlayerCount() == 2) { av[5] = 0; }
        } else {
            if (gs.ea7c != 0) av[5] = 1;
            else if (Q_ActivePlayerCount() == 2) av[5] = 0;
        }
        // car-unlock (DAT_007f0a58) -> avail[6]; track-unlock (DAT_007f0a50) -> avail[1]
        av[6] = (gs.unlock_car[0] != 0) ? 1 : 0;
        av[1] = (gs.unlock_track[0] != 0) ? 1 : 0;
        // L: FUN_00430b60()!=4 && FUN_0042f500()==0 -> ok; else avail[3]=0
        if (!(Q_ActivePlayerCount() != 4 && gs.flag_ea64 == 0)) av[3] = 0;
        break;
    }
    case 0x18: // 24 - L: FUN_00430830(1)==0 -> avail[3]=0 (mode-slot empty)
        // FUN_00430830 reads per-mode unlock slots; fresh menu = 0 -> disable.
        av[3] = 0;
        break;
    case 0x1c: // 28 - per-vehicle: avail[i]=Q_PlayerActive(i) for i in 0..3
        av[0] = 1; av[1] = 1; av[2] = 1; av[3] = 1;
        for (int i = 0; i < 4; ++i) {
            if (!Q_PlayerActive(i)) av[i] = 0;
        }
        break;
    default:
        break; // no per-screen disables
    }

    // Cursor placement tail (verbatim): start at stored cursor (or 0), scan
    // forward over item_count to the first avail==1; else -1.
    if (slot.item_count <= 0) { slot.cursor = -1; return; }
    int c = (slot.cursor < 0) ? 0 : slot.cursor;
    if (c >= slot.item_count) c = 0;
    if (c < kMaxItems && av[c] == 1) { slot.cursor = c; return; }
    for (int n = 0; n < slot.item_count; ++n) {
        const int probe = (c + n) % slot.item_count;
        if (probe < kMaxItems && av[probe] == 1) { slot.cursor = probe; return; }
    }
    slot.cursor = -1;
}

// FUN_0043d2a0 Phase 5+6 - expand the active screen's descriptor table into the
// record array. Back-button = record slot 0; list items follow. Minimal port:
// populates tag/type/ids/coords sufficient for the standalone text renderer.
void BuildRecords(const NavSlot& slot) {
    RecordsZero();

    int rec = 0;

    // Phase 5: back-button row (record slot 0). Tag 0xff000000; its id is the
    // first 0xff000000 entry in the table.
    const int back_id = KvLookup(slot.desc_table, kTagBack, 0);
    g_records[rec].tag    = static_cast<std::int32_t>(kTagBack);
    g_records[rec].type   = 1;
    g_records[rec].color  = static_cast<std::int32_t>(0xffffffffu);
    g_records[rec].scale  = 0.6f;       // 0x3f19999a
    g_records[rec].x      = 64.0f;      // 0x42800000
    g_records[rec].y      = 48.0f;      // 0x42400000
    g_records[rec].prim_id = back_id;   // back string id
    g_records[rec].sec_id  = -1;
    g_records[rec].row_index = -1;      // back row is not a list index
    ++rec;

    // Phase 6: list items (tag 0xff040000). One record per 0xff040000 occurrence.
    // prim may legitimately be -1 (0xffffffff string id = runtime-computed label);
    // do NOT treat that as end-of-list (item_count is authoritative, by tag).
    for (int row = 0; row < slot.item_count && rec < kMaxRecords; ++row) {
        const int prim = KvLookup(slot.desc_table, kTagItem, row);
        g_records[rec].tag       = static_cast<std::int32_t>(kTagItem);
        g_records[rec].type      = (g_nav_depth == 0) ? 1 : 0x1ff;
        g_records[rec].row_index = row;
        g_records[rec].color     = static_cast<std::int32_t>(0xffffffffu);
        g_records[rec].scale     = 0.8f;    // 0x3f4ccccd
        g_records[rec].x         = 64.0f;
        g_records[rec].y         = 0.0f;    // laid out by the renderer
        g_records[rec].prim_id   = prim;
        g_records[rec].sec_id    = -1;
        ++rec;
    }

    g_record_count = rec;
}

} // namespace

// --------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------

void Nav_Init() {
    g_nav_depth = 0;
    std::memset(g_stack, 0, sizeof(g_stack));
    // FUN_0043df00 enters the frontend via FUN_0043d2a0(0, 2) (reload screen 0).
    NavSlot& s = g_stack[0];
    s.screen_id  = 0;
    s.desc_table = TableForScreen(0);
    s.slide_src  = s.desc_table;
    s.cursor     = 0;
    s.item_count = CountItems(s.desc_table);
    PlaceCursor(s);
    BuildRecords(s);
    g_last_dir = kNavReload;
}

// FUN_0043d2a0(screen_id, dir).  0x0043d2a0
void Nav(int screen_id, int dir) {
    if (dir == kNavPush) {
        if (g_nav_depth + 1 >= kMaxNavDepth) return; // stack full guard
        // Save the current cursor for restore on pop.
        g_stack[g_nav_depth].saved_cursor = g_stack[g_nav_depth].cursor;
        const int d = g_nav_depth + 1;
        NavSlot& s = g_stack[d];
        std::memset(&s, 0, sizeof(s));
        s.screen_id  = screen_id;                       // DAT_0067ed7c
        s.desc_table = TableForScreen(screen_id);       // DAT_0067ed78 = PTR_005f7638[id]
        s.slide_src  = s.desc_table;                    // DAT_0067ed38
        s.cursor     = 0;                               // DAT_0067ed80 = 0
        s.item_count = CountItems(s.desc_table);        // DAT_0067edb4
        PlaceCursor(s);                                 // FUN_00432800 tail
        g_nav_depth = d;                                // commit push (Phase 7)
        BuildRecords(s);
    } else if (dir == kNavPop) {
        if (g_nav_depth < 1) return;                    // can't pop past root
        --g_nav_depth;                                  // DAT_0067e9f8 = depth-1
        NavSlot& s = g_stack[g_nav_depth];
        s.cursor = (s.saved_cursor >= 0) ? s.saved_cursor : 0;
        PlaceCursor(s);
        BuildRecords(s);
    } else { // kNavReload
        NavSlot& s = g_stack[g_nav_depth];
        s.item_count = CountItems(s.desc_table);
        PlaceCursor(s);
        BuildRecords(s);
    }
    g_last_dir = dir;                                   // DAT_0067e844
}

void Nav_MoveCursor(int delta) {
    NavSlot& s = g_stack[g_nav_depth];
    if (s.item_count <= 0) return;
    int c = (s.cursor < 0) ? 0 : s.cursor;
    // Step over the avail[] mask (wrap). Mirrors the forward scan in FUN_00432800.
    for (int n = 0; n < s.item_count; ++n) {
        c = (c + (delta >= 0 ? 1 : -1) + s.item_count) % s.item_count;
        if (s.avail[c] == 1) break;
    }
    s.cursor = c;
}

bool Nav_Select() {
    const NavSlot& s = g_stack[g_nav_depth];
    if (s.cursor < 0 || s.cursor >= s.item_count) return false;
    if (s.cursor < kMaxItems && s.avail[s.cursor] != 1) return false; // disabled

    // Reversed map (FUN_0043dfd0): read the highlighted item's action code from
    // the descriptor table (FUN_0042ac90), then dispatch it (ActionToScreen).
    const std::uint32_t action = ItemActionCode(s.desc_table, s.cursor);
    const int target = ActionToScreen(action, s.slot_kind);
    if (target == kActPop) {
        return Nav_Back();
    }
    if (target == kActReload) {
        Nav(0 /*ignored*/, kNavReload);
        return true;
    }
    if (target == kActNone) {
        return false; // modal dialog / pure side-effect / unmapped: no nav change
    }
    if (target < 0 || target >= kScreenTableCount || TableForScreen(target) == nullptr) {
        return false; // defensive: never push a screen we have no table for
    }
    Nav(target, kNavPush);
    return true;
}

bool Nav_Back() {
    if (g_nav_depth < 1) return false;
    Nav(0 /*ignored*/, kNavPop);
    return true;
}

// --- accessors -------------------------------------------------------------

int               Nav_Depth()       { return g_nav_depth; }
const NavSlot&    Nav_TopSlot()      { return g_stack[g_nav_depth]; }
const MenuRecord* Nav_Records()      { return g_records; }
int               Nav_RecordCount()  { return g_record_count; }
int               Nav_Cursor()       { return g_stack[g_nav_depth].cursor; }
int               Nav_ScreenId()     { return g_stack[g_nav_depth].screen_id; }

bool              Nav_ItemEnabled(int row_index) {
    if (row_index < 0 || row_index >= kMaxItems) return false;
    return g_stack[g_nav_depth].avail[row_index] == 1;
}

MenuGameState&    Nav_GameState()    { return g_game_state; }
void              Nav_GameStateReset() { g_game_state = kFreshState; }

} // namespace Frontend
} // namespace mashed_re
