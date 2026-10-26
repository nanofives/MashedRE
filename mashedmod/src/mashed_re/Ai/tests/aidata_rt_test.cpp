// Round-trip identity test for the AI path-data writer FUN_00423540 (D4-WSF-TRACKDATA-RT).
// parse (AiData_Validate + AiData_LoadInto, the FUN_004235b0 verbatim copy) -> emit
// (AiData_Emit, the FUN_00423540 byte order) and compare with the input bytes.
// Test-only: not in either rsp list. Build:
//   cl /EHsc /std:c++17 aidata_rt_test.cpp
//   aidata_rt_test.exe <AI%d.AI> [...]                 shipped members
//   aidata_rt_test.exe --orig <written.AI> <mem.bin>   original writer output vs
//                                                      emit of the DAT_007f1a9c snapshot
#include "../AiData.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

static std::vector<std::uint8_t> slurp(const char* p) {
    std::vector<std::uint8_t> b;
    std::FILE* f = std::fopen(p, "rb");
    if (!f) return b;
    std::fseek(f, 0, SEEK_END); long n = std::ftell(f); std::fseek(f, 0, SEEK_SET);
    b.resize(n > 0 ? n : 0);
    if (n > 0) b.resize(std::fread(b.data(), 1, n, f));
    std::fclose(f);
    return b;
}

// Byte-diff summary: count, first and last differing offsets.
static int diffcount(const std::uint8_t* a, const std::uint8_t* b, std::size_t n,
                     long* first, long* last) {
    int d = 0; *first = -1; *last = -1;
    for (std::size_t i = 0; i < n; ++i)
        if (a[i] != b[i]) { if (*first < 0) *first = (long)i; *last = (long)i; ++d; }
    return d;
}

static const char* base(const char* p) {
    const char* s = p;
    for (const char* c = p; *c; ++c) if (*c == '/' || *c == '\\') s = c + 1;
    return s;
}

int main(int argc, char** argv) {
    std::printf("kAiWriterVersion = 0x%08x (FUN_004cc580 pack of 0x37002, 10)\n",
                Ai::kAiWriterVersion);
    std::vector<std::uint8_t> out(Ai::kAiChunkTotal);
    std::vector<std::uint8_t> image(Ai::kAiPayloadSize);

    if (argc == 4 && std::strcmp(argv[1], "--orig") == 0) {
        auto written = slurp(argv[2]);
        auto mem = slurp(argv[3]);
        if (written.size() != Ai::kAiChunkTotal || mem.size() != Ai::kAiPayloadSize) {
            std::printf("[1/1] %s FAIL sizes written=%zu mem=%zu\n", base(argv[2]),
                        written.size(), mem.size());
            return 1;
        }
        Ai::AiData_Emit(mem.data(), out.data());
        long f, l;
        int d = diffcount(out.data(), written.data(), out.size(), &f, &l);
        std::printf("[1/1] %s %s ndiff=%d first=%ld last=%ld\n", base(argv[2]),
                    d == 0 ? "PASS" : "FAIL", d, f, l);
        return d == 0 ? 0 : 1;
    }

    int n = argc - 1, pass = 0, verOnly = 0;
    for (int i = 1; i < argc; ++i) {
        auto raw = slurp(argv[i]);
        bool ok = Ai::AiData_LoadInto(raw.data(), raw.size(), image.data());
        if (!ok) { std::printf("[%d/%d] %s FAIL parse\n", i, n, base(argv[i])); continue; }
        Ai::AiData_Emit(image.data(), out.data());
        long f, l;
        int d = diffcount(out.data(), raw.data(), raw.size(), &f, &l);
        std::uint32_t ver; std::memcpy(&ver, raw.data() + 8, 4);
        // A difference confined to bytes 8..11 is the version stamp only.
        bool stampOnly = d > 0 && f >= 8 && l <= 11;
        if (d == 0) ++pass;
        if (stampOnly) ++verOnly;
        std::printf("[%d/%d] %s %s ndiff=%d first=%ld last=%ld file_ver=0x%08x\n", i, n,
                    base(argv[i]), d == 0 ? "PASS" : (stampOnly ? "STAMP" : "FAIL"),
                    d, f, l, ver);
    }
    std::printf("identical %d/%d, version-stamp-only %d, other %d\n", pass, n, verOnly,
                n - pass - verOnly);
    return (pass + verOnly == n) ? 0 : 1;
}
