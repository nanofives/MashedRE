// Mashed RE — Save/FileWrappers.cpp: the two RW-stream file wrappers, one body each in BOTH rsp files (lane savelink2,
// 2026-10-23, option C). Split out of Save/SettingsAndIO.cpp, which keeps the video-dialog helpers and stays asi-only.
//
//   0x004b3b70  FileReadWrapper   int(const char* path, void* buf, uint32 size): open(2, 1, path), read, close(stream, 0)
//   0x004b3bb0  FileWriteWrapper  int(const char* path, const void* buf, uint32 size): open(2, 2, path), write,
//                                 close(WRITE RESULT, 0), return the write result
//
// Read from the disassembly of original/MASHED.exe.unpatched 0x004b3b70..0x004b3bac / 0x004b3bb0..0x004b3be6.
// Callers in the original: SaveLoad 0x00404e50 and the .dff loads 0x00411f30 / 0x004587a0 / 0x0048ff20 (read);
// SaveWrite 0x00404f50 (write). Completeness of those lists: no xref dump read.
//
// The write wrapper closes the value RwStreamWrite returned (0x004b3bd6..0x004b3bdb), not the stream it opened: the
// stream on success, NULL on a failed transfer. RwStreamClose 0x004cc160 dereferences its argument with no NULL test
// (0x004cc164..0x004cc169 mov esi,[esp+0x10] / mov eax,[esi]), so in the original a write that opens and then fails
// crashes inside the close, and a successful write returns a pointer to a stream it has just closed. Both kept.
// The original also pushes the caller's ESI as a fourth argument to RwStreamWrite (0x004b3bcd, U-0287), which takes
// three; it is not reproduced.
//
// Per-target host thunks (#ifdef MASHED_STANDALONE):
//   exe: no RW stream open / close body exists (0x004cc230 / 0x004cc160 / 0x005507b0 / 0x00550910 have none in the
//        tree), so the open reports failure and both wrappers return 0. Dead code today: the exe's save path uses
//        Save/GameSave.cpp's CRT stand-ins. Replacing those with these wrappers (option B) is a behaviour change and a
//        decision for Mariano; it would also inherit the crash above.
//   asi: the original open / close by address. Read / write go through the shared Render/RwStreamCore.cpp bodies.
//
// Anchor: MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (on .unpatched)
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "../Render/RwStreamCore.h"
#ifndef MASHED_STANDALONE
#include <windows.h>
#include "../Core/HookSystem.h"
#endif

namespace {
#ifdef MASHED_STANDALONE
std::uint32_t* HostStreamOpen(int, int, const char*) { return nullptr; }
void           HostStreamClose(std::uint32_t*, int) {}
#else
typedef std::uint32_t* (__cdecl* StreamOpen_t)(int type, int access, const char* name);
typedef int            (__cdecl* StreamClose_t)(std::uint32_t* stream, int arg);
std::uint32_t* HostStreamOpen(int type, int access, const char* name) {
    return reinterpret_cast<StreamOpen_t>(0x004cc230u)(type, access, name);
}
void HostStreamClose(std::uint32_t* s, int a) { reinterpret_cast<StreamClose_t>(0x004cc160u)(s, a); }
#endif
} // namespace

// 0x004b3b70
int FileReadWrapperCore(const char* path, void* buf, std::uint32_t size)
{
    std::uint32_t* s = HostStreamOpen(2, 1, path);                            // 0x004b3b76..0x004b3b7d
    if (s == nullptr) return 0;                                               // 0x004b3b7b edi=0, 0x004b3b87..0x004b3b89
    const std::uint32_t n = mashed_re::Render::RwStreamCoreRead(s, buf, size);  // 0x004b3b8b..0x004b3b96
    HostStreamClose(s, 0);                                                    // 0x004b3b9b..0x004b3ba0
    return static_cast<int>(n);                                               // 0x004b3ba8
}

// 0x004b3bb0
int FileWriteWrapperCore(const char* path, const void* buf, std::uint32_t size)
{
    std::uint32_t* s = HostStreamOpen(2, 2, path);                            // 0x004b3bb4..0x004b3bb9
    if (s == nullptr) return 0;                                               // 0x004b3bc1..0x004b3bc3 (eax = 0)
    std::uint32_t* r = mashed_re::Render::RwStreamCoreWrite(s, buf, size);    // 0x004b3bc5..0x004b3bd1
    HostStreamClose(r, 0);                                                    // 0x004b3bd6..0x004b3bdb: the RESULT, U-0288
    return static_cast<int>(reinterpret_cast<std::uintptr_t>(r));             // 0x004b3be3
}

#ifndef MASHED_STANDALONE
// =====================================================================================================================
// .asi install + A/B (MASHED_LEAF_SELFTEST=1; log leaf_selftest.log in the CWD).
//   Live reads: the original twice through a trampoline (noo), then the shared body (ndiff); return and buffer bytes.
//   Live writes: the shared body once (counted; a re-run would rewrite the file).
//   FileWrappersSweep() (exported; nav_champ_probe.py --plan exportsweep, private root only): temp files in the CWD.
//     read: sizes against a 64-byte file (0, 1, 63, 64, and 65 / 200 = short reads), a missing file;
//     write: sizes 0 / 1 / 64 / 200 to a fresh file, and a read-only target (open fails).
//     Compared: the return (the write's pointer folded to non-zero / zero), the destination bytes or the written file,
//     and the RW error slot (*DAT_007d3ff8 + DAT_007d6c5c, reset to {0, 0x80000000} per run).
//     Not run: a write that opens and then fails (the original crashes in RwStreamClose, see the header).
// =====================================================================================================================
namespace {
void* g_readResume  = reinterpret_cast<void*>(0x004b3b75);
void* g_writeResume = reinterpret_cast<void*>(0x004b3bb5);
// MOV EAX,[ESP+4] (4 bytes) + PUSH ESI / PUSH EAX (1 byte) replayed.
__declspec(naked) int __cdecl ReadOrig(const char*, void*, std::uint32_t) {
    __asm {
        mov  eax, dword ptr [esp + 4]
        push esi
        jmp  dword ptr [g_readResume]
    }
}
__declspec(naked) int __cdecl WriteOrig(const char*, const void*, std::uint32_t) {
    __asm {
        mov  eax, dword ptr [esp + 4]
        push eax
        jmp  dword ptr [g_writeResume]
    }
}

int SelfTestOn() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST"); v = (s && s[0] && s[0] != '0') ? 1 : 0; }
    return v;
}
void Log(const char* s) {
    HANDLE h = CreateFileA("leaf_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
DWORD ReadAll(const char* path, unsigned char* out, DWORD cap) {
    HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return 0xffffffffu;
    DWORD n = 0; ReadFile(h, out, cap, &n, nullptr); CloseHandle(h);
    return n;
}
void WriteAll(const char* path, const unsigned char* in, DWORD n) {
    HANDLE h = CreateFileA(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD w; WriteFile(h, in, n, &w, nullptr); CloseHandle(h);
}
std::uint32_t* ErrSlot() {
    const std::uint32_t g = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    return g ? reinterpret_cast<std::uint32_t*>(g + *reinterpret_cast<std::uint32_t*>(0x007d6c5cu)) : nullptr;
}

volatile LONG g_rCalls, g_rNoo, g_rNd, g_wCalls;
const std::uint32_t kLiveMax = 4u << 20;

const char* const kSrc = "leaf_fw_src.bin";
const char* const kDst = "leaf_fw_dst.bin";
const char* const kMissing = "leaf_fw_missing.bin";
unsigned char g_src[256];

struct Out { std::uint32_t ret, err[2]; DWORD fileN; unsigned char bytes[256]; };
// which: 0 = original, 1 = shared. write: 0 = read case, 1 = write case. ro: write target read-only.
void Run(int which, int write, const char* path, std::uint32_t size, int ro, Out* o) {
    std::uint32_t* es = ErrSlot();
    if (es) { es[0] = 0; es[1] = 0x80000000u; }
    std::memset(o->bytes, 0xa5, sizeof o->bytes);
    if (!write) {
        unsigned char dst[256]; std::memset(dst, 0xa5, sizeof dst);
        const int r = which == 0 ? ReadOrig(path, dst, size) : FileReadWrapperCore(path, dst, size);
        o->ret = static_cast<std::uint32_t>(r);
        std::memcpy(o->bytes, dst, sizeof dst);
        o->fileN = 0;
    } else {
        SetFileAttributesA(kDst, FILE_ATTRIBUTE_NORMAL); DeleteFileA(kDst);
        if (ro) { WriteAll(kDst, g_src, 8); SetFileAttributesA(kDst, FILE_ATTRIBUTE_READONLY); }
        const int r = which == 0 ? WriteOrig(kDst, g_src, size) : FileWriteWrapperCore(kDst, g_src, size);
        o->ret = r != 0 ? 1u : 0u;
        o->fileN = ReadAll(kDst, o->bytes, sizeof o->bytes);
        SetFileAttributesA(kDst, FILE_ATTRIBUTE_NORMAL); DeleteFileA(kDst);
    }
    o->err[0] = es ? es[0] : 0; o->err[1] = es ? es[1] : 0;
}
int Diff(const Out& a, const Out& b) {
    return (a.ret != b.ret) + (std::memcmp(a.err, b.err, sizeof a.err) != 0) + (a.fileN != b.fileN) +
           (std::memcmp(a.bytes, b.bytes, sizeof a.bytes) != 0);
}
long g_cases, g_noo, g_nd, g_errs;
void Case(int write, const char* path, std::uint32_t size, int ro) {
    Out o[3];
    Run(0, write, path, size, ro, &o[0]);
    Run(0, write, path, size, ro, &o[1]);
    Run(1, write, path, size, ro, &o[2]);
    const int noo = Diff(o[0], o[1]), nd = Diff(o[0], o[2]);
    ++g_cases; g_noo += (noo != 0); g_nd += (nd != 0); g_errs += (o[0].err[0] != 0 || o[0].err[1] != 0x80000000u);
    char line[300];
    wsprintfA(line, "[fw%ld] fn=%s path=%s size=%u ro=%d ret=%u/%u file=%lu/%lu err=%x/%x noo=%d ndiff=%d\r\n", g_cases,
              write ? "4b3bb0" : "4b3b70", path, size, ro, o[0].ret, o[2].ret, o[0].fileN, o[2].fileN, o[0].err[1],
              o[2].err[1], noo, nd);
    Log(line);
}
}  // namespace

extern "C" __declspec(dllexport) int __cdecl FileWrappersSweep()
{
    std::uint32_t* es = ErrSlot();
    const std::uint32_t keep[2] = { es ? es[0] : 0, es ? es[1] : 0 };
    char line[160];
    wsprintfA(line, "LIVE fn=filewrappers reads=%ld noo=%ld ndiff=%ld writes=%ld\r\n", g_rCalls, g_rNoo, g_rNd, g_wCalls);
    Log(line);
    for (int i = 0; i < 256; ++i) g_src[i] = static_cast<unsigned char>(i * 29 + 11);
    WriteAll(kSrc, g_src, 64);
    DeleteFileA(kMissing);
    const std::uint32_t rs[] = { 0, 1, 63, 64, 65, 200 };
    for (std::uint32_t n : rs) Case(0, kSrc, n, 0);
    Case(0, kMissing, 16, 0);
    const std::uint32_t ws[] = { 0, 1, 64, 200 };
    for (std::uint32_t n : ws) Case(1, kDst, n, 0);
    Case(1, kDst, 16, 1);
    DeleteFileA(kSrc);
    if (es) { es[0] = keep[0]; es[1] = keep[1]; }
    const int clean = GetFileAttributesA(kSrc) == INVALID_FILE_ATTRIBUTES && GetFileAttributesA(kDst) == INVALID_FILE_ATTRIBUTES;
    wsprintfA(line, "SWEEP fn=filewrappers cases=%ld noo=%ld ndiff=%ld err_emitted=%ld clean=%d\r\n", g_cases, g_noo, g_nd,
              g_errs, clean);
    Log(line);
    return static_cast<int>(g_noo + g_nd);
}

// The installed entries and the path1 exports (names kept from Save/SettingsAndIO.cpp).
extern "C" __declspec(dllexport) int __cdecl FileReadWrapper_i3(const char* path, void* buf, std::uint32_t size)
{
    if (!SelfTestOn() || size > kLiveMax || buf == nullptr) return FileReadWrapperCore(path, buf, size);
    unsigned char* save = static_cast<unsigned char*>(std::malloc(size * 2 + 1));
    if (!save) return FileReadWrapperCore(path, buf, size);
    unsigned char* o1 = save + size;
    std::memcpy(save, buf, size);
    const int r1 = ReadOrig(path, buf, size);
    std::memcpy(o1, buf, size);
    std::memcpy(buf, save, size);
    const int r2 = ReadOrig(path, buf, size);
    const int noo = (r1 != r2) + (std::memcmp(o1, buf, size) != 0);
    std::memcpy(buf, save, size);
    const int rm = FileReadWrapperCore(path, buf, size);
    const int nd = (r1 != rm) + (std::memcmp(o1, buf, size) != 0);
    std::free(save);
    const LONG c = InterlockedIncrement(&g_rCalls);
    if (noo) InterlockedIncrement(&g_rNoo);
    if (nd) InterlockedIncrement(&g_rNd);
    if (noo || nd || c <= 8) {
        char line[300];
        wsprintfA(line, "[%ld] fn=4b3b70 path=\"%.80s\" size=%u r=%d m=%d noo=%d ndiff=%d\r\n", c, path ? path : "(null)",
                  size, r1, rm, noo, nd);
        Log(line);
    }
    return rm;
}
RH_ScopedInstall(FileReadWrapper_i3, 0x004b3b70);
extern "C" __declspec(dllexport) int __cdecl FileWriteWrapper_i3(const char* path, void* buf, std::uint32_t size)
{
    InterlockedIncrement(&g_wCalls);
    return FileWriteWrapperCore(path, buf, size);
}
RH_ScopedInstall(FileWriteWrapper_i3, 0x004b3bb0);
#endif
