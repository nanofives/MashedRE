// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* RWC-FRAME-CLUMP 2026-11-13 0x004e7420 (RpClumpStreamRead core): rewritten from the disassembly, 52.3% under rw9044
   (regs 70.7%, ops 72.9%, 753/715 ins), was 37.9% (DDK) for the Ghidra-structure source. The old source read the
   12-byte clump header (0x004cbd30(stream, buf, 0xc) at 0x004e7493, buffer [esp+0x48] = frame+0x38) into one
   4-byte local and passed a 4-byte local as the geometry list, which overran the port's frame: the AV at EIP 1 in the
   cup.dff load (RESULT_RWC-FRAME-492E90.md section 5, item 3). This form has the original's 0x44 frame: version
   +4, frame-chunk version +8, size +0xc, geometry header size +0x10 / version +0, geometry list +0x18 (8 B), frame
   list +0x20 (8 B), header +0x38 (12 B), the geometry count and the light / camera frame index in the stream
   parameter home (+0x48, 0x004e76f0 / 0x004e798d), one RwError per error site. The geometry list read is an inlined
   helper (rw9044 inlines it and reuses the parameter home as the original does); the geometry list free is inlined at
   0x004e786d / 0x004e7a15 / 0x004e7a41 / 0x004e7bb2 / 0x004e7b1c and called at 0x004e78be / 0x004e78d0 (0x004e7010).
   Residual: (1) the original holds 0 / the loop counter in ebx and the stream in edi, ours swaps them; (2) after the
   inlined read the original tests the returned pointer (0x004e77d2 lea eax,[esp+0x28]; test; jne 0x004e7901) and
   places the failure block first, ours folds the test and places the atomics loop first. The same source with all six
   geometry list frees inlined scores 63.2% (721 ins). Not reached by: declaration order of i / count / stream copy,
   register, == 0 forms, single-exit helper, int / bool helper returns, flag sets (/Ob2 /Ox /O1 /G6 /Op). equiv.py:
   NOT-PROVEN call-regargs (the ebx / edi swap is visible to the callees). Provisional (L-126), not C4. */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
#ifdef __cplusplus
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#else
#define MGLOBAL(T, n, a) extern T n
#endif
#endif
typedef struct { int pluginID; int errorCode; } RwError;
typedef struct { int numAtomics; int numLights; int numCameras; } ClumpInfo;
typedef struct { void **frames; int numFrames; } FrameList;
typedef struct { void **geometries; int numGeoms; } GeomList;
typedef struct LLLink { struct LLLink *next; struct LLLink *prev; } LLLink;
typedef struct { void *clump; LLLink lInClump; } ClumpExt;
typedef struct Clump {
    unsigned char type, subType, flags, privateFlags;
    void *parent;
    LLLink atomicList;
    LLLink lightList;
    LLLink cameraList;
} Clump;
typedef struct Atomic { unsigned char pad[0x3c]; Clump *clump; LLLink inClumpLink; } Atomic;
typedef void *(*FreeFn)(void *);
typedef void *(*MallocFn)(unsigned int, unsigned int);
typedef void (*FlFreeFn)(void *, void *);

MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(int, DAT_007d7230, 0x007d7230);
MGLOBAL(int, DAT_007d722c, 0x007d722c);
MGLOBAL(int, DAT_0061867c, 0x0061867c);

int FUN_004cc5e0(void *stream, int type, unsigned int *size, unsigned int *version);
unsigned int FUN_004cbd30(void *stream, void *buf, unsigned int len);
void *FUN_004cc790(void *stream, void *buf, unsigned int len);
int FUN_004d7ff0(int code, ...);
void FUN_004d8480(RwError *err);
int FUN_004d8060(void *reg, void *obj);
void *FUN_004e1b60(void *reg, void *stream, void *obj);
Clump *FUN_004e6d80(void);
void FUN_004e6920(Atomic *atomic);
void FUN_004e66d0(Clump *clump, void (*cb)(void), void *data);
void FUN_004e6760(Clump *clump, void (*cb)(void), void *data);
void FUN_004e6710(Clump *clump, void (*cb)(void), void *data);
void FUN_004c0de0(void *frame);
void FUN_004e6d00(void);
void FUN_004e6d40(void);
void FUN_004e7cc0(void);
void FUN_004e8ea0(void *geom);
void *FUN_004e9330(void *stream);
Atomic *FUN_004e7060(void *stream, FrameList *fl, GeomList *gl);
void *FUN_004e4c30(void *stream);
void *FUN_005c4670(void *stream);
void FUN_004c0740(void *object, void *frame);
FrameList *FUN_005c47e0(void *stream, FrameList *fl);
FrameList *FUN_005c47b0(FrameList *fl);
void FUN_004e6e00(Clump *clump);

#define RWERR(code) { RwError err; err.pluginID = 2; err.errorCode = FUN_004d7ff0 code; FUN_004d8480(&err); }
#define CLUMP_FREE(c) ((FlFreeFn)*(void **)(DAT_007d3ff8 + 0x11c))(*(void **)(DAT_007d3ff8 + DAT_007d7200 + 4), (c))
#define RWFREE(p) ((FreeFn)*(void **)(DAT_007d3ff8 + 0x10c))(p)
#define RWMALLOC(n, h) ((MallocFn)*(void **)(DAT_007d3ff8 + 0x108))((n), (h))

GeomList *FUN_004e7010(GeomList *gl);
static __inline GeomList *GeomListDeinit(GeomList *gl)
{
    int i;
    for (i = 0; i < gl->numGeoms; i++) FUN_004e8ea0(gl->geometries[i]);
    if (gl->geometries) { RWFREE(gl->geometries); gl->geometries = 0; }
    return gl;
}

static __inline GeomList *GeomListRead(void *stream, GeomList *gl)
{
    unsigned int size, version;
    int count, i;
    if (!FUN_004cc5e0(stream, 1, &size, &version)) return 0;
    if (version < 0x35000 || version > 0x37002) {
        RWERR((0x80000004));
        return 0;
    }
    if (FUN_004cbd30(stream, &count, 4) != 4) return 0;
    gl->numGeoms = 0;
    if (count > 0) {
        gl->geometries = (void **)RWMALLOC(count * 4, 0x3000f);
        if (!gl->geometries) {
            RWERR((0x80000013, count * 4));
            return 0;
        }
    }
    else gl->geometries = 0;
    for (i = 0; i < count; i++) {
        if (!FUN_004cc5e0(stream, 0xf, 0, &version)) {
            GeomListDeinit(gl);
            return 0;
        }
        if (version < 0x35000 || version > 0x37002) {
            FUN_004e7010(gl);
            return 0;
        }
        gl->geometries[i] = FUN_004e9330(stream);
        if (!gl->geometries[i]) {
            FUN_004e7010(gl);
            return 0;
        }
        gl->numGeoms++;
    }
    return gl;
}

// MATCH: 0x004e7420 FUN_004e7420
Clump *FUN_004e7420(void *stream)
{
    unsigned int size, version;

    if (!FUN_004cc5e0(stream, 1, &size, &version)) {
        RWERR((0x8000001a));
        return 0;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        Clump *clump;
        ClumpInfo cl;
        FrameList fl;
        GeomList gl;
        unsigned int fversion;
        int i;

        if (FUN_004cbd30(stream, &cl, sizeof(cl)) != sizeof(cl)) {
            RWERR((0x8000001a));
            return 0;
        }
        clump = FUN_004e6d80();
        if (!clump) return 0;

        if (!FUN_004cc5e0(stream, 0xe, 0, &fversion)) {
            LLLink *cur, *next;
            Atomic *atomic;
            FUN_004d8060(&DAT_0061867c, clump);
            for (cur = clump->atomicList.next; cur != &clump->atomicList; cur = next) {
                next = cur->next;
                atomic = (Atomic *)((char *)cur - 0x40);
                FUN_004e6920(atomic);
                if (!atomic) break;
            }
            FUN_004e6760(clump, FUN_004e6d00, 0);
            FUN_004e6710(clump, FUN_004e6d40, 0);
            if (clump->parent) FUN_004c0de0(clump->parent);
            CLUMP_FREE(clump);
            RWERR((0x8000001a));
            return 0;
        }
        if (!FUN_005c47e0(stream, &fl)) {
            LLLink *cur, *next;
            Atomic *atomic;
            FUN_004d8060(&DAT_0061867c, clump);
            for (cur = clump->atomicList.next; cur != &clump->atomicList; cur = next) {
                next = cur->next;
                atomic = (Atomic *)((char *)cur - 0x40);
                FUN_004e6920(atomic);
                if (!atomic) break;
            }
            FUN_004e6760(clump, FUN_004e6d00, 0);
            FUN_004e6710(clump, FUN_004e6d40, 0);
            if (clump->parent) FUN_004c0de0(clump->parent);
            CLUMP_FREE(clump);
            RWERR((0x8000001a));
            return 0;
        }
        clump->parent = fl.frames[0];
        if (!FUN_004cc5e0(stream, 0x1a, 0, &fversion)) {
            LLLink *cur, *next;
            Atomic *atomic;
            FUN_005c47b0(&fl);
            FUN_004d8060(&DAT_0061867c, clump);
            for (cur = clump->atomicList.next; cur != &clump->atomicList; cur = next) {
                next = cur->next;
                atomic = (Atomic *)((char *)cur - 0x40);
                FUN_004e6920(atomic);
                if (!atomic) break;
            }
            FUN_004e6760(clump, FUN_004e6d00, 0);
            FUN_004e6710(clump, FUN_004e6d40, 0);
            if (clump->parent) FUN_004c0de0(clump->parent);
            CLUMP_FREE(clump);
            RWERR((0x8000001a));
            return 0;
        }

        if (!GeomListRead(stream, &gl)) {
            FUN_005c47b0(&fl);
            FUN_004d8060(&DAT_0061867c, clump);
            FUN_004e66d0(clump, FUN_004e7cc0, 0);
            FUN_004e6760(clump, FUN_004e6d00, 0);
            FUN_004e6710(clump, FUN_004e6d40, 0);
            if (clump->parent) FUN_004c0de0(clump->parent);
            CLUMP_FREE(clump);
            RWERR((0x8000001a));
            return 0;
        }

        for (i = 0; i < cl.numAtomics; i++) {
            Atomic *atomic;
            if (!FUN_004cc5e0(stream, 0x14, 0, &version) ||
                !(atomic = FUN_004e7060(stream, &fl, &gl))) {
                GeomListDeinit(&gl);
                FUN_005c47b0(&fl);
                FUN_004d8060(&DAT_0061867c, clump);
                FUN_004e66d0(clump, FUN_004e7cc0, 0);
                FUN_004e6760(clump, FUN_004e6d00, 0);
                FUN_004e6710(clump, FUN_004e6d40, 0);
                if (clump->parent) FUN_004c0de0(clump->parent);
                CLUMP_FREE(clump);
                RWERR((0x8000001a));
                return 0;
            }
            atomic->inClumpLink.next = clump->atomicList.next;
            atomic->inClumpLink.prev = &clump->atomicList;
            clump->atomicList.next->prev = &atomic->inClumpLink;
            clump->atomicList.next = &atomic->inClumpLink;
            atomic->clump = clump;
        }
        for (i = 0; i < cl.numLights; i++) {
            void *light;
            int frameIndex;
            ClumpExt *ext;
            if (!FUN_004cc5e0(stream, 1, 0, 0) ||
                !FUN_004cc790(stream, &frameIndex, 4) ||
                !FUN_004cc5e0(stream, 0x12, 0, 0) ||
                !(light = FUN_004e4c30(stream))) {
                GeomListDeinit(&gl);
                FUN_005c47b0(&fl);
                FUN_004d8060(&DAT_0061867c, clump);
                FUN_004e66d0(clump, FUN_004e7cc0, 0);
                FUN_004e6760(clump, FUN_004e6d00, 0);
                FUN_004e6710(clump, FUN_004e6d40, 0);
                if (clump->parent) FUN_004c0de0(clump->parent);
                CLUMP_FREE(clump);
                RWERR((0x8000001a));
                return 0;
            }
            FUN_004c0740(light, fl.frames[frameIndex]);
            ext = (ClumpExt *)((char *)light + DAT_007d7230);
            ext->lInClump.next = clump->lightList.next;
            ext->lInClump.prev = &clump->lightList;
            clump->lightList.next->prev = &ext->lInClump;
            clump->lightList.next = &ext->lInClump;
            ext->clump = clump;
        }
        for (i = 0; i < cl.numCameras; i++) {
            void *camera;
            int frameIndex;
            ClumpExt *ext;
            if (!FUN_004cc5e0(stream, 1, 0, 0) ||
                !FUN_004cc790(stream, &frameIndex, 4) ||
                !FUN_004cc5e0(stream, 5, 0, 0) ||
                !(camera = FUN_005c4670(stream))) {
                GeomListDeinit(&gl);
                FUN_005c47b0(&fl);
                FUN_004d8060(&DAT_0061867c, clump);
                FUN_004e66d0(clump, FUN_004e7cc0, 0);
                FUN_004e6760(clump, FUN_004e6d00, 0);
                FUN_004e6710(clump, FUN_004e6d40, 0);
                if (clump->parent) FUN_004c0de0(clump->parent);
                CLUMP_FREE(clump);
                RWERR((0x8000001a));
                return 0;
            }
            FUN_004c0740(camera, fl.frames[frameIndex]);
            ext = (ClumpExt *)((char *)camera + DAT_007d722c);
            ext->lInClump.next = clump->cameraList.next;
            ext->lInClump.prev = &clump->cameraList;
            clump->cameraList.next->prev = &ext->lInClump;
            clump->cameraList.next = &ext->lInClump;
            ext->clump = clump;
        }
        GeomListDeinit(&gl);
        FUN_005c47b0(&fl);
        if (!FUN_004e1b60(&DAT_0061867c, stream, clump)) {
            FUN_004e6e00(clump);
            RWERR((0x8000001a));
            return 0;
        }
        return clump;

    }
    RWERR((0x80000004));
    return 0;
}
