// maskcheck_42d3e0.cpp - dualcopy4 G-MASK for 0x0042d3e0. Host test, no game.
//   NEW = Frontend/MenuEntryZero.h:MenuEntryZeroBody ; OLD = Frontend/MenuInit.cpp:MenuEntryArrayInit as it stood at 9b0248128
//   (copied below with the absolute base/end replaced by a buffer, the 14 stores otherwise verbatim).
//   EXPECT = the 14 stores of the unpatched original (disasm_42d3e0.txt): dwords entry+0,+4,+8, bytes +12..+15, dwords +16,+20,+24,+28,+36,+40,+44.
// Exit 0 when NEW == EXPECT for every one of 30 entries and the OLD body differs at exactly the predicted offsets (PREREG_dualcopy4_b4.md section 4).
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <set>
#include <vector>

#include "../../mashedmod/src/mashed_re/Frontend/MenuEntryZero.h"

static constexpr int kStride = 0x34, kEntries = 30, kLead = 16, kTail = 2 * kStride;

static void OldBody(std::uint8_t* arr) {                       // arr = array base (0x00898ac0 analogue)
    std::uint32_t* puVar1 = reinterpret_cast<std::uint32_t*>(arr + 4);
    const std::uint32_t* const end = reinterpret_cast<const std::uint32_t*>(arr + 4 + 0x618);
    while (puVar1 < end) {
        std::uint8_t* base = reinterpret_cast<std::uint8_t*>(puVar1) - 4;
        *reinterpret_cast<std::uint32_t*>(base - 4)  = 0u;
        *reinterpret_cast<std::uint32_t*>(base +  0) = 0u;
        *reinterpret_cast<std::uint32_t*>(base +  4) = 0u;
        *reinterpret_cast<std::uint32_t*>(base +  8) = 0u;
        *reinterpret_cast<std::uint8_t*> (base +  9) = 0u;
        *reinterpret_cast<std::uint8_t*> (base + 10) = 0u;
        *reinterpret_cast<std::uint8_t*> (base + 11) = 0u;
        *reinterpret_cast<std::uint8_t*> (base + 12) = 0u;
        *reinterpret_cast<std::uint32_t*>(base + 16) = 0u;
        *reinterpret_cast<std::uint32_t*>(base + 20) = 0u;
        *reinterpret_cast<std::uint32_t*>(base + 24) = 0u;
        *reinterpret_cast<std::uint32_t*>(base + 32) = 0u;
        *reinterpret_cast<std::uint32_t*>(base + 36) = 0u;
        *reinterpret_cast<std::uint32_t*>(base + 40) = 0u;
        puVar1 += 0xd;
    }
}

static std::vector<std::uint8_t> Expect() {                    // buffer layout: kLead bytes, kEntries+2 entries, FF everywhere then the 14 stores
    std::vector<std::uint8_t> b(kLead + (kEntries + 2) * kStride, 0xFF);
    for (int k = 0; k < kEntries; ++k) {
        std::uint8_t* e = b.data() + kLead + k * kStride;
        for (int o : {0, 4, 8, 16, 20, 24, 28, 36, 40, 44}) std::memset(e + o, 0, 4);
        std::memset(e + 12, 0, 4);                              // four byte stores at +12..+15
    }
    return b;
}

int main() {
    const std::vector<std::uint8_t> want = Expect();
    std::vector<std::uint8_t> a(want.size(), 0xFF), b(want.size(), 0xFF);
    mashed_re::Frontend::MenuEntryZeroBody(a.data() + kLead, kEntries);
    OldBody(b.data() + kLead);
    int newBad = 0;
    for (size_t i = 0; i < a.size(); ++i) newBad += a[i] != want[i];
    std::printf("NEW MenuEntryZeroBody vs the 14 stores of the original: %d differing bytes of %zu  -> %s\n", newBad, a.size(), newBad ? "FAIL" : "PASS");
    std::set<int> extra, missing;                                // entry-relative offsets (can be negative: before an entry)
    int oldBad = 0;
    for (size_t i = 0; i < b.size(); ++i) {
        if (b[i] == want[i]) continue;
        ++oldBad;
        const int rel = static_cast<int>(i) - kLead;
        int off = ((rel % kStride) + kStride) % kStride;
        if (rel < 0) off = rel;                                  // before entry 0
        (b[i] == 0 ? extra : missing).insert(off);
    }
    std::printf("OLD MenuEntryArrayInit (9b0248128) vs the same: %d differing bytes\n  cleared but the original keeps (entry-relative byte offsets):", oldBad);
    for (int o : extra) std::printf(" %d", o);
    std::printf("\n  kept but the original clears:");
    for (int o : missing) std::printf(" %d", o);
    std::printf("\n");
    return newBad ? 1 : 0;
}
