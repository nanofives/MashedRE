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
#endif

void StreamErr(std::uint32_t code)
{
    std::uint32_t blk[2];
    blk[0] = 1;
    blk[1] = HostErrFmt(code);
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

} // namespace Render
} // namespace mashed_re

#ifndef MASHED_STANDALONE
// =====================================================================================================================
// .asi install + in-process A/B. MASHED_LEAF_SELFTEST=1, per call, by stream type:
//   memory (3), unknown type, and a zero skip: no external effect, so the stream words (9) and the destination bytes
//     are snapshotted, the original runs twice through a trampoline (noo), then the shared body (ndiff).
//   file (1, 2): the original runs once, the file is rewound with FUN_005509b0(h, -count, 1), then the shared body runs
//     (ndiff). A skip the original failed is not re-run (position unknown; counted `unrun`, the original's result kept).
//   callback (4): the original runs once with the stream's fn / user slots swapped for a recorder that calls the real
//     callback; the shared body then runs against a replay stub returning the recorded value, and the call count and
//     arguments are compared (ndiff).
// The game keeps the shared body's result. Log: leaf_selftest.log in the CWD (shared with the other leaf self-tests).
// =====================================================================================================================
namespace {
using mashed_re::Render::RwStreamCoreRead;
using mashed_re::Render::RwStreamCoreSkip;
using mashed_re::Render::HostFileSeek;

// Trampolines: the 5-byte E9 covers SUB ESP,8 + PUSH EBX + the first byte of MOV EBX,[ESP+0x10] (read), and
// MOV EAX,[ESP+8] + the first byte of SUB ESP,8 (skip). Each re-runs the clobbered instructions and resumes.
void* g_readResume = reinterpret_cast<void*>(0x004cbd38);
void* g_skipResume = reinterpret_cast<void*>(0x004cc057);
__declspec(naked) std::uint32_t __cdecl ReadOrig(std::uint32_t*, void*, std::uint32_t) {
    __asm {
        sub  esp, 8
        push ebx
        mov  ebx, dword ptr [esp + 10h]
        jmp  dword ptr [g_readResume]
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

// [fn 0 = read, 1 = skip][class 0 = pure, 1 = file, 2 = callback]
volatile LONG g_calls[2][3], g_noo[2][3], g_ndiff[2][3], g_unrun[2][3], g_big[2];
volatile LONG g_total[2];

int ClassOf(std::uint32_t type) { return (type == 1 || type == 2) ? 1 : (type == 4 ? 2 : 0); }

void Account(int fn, int cls, std::uint32_t type, std::uint32_t n, std::uint32_t r1, std::uint32_t rm, int noo, int nd,
             int unrun) {
    const LONG c = InterlockedIncrement(&g_calls[fn][cls]);
    if (noo) InterlockedIncrement(&g_noo[fn][cls]);
    if (nd) InterlockedIncrement(&g_ndiff[fn][cls]);
    if (unrun) InterlockedIncrement(&g_unrun[fn][cls]);
    const LONG t = InterlockedIncrement(&g_total[fn]);
    const char* name = fn == 0 ? "4cbd30" : "4cc050";
    char line[300];
    if (noo || nd || c <= 4) {
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

__declspec(noinline) std::uint32_t __cdecl ReadDispatch(std::uint32_t* s, void* dst, std::uint32_t size) {
    if (!StreamSelfTestOn() || !s) return RwStreamCoreRead(s, dst, size);
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
        if (r1 && HostFileSeek(s[3], static_cast<std::uint32_t>(-static_cast<std::int32_t>(r1)), 1) != 0) {
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
#endif
