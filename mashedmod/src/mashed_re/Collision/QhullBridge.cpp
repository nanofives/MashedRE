// Mashed RE — B5b: RwpQHullWrapper.c — the ONLY door between game code and the
// vendored qhull-2002.1 island (OU#3 RESOLVED: sole caller FUN_00481e00).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-14). Verbatim transcription of FUN_0057ca30 +
// FUN_00563840 (slab alloc) + FUN_0057c670 (qhull-result packaging).
// Source map: re/analysis/B5b_RWP37_QHULL_VENDOR_2026-07-14.md.
//
// qhull is vendored (mashedmod/deps/qhull-2002.1, REALfloat=1 -> coordT=float). The
// FUN_ callees that live inside the island [0x0057c5b0,0x005a5910) are the qhull
// library (mapped to qh_* symbols below); the ones below that band are game/RWP code
// ported here. qhull struct field offsets (facet->normal +0xc, ->next +0x1c,
// ->ridges +0x24, ->visitid +0x34; vertex->point +8, ->visitid +0x10) are accessed
// raw, exactly as the decomp — they hold iff REALfloat==1 (see user.h edit).
#include "CollisionBuildDeps.h"
#include <cstdlib>
#include <cstring>

extern "C" {
#include "qhull_a.h"   // vendored qhull-2002.1 (qh_qh, qh_new_qhull, qh_setsize, ...)
}

namespace mashed_re {
namespace Collision {

// default qhull command "qhull s Pp" @ 0x0062406c
static const char* kQhullCmd = "qhull s Pp";

// The RWP slab allocator is a vtable call (**(DAT_007d3ff8+0x108))(size, 0x30900).
// Hookable so the B5b self-test can supply malloc while the real RWP allocator is a
// B5c port target. Default = malloc.
void* (*g_collSlabAlloc)(unsigned size, unsigned tag) = 0;
static void* DefaultSlabAlloc(unsigned size, unsigned /*tag*/) { return std::malloc(size); }

// 0x00563940 RWP finalize (extern; B5c). Declared here (game-side, below the island).
extern "C" void FUN_00563940(void* slab);

// ---------------------------------------------------------------------------
// 0x00563840 — allocate the packed collision-hull slab.
//   header (uint[]): [0]=totalSize [1]=verts [2]=vtxLoopTbl [3]=normals
//                    [4]=facetStart(short) [5]=edges [6]=adjacency
//                    +0x1c(short)=numVerts +0x1e(short)=numFacets +0x20(short)=numEdges
//                    [0x21]=1 [0x22]=0 ; data begins at +0x8c (puVar3+0x23)
//   void FUN_00563840(int param_1 /*numVerts*/, int param_2 /*numEdges*/, int param_3 /*numFacets*/)
// (returns the slab pointer in EAX; the decomp mistypes the return as void.)
// ---------------------------------------------------------------------------
void* CollHull_Alloc(int numVerts, int numEdges, int numFacets)
{
    unsigned uVar6 = (numVerts * 0xc + 0x9b) & 0xfff0;
    unsigned uVar7 = (uVar6 + 0x11 + numVerts * 2) & 0xfff0;
    unsigned uVar4 = (uVar7 + 1 + numFacets * 0xc) & 0xfffe;
    unsigned uVar5 = (uVar4 + 5 + numFacets * 2) & 0xfffc;
    unsigned uVar1 = (uVar5 + 1 + numEdges * 0xc) & 0xfffe;
    unsigned uVar2 = (uVar1 + numEdges * 2) & 0xffff;

    void* (*alloc)(unsigned, unsigned) = g_collSlabAlloc ? g_collSlabAlloc : DefaultSlabAlloc;
    unsigned* puVar3 = (unsigned*)alloc(uVar2, 0x30900);
    if (!puVar3) return 0;

    puVar3[1] = (unsigned)(puVar3 + 0x23);           // data base (+0x8c)
    puVar3[0] = uVar2;                               // total size
    puVar3[3] = uVar7 + (unsigned)puVar3;            // normals
    puVar3[2] = uVar6 + (unsigned)puVar3;            // vtx-loop table
    puVar3[6] = uVar1 + (unsigned)puVar3;            // adjacency
    *(short*)(puVar3 + 7) = (short)numVerts;         // +0x1c numVerts
    puVar3[5] = uVar5 + (unsigned)puVar3;            // edges
    puVar3[4] = uVar4 + (unsigned)puVar3;            // facet-start shorts
    *(short*)(puVar3 + 8) = (short)numEdges;         // +0x20 numEdges
    *(short*)((int)puVar3 + 0x1e) = (short)numFacets;// +0x1e numFacets
    puVar3[0x21] = 1;
    puVar3[0x22] = 0;
    return puVar3;
}

// ---------------------------------------------------------------------------
// 0x0057c670 — pack the qhull result (qh vertex_list / facet_list / ridges) into the
// slab. Verbatim; raw offsets are 1:1 with the decomp. qh_qh globals:
//   DAT_0091459c = qh_qh.facet_list ; DAT_009145c4 = qh_qh.vertex_list
//   FUN_00585ee0 = qh_makeridges ; FUN_005834a0 = qh_setsize ;
//   FUN_00581170 = qh_nextridge3d [VERIFY at link] ; FUN_00563940 = RWP finalize.
//   int FUN_0057c670(int param_1 /*slab*/)
// ---------------------------------------------------------------------------
void* CollHull_Pack(void* slabp)
{
    int param_1 = (int)slabp;
    if (param_1 == 0 || *(int*)(param_1 + 0xc) == 0 || *(int*)(param_1 + 4) == 0 ||
        *(int*)(param_1 + 0x14) == 0 || *(int*)(param_1 + 0x18) == 0)
        return 0;

    // --- copy vertex coords: qh vertex_list -> slab[+4], store vertex index at vertex+0x10 ---
    unsigned uVar14 = 0;
    for (int* piVar11 = (int*)qh_qh.vertex_list;                 // DAT_009145c4
         piVar11 != 0 && *piVar11 != 0;                          // stop at vertex_tail
         piVar11 = (int*)*piVar11) {                             // vertex->next (+0)
        unsigned uVar10 = uVar14 & 0xffff;
        uVar14 = uVar14 + 1;
        int* dst = (int*)(*(int*)(param_1 + 4) + uVar10 * 0xc);
        int* src = (int*)piVar11[2];                             // vertex->point (+8)
        dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
        piVar11[4] = uVar10;                                     // vertex->visitid (+0x10) = index
    }

    // --- copy facet normals: qh facet_list -> slab[+0xc], store facet index at facet+0x34 ---
    uVar14 = 0;
    for (int iVar5 = (int)qh_qh.facet_list;                      // DAT_0091459c
         iVar5 != 0 && *(int*)(iVar5 + 0x1c) != 0;               // facet->next (+0x1c); stop at tail
         iVar5 = *(int*)(iVar5 + 0x1c)) {
        unsigned uVar10 = uVar14 & 0xffff;
        uVar14 = uVar14 + 1;
        int* dst = (int*)(*(int*)(param_1 + 0xc) + uVar10 * 0xc);
        int* src = *(int**)(iVar5 + 0xc);                       // facet->normal (+0xc)
        dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
        *(unsigned*)(iVar5 + 0x34) = uVar10;                    // facet->visitid (+0x34) = index
    }

    // --- build the edge table: per facet make ridges, walk them, emit edges ---
    uVar14 = 0;
    short sVar17 = 0;
    for (int iVar5 = (int)qh_qh.facet_list;
         iVar5 != 0 && *(int*)(iVar5 + 0x1c) != 0;
         iVar5 = *(int*)(iVar5 + 0x1c)) {
        qh_makeridges((facetT*)iVar5);                          // FUN_00585ee0
        *(short*)(*(int*)(param_1 + 0x10) + (uVar14 & 0xffff) * 2) = sVar17;  // facet-start
        int iVar12 = *(int*)(iVar5 + 0x24);                    // facet->ridges (+0x24)
        if (iVar12 != 0) {                                      // clear ridge->seen bit (bit24 of +0xc)
            int* piVar11 = (int*)(iVar12 + 4);
            iVar12 = *(int*)(iVar12 + 4);
            while (iVar12 != 0) {
                piVar11 = piVar11 + 1;
                *(unsigned*)(iVar12 + 0xc) = *(unsigned*)(iVar12 + 0xc) & 0xfeffffff;
                iVar12 = *piVar11;
            }
        }
        short sVar7 = (short)qh_setsize(*(setT**)(iVar5 + 0x24));    // FUN_005834a0(facet->ridges)
        unsigned short uVar8 = sVar17 + sVar7;
        for (int* piVar11 = *(int**)(*(int*)(iVar5 + 0x24) + 4);
             piVar11 != 0 && (*(unsigned char*)((int)piVar11 + 0xf) & 1) == 0;
             piVar11 = (int*)qh_nextridge3d((ridgeT*)piVar11, (facetT*)iVar5, 0)) {  // FUN_00581170
            uVar8 = uVar8 - 1;
            piVar11[3] = piVar11[3] | 0x1000000;               // ridge->seen (bit24)
            unsigned short uVar2 = *(unsigned short*)(*(int*)(*piVar11 + 4) + 0x10); // v0 idx
            unsigned short uVar3 = *(unsigned short*)(*(int*)(*piVar11 + 8) + 0x10); // v1 idx
            int e = (unsigned)uVar8 * 0xc;
            int edges = *(int*)(param_1 + 0x14);
            if (piVar11[2] == iVar5) {                          // ridge->bottom (+8) == facet
                *(unsigned short*)(e + 8 + edges)  = *(unsigned short*)(piVar11[1] + 0x34);
                *(unsigned short*)(e + 10 + edges) = *(unsigned short*)(piVar11[2] + 0x34);
                *(unsigned short*)(e + 4 + edges)  = uVar2;
                *(unsigned short*)(e + 6 + edges)  = uVar3;
            } else {
                *(unsigned short*)(e + 8 + edges)  = *(unsigned short*)(piVar11[2] + 0x34);
                *(unsigned short*)(e + 10 + edges) = *(unsigned short*)(piVar11[1] + 0x34);
                *(unsigned short*)(e + 4 + edges)  = uVar3;
                *(unsigned short*)(e + 6 + edges)  = uVar2;
            }
        }
        uVar14 = uVar14 + 1;
        sVar17 = uVar8 + sVar7;
    }
    unsigned short uVar9 = 0;
    *(short*)(*(int*)(param_1 + 0x10) + (uVar14 & 0xffff) * 2) = sVar17;

    // --- build the per-vertex adjacency loop table (slab[+8] indices, slab[+0x18] loop) ---
    unsigned short uVar8 = 0;
    unsigned short uVar6 = 0;
    if (*(short*)(param_1 + 0x1c) != 0) {                       // numVerts
        do {
            unsigned uVar14b = (unsigned)uVar6;
            *(unsigned short*)(*(int*)(param_1 + 8) + uVar14b * 2) = uVar8;
            uVar9 = 0;
            if (*(unsigned short*)(param_1 + 0x20) != 0) {      // numEdges
                do {
                    if (uVar14b == *(unsigned short*)(*(int*)(param_1 + 0x14) + 4 + (unsigned)uVar9 * 0xc))
                        goto LAB_0057c90a;
                    uVar9 = uVar9 + 1;
                } while (uVar9 < *(unsigned short*)(param_1 + 0x20));
            }
            uVar9 = *(unsigned short*)(param_1 + 0x20);
LAB_0057c90a:
            uVar9 = *(unsigned short*)(*(int*)(param_1 + 0x14) + 8 + (unsigned)uVar9 * 0xc);
            unsigned uVar10 = (unsigned)uVar9;
            unsigned short uVar16, uVar13, uVar18;   // hoisted (function-level in decomp)
            do {
                uVar16 = *(unsigned short*)(param_1 + 0x20);
                uVar13 = 0;
                if ((unsigned short)uVar10 < *(unsigned short*)(param_1 + 0x1e)) {  // numFacets
                    uVar13 = *(unsigned short*)(*(int*)(param_1 + 0x10) + uVar10 * 2);
                    uVar16 = *(unsigned short*)(*(int*)(param_1 + 0x10) + 2 + uVar10 * 2);
                }
                if (uVar13 < uVar16) {
                    do {
                        if (uVar14b == *(unsigned short*)(*(int*)(param_1 + 0x14) + 6 + (unsigned)uVar13 * 0xc))
                            goto LAB_0057c976;
                        uVar13 = uVar13 + 1;
                    } while (uVar13 < uVar16);
                }
                uVar13 = *(unsigned short*)(param_1 + 0x20);
LAB_0057c976:
                uVar16 = *(unsigned short*)(param_1 + 0x20);
                uVar18 = 0;
                uVar13 = *(unsigned short*)(*(int*)(param_1 + 0x14) + 8 + (unsigned)uVar13 * 0xc);
                uVar10 = (unsigned)uVar13;
                if (uVar13 < *(unsigned short*)(param_1 + 0x1e)) {
                    uVar18 = *(unsigned short*)(*(int*)(param_1 + 0x10) + uVar10 * 2);
                    uVar16 = *(unsigned short*)(*(int*)(param_1 + 0x10) + 2 + uVar10 * 2);
                }
                for (; uVar18 < uVar16; uVar18 = uVar18 + 1) {
                    if (uVar14b == *(unsigned short*)(*(int*)(param_1 + 0x14) + 4 + (unsigned)uVar18 * 0xc))
                        goto LAB_0057c9cc;
                }
                uVar18 = *(unsigned short*)(param_1 + 0x20);
LAB_0057c9cc:
                unsigned uVar15 = (unsigned)uVar8;
                uVar8 = uVar8 + 1;
                *(unsigned short*)(*(int*)(param_1 + 0x18) + uVar15 * 2) = uVar18;
            } while (uVar13 != uVar9);
            uVar9 = uVar6 + 1;
            uVar6 = uVar9;
        } while (uVar9 < *(unsigned short*)(param_1 + 0x1c));
    }
    *(unsigned short*)(*(int*)(param_1 + 8) + (unsigned)uVar9 * 2) = uVar8;
    FUN_00563940(slabp);                                       // RWP finalize
    return slabp;
}

// ---------------------------------------------------------------------------
// 0x0057ca30 — the qhull bridge.
//   undefined4 FUN_0057ca30(undefined4 param_1 /*numpoints*/, int param_2 /*points*/,
//                           char *param_3 /*opt*/, undefined4 param_4 /*file*/)
// ---------------------------------------------------------------------------
void* QhullBuildHull(int numpoints, float* points, const char* opt, void* file)
{
    if (points == 0) return 0;                                 // guard on param_2
    const char* cmd = opt ? opt : kQhullCmd;                   // default "qhull s Pp"

    int rc = qh_new_qhull(3, numpoints, points, False,         // FUN_0058f520
                          const_cast<char*>(cmd), (FILE*)file, (FILE*)file);
    void* result = 0;
    if (rc == 0) {                                             // low-dword == 0 => success
        int total = 0;
        facetT* facet;
        FORALLfacets                                           // for(facet=qh facet_list; facet && facet->next; ...)
            total += qh_setsize(facet->neighbors);            // FUN_005834a0(facet+0x28 = neighbors)
        void* slab = CollHull_Alloc(qh num_vertices, total, qh num_facets); // FUN_00563840(d4,total,d0)
        if (slab)
            result = CollHull_Pack(slab);                     // FUN_0057c670
    }
    qh_freeqhull(!qh_ALL);                                     // FUN_00589dc0(0)
    { int curlong, totlong; qh_memfreeshort(&curlong, &totlong); }  // FUN_0058f0a0
    return result;
}

} // namespace Collision
} // namespace mashed_re
