// Render/RfxEmitter.h — the game's RpPTank particle-batch API, ONE core for both build targets.
//
// Every particle effect in the original (smoke, flames, skid marks, the pickup glow, car shadows,
// shockwaves) goes through one small API: FUN_004770c0 creates a batch, a set of staging setters
// (FUN_004769a0 .. FUN_00476a80) fill one particle's fields into globals, FUN_00476d00 appends the
// staged particle, FUN_00476df0 copies the batch into the RpPTank arrays, FUN_004770a0 renders.
//
// [D4-DC-RFX-CORE 2026-11-01] Render/RfxEmitter.cpp is in BOTH exe_sources.rsp and asi_sources.rsp. The 15
// staging / batch / submit bodies are the exported .asi hooks (same export names the Frida registry drives),
// written on the original's layouts: the staging globals at their absolute addresses (0x006924d8 ..
// 0x006925a8, the exe's image-pad owns that range), the 0x40-byte batch struct (Batch) and the plugin block
// reached as *(atomic + [0x007dc57c]) (PTank, by offset). The exe owns its batches through Emitter, which
// carries a stand-in atomic / geometry / material so the one body resolves the plugin and material 0 the
// way the original does. Create / Flush / SelectCallbacks / Instance are the exe's (no .asi install).
// The librw draw is RaceFx_DrawEmitter (LibRw/RwRaceFx.h). Evidence: verify/rfx_emitters_20261028/,
// verify/rfxcore_20261101/.
// Not ported: the PTank's own allocation (FUN_00534d00 beyond the fields below), the bounding box block of
// FUN_00537fb0 (0x00537fd4..0x0053807b), and the instance callbacks Instance() rejects (see the .cpp).
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

static_assert(sizeof(void*) == 4, "RfxEmitter: the original's layouts are 32-bit");

extern "C" {
// 0x004769a0 / 0x004769d0 / 0x004769f0 / 0x00476a10 / 0x00476a30 / 0x00476a40 / 0x00476a80 (staging)
__declspec(dllexport) void __cdecl ParticleEmitter_SetPosition(float* p);
__declspec(dllexport) void __cdecl ParticleEmitter_SetVelocity(float* p);
__declspec(dllexport) void __cdecl ParticleEmitter_SetColour(std::uint32_t* p);
__declspec(dllexport) void __cdecl Copy476a10(std::uint8_t* src);
__declspec(dllexport) void __cdecl ParticleEmitter_SetScalar(std::uint32_t v);
__declspec(dllexport) void __cdecl ParticleEmitter_SetRGBA(std::uint32_t* p);
__declspec(dllexport) void __cdecl Copy476a80(std::uint32_t* p);
// 0x00476ae0 / 0x00476b30 / 0x00476b90 / 0x00476be0 / 0x00476c10 / 0x00476c60 / 0x00476cb0 (batch, p1 = Batch*)
__declspec(dllexport) void __cdecl Batch476ae0(std::uint32_t* p1, std::uint32_t* p2);
__declspec(dllexport) void __cdecl Set476b30(std::uint8_t* p1, std::uint8_t* p2);
__declspec(dllexport) void __cdecl Batch476b90(std::uint32_t* p1, std::uint32_t* p2);
__declspec(dllexport) void __cdecl Batch476be0(std::uint32_t* p1, std::uint32_t v);
__declspec(dllexport) void __cdecl Batch476c10(std::uint32_t* p1, std::uint32_t* p2);
__declspec(dllexport) void __cdecl Batch476c60(std::uint32_t* p1, std::uint32_t* p2);
__declspec(dllexport) void __cdecl Batch476cb0(std::uint32_t* p1, std::uint32_t p2, std::uint32_t p3);
// 0x00476d00 (submit, p = Batch*)
__declspec(dllexport) void __cdecl MultiArrayScatter476d00(unsigned char* p);
}

namespace mashed_re {
namespace Rfx {

// One {pointer, stride} pair of the plugin's array descriptor (FUN_00535700 hands both to its caller).
struct Lock {
    std::uint32_t* ptr;
    std::uint32_t  stride;
};

// The RpPTank plugin block, *(atomic + [0x007dc57c]). Only the fields this API reads or writes are named;
// the gaps are the original's, unread here.
struct PTank {
    std::int32_t  max_count;     // +0x00 (FUN_00534d00 0x00534d67, arg 1)
    std::int32_t  count;         // +0x04 (FUN_00534d00 0x00534d69 = 0; FUN_00476df0 0x00476e19)
    std::uint32_t u08[13];       // +0x08
    std::uint32_t lock_bits;     // +0x3c (FUN_00535700 0x005357ed ORs, FUN_00535910 moves into +0x40)
    std::uint32_t dirty;         // +0x40 (FUN_00534d00 0x00534d79 = 0xffffffff)
    std::uint32_t u44[2];        // +0x44 .. +0x48 (+0x48 is the descriptor FUN_00535700 adds, 0x00535712)
    // +0x4c .. +0x93: {ptr, stride} for lock flag 1 / 8 / 0x10 / 4 / 2 / 0x40 / 0x20 / 0x80 / 0x100
    // (FUN_00535700 jump tables 0x005357f8 / 0x00535818: desc +4 / +0xc / +0x14 / +0x1c / +0x24 / +0x2c /
    // +0x34 / +0x3c / +0x44).
    Lock          pos;           // +0x4c  flag 1
    Lock          mat;           // +0x54  flag 8
    Lock          nrm;           // +0x5c  flag 0x10
    Lock          size;          // +0x64  flag 4
    Lock          col;           // +0x6c  flag 2
    Lock          col4;          // +0x74  flag 0x40
    Lock          rot;           // +0x7c  flag 0x20
    Lock          uv2;           // +0x84  flag 0x80
    Lock          uv4;           // +0x8c  flag 0x100
    std::uint32_t u94[2];        // +0x94
    std::uint32_t stride_all;    // +0x9c  (desc +0x54: overrides every stride when non-zero, 0x005357c0)
    std::uint32_t data_flags;    // +0xa0  (FUN_00534d00 0x00534d6c, ARRAY layout; desc +0x58, 0x00535715)
    std::int32_t  src_blend;     // +0xa4
    std::int32_t  dst_blend;     // +0xa8
    std::int32_t  vtx_alpha;     // +0xac
    std::uint32_t ub0[2];        // +0xb0
    std::uint32_t f_b8[2];       // +0xb8 (FUN_00476ae0)
    std::uint32_t f_c0;          // +0xc0 (FUN_00476be0)
    std::uint32_t f_c4;          // +0xc4 (FUN_00476b30), read as RGBA bytes by FUN_00537d20
    std::uint32_t uc8[4];        // +0xc8
    std::uint32_t f_d8[8];       // +0xd8 (FUN_00476c10 4 dwords, FUN_00476c60 8 dwords)
    std::uint32_t f_f8[16];      // +0xf8 (FUN_00476b90)
};
static_assert(offsetof(PTank, lock_bits) == 0x3c && offsetof(PTank, dirty) == 0x40, "PTank layout");
static_assert(offsetof(PTank, pos) == 0x4c && offsetof(PTank, uv4) == 0x8c, "PTank layout");
static_assert(offsetof(PTank, stride_all) == 0x9c && offsetof(PTank, data_flags) == 0xa0, "PTank layout");
static_assert(offsetof(PTank, f_b8) == 0xb8 && offsetof(PTank, f_c4) == 0xc4, "PTank layout");
static_assert(offsetof(PTank, f_d8) == 0xd8 && offsetof(PTank, f_f8) == 0xf8, "PTank layout");
static_assert(sizeof(PTank) == 0x138, "PTank layout");

// FUN_004770c0's 0x40-byte batch struct (param_1).
struct Batch {
    std::uint32_t  flags;        // +0x00 game flags
    std::uint8_t*  atomic;       // +0x04
    std::int32_t   capacity;     // +0x08
    std::int32_t   count;        // +0x0c
    // +0x10 .. +0x28: 3 / 2 / 1 / 16 / 1 / 4 / 8 dwords per element (FUN_00474db0 sizes 0xc / 8 / 4 /
    // 0x40 / 4 / 2*8 / 4*8); 0 when the flag is clear.
    std::uint32_t* pos;          // +0x10 flag 1
    std::uint32_t* size;         // +0x14 flag 2
    std::uint32_t* col;          // +0x18 flag 4
    std::uint32_t* mat;          // +0x1c flag 8
    std::uint32_t* rot;          // +0x20 flag 0x10
    std::uint32_t* uv2;          // +0x24 flag 0x20
    std::uint32_t* uv4;          // +0x28 flag 0x40
    std::uint32_t  u2c[5];       // +0x2c
};
static_assert(offsetof(Batch, pos) == 0x10 && offsetof(Batch, uv4) == 0x28 && sizeof(Batch) == 0x40,
              "Batch layout");

// The exe's owner of one batch (port-only). `batch` is first, so &emitter is the Batch* every hook takes.
// The stand-ins give FUN_00476b30's chain its shape: atomic +[0x007dc57c] -> PTank, atomic +0x18 ->
// geometry, geometry +0x20 -> material list, list[0] -> material (+0 texture, +4 colour). Not copyable:
// the batch points into it.
struct Emitter {
    Batch          batch{};
    PTank          tank{};
    struct Material { const void* texture; std::uint32_t colour; } mat0{};
    Material*      mat_list[1] = {};
    std::uint32_t  geo[0x24 / 4] = {};       // +0x20 holds &mat_list
    std::uint32_t  atom[0x80 / 4] = {};      // +[0x007dc57c] holds &tank, +0x18 holds &geo
    bool           ok = false;               // Create placed the plugin pointer
    bool           c4_set = false;           // +0xc4 was written by FUN_00476b30 (its initial value is not transcribed)
    std::vector<std::uint32_t> b_pos, b_size, b_col, b_mat, b_rot, b_uv2, b_uv4;   // batch storage
    std::vector<std::uint32_t> t_pos, t_size, t_col, t_mat, t_rot, t_uv2, t_uv4;   // plugin storage

    Emitter() = default;
    Emitter(const Emitter&) = delete;
    Emitter& operator=(const Emitter&) = delete;
};

// 0x00534b60 — the PTank data-flag normalisation (the part before FUN_00534d00).
std::uint32_t PTankNormaliseFlags(std::uint32_t flags);
// 0x004770c0
void Create(Emitter& e, std::uint32_t game_flags, std::int32_t capacity, const void* texture);

// The staging setters: a null pointer selects the original's default (see the .cpp).
inline void SetPos(const float* p3) { ParticleEmitter_SetPosition(const_cast<float*>(p3)); }
inline void SetSize(const float* s2) { ParticleEmitter_SetVelocity(const_cast<float*>(s2)); }
inline void SetColour(const std::uint32_t* rgba) { ParticleEmitter_SetColour(const_cast<std::uint32_t*>(rgba)); }
inline void SetMatrix(const std::uint32_t* m16) {
    Copy476a10(reinterpret_cast<std::uint8_t*>(const_cast<std::uint32_t*>(m16)));
}
inline void SetRotateBits(std::uint32_t bits) { ParticleEmitter_SetScalar(bits); }
void SetRotate(float r);
inline void SetUv2(const float* uv4f) {
    ParticleEmitter_SetRGBA(reinterpret_cast<std::uint32_t*>(const_cast<float*>(uv4f)));
}
inline void SetUv4(const float* uv8f) {
    Copy476a80(reinterpret_cast<std::uint32_t*>(const_cast<float*>(uv8f)));
}

// The batch setters.
inline std::uint32_t* B(Emitter& e) { return reinterpret_cast<std::uint32_t*>(&e.batch); }
inline void SetBatchB8(Emitter& e, const std::uint32_t* v2) { Batch476ae0(B(e), const_cast<std::uint32_t*>(v2)); }
void SetBatchColour(Emitter& e, const std::uint32_t* rgba);
inline void SetBatchMatrix(Emitter& e, const std::uint32_t* m16) {
    Batch476b90(B(e), const_cast<std::uint32_t*>(m16));
}
inline void SetBatchC0(Emitter& e, std::uint32_t v) { Batch476be0(B(e), v); }
inline void SetBatchUv2(Emitter& e, const std::uint32_t* v4) { Batch476c10(B(e), const_cast<std::uint32_t*>(v4)); }
inline void SetBatchUv4(Emitter& e, const std::uint32_t* v8) { Batch476c60(B(e), const_cast<std::uint32_t*>(v8)); }
inline void SetBlend(Emitter& e, std::int32_t src, std::int32_t dst) {
    Batch476cb0(B(e), static_cast<std::uint32_t>(src), static_cast<std::uint32_t>(dst));
}

inline void Submit(Emitter& e) { if (e.ok) MultiArrayScatter476d00(reinterpret_cast<unsigned char*>(&e.batch)); }
// 0x00476df0
void Flush(Emitter& e);

// The material-0 texture FUN_004770c0 binds (FUN_004e8090); the draw reads it.
inline const void* Texture(const Emitter& e) { return e.mat0.texture; }
inline void SetTexture(Emitter& e, const void* t) { e.mat0.texture = t; }

// Element i of a plugin array (nullptr when the flag's array is absent).
const std::uint32_t* TankElem(const PTank& t, const Lock& l, int i);

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
// r = -LTM right, u = LTM up). `c4_known`: plugin +0xc4 has been written. Returns false (streams untouched)
// for a callback the port does not carry.
bool Instance(const PTank& tank, bool c4_known, const float r[3], const float u[3], Streams& out);

}  // namespace Rfx
}  // namespace mashed_re
