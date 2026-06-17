// Mashed RE — WS-E2: BSP -> RpWorld world-stream loader (verbatim). See RwWorldLoad.h.
// PENDING diff-original C4. Ghidra pool6 read-only 2026-06-16; every RVA cited.
// Faithful transcription of FUN_004e99b0 / FUN_004e9e40 / FUN_004ea220; the RW
// stream/error/plugin/instance helpers are an inert stub seam (RwWorldLoadStubs.cpp).
#include "RwWorldLoad.h"
#include <cstdint>

namespace mashed_re {
namespace D3d9Render {

// ---- word-index (puVar[i]) + byte views on RW structs -----------------------
static inline int&   W (void* p, int i)  { return *reinterpret_cast<int*>(reinterpret_cast<char*>(p) + i * 4); }
static inline float& WF(void* p, int i)  { return *reinterpret_cast<float*>(reinterpret_cast<char*>(p) + i * 4); }
static inline unsigned char& BY(void* p, int o) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(p) + o); }
static inline short& SH(void* p, int o)  { return *reinterpret_cast<short*>(reinterpret_cast<char*>(p) + o); }

// ---- RW helper deps (stubbed in RwWorldLoadStubs.cpp; real RVAs cited) -------
int   Rwl_StreamFindChunk(void* s, int type, int* lenOut, unsigned* verOut); // FUN_004cc5e0
int   Rwl_StreamRead(void* s, void* buf, int len);                            // FUN_004cbd30
int   Rwl_StreamReadReal(void* s, void* buf, int len);                        // FUN_004cc790
unsigned Rwl_ErrFmt(unsigned code, unsigned a);                               // FUN_004d7ff0
void  Rwl_ErrEmit(void* blk);                                                 // FUN_004d8480
void  Rwl_PluginCtor(void* desc, void* obj);                                  // FUN_004d8000
int   Rwl_ReadExtensions(void* desc, void* s, void* obj);                     // FUN_004e1b60
void  Rwl_BinMeshAttach(void* desc, int natA, void* obj, int natB);           // FUN_004e1c90
void* Rwl_Alloc(unsigned size, unsigned tag);                                 // (**(DAT_007d3ff8+0x108))
void  Rwl_WorldSubInit(void* world, unsigned size);                           // FUN_004e5280
void  Rwl_WorldSubInit2(void* world);                                         // FUN_004e51e0
void  Rwl_WorldSetRenderCB(void* world, int mode);                            // FUN_004e5820
void  Rwl_MaterialInit(void* mat, int first);                                 // FUN_004e8090
int   Rwl_MatListRead(void* s, void* matListField);                           // FUN_004f3e90
void  Rwl_WorldDestroy(void* world);                                          // RpWorldDestroy
int   Rwl_WorldInstance(void* world);                                         // RpWorldInstance
void  Rwl_FrameDestroy();                                                     // RwFrameDestroy
void  Rwl_FrameDestroyHierarchy(void* obj);                                   // RwFrameDestroyHierarchy
extern void* g_rpWorldPluginDesc;   // &DAT_0061864c (plugin registry; value-as-int = 0x70)
extern void* g_rpSectorPluginDesc;  // &DAT_006189e4 (plugin registry; value-as-int = 0x88)
extern int   g_binMeshWorldA, g_binMeshWorldB;   // DAT_007d7278 / DAT_007d7274
extern int   g_binMeshSectorA, g_binMeshSectorB; // DAT_007d727c / DAT_007d7270

static void* PlaneSectorRead(void* stream, int* arena, void* world, unsigned fmt);

// ===========================================================================
// 0x004e9e40  RpWorldSectorRead(stream, arena, world, fmt) — rwID_ATOMICSECTOR(9) leaf
// Allocates a 0x88 sector from the inline arena, reads bbox+counts header (0x2c),
// then bump-allocates triangles/positions/normals/prelight/texcoords. Returns sector.
// ===========================================================================
static void* WorldSectorRead(void* stream, int* arena, void* world, unsigned fmt)
{
    unsigned ver = 0;
    if (Rwl_StreamFindChunk(stream, 1, nullptr, &ver) == 0) return nullptr;
    if (ver < 0x35000u || 0x37002u < ver) {                         // version gate
        void* eb[2]; eb[0] = reinterpret_cast<void*>(2);
        eb[1] = reinterpret_cast<void*>(Rwl_ErrFmt(0x80000004, 0)); Rwl_ErrEmit(eb);
        return nullptr;
    }
    // 0x2c-byte sector header: [u16 matWinBase, u16 _, i32 numTris, i32 numVerts, i32 bbox6...]
    unsigned char hdr[0x2c] = {0};
    if (Rwl_StreamRead(stream, hdr, 0x2c) != 0x2c) return nullptr;
    const unsigned short matWinBase = *reinterpret_cast<unsigned short*>(hdr + 0x00);
    const int numTris  = *reinterpret_cast<int*>(hdr + 0x04);   // local_28
    const int numVerts = *reinterpret_cast<int*>(hdr + 0x08);   // local_24
    const int* bbox    = reinterpret_cast<int*>(hdr + 0x0c);    // local_20..local_c (6 ints)

    void* s = reinterpret_cast<void*>(*arena);                  // bump-alloc the sector
    *arena = reinterpret_cast<int>(s) + static_cast<int>(rwl::kRpWorldSectorSize);
    if (!s) { void* eb[2]; eb[0]=reinterpret_cast<void*>(2);
        eb[1]=reinterpret_cast<void*>(Rwl_ErrFmt(0x80000013, rwl::kRpWorldSectorSize)); Rwl_ErrEmit(eb); return nullptr; }

    W(s, 0) = -1;                                               // +0x00 leaf tag 0xffffffff
    W(s, 0x1b) = bbox[0]; W(s, 0x1c) = bbox[1]; W(s, 0x1d) = bbox[2];  // +0x6c..0x74
    W(s, 0x18) = bbox[3]; W(s, 0x19) = bbox[4]; W(s, 0x1a) = bbox[5];  // +0x60..0x68
    SH(s, 0x80) = static_cast<short>(matWinBase);
    SH(s, 0x84) = static_cast<short>(numTris);
    SH(s, 0x82) = static_cast<short>(numVerts);
    W(s, 1) = 0; W(s, 2) = 0; W(s, 0xc) = 0; W(s, 3) = 0;       // tris/verts/prelight/normals ptrs
    for (int k = 0; k < 8; ++k) W(s, 4 + k) = 0;               // texcoord-set ptrs +0x10..
    W(s, 0xf) = reinterpret_cast<int>(&W(s, 0xe)); W(s, 0xe) = reinterpret_cast<int>(&W(s, 0xe)); // +0x38 self-link
    W(s, 0x1f) = 0; W(s, 0xd) = 0; W(s, 0x1e) = 0;
    W(s, 0x11) = reinterpret_cast<int>(&W(s, 0x10)); W(s, 0x10) = reinterpret_cast<int>(&W(s, 0x10)); // +0x40 self-link

    bool ok = true;
    if ((static_cast<unsigned>(W(world, 2)) & 0x1000000) == 0) {     // world+0x08 format flags
        if (numVerts != 0) {
            int p = *arena; *arena = p + numVerts * 0xc; W(s, 2) = p; // +0x08 positions
            if (!p) ok = false;
            else if (Rwl_StreamReadReal(stream, reinterpret_cast<void*>(p), numVerts * 0xc) == 0) ok = false;
            if (ok && (fmt & 0x10) != 0) {                           // normals
                int n = *arena; *arena = n + numVerts * 4; W(s, 3) = n;
                if (!n || Rwl_StreamRead(stream, reinterpret_cast<void*>(n), numVerts * 4) != numVerts * 4) ok = false;
            }
            if (ok && (fmt & 8) != 0) {                              // prelight RGBA
                int c = *arena; *arena = c + numVerts * 4; W(s, 0xc) = c;
                if (!c || Rwl_StreamRead(stream, reinterpret_cast<void*>(c), numVerts * 4) == 0) ok = false;
            }
            if (ok) {                                               // texcoord sets (world+0x20)
                int sets = W(world, 8);  // *(int*)(world+0x20) numTexCoordSets
                int bytes = static_cast<int>(static_cast<unsigned short>(SH(s, 0x82))) * 8;
                for (int i = 0; ok && i < sets; ++i) {
                    int t = *arena; *arena = t + bytes; W(s, 4 + i) = t;
                    if (!t || Rwl_StreamReadReal(stream, reinterpret_cast<void*>(t), bytes) == 0) ok = false;
                }
            }
        }
        if (ok && numTris != 0) {                                    // triangles +0x04 (numTris*8)
            int tr = *arena; *arena = tr + numTris * 8; W(s, 1) = tr;
            if (!tr || Rwl_StreamReadReal(stream, reinterpret_cast<void*>(tr), numTris * 8) == 0) ok = false;
            else if (ver < 0x36002u) {                               // pre-0x36002 byte-swap tri words
                for (int i = 0; i < numTris; ++i) {
                    unsigned* w0 = reinterpret_cast<unsigned*>(tr + i * 8);
                    unsigned* w1 = reinterpret_cast<unsigned*>(tr + i * 8 + 4);
                    unsigned a = *w0, b = *w1;
                    *reinterpret_cast<short*>(w0)             = static_cast<short>(a >> 16);
                    *reinterpret_cast<short*>((char*)w0 + 2)  = static_cast<short>(b);
                    *reinterpret_cast<short*>(w1)             = static_cast<short>(b >> 16);
                    *reinterpret_cast<short*>((char*)w1 + 2)  = static_cast<short>(a);
                }
            }
        }
    }
    if (!ok) {
        if ((BY(world, 3) & 1) != 0) { Rwl_FrameDestroy(); return nullptr; }
        Rwl_FrameDestroyHierarchy(s); return nullptr;
    }
    // rwID_EXTENSION plugins (binMesh native data -> DAT_007d727c / DAT_007d7270)
    Rwl_PluginCtor(g_rpSectorPluginDesc, s);
    g_binMeshSectorA = 0; g_binMeshSectorB = 0;
    if (Rwl_ReadExtensions(g_rpSectorPluginDesc, stream, s) == 0) {
        if ((BY(world, 3) & 1) != 0) { Rwl_FrameDestroy(); return nullptr; }
        Rwl_FrameDestroyHierarchy(s); return nullptr;
    }
    if (g_binMeshSectorA != 0)
        Rwl_BinMeshAttach(g_rpSectorPluginDesc, g_binMeshSectorA, s, g_binMeshSectorB);
    return s;
}

// ===========================================================================
// 0x004ea220  RpPlaneSectorRead(stream, arena, world, fmt) — rwID_PLANESECTOR(10) node
// 0x18-byte plane sector: +0x00 axis, +0x04 split, +0x08/+0x0c children, +0x10/+0x14 bounds.
// Recurses (plane child) or descends to WorldSectorRead (atomic child) per hdr flags.
// ===========================================================================
static void* PlaneSectorRead(void* stream, int* arena, void* world, unsigned fmt)
{
    int len = 0; unsigned ver = 0;
    if (Rwl_StreamFindChunk(stream, 1, &len, &ver) == 0) return nullptr;
    if (ver < 0x35000u || 0x37002u < ver) {
        void* eb[2]; eb[0]=reinterpret_cast<void*>(2); eb[1]=reinterpret_cast<void*>(Rwl_ErrFmt(0x80000004,0)); Rwl_ErrEmit(eb);
        return nullptr;
    }
    int h[6] = {0};                                            // plane-sector header (read len bytes)
    if (Rwl_StreamRead(stream, h, len) != len) return nullptr;

    void* s = reinterpret_cast<void*>(*arena);                 // bump-alloc 0x18
    *arena = reinterpret_cast<int>(s) + 0x18;
    if (!s) { void* eb[2]; eb[0]=reinterpret_cast<void*>(2); eb[1]=reinterpret_cast<void*>(Rwl_ErrFmt(0x80000013,0x18)); Rwl_ErrEmit(eb); return nullptr; }

    W(s, 0) = h[0];                                            // +0x00 axis/type
    W(s, 1) = h[1];                                            // +0x04 split value
    if ((fmt & 0x40000000) == 0) { W(s, 4) = h[1]; W(s, 5) = h[1]; } // +0x10/+0x14 bounds
    else                         { W(s, 4) = h[4]; W(s, 5) = h[5]; }

    // left child: plane (h[2]==0) or atomic
    if (h[2] == 0) {
        if (Rwl_StreamFindChunk(stream, 10, nullptr, &ver) == 0) goto fail;
        if (ver < 0x35000u || 0x37003u <= ver) goto badver;
        W(s, 2) = reinterpret_cast<int>(PlaneSectorRead(stream, arena, world, fmt));
    } else {
        if (Rwl_StreamFindChunk(stream, 9, nullptr, &ver) == 0) goto fail;
        if (ver < 0x35000u || 0x37003u <= ver) goto badver;
        W(s, 2) = reinterpret_cast<int>(WorldSectorRead(stream, arena, world, fmt));
    }
    if (W(s, 2) == 0) goto fail;
    // right child: plane (h[3]==0) or atomic
    if (h[3] == 0) {
        if (Rwl_StreamFindChunk(stream, 10, nullptr, &ver) == 0) goto fail;
        if (ver < 0x35000u || 0x37002u < ver) goto badver;
        W(s, 3) = reinterpret_cast<int>(PlaneSectorRead(stream, arena, world, fmt));
    } else {
        if (Rwl_StreamFindChunk(stream, 9, nullptr, &ver) == 0) goto fail;
        if (ver < 0x35000u || 0x37002u < ver) goto badver;
        W(s, 3) = reinterpret_cast<int>(WorldSectorRead(stream, arena, world, fmt));
    }
    if (W(s, 3) != 0) return s;
    goto fail;

badver:
    { void* eb[2]; eb[0]=reinterpret_cast<void*>(2); eb[1]=reinterpret_cast<void*>(Rwl_ErrFmt(0x80000004,0)); Rwl_ErrEmit(eb); }
fail:
    if (W(s, 2) == 0) return nullptr;
    if ((BY(world, 3) & 1) == 0) { Rwl_FrameDestroyHierarchy(reinterpret_cast<void*>(W(s, 2))); W(s, 2) = 0; return nullptr; }
    Rwl_FrameDestroy(); W(s, 2) = 0; return nullptr;
}

// ===========================================================================
// 0x004e99b0  RpWorldStreamRead(stream) — builds the RpWorld from the rwID_WORLD chunk.
// ===========================================================================
void* RwWorldLoad_StreamRead(void* stream)
{
    int len = 0; unsigned ver = 0;
    if (Rwl_StreamFindChunk(stream, 1, &len, &ver) == 0) return nullptr;
    if (ver < 0x35000u || 0x37002u < ver) {
        void* eb[2]; eb[0]=reinterpret_cast<void*>(2); eb[1]=reinterpret_cast<void*>(Rwl_ErrFmt(0x80000004,0)); Rwl_ErrEmit(eb);
        return nullptr;
    }
    int hdr[16] = {0};                                         // 0x40-byte world header
    if (Rwl_StreamRead(stream, hdr, len) != len) return nullptr;
    const int  treeType = hdr[0];        // local_40 (0 -> planesector root, else atomicsector)
    const float bboxX = reinterpret_cast<float&>(hdr[1]);  // fStack_3c
    const float bboxY = reinterpret_cast<float&>(hdr[2]);  // fStack_38
    const float bboxZ = reinterpret_cast<float&>(hdr[3]);  // fStack_34
    const int  numTris = hdr[4];   // local_30
    const int  numVerts = hdr[5];  // local_2c
    const int  numPlaneSec = hdr[6]; // local_28 (×0x18)
    const int  numMat = hdr[7];    // local_24 (×sectorSize)
    const int  guard = hdr[8];     // local_20 (must be < 1)
    const unsigned format = static_cast<unsigned>(hdr[9]); // local_1c
    const int* originBBox = &hdr[10]; // auStack_18[6] -> world+0x14..0x19

    if (guard >= 1) return nullptr;

    // compute alloc size from format flags (verbatim §9 step 3)
    unsigned uVar5;
    if ((format & 0xff0000) == 0) {
        if (static_cast<char>(format) < 0) uVar5 = 2;
        else uVar5 = (format >> 2) & 1;
    } else uVar5 = (format >> 16) & 0xff;          // numTexCoordSets

    unsigned size = rwl::kRpWorldBaseSize + rwl::kRpWorldSectorSize * static_cast<unsigned>(numMat)
                  + static_cast<unsigned>(numPlaneSec) * 0x18;
    if ((format & 0x1000000) == 0) {
        int e = static_cast<int>(size) + numVerts * 0xc;
        if (format & 0x10) e += numVerts * 4;     // normals
        if (format & 8)    e += numVerts * 4;     // prelight
        if (uVar5)         e += numVerts * static_cast<int>(uVar5) * 8; // texcoords
        size = static_cast<unsigned>(e) + numTris * 8;
    }

    void* w = Rwl_Alloc(size, 0x3000b);
    if (!w) { void* eb[2]; eb[0]=reinterpret_cast<void*>(2); eb[1]=reinterpret_cast<void*>(Rwl_ErrFmt(0x80000013,0)); Rwl_ErrEmit(eb); return nullptr; }
    for (unsigned i = 0; i < size; ++i) reinterpret_cast<unsigned char*>(w)[i] = 0;

    int arena = reinterpret_cast<int>(w) + static_cast<int>(rwl::kRpWorldBaseSize); // inline sector cursor
    BY(w, 0) = 7;                                  // rpWORLD type
    BY(w, 1) = 0; BY(w, 2) = 0; W(w, 1) = 0; BY(w, 3) = 1;
    Rwl_WorldSubInit(w, size);                     // FUN_004e5280
    W(w, 3) = 2;                                   // +0x0c = 2
    W(w, 2) = static_cast<int>(format);            // +0x08 format flags
    WF(w, 0x11) = bboxX * rwl::kBBoxScale;         // +0x44..
    WF(w, 0x12) = bboxY * rwl::kBBoxScale;
    WF(w, 0x13) = bboxZ * rwl::kBBoxScale;
    W(w, 8) = static_cast<int>(uVar5);             // +0x20 numTexCoordSets
    W(w, 0xe) = reinterpret_cast<int>(&W(w, 0xd)); W(w, 0xd) = reinterpret_cast<int>(&W(w, 0xd)); // +0x34 self-link
    W(w, 7) = 0; W(w, 0x1b) = 0;
    W(w, 0xc) = reinterpret_cast<int>(&W(w, 0xb)); W(w, 0xb) = reinterpret_cast<int>(&W(w, 0xb)); // +0x2c self-link
    W(w, 0x10) = reinterpret_cast<int>(&W(w, 0xf)); W(w, 0xf) = reinterpret_cast<int>(&W(w, 0xf)); // +0x3c self-link

    if (Rwl_StreamFindChunk(stream, 8, nullptr, &ver) == 0) { Rwl_WorldDestroy(w); return nullptr; } // rwID_MATLIST
    if (ver < 0x35000u || 0x37002u < ver) { void* eb[2]; eb[0]=reinterpret_cast<void*>(2); eb[1]=reinterpret_cast<void*>(Rwl_ErrFmt(0x80000004,0)); Rwl_ErrEmit(eb); Rwl_WorldDestroy(w); return nullptr; }
    if (Rwl_MatListRead(stream, &W(w, 4)) == 0) { Rwl_WorldDestroy(w); return nullptr; }   // world+0x10

    // sector tree root -> world+0x1c (word 7)
    void* root;
    if (treeType == 0) {
        if (Rwl_StreamFindChunk(stream, 10, nullptr, &ver) == 0) { Rwl_WorldDestroy(w); return nullptr; }
        if (ver < 0x35000u || 0x37002u < ver) { void* eb[2]; eb[0]=reinterpret_cast<void*>(2); eb[1]=reinterpret_cast<void*>(Rwl_ErrFmt(0x80000004,0)); Rwl_ErrEmit(eb); Rwl_WorldDestroy(w); return nullptr; }
        root = PlaneSectorRead(stream, &arena, w, format);
    } else {
        if (Rwl_StreamFindChunk(stream, 9, nullptr, &ver) == 0) { Rwl_WorldDestroy(w); return nullptr; }
        if (ver < 0x35000u || 0x37002u < ver) { void* eb[2]; eb[0]=reinterpret_cast<void*>(2); eb[1]=reinterpret_cast<void*>(Rwl_ErrFmt(0x80000004,0)); Rwl_ErrEmit(eb); Rwl_WorldDestroy(w); return nullptr; }
        root = WorldSectorRead(stream, &arena, w, format);
    }
    W(w, 7) = reinterpret_cast<int>(root);         // +0x1c root sector
    if (!root) { Rwl_WorldDestroy(w); return nullptr; }

    W(w, 0xa) = reinterpret_cast<int>(&W(w, 0xb)); W(w, 9) = 0;
    for (int i = 0; i < 6; ++i) W(w, 0x14 + i) = originBBox[i];  // +0x50..0x67 origin/bbox

    Rwl_WorldSubInit2(w);                          // FUN_004e51e0
    Rwl_WorldSetRenderCB(w, 0);                    // FUN_004e5820 -> world+0x68 = FUN_004e5190
    for (int i = 0; i < W(w, 5); ++i) {            // per-material init (world+0x14 list, world+0x14 count)
        int* mat = *reinterpret_cast<int**>(W(w, 4) + i * 4);
        if (mat && *mat != 0) Rwl_MaterialInit(mat, *mat);
    }
    Rwl_PluginCtor(g_rpWorldPluginDesc, w);        // FUN_004d8000
    g_binMeshWorldA = 0; g_binMeshWorldB = 0;
    if (Rwl_ReadExtensions(g_rpWorldPluginDesc, stream, w) == 0) { Rwl_WorldDestroy(w); return nullptr; }
    if (g_binMeshWorldA != 0)
        Rwl_BinMeshAttach(g_rpWorldPluginDesc, g_binMeshWorldA, w, g_binMeshWorldB);
    if (Rwl_WorldInstance(w) == 0) { Rwl_WorldDestroy(w); return nullptr; } // RpWorldInstance (device VB/IB)
    return w;
}

} // namespace D3d9Render
} // namespace mashed_re
