// Mashed RE — RWS audio format-descriptor comparators cluster (c3-batch-j-s1).
//
// Cluster of 2 reimplementations promoted C2 -> C3 in c3-batch-j-s1:
//   0x005ac5f0  AudioFmtDescEqual       — 5-field AND equality comparator (2-arg)
//   0x005ac9e0  AudioFmtEntryMatch      — 6-condition format-table entry match (2-arg)
//
// Both newly unblocked by the `fmt_desc_pair_compare` arg_type added to
// re/frida/diff_template.js in commit 656273b.
//
// 0x005acaa0 AudioFmtDescPackUnpack (the 4-arg pack/unpack also unblocked by
// fmt_desc_pair_compare) was REFUSED in this session — see PROMOTION_QUEUE.md
// note: ~620-byte multi-branch serialise/deserialise body too dense to
// reimplement on a single-session budget without divergence risk on the
// per-field bswap ordering and fmt-key/extra-data fixup paths.
//
// Source analysis:
//   re/analysis/promote_c2_audio_rws/005ac5f0.md
//   re/analysis/promote_c2_audio_rws/005ac9e0.md
//
#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ac5f0  FUN_005ac5f0  AudioFmtDescEqual   (~80 bytes)
//
// undefined4 FUN_005ac5f0(int *param_1, int *param_2)
//   param_1, param_2 = format-descriptor pointers (28-byte layout).
//   Returns 1 if equal across 5 fields, 0 otherwise.
//
// Decompilation (verbatim from analysis note):
//   iVar1 = FUN_005adf30(param_1[1], param_2[1]);   // 16-byte fmt-key compare
//   if ((((iVar1 == 0)
//        && (*param_1 == *param_2))                                       // +0x00 equal
//       && ((char)param_1[3] == (char)param_2[3]))                        // +0x0c equal
//      && ((*(char *)((int)param_1 + 0xd) == *(char *)((int)param_2 + 0xd)) // +0x0d equal
//         && (((*(byte *)(param_1 + 6) ^ *(byte *)(param_2 + 6)) & 0xfd) == 0))) // flags & ~bit1
//     return 1;
//   return 0;
//
// Field layout (both ptrs, identical shape):
//   +0x00  u32   field[0] — direct equality
//   +0x04  16B   fmt-key  — compared via FUN_005adf30 (returns 0 on equal)
//   +0x0c  s8    char     — direct equality
//   +0x0d  s8    char     — direct equality
//   +0x18  u8    flags    — compared with mask 0xfd (bit1 / 0x02 ignored)
//
// Constants:
//   0xfd  (flags mask; bit1 excluded from comparison — dirty/written marker)
//
// Callee: FUN_005adf30 at 0x005adf30 (AudioFmtKeyCompare, C3) — 16-byte
// lexicographic memcmp returning -1/0/+1; we want 0 for equal.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl
AudioFmtDescEqual(int* param_1, int* param_2)
{
    typedef int (__cdecl *KeyCmpFn)(int a, int b);
    static constexpr std::uintptr_t kKeyCmp = 0x005adf30u;
    const auto keyCmp = reinterpret_cast<KeyCmpFn>(kKeyCmp);

    // 5-condition short-circuit AND. Order matches the decomp body exactly:
    //   1. fmt-key compare result == 0
    //   2. word at +0x00 equal
    //   3. byte at +0x0c equal (cast s8 to match sign-extension behaviour)
    //   4. byte at +0x0d equal
    //   5. flags byte at +0x18 equal under mask 0xfd
    const int keyEq = keyCmp(param_1[1], param_2[1]);
    if (keyEq != 0) return 0u;

    if (param_1[0] != param_2[0]) return 0u;

    // param_1[3] is the word containing the byte at +0x0c — the decomp
    // narrows via (char) cast (sign-extending low byte).
    if (static_cast<char>(param_1[3]) != static_cast<char>(param_2[3])) return 0u;

    // Byte read at +0x0d uses ((int)ptr + 0xd) form in decomp.
    const std::int8_t p1_0d = *reinterpret_cast<std::int8_t*>(
        reinterpret_cast<std::uintptr_t>(param_1) + 0x0du);
    const std::int8_t p2_0d = *reinterpret_cast<std::int8_t*>(
        reinterpret_cast<std::uintptr_t>(param_2) + 0x0du);
    if (p1_0d != p2_0d) return 0u;

    // Flags byte at +0x18 (param_1 + 6 words = +0x18 bytes), mask 0xfd:
    //   bit1 (0x02) is intentionally ignored — dirty/written marker.
    const std::uint8_t f1 = *reinterpret_cast<std::uint8_t*>(param_1 + 6);
    const std::uint8_t f2 = *reinterpret_cast<std::uint8_t*>(param_2 + 6);
    if (((f1 ^ f2) & 0xfdu) != 0u) return 0u;

    return 1u;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(AudioFmtDescEqual, 0x005ac5f0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ac9e0  FUN_005ac9e0  AudioFmtEntryMatch   (~100 bytes)
//
// undefined4 FUN_005ac9e0(undefined4 *param_1, uint *param_2)
//   param_1 = format-table entry pointer.
//   param_2 = candidate format-descriptor pointer.
//   Returns 1 if all six conditions match, 0 otherwise.
//
// Decompilation (verbatim from analysis note):
//   if ((((*(char *)((int)param_2 + 0xd) == *(char *)((int)param_1 + 5))   // cnd1
//        && ((char)param_2[3] == *(char *)(param_1 + 1)))                   // cnd2
//       && ((uint)param_1[2] <= *param_2))                                  // cnd3
//      && (*param_2 <= (uint)param_1[3])) {                                 // cnd4
//     iVar1 = FUN_005adf30(param_2[1], *param_1);                           // cnd5
//     if ((iVar1 == 0) &&
//         (((*(byte *)(param_1 + 4) ^ (byte)param_2[6]) & 1) == 0))         // cnd6
//       return 1;
//   }
//   return 0;
//
// Conditions:
//   1. channel byte: entry +0x05 (s8) == candidate +0x0d (s8)
//   2. format char:  entry +0x04 (s8) == candidate +0x0c (s8 via lo-byte of word)
//   3. min sample-rate: entry +0x08 (u32) <= candidate +0x00 (u32)
//   4. max sample-rate: candidate +0x00 (u32) <= entry +0x0c (u32)
//   5. fmt-key compare: FUN_005adf30(candidate +0x04, entry +0x00) == 0
//   6. flag bit0: (entry +0x10 ^ candidate +0x18) & 1 == 0
//
// Callee: FUN_005adf30 at 0x005adf30 (AudioFmtKeyCompare, C3).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl
AudioFmtEntryMatch(std::uint32_t* param_1, std::uint32_t* param_2)
{
    typedef int (__cdecl *KeyCmpFn)(int a, int b);
    static constexpr std::uintptr_t kKeyCmp = 0x005adf30u;
    const auto keyCmp = reinterpret_cast<KeyCmpFn>(kKeyCmp);

    // cnd1: channel byte equal (entry +0x05 vs candidate +0x0d, both s8).
    const std::int8_t e_05 = *reinterpret_cast<std::int8_t*>(
        reinterpret_cast<std::uintptr_t>(param_1) + 0x05u);
    const std::int8_t c_0d = *reinterpret_cast<std::int8_t*>(
        reinterpret_cast<std::uintptr_t>(param_2) + 0x0du);
    if (c_0d != e_05) return 0u;

    // cnd2: format char (entry +0x04 vs candidate +0x0c).
    //   entry: *(char *)(param_1 + 1) -- word-stride byte at +0x04
    //   cand:  (char)param_2[3]       -- low byte of word at +0x0c
    const std::int8_t e_04 = *reinterpret_cast<std::int8_t*>(param_1 + 1);
    const std::int8_t c_0c = static_cast<std::int8_t>(param_2[3] & 0xffu);
    if (c_0c != e_04) return 0u;

    // cnd3: min sample-rate (unsigned compare): entry +0x08 <= cand +0x00.
    const std::uint32_t e_min   = param_1[2];
    const std::uint32_t c_value = param_2[0];
    if (e_min > c_value) return 0u;

    // cnd4: max sample-rate (unsigned compare): cand +0x00 <= entry +0x0c.
    const std::uint32_t e_max = param_1[3];
    if (c_value > e_max) return 0u;

    // cnd5: 16-byte fmt-key compare.
    //   entry: *param_1 (key ptr at +0x00)
    //   cand:  param_2[1] (key ptr at +0x04)
    const int keyEq = keyCmp(static_cast<int>(param_2[1]),
                             static_cast<int>(param_1[0]));
    if (keyEq != 0) return 0u;

    // cnd6: flag bit0 agreement.
    //   entry: *(byte *)(param_1 + 4) = byte at +0x10
    //   cand:  (byte)param_2[6]       = low byte of word at +0x18
    const std::uint8_t e_10 = *reinterpret_cast<std::uint8_t*>(param_1 + 4);
    const std::uint8_t c_18 = static_cast<std::uint8_t>(param_2[6] & 0xffu);
    if (((e_10 ^ c_18) & 0x01u) != 0u) return 0u;

    return 1u;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(AudioFmtEntryMatch, 0x005ac9e0);
