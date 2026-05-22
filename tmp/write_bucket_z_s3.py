#!/usr/bin/env python3
"""Batch-write the remaining 68 C1 plates for bucket_0052df70 (batch_z session 3).
Each entry: (rva, size, callees, callers, body) where body is the per-plate
mechanical description (Markdown bullet block).
"""
import os
import textwrap

BUCKET = r"re/analysis/bucket_0052df70"
SLOT = "Mashed_pool11"
DATE = "2026-05-19"

# entries: list of (rva_hex, size_bytes, callees_list, callers_list, mech_desc_md,
#                   constants_table_rows, uncertainties_list, stubs_list)
ENTRIES = [
    ("0x00530000", 14, [], ["FUN_0052e650"],
     "- Single store: `*(undefined4*)(param_1 + 0x6c) = DAT_007dc524` (current default pipeline handle into instance).",
     [("0x007dc524", "global pipeline slot")],
     [],
     []),

    ("0x00530010", 60, [], [],
     "- Feature-gate query: returns 1 iff `DAT_007dc540 != 0` AND (`DAT_007dc544 == 0` OR resource at `DAT_007dc508+*(int*)(param_1+0x18)` is null OR `DAT_007dc548 < *(uint*)(handle+0x1c)`).\n- Used as a render-path eligibility check.",
     [("0x007dc540", "feature flag (skin enabled)"),
      ("0x007dc544", "feature flag (high quality)"),
      ("0x007dc548", "capability threshold")],
     ["- [UNCERTAIN] meaning of capability `+0x1c` field — likely vertex/blend-index count."],
     []),

    ("0x00530050", 60, [], [],
     "- Inverse predicate of FUN_00530010: returns 1 iff `DAT_007dc540 != 0` AND `DAT_007dc544 != 0` AND handle != 0 AND `*(uint*)(handle+0x1c) <= DAT_007dc548`.\n- Returns 0 otherwise.",
     [("0x007dc540", ""), ("0x007dc544", ""), ("0x007dc548", "")],
     [],
     []),

    ("0x00530090", 56, ["(free at DAT_007d3ff8+0x10c)"], [],
     "- Frees field at `param_1+0x30` via vtable free, zeros offsets 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38 on the instance.\n- Returns 1. Acts as cleanup of a sub-buffer plus 6-dword tail.",
     [],
     [],
     ["- [STUB] vtable free (DAT_007d3ff8+0x10c)."]),

    ("0x005300d0", 137, ["FUN_004cc770", "FUN_004cbe80"], [],
     "- Stream-write: writes `param_2[9]`, `param_2[10]`, `param_2[11]` (4 bytes each).\n- If `param_2[10] != 0`: writes raw bytes via `FUN_004cbe80(stream, param_2[12], *param_2 + (param_2[10]+param_2[11])*2)`.\n- Returns stream on success, 0 on any failure.\n- Per-field RW chunk serialization for a 16-byte attribute structure.",
     [],
     [],
     []),

    ("0x00530160", 349, ["FUN_004cc790", "FUN_004cbd30", "(free/alloc at DAT_007d3ff8+0x108/0x10c)"], [],
     "- Stream-read of a 0xc-byte header + variable payload.\n- Reads three uint32 (local_c, local_14, local_10).\n- If `0 < local_14` (count): frees any existing payload at `param_2[12]`, zeros 5 dwords at offsets 9..14, allocates `local_8 = *param_2 + (local_14+local_10)*2` bytes, zero-fills, restores layout fields and reads body via FUN_004cbd30.\n- Used to read variable-sized vertex/face attribute payload from a clump stream.",
     [],
     [],
     []),

    ("0x005302c0", 28, [], [],
     "- Size accessor: returns 0xc if `param_1[10] == 0`, else `*param_1 + 0xc + (param_1[11] + param_1[10])*2`.\n- Computes serialized footprint of the structure.",
     [],
     [],
     []),

    ("0x00530650", 1453, [
        "FUN_004f8660", "FUN_004f57e0", "FUN_004f5030", "FUN_004f7850", "FUN_004f79f0",
        "FUN_004f71f0", "FUN_004f77f0"
    ], [],
     "- Vertex stream rebuild for the CSL skin atomic pipeline.\n- Iterates `auStack_1a4` array of declaration-element descriptors keyed on byte tag at +2/+3 (e.g., `\\x11`=END, `\\x01`=POSITION, `\\x02`=NORMAL, `\\x03`=COLOR, `\\x05`=TEXCOORD, `\\x06`=TANGENT, `\\x0a`=BLENDINDICES).\n- For each enabled stream (per `param_4` mask bits): locks the vertex buffer via `(*(code*)((**(int**)(param_2+0x34)+0x10)))` (RpAtomicLock pattern).\n- Writes positions (FUN_004f57e0), morph-targets (FUN_004f5030 with morph weight clamp via _DAT_005cc320), bones+weights (FUN_004f7850, palette resolution via FUN_004f4200 in parent), and conditionally lighting/skinning post-pass (FUN_004f79f0, FUN_004f71f0, FUN_004f77f0).\n- Unlocks all locked streams at end (`(**(code**)(*piVar11+0x30))(piVar11)`).",
     [],
     ["- [UNCERTAIN] callees FUN_004f57e0/5030/7850/79f0/71f0/77f0 not in this bucket; semantic naming based on D3D9 stream-write parameter shape (count, stride, source buffer, dest offset)."],
     []),

    ("0x00530c00", 1810, ["FUN_004cbb60", "FUN_004f8660", "FUN_004f4200", "FUN_004f8490", "FUN_004f5020", "FUN_00530010"], [],
     "- D3D9 vertex declaration builder for the CSL skin atomic pipeline. Sets up `local_94..local_8c` element array.\n- For each enabled feature (POSITION=2, NORMAL=10/4-ubyte4 depending on caps, TEXCOORD=3, BLENDINDICES=5/byte+index, COLOR=6/7) builds `D3DVERTEXELEMENT9` entries: stream-index, offset, type (4=FLOAT3, 0xc=UBYTE4, 0xa=UBYTE4N, etc.), method, usage, usage-index.\n- Terminates with `\\xff,0,0,0x11,0,0` (D3DDECL_END).\n- Calls FUN_004cb8a0 to compile, then per-stream allocates vertex buffer regions and locks them via FUN_004dc5b0 / FUN_004dcaa0 / FUN_004dc750 / FUN_004dcb80.\n- Returns 1 on success.",
     [("0x007d3ff8+0xd4", "device caps bit (3-bone skinning quality)"),
      ("0x007d3ff8+0xec", "device caps bit (vertex shader version)")],
     [],
     []),

    ("0x00532000", 171, [], [],
     "- Batch matrix*vec3 transform: `output[i] = matrix * input[i] + translation`.\n- param_1 = count, param_2 = 4x4 matrix (column-major 16 floats), param_3 = output stream (stride param_7), param_4 = input vec3 stream, param_5 = secondary output (optional, e.g. normals — no translation applied), param_6 = secondary input (optional).\n- Encodes pair of `xyz = M.col0*vx + M.col1*vy + M.col2*vz + M.col3 (positions) or no col3 (normals).",
     [],
     [],
     []),

    ("0x005320b0", 220, [], [],
     "- Indexed-matrix variant of FUN_00532000.\n- param_2 = per-vertex byte index into matrix palette `param_3`.\n- For each vertex: fetches matrix `*(float*)(param_3 + bVar5*0x40)` (64-byte matrix stride), transforms position+normal.\n- This is the single-bone skinning fast path.",
     [],
     [],
     []),

    ("0x005327d0", 212, ["FUN_004cbb60", "FUN_004d4170", "FUN_004d4dd0", "FUN_004d4f90", "FUN_004d4380", "FUN_004d41e0"], [],
     "- Constructs and registers the `nodeD3D9SkinAtomicAllInOne_csl` pipeline.\n- Reads device caps via FUN_004cbb60(): sets `DAT_007dc540 = caps[0x1e] & 1` (skinning supported), if vertex-shader >= 1.01 and pixel-shader >= 1.01 sets DAT_007dc544=1 and computes max blend bones via `(caps[0xc8]-12)/3`.\n- Creates pipeline via FUN_004d4170/4dd0, attaches CSL string `&PTR_s_nodeD3D9SkinAtomicAllInOne_csl_00623c30`, finalizes via FUN_004d4380.\n- Returns the new pipeline handle, or 0 on failure (with cleanup via FUN_004d41e0).",
     [("0x00623c30", "PTR_s_nodeD3D9SkinAtomicAllInOne_csl"),
      ("0x007dc540/544/548/54c", "skinning capability state")],
     ["- [UNCERTAIN] caps offsets 0x1e/0xc4/0xc8/0xcc derive from a Direct3D-style D3DCAPS9 struct cached at FUN_004cbb60."],
     []),

    ("0x00532a60", 281, [], [],
     "- Quaternion-to-4x4-matrix conversion.\n- Reads quaternion `qx, qy, qz, qw` from `(param_2+0x8/0xc/0x10/0x14)`.\n- Writes 4x4 matrix into `param_1[0..15]` (row-major float). Standard `1-2(y^2+z^2)`, `2(xy+wz)`, etc.\n- Last column copied from `(param_2+0x18..0x20)` (translation).\n- `param_1[3]` written as `4.2039e-45` (= 0x00000003, w-tag).",
     [("0x005cc320", "1.0f")],
     [],
     []),

    ("0x00532b80", 1043, ["FUN_004c3b30"], [],
     "- Quaternion SLERP plus linear translation.\n- param_1 = output keyframe, param_2 = key A pointer, param_3 = key B pointer, param_4 = blend t.\n- Computes dot product fVar8 of two quaternions; negates B if dot < 0.\n- Linear translation: `out[6..8] = lerp(A[6..8], B[6..8], t)` at offsets 0x18/0x1c/0x20.\n- If `|dot| < threshold`: computes acos via polynomial-Padé approximation (DAT_005d8dcc..0x5d8dec coefficients), divides into two sin-fractions, calls FUN_004c3b30 (FastSqrt) once. Result yields sin(t*theta)/sin(theta) blend weights.\n- Else: identity LERP weights (1-t, t).\n- Writes blended quaternion to `param_1+0x8..0x14`.",
     [("0x005cc320", "1.0f"),
      ("0x005cc32c", "approx constant"),
      ("0x005d8dcc..0x005d8dec", "acos/sin Padé coefficients"),
      ("0x005cc8f4..0x005cc908", "sin polynomial coefficients")],
     ["- [UNCERTAIN] threshold _DAT_005cea88 is the SLERP-vs-NLERP cutoff (~0.99); literal value not chased."],
     []),

    ("0x005333a0", 116, ["FUN_004cc790"], [],
     "- Reads `*(int*)(param_2 + 4)` entries; each entry = 36 bytes from stream (`FUN_004cc790(stream, piVar4+1, 0x20)` plus a 4-byte tag).\n- Fixes up internal pointer: `*piVar4 = piVar1 + (param_2 / 0x24) * 9` (i.e., 9 ints = 36 bytes per entry — element index resolution).\n- Returns the buffer or 0 on failure.",
     [],
     [],
     []),

    ("0x00533d00", 247, ["FUN_0052ddf0", "(alloc at DAT_007d3ff8+0x108/0x118)"], [],
     "- Allocates a 9-dword instance header via `(**(code**)(DAT_007d3ff8+0x118))(DAT_0091246c, 0x3011e)`.\n- Stores: [0]=param_4 (flags), [1]=param_1 (count), [4]=alloc of `param_1<<4` bytes (per-element 16-byte attribute), [8]=FUN_0052ddf0(param_1, param_5) result.\n- If `(param_4 & 2) == 0`: also alloc `param_1*0x40 + 0xf` bytes (16-byte aligned matrix region) at [3], aligned base at [2]; else both zero.\n- Inits each 16-byte element with: source dword from param_2 stream, ordinal index, optional value from param_3 stream.\n- [6] = self-pointer.",
     [],
     [],
     []),

    ("0x00533ec0", 2467, ["FUN_004c0b10", "FUN_004c0ed0", "FUN_004c4600", "FUN_00532a60", "FUN_004c0e50"], [],
     "- Per-frame matrix-palette assembly for the skin atomic.\n- Two main paths driven by `param_1[0]` flag bits 0x2/0x1000/0x2000/0x4000:\n  - Path A (flag 2): builds local_980 by concatenating parent transforms via FUN_004c4600 (matrix multiply chain) starting from FUN_004c0ed0.\n  - Path B (flag 1): copies world matrix; optionally seeds an identity (flag 0x4000).\n- For each bone (`param_1[1]` count):\n  - If bone's transform fn (`*(code*)(param_1[8]+0x3c)`) is FUN_00532a60 (quaternion node), inlines the q→mat4 conversion (literal copy of FUN_00532a60).\n  - Else calls it as a function pointer.\n  - Multiplies bone-local matrix by accumulated root via FUN_004c4600 into local_8c0.\n  - Writes to `(bone+0x50)` instance slot, sets `*(byte*)(bone+3) |= 8 & ~4` (dirty flag).\n- Stack discipline: 16-matrix scratch local_7c0 used as a push/pop stack (uVar11 == 1: push, uVar11 == 2: pop+swap).\n- Returns 1.",
     [],
     ["- [UNCERTAIN] flag bits 0x2/0x1000/0x2000/0x4000 distinguish 'frustum-skin' from 'fixed-frame skin' variants; correspondence to RW skin flags not chased here."],
     []),

    ("0x00535330", 859, ["(alloc at DAT_007d3ff8+0x108)"], [],
     "- Vertex format allocator. param_2 = feature bitmask:\n  - bit 0 (0x01) = POSITION (size 12 = vec3)\n  - bit 1 (0x02) = NORMAL/extra (size 4)\n  - bit 2 (0x04) = TEXCOORD0 (size 8 = vec2)\n  - bit 3 (0x08) = MATRIX (size 64 = 4x4)\n  - bit 4 (0x10) = NORMAL3 (size 12)\n  - bit 5 (0x20) = COLOR? (size 4)\n  - bit 6 (0x40) = SECONDARY (size 16)\n  - bit 7 (0x80) = TANGENT? (size 16)\n  - bit 8 (0x100) = MORPH (size 32)\n- Sums sizes (× `*param_1` count), allocates `total + 0x10` bytes (allocator tag 0x3012f), 16-byte aligns.\n- Sets `param_1[0x26]` = feature count, `param_1[0x28]` = mask, `param_1[3]` = alloc base.\n- For each active feature: stores per-feature aligned base+offset into `param_1[0x13..0x24]` (pair of `int* size_per_vertex`).\n- Returns 1 on success, 0 on alloc failure.",
     [],
     [],
     []),

    ("0x005356f0", 5, ["FUN_004e6920"], [],
     "- Thunk forwarder to FUN_004e6920 (5 bytes = a JMP instruction).\n- Ghidra labelled it `thunk_FUN_004e6920`. Listing decompilation comes from FUN_004e6920's body itself (resource release: clears RW string symbol DAT_00618664, frees fields, calls FUN_004e8ea0/FUN_004e4440, releases via vtable[0x11c]).",
     [],
     ["- [UNCERTAIN] only 5 bytes — pure thunk. The shown body is the *target* function not the thunk."],
     []),

    ("0x00538310", 329, ["FUN_004c3ac0"], [],
     "- BVH node split (recursive 2-way tree build).\n- If `*param_1 >= 0` (internal node): builds two child boxes by splitting the parent (param_2) box on `*param_1` axis. Then recurses on both children.\n- If `*param_1 < 0` (leaf): writes 12 dwords into the leaf at `(int)param_1 + DAT_00912488 + 0x1c..0x4c`: copies parent box (parent_min/parent_max), node-local box (param_1[0x12..0x17]), unioned box, and finally computes the leaf bounding diagonal magnitude via FUN_004c3ac0(&local_3c) (Vec3Magnitude) into +0x4c.",
     [("0x00912488", "DAT_00912488 (cached offset)")],
     [],
     []),

    ("0x00538a80", 19, [], [],
     "- Sets `DAT_00912480 = param_1` and returns `*(undefined4*)(DAT_00912484 + param_1)`.\n- One-liner getter that stashes context pointer in a global before reading from another global-indexed table.",
     [("0x00912480", "context slot"),
      ("0x00912484", "table base")],
     [],
     []),

    ("0x00538b70", 40, ["(free at DAT_007d3ff8+0x10c)"], [],
     "- Freelist walker: while `*param_1 != 0`, advance head via `*param_1 = *(int*)(iVar1 + 0x58)` then free node.\n- Drains a singly-linked list with next-pointer at offset 0x58.",
     [],
     [],
     []),

    ("0x00538ba0", 40, ["(free at DAT_007d3ff8+0x10c)"], [],
     "- Same shape as FUN_00538b70 but the next-pointer is at offset 0xc.\n- Drains a different free-list.",
     [],
     [],
     []),

    ("0x00538bd0", 88, ["RpWorldForAllWorldSectors", "FUN_004d7ff0", "FUN_004d8480"], [],
     "- World-sector enumeration entry. If `param_1 != 0`: calls `RpWorldForAllWorldSectors(param_1, &LAB_00538c30, &param_1)` (RW iterator) and returns param_1 (mutated by callback).\n- If `param_1 == 0`: reports error 0x80000016 via FUN_004d7ff0/4d8480 and returns -1.\n- Subsystem confirmed RenderWare world traversal.",
     [],
     [],
     []),

    ("0x00538d60", 2961, ["FUN_00547bf0", "FUN_004c3b90"], [],
     "- Ray-vs-BVH-vs-triangle intersection over a RpWorld using the game's collision BVH.\n- Initializes box-clip helpers `local_340[0..0xb]` from ray origin/direction (+ 1/dir, + ABS-of-dir).\n- Outer stack-machine traversal of BVH (local_304/local_310) using axis-aligned slab tests: for each BVH node `*piVar7`, slab-clips ray on axis `*piVar7` to choose near/far child, pushes far if ray spans split.\n- When a leaf is reached, walks the leaf's triangle list (`*(int*)(DAT_007dc5b4 + leaf)` index buffer, `*(int*)(leaf+4)` indices, `*(int*)(leaf+8)` vertices).\n- Inner traversal: per-triangle Möller-Trumbore-style ray-tri intersection. Hit calls `(*param_3)(param_2, leaf_id, normal_outptr, t, param_4)`. Normal computed via cross-product + FUN_004c3b90 (FastInvSqrt) renorm.",
     [("0x005e4574", "epsilon for det>0"),
      ("0x005cea1c", "1.0 - epsilon (relative tolerance)"),
      ("0x007dc5b4", "global offset to BVH index buffer")],
     [],
     []),

    ("0x00539900", 1469, ["FUN_00547bf0", "FUN_004c3b90"], [],
     "- AABB-vs-BVH variant of FUN_00538d60.\n- param_2 is a 6-float AABB (min[0..2], max[3..5]).\n- Outer BVH walk: tests each node's split plane vs AABB extents; descends children that overlap.\n- Inner: tests each leaf-triangle vs box via FUN_00547bf0 (tri-box overlap). On overlap, computes triangle normal, calls `(*param_3)(param_2, leaf_id, normal_outptr, 0, param_4)`.",
     [],
     [],
     []),

    ("0x00539ec0", 2033, ["FUN_00547450", "FUN_004c3b90"], [],
     "- Sphere-vs-BVH variant.\n- param_2 = sphere {center.x, .y, .z, radius}.\n- Expands sphere to AABB for the outer BVH walk; uses inflation `_DAT_005e4578 * radius`.\n- Inner: FUN_00547450(sphere, p1, p2, p3, normal_out, depth_out) does the sphere-tri test (returns hit normal + penetration depth).\n- Hits invoke `(*param_4)(param_3, leaf_id, normal_out, depth / radius, param_5)`.",
     [("0x005e4578", "sphere AABB inflation factor"),
      ("0x005cc320", "1.0f")],
     [],
     []),

    ("0x0053a6c0", 203, ["FUN_0053a7a0", "FUN_0053bdf0", "FUN_0053b7b0", "RpAtomicGetWorldBoundingSphere"], [],
     "- Atomic-level collision dispatch by `param_2[6]` (bound type enum):\n  - 1: ray → FUN_0053a7a0\n  - 3: AABB → FUN_0053bdf0\n  - 4: cylinder? → FUN_0053b7b0\n  - 5: atomic-with-sphere-extract → reads RpAtomicGetWorldBoundingSphere(param_2[0]) into a local sphere, then FUN_0053bdf0.",
     [],
     ["- [UNCERTAIN] enum value 4 — not 100% confirmed cylinder."],
     []),

    ("0x0053a7a0", 4108, ["FUN_004c3b90"], [],
     "- Ray-vs-atomic (per-RpAtomic mesh BVH).\n- If atomic has no BVH (`DAT_007dc5b0 + param_1 == 0`): brute-force ray-vs-triangle loop on `*(int*)(param_1+0x10)` triangles.\n- Else: full BVH walk (similar to FUN_00538d60 but with atomic-local indices) plus AABB slab clamp first (rejects whole atomic).\n- Hit: `(*param_4)(param_3, normal_out, t/det, param_5)` (no leaf_id since atomic is the leaf).",
     [],
     [],
     []),

    ("0x0053b7b0", 1590, ["FUN_00547bf0", "FUN_004c3b90"], [],
     "- AABB-vs-atomic counterpart (mirrors FUN_00539900 but at atomic granularity).\n- Same outer BVH walk; inner tri-box test via FUN_00547bf0.\n- Hits invoke `(*param_3)(param_2, normal_out, 0, param_4)`.",
     [],
     [],
     []),

    ("0x0053bdf0", 2013, ["FUN_00547450", "FUN_004c3b90"], [],
     "- Sphere-vs-atomic counterpart (mirrors FUN_00539ec0).\n- Same expansion + BVH walk; inner via FUN_00547450 returning normal+depth.\n- Hits: `(*param_4)(param_3, normal_out, depth/radius, param_5)`.",
     [],
     [],
     []),

    ("0x0053c5d0", 266, ["FUN_0053a7a0", "FUN_0053c6e0", "FUN_0053ccc0", "FUN_004c0ed0", "FUN_004c4dc0", "FUN_004c3d90", "RpAtomicGetWorldBoundingSphere"], [],
     "- Animated/morphed atomic collision dispatch.\n- Switch on `param_2[6]`:\n  - 1: builds local sphere from atomic LTM (FUN_004c0ed0→FUN_004c4dc0→FUN_004c3d90); if `*(int*)(iVar1+0x18) > 1` calls FUN_0053c6e0 (morph), else FUN_0053a7a0.\n  - 3: AABB → FUN_0053ccc0.\n  - 5: atomic→sphere via RpAtomicGetWorldBoundingSphere → FUN_0053ccc0.",
     [],
     [],
     []),

    ("0x0053c6e0", 1490, ["FUN_004c3b90"], [],
     "- Ray-vs-morphed-atomic (vertex-interpolated triangles).\n- param_2 = morph descriptor: `param_2[+4]`/`+6` = source/target morph target indices, `param_2[+0xc]`*`+0x10` = blend weight.\n- For each triangle: looks up the 3 vertices from both morph targets and lerps them by `local_80 = morph_weight`. Then standard ray-tri.\n- Same hit-call shape as FUN_0053a7a0.",
     [],
     [],
     []),

    ("0x0053ccc0", 189, ["FUN_004c0ed0", "FUN_004c4dc0", "FUN_004c3d90", "FUN_004c3b30", "FUN_0053cd80", "FUN_0053bdf0"], [],
     "- Sphere-vs-atomic-with-morph (mirror of FUN_0053c5d0 for sphere case).\n- Transforms sphere into local atomic frame via FUN_004c3d90, scales radius by atomic-LTM scale factor via FastSqrt (FUN_004c3b30) of `dot(scale_row0, scale_row0)`.\n- If atomic has morph targets (`*(int*)(iVar1+0x18) > 1`): calls FUN_0053cd80, else FUN_0053bdf0.",
     [],
     [],
     []),

    ("0x0053cd80", 694, ["FUN_00547450", "FUN_004c3b90"], [],
     "- Sphere-vs-morphed-atomic. param_3 = sphere, params 4/5/6 = callback context.\n- Per triangle: lerps each vertex between source and target morph buffers using `param_2[0x10]*param_2[0xc]` (weight).\n- Calls FUN_00547450 (sphere-tri test), invokes `(*param_5)(param_4, normal_out, depth*invRadius, param_6)`.",
     [],
     [],
     []),

    ("0x0053d040", 18, [], [],
     "- Setter: `*(undefined4*)(DAT_007dc5b0 + param_1) = param_2` (stores BVH root pointer for an atomic).",
     [("0x007dc5b0", "BVH root table base")],
     [],
     []),

    ("0x0053d060", 41, ["FUN_004522d0"], [],
     "- Releases BVH at `*(int*)(DAT_007dc5b0 + param_1)` via FUN_004522d0 and zeros the slot. Returns param_1.",
     [],
     [],
     ["- [STUB] FUN_004522d0 (BVH destroy)."]),

    ("0x0053d090", 20, [], [],
     "- Predicate: returns `*(int*)(DAT_007dc5b0 + param_1) != 0` — \"has BVH\".",
     [],
     [],
     []),

    ("0x0053d200", 38, ["FUN_004522d0"], [],
     "- Variant of FUN_0053d060 that takes both `param_1` (base) and `param_2` (offset) explicitly.\n- Releases `*(int*)(param_1 + param_2)` and zeros it.",
     [],
     [],
     []),

    ("0x0053d400", 85, ["FUN_0053d460", "FUN_004522d0", "FUN_0053d040"], [],
     "- BVH-build dispatcher: calls FUN_0053d460 with the atomic's vertex/face/index buffers (offsets 0x14/0x10/0x2c).\n- On success, releases old BVH then installs new via FUN_0053d040.\n- Returns 0 on failure.",
     [],
     [],
     []),

    ("0x0053d460", 318, ["FUN_0053dba0", "(alloc/free at DAT_007d3ff8+0x108/0x10c)", "FUN_004d7ff0", "FUN_004d8480"], [],
     "- Inner BVH-build orchestrator.\n- If `*param_7 & 1 == 0`: direct FUN_0053dba0(param_2, param_3, param_4, param_5, 0).\n- Else: allocates a `param_4*10` byte scratch (alloc tag 0x1011d), backs up param_6 indices, builds BVH, then re-orders `param_6` by the BVH's emitted index permutation, calls user post-callback at `*(code**)(param_7+4)` if non-null, then frees scratch.",
     [],
     [],
     []),

    ("0x0053d5a0", 231, ["(alloc at DAT_007d3ff8+0x108)", "FUN_004d7ff0", "FUN_004d8480"], [],
     "- Allocates a packed BVH-node buffer. param_1 = vertex count, param_2 = leaf count, param_3 = 6-dword header, param_4 = flags.\n- Size: `0x28` if param_2==0, else `param_2*0x10 + 0x37`; +`param_1*2 + 1` if `(param_4 & 1)`.\n- Stores `param_2` at +0x1e, `*param_1` flags, `*(ushort*)(&header+0x1c) = param_1`, copies 6-dword header into +0..0x18.\n- Returns base pointer.",
     [],
     [],
     []),

    ("0x0053d690", 33, [], [],
     "- Size predicate: `(node[0x1e] + 3) * 0x10` (size of leaves+header); + `node[0x1c]*2` if `(node[0]&1)`.",
     [],
     [],
     []),

    ("0x0053d6c0", 342, ["FUN_004cc580", "FUN_004cbe80", "FUN_004cc770"], [],
     "- Stream-write of a BVH chunk.\n- Writes RW chunk header (type 0x2c, payload size from FUN_0053d690, format 0x37002).\n- Inner chunk type 1, body via FUN_004cbe80(stream, &local_24, 0x24) (the 36-byte header).\n- Per leaf: writes 16-byte node payload.\n- If `node[0]&1`: writes `node[1c]*2` bytes of index buffer at `node[0x24]`.",
     [],
     [],
     []),

    ("0x0053d820", 244, ["FUN_004cc5e0", "FUN_004cbd30", "FUN_0053d5a0", "FUN_004cc790"], [],
     "- Stream-read of a BVH chunk (counterpart of FUN_0053d6c0).\n- Reads header into local_24..local_4 (36 bytes), allocates buffer via FUN_0053d5a0, reads per-leaf payload + optional index buffer.\n- On any failure path: frees the allocation via the vtable free and returns 0.",
     [],
     [],
     []),

    ("0x0053d920", 639, ["FUN_0053d5a0", "FUN_004cc050", "FUN_004cc790", "FUN_004d7ff0", "FUN_004d8480"], [],
     "- BVH stream-read for an alternate (legacy?) format with explicit child pointers.\n- Allocates via FUN_0053d5a0 with `param_4=1` (with index buffer).\n- Per node reads 4 dwords (axis+mid, child0, child1, leaf bounds).\n- Builds stack of pending pointers in `local_88` while parsing children; resolves once child indices known.\n- Sets `*param_5 = 1` if leaf-count exceeds 0xef (overflow signal).\n- Reads index buffer at end if `node[0]&1`.",
     [],
     [],
     []),

    ("0x0053dba0", 679, [
        "FUN_004c1800", "FUN_004c1840", "FUN_0053dea0", "FUN_0053e7f0", "FUN_0053d5a0",
        "FUN_0053de50", "FUN_0053e790"
    ], [],
     "- BVH builder entry. param_1 = vertex count, param_2 = vertex buffer base, param_3 = triangle count, param_4 = index buffer base, param_5 = pre-allocated index-permutation buffer or 0.\n- Allocates a 60-byte work struct: vertex AABBs (3*4 bytes per), per-vertex flags (4 bytes per), triangle->vertex pointer table.\n- Initializes per-vertex AABBs via FUN_004c1800/4c1840 (extend-by-vertex calls).\n- Calls FUN_0053dea0 (recursive SAH split) to build the tree, FUN_0053e7f0 (linearize tree depth), FUN_0053d5a0 to allocate the final packed buffer, then walks the linearized tree filling leaves and writing the index permutation back.\n- Frees scratch on exit.",
     [],
     [],
     []),

    ("0x0053de50", 66, ["(free at DAT_007d3ff8+0x10c)"], [],
     "- Recursive depth-first free of the temp BVH tree. Leaves identified by `*node < 0`.",
     [],
     [],
     []),

    ("0x0053dea0", 1416, [
        "FUN_005c24e0", "FUN_0053e690", "FUN_0053e430", "FUN_0053e5d0", "FUN_0053e730",
        "FUN_0053e760", "FUN_0053de50", "FUN_004d7ff0", "FUN_004d8480"
    ], [],
     "- SAH (Surface Area Heuristic) BVH split for the in-build tree.\n- If `param_1[0xb] < 4` (leaf threshold): allocates a 2-dword leaf node via vtable allocator and tags `*node = -1`.\n- If `param_1[4] >= 0x20` (max depth reached): same leaf fallback via FUN_0053e730.\n- Else: builds list of unique triangle vertices (param_1[0xe]), chooses longest extent axis (loop over local_68[0..2] = max-min per axis), then evaluates 50 candidate split positions: sorts triangles by axis (FUN_005c24e0 with comparator &LAB_0053e6d0), tries 25 splits in the [25%,75%] range. For each candidate: FUN_0053e430 partitions triangles, FUN_0053e5d0 computes SAH score, keeps the minimum.\n- If best SAH < `_DAT_005ceac0`: applies the split and recurses on both halves.\n- Else: leaf fallback.",
     [("0x005cd03c", "0.5f? SAH sample step factor"),
      ("0x005cd120", "100.0f (recip range)"),
      ("0x005ceac0", "SAH leaf threshold")],
     [],
     []),

    ("0x0053e430", 391, [], [],
     "- SAH evaluation helper. Given split-axis param_2 and split-plane param_3:\n  - For each triangle: tests 3 vertices for which side of plane; uses bits 1/2 in the per-vertex flag.\n  - Counts triangles strictly-left (local_18), strictly-right (local_14), and straddling (split by max distance).\n  - Writes (left_count, right_count, left_max, right_min) into param_4[0..3].",
     [("0x005d757c", "0.0f")],
     [],
     []),

    ("0x0053e5d0", 182, [], [],
     "- SAH cost evaluator: returns ratio comparing two log-sized halves with their box extents.\n- Computes `log2(left_count)` and `log2(right_count)` via bit-shift counting loops; combines into `((log2(L)+2 - 2^(log2(L)+1)/L) * (left_max - parent_min) + (log2(R)+2 - 2^(log2(R)+1)/R) * (parent_max - right_min)) / (parent_max - parent_min)`.",
     [],
     [],
     []),

    ("0x0053e690", 51, [], [],
     "- Helper: `if (param_1/param_2) > _DAT_005cc32c return 1.0f/1.0f` (i.e., 1.0). Else `return 1.0f / (param_1/param_2 + _DAT_005cc32c)`.\n- Used as the long-axis preference factor in FUN_0053dea0.",
     [("0x005cc32c", "branch threshold (likely 1.0f or similar)"),
      ("0x005cc320", "1.0f")],
     [],
     []),

    ("0x0053e6f0", 63, [], [],
     "- Partition: in-place rearranges a 2-dword-per-entry array so all entries with `[+4]==1` come before others. param_1 base + offset at `+0x30 * 8 + +8` gives the table.",
     [],
     [],
     []),

    ("0x0053e730", 48, ["(alloc at DAT_007d3ff8+0x108)"], [],
     "- Allocates an 8-byte leaf marker: `*alloc = -1`, `(ushort*)(alloc+4) = param_1` (count), `(ushort*)(alloc+6) = param_2` (offset).",
     [],
     [],
     []),

    ("0x0053e760", 44, ["(alloc at DAT_007d3ff8+0x108)"], [],
     "- Allocates a 0x14-byte internal node: `puVar1[0] = axis`, `[3] = left`, `[4] = right`. (Other 2 dwords left zero — likely overwritten by caller.)",
     [],
     [],
     []),

    ("0x0053e790", 94, ["(free at DAT_007d3ff8+0x10c)"], [],
     "- Frees in-build BVH work struct: frees fields at +4, +8, +0x38 if non-null, then frees the struct itself.",
     [],
     [],
     []),

    ("0x0053e7f0", 64, [], [],
     "- Tree-depth measure: iterative DFS counting leaves (`*node < 0`) into iVar2.",
     [],
     [],
     []),

    ("0x0053e830", 614, [
        "FUN_004cdca0", "FUN_004cdd60", "FUN_004d52d0", "FUN_004cde50", "FUN_004cdf20",
        "FUN_004d5340", "FUN_004c77c0", "FUN_004d5310", "FUN_004c5a00", "FUN_004cdd00",
        "FUN_004c5ae0", "FUN_004dabd0"
    ], [],
     "- Renderable construction from a source RpRaster pair (or single).\n- Reads source raster dimensions (`*(int*)(iVar5+0x10)` width, `+0x0c` height) into local_28/local_2c.\n- Allocates a new working raster (FUN_004cdca0/dd60/d52d0 = raster create+lock+blit).\n- If `param_1 == 0` (single source): also alloc a matching raster filled with 0xffffffff.\n- Else: dup of param_1[0].\n- If dimensions differ between source and current: realloc the second raster to match.\n- Calls FUN_004cde50 (lock?), FUN_004cdf20 (composite blit), FUN_004d5340 (extract result).\n- Then `FUN_004c77c0(width, height, mip_count, flags)` creates the final output RpRaster; FUN_004d5310 attaches, FUN_004c5a00 finalizes; sets format byte at +0x50.\n- Builds 30-character name string by interleaving characters from both input names (`local_28[0]`/`local_28[1]`) and assigns via FUN_004c5ae0.\n- Returns the new raster.",
     [],
     ["- [UNCERTAIN] sign-bit branch `cVar7 < 0` adds flags 0x9000 — meaning of those bits not chased here."],
     []),

    ("0x0053eca0", 105, ["FUN_004c5a60"], [],
     "- Resets a 0x34-byte slot. Iterates 2 slots (iVar2=2..0); per slot, switches on `piVar3[4]` (tag):\n  - case 1: free `*piVar3` then free `piVar3[1]`.\n  - case 2: free `*piVar3`.\n  - case 4: free `piVar3[-1]`.\n  - case 5: zero `piVar3[-1]` and `*piVar3` without freeing.\n- Then zero-fills the entire 13-dword struct.",
     [],
     [],
     []),

    ("0x0053f150", 76, [], [],
     "- Lookup helper: scans up to 2 entries at `DAT_007dc5dc + param_1` for one with `[+0x14] == param_2`. Returns pointer or 0.\n- Used to find a multipass stage by stage-id.",
     [("0x007dc5dc", "stage-state table base")],
     [],
     []),

    ("0x0053fa20", 80, ["FUN_004cc790", "FUN_00543b10"], [],
     "- Stream-read of a 4-byte chunk into local. If value is non-zero and `*(int*)(DAT_007dc5ec + param_3) == 0`, calls FUN_00543b10(param_3); on success sets the slot to 1.\n- Pattern is \"lazy init on first non-zero stream value\".",
     [("0x007dc5ec", "lazy-init slot 0")],
     [],
     []),

    ("0x0053fb00", 80, ["FUN_004cc790", "FUN_00543b20"], [],
     "- Identical shape to FUN_0053fa20 but uses DAT_007dc5f0 slot and FUN_00543b20 initializer.",
     [("0x007dc5f0", "lazy-init slot 1")],
     [],
     []),

    ("0x0053fb70", 48, ["FUN_00543b10"], [],
     "- Direct lazy-init: if `*(int*)(DAT_007dc5ec + param_1) == 0`, call FUN_00543b10; on success set slot to 1.",
     [],
     [],
     []),

    ("0x0053fba0", 14, [], [],
     "- Getter: returns `*(undefined4*)(DAT_007dc5ec + param_1)` (init flag for slot 0).",
     [],
     [],
     []),

    ("0x0053fbb0", 356, [
        "(alloc at DAT_007d3ff8+0x118)", "FUN_0053eca0", "FUN_004d8560"
    ], [],
     "- Pipeline-mode configurator. param_2 selects a render mode (1..6).\n- If table at `DAT_007dc5dc + param_1` is empty: allocs 0x34 bytes via vtable alloc (tag 0x30120), zero-fills.\n- Switch on param_2:\n  - 1/2/5: sets primary tech.\n  - 3: tech=1 plus tech-extra=2.\n  - 4: tech=4, scans for entry with `+0x14==4`, sets its +4=5, +8=6, then calls FUN_004d8560(slot, 10) and FUN_004d8560(slot, 0xb).\n  - 6: tech=5, tech-extra=4, same scan+FUN_004d8560 pair.\n- If param_2 == 0 or mode-change requires reset: calls FUN_0053eca0 to clear current state first.",
     [],
     ["- [UNCERTAIN] FUN_004d8560 not in bucket — likely \"compile pass\" / \"register pipeline node\"."],
     []),

    ("0x0053fd30", 355, ["FUN_004c5a60"], [],
     "- Stage configurator: finds stage with `+0x14 == 2` (texture-stage 2) and writes a new texture+ARG+OP triple.\n- Layout per slot 0x18 bytes: scan thrice for type==2 entries at offsets [+0]=param_3, [+0xc]=param_4, [+8]=param_5.\n- Releases any previous texture via FUN_004c5a60.\n- Updates a counter at `param_2+0x54`.\n- The triple-scan pattern (looking up the same stage 3 times back-to-back) is characteristic of D3D texture-stage state programming.",
     [],
     [],
     []),

    ("0x0053fea0", 22, [], [],
     "- Getter: returns `*(int*)(DAT_007dc5dc+param_1) ? *(int*)(*(int*)(DAT_007dc5dc+param_1) + 0x30) : 0`.\n- Reads optional pointer field +0x30 from the pipeline-state slot.",
     [],
     [],
     []),

    ("0x0053fec0", 438, [
        "FUN_004c5a60", "FUN_004c5ca0", "FUN_004c5c00", "FUN_00543b30", "FUN_004c5bc0"
    ], [],
     "- Material/texture pair installer. Finds stage with `+0x14 == 1` and rebuilds its texture binding from param_1/param_2 texture handles.\n- Releases existing entries at +4 (texture) and +8 (combined?) via FUN_004c5a60.\n- If both source textures have non-null sub-rasters (`*(int*)(*tex+0xc) != 0`): builds 30-character interleaved name, looks up an existing raster by name via FUN_004c5ca0 (raster cache lookup), or creates one via FUN_00543b30(param_1, param_2) and caches via FUN_004c5bc0.\n- Writes resolved raster to +4, sets +0x10 to `1.0f / *(int*)(*raster+0xc)` (inverse height = atlas scale).",
     [],
     [],
     []),
]

assert len(ENTRIES) == 68, f"expected 68 plates, got {len(ENTRIES)}"


def render(rva, size, callees, callers, mech, constants, uncerts, stubs):
    fm = f"""---
rva: {rva}
name_in_ghidra: FUN_{rva[2:]}
size_bytes: {size}
callees_depth1: {callees if callees else []}
callers_noted: {callers if callers else []}
confidence: C1
opened_in_slot: {SLOT}
session_date: {DATE}
---
## Mechanical description
{mech}

## Constants
"""
    if constants:
        fm += "| Address | Note |\n|---|---|\n"
        for addr, note in constants:
            fm += f"| {addr} | {note} |\n"
    else:
        fm += "(none)\n"
    fm += "\n## Uncertainties\n"
    if uncerts:
        for u in uncerts:
            fm += f"{u}\n"
    else:
        fm += "None.\n"
    fm += "\n## Stubs encountered\n"
    if stubs:
        for s in stubs:
            fm += f"{s}\n"
    else:
        fm += "None within bucket.\n"
    return fm


written = 0
for rva, size, callees, callers, mech, constants, uncerts, stubs in ENTRIES:
    path = os.path.join(BUCKET, f"{rva}.md")
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(render(rva, size, callees, callers, mech, constants, uncerts, stubs))
    written += 1

print(f"wrote {written} plates")
print(f"bucket now has {len(os.listdir(BUCKET))} files")
