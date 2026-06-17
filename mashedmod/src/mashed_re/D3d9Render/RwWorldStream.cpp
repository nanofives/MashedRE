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
#include <cstdlib>

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

// ===========================================================================
// WS-E-DEVICE-2: parse-completing chunk consumers (DEVICE-AGNOSTIC).
//
// RwWorldLoad_FromBytes was blocked: Rwl_MatListRead (FUN_004f3e90) and
// Rwl_ReadExtensions (FUN_004e1b60) were inert stubs that did NOT consume their
// chunks, so the stream mis-aligned before the sector tree and the sectors never
// parsed. These consume them by-size (GEOMETRY-ONLY: materials + binMesh strips
// are skipped, not bound — the sectors' raw positions/UV/triangles parse intact;
// material binding + the device VB/IB submit remain the RwEngineOpen-bound tail).
// ===========================================================================
int Rwl_StreamSkip(void* stream, int n) { return StreamSkip(stream, n); }   // expose case-3 skip
void Rwl_WorldDestroy(void* w);                                             // RpWorldDestroy (free)

// FUN_004f3e90 RpMaterialListStreamRead — consumes [rwID_STRUCT: i32 count]
// [i32 idx[count]] [rwID_MATERIAL chunk per idx<0]. Leaves the matlist empty
// (out[0..2]=0); only advances the stream so the sector tree reads correctly.
int Rwl_MatListRead(void* stream, void* matListField)
{
    int* out = static_cast<int*>(matListField);     // world+0x10: [0]=list [1]=count [2]=cap
    out[0] = 0; out[1] = 0; out[2] = 0;
    int len = 0; unsigned ver = 0;
    if (Rwl_StreamFindChunk(stream, 1, &len, &ver) == 0) return 0;          // rwID_STRUCT
    int count = 0;
    if (Rwl_StreamReadReal(stream, &count, 4) == 0) return 0;
    if (count <= 0) return 1;                                               // empty matlist
    int* idx = static_cast<int*>(std::malloc(static_cast<size_t>(count) * 4));
    if (!idx) return 0;
    if (Rwl_StreamReadReal(stream, idx, count * 4) == 0) { std::free(idx); return 0; }
    int rc = 1;
    for (int i = 0; i < count; ++i) {                                       // idx<0 => a new material chunk
        if (idx[i] < 0) {
            int mlen = 0; unsigned mver = 0;
            if (Rwl_StreamFindChunk(stream, 7, &mlen, &mver) == 0) { rc = 0; break; }  // rwID_MATERIAL
            if (mlen > 0 && Rwl_StreamSkip(stream, mlen) == 0)     { rc = 0; break; }  // skip body (deferred)
        }
    }
    std::free(idx);
    return rc;
}

// FUN_004e1b60 plugin-extension stream reader — consumes the rwID_EXTENSION
// wrapper (+ all its sub-chunks). Geometry-only: skip the whole wrapper (binMesh
// native strips etc. deferred; the sector raw triangles are used instead).
int Rwl_ReadExtensions(void* /*desc*/, void* stream, void* /*obj*/)
{
    int len = 0; unsigned ver = 0;
    if (Rwl_StreamFindChunk(stream, 3, &len, &ver) == 0) return 0;          // rwID_EXTENSION
    if (len > 0 && Rwl_StreamSkip(stream, len) == 0) return 0;
    return 1;
}

// ---- parse self-check: exercise the now-complete loader + count geometry -----
// Walks the parsed sector tree (leaf tag +0x00 == -1; plane children +0x08/+0x0c;
// leaf counts u16 @ +0x82 verts / +0x84 tris). Cross-check the totals vs the
// standalone's byte-faithful Track::World in the MAIN tree. Returns 1 on a valid parse.
static void PC_Walk(void* sec, int* nSec, long* nV, long* nT)
{
    if (!sec) return;
    if (*static_cast<int*>(sec) == -1) {                                    // leaf
        *nSec += 1;
        *nV += static_cast<unsigned short>(*reinterpret_cast<short*>(static_cast<char*>(sec) + 0x82));
        *nT += static_cast<unsigned short>(*reinterpret_cast<short*>(static_cast<char*>(sec) + 0x84));
        return;
    }
    PC_Walk(*reinterpret_cast<void**>(static_cast<char*>(sec) + 0x08), nSec, nV, nT);  // plane left
    PC_Walk(*reinterpret_cast<void**>(static_cast<char*>(sec) + 0x0c), nSec, nV, nT);  // plane right
}

int RwWorldLoad_ParseCheck(const void* bsp, int len, int* outSectors, long* outVerts, long* outTris)
{
    void* w = RwWorldLoad_FromBytes(bsp, len);
    if (!w) return 0;
    int nSec = 0; long nV = 0, nT = 0;
    PC_Walk(*reinterpret_cast<void**>(static_cast<char*>(w) + 0x1c), &nSec, &nV, &nT);  // world+0x1c root
    if (outSectors) *outSectors = nSec;
    if (outVerts)   *outVerts   = nV;
    if (outTris)    *outTris    = nT;
    Rwl_WorldDestroy(w);                                                    // single-block free
    return 1;
}

} // namespace D3d9Render
} // namespace mashed_re
