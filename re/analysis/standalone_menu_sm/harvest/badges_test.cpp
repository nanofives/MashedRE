// R2-5 offline probe: decode BADGES.TXD (sfx.piz) with the existing
// Txd::Dictionary and list its named textures. Build:
//   cl /nologo /EHsc /std:c++17 /I ..\..\..\..\mashedmod\src\mashed_re ^
//      badges_test.cpp ..\..\..\..\mashedmod\src\mashed_re\Txd\TxdDecoder.cpp ^
//      ..\..\..\..\mashedmod\src\mashed_re\Piz\PizReader.cpp /Fe:badges_test.exe
#include <cstdio>
#include <cstring>
#include "Piz/PizReader.h"
#include "Txd/TxdDecoder.h"

using namespace mashed_re;

int main() {
    Piz::Archive piz;
    if (!piz.Load("../../../../original/TOASTART/Common/sfx.piz")) {
        std::printf("FAIL piz: %s\n", piz.last_error());
        return 1;
    }
    const std::uint8_t* blob = nullptr;
    std::uint32_t len = 0;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        if (std::strcmp(piz.entry(i).name, "BADGES.TXD") == 0) {
            blob = piz.blob(i);
            len  = piz.entry(i).length;
            break;
        }
    }
    if (!blob) { std::printf("FAIL find BADGES.TXD\n"); return 1; }
    std::printf("BADGES.TXD len=%#x\n", len);

    Txd::Dictionary dict;
    if (!dict.Decode(blob, len)) {
        std::printf("FAIL decode: %s\n", dict.last_error());
        return 1;
    }
    std::printf("textures=%u\n", dict.count());
    for (std::uint32_t i = 0; i < dict.count(); ++i) {
        const Txd::Texture& t = dict.texture(i);
        std::printf("  [%2u] %-20s %ux%u depth=%u mips=%u fmt=%d\n",
                    i, t.name, t.width(), t.height(), t.depth(),
                    t.mip_count, static_cast<int>(t.format()));
    }
    return 0;
}
