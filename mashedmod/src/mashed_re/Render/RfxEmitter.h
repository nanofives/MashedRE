// Render/RfxEmitter.h — the game's RpPTank particle-batch API (D5-RFX-EMITTERS), pure core.
//
// Every particle effect in the original (smoke, flames, skid marks, the pickup glow, car shadows,
// shockwaves) goes through one small API: FUN_004770c0 creates a batch, a set of staging setters
// (FUN_004769a0 .. FUN_00476a80) fill one particle's fields into globals, FUN_00476d00 appends the
// staged particle, FUN_00476df0 copies the batch into the RpPTank arrays, FUN_004770a0 renders.
// This header is that state machine with no RenderWare dependency, so it runs in a test exe; the librw
// draw is RaceFx_DrawEmitter (LibRw/RwRaceFx.h). Evidence: verify/rfx_emitters_20261028/.
// Not ported: the PTank's own allocation (FUN_00534d00 beyond the fields below), the bounding box block of
// FUN_00537fb0 (0x00537fd4..0x0053807b), and the instance callbacks Instance() rejects (see the .cpp).
#pragma once

#include <cstdint>
#include <vector>

namespace mashed_re {
namespace Rfx {

// The RpPTank plugin block, *(atomic + [0x007dc57c]); only the fields this API reads or writes.
struct PTank {
    std::int32_t  max_count = 0;     // +0x00 (FUN_00534d00 0x00534d67, arg 1)
    std::int32_t  count = 0;         // +0x04 (FUN_00534d00 0x00534d69 = 0; FUN_00476df0 writes the batch count)
    std::uint32_t lock_bits = 0;     // +0x3c (FUN_00535700 ORs, FUN_00535910 moves into +0x40)
    std::uint32_t dirty = 0;         // +0x40 (FUN_00534d00 0x00534d79 = 0xffffffff)
    std::uint32_t data_flags = 0;    // +0xa0 (FUN_00534d00 0x00534d6c, ARRAY layout)
    std::int32_t  src_blend = 0;     // +0xa4
    std::int32_t  dst_blend = 0;     // +0xa8
    std::int32_t  vtx_alpha = 0;     // +0xac
    std::uint32_t f_b8[2] = {};      // +0xb8 (FUN_00476ae0)
    std::uint32_t f_c0 = 0;          // +0xc0 (FUN_00476be0)
    std::uint32_t f_c4 = 0;          // +0xc4 (FUN_00476b30), read as RGBA bytes by FUN_00537d20
    bool          f_c4_set = false;  // the port's own: +0xc4 was written by FUN_00476b30
    std::uint32_t f_d8[8] = {};      // +0xd8 (FUN_00476c10 4 dwords, FUN_00476c60 8 dwords)
    std::uint32_t f_f8[16] = {};     // +0xf8 (FUN_00476b90)
    std::uint32_t material_colour = 0;  // material 0 +4 (FUN_00476b30 0x00476b74)
    const void*   texture = nullptr; // material 0 texture (FUN_004e8090, called by FUN_004770c0)
    // The locked arrays FUN_00476df0 writes (FUN_00535700 cases 1 / 4 / 2 / 8 / 0x20 / 0x80 / 0x100:
    // plugin +0x4c / +0x64 / +0x6c / +0x54 / +0x7c / +0x84 / +0x8c), max_count elements each when present.
    std::vector<std::uint32_t> pos, size, col, mat, rot, uv2, uv4;
};

// FUN_004770c0's 0x40-byte batch struct (param_1).
struct Emitter {
    std::uint32_t flags = 0;    // +0x00 game flags
    PTank         tank;         // +0x04 is the atomic; its plugin block is held by value
    std::int32_t  capacity = 0; // +0x08
    std::int32_t  count = 0;    // +0x0c
    // +0x10 .. +0x28: 3 / 2 / 1 / 16 / 1 / 4 / 8 dwords per element (FUN_00474db0 sizes 0xc / 8 / 4 /
    // 0x40 / 4 / 2*8 / 4*8); empty when the flag is clear (the pointer is 0).
    std::vector<std::uint32_t> pos, size, col, mat, rot, uv2, uv4;
};

// The staging globals FUN_00476d00 copies (0x006924d8 .. 0x006925a4) and the matrix template
// FUN_004770c0 writes (0x00692558 .. 0x00692594). One set for the whole game, as in the original.
struct Staging {
    std::uint32_t pos[3] = {};    // DAT_00692528 (FUN_004769a0)
    std::uint32_t size[2] = {};   // DAT_006924dc (FUN_004769d0)
    std::uint32_t col = 0;        // DAT_00692554 (FUN_004769f0)
    std::uint32_t mat[16] = {};   // DAT_006924e8 (FUN_00476a10)
    std::uint32_t rot = 0;        // DAT_006924d8 (FUN_00476a30)
    std::uint32_t uv2[4] = {};    // DAT_00692598 (FUN_00476a40)
    std::uint32_t uv4[8] = {};    // DAT_00692534 (FUN_00476a80)
    std::uint32_t tmpl[16] = {};  // DAT_00692558 (FUN_004770c0)
};
Staging& Stage();

// 0x00534b60 — the PTank data-flag normalisation (the part before FUN_00534d00).
std::uint32_t PTankNormaliseFlags(std::uint32_t flags);
// 0x004770c0
void Create(Emitter& e, std::uint32_t game_flags, std::int32_t capacity, const void* texture);

// 0x004769a0 / 0x004769d0 / 0x004769f0 / 0x00476a10 / 0x00476a30 / 0x00476a40 / 0x00476a80.
// A null pointer selects the original's default (see the .cpp).
void SetPos(const float* p3);
void SetSize(const float* s2);
void SetColour(const std::uint32_t* rgba);
void SetMatrix(const std::uint32_t* m16);
void SetRotate(float r);
void SetUv2(const float* uv4f);
void SetUv4(const float* uv8f);

// 0x00476ae0 / 0x00476b30 / 0x00476b90 / 0x00476be0 / 0x00476c10 / 0x00476c60 / 0x00476cb0.
void SetBatchB8(Emitter& e, const std::uint32_t* v2);
void SetBatchColour(Emitter& e, const std::uint32_t* rgba);
void SetBatchMatrix(Emitter& e, const std::uint32_t* m16);
void SetBatchC0(Emitter& e, std::uint32_t v);
void SetBatchUv2(Emitter& e, const std::uint32_t* v4);
void SetBatchUv4(Emitter& e, const std::uint32_t* v8);
void SetBlend(Emitter& e, std::int32_t src, std::int32_t dst);

// 0x00476d00 / 0x00476df0
void Submit(Emitter& e);
void Flush(Emitter& e);

// 0x00536780: the instance callbacks for a PTank data-flag word, as the original's addresses.
struct Callbacks { std::uint32_t render, pos, uv, col, normal, end; };
Callbacks SelectCallbacks(std::uint32_t data_flags);

// The vertex streams FUN_00537fb0 hands the callbacks: positions (stride 0xc), uv (stride 8, when
// any uv flag), colour (stride 4, D3DCOLOR as written by FUN_00537b20 / FUN_00537d20), 4 per particle.
struct Streams {
    std::vector<float> xyz;
    std::vector<float> uv;
    std::vector<std::uint32_t> argb;
};
// Instance tank.count particles against the camera vectors FUN_00537fb0 builds (0x005381c0..0x005381e2:
// r = -LTM right, u = LTM up). Returns false (streams untouched) for a callback the port does not carry.
bool Instance(const PTank& tank, const float r[3], const float u[3], Streams& out);

}  // namespace Rfx
}  // namespace mashed_re
