// Mashed RE — B5b acceptance harness (offline replay side).
//
// Links the vendored qhull-2002.1 (REALfloat=1) + the ported RwpQHullWrapper bridge
// (QhullBridge.cpp) and runs a point cloud through QhullBuildHull, dumping the packed
// collision-hull slab. Two modes:
//   (default)         synthetic cloud  -> proves qhull + bridge run end-to-end.
//   --replay <file>   a Frida-captured original cloud (capture_qhull_hull.py) -> emit
//                     the standalone slab for a bit-for-bit diff vs the captured original.
//
// Build: build_b5b_selftest.bat  (x87 /arch:IA32, links qhull_2002_1.lib).
#include "CollisionBuildDeps.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <float.h>   // _controlfp — match the game's x87 precision-control word

extern "C" {
#include "qhull_a.h"   // for --diag (inspect qhull's computed merge/precision globals)
}

using namespace mashed_re::Collision;

// --diag: run qh_new_qhull directly and print the merge/precision config qhull computed.
static int diag_cloud(int n, float* pts) {
    const char* opt = std::getenv("MASHED_QHULL_OPT");
    if (!opt) opt = "qhull s Pp";
    printf("[diag] n=%d opt=\"%s\"\n", n, opt);
    int rc = qh_new_qhull(3, n, pts, False, const_cast<char*>(opt), 0, 0);
    printf("  rc=%d  nFacets=%d nVertices=%d\n", rc, qh num_facets, qh num_vertices);
    printf("  MERGING=%d MERGEexact=%d  DISTround=%.9g  MAXcoplanar=%.9g  MINoutside=%.9g\n",
           qh MERGING, qh MERGEexact, qh DISTround, qh MAXcoplanar, qh MINoutside);
    printf("  premerge_centrum=%.9g postmerge_centrum=%.9g  premerge_cos=%.9g postmerge_cos=%.9g cos_max=%.9g\n",
           qh premerge_centrum, qh postmerge_centrum, qh premerge_cos, qh postmerge_cos, qh cos_max);
    printf("  MAXabs_coord=%.9g  MAXwidth=%.9g  min_vertex=%.9g  hull_dim=%d  REALepsilon=%.9g\n",
           qh MAXabs_coord, qh MAXwidth, qh min_vertex, qh hull_dim, (double)REALepsilon);
    // raw max |coord| of the input, for comparison to qh maxmaxcoord
    { float mx = 0; for (int i=0;i<n*3;++i){ float a = pts[i]<0?-pts[i]:pts[i]; if (a>mx) mx=a; }
      printf("  raw max|coord|=%.9g\n", (double)mx); }
    // qhT field offsets relative to facet_list (whose original VA is 0x0091459c). Lets a
    // Frida read of the ORIGINAL's qh_qh compare DISTround/premerge/MERGING/maxmaxcoord.
    char* fl = (char*)&qh facet_list;
    printf("  [offsets vs facet_list @orig 0x0091459c]  DISTround %+d  premerge_centrum %+d"
           "  postmerge_centrum %+d  MERGING %+d  MAXcoplanar %+d  MINoutside %+d\n",
           (int)((char*)&qh DISTround - fl), (int)((char*)&qh premerge_centrum - fl),
           (int)((char*)&qh postmerge_centrum - fl), (int)((char*)&qh MERGING - fl),
           (int)((char*)&qh MAXcoplanar - fl), (int)((char*)&qh MINoutside - fl));
    qh_freeqhull(!qh_ALL); { int a,b; qh_memfreeshort(&a,&b); }
    return rc;
}

// ---- externs QhullBridge needs that are NOT qhull (RWP, un-ported) ----
extern "C" void FUN_00563940(void* /*slab*/) { /* RWP finalize — no-op for the harness */ }

// The packed slab header (from FUN_00563840): [0]=size [1]=verts [2]=vtxLoop [3]=normals
// [4]=facetStart [5]=edges [6]=adjacency; +0x1c(u16)=nVerts +0x1e(u16)=nFacets +0x20(u16)=nEdges.
static void dump_slab(const void* slabp) {
    if (!slabp) { printf("  slab = NULL (qhull failed or empty hull)\n"); return; }
    const unsigned* h = (const unsigned*)slabp;
    unsigned size    = h[0];
    unsigned short nV = *(const unsigned short*)((const char*)slabp + 0x1c);
    unsigned short nF = *(const unsigned short*)((const char*)slabp + 0x1e);
    unsigned short nE = *(const unsigned short*)((const char*)slabp + 0x20);
    printf("  slab size=%u  nVerts=%u  nFacets=%u  nEdges=%u\n", size, nV, nF, nE);
    const float* verts   = *(float* const*)((const char*)slabp + 0x04);
    const float* normals = *(float* const*)((const char*)slabp + 0x0c);
    printf("  first verts:");
    for (unsigned i = 0; i < (nV < 4 ? nV : 4); ++i)
        printf(" (%.6f,%.6f,%.6f)", verts[i*3], verts[i*3+1], verts[i*3+2]);
    printf("\n  first normals:");
    for (unsigned i = 0; i < (nF < 4 ? nF : 4); ++i)
        printf(" (%.6f,%.6f,%.6f)", normals[i*3], normals[i*3+1], normals[i*3+2]);
    printf("\n");
}

// crc32 of the whole slab (for a quick equality check vs a captured original).
static unsigned crc32(const unsigned char* p, unsigned n) {
    unsigned c = 0xffffffffu;
    for (unsigned i = 0; i < n; ++i) {
        c ^= p[i];
        for (int k = 0; k < 8; ++k) c = (c >> 1) ^ (0xedb88320u & (unsigned)-(int)(c & 1));
    }
    return ~c;
}

static void run_cloud(int n, float* cloud, const char* label) {
    printf("[%s] %d points\n", label, n);
    void* slab = QhullBuildHull(n, cloud, 0, 0);
    dump_slab(slab);
    if (slab) {
        unsigned size = ((unsigned*)slab)[0];
        printf("  slab crc32 = 0x%08x  (size=%u)\n", crc32((unsigned char*)slab, size), size);
    }
    std::free(slab);
}

int main(int argc, char** argv) {
    // MASHED/RenderWare runs the x87 FPU in SINGLE-precision (24-bit) rounding; qhull's
    // coplanar-merge decisions are knife-edge sensitive to intermediate rounding, so match
    // it. Toggle with MASHED_QHULL_PC={24,53,64}; default 24 (=_PC_24).
    {
        const char* pc = std::getenv("MASHED_QHULL_PC");
        unsigned mode = _PC_24;
        if (pc) { if (!std::strcmp(pc,"53")) mode=_PC_53; else if (!std::strcmp(pc,"64")) mode=_PC_64; }
        _controlfp(mode, _MCW_PC);
    }
    if (argc >= 3 && std::strcmp(argv[1], "--diag") == 0) {
        FILE* f = std::fopen(argv[2], "rb");
        if (!f) { printf("cannot open %s\n", argv[2]); return 2; }
        int n = 0; std::fread(&n, 4, 1, f);
        std::vector<float> cloud((size_t)n * 3);
        std::fread(cloud.data(), 4, (size_t)n * 3, f);
        std::fclose(f);
        return diag_cloud(n, cloud.data());
    }
    if (argc >= 3 && std::strcmp(argv[1], "--replay") == 0) {
        // captured-cloud file: [int32 n][n*3 float32].  Optional argv[3] = original slab
        // to diff against (bit-identity C4 acceptance).
        FILE* f = std::fopen(argv[2], "rb");
        if (!f) { printf("cannot open %s\n", argv[2]); return 2; }
        int n = 0; std::fread(&n, 4, 1, f);
        std::vector<float> cloud((size_t)n * 3);
        std::fread(cloud.data(), 4, (size_t)n * 3, f);
        std::fclose(f);
        printf("[replay %s] %d points\n", argv[2], n);
        void* slab = QhullBuildHull(n, cloud.data(), 0, 0);
        dump_slab(slab);
        if (argc >= 4 && slab) {
            // load original slab
            FILE* g = std::fopen(argv[3], "rb");
            if (!g) { printf("cannot open %s\n", argv[3]); std::free(slab); return 2; }
            std::fseek(g, 0, SEEK_END); long osz = std::ftell(g); std::fseek(g, 0, SEEK_SET);
            std::vector<unsigned char> orig((size_t)osz);
            std::fread(orig.data(), 1, (size_t)osz, g); std::fclose(g);
            unsigned char* mine = (unsigned char*)slab;
            unsigned msz = ((unsigned*)slab)[0];
            printf("  DIFF vs %s (orig %ld bytes, mine %u bytes)\n", argv[3], osz, msz);
            bool ok = (msz == (unsigned)osz);
            if (!ok) printf("  RED: size mismatch\n");
            // Vertex sub-array is at +0x8c in both (data base). Compare it regardless of
            // overall size, to isolate whether the input+vertex-set is bit-identical.
            {
                unsigned short nvO = *(unsigned short*)(orig.data()+0x1c);
                unsigned short nvM = *(unsigned short*)(mine+0x1c);
                if (nvO == nvM) {
                    unsigned vb = nvO * 12u;
                    int vd = std::memcmp(orig.data()+0x8c, mine+0x8c, vb);
                    printf("  vertex array (%u verts, %u bytes @+0x8c): %s\n",
                           nvO, vb, vd == 0 ? "BIT-IDENTICAL" : "differ");
                }
            }
            // Compare the 3 counts + the geometry DATA region [0x8c:size]. The slab header
            // [0:0x8c] holds allocation-address pointers ([1..6]), runtime flags
            // ([0x21]/[0x22]) and an uninitialised gap (0x24..0x88) — none of which is hull
            // geometry — so it is skipped. All sub-arrays (verts/normals/edges/adjacency/
            // facet-starts) live at >= +0x8c, so a byte match there IS the bit-identity test.
            bool cok = *(unsigned short*)(orig.data()+0x1c) == *(unsigned short*)(mine+0x1c)
                    && *(unsigned short*)(orig.data()+0x1e) == *(unsigned short*)(mine+0x1e)
                    && *(unsigned short*)(orig.data()+0x20) == *(unsigned short*)(mine+0x20);
            if (ok && cok) {
                unsigned firstBad = 0xffffffff, nbad = 0;
                for (unsigned o = 0x8c; o < msz; ++o) {
                    if (mine[o] != orig[o]) { if (firstBad == 0xffffffff) firstBad = o; nbad++; }
                }
                if (nbad == 0) printf("  GREEN: bit-identical hull (counts + data region [0x8c:%u] match)\n", msz);
                else printf("  RED: %u data byte(s) differ, first at offset 0x%x\n", nbad, firstBad);
                // characterise the normals sub-array (facet plane normals): ULP-diff (FP)
                // vs reordered/different. relative offset = header[3] - slab base.
                unsigned normOff = *(unsigned*)(mine+0x0c) - (unsigned)mine;
                unsigned nF = *(unsigned short*)(mine+0x1e);
                if (normOff + nF*12 <= msz) {
                    const float* nO = (const float*)(orig.data()+normOff);
                    const float* nM = (const float*)(mine+normOff);
                    float maxd = 0; int exact = 0;
                    for (unsigned i=0;i<nF*3;++i){ float d=nO[i]-nM[i]; if(d<0)d=-d; if(d>maxd)maxd=d; if(nO[i]==nM[i])exact++; }
                    printf("  normals[%u @+0x%x]: exact=%d/%u  maxAbsDiff=%g\n", nF, normOff, exact, nF*3, (double)maxd);
                }
            } else if (ok) {
                printf("  RED: count mismatch\n");
            }
            // always report the key counts side by side
            printf("  counts orig: nV=%u nF=%u nE=%u | mine: nV=%u nF=%u nE=%u\n",
                *(unsigned short*)(orig.data()+0x1c), *(unsigned short*)(orig.data()+0x1e), *(unsigned short*)(orig.data()+0x20),
                *(unsigned short*)(mine+0x1c), *(unsigned short*)(mine+0x1e), *(unsigned short*)(mine+0x20));
        }
        std::free(slab);
        return 0;
    }
    // synthetic: ~64 points on a unit sphere (mirrors what the cone-cast produces).
    const int N = 64;
    std::vector<float> cloud((size_t)N * 3);
    for (int i = 0; i < N; ++i) {
        double phi = 3.141592653589793 * (double)i / (double)N;
        double th  = 6.283185307179586 * (double)(i * 7 % N) / (double)N;
        cloud[i*3+0] = (float)(sin(phi) * cos(th));
        cloud[i*3+1] = (float)(sin(phi) * sin(th));
        cloud[i*3+2] = (float)cos(phi);
    }
    run_cloud(N, cloud.data(), "synthetic-sphere");
    return 0;
}
