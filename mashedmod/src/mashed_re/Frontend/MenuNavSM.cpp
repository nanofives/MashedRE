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
    2,                   // has_profiles (DAT_007f0ad4) - LIVE-PROBED 2026-06-12:
                         // the original holds 2 at the menu with an all-zero
                         // gamesave.bin (boot default, NOT save-derived)
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
constexpr std::uint32_t kTagPrompt = 0xff080000u; // -0xf80000 (prompt-strip string id key)
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

// FUN_0042ac50 (0x0042ac50) - vertical-centering base for the item list. The
// build loop writes each item's Y as base + spacing*row, where `base` is this
// function's return. Verbatim transcription of the decomp (pool13, anchor
// BDCAE093...):  in_EAX = visible item count, param_1 = per-item spacing (0x1e):
//   if (count & 1) == 0: base = (0xf0 - spacing/2) - ((count-1)>>1)*spacing
//   else               : base =  0xf0           - ((count-1)>>1)*spacing
// 0xf0 = 240 is the vertical center in MASHED's 640x480 virtual coord space.
// Returns the Y of the FIRST item (row 0); each subsequent row adds `spacing`.
int Q_ListBaseY(int item_count, int spacing) {
    const unsigned cnt = static_cast<unsigned>(item_count);
    if ((cnt & 1u) == 0u) {
        return (0xf0 - spacing / 2) - static_cast<int>((cnt - 1) >> 1) * spacing;
    }
    return 0xf0 - static_cast<int>((cnt - 1) >> 1) * spacing;
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
        // LIVE-PROBED 2026-06-12: DAT_00771968 == 1 at the original's menu
        // (save system ready even with a blank gamesave) -> Load/Save stay
        // ENABLED. The standalone's save layer (R2-2) is functional, so the
        // ready state is 1 here too; the disable path applies only if the
        // save layer failed to initialize.
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
// record array. Back-button = record slot 0; list items follow. The coordinate /
// scale / color fields are now populated FAITHFULLY (verbatim from the pool13
// decomp of FUN_0043d2a0's record writes) so the ported draw loop can read each
// record's EXACT stored x/y/scale/color (640x480 virtual coords; the renderer
// applies the 800x600 1.25x screen scale, mirroring FUN_00427680/FUN_00472c60).
//
// Field writes transcribed from the decomp (puVar4 = &DAT_00898ad8 = record+0x18):
//   back row 0:  +0x00 tag 0xff000000, +0x0c color 0xffffffff (rgb bytes),
//                +0x18 X 0x42800000 (64.0), +0x1c Y 0x42400000 (48.0),
//                +0x14 scale 0x3f19999a (0.6), +0x28 sec, +0x2c prim.
//   item rows :  +0x00 tag 0xff040000, +0x0c color 0xffffffff,
//                +0x18 X 0x42800000 (64.0), +0x14 scale 0x3f4ccccd (0.8),
//                +0x1c Y = Q_ListBaseY(count,0x1e) + 0x1e*row  (FUN_0042ac50 +
//                spacing), +0x10 slide 0x1ff (0 + type 1 at root), +0x2c prim.
void BuildRecords(const NavSlot& slot) {
    RecordsZero();

    int rec = 0;

    // Phase 5: back-button row (record slot 0). Tag 0xff000000; its id is the
    // first 0xff000000 entry in the table. X 64.0 / Y 48.0 / scale 0.6 (verbatim
    // DAT_00898ad8/adc/ad4 writes).
    const int back_id = KvLookup(slot.desc_table, kTagBack, 0);
    // #15 (user review): the back-row record at (64,48) IS the screen TITLE
    // (top-left). Its native id already resolves to the right title in the
    // table the game actually uses — Font36.piz/USA.DAT (g_msg_dat), NOT the
    // loose English.dat: 0x40="Game Type Select" (main menu kT1), 0x21="Single
    // Player", 0x27="Options", etc. (The loose English.dat DIVERGES — its 0x40
    // is "Total" — which is why an earlier 0x43 override drew a prompt string.
    // All menu text resolves through GetMenuMessage -> USA.DAT(piz).)
    g_records[rec].tag    = static_cast<std::int32_t>(kTagBack);
    // FUN_0043d2a0 back-row anim init: with a live back id the row enters
    // animating (DAT_00898ad0=0x1ff, DAT_00898ac4=0 or 2); only the no-back
    // case settles immediately (type=1, slide=0). Root starts settled.
    g_records[rec].type   = (g_nav_depth == 0) ? 1 : 0;
    g_records[rec].color  = static_cast<std::int32_t>(0xffffffffu);
    g_records[rec].scale  = 0.6f;       // 0x3f19999a
    g_records[rec].x      = 64.0f;      // 0x42800000
    g_records[rec].y      = 48.0f;      // 0x42400000
    g_records[rec].prim_id = back_id;   // back string id
    g_records[rec].sec_id  = -1;
    g_records[rec].row_index = -1;      // back row is not a list index
    g_records[rec].slide   = (g_nav_depth == 0) ? 0 : 0x1ff;  // FUN_0043d2a0
    ++rec;

    // Phase 6: list items (tag 0xff040000). One record per 0xff040000 occurrence.
    // prim may legitimately be -1 (0xffffffff string id = runtime-computed label);
    // do NOT treat that as end-of-list (item_count is authoritative, by tag).
    // Y is the FUN_0042ac50 vertical-centering base + 0x1e(30) per row, so the
    // list is centered on virtual-Y 0xf0 (240) exactly as the original lays it out.
    constexpr int kSpacing = 0x1e;      // 30, the per-row Y step (local_c += 0x1e)
    const int base_y = Q_ListBaseY(slot.item_count, kSpacing);
    for (int row = 0; row < slot.item_count && rec < kMaxRecords; ++row) {
        const int prim = KvLookup(slot.desc_table, kTagItem, row);
        g_records[rec].tag       = static_cast<std::int32_t>(kTagItem);
        g_records[rec].row_index = row;
        g_records[rec].color     = static_cast<std::int32_t>(0xffffffffu);
        g_records[rec].scale     = 0.8f;    // 0x3f4ccccd
        g_records[rec].x         = 64.0f;   // 0x42800000
        g_records[rec].y         = static_cast<float>(base_y + kSpacing * row);
        g_records[rec].prim_id   = prim;
        g_records[rec].sec_id    = -1;
        // FUN_0043d2a0 builder: puVar4[2] = iVar6 (record+0x20 = running row
        // counter; pool0 decomp 2026-06-09) — the anim tick's type-0 settle
        // recomputes Y from this field (FUN_004325c0 @0x004326b8).
        g_records[rec].pad20     = row;
        // Anim init (FUN_0043d2a0): slide counter +0x10 = 0x1ff (item slides in),
        // type +0x04 = 0 (not frozen). The anim tick (FUN_004325c0) drives slide
        // -> 0 then settles (type=1). Root screen items start settled (type=1,
        // slide=0 — puVar4[-5]=1 / puVar4[-2]=0 when DAT_0067e9f8 == 0).
        g_records[rec].slide     = (g_nav_depth == 0) ? 0 : 0x1ff;
        g_records[rec].type      = (g_nav_depth == 0) ? 1 : 0;
        ++rec;
    }

    g_record_count = rec;
}

// --------------------------------------------------------------------------
// FUN_00432b30 (0x00432b30) — footer prompt-strip row builder. VERBATIM port
// of the C4-verified hook twin (Frontend/PromptStripTwin.cpp, diff GREEN
// 264/264, re/analysis/standalone_menu_sm/c4_promptstrip_20260611.md).
// Called once per nav op by FUN_0043d2a0 Phase 7 (call site 0x0043d79c) with
// EAX=mode (0 push / 1 pop-reveal / 2 reload), key = the SHOWN screen's
// 0xff080000 kind, cmp = pushed screen id (0 on pop). Inside, rel =
// Add0(0xff080000, 0) = the kind of the screen on the OTHER side of the
// transition (pre-commit depth slot on push; depth+1 popped slot on pop,
// via the inc/dec window 0x00432b52..0x00432b6e). Mode 2 zeroes key
// (0x00432b45) so reloads never append a row.
// --------------------------------------------------------------------------

// FUN_0042a9c0 ModeCodeLookup (C3, Frontend/BatchAA_s4.cpp, exe-built leaf).
extern "C" std::uint32_t __cdecl ModeCodeLookup();
// FUN_00472640 fade-target setter analogue (DAT_0086ecc8 = v). Xref-confirmed
// writers (pool0, 2026-06-12): the FUN_0043d2a0 head raises it to 0xff on
// dir>=2 (reload) ops ONLY (dispatch at 0x0043d2b1..b7 routes push/pop away
// from 0x0043d2c2 PUSH 0xff), and FUN_0043dfd0 raises it on select
// (0x0043f9d7, alongside DAT_0067ecb8=1). The arc-wash strips read it via
// the DAT_0086eccc chaser inside FUN_00473ee0.
extern "C" void __cdecl LogoOverlayFadeSet(int target, int cur);

namespace {

// finisher codes (cell tails in FUN_00432b30's inner jump-table bodies)
enum PromptFin { kPZero, kPFrozen, kPDefault };
struct PromptCell { int prim; bool a9c0; bool dbl48; PromptFin fin; };
constexpr PromptCell PC(int p, PromptFin f) { return PromptCell{p, false, false, f}; }
constexpr PromptCell PCA(PromptFin f)       { return PromptCell{0, true, false, f}; }
constexpr PromptCell PCD(PromptFin f)       { return PromptCell{0, false, true, f}; }
constexpr PromptCell PFRZ()                 { return PromptCell{-1, false, false, kPFrozen}; }
constexpr PromptCell PDEF()                 { return PromptCell{-1, false, false, kPDefault}; }

struct PromptKindRow {
    std::uint32_t tag;
    int sec;           // -2 = ModeCodeLookup(); -3 = 0x48-then-0x13; -4 = 0x42 w/ b920 gate
    PromptCell cells[10];
};
// Jump tables transcribed literally from MASHED.exe data: outer 10 dwords at
// 0x433060, inner 10 each at 0x433088/b0/d8/100/128/150/178; every cell body
// walked linearly in FUN_00432b30_full.asm (ebx=-1, ebp=0). Index = rel-1;
// out-of-range = PDEF. Keys 3/7/9 dispatch straight to the epilogue (no row).
const PromptKindRow kPromptRows[10] = {
    /*key1 */ {0xff100000u, -4, {PFRZ(), PC(0x43,kPZero), PDEF(), PC(0x225,kPZero), PCD(kPZero), PC(0x58,kPZero), PDEF(), PCA(kPZero), PDEF(), PC(0x133,kPZero)}},
    /*key2 */ {0xff100000u, 0x43, {PC(0x42,kPZero), PFRZ(), PDEF(), PC(0x225,kPZero), PCD(kPZero), PC(0x58,kPZero), PDEF(), PCA(kPZero), PDEF(), PC(0x133,kPZero)}},
    /*key3 */ {0, 0, {PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF()}},
    /*key4 */ {0xff100000u, 0x225, {PC(0x42,kPZero), PC(0x43,kPZero), PDEF(), PFRZ(), PCD(kPZero), PC(0x58,kPZero), PDEF(), PCA(kPZero), PDEF(), PC(0x133,kPZero)}},
    /*key5 */ {0xff100000u, -3, {PC(0x42,kPZero), PC(0x43,kPZero), PDEF(), PC(0x225,kPZero), PCD(kPZero), PC(0x58,kPZero), PDEF(), PFRZ(), PDEF(), PC(0x133,kPZero)}},
    /*key6 */ {0xff110000u, 0x58, {PC(0x42,kPZero), PC(0x43,kPZero), PDEF(), PC(0x225,kPZero), PFRZ(), PC(0x58,kPZero), PDEF(), PCA(kPZero), PDEF(), PC(0x133,kPZero)}},
    /*key7 */ {0, 0, {PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF()}},
    /*key8 */ {0xff230000u, -2, {PC(0x42,kPZero), PC(0x43,kPZero), PDEF(), PC(0x225,kPZero), PCD(kPZero), PFRZ(), PDEF(), PCA(kPZero), PDEF(), PC(0x133,kPZero)}},
    /*key9 */ {0, 0, {PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF(),PDEF()}},
    /*key10*/ {0xff100000u, 0x133, {PC(0x42,kPZero), PC(0x43,kPZero), PDEF(), PC(0x225,kPZero), PCD(kPZero), PC(0x58,kPZero), PDEF(), PCA(kPZero), PDEF(), PFRZ()}},
};

// FUN_0042ad10 (0x0042ad10, C2 note frontend_c1_to_c2_s6) — prompt row init,
// called with (0x40, 0x1ac, 0): tag, RGBA bytes 0xff at +0x0c..0x0f, scale
// 0x3f19999a (0.6) at +0x14, x=(float)0x40 at +0x18, y=(float)0x1ac at +0x1c,
// param_5=0 at +0x24.
void PromptRowInit(MenuRecord& r, std::uint32_t tag) {
    r.tag    = static_cast<std::int32_t>(tag);
    r.color  = static_cast<std::int32_t>(0xffffffffu);
    r.scale  = 0.6f;        // 0x3f19999a
    r.x      = 64.0f;       // (float)0x40
    r.y      = 428.0f;      // (float)0x1ac
    r.flag24 = 0;
}

// rel normalization: -1 -> 0 (0x00432b62..66 / 0x00432b84..89).
inline int PromptRelNormalize(int rel) { return (rel == -1) ? 0 : rel; }

// The strip body proper (post-head; key already resolved, mode folded into
// the rel the caller computed). Appends at the current record cursor.
void PromptStripAppend(int key, int cmp, int rel) {
    const unsigned k = static_cast<unsigned>(key) - 1u;
    if (k > 9u) return;                            // ja 0x43305b
    const PromptKindRow& row = kPromptRows[k];
    if (row.tag == 0) return;                      // keys 3/7/9: no row
    if (g_record_count >= kMaxRecords) return;     // standalone bounds guard
    MenuRecord& r = g_records[g_record_count];
    PromptRowInit(r, row.tag);
    if (row.sec == -2)      r.sec_id = static_cast<std::int32_t>(ModeCodeLookup());
    else if (row.sec == -3) { r.sec_id = 0x48; r.sec_id = 0x13; }
    else if (row.sec == -4) {
        r.sec_id = 0x42;                           // 0x432bbf
        // FUN_0042b920 = ConstantGetter22 (C3): returns 0x16. Gate hides the
        // back glyph when pushing the root/title screen (cmp == 22).
        if (cmp == 0x16) r.sec_id = -1;
    } else {
        r.sec_id = row.sec;
    }
    const unsigned ri = static_cast<unsigned>(rel) - 1u;
    const PromptCell cell = (ri > 9u) ? PDEF() : row.cells[ri];
    if (cell.a9c0)        r.prim_id = static_cast<std::int32_t>(ModeCodeLookup());
    else if (cell.dbl48)  { r.prim_id = 0x48; r.prim_id = 0x13; }
    else                  r.prim_id = cell.prim;
    switch (cell.fin) {
        case kPZero:    r.type = 0;       r.slide = 0x1ff; break;
        case kPFrozen:  r.type = 0x1000;  r.slide = 0x1ff; break;
        case kPDefault: r.type = 1;       r.slide = 0;     break;
    }
    ++g_record_count;                              // ++(*rec_index)
}

} // namespace

// FUN_004325c0 (0x004325c0) - Menu Slide Animation Tick. Walks every record
// (stride 0xd ints; piVar6 = record+0x10) and advances its slide counter
// (record+0x10) toward the settled state, keyed on the record tag (record+0x00)
// and type/state (record+0x04; 0x1000 = frozen). FAITHFUL semantics port:
//   - list-item tag (0xff040000) with type 0 or 2: slide += delta (delta<0);
//     when slide < 1 -> freeze (type 2 -> slide=0x1ff,type=0x1000; type 0 ->
//     slide=0,type=1, settled at final Y).
//   - other tags with type 0/2 and the 0xff0e/0d-class: slide += delta; when
//     slide > 399 -> clamp 0x1ff, freeze.
// `delta` stands in for FUN_004a2c48 (per-frame time-scaled step). The standalone
// drives a fixed step per RenderFrame; sign matches the original (counts down).
// Returns true once every record is frozen (all slides settled) = animation done.
// --------------------------------------------------------------------------
// Frame clock — verbatim port of FUN_00493480 (0x00493480), disasm-verified
// 2026-06-09 (pool0, anchor BDCAE093...). The original's ms source is
// FUN_00493390 (CALL at 0x00493480); the standalone's clock supplies raw_ms.
//   - snap bands (0x0049348a..0x004934cf): 47..53->50, 97..103->100,
//     147..153->150, 197..203->200;
//   - carry accumulator DAT_007719d4; ticks = (carry+ms)/50 with remainder
//     kept (0x004934e3: magic-mul 0x51eb851f >>4 = /50);
//   - remainder > 47 rounds up one tick and zeroes the carry (0x004934fd);
//     remainder < 3 drops the carry (0x00493502);
//   - DAT_007f1000 = ticks*50 (0x00493514);
//     DAT_007f1004 = DAT_007f1000 * 3.3333333e-4f (_DAT_005cc948 @0x005cc948
//     = 0x39aec33e = 1/3000) (0x0049351f..0x0049352b).
int   g_tick_carry = 0;     // DAT_007719d4
int   g_tick_ms    = 0;     // DAT_007f1000
float g_frame_dt   = 0.0f;  // DAT_007f1004
// DAT_0067eca4 model: frontend phase. The menu-item draw loop runs at phase
// 2..3 (FUN_0043c5b0 gate ladder, port-spec table); the anim tick selects its
// base speed at 0x004325c8: CMP [0067eca4],4; JL -> 1.0f (0x005cc320) else
// 2.0f (0x005cc574). The standalone sits in the "menu active" phase (2).
int   g_anim_phase = 2;     // DAT_0067eca4 model
int   g_anim_gate  = 1;     // DAT_0067e914 (anim-system gate; menu = nonzero)

void FrameClockUpdate(int raw_ms) {
    int ms = raw_ms;
    if      (ms >= 0x2f && ms <= 0x35) ms = 0x32;   // 0x0049348a..94
    else if (ms >= 0x61 && ms <= 0x67) ms = 0x64;   // 0x0049349b..a5
    else if (ms >= 0x93 && ms <= 0x99) ms = 0x96;   // 0x004934ac..ba
    else if (ms >= 0xc5 && ms <= 0xcb) ms = 0xc8;   // 0x004934c1..cf
    int acc   = g_tick_carry + ms;                  // 0x004934d4..da
    int ticks = 0;
    if (acc >= 0x32) { ticks = acc / 50; acc -= ticks * 50; }  // 0x004934e3..f2
    g_tick_carry = acc;                             // 0x004934f7
    if      (acc > 0x2f) { ++ticks; g_tick_carry = 0; }        // 0x004934fd
    else if (acc < 3)    { g_tick_carry = 0; }                 // 0x00493502
    g_tick_ms  = ticks * 0x32;                      // 0x00493511..14
    g_frame_dt = static_cast<float>(g_tick_ms) * 3.3333333e-4f; // 0x0049351f
}

// FUN_004a2c48 (0x004a2c48) is the MSVC _ftol2-style float->int helper:
// FISTP round-to-nearest, then sign-aware carry adjust (0x004a2c6b..0x004a2ca5)
// = TRUNCATION toward zero. (The hooks.csv C2 note calling it "banker's
// rounding" is wrong — corrected in CHANGELOG 2026-06-09.) A plain C integer
// cast is exactly that truncation.
inline int AnimStep(float v) { return static_cast<int>(v); }

// Float view of record+0x08: the default-tag anim path (incl. the 0xff080000
// prompt class) accumulates a FLOAT in the +0x08 cell that item rows use as
// the int row_index (0x00432751..0x00432777 stores raw 0x43340000 = 180.0f).
inline float F8Get(const MenuRecord& r) {
    float f; std::memcpy(&f, &r.row_index, 4); return f;
}
inline void F8Set(MenuRecord& r, float f) { std::memcpy(&r.row_index, &f, 4); }

// FUN_004325c0 (0x004325c0) — verbatim port from the 2026-06-09 pool0 disasm.
// Walks the first 30 records (ESI 0x898ad0 stride 0x34 while < 0x8990e8 =
// records 0..29). Base speed held in ST1 for the whole walk (see g_anim_phase
// above). Per-tag steps, each "FLD DAT_007f1004; FMUL ST1(base); FMUL K;
// CALL 0x004a2c48 (trunc)":
//   0xff040000 item, type 0/2:  K=-2000.0f (0x005cd904, @0x00432682); settle
//     on <= 0: type2 -> slide=0x1ff freeze; type0 -> slide=0,type=1 and Y
//     recompute (0x004326b8): Y = Q_ListBaseY(DAT_0067ece0, 0x1e) +
//     rec[+0x20]*0x1e; screen 24 (&DAT_005f7370): Q(0x1c)+rec[+0x20]*0x1c
//     - 58.0f (_DAT_005cd900); screen 18 (&DAT_005f72a0): Y -= 58.0f.
//   0xff040000 item, other type: K=+1000.0f (0x005cc9fc, @0x0043264e);
//     >= 0x190 -> slide=0x1ff, freeze.
//   {0xff110000,0xff000000,0xff120000,0xff130000,0xff230000,0xff100000}
//     (LAB_0043277c class), type 0/2: K=-800.0f (0x005cd8f8, @0x0043279b);
//     settle on <= 0 (type2 freeze / type0 -> 0, type=1; no Y recompute);
//     other type: K=+500.0f (0x005ccd04, @0x00432788) joining the 0x190 clamp.
//   any other tag (incl. 0 and the prompt class): float path (@0x00432751) —
//     f8 += DAT_007f1004 * 300.0f (0x005cd8fc); >= 180.0f (0x005cd09c) ->
//     f8 = 180.0f raw 0x43340000, freeze. (No base multiplier on this path.)
// Returns 1 when nothing is left animating; a record counts as animating when
// tag != 0 && type != 0x1000 && DAT_0067e914 != 0 (0x004325f5..0x0043260d).
bool Anim_Tick() {
    const float base = (g_anim_phase >= 4) ? 2.0f : 1.0f;   // 0x004325c8..db
    const NavSlot& slot = g_stack[g_nav_depth];
    int all_settled = 1;                                    // [ESP+0x10]
    for (int i = 0; i < 30 && i < kMaxRecords; ++i) {       // 0x898ad0..0x8990e8
        MenuRecord& r = g_records[i];
        const std::uint32_t tag = static_cast<std::uint32_t>(r.tag);
        if (tag != 0 && r.type != 0x1000 && g_anim_gate != 0)
            all_settled = 0;                                // 0x004325f5..0d
        const bool slide_class =
            (tag == 0xff110000u || tag == 0xff000000u || tag == 0xff120000u ||
             tag == 0xff130000u || tag == 0xff230000u || tag == 0xff100000u);
        if (tag == 0xff040000u) {
            if (r.type == 0 || r.type == 2) {
                r.slide += AnimStep(g_frame_dt * base * -2000.0f);
                if (r.slide <= 0) {                         // JG 0x004327d7
                    if (r.type == 2) { r.slide = 0x1ff; r.type = 0x1000; }
                    else {                                  // 0x004326b8 settle
                        const int row20 = r.pad20;          // [ESI+0x10]=+0x20
                        r.slide = 0; r.type = 1;
                        float y = static_cast<float>(
                            Q_ListBaseY(slot.item_count, 0x1e) + row20 * 0x1e);
                        if (slot.screen_id == 24) {         // &DAT_005f7370
                            y = static_cast<float>(
                                    Q_ListBaseY(slot.item_count, 0x1c) +
                                    row20 * 0x1c) - 58.0f;  // _DAT_005cd900
                        }
                        if (slot.screen_id == 18) {         // &DAT_005f72a0
                            y -= 58.0f;
                        }
                        r.y = y;                            // FSTP [ESI+0xc]
                    }
                }
            } else {                                        // 0x0043264e
                r.slide += AnimStep(g_frame_dt * base * 1000.0f);
                if (r.slide >= 0x190) { r.slide = 0x1ff; r.type = 0x1000; }
            }
        } else if (slide_class) {                           // LAB_0043277c
            if (r.type == 0 || r.type == 2) {               // 0x0043279b
                r.slide += AnimStep(g_frame_dt * base * -800.0f);
                if (r.slide <= 0) {
                    if (r.type == 2) { r.slide = 0x1ff; r.type = 0x1000; }
                    else             { r.slide = 0;     r.type = 1; }
                }
            } else {                                        // 0x00432788
                r.slide += AnimStep(g_frame_dt * base * 500.0f);
                if (r.slide >= 0x190) { r.slide = 0x1ff; r.type = 0x1000; }
            }
        } else {                                            // 0x00432751 float
            const float f = g_frame_dt * 300.0f + F8Get(r);
            F8Set(r, f);
            if (f >= 180.0f) { F8Set(r, 180.0f); r.type = 0x1000; }
        }
    }
    return all_settled != 0;
}

} // namespace

// --------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------

// Frontend main-menu root screen. ROOT-CAUSE FIX (user review #12, 2026-06-12):
// screen 0 (kT0) is the IN-RACE PAUSE menu ("Powerup Status Select": Continue/
// Options/Restart Race/Quit Race/Quit Game) — FUN_0043d2a0(0,2) ENTERS A RACE.
// The real frontend MAIN MENU is screen 1 (kT1: Single Player/Multi Player/
// Options, title 0x43 "Game Type Select"); FUN_0043d7c0 returns to it via
// FUN_0043d2a0(1,0). Rooting the frontend nav at 0 made "back" reveal the pause
// menu and ESC quit. The frontend-only standalone must root at the main menu.
static constexpr int kMainMenuScreen = 1;

void Nav_Init() {
    g_nav_depth = 0;
    std::memset(g_stack, 0, sizeof(g_stack));
    NavSlot& s = g_stack[0];
    s.screen_id  = kMainMenuScreen;
    s.desc_table = TableForScreen(kMainMenuScreen);
    s.slide_src  = s.desc_table;
    s.cursor     = 0;
    s.item_count = CountItems(s.desc_table);
    PlaceCursor(s);
    BuildRecords(s);
    // #12b: append the footer prompt strip for the ROOT too (Nav() only did this
    // on push/pop, so rooting the frontend here dropped the footer). For a kind-1
    // screen the -4 key resolves sec_id to 0x42 = "(glyph) Select" (USA.DAT piz)
    // — Select only, NO Back (matches #11: no Back on the main menu). cmp must NOT
    // be 0x16 (that sentinel blanks the whole row).
    PromptStripAppend(KvLookup(s.desc_table, kTagPrompt, 0), kMainMenuScreen, 0);
    // (No fade raise here: Nav_Init runs at BOOT, before the title. The
    // original's pair stays BSS-zero through the title — no wash there — and
    // is raised by the title->menu reload. The raise lives at the phase-3
    // entries in exe_main; reload/select ops below still raise it.)
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
        // FUN_00432b30 mode-0 head (0x00432b70): rel = the kind of the screen
        // being LEFT, looked up at the PRE-commit depth slot; depth==0 skips
        // the lookup entirely (rel stays 0). -1 normalized (0x00432b84..89).
        const int strip_rel = (g_nav_depth != 0)
            ? PromptRelNormalize(KvLookup(g_stack[g_nav_depth].desc_table,
                                          kTagPrompt, 0))
            : 0;
        g_nav_depth = d;                                // commit push (Phase 7)
        BuildRecords(s);
        // Phase 7 strip call (0x0043d79c): key = SHOWN screen kind, cmp =
        // pushed screen id (probe_prompt_calls.json: push -> mode 0).
        PromptStripAppend(KvLookup(s.desc_table, kTagPrompt, 0),
                          screen_id, strip_rel);
    } else if (dir == kNavPop) {
        if (g_nav_depth < 1) return;                    // can't pop past root
        --g_nav_depth;                                  // DAT_0067e9f8 = depth-1
        NavSlot& s = g_stack[g_nav_depth];
        s.cursor = (s.saved_cursor >= 0) ? s.saved_cursor : 0;
        PlaceCursor(s);
        BuildRecords(s);
        // FUN_00432b30 mode-1 head (pop-reveal, 0x00432b52..6e): rel = the
        // POPPED screen's kind read through the depth+1 inc/dec window (the
        // popped slot is still intact). key = revealed screen kind, cmp = 0
        // (probe_prompt_calls.json: pop rows).
        PromptStripAppend(KvLookup(s.desc_table, kTagPrompt, 0), 0,
                          PromptRelNormalize(KvLookup(
                              g_stack[g_nav_depth + 1].desc_table,
                              kTagPrompt, 0)));
    } else { // kNavReload
        // 0x0043d2c2: reload ops raise the arc-wash fade target to 0xff
        // (push/pop do NOT - the head dispatch skips the setter for dir<2).
        LogoOverlayFadeSet(0xff, -1);
        NavSlot& s = g_stack[g_nav_depth];
        s.item_count = CountItems(s.desc_table);
        PlaceCursor(s);
        BuildRecords(s);
        // FUN_00432b30 mode-2 head zeroes key (0x00432b45) -> the key-1 bounds
        // check (0x00432b8e..96) always early-returns: reloads append NO row.
    }
    g_last_dir = dir;                                   // DAT_0067e844
}

// Force the current screen's records to slide in (slide=0x1ff, type animating)
// so the whole menu sweeps in on entry. Used for the title->main-menu
// transition, where the root would otherwise appear with no motion (slide=0)
// and read as a pop/fade. The arc-wash fade is raised too (reload-style).
void Nav_AnimateIn() {
    LogoOverlayFadeSet(0xff, -1);
    for (int i = 0; i < g_record_count; ++i) {
        MenuRecord& r = g_records[i];
        const std::uint32_t tag = static_cast<std::uint32_t>(r.tag);
        if (tag == 0) continue;
        // Only the list-item / back / prompt rows animate (type 0/2); leave
        // frozen-by-design rows alone.
        if (r.type == 1 || r.type == 0) { r.slide = 0x1ff; r.type = 0; }
    }
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

    // FUN_0043dfd0 select path raises the arc-wash fade (0x0043f9d7,
    // PUSH 0xff -> FUN_00472640) before dispatching the action.
    LogoOverlayFadeSet(0xff, -1);
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

void              Nav_FrameClockUpdate(int raw_ms) { FrameClockUpdate(raw_ms); }
bool              Nav_AnimTick() { return Anim_Tick(); }

// Test-support accessors for the internal push-map machinery (linked by the
// logic harness re/analysis/standalone_menu_sm/harvest/navsm_test.cpp for the
// 34-screen reachability pass; not part of the runtime API).
const std::uint32_t* NavTest_TableForScreen(int s) { return TableForScreen(s); }
std::uint32_t NavTest_ItemActionCode(const std::uint32_t* t, int i) { return ItemActionCode(t, i); }
int NavTest_ActionToScreen(std::uint32_t a, int kind) { return ActionToScreen(a, kind); }
int NavTest_KvLookup(const std::uint32_t* t, std::uint32_t tag, int n) { return KvLookup(t, tag, n); }
// Vertical-grow fraction for the menu-entry animation (0 = collapsed to a
// thin line, 1 = full height). The original animates item plates by GROWING
// their height (capture: plate h 2->26px, x constant, centered on row y —
// NOT a horizontal slide). The slide counter (record+0x10) runs 0x1ff->0 as
// the row settles, so grow = 1 - slide/0x1ff. Settled/non-animating rows = 1.
float             Nav_RecordGrow(int rec_index) {
    if (rec_index < 0 || rec_index >= g_record_count) return 1.0f;
    const MenuRecord& r = g_records[rec_index];
    if (r.type != 0 && r.type != 2) return 1.0f;        // settled / frozen
    float g = 1.0f - static_cast<float>(r.slide) / 511.0f;
    return g < 0.0f ? 0.0f : (g > 1.0f ? 1.0f : g);
}

int               Nav_RecordSlide(int rec_index) {
    if (rec_index < 0 || rec_index >= g_record_count) return 0;
    const MenuRecord& r = g_records[rec_index];
    // The horizontal entry offset only applies while the record is actually
    // sliding in (type 0/2). The post-settle count-up phase (type 1 -> frozen
    // 0x1000 holding slide=0x1ff, per FUN_004325c0's item lifecycle) leaves
    // the record at its final position.
    if (r.type != 0 && r.type != 2) return 0;
    return r.slide;
}

int               Nav_PromptId() {
    // FUN_0043d2a0 Phase 7 reads the screen's prompt-strip string id via
    // FUN_0042ad90(EBX=0xff080000, EDI=0) and passes it to FUN_00432b30 (the
    // prompt-strip glyph builder). The standalone resolves the SAME id and draws
    // it as a glyph run at the strip position (virtual X=0x40, Y=0x1ac, scale
    // 0x3f19999a per FUN_0042ad10). Returns -1 if the table has no prompt entry.
    return KvLookup(g_stack[g_nav_depth].desc_table, kTagPrompt, 0);
}

bool              Nav_ItemEnabled(int row_index) {
    if (row_index < 0 || row_index >= kMaxItems) return false;
    return g_stack[g_nav_depth].avail[row_index] == 1;
}

MenuGameState&    Nav_GameState()    { return g_game_state; }
void              Nav_GameStateReset() { g_game_state = kFreshState; }

// Scoped port of FUN_00404e80 (0x00404e80) step 1 — see header. The span
// model below stands in for live 0x007f0a40..0x007f0f60 (0x520 bytes,
// 0x148 dwords; REP MOVSD at 0x00404e91 from DAT_00827d98 = save_buf+0x24A40).
static unsigned char g_save_span[0x520];

bool Nav_GameStateLoadSave(const unsigned char* data, unsigned len) {
    constexpr unsigned kSaveSize  = 0x24FA0;     // gamesave.bin total
    constexpr unsigned kSpanFile  = 0x24A40;     // 0x00827d98 - 0x00803358
    constexpr unsigned kSpanBytes = 0x520;       // 0x148 dwords
    if (data == nullptr || len != kSaveSize) return false;
    // Caller-side gate: magic 0xDEADBEEF at +0 (written by 0x00404F37 on first
    // save). The shipped blank gamesave.bin has 0 here -> no savedata.
    std::uint32_t magic;
    std::memcpy(&magic, data, 4);
    if (magic != 0xDEADBEEFu) return false;

    std::memcpy(g_save_span, data + kSpanFile, kSpanBytes);

    // Re-derive the MenuGameState fields the grey-out/routing ports read,
    // straight from the restored span (offsets relative to 0x007f0a40):
    //   DAT_007f0f2c savedata gate  -> +0x4ec   (screen 1 item 3)
    //   DAT_007f0ad4 profile gate   -> +0x094   (screen 2 item 1)
    //   DAT_007f0a50[set*0x30]      -> +0x010 + set*0x30  (track unlock)
    //   DAT_007f0a58[set*0x30]      -> +0x018 + set*0x30  (car unlock)
    auto span32 = [](unsigned off) {
        std::int32_t v;
        std::memcpy(&v, g_save_span + off, 4);
        return v;
    };
    MenuGameState& gs = g_game_state;
    gs.has_savedata = span32(0x4ec);
    gs.has_profiles = span32(0x094);
    const unsigned set = static_cast<unsigned>(gs.cur_track_set) & 0xf;
    gs.unlock_track[0] = span32(0x010 + set * 0x30);
    gs.unlock_car[0]   = span32(0x018 + set * 0x30);
    return true;
}

} // namespace Frontend
} // namespace mashed_re
