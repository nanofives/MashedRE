// kvfind_equiv_test.cpp - dualcopy4: the exe's old MenuNavSM.cpp:KvLookup (copied VERBATIM from 9b0248128, kept here as OldKvLookup) against the
// shared Frontend/KvFind.cpp:Find42ad90 that replaced it, over random flat u32 streams. Exit 0 = every case equal.
//   cl /nologo /EHsc /std:c++17 kvfind_equiv_test.cpp ..\..\mashedmod\src\mashed_re\Frontend\KvFind.cpp /Fe:kvfind_equiv_test.exe
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

extern "C" std::uint32_t __cdecl Find42ad90(std::uint8_t* arr, std::uint32_t key, std::uint32_t n);

constexpr std::uint32_t kTermVal = 0xff070000u;

// ---- verbatim from 9b0248128 Frontend/MenuNavSM.cpp:378-398 -------------------------------------------------------------
static int OldKvLookup(const std::uint32_t* table, std::uint32_t tag_key, int occurrence) {
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
// ---- the new KvLookup (MenuNavSM.cpp, after this lane) ------------------------------------------------------------------
static int NewKvLookup(const std::uint32_t* table, std::uint32_t tag_key, int occurrence) {
    if (occurrence < 0) return -1;
    return static_cast<std::int32_t>(Find42ad90(
        reinterpret_cast<std::uint8_t*>(const_cast<std::uint32_t*>(table)), tag_key, static_cast<std::uint32_t>(occurrence)));
}

int main() {
    std::srand(0xD0C0);
    const std::uint32_t keys[] = {0xff000000u, 0xff040000u, 0xff080000u, 0xff050000u, 0xff140000u, 0xff060000u, kTermVal, 0u, 1u, 0xffffffffu, 0x12345678u};
    long cases = 0, bad = 0, nonneg1 = 0, neg1 = 0;
    for (int t = 0; t < 400000; ++t) {
        const int len = 1 + std::rand() % 12;
        std::vector<std::uint32_t> v;
        for (int i = 0; i < len; ++i) v.push_back(keys[std::rand() % 11]);
        if (std::rand() % 8) v.push_back(kTermVal);            // 7/8 terminated; the rest read into the padding
        for (int i = 0; i < 16; ++i) v.push_back(kTermVal);     // guard so an unterminated stream still ends
        const std::uint32_t key = keys[std::rand() % 11];
        const int occ = (std::rand() % 7) - 1;                  // -1 .. 5
        const std::uint32_t* tb = (std::rand() % 50 == 0) ? nullptr : v.data();
        const int a = OldKvLookup(tb, key, occ), b = NewKvLookup(tb, key, occ);
        ++cases;
        (a == -1 ? neg1 : nonneg1)++;
        if (a != b) {
            if (++bad < 6) std::printf("MISMATCH case %ld key=%08x occ=%d old=%d new=%d\n", cases, key, occ, a, b);
        }
    }
    std::printf("cases=%ld mismatches=%ld  (result -1: %ld, other: %ld)\n", cases, bad, neg1, nonneg1);
    return bad ? 1 : 0;
}
