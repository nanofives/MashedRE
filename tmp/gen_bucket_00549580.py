#!/usr/bin/env python3
"""Generate 80 C1 plates for bucket_00549580 (RpPatch+RtFS+Rt2d cluster)."""
import os, pathlib

OUT = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\re\analysis\bucket_00549580")
OUT.mkdir(parents=True, exist_ok=True)

SLOT = "Mashed_pool7"
DATE = "2026-05-19"

# Each entry: (rva_hex, size_dec, callees_list, callers_list, mech_lines, consts_rows, uncertainties, stubs)
PLATES = [
    # ============================== RpPatch plugin ==============================
    ("0x00549580", 29, ["FUN_00549aa0"], ["FUN_00549b20"], [
        "1-param `(param_1)`; treats `param_1` as a struct pointer.",
        "If `*(param_1+0x54) == 0`, calls `FUN_00549aa0(param_1)` and stores result back at `+0x54`.",
        "Returns `*(param_1+0x54)`.",
        "Lazy-init / cache pattern on field +0x54 of an RpPatchAtomic-like struct.",
    ], [
        ("0x00549580", "+0x54 (cached child handle slot)", "field offset on patch-atomic struct"),
    ], ["[UNCERTAIN] Concrete meaning of +0x54 — names as 'cached patch mesh' in RpPatch context (resolves when 0x00549aa0 is classified)."], []),

    ("0x005495a0", 8, [], ["FUN_00549580 family"], [
        "1-param `(param_1)`. Simple field-bump: `*(param_1+0x44) += 1`.",
        "Reference-count increment on RpPatchAtomic instance.",
    ], [
        ("0x005495a3", "+0x44 (refcount field)", "RpPatchAtomic ref-counter"),
    ], [], []),

    ("0x005495b0", 83, ["FUN_004e8ea0", "FUN_004f3b60", "indirect via DAT_007d3ff8+0x10c (free)"], ["FUN_00549620 (release path)"], [
        "Release/destroy on a patch-atomic struct `param_1`.",
        "Decrements `*(param_1+0x44)` (refcount). On reaching 1: zeros `+0x54` and forwards to `FUN_004e8ea0` (RW pipeline release).",
        "On reaching 0: calls `FUN_004f3b60(param_1+0x48)` (RwList destroy), then dispatches via `(*(DAT_007d3ff8+0x10c))(param_1)` (RW free).",
        "DAT_007d3ff8 = RwEngine globals table; offset +0x10c = pfnFree.",
    ], [
        ("0x005495b6", "+0x44 refcount", ""),
        ("0x005495bf", "DAT_007d3ff8+0x10c", "RwEngine free vtbl slot"),
    ], [], ["[STUB] FUN_004e8ea0 — likely RpAtomic-plugin destructor."]),

    ("0x00549610", 12, [], ["FUN_00549620 family"], [
        "2-param `(param_1, param_2)`. `*(param_1+0x40) |= param_2`. OR-flag setter on RpPatchAtomic flag word.",
    ], [
        ("0x00549613", "+0x40 (flags)", "RpPatchAtomic dirty/build flag word"),
    ], [], []),

    ("0x00549620", 30, ["FUN_00549b20"], ["external"], [
        "1-param `(param_1)`. If low-16 of `*(param_1+0x40) != 0`, calls `FUN_00549b20(param_1)` (rebuild patch mesh).",
        "Then zeros `*(param_1+0x40)`. Clears the dirty/build flag after force-flush.",
    ], [
        ("0x00549630", "0xffff mask", "low-16-bit dirty flag region"),
        ("0x00549636", "+0x40", ""),
    ], [], []),

    ("0x00549900", 112, [], ["external"], [
        "1-param `(param_1)` (pointer to 4 uints).",
        "Writes 6 floats into `(DAT_007dc720 + DAT_007d3ff8)`-relative buffer.",
        "Stores incoming 4 uints at offsets 0..3, computes `_DAT_005cc320 / ((float)param_1[3] - (float)param_1[2])` into slot[5], and `(*p - (float)p[1]) * slot[5]` into slot[4].",
        "Build of a scaled/biased reference structure for the active patch material.",
    ], [
        ("0x00549908", "DAT_007dc720", "static index into engine globals"),
        ("0x00549913", "_DAT_005cc320 (= 1.0f)", "RW unit constant"),
    ], ["[UNCERTAIN] Whether the buffer is per-frame state or per-atomic."], []),

    ("0x00549970", 127, ["FUN_00549a50", "FUN_005c9d00 x3", "FUN_004cbb60"], ["external"], [
        "Module init for the patch subsystem.",
        "Zeros DAT_007dc73c..DAT_007dc748 then re-populates DAT_007dc73c from `FUN_00549a50()` (RW pipeline registration) and DAT_007dc740/744/748 each from `FUN_005c9d00()` (RxRenderState build).",
        "Calls `FUN_004cbb60()` to fetch a context object, reads `*(ctx+0x1e) & 1` into DAT_007dc74c and `*(ctx+0xc4) & 0xffff` to compute DAT_007dc750 (set to 1 only if D3D capability count > 0x100).",
    ], [
        ("0x00549973", "DAT_007dc73c", "global: registered pipeline"),
        ("0x00549987", "DAT_007dc740/744/748", "globals: 3 sub-pipelines"),
        ("0x005499d3", "0x101", "D3D max-render-state cap threshold"),
    ], ["[UNCERTAIN] Subsystem name; matches RpPatchPluginAttach init pattern."], []),

    ("0x005499f0", 45, ["FUN_004d41e0"], ["external (Plugin term)"], [
        "0-param. Walks `DAT_007dc73c..DAT_007dc748` (4 pipeline globals).",
        "For each non-null entry: calls `FUN_004d41e0(*p)` (RxPipelineDestroy) and zeros it.",
        "Module-term counterpart of FUN_00549970.",
    ], [
        ("0x005499f3", "&DAT_007dc73c", "start of 4-entry pipeline array"),
        ("0x00549a08", "0x7dc74c", "exclusive end (4*4=16 bytes covered)"),
    ], [], []),

    ("0x00549a50", 56, ["FUN_004d4170", "FUN_004d4dd0", "FUN_004d4f90", "FUN_004d4380"], ["FUN_00549970"], [
        "0-param. Builds a single RxPipeline:",
        "`iVar1 = FUN_004d4170()` (RxPipelineCreate).",
        "Sets `*(iVar1+0x2c)=0x123` (signature/id), `*(iVar1+0x30)=1` (node count).",
        "Chains FUN_004d4dd0(iVar1) (pipeline-lock), then FUN_004d4f90(uVar2, 0, &PTR_s_PatchAtomic_csl_00623d98, 0) (register node from CSL — Custom Sub-Language string at 0x00623d98 = 'PatchAtomic.csl').",
        "Finally FUN_004d4380(uVar2) (pipeline-unlock/build).",
    ], [
        ("0x00549a5e", "0x123", "pipeline id/signature"),
        ("0x00549a65", "PTR_s_PatchAtomic_csl_00623d98", "RW plugin CSL string — RpPatch identifier"),
    ], [], ["[STUB] FUN_004d4f90 — RxPipelineNodeRequestCluster."]),

    ("0x00549a90", 14, [], ["external"], [
        "1-param `(param_1)`. `*(param_1+0x6c) = DAT_007dc73c` (assigns master pipeline pointer into atomic).",
    ], [
        ("0x00549a96", "+0x6c", "atomic pipeline-binding slot"),
    ], [], []),

    ("0x00549aa0", 126, ["FUN_004e8c70", "FUN_005495a0"], ["FUN_00549580"], [
        "1-param `(param_1)`. Reads flag word `uVar1 = *(param_1+0x2c)`.",
        "Bit-rearranges uVar1 into a different encoding (bits 0->1, 1->4, 2->3, 3->2, 4->5, 5->6 plus high byte 0xff0000 preserved, low bit 0 forced to 1) and forwards to `FUN_004e8c70(*(param_1+0x30), 0, encoded_flags)` (RpGeometryCreate).",
        "Stores returned geometry pointer into `*(DAT_007dc728 + iVar2) = param_1` (registers atomic at index iVar2 of pool DAT_007dc728).",
        "Calls FUN_005495a0(param_1) to bump refcount.",
    ], [
        ("0x00549ab1", "+0x2c", "atomic flag-word source"),
        ("0x00549b00", "+0x30", "vertex count"),
        ("0x00549b10", "DAT_007dc728", "atomic->geometry pool base"),
    ], [], []),

    ("0x00549b20", 1203, [
        "FUN_004522d0", "FUN_004e8640", "FUN_004e89e0", "FUN_004e8a10",
        "FUN_004f0970", "FUN_004f1130", "FUN_004f3b60", "FUN_004f3be0",
        "FUN_00549580", "FUN_0054b340"
    ], ["FUN_00549620"], [
        "Main patch->mesh build / refresh on atomic `param_1`. Approx 1.2KB body.",
        "Acquires registered geometry via FUN_00549580, then dispatches by `param_1[0x10]` flag word's bits 1..3 to update (in order): vertices+texcoords (bit 0), normals (bit 1), colors (bit 2), per-stage UV sets (bits 8..15).",
        "Per-vertex path: FUN_004f0970 (malloc) sized `(vcount*16 + ucount*6)*2 + 0x10`; writes header then per-quad block of 16 floats.",
        "Normal-recompute branch: optionally calls FUN_0054b340 with constants `0x38d1b717 (= ~9.8e-5f, tangent-tol)` and `0x3f000000 (= 0.5f, cos threshold)`.",
        "Texcoord-recompute pre-fills `0x461c4000 (= 10000.0f)` markers in slots 0x14/+4 per influence then clears.",
        "Calls FUN_004e8a10 (RpGeometryUnlock) on completion.",
    ], [
        ("0x00549b65", "+0xc / +0x10 / +0x12 / +0x13", "atomic struct fields: vbuf size, flags, ucount, vcount"),
        ("0x00549d60", "0x38d1b717", "= 9.832e-05f (angle tolerance for tangent smoothing)"),
        ("0x00549d6c", "0x3f000000", "= 0.5f (cos threshold)"),
        ("0x00549eb8", "0x461c4000", "= 10000.0f (sentinel for unassigned texcoord)"),
        ("0x00549c33", "0x12 / 0x13 / 0x16 / 0x17", "field offsets into atomic for normals/UVs/influences"),
    ], ["[UNCERTAIN] Whether `0x38d1b717` truly is a cos angle or an epsilon — not relevant for C1."], []),

    ("0x0054a120", 2315, [
        "FUN_004e89e0", "FUN_0054ba20", "FUN_0054bda0", "FUN_0054c400",
        "FUN_0054c4f0", "FUN_0054c870", "FUN_0054ceb0", "FUN_0054cfa0",
        "FUN_0054d090"
    ], ["external geometry-render entry"], [
        "Patch-geometry tessellator. 2-stage outer/inner loop driven by `param_3` (subdivision level).",
        "For each control patch (read via *(param_1+0xc) etc.) iterates over `param_1[0xd] + param_1[0xe]` influence quads.",
        "Allocates a 65-float scratch grid `local_108`, then per UV-stage calls FUN_0054f410 (control-net build) and one of FUN_0054d4d0/dbd0 (forward-diff matrix) + FUN_0054e520/eb90 (incremental evaluator).",
        "Two code paths: morph-target (bit-set on each influence) and standard 4x4 patch (uses FUN_0054c4f0/c870 evaluators).",
        "Emits triangle-index list via FUN_0054cfa0/d090.",
    ], [
        ("0x0054a134", "+0xc/+0x10/+0x13/+0x16/+0x17", "atomic vbuf/uv/influence offsets"),
        ("0x0054a233", "_DAT_005cc320 (= 1.0f)", "RW unit"),
    ], ["[UNCERTAIN] Exact RW node name — likely RxPatchAtomicTessellate."], []),

    ("0x0054b340", 739, [
        "DAT_007d3ff8+0x108 (malloc)", "DAT_007d3ff8+0x10c (free)",
        "FUN_0054b630", "FUN_004c3ac0 (Vec3Magnitude C3 hook)"
    ], ["FUN_00549b20"], [
        "5-param `(patch*, angle_tol_f, cos_thresh_f, vbuf, nbuf)`.",
        "Phase 1: builds per-vertex adjacency in `param_4` (malloc'd, vcount*0xc bytes). For each unique vertex pair within `angle_tol`, calls FUN_0054b630 twice (one for each direction).",
        "Phase 2: walks vbuf computing weighted-average normals across adjacent faces meeting the `cos_thresh` test (dot-product gate).",
        "Uses FUN_004c3ac0 (Vec3Magnitude) + division by _DAT_005cc320 to renormalize.",
        "Free adjacency on exit.",
    ], [
        ("0x0054b362", "0x30123", "RW memory tag for vertex-adjacency"),
        ("0x0054b40e", "DAT_005d757c (= 0.0f)", "epsilon comparison"),
    ], [], []),

    ("0x0054b630", 150, ["DAT_007d3ff8+0x108 (malloc)", "DAT_007d3ff8+0x10c (free)"], ["FUN_0054b340"], [
        "3-param `(buf, slot_idx, value16)`. Appends `param_3` (a uint16) to a per-slot dynamic array.",
        "Slot record at `buf + param_2*0xc` is: `{count, capacity, *data}`.",
        "If count==capacity: doubles capacity (`+8`), reallocates via DAT_007d3ff8+0x108, memcpys old data, frees old.",
        "Writes value16 at `data[count]` and increments count.",
    ], [
        ("0x0054b652", "0x30123", "RW memory tag"),
    ], [], []),

    ("0x0054b6d0", 100, ["FUN_004f3ce0"], ["external"], [
        "1-param `(param_1)` (patch atomic).",
        "Computes serialized-stream payload size: returns `FUN_004f3ce0(param_1+0x48) + ((bit-rearrange(uVar1) ...) * iVar3 + iVar2*0x11 + iVar4*0xb)*4 + 0x20`.",
        "Bit pattern: `((uVar1>>1&1) + (uVar1&1))*3 + (uVar1>>0x10&0xff)*2 + (uVar1>>2&1)` — count of active vertex attribute streams.",
        "Header is 0x20 bytes; positions 0x11 floats per quad, normals 0xb floats per quad.",
    ], [
        ("0x0054b6da", "+0x2c flags / +0x30 vcount / +0x34 vcount2 / +0x38 ucount", ""),
        ("0x0054b724", "0x20", "stream header bytes"),
        ("0x0054b727", "0x11", "per-positional-quad float count"),
        ("0x0054b72b", "0xb", "per-normal-quad float count"),
    ], [], []),

    ("0x0054b740", 376, ["FUN_004cc790 (RwStreamRead)", "FUN_005493e0", "FUN_004cc5e0 (RwStreamSkip)", "FUN_004f3e90"], ["external — stream-read dispatcher"], [
        "1-param `(stream_handle)`.",
        "Reads 0x14-byte header into local_14, then calls FUN_005493e0(header) to allocate a PatchMesh struct (returns puVar4).",
        "Conditionally reads from stream into puVar4[0..3] when corresponding flag bits set: positions (vcount*0xc), normals (vcount*0xc), colors (vcount*4), N UV sets (vcount*nUV*8), influences (icount*0x44), patch-defs (pcount*0x2c).",
        "Calls FUN_004cc5e0(stream, 8, 0, 0) (skip post-blob), then FUN_004f3e90(stream, puVar4+0x12) (stream-read RWList linkage).",
        "Returns the patch-mesh pointer or 0 on stream-read failure.",
    ], [
        ("0x0054b756", "0x14", "patch-mesh on-disk header size"),
        ("0x0054b76e", "+0xb / +0xf", "flag-byte / numUV-byte fields"),
        ("0x0054b86e", "0x44", "influence record stride"),
        ("0x0054b884", "0x2c", "patch-def record stride"),
    ], [], ["[STUB] FUN_005493e0 — PatchMesh allocator (lives in adjacent bucket 0x00549...)."]),

    ("0x0054b8c0", 344, ["FUN_004cc770 (RwStreamWrite)", "FUN_004cc6e0", "FUN_004f3d40"], ["external — stream-write dispatcher"], [
        "2-param `(patch_mesh*, stream_handle)`. Inverse of FUN_0054b740.",
        "Writes 0x14-byte header, then conditionally writes positions/normals/colors/UV sets, influences (0x44 stride), patch-defs (0x2c stride) to stream.",
        "Final FUN_004f3d40(patch_mesh+0x12, stream) writes RWList linkage.",
    ], [
        ("0x0054b8d2", "+0xc/+0xd/+0xe/+0xf", "vcount/pcount/icount/numUV fields"),
        ("0x0054b8db", "0x14", "header size"),
        ("0x0054b9c5", "0x44 / 0x2c", "record strides (matches reader)"),
    ], [], []),

    ("0x0054ba20", 896, ["FUN_0054f410", "FUN_0054d4d0", "FUN_0054e520"], ["FUN_0054a120"], [
        "4-param `(out_buf, stride, control_mat, levels)`.",
        "Position-only patch evaluator (3-coord output).",
        "Computes step `_DAT_005cc320 / (float)param_4`. Calls FUN_0054f410 (control-net builder) into local_200, FUN_0054d4d0 (3-coord forward-diff matrix) into local_100, FUN_0054e520 to seed local_330..local_278 forward-difference registers.",
        "Nested loops over u/v of (param_4+1)x(param_4+1) grid emit `*out=(x,y,z)` and advance forward-difference accumulators by their next-order partners.",
        "Classic cubic-Bezier forward-difference patch tessellator (3-component).",
    ], [
        ("0x0054ba32", "_DAT_005cc320 (= 1.0f)", "RW unit"),
    ], [], []),

    ("0x0054bda0", 1621, ["FUN_0054f410", "FUN_0054dbd0", "FUN_0054eb90"], ["FUN_0054a120"], [
        "4-param `(out_buf, stride, control_mat, levels)`.",
        "4-coord (RGBA byte) patch evaluator — twin of FUN_0054ba20 but for color outputs.",
        "Calls FUN_0054dbd0 (4-coord forward-diff matrix) + FUN_0054eb90 (4-coord seed).",
        "Quantizes each emit via `(undefined1)(int)ROUND(...)` — float -> u8 conversion for RGBA8.",
    ], [
        ("0x0054bdb2", "_DAT_005cc320 (= 1.0f)", ""),
    ], [], []),

    ("0x0054c400", 230, [], ["FUN_0054a120"], [
        "4-param `(out_buf, stride, src6_floats, levels)`.",
        "Bilinear (2D-affine, 2-channel) interpolator over a 3-corner barycentric weighted sum.",
        "For each (i,j) in `(0..param_4)`: emits `(p1.x*a + p3.x*c + p2.x*b)`-style barycentric blend of `src[0..5]` (3 uv pairs).",
        "Step = `_DAT_005cc320 / (float)param_4`. Pure-2D version of the 3D evaluator.",
    ], [
        ("0x0054c405", "_DAT_005cc320", ""),
    ], [], []),

    ("0x0054c4f0", 888, ["FUN_0054d4d0", "FUN_0054e520"], ["FUN_0054a120"], [
        "4-param `(out_buf, stride, control_mat, levels)`. Position-only forward-diff evaluator over an already-built control net `param_3`.",
        "Twin of FUN_0054ba20 but skips the FUN_0054f410 step — caller passes the control net directly.",
    ], [
        ("0x0054c4f5", "_DAT_005cc320", ""),
    ], [], []),

    ("0x0054c870", 1590, ["FUN_0054dbd0", "FUN_0054eb90"], ["FUN_0054a120"], [
        "4-param `(out_buf, stride, control_mat, levels)`. 4-channel byte-quantized evaluator.",
        "Twin of FUN_0054bda0 but no FUN_0054f410 prep — caller provides the control net.",
    ], [
        ("0x0054c875", "_DAT_005cc320", ""),
    ], [], []),

    ("0x0054ceb0", 230, [], ["FUN_0054a120"], [
        "4-param `(out_buf, stride, src8_floats, levels)`.",
        "Bilinear UV interpolator over a 4-corner quad (8 floats = 4 uv pairs).",
        "For each (i,j) in `(0..param_4)`: emits horizontally-blended (then vertically-blended) UV interpolation. Used for UV-stage tessellation.",
    ], [
        ("0x0054ceb5", "_DAT_005cc320", ""),
    ], [], []),

    ("0x0054cfa0", 225, [], ["FUN_0054a120"], [
        "3-param `(out_indices, base_index, levels)` (short pointer / short / uint).",
        "Emits triangle-fan index pattern for a single patch quadrant.",
        "Two-zigzag fill: pairs of adjacent vertices `(i, n-i+1+base)` then closing strip.",
        "For odd `param_3`: duplicates last index.",
        "Generates RW triangle list expected by RxClusterRender.",
    ], [
        ("0x0054cfa6", "i + (n-i)+1 form", "zig-zag index pairing"),
    ], [], []),

    ("0x0054d090", 150, [], ["FUN_0054a120"], [
        "3-param `(out_indices, base, levels)`. Triangle-strip variant of FUN_0054cfa0.",
        "Two-row zigzag pattern same as FUN_0054cfa0 but without the closing odd-case duplication branch.",
    ], [], [], []),

    ("0x0054d4d0", 1785, [], ["FUN_0054ba20", "FUN_0054c4f0"], [
        "2-param `(out_dst*[float], src_control_net*[float])`. Forward-difference matrix builder, 3-component (xyz).",
        "Body is a long sequence of pure float copies/subtractions/multiplications by `_DAT_005cc31c (= 0.333...f)`.",
        "Produces a 3x16 forward-difference matrix from a 4x4 control net so subsequent evaluator (FUN_0054e520) can step incrementally.",
        "Classic De Casteljau -> forward-difference transform for cubic Bezier patches.",
    ], [
        ("0x0054d4d0", "_DAT_005cc31c (= ~0.333..f / cubic-FD constant)", "used ~50x"),
    ], [], []),

    ("0x0054dbd0", 2379, [], ["FUN_0054bda0", "FUN_0054c870"], [
        "2-param `(out_dst*, src_control_net*)`. Identical structure to FUN_0054d4d0 but for 4-component (RGBA) source.",
        "Builds a 4-channel forward-difference matrix from a 4x4 color control net.",
    ], [
        ("0x0054dbd0", "_DAT_005cc31c (cubic-FD constant)", ""),
    ], [], []),

    ("0x0054e520", 1646, [], ["FUN_0054ba20", "FUN_0054c4f0"], [
        "4-param `(out_state, fd_matrix, u_step, v_step)`. 3-coord forward-difference seed.",
        "Computes `u, u^2, u^3, u*v, u^2*v, u^3*v, v, v*u, v^2, v^2*u, v^2*u^2, v^2*u^3, v^3, v^3*u, ...` weighted with `_DAT_005cd0a0 (= 3.0f)`-style cubic Bernstein constants.",
        "Initial state register dump for incremental evaluation in the evaluator loop.",
    ], [
        ("0x0054e520", "_DAT_005cd0a0 (= 3.0f / cubic-Bernstein scalar)", ""),
    ], [], []),

    ("0x0054eb90", 2161, [], ["FUN_0054bda0", "FUN_0054c870"], [
        "4-param `(out_state, fd_matrix, u_step, v_step)`. 4-coord forward-difference seed — twin of FUN_0054e520 for RGBA.",
    ], [
        ("0x0054eb90", "_DAT_005cd0a0 (= 3.0f)", ""),
    ], [], []),

    ("0x0054f410", 1216, [], ["FUN_0054a120", "FUN_0054ba20", "FUN_0054bda0"], [
        "2-param `(out_dst*, src_4x4_control_points*)`. Control-net builder.",
        "Copies 4x4 corner points (16 dst slots at +0, +4, +8, +0xc, +0x10, +0x14, +0x18, +0x1c, +0x20, +0x24, +0x28, +0x2c, +0x30, +0x34, +0x38, +0x3c).",
        "Computes interior 2x2 control points via Bezier blend formulas using `_DAT_005ccac8 (= ~0.166..f, 1/6 scaling)` and `_DAT_005cc31c (= 1/3)`.",
        "Equivalent to Catmull-Rom / Bezier conversion of corner-tangent net to Bezier control points.",
    ], [
        ("0x0054f410", "_DAT_005ccac8 (= 1/6, control-net Bezier-mix constant)", ""),
        ("0x0054f410", "_DAT_005cc31c (= 1/3)", ""),
    ], [], []),

    # ============================== Image stream loader (0x0054fd60) =====================
    ("0x0054fd60", 967, [
        "FUN_004cbd30 (RwStreamRead)", "DAT_007d3ff8+0x108 (RwMalloc)",
        "FUN_004d7ff0", "FUN_004d8480", "FUN_004cc5e0", "FUN_004cee90", "FUN_004cefd0",
        "FUN_004d5340", "FUN_004c77c0 (RasterCreate)", "FUN_004cdd00 (ImageDestroy)",
        "FUN_004c7650", "FUN_004c76f0", "FUN_004db2e0", "FUN_004c7860",
        "FUN_004d5310", "FUN_004c7600", "FUN_004c5a00 (TextureCreate)",
        "FUN_004c5ae0", "FUN_004c5b50", "FUN_004c5bc0"
    ], ["external Texture-stream-read entry"], [
        "2-param `(stream, out_texture_ptr*)`. Reads a texture dictionary entry from stream.",
        "Reads 0x48-byte header into local_48, allocates `local_8 * 4` image-pointer array.",
        "Loop: per-image, reads 0x18-byte sub-chunk header, calls FUN_004cee90 (ImageStreamRead) and optionally FUN_004cefd0 (FindGamma) when raster bit-flag clear.",
        "Determines raster format (0x4 RGBA / 0x9004 cube-RGBA / 0x8004 mip-stack) by image count + low-byte mask.",
        "Builds a RasterCreate via FUN_004c77c0; on failure tries 0x500 (DDS-compressed) then 0x100 fallback flags.",
        "Locks/unlocks via FUN_004c7860/004c7600, copying each image into the raster face/level via FUN_004d5310.",
        "Finally allocates a Texture via FUN_004c5a00, sets name (local_48) and mask name (auStack_28), tweaks address-mode bits from uStack_4, and stores in *param_2.",
    ], [
        ("0x0054fd92", "0x48", "texture-header size"),
        ("0x0054fdde", "0x18", "image-sub-chunk header size"),
        ("0x0054fe8d", "0x4 / 0x9004 / 0x8004", "raster type flags: regular / cubemap / mip-stack"),
        ("0x0054ff52", "0x500 / 0x100", "DDS / compressed fallback flags"),
        ("0x0054ffd5", "+0x50", "Texture filter/address-mode bits"),
        ("0x0054ffe6", "0xf00 / 0xf000", "address-mode U/V bit masks"),
    ], ["[UNCERTAIN] Whether bucket-internal 'TextureStreamRead' or extension thereof; ID 0x500 is D3D format hint."], ["[STUB] FUN_004db2e0 — likely RwImageCopy-to-raster-mip."]),

    # ============================== RtFS subsystem ==============================
    ("0x00550400", 45, ["DAT_007dc768 (errfn)"], ["FUN_00551330"], [
        "0-param. Reads `DAT_007dc758` and `DAT_007dc75c`.",
        "Returns 1 if `DAT_007dc758` is outside `(0, DAT_007dc75c+1)` (capacity check).",
        "Otherwise calls error callback `(*DAT_007dc768)(5)` and returns 0. RtFS pre-registration capacity guard.",
    ], [
        ("0x0055040c", "DAT_007dc758", "current registered FS count"),
        ("0x00550410", "DAT_007dc75c", "max registered FS slots"),
        ("0x0055041c", "5", "errcode: REGISTRY FULL"),
    ], [], []),

    ("0x005504d0", 72, ["DAT_007dc768 (errfn)", "DAT_007d3ff8+0xe8 (strcmp)"], ["FUN_00551330"], [
        "1-param `(name_string)`. Linear search of registered-FS list rooted at DAT_007dc754.",
        "Each node has next-pointer at +0 and name-string at +0x14.",
        "Returns the matching node or NULL (and calls errfn(6) when not found).",
    ], [
        ("0x005504d3", "DAT_007dc754", "RtFS registered list head"),
        ("0x005504e0", "6", "errcode: NAME_NOT_FOUND"),
        ("0x005504ee", "+0x14", "name-string field offset"),
    ], [], []),

    ("0x00550520", 95, ["DAT_007dc768 (errfn)", "DAT_007d3ff8+0xe8 (strcmp)"], ["FUN_00551330"], [
        "1-param `(name)`. Like FUN_005504d0 but inverse — returns 1 and calls errfn(7) when a match is FOUND (i.e., name-collision check during registration).",
        "Returns 0 when no collision.",
    ], [
        ("0x00550535", "7", "errcode: NAME_ALREADY_REGISTERED"),
    ], [], []),

    ("0x00550580", 237, ["EnterCriticalSection", "LeaveCriticalSection", "indirect via fs_vtbl[0x14] (slot-fetch) and fs_vtbl[0x28] (open-impl)"], ["FUN_00550670"], [
        "5-param `(fs_record, name, flags, async_cb_ctx, async_cb_arg)`. Open-file routine on FS instance.",
        "Enters critical section DAT_009124e0; iterates `*(fs+4)` slot count using `(*(fs+0x14))(fs, idx)` to fetch each file-handle, finds first with `*(fh+0x20) == 1` (FREE), marks it state=5 (BUSY).",
        "Leaves critical section. If `flags & 0x10` set OR `*(fh+0x18) != 0`: sets `*(fh+0x28)=2`, stores `param_4`, `param_5` into +0x30, +0x34 (async cb).",
        "Forces async flag (`flags|=0x10`) if `*(fs+0x10) != 0`. Invokes open-impl `(*(fs+0x28))(fs, fh, name, flags)`; on success returns fh with `*(fh+0x1c)=2` (state=OPENING); on failure marks state=1 (FREE) and returns 0.",
        "On exhaustion sets _DAT_007dc764 = 8 and calls errfn(8).",
    ], [
        ("0x005505a3", "DAT_009124e0", "RtFS critical section"),
        ("0x005505e0", "+0x20 file-state (1=FREE, 5=BUSY)", ""),
        ("0x00550612", "+0x28 cb-state, +0x30/+0x34 cb args, +0x18 async-flag, +0x1c open-state", ""),
        ("0x0055062b", "0x10 (FLAG_ASYNC)", ""),
        ("0x00550660", "8", "errcode: NO_FREE_FILE_SLOTS"),
    ], [], []),

    ("0x00550670", 200, ["DAT_007d3ff8+0xf4 (strlen)", "DAT_007d3ff8+0xf0 (strcmp)", "_strncpy", "FUN_00550580"], ["external"], [
        "4-param `(path, flags, cb_ctx, cb_arg)`. Top-level Open-by-path entry. Parses leading 'mount:' prefix.",
        "Walks the path to first ':'; copies prefix into local 100-byte buffer; linear-scans DAT_007dc754 registry comparing prefix to fs->mount-name (`puVar[3]` field).",
        "On match, jumps to FUN_00550580 with the matched fs. If no ':' or no match, falls back to default fs `DAT_007dc76c`.",
        "Returns 0 + errfn(6) when both registered FS list and default are empty.",
    ], [
        ("0x0055067d", "DAT_007dc75c", "registered-count gate"),
        ("0x005506a2", "':' separator", "mount-prefix delimiter"),
        ("0x005506b1", "+0x14 of fs record", "fs name pointer"),
        ("0x005506d6", "DAT_007dc76c", "default-FS fallback"),
    ], [], []),

    ("0x00550740", 6, [], ["FUN_00551090 family"], [
        "0-param. Returns address `&DAT_007dc754` — pointer to the FS-list head pointer itself (used by callers for set-on-empty).",
    ], [
        ("0x00550740", "&DAT_007dc754", ""),
    ], [], []),

    ("0x00550750", 56, ["DAT_007d3ff8+0xf0 (strcmp)"], ["FUN_00551330"], [
        "1-param `(name)`. Quiet variant of FUN_005504d0 — walks DAT_007dc754 and returns matching node OR null. No errfn invocation.",
    ], [
        ("0x00550753", "DAT_007dc754", ""),
        ("0x00550761", "+0xc / +0x10", "(fields skipped)"),
        ("0x00550767", "+0x14 (name)", ""),
    ], [], []),

    ("0x005507a0", 6, [], ["external"], [
        "0-param. Returns `DAT_007dc76c` — the default-FS pointer.",
    ], [
        ("0x005507a0", "DAT_007dc76c", "default FS instance"),
    ], [], []),

    ("0x00550a20", 193, ["indirect via fs_vtbl[0x30] (read-impl) and +0x38 (seek-impl)"], ["external"], [
        "3-param `(buf, size, fh)`. `gets`-equivalent on open RtFS file handle.",
        "Reads up to `size-1` bytes via `(*(fh.fs.vtbl+0x30))(fh, buf, size-1)`.",
        "Walks buffer; on '\\n' terminates and seeks back to put cursor after the newline via `(*(fs.vtbl+0x38))(fh, off, SEEK_CUR=2)`.",
        "Strips '\\r' from '\\r\\n' pairs in-place (memmove).",
        "Sets async cb-state to 4/3 on '\\n' break.",
    ], [
        ("0x00550a31", "+0x38 fs.vtbl (= fh.parent)", ""),
        ("0x00550a47", "'\\n' / '\\r'", "line terminators"),
        ("0x00550a54", "+0x28=4 (cb-state EOF-LINE), +0x20=3 (file-state)", ""),
    ], [], []),

    ("0x00550bd0", 14, ["indirect via fs_vtbl[0x40]"], ["external"], [
        "1-param `(fh)`. Tail-call into fs vtbl slot 0x40 (likely Tell/GetPos).",
        "Single thunk: `(**(code **)(*(fh+0x38)+0x40))()` — Ghidra warns no callees recoverable.",
    ], [
        ("0x00550bd5", "+0x38 (fh.fs.vtbl)", ""),
        ("0x00550bd8", "+0x40", "vtbl slot 16 (e.g. RtFileTell)"),
    ], [], []),

    ("0x00551090", 240, [
        "FUN_004d7ff0", "GetOverlappedResult", "GetLastError",
        "FUN_00550740", "FUN_00551550", "indirect via fs_vtbl[0x38] (seek-impl)"
    ], ["FUN_005512b0"], [
        "2-param `(fh, wait_BOOL)`. Async-IO completion poll for disk-backed RtFS handle.",
        "Only runs if `fh[6] != 0` AND `fh[8] == 3` (state=PENDING).",
        "Calls FUN_004d7ff0 to compute new offset. If `fh[10] == 5` (overlapped IO outstanding) calls GetOverlappedResult on `(HANDLE)fh[0x10]`, `(LPOVERLAPPED)(fh+0x11*4)`.",
        "On failure: writes GetLastError into `(&DAT_007dc754)+0x10`, marks state=4 (FAILED) and fires async cb via FUN_00551550.",
        "On success: bumps offset, sets state=2 (DONE), fires cb.",
        "Codes 0x3e4/0x3e5 treated as 'still pending', goto LAB_00551179 returns current state.",
    ], [
        ("0x005510b9", "+0x18 (overlapped HANDLE slot)", ""),
        ("0x005510ca", "+0x40 (OVERLAPPED struct base)", ""),
        ("0x0055112b", "0x3e4 (ERROR_IO_INCOMPLETE) / 0x3e5 (ERROR_IO_PENDING)", "Win32 codes"),
    ], [], []),

    ("0x00551190", 251, [
        "FUN_005504d0", "DAT_007d3ff8+0x108 (malloc)", "DAT_007d3ff8+0x114 (calloc-equiv)",
        "FUN_00551330", "DAT_007d3ff8+0x10c (free)"
    ], ["external"], [
        "3-param `(slot_count, name, mount_prefix)`. Constructs a disk-backed FS instance.",
        "Checks FUN_005504d0(mount_prefix) is empty (no collision), allocates 0x5c-byte FS object, fills vtbl with 11 method pointers: open(LAB_00550c30), close(LAB_00551290), read(LAB_00550c50), write(LAB_00550d80), seek(LAB_00550da0), tell(LAB_00550f20), getsize(LAB_00551050), poll(FUN_00551090), ?(LAB_00551180), ?(LAB_005514e0), wait(FUN_005512b0), ?(LAB_005512d0).",
        "Allocates `slot_count * 0x60` file-handle pool, links each fh.parent back to fs via +0x38.",
        "Calls FUN_00551330(fs, slot_count, name, mount_prefix) to register; cleans up on failure.",
    ], [
        ("0x005511aa", "0x5c", "FS struct size"),
        ("0x005511af..0x00551208", "+0x14..+0x4c", "vtbl method slots"),
        ("0x00551215", "0x60", "file-handle struct size"),
        ("0x00551218", "0x401be", "RW memory tag for FS"),
    ], [], ["[STUB] LAB_00550c30 / LAB_00551290 / LAB_00550c50 / LAB_00550d80 / LAB_00550da0 / LAB_00550f20 / LAB_00551050 / LAB_00551180 / LAB_005514e0 / LAB_005512d0 — internal labels = the 10 RtFSDisk method implementations."]),

    ("0x005512b0", 27, ["FUN_00551090"], ["external"], [
        "1-param `(fh)`. Synchronous-wait wrapper.",
        "If `*(fh+0x20) != 1` (not FREE) call FUN_00551090(fh, 0=non-blocking poll). Returns fh+0x20 state.",
    ], [
        ("0x005512b3", "+0x20 (file-state slot)", ""),
    ], [], []),

    ("0x00551330", 216, [
        "FUN_00550400", "FUN_00550520", "DAT_007d3ff8+0xcc (strcpy)",
        "indirect via fs_vtbl[0x14] (slot-fetch)",
        "FUN_00550750", "DAT_007d3ff8+0xf4 (strlen)", "DAT_007d3ff8+0x108 (malloc)"
    ], ["FUN_00551190"], [
        "4-param `(fs, slot_count, name, mount_prefix)`. RtFSManagerRegister-equivalent.",
        "Returns 2 if capacity full (FUN_00550400 fails), 6 if name already registered (FUN_00550520 returns non-zero).",
        "strcpy's `name` into `fs+0x50` (name slot, 32 bytes).",
        "Initializes all `slot_count` file handles via fs.vtbl[0x14] indirect: sets fh+0x20=1 (FREE), fh+0x18 = fs+0x10 (parent ref).",
        "Validates mount_prefix not already registered (FUN_00550750), strdup's it into fs+0xc (alloc DAT_007d3ff8+0x108).",
        "Marks fs state: +0x8 = 2, +0x20 = 0, +0x24 = 0. Returns 1 on success or 7 on mount-prefix collision.",
    ], [
        ("0x00551367", "+0x50 (fs.name)", ""),
        ("0x00551373", "+0x4 (slot count)", ""),
        ("0x0055138a..", "+0x20=1 (FREE state)", ""),
        ("0x005513c4", "+0xc (mount-prefix dup pointer)", ""),
        ("0x005513e7", "+0x8 = 2 (REGISTERED state)", ""),
        ("0x00551400", "+0x24 = 0 (counters cleared)", ""),
    ], [], []),

    ("0x00551410", 66, ["DAT_007d3ff8+0xf4 (strlen)"], ["external"], [
        "1-param `(path)`. Returns pointer to char immediately AFTER first ':' in `path`, or `path` if no ':'.",
        "Path-suffix splitter for 'mount:rest' parsing.",
    ], [], [], []),

    ("0x00551460", 127, ["DAT_007d3ff8+0xcc (strcpy)"], ["external"], [
        "5-param `(out_buf, buf_size, prefix, suffix, sep_char)`. Concatenate `prefix+suffix` into `out_buf`.",
        "Computes both string lengths (manual strlen loop with `0xffffffff` countdown — MSVC strlen idiom). Verifies combined length fits.",
        "strcpy's prefix; appends suffix char-by-char, replacing every '/' or '\\\\' in suffix with `sep_char` (path-separator normalization).",
    ], [
        ("0x00551466", "0xffffffff", "manual strlen initial counter"),
        ("0x005514c1", "'/' / '\\\\'", "input path separators to normalize"),
    ], [], []),

    ("0x00551550", 77, ["indirect via fh[0x30] (cb-fn)"], ["FUN_00551090 family"], [
        "1-param `(fh)`. User-callback dispatcher on async-IO completion.",
        "Only fires when `*(fh+0x18) != 0` (async flag) AND `*(fh+0x28) != 1` (cb-state not idle) AND `*(fh+0x30) != 0` (cb-fn set).",
        "Picks arg from `*(fh+8)` (when cb-state==4) or `*(fh+0x2c)` else; marks `*(fh+0x28)=1` (idle); invokes `(*(fh+0x30))(fh, arg, *(fh+0x20), cb_state, *(fh+0x34))`.",
    ], [
        ("0x00551560", "+0x18 (async-enabled flag)", ""),
        ("0x00551565", "+0x28 (cb-state)", ""),
        ("0x00551581", "+0x30 (cb-fn) / +0x34 (cb-arg2)", ""),
    ], [], []),

    # ============================== Animation channel system ==============================
    ("0x00551840", 167, ["FUN_004cc580 (RwStreamWriteChunkHeader)", "FUN_004cc770", "FUN_004cbe80"], ["external"], [
        "3-param `(stream, ?, channel_idx*0x60)`. Serialize the 8-track set of animation channel `param_3`.",
        "Walks `DAT_009124f8 + 8 + param_3` (8 pointer slots).",
        "Builds 32-bit bitmask `local_4` with one bit per non-null track; size = 4 + 0x20*active_count.",
        "Writes header chunk(id=1, computed_size, 0x37002, 10) via FUN_004cc580 then the bitmask via FUN_004cc770.",
        "For each non-null slot, writes 0x20-byte payload extracted from `*(track[0])+0x14` via FUN_004cbe80.",
    ], [
        ("0x00551849", "DAT_009124f8+8", "animation channels-array base"),
        ("0x00551851", "8", "max tracks per channel"),
        ("0x0055185c", "0x20", "per-track payload bytes"),
        ("0x00551877", "id=1, tag=0x37002, ver=10", "RW chunk header"),
    ], [], []),

    ("0x00551a50", 121, ["FUN_004c57a0", "FUN_004c5770"], ["external"], [
        "1-param `(channel*)`. For each of 2 sub-outputs of channel: scan 8 tracks looking for one whose track[0]->keyframe[0x20+offs*4] matches the current output index.",
        "On match, lazy-allocate output via FUN_004c57a0 (track-set head create). On miss across all tracks, free via FUN_004c5770 if pre-allocated.",
        "Manages per-output (Pos/Rot or similar) blended-track-collection lifetime.",
    ], [
        ("0x00551a64", "+0x14 (keyframe-table ptr)", ""),
        ("0x00551a73", "+0x20 (target-output id)", ""),
        ("0x00551aa3", "+0xb (track count)", ""),
    ], [], ["[STUB] FUN_004c57a0 — head allocator; FUN_004c5770 — head destroy."]),

    ("0x00551ad0", 285, [
        "FUN_0052da20", "DAT_007d3ff8+0x118 (specialised malloc)",
        "_strncpy", "FUN_005522e0"
    ], ["external"], [
        "1-param `(name_string)`. Construct an animation/clip object.",
        "Calls FUN_0052da20(DAT_00623e24, 2, 0, 1.0f) to allocate parent (DAT_00623e24 = clip-class id).",
        "Allocates 0x44-byte name struct from DAT_007dc770 pool (tag 0x30135); strncpy's name (32 chars max) and sets +0x40 = 1 (refcount).",
        "Calls FUN_005522e0 twice to install start and end keyframes (`time=0, time=clip_duration`) with identity transform (translation 0, rotation identity, scale 1).",
    ], [
        ("0x00551adb", "DAT_00623e24", "animation-clip class id"),
        ("0x00551b08", "DAT_007dc770", "keyframe pool"),
        ("0x00551b18", "0x20 (name length)", ""),
        ("0x00551b9d", "0x3f800000 = 1.0f", "identity scale/rotation"),
        ("0x00551b8b", "0x20003", "initial flags (set via |= mask)"),
    ], [], []),

    ("0x00551c40", 94, ["DAT_007d3ff8+0x118 (specialised malloc)", "FUN_004cbd30", "FUN_004cc790"], ["external"], [
        "1-param `(stream)`. Allocate a 0x40-byte keyframe + stream-read its two 0x20-byte halves.",
        "Allocates from DAT_007dc770 pool. FUN_004cbd30(stream, kf, 0x20) reads first half; FUN_004cc790(stream, kf+0x20, 0x20) reads second; sets +0x40=1.",
    ], [
        ("0x00551c47", "DAT_007dc770", "keyframe pool"),
        ("0x00551c66", "0x20 / 0x20", "keyframe halves"),
        ("0x00551c8e", "+0x40 = 1 (refcount)", ""),
    ], [], []),

    ("0x00551ca0", 53, ["FUN_004cbe80", "FUN_004cc770"], ["external"], [
        "2-param `(keyframe*, stream)`. Stream-write a 0x40-byte keyframe.",
        "FUN_004cbe80(stream, kf, 0x20) + FUN_004cc770(stream, kf+0x20, 0x20).",
        "Returns `kf & -uint(success)` (kf on success, 0 on failure).",
    ], [
        ("0x00551cb0", "0x20", "half size"),
    ], [], []),

    ("0x00551ce0", 6, [], ["FUN_005524a0"], [
        "0-param. Trivial constant return: `0x40` (64) — keyframe record size.",
    ], [
        ("0x00551ce0", "0x40", "keyframe size"),
    ], [], []),

    ("0x00551cf0", 56, ["indirect via DAT_007d3ff8+0x11c (pool free)", "FUN_00562560"], ["external"], [
        "1-param `(track)`. Refcount-decrement on attached keyframe at `*(track+0x14) + 0x40`.",
        "When ref reaches 0: returns the keyframe to DAT_007dc770 pool, then calls FUN_00562560(track) (track-destroy).",
    ], [
        ("0x00551cfa", "+0x14 (keyframe pointer)", ""),
        ("0x00551cff", "+0x40 (refcount)", ""),
        ("0x00551d11", "DAT_007dc770", "keyframe pool"),
    ], [], []),

    ("0x00551d40", 649, [
        "FUN_004c45f0 (matrix-init)", "FUN_004c51a0 (matrix-rotate)",
        "FUN_004c4d20 (matrix-translate-or-scale)", "FUN_004c52f0 (matrix-apply)",
        "FUN_005402d0"
    ], ["external"], [
        "1-param `(channel_offset)`. Big evaluator: for each of 2 channel-outputs at DAT_009124f8+channel_offset, resets transform-state to identity + zero scale.",
        "Walks 8 tracks. For each track with non-null track[9]=stride and track[0xf]=callback:",
        "  Per influence (param[0xb] entries), looks up output index in `track[0]+0x14+0x20+i*4`, finds matching channel output (must be 0 or 1).",
        "  Builds local matrix `local_40..local_8` from track sample data (`piVar9[-3..+2]`), passing through either `LAB_005524c0`-shaped path (raw transform copy) or `LAB_00552050`-shaped path (rotate-translate-rotate composition with `_DAT_005cd100` scaling), or generic `(*pcVar5)(&local_40, local_68)` callback.",
        "  Applies to output via FUN_004c52f0(out, &local_40, 1).",
        "After loop: FUN_005402d0(channel_offset, output_0, output_1) propagates result.",
    ], [
        ("0x00551d6f..0x00551d80", "+0x3..0xe (transform fields)", "init to 0; +0x0..2=1.0f, +0x3 |= 0x20003"),
        ("0x00551da7", "LAB_005524c0", "raw-transform copy branch"),
        ("0x00551dca", "LAB_00552050", "rotate-translate-rotate branch"),
        ("0x00551e02", "_DAT_005cd100", "rotate-rate constant"),
        ("0x00551e2c", "DAT_005e488c / DAT_005e4898", "Y-axis / -Y-axis vector literals"),
    ], ["[UNCERTAIN] Whether LAB_005524c0/00552050 are inline mini-funcs or true callees — function_callees may surface them as separate entries."], []),

    ("0x00551fd0", 8, [], ["external"], [
        "1-param `(track)`. Trivial getter: returns `*(track+0x14)` — keyframe-table pointer.",
    ], [
        ("0x00551fd3", "+0x14", ""),
    ], [], []),

    ("0x00551fe0", 55, ["FUN_0052e0e0"], ["external"], [
        "2-param `(channel_offset, arg2)`. For each of 8 track slots at DAT_009124f8+8+channel_offset, if non-null call FUN_0052e0e0(track, arg2).",
        "Broadcast-method on the 8-track set.",
    ], [
        ("0x00551fe7", "DAT_009124f8+8", ""),
        ("0x00551fed", "8", "track count"),
    ], [], []),

    ("0x00552020", 39, [], ["external"], [
        "1-param `(channel_offset)`. Has-any-active query. Walks 8 track slots; returns 1 on first non-null, 0 if all null.",
    ], [], [], []),

    # ============================== Rt2d path engine ==============================
    ("0x00552230", 174, [
        "fpatan (FPU intrinsic)", "FUN_004c51a0 (rot-Y axis)",
        "FUN_004c4d20 (rot-by-angle)"
    ], ["FUN_005522e0"], [
        "2-param `(out_transform[6], src_matrix[16])`.",
        "Copies 4x4 source into local 16-float buffer, computes Z-rotation `out[0] = fpatan(-local_30, local_40[0])`.",
        "Rotates the local matrix by -Z axis then +Y axis, with angle in radians * `_DAT_005e48a8`.",
        "Packs 6 floats `(angle, m00, m01, m10, t_x, t_y)` into out_transform — extracts 2D affine from a 3D matrix for a keyframe.",
    ], [
        ("0x0055226d", "DAT_005e488c", "Y-axis vector literal"),
        ("0x00552278", "_DAT_005e48a8", "angle scale (likely 1.0f)"),
        ("0x00552288", "DAT_005e4898", "-Y-axis vector literal"),
    ], [], []),

    ("0x005522e0", 92, ["FUN_00552720", "FUN_00552230"], ["FUN_00551ad0"], [
        "5-param `(parent_obj, kf_slot, time_lo, time_hi, src_data)`.",
        "Writes time_lo, time_hi into kf_slot[0..1].",
        "If parent.class_id == DAT_00623e24 and FUN_00552720(kf+2, src) succeeded: returns 0.",
        "Else if parent.class_id == DAT_00623e54: calls FUN_00552230(kf+2, src).",
        "Class-dispatch keyframe writer.",
    ], [
        ("0x005522fa", "DAT_00623e24", "translate-class id"),
        ("0x00552315", "DAT_00623e54", "rotate-class id"),
    ], [], []),

    ("0x005524a0", 32, ["FUN_00551ce0"], ["external"], [
        "1-param `(clip)`. Compute size of serialized clip = `FUN_00551ce0(*(clip+0x14)) + *(clip+4)*0x20 + 4`.",
        "FUN_00551ce0 returns 0x40 (keyframe size) -> `0x40 + clip.frame_count*0x20 + 4`.",
    ], [
        ("0x005524ac", "+0x14 (keyframe-table)", ""),
        ("0x005524b2", "+0x4 (frame count)", ""),
        ("0x005524b6", "0x20 / 4", ""),
    ], [], []),

    ("0x00552720", 43, [], ["FUN_005522e0"], [
        "2-param `(dst[6], src[14+])`. Extract 6 floats from src to dst at picks src[0], src[1], src[4], src[5], src[0xc], src[0xd].",
        "Translation-keyframe extraction (column-vector slots from a 4x4 matrix).",
    ], [
        ("0x00552723", "src[0,1,4,5,0xc,0xd]", "matrix columns picked"),
    ], [], []),

    ("0x00552890", 142, ["FUN_00552e40", "FUN_004c3dc0"], ["FUN_00553620"], [
        "0-param. Lazy-cached squared-distance computation.",
        "If `DAT_00912bec == 0`: calls FUN_00552e40 (recompute camera/view), then builds `local_c = (DAT_00912b08 / _DAT_00912b10) * _DAT_00912be4 * DAT_00912be0`, applies FUN_004c3dc0(local_c, local_c, &DAT_00912b58) (matrix-transform), and stores `local_c^2+local_8^2+local_4^2` into _DAT_00912be8.",
        "Marks DAT_00912bec=1 (cache valid). Returns _DAT_00912be8.",
        "Pixel-to-world distance² threshold for curve subdivision.",
    ], [
        ("0x005528b8", "DAT_00912b08 / _DAT_00912b10", "screen-width / scale"),
        ("0x005528bf", "_DAT_00912be4 * DAT_00912be0", "view-scale * subdivision-factor"),
        ("0x005528d3", "DAT_00912b58", "current viewport matrix"),
    ], [], []),

    ("0x00552920", 316, [
        "FUN_00552e40", "FUN_004c3d90 (matrix-batch-transform)",
        "FUN_00552fa0 (path reset)", "FUN_00553030 (move-to)",
        "FUN_005530f0 (line-to)", "FUN_005533d0 (close-path)"
    ], ["external"], [
        "1-param `(path_obj)`. Build a screen-space rectangle path matching the current viewport.",
        "Sets `local_70` to 1.0 if `*(DAT_00912c0c+0x14) == 2` else DAT_00912be0 (viewport scale).",
        "Computes 4 corner points from DAT_00912b08 (width) * DAT_00912b0c (height) scaled by 0.5 (DAT_005cc32c) and viewport-flip (DAT_005cd50c).",
        "FUN_004c3d90 transforms 4 points by DAT_00912b58 (viewport matrix), then issues: reset, move-to(0), line-to(1), line-to(2), line-to(3), close-path.",
    ], [
        ("0x00552927", "+0x14 == 2", "viewport mode (likely 'fullscreen')"),
        ("0x0055292f", "DAT_00912be0 (viewport-scale)", ""),
        ("0x00552948", "_DAT_005cd50c (= -0.5 or screen-flip)", ""),
        ("0x00552959", "_DAT_005cc32c (= 0.5f)", ""),
        ("0x00552998", "DAT_00912b58 (viewport matrix)", ""),
    ], [], []),

    ("0x00552d70", 39, [], ["external"], [
        "0-param. Decrement camera/state stack: `if (DAT_00912b04 > 0) { DAT_00912b04--; DAT_00912bd8 = 0; DAT_00912bec = 0; }`.",
        "Invalidates the cached distance threshold (DAT_00912bec) and stage-state (DAT_00912bd8) when popping a viewport.",
    ], [
        ("0x00552d70", "DAT_00912b04", "viewport stack depth"),
        ("0x00552d83", "DAT_00912bd8 / DAT_00912bec", "invalidation slots"),
    ], [], []),

    ("0x00552fa0", 129, [
        "FUN_005c4d50", "indirect via DAT_007d3ff8+0x10c (free)",
        "DAT_007d3ff8+0x11c (pool-free)", "FUN_00553e80",
        "FUN_005c4ba0"
    ], ["FUN_00552920", "FUN_00553cf0"], [
        "1-param `(path_obj)`. Reset/empty a path object in-place.",
        "If `*(path+0x14) (= subpath_list)` non-null: walks chain. Each node has buffer at +0 and tag at +4. Frees buffer (FUN_005c4d50 if tag==-1, else regular free), then returns node to DAT_00912a24 pool, then FUN_00553e80(node.children) — recursive free of sub-tree.",
        "Resets path fields: +0x14 = 0, calls FUN_005c4ba0(path[0]) on root sub-buffer, +0x8 = 0, +0xc = 1, +0x18 = path itself (cursor at root).",
    ], [
        ("0x00552fa8", "+0x14 (sub-list head)", ""),
        ("0x00552fb5", "+0x4 == -1 (large/external sentinel)", ""),
        ("0x00552fe4", "DAT_00912a24 (sub-list pool)", ""),
    ], [], []),

    ("0x00553030", 192, [
        "FUN_0055deb0 (sub-buf count?)", "FUN_005c4bb0 (sub-buf append entry)",
        "DAT_007d3ff8+0x118 (pool-alloc)", "FUN_00553f40 (sub-list register)"
    ], ["FUN_00552920", "FUN_00553620"], [
        "3-param `(path, x, y)`. Append a move-to (type=0) command at (x, y).",
        "If current sub-buffer is non-empty (FUN_0055deb0 != 0): allocates a NEW sub-list node from DAT_00912a24, initialises (sub-buf, +0x4=0, +0xc=1, +0x14=path-self, +0x10=0, +0x18=node), links it as `path.cursor.children` via FUN_00553f40, advances cursor, then recursively retries the move-to.",
        "Else: appends 6-float record `{type=0, x, y, 0, 0, 0}` via FUN_005c4bb0 onto the active sub-buffer.",
    ], [
        ("0x00553043", "FUN_005c4bb0", "appends 6 floats to sub-buffer"),
        ("0x00553055", "0x30190", "RW memory tag for path-segment"),
        ("0x00553075", "0x301a1", "RW memory tag for sub-list node"),
    ], [], []),

    ("0x005530f0", 244, [
        "FUN_0055deb0", "FUN_005c4bb0", "FUN_004c3b30 (FastSqrt C3 hook)",
        "FUN_004c3b90 (FastInvSqrt C3 hook)"
    ], ["FUN_00552920", "FUN_005533d0"], [
        "3-param `(path, x, y)`. Append line-to (type=1) and compute its unit-tangent + arc-length.",
        "Computes Δx = path.last.x - x, Δy = y - path.last.y, len² = Δx²+Δy².",
        "Uses FUN_004c3b30 (FastSqrt) for length and FUN_004c3b90 (FastInvSqrt) for normalisation. Stores: type=1, x, y, tx (unit), ty (unit), arc_length_cumulative.",
        "If previous segment exists (FUN_0055deb0 >= 2): averages this segment's tangent with the previous segment's tangent and re-normalizes (smooth join). Else: copies tangent into prev slot.",
    ], [
        ("0x005530fb", "type code = 1 (line-to)", ""),
        ("0x00553150", "+0xc/+0x10/+0x14 (prev tangent fields)", ""),
    ], [], []),

    ("0x005531f0", 469, [
        "FUN_0055deb0", "FUN_005c4c60 (batch-append n=3)",
        "FUN_004c3b30 (FastSqrt)", "FUN_004c3b90 (FastInvSqrt)"
    ], ["external"], [
        "7-param `(path, cx1, cy1, cx2, cy2, x, y)`. Append THREE consecutive segments (type=2 each) — a cubic Bezier expressed as 3 sub-segments.",
        "First segment treats (cx1, cy1) as a knot, computes unit tangent same as FUN_005530f0 (line-to). Smooths with previous if applicable.",
        "Second + third segments duplicate the same fast-norm pattern, building the polyline approximation of a Bezier control hull.",
        "Final: sets `path.cursor.flag (+0xc) = 0` (not closed).",
    ], [
        ("0x005531fb", "type code = 2 (curve-to)", ""),
        ("0x00553208", "FUN_005c4c60(.., 3, .)", "batch-allocate 3 segments at once"),
    ], [], []),

    ("0x005533d0", 441, [
        "FUN_0055deb0", "FUN_005c4d30 (first-segment getter)",
        "FUN_005c4df0 (last-segment getter)", "FUN_005c4bb0",
        "FUN_004c3b30 (FastSqrt)", "FUN_004c3b90 (FastInvSqrt)"
    ], ["FUN_00552920", "FUN_00553620"], [
        "1-param `(path)`. Close-path: if last point != first point, emit a line-to to first point.",
        "Then averages the last tangent with the first tangent (joint smoothing) and writes both fields with averaged unit vector.",
        "Sets path.cursor.flag (+0x8) = 1 (closed).",
    ], [
        ("0x005533f0", "DAT_005d757c (= 0.0f)", "tangent-equality epsilon"),
        ("0x005534f4", "+0x8 = 1 (closed)", ""),
    ], [], []),

    ("0x00553590", 134, ["FUN_005c4d50", "DAT_007d3ff8+0x10c (free)", "DAT_007d3ff8+0x11c (pool-free)", "FUN_00553e80", "FUN_005c4ba0"], ["external"], [
        "0-param. Static-target variant of FUN_00552fa0 — resets the global path `DAT_00912a28` (active path stack top).",
        "Body is a verbatim duplicate of FUN_00552fa0 with the path replaced by DAT_00912a28. Compiler likely inlined a const-target alias.",
    ], [
        ("0x00553594", "DAT_00912a28", "global path object"),
    ], [], []),

    ("0x00553620", 626, [
        "FUN_00552890 (cached threshold)", "FUN_005c4d30", "FUN_0055deb0",
        "FUN_005538a0 (recursive Bezier sub-divide)", "FUN_005c4bb0",
        "FUN_004c3b30 (FastSqrt)", "FUN_004c3b90 (FastInvSqrt)", "FUN_005533d0"
    ], ["FUN_00553cf0"], [
        "2-param `(dst_path, src_path)`. Copy src into dst with curve-segment tessellation.",
        "Refreshes _DAT_00912be8 via FUN_00552890 (squared pixel-distance threshold).",
        "Iterates src.segments. type==2 segments call FUN_005538a0 (recursive De Casteljau sub-divide) emitting line-to's into dst. type==1 segments are emitted with fast-norm tangent. type==0 (move-to) is verbatim copied (6 ints memcpy).",
        "If src.closed (+0x8 != 0): closes dst via FUN_005533d0.",
    ], [
        ("0x0055362b", "DAT_00912be8 (cached distance²)", ""),
        ("0x00553636", "src.flag == -1 (large/external indicator)", ""),
        ("0x005536c2", "DAT_00912bf0 (max subdivision depth)", ""),
    ], [], []),

    ("0x005538a0", 936, [
        "FUN_0055deb0", "FUN_005c4bb0",
        "FUN_004c3b30 (FastSqrt)", "FUN_004c3b90 (FastInvSqrt)"
    ], ["FUN_00553620", "FUN_005538a0 (self)"], [
        "3-param `(dst_path, src_4_points[8], depth)`. Recursive cubic-Bezier subdivision (De Casteljau).",
        "Base case: `depth-1 < 0` -> emit single line-to to final control-point (P3) and exit.",
        "Flatness test (subdivision-needed): for both intermediate control points P1 and P2, compute perpendicular squared distance from chord P0->P3; if either exceeds `_DAT_00912be8` -> subdivide.",
        "Subdivide: classic De Casteljau midpoint formula using `_DAT_005cc32c (= 0.5f)`. Recurse twice into two halves with `depth-1`.",
        "Flat case: emit one line-to to P3 with smooth-tangent join.",
    ], [
        ("0x005538ad", "_DAT_00912be8", "distance² tolerance from FUN_00552890"),
        ("0x005539d2", "_DAT_005cc32c (= 0.5f)", "midpoint blend factor"),
    ], [], []),

    ("0x00553c50", 157, [], ["FUN_00553cf0"], [
        "2-param `(path*, bbox[4])`. Compute axis-aligned bounding box of a single path.",
        "Walks segments at `path[0]` (offset by 0xc-floats each). For each, updates `bbox[0..1]` (min) and `bbox[2..3]` (max) for x and y.",
        "If `path[1] == -1` (external buffer), substitutes via FUN_005c4d30.",
    ], [
        ("0x00553c5c", "path[1] == -1 indirect-buf sentinel", ""),
        ("0x00553c81", "+8 stride (3 floats = 12 bytes per segment header)", ""),
    ], [], []),

    ("0x00553cf0", 262, [
        "FUN_005c4d30", "FUN_0055deb0", "FUN_005c4d50",
        "DAT_007d3ff8+0x10c (free)", "DAT_007d3ff8+0x11c (pool-free)",
        "FUN_00553e80", "FUN_005c4ba0", "FUN_00553620", "FUN_00553c50"
    ], ["external"], [
        "2-param `(path*, bbox[4])`. Compute total bounding box of a path tree.",
        "Seeds bbox from path[0]'s first point. Walks sub-list (path[5] chain).",
        "Per node: if `node[3] == 0` (flag — needs flattening), resets temporary path DAT_00912a28 (same body as FUN_00552fa0/FUN_00553590), calls FUN_00553620(DAT_00912a28, node) to flatten curves, then FUN_00553c50(DAT_00912a28, bbox).",
        "Else FUN_00553c50(node, bbox) directly.",
        "Finally converts bbox to (min_x, min_y, w, h): subtracts min from max in slots [2,3].",
    ], [
        ("0x00553d18", "node[3] (needs-flatten flag)", ""),
        ("0x00553d6f", "DAT_00912a28 (temp work path)", ""),
        ("0x00553def", "bbox[2] -= bbox[0]; bbox[3] -= bbox[1]", "convert to width/height"),
    ], [], []),

    ("0x00553e00", 115, ["FUN_0055deb0", "FUN_005c4d50", "DAT_007d3ff8+0x10c (free)", "DAT_007d3ff8+0x11c (pool-free)"], ["FUN_00553e00 (self)"], [
        "1-param `(node)`. Recursive prune of singly-linked sub-list (rooted at node[5]).",
        "Recurses into node[5] first; replaces node[5] with the result.",
        "If `FUN_0055deb0(node[0]) == 1` (sub-buffer is degenerate / single-point): frees node's buffer (via FUN_005c4d50 if tag==-1 else free), zeros node[0], returns to DAT_00912a24 pool, and returns the next sub-list link (effectively unlinks node).",
        "Else returns node unchanged.",
    ], [
        ("0x00553e1f", "DAT_00912a24 (sub-list pool)", ""),
    ], [], []),

    ("0x00553e80", 100, ["FUN_005c4d50", "DAT_007d3ff8+0x10c (free)", "DAT_007d3ff8+0x11c (pool-free)", "FUN_00553e80 (self)"], ["FUN_00552fa0", "FUN_00553590", "FUN_00553cf0", "FUN_00553e80 (self)"], [
        "1-param `(node)`. Recursive free of an entire sub-list chain.",
        "Per node: stash node[5] (next), free buffer at node[0] (FUN_005c4d50 or regular free per node[1]==-1 sentinel), zero node[0], pool-free the node, then self-recurse on the stashed next.",
        "Returns `param_1 != 0` (was the input non-null?).",
    ], [
        ("0x00553eb6", "DAT_00912a24 (pool)", ""),
    ], [], []),

    ("0x00553ef0", 72, ["DAT_007d3ff8+0x118 (pool-alloc)", "FUN_00553f40 (sub-list register)"], ["external"], [
        "0-param. Allocate a fresh empty sub-list head node from DAT_00912a24 pool (tag 0x301a1).",
        "Initialises 7 fields: +0=0, +4=0, +8=0, +0xc=1, +0x14=0, +0x18=self, +0x10=0. Calls FUN_00553f40(node) to register.",
        "Returns the new node.",
    ], [
        ("0x00553ef9", "DAT_00912a24", "sub-list pool"),
        ("0x00553f0c", "0x301a1", "RW memory tag"),
        ("0x00553f1a", "+0xc = 1 (initial subdivision flag)", ""),
        ("0x00553f25", "+0x18 = self (cursor anchor)", ""),
    ], [], ["[STUB] FUN_00553f40 — sub-list registration helper (next-bucket-up)."]),
]

def write_plate(rva, size, callees, callers, mech, consts, uncerts, stubs):
    fname = OUT / f"{rva}.md"
    cs = ", ".join(callees) if callees else ""
    rs = ", ".join(callers) if callers else ""
    lines = []
    lines.append("---")
    lines.append(f"rva: {rva}")
    lines.append(f"name_in_ghidra: FUN_{rva[2:]}")
    lines.append(f"size_bytes: {size}")
    lines.append("confidence: C1")
    lines.append(f"callees_depth1: [{cs}]")
    lines.append(f"callers_noted: [{rs}]")
    lines.append(f"opened_in_slot: {SLOT}")
    lines.append(f"session_date: {DATE}")
    lines.append("---")
    lines.append("")
    lines.append("## Mechanical description")
    lines.append("")
    for m in mech:
        lines.append(f"- {m}")
    lines.append("")
    lines.append("## Constants")
    lines.append("")
    if consts:
        lines.append("| Address | Value | Note |")
        lines.append("|---|---|---|")
        for a, v, n in consts:
            lines.append(f"| {a} | `{v}` | {n} |")
    else:
        lines.append("(none — all operations on parameter / dereferenced struct fields)")
    lines.append("")
    lines.append("## Uncertainties")
    lines.append("")
    if uncerts:
        for u in uncerts:
            lines.append(f"- {u}")
    else:
        lines.append("(none above C1 — mechanical-only plate)")
    lines.append("")
    lines.append("## Stubs encountered")
    lines.append("")
    if stubs:
        for s in stubs:
            lines.append(f"- {s}")
    else:
        lines.append("(none — all callees named or trivial RW-table indirects)")
    lines.append("")
    fname.write_text("\n".join(lines), encoding="utf-8")

count = 0
for p in PLATES:
    write_plate(*p)
    count += 1
print(f"Wrote {count} plates to {OUT}")
