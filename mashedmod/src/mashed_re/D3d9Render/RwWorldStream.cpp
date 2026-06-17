// Mashed RE — WS-E-DEVICE (stream half): the RW memory-stream readers that
// un-stub the BSP->RpWorld loader (RwWorldLoad.cpp).
//
// These are the DEVICE-AGNOSTIC half of WS-E-DEVICE: pure RW-stream chunk parsing
// (no D3D9, no RW device). With them real, Rwl_StreamFindChunk no longer returns 0,
// so RwWorldLoad_StreamRead actually walks a track's rwID_WORLD chunk and builds the
// RpWorld + sector tree in memory. The DEVICE submit half (RpWorldInstance device
// VB/IB, the RxPipeline node bodies FUN_004eb3d0/FUN_004ea6d0, the
// IDirect3DDevice9::DrawIndexedPrimitive site at 0x004e1007) stays the deferred
// stub seam — it is RW-engine / D3D9-device-state bound (needs RwEngineOpen +
// the RW device table DAT_007d3ff8) and is the next session's work (see report).
//
// Verbatim ports (Ghidra pool6, read-only, 2026-06-16; every RVA cited):
//   FUN_004cbd30 RwStreamRead        -> Rwl_StreamRead       (case 3 = memory)
//   FUN_004cc790 (read-or-error)     -> Rwl_StreamReadReal
//   FUN_004cc5e0 RwStreamFindChunk   -> Rwl_StreamFindChunk
//   FUN_004cc400 RwStreamReadChunkHeader (internal: ChunkHeaderRead)
//   FUN_004cc050 RwStreamSkip            (internal: StreamSkip, case 3)
//   FUN_004cc4f0 type-validator: return discarded by the header reader, so the
//     chunk type passes through unchanged — faithfully omitted.
// STATUS: verbatim, PENDING diff-original C4. The memory-stream struct layout is
// taken from FUN_004cbd30/004cc050 case 3 ([0]=type, [3]=pos, [4]=len, [5]=base).
//
// Anchor: MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "RwWorldLoad.h"
#include <cstdint>
#include <cstring>

namespace mashed_re {
namespace D3d9Render {

// Error helpers stay no-op stubs (RwWorldLoadStubs.cpp): Rwl_ErrFmt / Rwl_ErrEmit.
unsigned Rwl_ErrFmt(unsigned code, unsigned a);   // FUN_004d7ff0
void     Rwl_ErrEmit(void* blk);                  // FUN_004d8480

// ---- RW memory stream (the [0]=type=3 case of FUN_004cbd30/004cc050) ---------
// Word layout matching the original RwStream: [0]=type, [3]=pos, [4]=len, [5]=base.
struct RwMemStream { int w[6]; };

void RwWorldLoad_OpenMemory(RwMemStream* s, const void* buf, int len)
{
    std::memset(s, 0, sizeof(*s));
    s->w[0] = 3;                                   // rwSTREAMMEMORY
    s->w[3] = 0;                                   // position
    s->w[4] = len;                                 // length
    s->w[5] = reinterpret_cast<int>(buf);          // base
}

// ---- 0x004cbd30  RwStreamRead (case 3: memory) ------------------------------
// Returns bytes read. Clamps to (len - pos); copies dword-then-byte; advances pos.
int Rwl_StreamRead(void* stream, void* dst, int size)
{
    int* p = static_cast<int*>(stream);
    if (p[0] != 3) return 0;                        // only the memory path is ported
    unsigned avail = static_cast<unsigned>(p[4] - p[3]);
    unsigned n = static_cast<unsigned>(size);
    if (avail < n) {                               // short read -> clamp (+ orig emits err 5)
        unsigned eb[2] = {1, Rwl_ErrFmt(5, 0)}; Rwl_ErrEmit(eb);
        n = avail;
    }
    const unsigned char* src = reinterpret_cast<const unsigned char*>(p[5] + p[3]);
    std::memcpy(dst, src, n);
    p[3] += static_cast<int>(n);
    return static_cast<int>(n);
}

// ---- 0x004cc790  read-or-error: returns the stream on full read, else 0 ------
int Rwl_StreamReadReal(void* stream, void* dst, int size)
{
    int got = Rwl_StreamRead(stream, dst, size);
    if (got == 0) { unsigned eb[2] = {1, Rwl_ErrFmt(0x8000001a, 0)}; Rwl_ErrEmit(eb); return 0; }
    return reinterpret_cast<int>(stream);          // (original returns param_1)
}

// ---- 0x004cc050  RwStreamSkip (case 3: memory) ------------------------------
static int StreamSkip(void* stream, int n)
{
    int* p = static_cast<int*>(stream);
    if (n == 0) return reinterpret_cast<int>(stream);
    if (p[0] != 3) return 0;
    if (static_cast<unsigned>(n + p[3]) <= static_cast<unsigned>(p[4])) {
        p[3] += n;
        return reinterpret_cast<int>(stream);
    }
    p[3] = p[4];                                    // clamp + err 5
    unsigned eb[2] = {1, Rwl_ErrFmt(5, 0)}; Rwl_ErrEmit(eb);
    return 0;
}

// ---- 0x004cc400  RwStreamReadChunkHeader (12 bytes: type, size, packedVer) ---
// Decodes the RW packed library-id version exactly as the original. Returns 1/0.
static int ChunkHeaderRead(void* stream, int* typeOut, int* sizeOut, unsigned* libIdOut)
{
    int hdr[3] = {0, 0, 0};
    if (Rwl_StreamRead(stream, hdr, 0xc) != 0xc) {
        unsigned eb[2] = {1, Rwl_ErrFmt(0x8000001a, 0)}; Rwl_ErrEmit(eb);
        return 0;
    }
    const int      type   = hdr[0];                 // local_20
    const int      size   = hdr[1];                 // local_1c
    const unsigned packed = static_cast<unsigned>(hdr[2]);  // local_18
    unsigned libId;
    if ((packed & 0xffff0000u) == 0) {
        libId = packed << 8;                        // old-style version
    } else {
        libId = ((packed >> 0xe) & 0x3ff00u) + 0x30000u | ((packed >> 0x10) & 0x3fu);
    }
    if (typeOut)  *typeOut  = type;                 // FUN_004cc4f0(type) result discarded
    if (sizeOut)  *sizeOut  = size;
    if (libIdOut) *libIdOut = libId;
    return 1;
}

// ---- 0x004cc5e0  RwStreamFindChunk -------------------------------------------
// Scans chunks (skipping non-matching by their size) until `type` is found, then
// version-gates [0x35000, 0x37002]. Sets *lenOut=size, *verOut=libId. Returns 1/0.
int Rwl_StreamFindChunk(void* stream, int type, int* lenOut, unsigned* verOut)
{
    int found = 0, size = 0; unsigned libId = 0;
    if (ChunkHeaderRead(stream, &found, &size, &libId) == 0) return 0;
    while (found != type) {
        if (StreamSkip(stream, size) == 0) return 0;
        if (ChunkHeaderRead(stream, &found, &size, &libId) == 0) return 0;
    }
    if (libId < 0x35000u || 0x37002u < libId) {     // version gate
        unsigned eb[2] = {1, Rwl_ErrFmt(0x80000004, 0)}; Rwl_ErrEmit(eb);
        return 0;
    }
    if (lenOut) *lenOut = size;
    if (verOut) *verOut = libId;
    return 1;
}

// Convenience: parse an in-memory rwID_WORLD blob into an RpWorld (or null).
void* RwWorldLoad_FromBytes(const void* buf, int len)
{
    RwMemStream s;
    RwWorldLoad_OpenMemory(&s, buf, len);
    return RwWorldLoad_StreamRead(&s);
}

} // namespace D3d9Render
} // namespace mashed_re
