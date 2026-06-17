// Mashed RE — WS-E1: RenderWare world-render generic traversal layer (verbatim).
// See RwWorldRender.h. PENDING parity. Ghidra pool6 read-only 2026-06-16; every
// RVA decompiled this session (NO-GUESSING). RW struct offsets cited inline.
#include "RwWorldRender.h"
#include <cstdio>
#include <cstdlib>

namespace mashed_re {
namespace D3d9Render {

// ---- RW engine substrate (bound via RwWorldRender_SetEngine; null in standalone) -
// g_rwGlobals  == the game's DAT_007d3ff8 (RwGlobals*).
// g_objExtBase == DAT_007d716c (per-object extension base; sector list lives here).
static unsigned char* g_rwGlobals  = nullptr;   // DAT_007d3ff8
static unsigned char* g_objExtBase = nullptr;   // DAT_007d716c
static bool           s_toggleChecked = false;
static bool           s_toggleOn       = false;

// ---- byte-offset views ------------------------------------------------------
static inline unsigned char* P (void* b, int off) { return reinterpret_cast<unsigned char*>(b) + off; }
static inline int&    I  (void* b, int off) { return *reinterpret_cast<int*>(P(b, off)); }
static inline void*&  PP (void* b, int off) { return *reinterpret_cast<void**>(P(b, off)); }
static inline short&  S  (void* b, int off) { return *reinterpret_cast<short*>(P(b, off)); }
static inline unsigned char& B(void* b, int off) { return *reinterpret_cast<unsigned char*>(P(b, off)); }
static inline float*  F  (void* b, int off) { return reinterpret_cast<float*>(P(b, off)); }

// ===========================================================================
// 0x004c1b40  RwCameraFrustumTestSphere(cam, sphere) -> 0 outside / 1 intersect / 2 inside
// 6 frustum planes at cam+0x94, stride 5 floats: [nx,ny,nz, d, _]; sphere = [cx,cy,cz, r].
// Verbatim of FUN_004c1b40 (body 0x004c1b40..0x004c1ba7). Self-contained — diff-checkable.
// ===========================================================================
int RwCameraFrustumTestSphere(void* cam, const float* sphere)
{
    int result = 2;
    float* pl = F(cam, 0x94);
    for (int i = 6; i != 0; --i) {
        float d = (pl[2] * sphere[2] + pl[0] * sphere[0] + pl[1] * sphere[1]) - pl[3];
        if (sphere[3] < d) return 0;            // fully outside this plane
        if (-sphere[3] < d) result = 1;          // straddles -> intersecting
        pl += 5;
    }
    return result;
}

// ---- deferred RW deps (bound when the RW atomic/device structs are mapped) ----
// RpAtomicGetWorldBoundingSphere @ (RW-recognized symbol) — returns the atomic's
// world bounding sphere [cx,cy,cz,r]. [UNCERTAIN exact atomic offset — bind at E2.]
static float s_zeroSphere[4] = {0, 0, 0, 0};
static float* RpAtomicGetWorldBoundingSphere(void* /*atomic*/)
{
    // TODO(E2): real impl reads the atomic's world bounding sphere. Inert here.
    return s_zeroSphere;
}
// FUN_004e1710 / FUN_004e2030 — RxPipeline heap/cleanup helpers (deferred no-ops).
static void Fn_004e1710(void* /*heapMgr*/) {}              // TODO(E2/B-full)
static void Fn_004e2030(void* /*arg*/)     {}              // TODO(E2/B-full)

// RxPipeline exec context globals (DAT_00911ac8/ac0/ad0/ad4). File-static mirror.
static int   s_pipeExecActive = 0;   // DAT_00911ac8
static void* s_pipeExecPipe   = nullptr; // _DAT_00911ac0
static void* s_pipeExecObj    = nullptr; // _DAT_00911ad0
static void* s_pipeExecHeap   = nullptr; // _DAT_00911ad4
// DAT_007d4710 render heap/cluster mgr; DAT_00911ae0 default-pipeline base. Null until bound.
static void* g_rwRenderHeap   = nullptr; // DAT_007d4710
static unsigned char* g_defPipeBase = nullptr; // DAT_00911ae0

// ===========================================================================
// 0x004d40d0  RxPipelineExecute(pipeline, obj, flush)
// Verbatim of FUN_004d40d0 (body 0x004d40d0..0x004d416e). The node-graph execution
// `node->vtable[1](node, &io)` (pipeline+8 -> first node) is the RxD3D9 pipeline =
// E2/B-full work; GUARDED here (null node -> returns 0) until the node graph is ported.
// ===========================================================================
unsigned RxPipelineExecute(void* pipeline, void* obj, int flush)
{
    if (!pipeline) return 0;
    if (flush != 0 && g_rwRenderHeap && I(g_rwRenderHeap, 0x18) != 0)
        Fn_004e1710(g_rwRenderHeap);                       // FUN_004e1710
    s_pipeExecActive = 1;
    s_pipeExecPipe   = pipeline;
    s_pipeExecObj    = obj;
    s_pipeExecHeap   = g_rwRenderHeap;
    I(pipeline, 0x10) = 0;                                 // pipeline result-state
    void* node = PP(pipeline, 8);                          // pipeline+8 = first node
    int ok = 0;
    if (node && PP(node, 0)) {                             // node vtable present?
        // node->vtable[1](node, &io)  — RxD3D9 node graph (DEFERRED E2/B-full).
        typedef int (*NodeExecFn)(void*, void*);
        NodeExecFn exec = reinterpret_cast<NodeExecFn>(PP(PP(node, 0), 4));
        void* io[2] = { obj, s_pipeExecHeap };             // &DAT_00911ad0 layout
        ok = exec ? exec(node, io) : 0;
    }
    if (ok == 0) s_pipeExecActive = 0;
    if (1 < I(pipeline, 0x10)) {
        I(pipeline, 0x10) = 2;
        Fn_004e2030(PP(pipeline, 0x14));                   // FUN_004e2030
    }
    s_pipeExecPipe = nullptr; s_pipeExecObj = nullptr; s_pipeExecHeap = nullptr;
    return s_pipeExecActive ? reinterpret_cast<unsigned>(pipeline) : 0u;
}

// ===========================================================================
// 0x004e5f90  RpAtomicDefaultRenderCallback(atomic)
// atomic+0x6c = atomic's own pipeline; if null, the engine default object pipeline
// at *(DAT_00911ae0 + 0x3c + RwGlobals). Verbatim of FUN_004e5f90.
// ===========================================================================
unsigned RpAtomicDefaultRenderCallback(void* atomic)
{
    void* pipe = PP(atomic, 0x6c);
    if (!pipe && g_defPipeBase && g_rwGlobals)
        pipe = *reinterpret_cast<void**>(g_defPipeBase + 0x3c + reinterpret_cast<intptr_t>(g_rwGlobals));
    unsigned r = RxPipelineExecute(pipe, atomic, 1);
    return (r != 0) ? reinterpret_cast<unsigned>(atomic) : 0u;
}

// ===========================================================================
// 0x004e5190  RwWorldPipelineRender(world) — the world's +0x68 render callback
// (installed by FUN_004e5820 = RwWorldLoad's Rwl_WorldSetRenderCB). Resolves the
// world pipeline (world+0x7c override, else the world-TYPE default
// (*(RwGlobals+4))+0x6c, else engine default WORLD pipe plugin+0x40) and executes it.
// Verbatim of FUN_004e5190 (0x004e5190..0x004e51d7).
// ===========================================================================
unsigned RwWorldPipelineRender(void* world)
{
    if (S(world, 0x84) == 0) return reinterpret_cast<unsigned>(world);  // empty-world guard
    void* pipe = PP(world, 0x7c);                                       // world's own pipeline override
    if (!pipe) {
        if (g_rwGlobals) {
            void* o = PP(g_rwGlobals, 4);                              // world-TYPE object O (§1.2)
            if (o) pipe = PP(o, 0x6c);
        }
        if (!pipe && g_defPipeBase && g_rwGlobals)                    // engine default WORLD pipeline
            pipe = *reinterpret_cast<void**>(g_defPipeBase + 0x40 + reinterpret_cast<intptr_t>(g_rwGlobals));
    }
    unsigned r = RxPipelineExecute(pipe, world, 1);
    return (r != 0) ? reinterpret_cast<unsigned>(world) : 0u;
}

// ===========================================================================
// 0x004f0900  RwWorldSectorCull(sector) — indirect device dispatch
// `(**(code**)(*(RwGlobals+4) + 0x68))()` — a RW device-table call (the sector
// visibility/frustum predicate). The device table is the RW D3D9 driver (B-full,
// DEFERRED). GUARDED: with no device bound, default to "render this sector" (1).
// Verbatim shape of FUN_004f0900 (0x004f0900).
// ===========================================================================
int RwWorldSectorCull(void* /*sector*/)
{
    if (!g_rwGlobals) return 1;                            // inert -> render (no cull)
    void* dev = PP(g_rwGlobals, 4);                        // RwGlobals+4
    if (!dev) return 1;
    typedef int (*CullFn)();
    CullFn fn = reinterpret_cast<CullFn>(PP(dev, 0x68));   // device+0x68
    return fn ? fn() : 1;                                  // TODO(B-full): real device cull
}

// ===========================================================================
// 0x004e5680  RwWorldSectorRender(sector) — the per-sector render callback
// Verbatim of FUN_004e5680 (0x004e5680..0x004e56f5):
//   if cull(sector): for atomic in sector+0x38 intrusive list:
//     atomic = link[2]; if (atomic+2 & 4) && (atomic+0x60 != RwGlobals+2):
//        sphere = RpAtomicGetWorldBoundingSphere(atomic);
//        if RwCameraFrustumTestSphere(*RwGlobals, sphere): atomic->render(+0x48)(atomic);
//        stamp atomic+0x60 = RwGlobals+2.
// ===========================================================================
int RwWorldSectorRender(void* sector)
{
    if (RwWorldSectorCull(sector) != 0) {
        void* head = P(sector, 0x38);                      // sector+0x38 list head
        for (void** link = reinterpret_cast<void**>(PP(sector, 0x38));
             link != reinterpret_cast<void**>(head);
             link = reinterpret_cast<void**>(*link)) {      // link[0] = next
            void* atomic = link[2];                         // link+8 = atomic
            short curStamp = g_rwGlobals ? S(g_rwGlobals, 2) : 0; // RwGlobals+2 frame stamp
            if ((B(atomic, 2) & 4) != 0 && S(atomic, 0x60) != curStamp) {
                float* sphere = RpAtomicGetWorldBoundingSphere(atomic);
                void* cam = g_rwGlobals ? PP(g_rwGlobals, 0) : nullptr; // *RwGlobals
                int vis = cam ? RwCameraFrustumTestSphere(cam, sphere) : 1;
                if (vis != 0) {
                    // atomic->render(atomic)  (atomic+0x48) — RpAtomic default = RpAtomicDefaultRenderCallback.
                    typedef void (*RenderCB)(void*);
                    RenderCB cb = reinterpret_cast<RenderCB>(PP(atomic, 0x48));
                    if (cb) cb(atomic);
                }
                S(atomic, 0x60) = curStamp;                 // stamp current frame
            }
        }
    }
    return reinterpret_cast<int>(sector);
}

// ===========================================================================
// 0x004e47b0  RwForAllSectors(listOff, cb, ud) — walk the world sector list
// list head ptr at (objExtBase + listOff + 0); count at +8; cb(*element, ud) until cb==0.
// Verbatim of FUN_004e47b0. Returns sectors traversed.
// ===========================================================================
int RwForAllSectors(int listOff, int (*cb)(void*), void* /*ud*/)
{
    if (!g_objExtBase) return 0;
    void** elem = reinterpret_cast<void**>(PP(g_objExtBase, listOff + 0));
    int count = I(g_objExtBase, listOff + 8);
    int traversed = 0;
    while (count != 0) {
        if (cb(*elem) == 0) break;
        ++elem; --count; ++traversed;
    }
    return traversed;
}

// ===========================================================================
// 0x004e5660  RwWorldRenderDispatch — RwEngineForAllPlugins(ud): ForAllSectors(*RwGlobals, sectorCB, ud)
// Verbatim of RwEngineForAllPlugins (0x004e5660) [FidDB mis-name; mechanically the
// world render dispatcher]. *RwGlobals (first field) = device object-list base = sector listOff.
// ===========================================================================
int RwWorldRenderDispatch(void* ud)
{
    if (!g_rwGlobals) return 0;
    int listOff = reinterpret_cast<int>(PP(g_rwGlobals, 0));  // *DAT_007d3ff8
    return RwForAllSectors(listOff, RwWorldSectorRender, ud);
}

// ---- public API -------------------------------------------------------------
void RwWorldRender_SetEngine(void* globals, void* objExtBase)
{
    g_rwGlobals  = reinterpret_cast<unsigned char*>(globals);
    g_objExtBase = reinterpret_cast<unsigned char*>(objExtBase);
}

bool RwWorldRender_Enabled()
{
    if (!s_toggleChecked) {
        const char* e = std::getenv("MASHED_RW_RENDER");
        s_toggleOn = (e && e[0] && e[0] != '0');
        s_toggleChecked = true;
    }
    // ON requires the toggle AND a bound RW engine substrate (else the spike wins).
    return s_toggleOn && g_rwGlobals != nullptr && g_objExtBase != nullptr;
}

int RwWorldRender_Render(void* world, void* /*cam*/)
{
    if (!RwWorldRender_Enabled()) {
        static bool warned = false;
        if (std::getenv("MASHED_RW_RENDER") && !warned) {
            std::fprintf(stderr,
                "[RwWorldRender] MASHED_RW_RENDER set but RW engine/world not bound "
                "(RwEngineOpen + BSP->RpWorld loader pending E2/B-full) — keeping spike.\n");
            warned = true;
        }
        return 0;
    }
    // Shipping dispatch shape: FUN_004270f0 -> FUN_004844a0 -> RwEngineForAllPlugins(world).
    return RwWorldRenderDispatch(world);
}

} // namespace D3d9Render
} // namespace mashed_re
