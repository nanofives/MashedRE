// PORT-TOP-04F (L-96 top-down lane): the RW stream calls of the 0x00402750 loaders, bound for the MASHED_ORIG_TOP path.
// EXE ONLY. Not bodies of these RVAs: each one is a binding (BINDINGS.tsv kind bound / shim), named by its RVA.
//
// In the original, RwStreamOpen rva 0x004cc230 with type 2 opens the name through the RenderWare file system
// (RtFSManager, re/analysis/promote_c2_rw_engine_init/004cc230.md), which the piz mounts of 0x00495280 feed. The exe has
// no RtFSManager (RtFSManagerOpen is a shim, LibRw/RwApiBind.cpp). So the open binding reads the whole file once, from
// the piz mount stack of Match/PortTop_00402750_Pieces.cpp (newest mount first) or else from the disk under
// "original/" (the hand loaders' root, relative to MASHED_ROOT), and returns an RW memory stream over that copy:
// [0] type 3, [1] access 1, [3] position, [4] size, [5] base (the word layout of RwStreamRead rva 0x004cbd30 case 3,
// Render/RwStreamCore.h). Declared deviation: a type-2 open gives a type-3 stream. Every reader on the knob path goes
// through RwStreamRead / RwStreamSkip, which read the type word, so no reader sees a file handle.
//
//  - RwStreamRead rva 0x004cbd30 -> the exe body mashed_re::Render::RwStreamCoreRead (Render/RwStreamCore.cpp).
//  - RwStreamFindChunk rva 0x004cc5e0 -> the exe body mashed_re::D3d9Render::Rwl_StreamFindChunk (RwWorldStream.cpp).
//  - RwStreamClose rva 0x004cc160: the matched body (re/match/s20/004cc160.c) returns the record to the RwGlobals +0x11c
//    free list, which the exe's RwGlobals record does not carry. The binding frees the record and the file copy.
// Evidence: verify/porttop_20261108/RESULT_PORT-TOP-04F.md.
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../Render/RwStreamCore.h"

namespace mashed_re { namespace D3d9Render {
int Rwl_StreamFindChunk(void* stream, int type, int* lenOut, unsigned* verOut);
} }

extern "C" const void* PortTop_PizFs_Find(const char* entry, unsigned* len);

namespace {
void LogFs(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[ORIG_TOP] [RW_FS] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

// The RwStream record (re/match/s20/004cc160.c: type, accessType, position, 5-word union, rwOwned) plus the copy.
struct PortStream {
    std::uint32_t w[9];
    void* copy;
};

void* ReadDisk(const char* name, unsigned* len) {
    char path[400];
    const char* p = name;
    if (p[0] && p[1] == ':') p += 2;
    while (*p == '\\' || *p == '/') ++p;
    std::snprintf(path, sizeof(path), "original/%s", p);
    for (char* c = path; *c; ++c) if (*c == '\\') *c = '/';
    std::FILE* f = std::fopen(path, "rb");
    if (!f) return nullptr;
    std::fseek(f, 0, SEEK_END);
    const long n = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    void* buf = n > 0 ? std::malloc(static_cast<std::size_t>(n)) : nullptr;
    if (buf && std::fread(buf, 1, static_cast<std::size_t>(n), f) != static_cast<std::size_t>(n)) {
        std::free(buf);
        buf = nullptr;
    }
    std::fclose(f);
    if (buf) *len = static_cast<unsigned>(n);
    return buf;
}
}  // namespace

extern "C" {

// A malloc'd copy of <name> from the piz mount stack, else from the disk under "original/". 0 when neither has it.
void* PortTop_Fs_ReadAll(const char* name, unsigned* len, const char** from) {
    *len = 0;
    unsigned n = 0;
    if (const void* b = PortTop_PizFs_Find(name, &n)) {
        void* c = std::malloc(n ? n : 1);
        if (!c) return nullptr;
        std::memcpy(c, b, n);
        *len = n;
        if (from) *from = "piz";
        return c;
    }
    void* d = ReadDisk(name, len);
    if (d && from) *from = "disk";
    return d;
}

// [PORT-TOP-04G] rva 0x004b5350 (re/match/s18/004b5350.c: return FUN_00550b00(name)). 0x00550b00 (re/match/s24/
// 00550b00.c) picks the RtFS file system that owns the name (a "<fs>:" prefix, else the default one at DAT_007dc76c) and
// returns its +0x4c file-exists result, 0 when there is none. The exe has no RtFSManager (PORT-TOP-03), so the binding
// answers for the two places the stream binding above opens from: 1 when the name is in a mounted piz (PortTop_PizFs_Find)
// or is a file on the disk under "original/", else 0. The matched 0x0042a470 (path search of 0x0042a6b0) calls it.
int PortTop_FsFileExists_004b5350(const char* name) {
    if (!name) return 0;
    unsigned n = 0;
    if (PortTop_PizFs_Find(name, &n)) return 1;
    char path[400];
    const char* p = name;
    if (p[0] && p[1] == ':') p += 2;
    while (*p == '\\' || *p == '/') ++p;
    std::snprintf(path, sizeof(path), "original/%s", p);
    for (char* c = path; *c; ++c) if (*c == '\\') *c = '/';
    if (std::FILE* f = std::fopen(path, "rb")) {
        std::fclose(f);
        return 1;
    }
    return 0;
}

// RwStreamOpen binding (rva 0x004cc230). type 2 (file system) and type 3 (memory, data = {start, length}) for reading.
void* PortTop_RwStreamOpen_004cc230(int type, int access, const void* data) {
    if (access != 1 || !data || (type != 2 && type != 3)) {
        LogFs("0x004cc230(%d, %d, %p): binding opens types 2 / 3 for reading only, returns 0", type, access, data);
        return nullptr;
    }
    PortStream* s = static_cast<PortStream*>(std::calloc(1, sizeof(PortStream)));
    if (!s) return nullptr;
    unsigned n = 0;
    const std::uint8_t* base = nullptr;
    if (type == 2) {
        const char* from = "";
        s->copy = PortTop_Fs_ReadAll(static_cast<const char*>(data), &n, &from);
        if (!s->copy) {
            LogFs("0x004cc230(2, 1, \"%s\"): not in a mounted piz or on disk, returns 0", static_cast<const char*>(data));
            std::free(s);
            return nullptr;
        }
        base = static_cast<const std::uint8_t*>(s->copy);
        LogFs("0x004cc230(2, 1, \"%s\"): %u bytes from %s -> memory stream %p", static_cast<const char*>(data), n, from,
              static_cast<void*>(s));
    } else {
        const std::uint32_t* mem = static_cast<const std::uint32_t*>(data);
        base = reinterpret_cast<const std::uint8_t*>(static_cast<std::uintptr_t>(mem[0]));
        n = mem[1];
    }
    s->w[0] = 3;
    s->w[1] = 1;
    s->w[3] = 0;
    s->w[4] = n;
    s->w[5] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base));
    return s;
}

// RwStreamClose binding (rva 0x004cc160): frees the record and the file copy; returns 1 like the type-3 case.
int PortTop_RwStreamClose_004cc160(void* stream, void* memOut) {
    (void)memOut;   // only read for a write stream (accessType != 1); the binding opens read streams only
    PortStream* s = static_cast<PortStream*>(stream);
    if (!s) return 0;
    std::free(s->copy);
    std::free(s);
    return 1;
}

// RwStreamRead binding (rva 0x004cbd30) -> the exe body.
unsigned PortTop_RwStreamRead_004cbd30(void* stream, void* dst, unsigned size) {
    return mashed_re::Render::RwStreamCoreRead(static_cast<std::uint32_t*>(stream), dst, size);
}

// RwStreamFindChunk binding (rva 0x004cc5e0) -> the exe body.
int PortTop_RwStreamFindChunk_004cc5e0(void* stream, int type, int* lenOut, unsigned* verOut) {
    return mashed_re::D3d9Render::Rwl_StreamFindChunk(stream, type, lenOut, verOut);
}

}  // extern "C"
#endif  // MASHED_STANDALONE
