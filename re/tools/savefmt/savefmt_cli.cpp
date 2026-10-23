// savefmt_cli: drives the standalone's own gamesave format code (Save/GameSaveFormat.h)
// from the command line, for the savelink round-trip (verify/savelink_20261023).
//   savefmt_cli build <shipped.bin> <out.bin> <counter> [<row> <col> <value>]...
//   savefmt_cli parse <in.bin>      prints "PARSE ok|fail", "COUNTER n", then the span as 13 rows of 12 dwords
#include "../../../mashedmod/src/mashed_re/Save/GameSaveFormat.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
using namespace mashed_re::Save;

static bool readAll(const char* p, std::vector<unsigned char>& v) {
    FILE* f = std::fopen(p, "rb"); if (!f) return false;
    std::fseek(f, 0, SEEK_END); long n = std::ftell(f); std::fseek(f, 0, SEEK_SET);
    v.resize(static_cast<size_t>(n));
    size_t got = std::fread(v.data(), 1, v.size(), f); std::fclose(f);
    return got == v.size();
}

int main(int argc, char** argv) {
    if (argc >= 5 && std::string(argv[1]) == "build") {
        std::vector<unsigned char> in;
        if (!readAll(argv[2], in) || in.size() != kSaveSize) { std::puts("bad shipped file"); return 2; }
        unsigned char span[kSpanBytes];
        std::memcpy(span, in.data() + kSpanOff, kSpanBytes);
        const std::uint32_t counter = static_cast<std::uint32_t>(std::strtoul(argv[4], nullptr, 0));
        for (int i = 5; i + 2 < argc; i += 3) {
            const unsigned row = std::strtoul(argv[i], nullptr, 0), col = std::strtoul(argv[i + 1], nullptr, 0);
            const std::uint32_t val = static_cast<std::uint32_t>(std::strtoul(argv[i + 2], nullptr, 0));
            std::memcpy(span + row * kRowStride + col * 4, &val, 4);
        }
        std::vector<unsigned char> out(kSaveSize);
        BuildImage(span, counter, out.data());
        FILE* f = std::fopen(argv[3], "wb"); if (!f) return 3;
        std::fwrite(out.data(), 1, out.size(), f); std::fclose(f);
        return 0;
    }
    if (argc >= 3 && std::string(argv[1]) == "parse") {
        std::vector<unsigned char> in;
        if (!readAll(argv[2], in)) { std::puts("PARSE unreadable"); return 2; }
        unsigned char span[kSpanBytes]; std::uint32_t counter = 0;
        const bool ok = ParseImage(in.data(), static_cast<unsigned>(in.size()), span, &counter);
        std::printf("PARSE %s\nSIZE %zu\nCOUNTER %u\n", ok ? "ok" : "fail", in.size(), counter);
        if (ok) for (int r = 0; r < kSpanRows; ++r) {
            std::printf("row%2d:", r);
            for (int c = 0; c < 12; ++c) { std::uint32_t v; std::memcpy(&v, span + r * kRowStride + c * 4, 4); std::printf(" %u", v); }
            std::puts("");
        }
        return ok ? 0 : 1;
    }
    std::puts("usage: build <shipped> <out> <counter> [row col val]... | parse <in>");
    return 2;
}
