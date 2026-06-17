// Mashed RE — WS-E2: BSP -> RpWorld world-stream loader (verbatim port).
//
// The world SOURCE for the WS-E1 traversal layer (RwWorldRender.cpp): builds the
// RpWorld + RpWorldSector/RpPlaneSector tree from a track's rwID_WORLD chunk so
// RwWorldRender_SetEngine() can be fed a real world. Verbatim of the §9 loader
// chain in re/analysis/render_world_path_E2_prep_2026-06-16.md:
//   FUN_004e99b0 RpWorldStreamRead   (this file: RwWorldLoad_StreamRead)
//   FUN_004e9e40 RpWorldSectorRead   (WorldSectorRead — rwID_ATOMICSECTOR 9 leaf)
//   FUN_004ea220 RpPlaneSectorRead   (PlaneSectorRead — rwID_PLANESECTOR 10 node)
//
// STATUS: PENDING diff-original C4. The RW stream/error/plugin/instance helpers it
// calls are an inert stub seam (RwWorldLoadStubs.cpp) until they are bound to real
// RW-stream code (or the standalone Rws/Txd readers) — so the loader compiles into
// the .asi but is INERT in the standalone (a stubbed FindChunk returns 0 -> the
// loader returns null; the spike world draw keeps shipping). The transcription is a
// faithful, diff-checkable map; bit-identity is verified at WS-A-VERIFY-class diff.
//
// Anchor: MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// Ghidra pool6 read-only 2026-06-16. Every RVA + constant cited inline.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace D3d9Render {

// 0x004e99b0 RpWorldStreamRead — builds the RpWorld from the rwID_WORLD chunk on
// `stream`. Returns the RpWorld handle (== the game's world-A DAT_0065742c) or null.
// `arenaBase` receives the inline sector-allocation cursor (= world + 0x70).
void* RwWorldLoad_StreamRead(void* stream);

// WS-E-DEVICE-2: parse an in-memory rwID_WORLD (GRAPH*.BSP) blob into a geometry-only
// RpWorld (materials/binMesh skipped). Now fully parses (matlist + extensions
// consumers landed). Defined in RwWorldStream.cpp.
void* RwWorldLoad_FromBytes(const void* buf, int len);

// Exercise + validate the parse: counts sector-tree leaves/verts/tris. Cross-check
// the totals vs the byte-faithful Track::World in the MAIN tree (worktrees lack data).
// Returns 1 on a valid parse, 0 if the loader bailed.
int RwWorldLoad_ParseCheck(const void* bsp, int len, int* outSectors, long* outVerts, long* outTris);

// ---- RW size/scale constants (memory_read 2026-06-16) -----------------------
namespace rwl {
constexpr unsigned kRpWorldBaseSize   = 0x70;   // DAT_0061864c (112) RpWorld header size
constexpr unsigned kRpWorldSectorSize = 0x88;   // DAT_006189e4 (136) RpWorldSector size
constexpr float    kBBoxScale         = -1.0f;  // _DAT_005cc33c
} // namespace rwl

} // namespace D3d9Render
} // namespace mashed_re
