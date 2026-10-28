// Render/RfxEmitter.cpp — see RfxEmitter.h. EXE-ONLY, no RenderWare include (the test exe links it alone).
//
// [D5-RFX-EMITTERS 2026-10-28] Transcribed from the disassembly of original/MASHED.exe.unpatched and checked
// dword for dword against those bytes run in Unicorn (verify/rfx_emitters_20261028/emit_unicorn.py).
// Arrays hold raw dwords so every copy is a bit copy, as the original's MOVs and REP MOVSD are.

#include "RfxEmitter.h"

#include <cstring>

namespace mashed_re {
namespace Rfx {
namespace {

Staging g_stage;

float F(std::uint32_t u) { float f; std::memcpy(&f, &u, 4); return f; }
std::uint32_t U(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); return u; }

// Image constants (file-backed .data, read from the anchored binary).
const std::uint32_t k00613288[2] = {0x3f800000u, 0x3f800000u};              // 1.0, 1.0
const std::uint32_t k00613290    = 0xffffffffu;
const std::uint32_t k00613294[4] = {0u, 0u, 0x3f800000u, 0x3f800000u};      // 0, 0, 1, 1
const std::uint32_t k006132a4[8] = {0u, 0x3f800000u, 0x3f800000u, 0x3f800000u,
                                    0x3f800000u, 0u, 0u, 0u};               // 0,1,1,1,1,0,0,0
const std::int32_t  k006132c4 = 5, k006132c8 = 6;
const float         k005cc32c = 0.5f;
// DAT_006925a8 (FUN_004769a0's default) is BSS with no writer (decomp_pc.py --datarefs: 0 writes): zero.
const std::uint32_t k006925a8[3] = {0u, 0u, 0u};

// x87 FSINCOS under the precision control the original runs at (PC_24, D3D's FPU state). The exe's default
// control word is PC_53, so it is set for the one instruction and restored.
void SinCos24(float a, float* s, float* c) {
#if defined(_M_IX86)
    unsigned short cw, cw24;
    float ls, lc;
    __asm {
        fnstcw cw
        mov    ax, cw
        and    ax, 0xfcff
        mov    cw24, ax
        fldcw  cw24
        fld    a
        fsincos
        fstp   lc
        fstp   ls
        fldcw  cw
    }
    *s = ls;
    *c = lc;
#else
#error RfxEmitter.cpp needs the x86 FSINCOS path
#endif
}

void Resize(std::vector<std::uint32_t>& v, bool on, std::size_t n) {
    v.assign(on ? n : 0, 0u);
}

}  // namespace

Staging& Stage() { return g_stage; }

// 0x00534b60
std::uint32_t PTankNormaliseFlags(std::uint32_t f) {
    const std::uint8_t b = static_cast<std::uint8_t>(f);
    const bool pos = (b & 1) == 1, mat = (b & 8) == 8, rot = (b & 0x20) == 0x20;
    const std::uint32_t u100 = f & 0x100, u20000 = f & 0x20000, u40000 = f & 0x40000,
                        u80000 = f & 0x80000, u100000 = f & 0x100000;
    if (mat && (f & 0x8000) == 0x8000) f = 0x8000;
    if (rot && u20000 == 0x20000) f &= 0x20000;
    if ((b & 0x40) == 0x40 && u40000 == 0x40000) f &= 0x40000;
    if ((b & 0x80) == 0x80 && u80000 == 0x80000) f &= 0x80000;
    if (u100 == 0x100 && u100000 == 0x100000) f &= 0x100000;
    if (mat) {
        if (pos) f &= 0xfffffffeu;
        if ((b & 4) == 4) f &= 0xfffffffbu;
        if (rot) f &= 0xffffffdfu;
    }
    if (!mat && !pos) f |= 1;
    if ((f & 0x30000000) == 0) f |= 0x10000000;
    return f;
}

// 0x004770c0
void Create(Emitter& e, std::uint32_t flags, std::int32_t cap, const void* texture) {
    std::uint32_t d = 0x10000000u;
    if (flags == 0) flags = 0x807;
    if (flags & 1) d = 0x10000001u;
    if (flags & 2) d |= 4;
    if (flags & 4) d |= 2;
    if (flags & 8) d |= 8;
    if (flags & 0x10) d |= 0x20;
    if (flags & 0x20) d |= 0x80;
    if (flags & 0x40) d |= 0x100;
    if (flags & 0x200) d |= 0x8000;
    if (flags & 0x400) d |= 0x20000;
    if (flags & 0x800) d |= 0x80000;
    if (flags & 0x1000) d |= 0x100000;
    e = Emitter();                                   // FUN_004b6520(param_1, 0x40)

    // FUN_00534b60(cap, d, 0) -> FUN_00534d00: +0 / +4 / +0xa0 / +0x40 / +0x3c (0x00534d67..0x00534d80).
    // Not transcribed: the ARRAY-layout allocation (0x0053510c..). The port sizes each array max_count
    // elements by its data flag, at the element sizes FUN_00476df0 copies.
    PTank& t = e.tank;
    t.max_count = cap;
    t.count = 0;
    t.data_flags = PTankNormaliseFlags(d);
    t.dirty = 0xffffffffu;
    t.lock_bits = 0;
    const std::size_t n = cap > 0 ? static_cast<std::size_t>(cap) : 0;
    Resize(t.pos, (t.data_flags & 1) != 0, n * 3);
    Resize(t.size, (t.data_flags & 4) != 0, n * 2);
    Resize(t.col, (t.data_flags & 2) != 0, n);
    Resize(t.mat, (t.data_flags & 8) != 0, n * 16);
    Resize(t.rot, (t.data_flags & 0x20) != 0, n);
    Resize(t.uv2, (t.data_flags & 0x80) != 0, n * 4);
    Resize(t.uv4, (t.data_flags & 0x100) != 0, n * 8);

    // the matrix template DAT_00692558 (FUN_004770c0 decomp); +0x0c / +0x1c / +0x2c / +0x3c are left as they are
    std::uint32_t* m = g_stage.tmpl;
    m[3] |= 0x20003u;
    m[10] = 0x3f800000u; m[5] = 0x3f800000u; m[0] = 0x3f800000u;
    m[4] = 0; m[2] = 0; m[1] = 0; m[9] = 0; m[8] = 0; m[6] = 0; m[14] = 0; m[13] = 0; m[12] = 0;

    e.flags = flags;
    if (static_cast<std::int8_t>(flags) < 0) SetBatchB8(e, nullptr);
    if (flags & 0x100) SetBatchColour(e, nullptr);
    if (flags & 0x200) SetBatchMatrix(e, nullptr);
    if (flags & 0x400) SetBatchC0(e, 0);
    if (flags & 0x800) SetBatchUv2(e, nullptr);
    if (flags & 0x1000) SetBatchUv4(e, nullptr);
    t.texture = texture;                             // FUN_004e8090(material 0, param_4)
    t.vtx_alpha = 1;
    t.src_blend = k006132c4;
    t.dst_blend = k006132c8;
    t.dirty |= 0x10000000u;
    t.dirty |= 0x800000u;
    t.count = 0;
    // FUN_004c0b30 / FUN_004e7e30: a fresh frame (identity LTM) on the atomic.
    e.capacity = cap;
    Resize(e.pos, (flags & 1) != 0, n * 3);          // FUN_00474db0(cap, 0xc)
    Resize(e.size, (flags & 2) != 0, n * 2);         // (cap, 8)
    Resize(e.col, (flags & 4) != 0, n);              // (cap, 4)
    Resize(e.mat, (flags & 8) != 0, n * 16);         // (cap, 0x40)
    Resize(e.rot, (flags & 0x10) != 0, n);           // (cap, 4)
    Resize(e.uv2, (flags & 0x20) != 0, n * 4);       // (cap * 2, 8)
    Resize(e.uv4, (flags & 0x40) != 0, n * 8);       // (cap * 4, 8)
}

// 0x004769a0
void SetPos(const float* p) {
    const std::uint32_t* s = p ? reinterpret_cast<const std::uint32_t*>(p) : k006925a8;
    g_stage.pos[0] = s[0]; g_stage.pos[1] = s[1]; g_stage.pos[2] = s[2];
}
// 0x004769d0
void SetSize(const float* p) {
    const std::uint32_t* s = p ? reinterpret_cast<const std::uint32_t*>(p) : k00613288;
    g_stage.size[0] = s[0]; g_stage.size[1] = s[1];
}
// 0x004769f0
void SetColour(const std::uint32_t* p) { g_stage.col = p ? *p : k00613290; }
// 0x00476a10 (default: the template, 0x00476a1a)
void SetMatrix(const std::uint32_t* p) {
    std::memcpy(g_stage.mat, p ? p : g_stage.tmpl, sizeof(g_stage.mat));
}
// 0x00476a30 (no null guard: the value itself)
void SetRotate(float r) { g_stage.rot = U(r); }
// 0x00476a40
void SetUv2(const float* p) {
    std::memcpy(g_stage.uv2, p ? static_cast<const void*>(p) : k00613294, sizeof(g_stage.uv2));
}
// 0x00476a80
void SetUv4(const float* p) {
    std::memcpy(g_stage.uv4, p ? static_cast<const void*>(p) : k006132a4, sizeof(g_stage.uv4));
}

// 0x00476ae0
void SetBatchB8(Emitter& e, const std::uint32_t* v) {
    const std::uint32_t* s = v ? v : k00613288;
    e.tank.f_b8[0] = s[0]; e.tank.f_b8[1] = s[1];
    e.tank.dirty |= 0x4000;
}
// 0x00476b30 (+ material 0 +4 at 0x00476b74; the port's material always exists)
void SetBatchColour(Emitter& e, const std::uint32_t* v) {
    e.tank.f_c4 = v ? *v : k00613290;
    e.tank.f_c4_set = true;
    e.tank.material_colour = e.tank.f_c4;
    e.tank.dirty |= 0x2000;
}
// 0x00476b90 (default: the template, 0x00476b9a)
void SetBatchMatrix(Emitter& e, const std::uint32_t* v) {
    std::memcpy(e.tank.f_f8, v ? v : g_stage.tmpl, sizeof(e.tank.f_f8));
    e.tank.dirty |= 0x8000;
}
// 0x00476be0
void SetBatchC0(Emitter& e, std::uint32_t v) {
    e.tank.f_c0 = v;
    e.tank.dirty |= 0x20000;
}
// 0x00476c10
void SetBatchUv2(Emitter& e, const std::uint32_t* v) {
    std::memcpy(e.tank.f_d8, v ? v : k00613294, 4 * sizeof(std::uint32_t));
    e.tank.dirty |= 0x80000;
}
// 0x00476c60
void SetBatchUv4(Emitter& e, const std::uint32_t* v) {
    std::memcpy(e.tank.f_d8, v ? v : k006132a4, 8 * sizeof(std::uint32_t));
    e.tank.dirty |= 0x100000;
}
// 0x00476cb0
void SetBlend(Emitter& e, std::int32_t src, std::int32_t dst) {
    e.tank.src_blend = src;
    e.tank.dst_blend = dst;
    e.tank.dirty |= 0x10000000u;
}

// 0x00476d00
void Submit(Emitter& e) {
    if (e.count >= e.capacity) return;
    const std::size_t i = static_cast<std::size_t>(e.count);
    if (!e.pos.empty()) std::memcpy(&e.pos[i * 3], g_stage.pos, 12);
    if (!e.size.empty()) std::memcpy(&e.size[i * 2], g_stage.size, 8);
    if (!e.col.empty()) e.col[i] = g_stage.col;
    if (!e.mat.empty()) std::memcpy(&e.mat[i * 16], g_stage.mat, 64);
    if (!e.rot.empty()) e.rot[i] = g_stage.rot;
    if (!e.uv2.empty()) std::memcpy(&e.uv2[i * 4], g_stage.uv2, 16);
    if (!e.uv4.empty()) std::memcpy(&e.uv4[i * 8], g_stage.uv4, 32);
    ++e.count;
}

// 0x00476df0. Each array goes through FUN_00535700 (lock, 0x40000000) and one FUN_00535910 (unlock) ends
// the batch. A lock on a flag the PTank lacks returns a null pointer, which the original then writes
// through; the port skips that copy (no caller pairs such flags).
void Flush(Emitter& e) {
    PTank& t = e.tank;
    t.dirty |= 0x800000u;
    t.count = e.count;
    if (e.count == 0) return;
    const std::size_t n = static_cast<std::size_t>(e.count);
    auto copy = [&](const std::vector<std::uint32_t>& src, std::vector<std::uint32_t>& dst,
                    std::uint32_t lock, std::size_t per) {
        if (src.empty()) return;
        if ((t.data_flags & lock) == 0 || dst.empty()) return;
        t.lock_bits |= lock | 0x40000000u;
        std::memcpy(dst.data(), src.data(), n * per * 4);
    };
    copy(e.pos, t.pos, 1, 3);
    copy(e.size, t.size, 4, 2);
    copy(e.col, t.col, 2, 1);
    copy(e.mat, t.mat, 8, 16);
    copy(e.rot, t.rot, 0x20, 1);
    copy(e.uv2, t.uv2, 0x80, 4);
    copy(e.uv4, t.uv4, 0x100, 8);
    t.dirty |= t.lock_bits;                          // FUN_00535910
    t.lock_bits = 0;
    e.count = 0;
}

// 0x00536780
Callbacks SelectCallbacks(std::uint32_t f) {
    Callbacks c{};
    c.render = 0x00537fb0;
    const bool center = (f & 0x1000000) == 0x1000000;
    if ((f & 8) == 8) {
        c.pos = center ? 0x005379c0 : 0x00537890;
    } else if ((f & 0x8000) != 0x8000 && (f & 0x20) == 0x20) {
        if ((f & 4) == 4) c.pos = center ? 0x00537610 : 0x00536fe0;
        else              c.pos = center ? 0x005371f0 : 0x00536c90;
    } else if ((f & 0x8000) != 0x8000 && (f & 4) == 4) {
        c.pos = center ? 0x00537460 : 0x00536e90;
    } else {
        c.pos = center ? 0x00536ad0 : 0x00536940;
    }
    if ((f & 0x80) == 0x80)            c.uv = 0x00537d70;
    else if ((f & 0x100) == 0x100)     c.uv = 0x00537de0;
    else if ((f & 0x80000) == 0x80000) c.uv = 0x00537e60;
    else if ((f & 0x100000) == 0x100000) c.uv = 0x00537ed0;
    else                               c.uv = 0;
    if ((f & 2) == 2)                  c.col = 0x00537b20;
    else if ((f & 0x40) == 0x40)       c.col = 0x00537b80;
    else if ((f & 0x40000) == 0x40000) c.col = 0x00537c40;
    else                               c.col = 0x00537d20;
    c.end = 0x00538300;
    c.normal = ((f & 0x10) == 0x10) ? 0x00537f40 : 0;
    return c;
}

bool Instance(const PTank& t, const float r[3], const float u[3], Streams& out) {
    const Callbacks cb = SelectCallbacks(t.data_flags);
    if (cb.pos != 0x00536e90 && cb.pos != 0x00536fe0) return false;
    if (cb.uv != 0 && cb.uv != 0x00537d70 && cb.uv != 0x00537e60) return false;
    if (cb.col != 0x00537b20 && cb.col != 0x00537d20) return false;
    if (cb.col == 0x00537d20 && !t.f_c4_set) return false;   // +0xc4 initial value not transcribed
    if (cb.normal != 0) return false;
    const int n = t.count;
    if (n <= 0) { out.xyz.clear(); out.uv.clear(); out.argb.clear(); return true; }
    out.xyz.assign(static_cast<std::size_t>(n) * 12, 0.f);
    out.uv.assign(cb.uv ? static_cast<std::size_t>(n) * 8 : 0, 0.f);
    out.argb.assign(static_cast<std::size_t>(n) * 4, 0u);
    float* o = out.xyz.data();
    for (int i = 0; i < n; ++i, o += 12) {
        const float px = F(t.pos[i * 3]), py = F(t.pos[i * 3 + 1]), pz = F(t.pos[i * 3 + 2]);
        const float sx = F(t.size[i * 2]), sy = F(t.size[i * 2 + 1]);
        float Xx, Xy, Xz, Yx, Yy, Yz;
        if (cb.pos == 0x00536e90) {
            // 0x00536ed0..0x00536f11
            const float hx = sx * k005cc32c;
            Xx = hx * r[0]; Xy = hx * r[1]; Xz = hx * r[2];
            const float hy = sy * k005cc32c;
            Yx = hy * u[0]; Yy = hy * u[1]; Yz = hy * u[2];
        } else {
            // 0x00537042..0x00537133: fsincos, then the rotated pair scaled by size * 0.5
            float s, c;
            SinCos24(F(t.rot[i]), &s, &c);
            const float ns = -s;
            const float r0x = (ns * u[0]) + (c * r[0]);
            const float r0y = (ns * u[1]) + (c * r[1]);
            const float r0z = (ns * u[2]) + (c * r[2]);
            const float u0x = (s * r[0]) + (c * u[0]);
            const float u0y = (s * r[1]) + (c * u[1]);
            const float u0z = (s * r[2]) + (c * u[2]);
            Xx = (r0x * sx) * k005cc32c; Xy = (r0y * sx) * k005cc32c; Xz = (r0z * sx) * k005cc32c;
            Yx = (u0x * sy) * k005cc32c; Yy = (u0y * sy) * k005cc32c; Yz = (u0z * sy) * k005cc32c;
        }
        // v0 = (-X - Y) + p, v1 = (X - Y) + p, v2 = (Y + X) + p, v3 = (Y - X) + p (both instancers)
        o[0] = (-Xx - Yx) + px;  o[1] = (-Xy - Yy) + py;  o[2] = (-Xz - Yz) + pz;
        o[3] = (Xx - Yx) + px;   o[4] = (Xy - Yy) + py;   o[5] = (Xz - Yz) + pz;
        o[6] = (Yx + Xx) + px;   o[7] = (Yy + Xy) + py;   o[8] = (Yz + Xz) + pz;
        o[9] = (Yx - Xx) + px;   o[10] = (Yy - Xy) + py;  o[11] = (Yz - Xz) + pz;
    }
    // uv: (u0,v1) (u1,v1) (u1,v0) (u0,v0) from a per-particle (u0,v0,u1,v1) (0x00537d70) or plugin +0xd8
    // (0x00537e60)
    for (int i = 0; i < n && cb.uv; ++i) {
        const std::uint32_t* q = (cb.uv == 0x00537d70) ? &t.uv2[static_cast<std::size_t>(i) * 4] : t.f_d8;
        float* w = &out.uv[static_cast<std::size_t>(i) * 8];
        w[0] = F(q[0]); w[1] = F(q[3]);
        w[2] = F(q[2]); w[3] = F(q[3]);
        w[4] = F(q[2]); w[5] = F(q[1]);
        w[6] = F(q[0]); w[7] = F(q[1]);
    }
    // colour: the RGBA bytes b0..b3 packed b3<<24 | b0<<16 | b1<<8 | b2 on all four vertices
    for (int i = 0; i < n; ++i) {
        const std::uint32_t v = (cb.col == 0x00537b20) ? t.col[static_cast<std::size_t>(i)] : t.f_c4;
        const std::uint32_t b0 = v & 0xff, b1 = (v >> 8) & 0xff, b2 = (v >> 16) & 0xff, b3 = v >> 24;
        const std::uint32_t argb = (b3 << 24) | (b0 << 16) | (b1 << 8) | b2;
        for (int k = 0; k < 4; ++k) out.argb[static_cast<std::size_t>(i) * 4 + k] = argb;
    }
    return true;
}

}  // namespace Rfx
}  // namespace mashed_re
