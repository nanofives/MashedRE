// Port side of Arm U (PREREG_D4-WSB-PORT-SOLV-KV.md). uc_arm.py writes harness.cpp =
// prelude + the RwpVtableKV12.cpp text between `namespace mashed_re {` and
// `// --- registrations` (verbatim, or one mutant) + `} }` + this file.
//
// argv[1] = cases.bin, argv[2] = out.bin.
// cases.bin: u32 ncases, then per case u32 fn, u32 nwords, u32 arena_bytes, words, arena.
// out.bin:   per case u32 ret_bits, arena (arena_bytes).
#include <windows.h>
#include <float.h>
#include <cstdio>
#include <vector>

using namespace mashed_re::Collision;

static const DWORD kArena = 0x20000000u;

static void Thunk(unsigned char* page, DWORD rva, const void* target) {
    unsigned char* p = page + (rva - 0x00560000u);
    const DWORD rel = reinterpret_cast<DWORD>(target) - (rva + 5);
    p[0] = 0xE9;
    std::memcpy(p + 1, &rel, 4);
}

template <typename T> static T Frame(const std::vector<int>& w) {
    T f;
    std::memset(&f, 0, sizeof(f));
    std::memcpy(f.w, w.data(), (w.size() < sizeof(f.w) / 4 ? w.size() : sizeof(f.w) / 4) * 4);
    return f;
}

int main(int argc, char** argv) {
    if (argc < 3) return 2;
    // uc_arm.py commits 0x00560000..0x0056ffff RWX in this process while it is still suspended
    // (the range is taken by the time main runs). Use it if present, else try to allocate.
    unsigned char* page = nullptr;
    {
        MEMORY_BASIC_INFORMATION q = {};
        VirtualQuery(reinterpret_cast<void*>(0x0056a000u), &q, sizeof(q));
        if (q.State == MEM_COMMIT && q.Protect == PAGE_EXECUTE_READWRITE &&
            q.AllocationBase == reinterpret_cast<void*>(0x00560000u))
            page = reinterpret_cast<unsigned char*>(0x00560000u);
        else
            page = static_cast<unsigned char*>(VirtualAlloc(reinterpret_cast<void*>(0x00560000u), 0x10000,
                                                            MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));
    }
    void* arena = VirtualAlloc(reinterpret_cast<void*>(kArena), 0x100000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (page != reinterpret_cast<unsigned char*>(0x00560000u) || arena != reinterpret_cast<void*>(kArena)) {
        MEMORY_BASIC_INFORMATION mi = {};
        VirtualQuery(reinterpret_cast<void*>(0x0056a000u), &mi, sizeof(mi));
        std::fprintf(stderr, "fixed VirtualAlloc failed: page=%p arena=%p [0x0056a000] alloc_base=%p "
                     "size=%lx state=%lx type=%lx\n", page, arena, mi.AllocationBase,
                     (unsigned long)mi.RegionSize, (unsigned long)mi.State, (unsigned long)mi.Type);
        return 3;
    }
    Thunk(page, 0x0056a7a0u, reinterpret_cast<const void*>(&FUN_0056a7a0_impl));
    Thunk(page, 0x0056aae0u, reinterpret_cast<const void*>(&FUN_0056aae0_impl));
    Thunk(page, 0x0056ac40u, reinterpret_cast<const void*>(&FUN_0056ac40_impl));
    Thunk(page, 0x0056adb0u, reinterpret_cast<const void*>(&FUN_0056adb0_impl));

    unsigned int cw = 0;
    _controlfp_s(&cw, _PC_24, _MCW_PC);

    FILE* fi = std::fopen(argv[1], "rb");
    FILE* fo = std::fopen(argv[2], "wb");
    if (!fi || !fo) return 4;
    unsigned int n = 0;
    std::fread(&n, 4, 1, fi);
    for (unsigned int c = 0; c < n; ++c) {
        unsigned int hdr[3];
        std::fread(hdr, 4, 3, fi);
        std::vector<int> w(hdr[1]);
        std::fread(w.data(), 4, hdr[1], fi);
        std::fread(arena, 1, hdr[2], fi);
        std::uint32_t ret = 0;
        switch (hdr[0]) {
        case 0: FUN_0056a7a0_impl(Frame<PgsA7aFrame>(w)); break;
        case 1: FUN_0056aae0_impl(Frame<PgsAaeFrame>(w)); break;
        case 2: { float r = FUN_0056ac40_impl(Frame<PgsAc4Frame>(w)); std::memcpy(&ret, &r, 4); } break;
        case 3: FUN_0056adb0_impl(Frame<Kv12Frame>(w)); break;
        default: return 5;
        }
        std::fwrite(&ret, 4, 1, fo);
        std::fwrite(arena, 1, hdr[2], fo);
    }
    std::fclose(fi);
    std::fclose(fo);
    return 0;
}
