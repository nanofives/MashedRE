// Mashed RE — Render/RwStreamCore.cpp
// RwStreamRead (0x004cbd30) and RwStreamSkip (0x004cc050): the one body of each, in BOTH rsp files (lane dcstream,
// 2026-10-22). Replaces the exe-only memory-path copy (D3d9Render/RwWorldStream.cpp, which now calls through) and the
// asi-only four-case copy (Render/TextureLoader_q6.cpp, removed).
//
// Read from the disassembly of original/MASHED.exe.unpatched (re/tools/disasm_fn.py 004cbd30 004cbe6d / 004cc050
// 004cc149). Both switch on (type - 1) through a jump table: 0x004cbe70 = {0x004cbd4e, 0x004cbd4e, 0x004cbdc8,
// 0x004cbe28}, 0x004cc14c = {0x004cc07c, 0x004cc07c, 0x004cc0cd, 0x004cc10d}; (type - 1) > 3 takes the default.
// Every error goes out as {1, FUN_004d7ff0(code)} through FUN_004d8480.
//
// Per-target host thunks (#ifdef MASHED_STANDALONE): the error pair and the file-stream callees FUN_00550950 (read),
// FUN_00550af0 (EOF check), FUN_005509b0 (seek). The exe opens memory streams only (RwWorldLoad_OpenMemory in
// D3d9Render/RwWorldStream.cpp), so its file thunks are never reached; they report a failed transfer.
//
// RwStreamWrite (0x004cbe80) joined in lane savelink2 (2026-10-23), read from the disassembly 0x004cbe80..0x004cc037
// (jump table 0x004cc040). It replaces the asi-only copy in Save/RwStream.cpp (removed), whose four error paths called
// FUN_004d8480 with no block argument and dropped FUN_004d7ff0's second argument on the two allocator failures. The exe
// gets it as dead code: no exe caller yet (Save/SettingsAndIO.cpp's FileWriteWrapper is asi-only).
//
// Anchor: MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (on .unpatched)
#include "RwStreamCore.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>
#ifndef MASHED_STANDALONE
#include <windows.h>
#include "../Core/HookSystem.h"
#endif

#ifdef MASHED_STANDALONE
namespace mashed_re { namespace D3d9Render {
unsigned Rwl_ErrFmt(unsigned code, unsigned a);   // FUN_004d7ff0 (D3d9Render/RwWorldLoadStubs.cpp)
void     Rwl_ErrEmit(void* blk);                  // FUN_004d8480 (D3d9Render/RwWorldLoadStubs.cpp)
} }
#endif

namespace mashed_re {
namespace Render {
namespace {

typedef std::uint32_t (__cdecl* ReadCb_t)(std::uint32_t user, void* dst, std::uint32_t size);
typedef std::uint32_t (__cdecl* SkipCb_t)(std::uint32_t user, std::uint32_t n);

#ifdef MASHED_STANDALONE
std::uint32_t HostErrFmt(std::uint32_t code) { return D3d9Render::Rwl_ErrFmt(code, 0); }
void          HostErrEmit(std::uint32_t* blk) { D3d9Render::Rwl_ErrEmit(blk); }
std::uint32_t HostFileRead(void*, std::uint32_t, std::uint32_t, std::uint32_t) { return 0; }
std::uint32_t HostFileEof(std::uint32_t) { return 0; }
std::int32_t  HostFileSeek(std::uint32_t, std::uint32_t, std::int32_t) { return -1; }
// RwStreamWrite (lane savelink2): the exe opens no file stream for writing, so the file thunk reports a short write.
// Memory streams take the CRT heap in place of the RwGlobals allocator slots (+0x108 / +0x110); the flag words
// 0x30404 / 0x1030404 (U-2328 / U-2329) have no CRT analogue and are dropped. Declared deviation, exe only.
std::uint32_t HostErrFmt2(std::uint32_t code, std::uint32_t a) { return D3d9Render::Rwl_ErrFmt(code, a); }
std::uint32_t HostFileWrite(const void*, std::uint32_t, std::uint32_t, std::uint32_t) { return 0; }
std::uint32_t HostAlloc(std::uint32_t size, std::uint32_t) {
    return reinterpret_cast<std::uint32_t>(std::malloc(size));
}
std::uint32_t HostRealloc(std::uint32_t p, std::uint32_t size, std::uint32_t) {
    return reinterpret_cast<std::uint32_t>(std::realloc(reinterpret_cast<void*>(p), size));
}
#else
typedef std::uint32_t (__cdecl* ErrFmt_t)(std::uint32_t);
typedef void          (__cdecl* ErrEmit_t)(std::uint32_t*);
typedef std::uint32_t (__cdecl* FileRead_t)(void*, std::uint32_t, std::uint32_t, std::uint32_t);
typedef std::uint32_t (__cdecl* FileEof_t)(std::uint32_t);
typedef std::int32_t  (__cdecl* FileSeek_t)(std::uint32_t, std::uint32_t, std::int32_t);
std::uint32_t HostErrFmt(std::uint32_t code) { return reinterpret_cast<ErrFmt_t>(0x004d7ff0u)(code); }
void          HostErrEmit(std::uint32_t* blk) { reinterpret_cast<ErrEmit_t>(0x004d8480u)(blk); }
std::uint32_t HostFileRead(void* d, std::uint32_t e, std::uint32_t n, std::uint32_t h) {
    return reinterpret_cast<FileRead_t>(0x00550950u)(d, e, n, h);
}
std::uint32_t HostFileEof(std::uint32_t h) { return reinterpret_cast<FileEof_t>(0x00550af0u)(h); }
std::int32_t  HostFileSeek(std::uint32_t h, std::uint32_t n, std::int32_t whence) {
    return reinterpret_cast<FileSeek_t>(0x005509b0u)(h, n, whence);
}
typedef std::uint32_t (__cdecl* ErrFmt2_t)(std::uint32_t, std::uint32_t);
typedef std::uint32_t (__cdecl* FileWrite_t)(const void*, std::uint32_t, std::uint32_t, std::uint32_t);
typedef std::uint32_t (__cdecl* Alloc_t)(std::uint32_t, std::uint32_t);
typedef std::uint32_t (__cdecl* Realloc_t)(std::uint32_t, std::uint32_t, std::uint32_t);
std::uint32_t HostErrFmt2(std::uint32_t code, std::uint32_t a) { return reinterpret_cast<ErrFmt2_t>(0x004d7ff0u)(code, a); }
std::uint32_t HostFileWrite(const void* s, std::uint32_t e, std::uint32_t n, std::uint32_t h) {
    return reinterpret_cast<FileWrite_t>(0x00550980u)(s, e, n, h);
}
std::uint32_t HostAlloc(std::uint32_t size, std::uint32_t flags) {          // *(*DAT_007d3ff8 + 0x108), 0x004cbf05
    const std::uint32_t g = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    return (*reinterpret_cast<Alloc_t*>(g + 0x108))(size, flags);
}
std::uint32_t HostRealloc(std::uint32_t p, std::uint32_t size, std::uint32_t flags) {  // + 0x110, 0x004cbf7f
    const std::uint32_t g = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    return (*reinterpret_cast<Realloc_t*>(g + 0x110))(p, size, flags);
}
#endif

void StreamErr(std::uint32_t code)
{
    std::uint32_t blk[2];
    blk[0] = 1;
    blk[1] = HostErrFmt(code);
    HostErrEmit(blk);
}
// The two allocator failures in RwStreamWrite pass FUN_004d7ff0 a second argument (0x004cbf15 / 0x004cbf99).
void StreamErr2(std::uint32_t code, std::uint32_t a)
{
    std::uint32_t blk[2];
    blk[0] = 1;
    blk[1] = HostErrFmt2(code, a);
    HostErrEmit(blk);
}

} // namespace

// 0x004cbd30
std::uint32_t RwStreamCoreRead(std::uint32_t* stream, void* dst, std::uint32_t size)
{
    switch (stream[0]) {
    case 1:
    case 2: {
        const std::uint32_t h   = stream[3];                       // 0x004cbd4e
        const std::uint32_t got = HostFileRead(dst, 1, size, h);   // 0x004cbd62 FUN_00550950(dst, 1, size, h)
        if (got != size) {                                         // 0x004cbd6c
            if (HostFileEof(h) != 0) StreamErr(5);                 // 0x004cbd71 FUN_00550af0, 0x004cbd81
            else                     StreamErr(0x8000001au);       // 0x004cbda3
        }
        return got;
    }
    case 3: {
        const std::uint32_t avail = stream[4] - stream[3];         // 0x004cbdc8..0x004cbdd2
        if (size > avail) {                                        // 0x004cbdd4 jbe
            size = avail;                                          // 0x004cbdda
            StreamErr(5);                                          // 0x004cbdd8
        }
        // 0x004cbdfa..0x004cbe14: rep movsd (size >> 2) then rep movsb (size & 3), forward from base + pos.
        const unsigned char* src = reinterpret_cast<const unsigned char*>(stream[5] + stream[3]);
        unsigned char* out = static_cast<unsigned char*>(dst);
        std::uint32_t i = 0;
        for (std::uint32_t k = size >> 2; k != 0; --k, i += 4) {
            std::uint32_t w; std::memcpy(&w, src + i, 4); std::memcpy(out + i, &w, 4);
        }
        for (std::uint32_t k = size & 3; k != 0; --k, ++i) out[i] = src[i];
        stream[3] = stream[3] + size;                              // 0x004cbe16..0x004cbe1d
        return size;
    }
    case 4:
        return reinterpret_cast<ReadCb_t>(stream[4])(stream[7], dst, size);  // 0x004cbe28..0x004cbe36
    default:
        StreamErr(0xe);                                            // 0x004cbe44
        return 0;
    }
}

// 0x004cc050
std::uint32_t* RwStreamCoreSkip(std::uint32_t* stream, std::uint32_t n)
{
    if (n == 0) return stream;                                     // 0x004cc057 / 0x004cc05a, before the type
    switch (stream[0]) {                                           // 0x004cc069..0x004cc075
    case 1:
    case 2: {
        const std::uint32_t h = stream[3];                         // 0x004cc07d
        if (HostFileSeek(h, n, 1) == 0) return stream;             // 0x004cc084 FUN_005509b0(h, n, 1), 0x004cc08e
        if (HostFileEof(h) != 0) StreamErr(5);                     // 0x004cc091 FUN_00550af0, 0x004cc09d
        return nullptr;                                            // 0x004cc0be
    }
    case 3: {
        const std::uint32_t np = n + stream[3];                    // 0x004cc0cd / 0x004cc0d0
        if (np > stream[4]) {                                      // 0x004cc0d5 jbe
            stream[3] = stream[4];                                 // 0x004cc0db
            StreamErr(5);                                          // 0x004cc0d9
            return nullptr;                                        // 0x004cc0fc
        }
        stream[3] = np;                                            // 0x004cc103
        return stream;
    }
    case 4:
        // 0x004cc10d..0x004cc11c: neg / sbb / and, so a NON-ZERO callback result returns the stream.
        return reinterpret_cast<SkipCb_t>(stream[6])(stream[7], n) != 0 ? stream : nullptr;
    default:
        StreamErr(0xe);                                            // 0x004cc123
        return nullptr;
    }
}

// 0x004cbe80
// Jump table 0x004cc040 on (type - 1); (type - 1) > 3 takes the default (0x004cbe8a..0x004cbe96).
std::uint32_t* RwStreamCoreWrite(std::uint32_t* stream, const void* src, std::uint32_t size)
{
    switch (stream[0]) {
    case 1:
    case 2:
        // 0x004cbe9d..0x004cbead FUN_00550980(src, 1, size, h); 0x004cbeb5 a short count is an error.
        if (HostFileWrite(src, 1, size, stream[3]) == size) return stream;
        StreamErr(0x8000001cu);                                    // 0x004cbeb9..0x004cbed4
        return nullptr;
    case 3: {
        if (stream[5] == 0) {                                      // 0x004cbeee..0x004cbef3
            stream[5] = HostAlloc(0x200, 0x30404);                 // 0x004cbef5..0x004cbf0e, stored before the test
            if (stream[5] == 0) {
                StreamErr2(0x80000013u, 0x200);                    // 0x004cbf15..0x004cbf35
                return nullptr;
            }
            stream[4] = 0x200;                                     // 0x004cbf46
        }
        if (stream[4] - stream[3] < size) {                        // 0x004cbf4d..0x004cbf5d
            const std::uint32_t grow = stream[4] + (size < 0x200 ? 0x200 : size);  // 0x004cbf5f..0x004cbf6d
            const std::uint32_t np = HostRealloc(stream[5], grow, 0x1030404);      // 0x004cbf70..0x004cbf7f
            if (np == 0) {                                         // 0x004cbf88; the stream keeps the old buffer
                StreamErr2(0x80000013u, grow - stream[4]);         // 0x004cbf8c..0x004cbfad
                return nullptr;
            }
            stream[5] = np;                                        // 0x004cbfbe
            stream[4] = grow;                                      // 0x004cbfc1
        }
        // 0x004cbfc4..0x004cbfdf: rep movsd (size >> 2) then rep movsb (size & 3), forward to base + pos.
        unsigned char* dst = reinterpret_cast<unsigned char*>(stream[5] + stream[3]);
        const unsigned char* in = static_cast<const unsigned char*>(src);
        std::uint32_t i = 0;
        for (std::uint32_t k = size >> 2; k != 0; --k, i += 4) {
            std::uint32_t w; std::memcpy(&w, in + i, 4); std::memcpy(dst + i, &w, 4);
        }
        for (std::uint32_t k = size & 3; k != 0; --k, ++i) dst[i] = in[i];
        stream[3] = stream[3] + size;                              // 0x004cbfe1..0x004cbfe8
        return stream;
    }
    case 4: {
        // 0x004cbff3..0x004cc001: (*stream[5])(stream[7], src, size); neg / sbb / and keeps the stream on non-zero.
        typedef std::uint32_t (__cdecl* WriteCb_t)(std::uint32_t user, const void* src, std::uint32_t size);
        return reinterpret_cast<WriteCb_t>(stream[5])(stream[7], src, size) != 0 ? stream : nullptr;
    }
    default:
        StreamErr(0xe);                                            // 0x004cc014..0x004cc02c
        return nullptr;
    }
}

} // namespace Render
} // namespace mashed_re

#ifndef MASHED_STANDALONE
// =====================================================================================================================
// .asi install + in-process A/B. MASHED_LEAF_SELFTEST=1, per call, by stream type:
//   memory (3), unknown type, and a zero skip: no external effect, so the stream words (9) and the destination bytes
//     are snapshotted, the original runs twice through a trampoline (noo), then the shared body (ndiff).
//   file (1, 2): the original runs once, the file is rewound with FUN_005509b0(h, -count, 1), then the shared body runs
//     (ndiff). A short read and a failed skip are not re-run (the rewind does not restore the position after either;
//     counted `unrun`, the original's result kept).
//   callback (4): the original runs once with the stream's fn / user slots swapped for a recorder that calls the real
//     callback; the shared body then runs against a replay stub returning the recorded value, and the call count and
//     arguments are compared (ndiff).
// The game keeps the shared body's result. Log: leaf_selftest.log in the CWD (shared with the other leaf self-tests).
// =====================================================================================================================
namespace {
using mashed_re::Render::RwStreamCoreRead;
using mashed_re::Render::RwStreamCoreSkip;
using mashed_re::Render::RwStreamCoreWrite;
using mashed_re::Render::HostFileSeek;
using mashed_re::Render::HostAlloc;

// Trampolines: the 5-byte E9 covers SUB ESP,8 + PUSH EBX + the first byte of MOV EBX,[ESP+0x10] (read), and
// MOV EAX,[ESP+8] + the first byte of SUB ESP,8 (skip). Each re-runs the clobbered instructions and resumes.
void* g_readResume = reinterpret_cast<void*>(0x004cbd38);
void* g_skipResume = reinterpret_cast<void*>(0x004cc057);
// RwStreamWrite opens with the same SUB ESP,8 / PUSH EBX / MOV EBX,[ESP+0x10] as RwStreamRead (0x004cbe80..0x004cbe84).
void* g_writeResume = reinterpret_cast<void*>(0x004cbe88);
__declspec(naked) std::uint32_t __cdecl ReadOrig(std::uint32_t*, void*, std::uint32_t) {
    __asm {
        sub  esp, 8
        push ebx
        mov  ebx, dword ptr [esp + 10h]
        jmp  dword ptr [g_readResume]
    }
}
__declspec(naked) std::uint32_t* __cdecl WriteOrig(std::uint32_t*, const void*, std::uint32_t) {
    __asm {
        sub  esp, 8
        push ebx
        mov  ebx, dword ptr [esp + 10h]
        jmp  dword ptr [g_writeResume]
    }
}
__declspec(naked) std::uint32_t* __cdecl SkipOrig(std::uint32_t*, std::uint32_t) {
    __asm {
        mov  eax, dword ptr [esp + 8]
        sub  esp, 8
        jmp  dword ptr [g_skipResume]
    }
}

int StreamSelfTestOn() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST"); v = (s && s[0] && s[0] != '0') ? 1 : 0; }
    return v;
}
void StreamLog(const char* s) {
    HANDLE h = CreateFileA("leaf_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

const std::uint32_t kWords = 9;            // RwStream: type .. rwOwned
const std::uint32_t kMaxBytes = 4u << 20;  // larger transfers bypass the A/B (counted `big`)

// [fn 0 = read, 1 = skip, 2 = write][class 0 = pure, 1 = file, 2 = callback]
volatile LONG g_calls[3][3], g_noo[3][3], g_ndiff[3][3], g_unrun[3][3], g_big[3];
volatile LONG g_total[3];

int ClassOf(std::uint32_t type) { return (type == 1 || type == 2) ? 1 : (type == 4 ? 2 : 0); }

void Account(int fn, int cls, std::uint32_t type, std::uint32_t n, std::uint32_t r1, std::uint32_t rm, int noo, int nd,
             int unrun) {
    const LONG c = InterlockedIncrement(&g_calls[fn][cls]);
    if (noo) InterlockedIncrement(&g_noo[fn][cls]);
    if (nd) InterlockedIncrement(&g_ndiff[fn][cls]);
    if (unrun) InterlockedIncrement(&g_unrun[fn][cls]);
    const LONG t = InterlockedIncrement(&g_total[fn]);
    const char* name = fn == 0 ? "4cbd30" : (fn == 1 ? "4cc050" : "4cbe80");
    char line[300];
    if (noo || nd || unrun || c <= 4) {
        wsprintfA(line, "[%ld] fn=%s type=%u n=%u r=%08x m=%08x noo=%d ndiff=%d unrun=%d\r\n", t, name, type, n, r1, rm,
                  noo, nd, unrun);
        StreamLog(line);
    }
    if (t <= 16 || (t & 0xff) == 0) {
        wsprintfA(line, "SUM fn=%s calls=%ld pure=%ld/%ld/%ld file=%ld/%ld/%ld/u%ld cb=%ld/%ld/%ld/u%ld big=%ld\r\n", name,
                  t, g_calls[fn][0], g_noo[fn][0], g_ndiff[fn][0], g_calls[fn][1], g_noo[fn][1], g_ndiff[fn][1],
                  g_unrun[fn][1], g_calls[fn][2], g_noo[fn][2], g_ndiff[fn][2], g_unrun[fn][2], g_big[fn]);
        StreamLog(line);
    }
}

// Callback record / replay. The stream's user slot [7] is pointed at the record, so no global state is shared.
struct CbRec { std::uint32_t realFn, realUser, calls, dst, n, ret; };
std::uint32_t __cdecl RecReadCb(std::uint32_t rp, void* dst, std::uint32_t n) {
    CbRec* r = reinterpret_cast<CbRec*>(rp);
    ++r->calls; r->dst = reinterpret_cast<std::uint32_t>(dst); r->n = n;
    r->ret = reinterpret_cast<std::uint32_t (__cdecl*)(std::uint32_t, void*, std::uint32_t)>(r->realFn)(r->realUser, dst, n);
    return r->ret;
}
std::uint32_t __cdecl ReplayReadCb(std::uint32_t rp, void* dst, std::uint32_t n) {
    CbRec* r = reinterpret_cast<CbRec*>(rp);
    ++r->calls; r->dst = reinterpret_cast<std::uint32_t>(dst); r->n = n;
    return r->ret;
}
std::uint32_t __cdecl RecSkipCb(std::uint32_t rp, std::uint32_t n) {
    CbRec* r = reinterpret_cast<CbRec*>(rp);
    ++r->calls; r->n = n;
    r->ret = reinterpret_cast<std::uint32_t (__cdecl*)(std::uint32_t, std::uint32_t)>(r->realFn)(r->realUser, n);
    return r->ret;
}
std::uint32_t __cdecl ReplaySkipCb(std::uint32_t rp, std::uint32_t n) {
    CbRec* r = reinterpret_cast<CbRec*>(rp);
    ++r->calls; r->n = n;
    return r->ret;
}

// MASHED_LEAF_SELFTEST_SWEEP=1: once, at the first hooked read, synthetic streams the game does not produce naturally
// (memory overrun / clamp / 32-bit wrap, unknown types, callback streams on deterministic stubs) through the pure A/B:
// the original twice (noo), then the shared body (ndiff). Compared: return, the 9 stream words, 96 destination bytes,
// the stub's recorded arguments, and the RW error slot (*DAT_007d3ff8 + DAT_007d6c5c, 2 dwords; FUN_004d8480 stores
// only while it holds {0, 0x80000000}, 0x004d848d / 0x004d849c), which each case resets to that clear state first.
struct SweepCb { std::uint32_t calls, user, dst, n, ret; };
SweepCb g_scb;
std::uint32_t __cdecl SweepReadCb(std::uint32_t user, void* dst, std::uint32_t n) {
    ++g_scb.calls; g_scb.user = user; g_scb.dst = reinterpret_cast<std::uint32_t>(dst); g_scb.n = n;
    unsigned char* o = static_cast<unsigned char*>(dst);
    for (std::uint32_t i = 0; i < n && i < 8; ++i) o[i] = static_cast<unsigned char>(0x11 + i);
    return g_scb.ret;
}
std::uint32_t __cdecl SweepSkipCb(std::uint32_t user, std::uint32_t n) {
    ++g_scb.calls; g_scb.user = user; g_scb.n = n;
    return g_scb.ret;
}
std::uint32_t __cdecl SweepWriteCb(std::uint32_t user, const void* src, std::uint32_t n) {
    ++g_scb.calls; g_scb.user = user; g_scb.dst = reinterpret_cast<std::uint32_t>(src); g_scb.n = n;
    return g_scb.ret;
}
std::uint32_t* SweepErrSlot() {
    const std::uint32_t g = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    return g ? reinterpret_cast<std::uint32_t*>(g + *reinterpret_cast<std::uint32_t*>(0x007d6c5cu)) : nullptr;
}
struct SweepOut { std::uint32_t ret, w[kWords], err[2]; SweepCb cb; unsigned char dst[96]; };
unsigned char g_sweepSrc[256];
unsigned char g_sweepDst[96];

// which: 0 = original read, 1 = original skip, 2 = shared read, 3 = shared skip, 4 = original write, 5 = shared write.
// Write cases: alloc 1 = the stream starts with no buffer (first-alloc), 2 = a fresh RW-heap buffer of w0[4] bytes
// holding the 0xa5 fill (grow). Each run then owns its own buffer, so [5] is folded to 0 and its first 96 bytes compared.
// The RW-heap blocks are not freed (a one-shot sweep, a few KB).
void SweepRun(int which, const std::uint32_t* w0, std::uint32_t n, std::uint32_t cbRet, SweepOut* o, int alloc = 0) {
    std::uint32_t s[kWords]; std::memcpy(s, w0, sizeof s);
    std::memset(g_sweepDst, 0xa5, sizeof g_sweepDst);
    std::memset(&g_scb, 0, sizeof g_scb); g_scb.ret = cbRet;
    if (alloc == 1) s[5] = 0;
    if (alloc == 2) {
        s[5] = HostAlloc(s[4], 0x30404);
        if (s[5]) std::memset(reinterpret_cast<void*>(s[5]), 0xa5, s[4]);
    }
    std::uint32_t* es = SweepErrSlot();
    if (es) { es[0] = 0; es[1] = 0x80000000u; }
    switch (which) {
    case 0: o->ret = ReadOrig(s, g_sweepDst, n); break;
    case 1: o->ret = reinterpret_cast<std::uint32_t>(SkipOrig(s, n)); break;
    case 2: o->ret = RwStreamCoreRead(s, g_sweepDst, n); break;
    case 3: o->ret = reinterpret_cast<std::uint32_t>(RwStreamCoreSkip(s, n)); break;
    case 4: o->ret = reinterpret_cast<std::uint32_t>(WriteOrig(s, g_sweepSrc, n)); break;
    default: o->ret = reinterpret_cast<std::uint32_t>(RwStreamCoreWrite(s, g_sweepSrc, n)); break;
    }
    // A stream-pointer return is folded to 1 / 0 (each run uses its own local copy of the words).
    if (which == 1 || which == 3 || which >= 4)
        o->ret = (o->ret == reinterpret_cast<std::uint32_t>(s)) ? 1u : (o->ret ? 2u : 0u);
    if (alloc && s[5]) {
        // Only the defined prefix: the 0xa5 fill of the starting buffer (w0[4] bytes) and the bytes written up to the new
        // position. A grown or first-allocated block is uninitialized past that (W1 2026-10-23: 2 grow cases noo=1).
        const std::uint32_t def = s[3] > w0[4] ? s[3] : w0[4];
        const std::uint32_t cap = def < sizeof o->dst ? def : sizeof o->dst;
        std::memset(g_sweepDst, 0, sizeof g_sweepDst);
        std::memcpy(g_sweepDst, reinterpret_cast<void*>(s[5]), cap);
        s[5] = 0;
    }
    std::memcpy(o->w, s, sizeof s);
    o->err[0] = es ? es[0] : 0; o->err[1] = es ? es[1] : 0;
    o->cb = g_scb;
    std::memcpy(o->dst, g_sweepDst, sizeof g_sweepDst);
}
int SweepDiff(const SweepOut& a, const SweepOut& b) {
    return (a.ret != b.ret) + (std::memcmp(a.w, b.w, sizeof a.w) != 0) + (std::memcmp(a.err, b.err, sizeof a.err) != 0) +
           (std::memcmp(&a.cb, &b.cb, sizeof a.cb) != 0) + (std::memcmp(a.dst, b.dst, sizeof a.dst) != 0);
}
long g_swCases = 0, g_swNoo = 0, g_swNdiff = 0, g_swErr = 0;
// fn: 0 = read, 1 = skip, 2 = write
void SweepCase(int fn, const std::uint32_t* w0, std::uint32_t n, std::uint32_t cbRet, int alloc = 0) {
    SweepOut o1, o2, om;
    const int orig = fn == 0 ? 0 : (fn == 1 ? 1 : 4), mine = fn == 0 ? 2 : (fn == 1 ? 3 : 5);
    SweepRun(orig, w0, n, cbRet, &o1, alloc);
    SweepRun(orig, w0, n, cbRet, &o2, alloc);
    SweepRun(mine, w0, n, cbRet, &om, alloc);
    const int noo = SweepDiff(o1, o2), nd = SweepDiff(o1, om);
    ++g_swCases; g_swNoo += (noo != 0); g_swNdiff += (nd != 0); g_swErr += (o1.err[0] != 0 || o1.err[1] != 0x80000000u);
    if (noo || nd) {
        char line[300];
        wsprintfA(line, "[sw%ld] fn=%s type=%u pos=%u size=%u n=%08x cbret=%08x o=%u/%u/%08x m=%u/%u/%08x noo=%d ndiff=%d\r\n",
                  g_swCases, fn == 0 ? "4cbd30" : (fn == 1 ? "4cc050" : "4cbe80"), w0[0], w0[3], w0[4], n, cbRet, o1.ret, o1.w[3], o1.err[1], om.ret,
                  om.w[3], om.err[1], noo, nd);
        StreamLog(line);
    }
}
void StreamSweep() {
    const char* e = std::getenv("MASHED_LEAF_SELFTEST_SWEEP");
    if (!e || !e[0] || e[0] == '0') return;
    std::uint32_t* es = SweepErrSlot();
    std::uint32_t keep[2] = { es ? es[0] : 0, es ? es[1] : 0 };
    for (int i = 0; i < 256; ++i) g_sweepSrc[i] = static_cast<unsigned char>(i * 7 + 3);
    const std::uint32_t base = reinterpret_cast<std::uint32_t>(g_sweepSrc);
    std::uint32_t w[kWords];
    // Memory streams, size 64 inside a 256-byte source; pos 70 > size takes the unclamped path (bounded n only).
    const std::uint32_t pos[] = { 0, 5, 60, 64, 70 };
    const std::uint32_t rn[] = { 0, 1, 3, 4, 7, 59, 64, 65, 0xfffffff0u };
    const std::uint32_t sn[] = { 0, 1, 4, 59, 60, 64, 65, 0xfffffffbu, 0x80000000u };
    for (std::uint32_t p : pos) {
        std::memset(w, 0, sizeof w); w[0] = 3; w[3] = p; w[4] = 64; w[5] = base;
        for (std::uint32_t n : rn) if (p <= 64 || n <= 8) SweepCase(0, w, n, 0);
        for (std::uint32_t n : sn) SweepCase(1, w, n, 0);
    }
    // Unknown types: the default case (err 0xe), and a zero skip that returns before the type is read.
    const std::uint32_t types[] = { 0, 5, 0x80000000u, 0xffffffffu };
    for (std::uint32_t ty : types) {
        std::memset(w, 0, sizeof w); w[0] = ty;
        SweepCase(0, w, 0, 0); SweepCase(0, w, 4, 0); SweepCase(1, w, 0, 0); SweepCase(1, w, 4, 0);
    }
    // Callback streams on deterministic stubs.
    std::memset(w, 0, sizeof w); w[0] = 4; w[4] = reinterpret_cast<std::uint32_t>(&SweepReadCb);
    w[6] = reinterpret_cast<std::uint32_t>(&SweepSkipCb); w[7] = 0x1234abcdu;
    const std::uint32_t rr[] = { 0, 3, 8, 0xffffffffu };
    for (std::uint32_t r : rr) { SweepCase(0, w, 0, r); SweepCase(0, w, 8, r); }
    const std::uint32_t sr[] = { 0, 1, 0x80000000u };
    for (std::uint32_t r : sr) { SweepCase(1, w, 0, r); SweepCase(1, w, 5, r); }
    // RwStreamWrite (lane savelink2). Memory into the 96-byte destination with room for the write (no allocator call).
    const std::uint32_t wpos[] = { 0, 1, 5, 60, 64 };
    const std::uint32_t wn[] = { 0, 1, 3, 4, 7, 31, 32 };
    for (std::uint32_t p : wpos) {
        std::memset(w, 0, sizeof w); w[0] = 3; w[3] = p; w[4] = 96; w[5] = reinterpret_cast<std::uint32_t>(g_sweepDst);
        for (std::uint32_t n : wn) if (p + n <= 96) SweepCase(2, w, n, 0);
    }
    // First-alloc (no buffer: 0x200 from the RW heap), then grow from a 64-byte RW-heap buffer: by +0x200 below 0x200,
    // by the size from 0x200 up (0x004cbf5f..0x004cbf6d). Sizes stay inside the 256-byte source.
    std::memset(w, 0, sizeof w); w[0] = 3;
    SweepCase(2, w, 0, 0, 1); SweepCase(2, w, 10, 0, 1); SweepCase(2, w, 0xff, 0, 1);
    std::memset(w, 0, sizeof w); w[0] = 3; w[4] = 64;
    const std::uint32_t gpos[] = { 0, 60, 64 };
    const std::uint32_t gn[] = { 5, 10, 65, 0xff };
    for (std::uint32_t p : gpos) { w[3] = p; for (std::uint32_t n : gn) SweepCase(2, w, n, 0, 2); }
    // Unknown types: the default case (err 0xe).
    for (std::uint32_t ty : types) { std::memset(w, 0, sizeof w); w[0] = ty; SweepCase(2, w, 0, 0); SweepCase(2, w, 4, 0); }
    // Callback stream: the write fn sits in [5].
    std::memset(w, 0, sizeof w); w[0] = 4; w[5] = reinterpret_cast<std::uint32_t>(&SweepWriteCb); w[7] = 0x1234abcdu;
    const std::uint32_t wr[] = { 0, 3, 0x80000000u, 0xffffffffu };
    for (std::uint32_t r : wr) { SweepCase(2, w, 0, r); SweepCase(2, w, 8, r); }
    if (es) { es[0] = keep[0]; es[1] = keep[1]; }
    char line[200];
    wsprintfA(line, "SWEEP fn=4cbd30+4cc050+4cbe80 cases=%ld noo=%ld ndiff=%ld err_emitted=%ld errslot=%d\r\n", g_swCases, g_swNoo,
              g_swNdiff, g_swErr, es ? 1 : 0);
    StreamLog(line);
}

__declspec(noinline) std::uint32_t __cdecl ReadDispatch(std::uint32_t* s, void* dst, std::uint32_t size) {
    if (!StreamSelfTestOn() || !s) return RwStreamCoreRead(s, dst, size);
    static LONG s_swept = 0;
    if (InterlockedExchange(&s_swept, 1) == 0) StreamSweep();
    const std::uint32_t type = s[0];
    const int cls = ClassOf(type);
    if (size > kMaxBytes) { InterlockedIncrement(&g_big[0]); return RwStreamCoreRead(s, dst, size); }
    std::uint32_t snap[kWords], w1[kWords];
    std::memcpy(snap, s, sizeof snap);
    if (cls == 0) {
        const std::uint32_t len = (type == 3) ? ((s[4] - s[3]) < size ? (s[4] - s[3]) : size) : 0;
        unsigned char* buf = len ? static_cast<unsigned char*>(std::malloc(len * 2)) : nullptr;
        if (len && !buf) return RwStreamCoreRead(s, dst, size);
        unsigned char* pre = buf; unsigned char* o1 = buf + len;
        if (len) std::memcpy(pre, dst, len);
        const std::uint32_t r1 = ReadOrig(s, dst, size);
        if (len) std::memcpy(o1, dst, len);
        std::memcpy(w1, s, sizeof w1);
        std::memcpy(s, snap, sizeof snap); if (len) std::memcpy(dst, pre, len);
        const std::uint32_t r2 = ReadOrig(s, dst, size);
        const int noo = (r1 != r2) + (std::memcmp(w1, s, sizeof w1) != 0) + (len && std::memcmp(o1, dst, len) != 0);
        std::memcpy(s, snap, sizeof snap); if (len) std::memcpy(dst, pre, len);
        const std::uint32_t rm = RwStreamCoreRead(s, dst, size);
        const int nd = (rm != r1) + (std::memcmp(w1, s, sizeof w1) != 0) + (len && std::memcmp(o1, dst, len) != 0);
        std::free(buf);
        Account(0, cls, type, size, r1, rm, noo, nd, 0);
        return rm;
    }
    if (cls == 1) {
        const std::uint32_t r1 = ReadOrig(s, dst, size);
        std::memcpy(w1, s, sizeof w1);
        unsigned char* o1 = r1 ? static_cast<unsigned char*>(std::malloc(r1)) : nullptr;
        if (r1 && !o1) { Account(0, cls, type, size, r1, r1, 0, 0, 1); return r1; }
        if (r1) std::memcpy(o1, dst, r1);
        // A short read is not re-run: after one, FUN_005509b0(h, -count, 1) does not restore the read position (t0 run,
        // 2026-10-22: the original read 0x835a of 0x10000, the re-read after the rewind returned 0x10000).
        if (r1 != size ||
            (r1 && HostFileSeek(s[3], static_cast<std::uint32_t>(-static_cast<std::int32_t>(r1)), 1) != 0)) {
            std::free(o1); Account(0, cls, type, size, r1, r1, 0, 0, 1); return r1;
        }
        const std::uint32_t rm = RwStreamCoreRead(s, dst, size);
        const int nd = (rm != r1) + (std::memcmp(w1, s, sizeof w1) != 0) + (r1 && std::memcmp(o1, dst, r1) != 0);
        std::free(o1);
        Account(0, cls, type, size, r1, rm, 0, nd, 0);
        return rm;
    }
    CbRec rec = { s[4], s[7], 0, 0, 0, 0 };
    s[4] = reinterpret_cast<std::uint32_t>(&RecReadCb); s[7] = reinterpret_cast<std::uint32_t>(&rec);
    const std::uint32_t r1 = ReadOrig(s, dst, size);
    s[4] = rec.realFn; s[7] = rec.realUser;
    std::memcpy(w1, s, sizeof w1);
    CbRec rep = { 0, 0, 0, 0, 0, rec.ret };
    s[4] = reinterpret_cast<std::uint32_t>(&ReplayReadCb); s[7] = reinterpret_cast<std::uint32_t>(&rep);
    const std::uint32_t rm = RwStreamCoreRead(s, dst, size);
    s[4] = rec.realFn; s[7] = rec.realUser;
    const int nd = (rm != r1) + (rep.calls != rec.calls) + (rep.dst != rec.dst) + (rep.n != rec.n) +
                   (std::memcmp(w1, s, sizeof w1) != 0);
    Account(0, cls, type, size, r1, rm, 0, nd, 0);
    return rm;
}

__declspec(noinline) std::uint32_t* __cdecl SkipDispatch(std::uint32_t* s, std::uint32_t n) {
    if (!StreamSelfTestOn() || !s) return RwStreamCoreSkip(s, n);
    const std::uint32_t type = s[0];
    const int cls = (n == 0) ? 0 : ClassOf(type);
    std::uint32_t snap[kWords], w1[kWords];
    std::memcpy(snap, s, sizeof snap);
    if (cls == 0) {
        std::uint32_t* r1 = SkipOrig(s, n);
        std::memcpy(w1, s, sizeof w1);
        std::memcpy(s, snap, sizeof snap);
        std::uint32_t* r2 = SkipOrig(s, n);
        const int noo = (r1 != r2) + (std::memcmp(w1, s, sizeof w1) != 0);
        std::memcpy(s, snap, sizeof snap);
        std::uint32_t* rm = RwStreamCoreSkip(s, n);
        const int nd = (rm != r1) + (std::memcmp(w1, s, sizeof w1) != 0);
        Account(1, cls, type, n, reinterpret_cast<std::uint32_t>(r1), reinterpret_cast<std::uint32_t>(rm), noo, nd, 0);
        return rm;
    }
    if (cls == 1) {
        std::uint32_t* r1 = SkipOrig(s, n);
        std::memcpy(w1, s, sizeof w1);
        if (!r1 || HostFileSeek(s[3], static_cast<std::uint32_t>(-static_cast<std::int32_t>(n)), 1) != 0) {
            Account(1, cls, type, n, reinterpret_cast<std::uint32_t>(r1), reinterpret_cast<std::uint32_t>(r1), 0, 0, 1);
            return r1;
        }
        std::uint32_t* rm = RwStreamCoreSkip(s, n);
        const int nd = (rm != r1) + (std::memcmp(w1, s, sizeof w1) != 0);
        Account(1, cls, type, n, reinterpret_cast<std::uint32_t>(r1), reinterpret_cast<std::uint32_t>(rm), 0, nd, 0);
        return rm;
    }
    CbRec rec = { s[6], s[7], 0, 0, 0, 0 };
    s[6] = reinterpret_cast<std::uint32_t>(&RecSkipCb); s[7] = reinterpret_cast<std::uint32_t>(&rec);
    std::uint32_t* r1 = SkipOrig(s, n);
    s[6] = rec.realFn; s[7] = rec.realUser;
    std::memcpy(w1, s, sizeof w1);
    CbRec rep = { 0, 0, 0, 0, 0, rec.ret };
    s[6] = reinterpret_cast<std::uint32_t>(&ReplaySkipCb); s[7] = reinterpret_cast<std::uint32_t>(&rep);
    std::uint32_t* rm = RwStreamCoreSkip(s, n);
    s[6] = rec.realFn; s[7] = rec.realUser;
    const int nd = (rm != r1) + (rep.calls != rec.calls) + (rep.n != rec.n) + (std::memcmp(w1, s, sizeof w1) != 0);
    Account(1, cls, type, n, reinterpret_cast<std::uint32_t>(r1), reinterpret_cast<std::uint32_t>(rm), 0, nd, 0);
    return rm;
}
// Live writes: A/B only where the write has no external effect, a memory write that fits ([5] != 0 and room) or an
// unknown type. A file write, an allocating memory write and a callback write run the shared body once (unrun).
__declspec(noinline) std::uint32_t* __cdecl WriteDispatch(std::uint32_t* s, const void* src, std::uint32_t size) {
    if (!StreamSelfTestOn() || !s) return RwStreamCoreWrite(s, src, size);
    static LONG s_swept = 0;
    if (InterlockedExchange(&s_swept, 1) == 0) StreamSweep();
    const std::uint32_t type = s[0];
    const int cls = ClassOf(type);
    const int pure = (type == 3) ? (s[5] != 0 && s[4] - s[3] >= size) : (cls == 0);
    if (!pure || size > kMaxBytes) {
        std::uint32_t* rm = RwStreamCoreWrite(s, src, size);
        if (size > kMaxBytes) InterlockedIncrement(&g_big[2]);
        else Account(2, cls, type, size, 0, reinterpret_cast<std::uint32_t>(rm), 0, 0, 1);
        return rm;
    }
    std::uint32_t snap[kWords], w1[kWords];
    std::memcpy(snap, s, sizeof snap);
    unsigned char* dst = (type == 3) ? reinterpret_cast<unsigned char*>(s[5] + s[3]) : nullptr;
    unsigned char* buf = (dst && size) ? static_cast<unsigned char*>(std::malloc(size * 2)) : nullptr;
    if (dst && size && !buf) return RwStreamCoreWrite(s, src, size);
    unsigned char* pre = buf;
    unsigned char* o1 = buf ? buf + size : nullptr;
    if (buf) std::memcpy(pre, dst, size);
    std::uint32_t* r1 = WriteOrig(s, src, size);
    if (buf) std::memcpy(o1, dst, size);
    std::memcpy(w1, s, sizeof w1);
    std::memcpy(s, snap, sizeof snap); if (buf) std::memcpy(dst, pre, size);
    std::uint32_t* r2 = WriteOrig(s, src, size);
    const int noo = (r1 != r2) + (std::memcmp(w1, s, sizeof w1) != 0) + (buf && std::memcmp(o1, dst, size) != 0);
    std::memcpy(s, snap, sizeof snap); if (buf) std::memcpy(dst, pre, size);
    std::uint32_t* rm = RwStreamCoreWrite(s, src, size);
    const int nd = (rm != r1) + (std::memcmp(w1, s, sizeof w1) != 0) + (buf && std::memcmp(o1, dst, size) != 0);
    std::free(buf);
    Account(2, cls, type, size, reinterpret_cast<std::uint32_t>(r1), reinterpret_cast<std::uint32_t>(rm), noo, nd, 0);
    return rm;
}
}  // namespace

// The installed entries and the path1 exports (re/frida/hooks_registry.py rw_stream_read / rw_stream_skip).
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwStreamRead(std::uint32_t* stream, void* dst, std::uint32_t size) {
    return ReadDispatch(stream, dst, size);
}
RH_ScopedInstall(RwStreamRead, 0x004cbd30);
extern "C" __declspec(dllexport) std::uint32_t* __cdecl RwStreamSkip(std::uint32_t* stream, std::uint32_t n) {
    return SkipDispatch(stream, n);
}
RH_ScopedInstall(RwStreamSkip, 0x004cc050);
// Export name kept from the retired Save/RwStream.cpp copy (re/frida/hooks_registry.py, re/frida/bisect_save_hooks.py).
extern "C" __declspec(dllexport) std::uint32_t* __cdecl RwStreamWrite_s2(std::uint32_t* stream, const void* src,
                                                                         std::uint32_t size) {
    return WriteDispatch(stream, src, size);
}
RH_ScopedInstall(RwStreamWrite_s2, 0x004cbe80);
#endif
