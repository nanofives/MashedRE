// Mashed RE — ArcticTrackNodeSlot0, the "arctic" record's slot-0 track node.
// Original: 0x00448940  (no FUN_ name in Ghidra until this effort)  render  C0 -> C2/C3
// Plate: re/analysis/track_nodes_arctic/0x00448940.md
//
// WHY THE NAME IS WHAT IT IS. It claims exactly three things, each read out of
// original\MASHED.exe.unpatched and nothing more (memory
// `a-name-may-not-claim-more-than-its-comment`):
//   * the per-track node table lives at 0x005f33f8, stride 0x48, laid out
//     `{char name[0x10]; u32 Course_Id; void* slot[13]}`;
//   * record 2 at 0x005f3488 is `"arctic"` with `Course_Id` 0 at 0x005f3498;
//   * slot 0 at 0x005f349c is 0x00448940 — this function.
// It deliberately says nothing about intent or phase.
//
// CALLER. 0x0041e8b0 `TrackNodeDispatch14` (C3) is
//     0x0041e8b0  8b 0d e4 d7 63 00   mov ecx,dword ptr [0x0063d7e4]
//     0x0041e8b6  ff 61 14            jmp dword ptr [ecx + 0x14]
// and `+0x14` is slot 0 of the record selected into 0x0063d7e4 by
// `TrackNodeRecordScan` 0x0041e870 (C3). So the caller is named, is at C3, and
// the dispatch is read from the bytes rather than assumed.
//
// PURPOSE, in prose, from the body below. Bring the arctic course up: reset the
// render-state globals, run the three sibling arctic helpers, detach the course's
// clump[2] from the world frame and replicate it 24 times into a 25-entry tile
// array, translate the 25 tiles into a 5x5 grid 60 units apart at Y = -4.1,
// register a "smoke" particle emitter, instantiate the course's three object
// lists into the handle array under a shared cursor, append one more object, and
// — only when the course's +0x105f8 and +0x105fc floats compare equal — push two
// camera values and a triple.
//
// Disassembly read from original\MASHED.exe.unpatched with capstone,
// 0x00448940..0x00448caf (880 bytes, RET at 0x00448caf). Decompilation from a
// read-only Ghidra pool clone (Mashed_pool0, analyzeHeadless -readOnly -create;
// the function was NOT defined in Ghidra before this effort — it has no callers
// in the call graph because it is only ever reached through the table).
//
// ──────────────────────────────────────────────────────────────────────────────
// TWO GHIDRA ARITIES ARE WRONG, AND THE DISASSEMBLY SAYS SO
//
// Ghidra prints `FUN_004671a0(0,0x44480000)` and `FUN_004c1b10(uVar1)`. Both are
// off by one argument, and porting the decompiler's version would have pushed
// 800.0f into the wrong callee:
//
//   0x004671a0  call 0x42b930 / cmp eax,3 / cmp dword ptr [esp+4],-1
//               -> reads ONE stack argument.
//   0x004c1b10  fld dword ptr [esp+8] / mov esi,[esp+8] / fstp [esi+0x84]
//               -> reads TWO: a pointer and a float it stores at +0x84.
//
// and the call site cleans all 8 pushes at once (`add esp,4` at 0x00448c7d plus
// `add esp,0x1c` at 0x00448ca9 = 32 bytes = 8 dwords), which is what let the
// decompiler mis-attribute the slot. So the real calls are
// `FUN_004671a0(0)` and `FUN_004c1b10(raster, 800.0f)`.
// (memory `ghidra-warnings-are-hypotheses`, `decomp-is-silent-about-register-args`)
//
// ──────────────────────────────────────────────────────────────────────────────
// CONSTANTS — every one read as raw hex from the instruction that materialises
// it, never from a decompiler gloss (memory `plate-hex-gloss-authoritative`):
//
//   0x004489bb  0xc0833333  -4.099999904632568   tile Y, set once, never changed
//   0x00448a65  0xc3160000  -150.0               grid X start
//   0x00448a80  0xc3160000  -150.0               grid Z start (per outer step)
//   0x005cc728  0x42700000   60.0                grid step, added with FADD
//   0x00448ad2  0xc23af5c3  -46.7400016784668    emitter pos x
//   0x00448ada  0x3f3f3b64    0.746999979019165  emitter pos y
//   0x00448ae2  0xc28c245a  -70.07099914550781   emitter pos z
//   0x00448aea  0xbc03126f   -0.00800000037997961
//   0x00448af2  0x3ca3d70a    0.019999999552965164
//   0x00448afa  0xbc03126f   -0.00800000037997961
//   0x00448b02  0x3c03126f    0.00800000037997961
//   0x00448b0a  0x3da3d70a    0.07999999821186066
//   0x00448b12  0x3c03126f    0.00800000037997961
//   0x00448b33  0x3c23d70a    0.009999999776482582  passed BY VALUE as arg 2
//   0x00448c21  0xc204199a  -33.025001525878906
//   0x00448c29  0x401147ae    2.2699999809265137
//   0x00448c31  0xc2286666  -42.099998474121094
//   0x00448c71  0x44480000  800.0
//   0x00448c8f  0x43480000  200.0
//   0x005cca08  "smoke"     (bytes `73 6d 6f 6b 65 00`)
//
// ──────────────────────────────────────────────────────────────────────────────
// THE GRID IS ACCUMULATED, NOT MULTIPLIED. 0x00448aab and 0x00448ac1 are both
// `fadd dword ptr [0x5cc728]` on the running coordinate. -150, -90, -30, 30 and
// 90 are each exactly representable in binary32 and 60.0 is exact, so repeated
// addition and `-150 + 60*i` agree bit-for-bit here; the port still accumulates,
// because that is what the instructions do.
//
// ──────────────────────────────────────────────────────────────────────────────
// WHAT THE STANDALONE DOES WITH THIS FILE
//
// This TU is listed in BOTH mashedmod/exe_sources.rsp and asi_sources.rsp, so
// there is ONE body for the RVA (re/CONFIDENCE.md L43+, `exe_file == file`).
//
// `ArcticSeaTileGrid` below is live in both targets: the node's own loop calls
// it, and so does D3d9Render/TrackRenderer.cpp. It touches no MASHED address.
//
// `ArcticTrackNodeSlot0` itself is a DEAD EXPORT in mashed_re.exe — the
// GameSaveBuffer.cpp precedent. Its 22 callees are MASHED code addresses and its
// globals are MASHED data addresses; the standalone is based at 0x00010000 with
// an 11.9 MB .data that SPANS the MASHED range (measured, exe_main.cpp:7630-7643),
// so those addresses are our own bytes, not MASHED's. A guarded early return at
// the top of the body makes an accidental call inert instead of a jump into our
// own data. In the exe `HookSystem::Register` is a no-op (Stubs/HookSystemNoOp.cpp),
// so nothing installs it there either.
//
// STUBS (recorded in STUBS.md):
//   S-C  the whole 22-callee body is not executed in the standalone. The grid is
//        the part the standalone needs and it is shared, not duplicated.
//
// [UNCERTAIN] the semantics of the course-object fields this body reads are NOT
// established and are deliberately left as cited offsets:
//   +0x10004 / +0x10008 / +0x1000c  three counts, each `test eax,eax` + `jle`,
//                                    so SIGNED and <= 0 skips the loop
//   +0x1030 / +0x2030 / +0x3030     the matching record bases, stride 0x40
//   +0x10118                        the clump the sea tiles are cloned from
//   +0x105d4                        the frame the clump is detached from
//   +0x105f8 / +0x105fc             two floats compared with FCOMP/`test ah,0x44`
//                                    + `jp` — i.e. the body runs on EQUAL ONLY,
//                                    and an unordered compare also skips it
// and likewise 0x008962c0..0x008962cc (4 dwords copied out of frame+0x1c..+0x28),
// 0x008962e0 (the handle array), 0x0068324c (its cursor) and 0x00683248.
#include "ArcticTrackNodeSlot0.h"

#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Render {

// 0x00448a65..0x00448acb — the 5x5 grid, exactly as the two loops build it.
void ArcticSeaTileGrid(float out[kArcticSeaTileCount][3]) {
    // 0x004489bb — the Y component is written ONCE, before the clone loop, and
    // no later instruction touches it.
    const float y   = -4.099999904632568f;      // 0xc0833333
    const float step = 60.0f;                   // _DAT_005cc728 = 0x42700000

    int k = 0;                                  // 0x00448a6d `xor edi,edi`
    float x = -150.0f;                          // 0x00448a65 `0xc3160000`
    for (int ix = 0; ix < 5; ++ix) {            // 0x00448a6f `mov ebp,5`
        float z = -150.0f;                      // 0x00448a80 `0xc3160000`
        for (int iz = 0; iz < 5; ++iz) {        // 0x00448a88 `mov ebx,5`
            out[k][0] = x;
            out[k][1] = y;
            out[k][2] = z;
            ++k;                                // 0x00448ab4 `inc edi`
            z += step;                          // 0x00448aab `fadd [0x5cc728]`
        }
        x += step;                              // 0x00448ac1 `fadd [0x5cc728]`
    }
}

}  // namespace Render
}  // namespace mashed_re

#ifndef MASHED_STANDALONE
namespace {
// Callee thunks — MASHED code addresses, valid only inside the injected .asi.
// Arities are the ones the DISASSEMBLY supports (see the header comment for the
// two places where that differs from Ghidra).
typedef void (__cdecl* Fn_v)();
typedef void (__cdecl* Fn_p)(void*);
typedef void (__cdecl* Fn_u)(std::uint32_t);
typedef void (__cdecl* Fn_pu)(void*, std::uint32_t);
typedef void (__cdecl* Fn_pf)(void*, float);
typedef void (__cdecl* Fn_pp)(void*, void*);
typedef void (__cdecl* Fn_4u)(std::uint32_t, std::uint32_t, std::uint32_t,
                              std::uint32_t);
typedef void (__cdecl* Fn_3u)(std::uint32_t, std::uint32_t, std::uint32_t);
// 0x004c1340 — three pushes at the call site (`add esp,0xc` at 0x00448ab1).
typedef void (__cdecl* Fn_xlate)(std::uint32_t frame, const float* v,
                                 std::uint32_t combine);
typedef void* (__cdecl* Fn_u_p)(std::uint32_t);
typedef void* (__cdecl* Fn_p_p)(void*);
typedef void* (__cdecl* Fn_emit)(std::uint32_t, void*, std::uint32_t,
                                 std::uint32_t);
typedef void (__cdecl* Fn_part)(void*, std::uint32_t, void*, void*);
typedef void* (__cdecl* Fn_s_p)(const char*);
}  // namespace
#endif

// 0x00448940
extern "C" __declspec(dllexport) void __cdecl ArcticTrackNodeSlot0(void* course) {
    using mashed_re::Render::kArcticSeaTileCount;
    using mashed_re::Render::ArcticSeaTileGrid;

#ifdef MASHED_STANDALONE
    // DEAD EXPORT in mashed_re.exe. See the header comment: every callee below is
    // a MASHED code address that, in this target, is our own .data. Returning
    // here is the guard, not an implementation choice.
    (void)course;
    return;
#else
    std::uint8_t* const c = static_cast<std::uint8_t*>(course);

    // 0x00448947..0x00448986 — render-state reset and the three sibling helpers.
    reinterpret_cast<Fn_v>(0x0049a0e0u)();                                 // 0x00448947
    reinterpret_cast<Fn_u>(0x00496d00u)(0x0049a0c0u);                      // 0x00448951 (push 0x49a0c0)
    reinterpret_cast<Fn_pu>(0x00496d20u)(
        *reinterpret_cast<void**>(c + 0x105d4), 1u);                       // 0x00448963
    reinterpret_cast<Fn_4u>(0x00484000u)(0u, 0x80u, 0x80u, 0x600u);        // 0x00448979
    reinterpret_cast<Fn_u>(0x0049a0a0u)(
        reinterpret_cast<std::uint32_t>(
            reinterpret_cast<Fn_u_p>(0x004840b0u)(0u)));                   // 0x00448980/0x00448986
    reinterpret_cast<Fn_p>(0x00448770u)(course);                           // 0x0044898c
    reinterpret_cast<Fn_p>(0x00448880u)(course);                           // 0x00448992
    reinterpret_cast<Fn_p>(0x00448820u)(course);                           // 0x00448998

    // 0x0044899d — reset the source clump's frame matrix, then publish the clump
    // as tile 0 of the array at 0x008963e0.
    void** const tiles = reinterpret_cast<void**>(0x008963e0u);
    void* const  src   = *reinterpret_cast<void**>(c + 0x10118);
    reinterpret_cast<Fn_u>(0x004c15c0u)(
        *reinterpret_cast<std::uint32_t*>(
            reinterpret_cast<std::uint8_t*>(src) + 4));                    // 0x004489a7
    tiles[0] = src;                                                        // 0x004489cb

    // 0x004489d0 — bounding/visibility fixup on the clump, then copy four dwords
    // out of +0x1c..+0x28 into 0x008962c0..0x008962cc.
    std::uint8_t* const bb =
        static_cast<std::uint8_t*>(reinterpret_cast<Fn_p_p>(0x004b3f90u)(src));
    if ((*(bb + 0x4c) & 2) != 0)                                           // 0x004489d7/0x004489dd
        reinterpret_cast<Fn_p>(0x004e5fc0u)(bb);                           // 0x004489e2
    std::uint32_t* const mirror = reinterpret_cast<std::uint32_t*>(0x008962c0u);
    mirror[0] = *reinterpret_cast<std::uint32_t*>(bb + 0x1c);              // 0x004489f1
    mirror[1] = *reinterpret_cast<std::uint32_t*>(bb + 0x20);              // 0x004489fa
    mirror[2] = *reinterpret_cast<std::uint32_t*>(bb + 0x24);              // 0x00448a03
    mirror[3] = *reinterpret_cast<std::uint32_t*>(bb + 0x28);              // 0x00448a11

    // 0x00448a1f — RwFrameRemoveChild(course[+0x105d4], tiles[0]), then reset the
    // detached clump's frame matrix again.
    reinterpret_cast<Fn_pp>(0x004e45b0u)(
        *reinterpret_cast<void**>(c + 0x105d4), tiles[0]);                 // 0x00448a1f
    reinterpret_cast<Fn_u>(0x004c15c0u)(
        *reinterpret_cast<std::uint32_t*>(
            static_cast<std::uint8_t*>(tiles[0]) + 4));                    // 0x00448a2e

    // 0x00448a36..0x00448a63 — 24 clones into tiles[1..24]; the loop bound is
    // the ADDRESS test `cmp edi,0x896444`, i.e. one past tiles[24].
    for (int i = 1; i < kArcticSeaTileCount; ++i) {
        void* const clone = reinterpret_cast<Fn_p_p>(0x004e6ab0u)(tiles[0]); // 0x00448a47
        tiles[i] = clone;                                                    // 0x00448a4c
        reinterpret_cast<Fn_u>(0x004c15c0u)(
            *reinterpret_cast<std::uint32_t*>(
                static_cast<std::uint8_t*>(clone) + 4));                     // 0x00448a52
    }

    // 0x00448a65..0x00448acb — lay the grid. The translation vector is the same
    // stack local throughout; combine op 0 is rwCOMBINEREPLACE.
    float grid[kArcticSeaTileCount][3];
    ArcticSeaTileGrid(grid);
    for (int k = 0; k < kArcticSeaTileCount; ++k) {
        const float v[3] = { grid[k][0], grid[k][1], grid[k][2] };
        reinterpret_cast<Fn_xlate>(0x004c1340u)(
            *reinterpret_cast<std::uint32_t*>(
                static_cast<std::uint8_t*>(tiles[k]) + 4),                 // 0x00448a90/0x00448a97
            v,
            0u);                                                            // 0x00448a9a (push 0)
    }

    // 0x00448acd..0x00448b39 — the "smoke" emitter. Three vectors live in the
    // same stack frame; the fourth argument of 0x00491b20 is a BY-VALUE float.
    float emitPos[3] = { -46.7400016784668f,       // 0xc23af5c3  0x00448ad2
                           0.746999979019165f,     // 0x3f3f3b64  0x00448ada
                         -70.07099914550781f };    // 0xc28c245a  0x00448ae2
    float emitA[3]   = {  -0.00800000037997961f,   // 0xbc03126f  0x00448aea
                           0.019999999552965164f,  // 0x3ca3d70a  0x00448af2
                          -0.00800000037997961f }; // 0xbc03126f  0x00448afa
    float emitB[3]   = {   0.00800000037997961f,   // 0x3c03126f  0x00448b02
                           0.07999999821186066f,   // 0x3da3d70a  0x00448b0a
                           0.00800000037997961f }; // 0x3c03126f  0x00448b12
    reinterpret_cast<Fn_u>(0x00491c00u)(
        reinterpret_cast<std::uint32_t>(
            reinterpret_cast<Fn_s_p>(0x0040bb30u)(
                reinterpret_cast<const char*>(0x005cca08u))));             // 0x00448b1a/0x00448b20
    reinterpret_cast<Fn_part>(0x00491b20u)(
        emitPos, 0x3c23d70au, emitA, emitB);                               // 0x00448b39

    // 0x00448b3e..0x00448c0f — three object lists, each `count` SIGNED with a
    // `jle` skip, base `course + <off>`, stride 0x40, type 0xc / 7 / 2.
    std::uint32_t* const handles = reinterpret_cast<std::uint32_t*>(0x008962e0u);
    std::int32_t*  const cursor  = reinterpret_cast<std::int32_t*>(0x0068324cu);
    const struct { std::uint32_t off; std::uint32_t cnt; std::uint32_t type; }
        lists[3] = { { 0x1030u, 0x10004u, 0xcu },      // 0x00448b4d/0x00448b58
                     { 0x2030u, 0x10008u, 0x7u },      // 0x00448b90/0x00448b9b
                     { 0x3030u, 0x1000cu, 0x2u } };    // 0x00448bd3/0x00448be5
    for (int L = 0; L < 3; ++L) {
        std::uint8_t* rec = c + lists[L].off;
        const std::int32_t n = *reinterpret_cast<std::int32_t*>(c + lists[L].cnt);
        for (std::int32_t i = 0; i < n; ++i) {
            handles[*cursor] = reinterpret_cast<std::uint32_t>(
                reinterpret_cast<Fn_emit>(0x00487e70u)(
                    lists[L].type, rec, 0u, 0x14u));
            ++*cursor;
            rec += 0x40;                                                   // 0x00448b7d
        }
    }

    // 0x00448c11..0x00448c54 — one more object at a fixed position; 0x00683248
    // records the cursor BEFORE it is bumped for this one.
    float extra[3] = { -33.025001525878906f,   // 0xc204199a  0x00448c21
                         2.2699999809265137f,  // 0x401147ae  0x00448c29
                       -42.099998474121094f }; // 0xc2286666  0x00448c31
    *reinterpret_cast<std::int32_t*>(0x00683248u) = *cursor;               // 0x00448c39
    handles[*cursor] = reinterpret_cast<std::uint32_t>(
        reinterpret_cast<Fn_emit>(0x00487e70u)(0xcu, extra, 0u, 0x14u));   // 0x00448c3e
    ++*cursor;                                                             // 0x00448c53

    // 0x00448c5a..0x00448ca9 — runs ONLY when the two floats compare EQUAL.
    // `fcomp` + `test ah,0x44` + `jp` skips on both not-equal AND unordered, so
    // a NaN on either side also skips. `==` in C reproduces that.
    const float f8 = *reinterpret_cast<float*>(c + 0x105f8);
    const float fc = *reinterpret_cast<float*>(c + 0x105fc);
    if (f8 == fc) {
        void* const r = reinterpret_cast<Fn_u_p>(0x004671a0u)(0u);         // 0x00448c78
        reinterpret_cast<Fn_pf>(0x004c1b10u)(r, 800.0f);                   // 0x00448c81 (0x44480000)
        void* const r2 = reinterpret_cast<Fn_u_p>(0x004671a0u)(0u);        // 0x00448c88
        *reinterpret_cast<std::uint32_t*>(
            static_cast<std::uint8_t*>(r2) + 0x88) = 0x43480000u;          // 0x00448c8f (200.0f)
        reinterpret_cast<Fn_u_p>(0x004671a0u)(0u);                         // 0x00448c99 (result discarded)
        reinterpret_cast<Fn_3u>(0x004924c0u)(0x28u, 0x2cu, 0x30u);         // 0x00448ca4
    }
#endif
}

RH_ScopedInstall(ArcticTrackNodeSlot0, 0x00448940);
