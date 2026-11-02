// emit_port.cpp — port side of arm A1 (PREREG_D4-DC-RFX-CORE.md), the D5-RFX-EMITTERS harness
// (verify/rfx_emitters_20261028/emit_port.cpp) moved onto the shared core's layouts. Same case file, same dumped
// values in the same order, so its output is compared against the committed original-side output. The staging
// globals and [0x007dc57c] are absolute addresses in the core; the exe's image-pad provides them, this harness
// maps the two pages itself (zero-filled, as the pad and the original's BSS are).
// Build (vcvars32, x86): cl /nologo /EHsc /std:c++17 /O2 /W3 emit_port.cpp <RfxEmitter.cpp> <Stubs/HookSystemNoOp.cpp>
#include "../../mashedmod/src/mashed_re/Render/RfxEmitter.h"

#include <windows.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace mashed_re::Rfx;

static FILE* g_in;
static FILE* g_out;
static std::uint32_t rd() { unsigned v = 0; if (std::fscanf(g_in, "%x", &v) != 1) v = 0; return v; }
static void wr(std::uint32_t v) { std::fprintf(g_out, " %08x", v); }
static void wrv(const std::vector<std::uint32_t>& v) { for (auto x : v) wr(x); }
static float F(std::uint32_t u) { float f; std::memcpy(&f, &u, 4); return f; }
static std::uint32_t U(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); return u; }

static void dump_create(const Emitter& e) {
    const PTank& t = e.tank;
    wr(e.batch.flags); wr(static_cast<std::uint32_t>(e.batch.capacity)); wr(static_cast<std::uint32_t>(e.batch.count));
    // allocated sizes in bytes, the FUN_00474db0 n * size of each array (0 = not allocated)
    wr(static_cast<std::uint32_t>(e.b_pos.size() * 4)); wr(static_cast<std::uint32_t>(e.b_size.size() * 4));
    wr(static_cast<std::uint32_t>(e.b_col.size() * 4)); wr(static_cast<std::uint32_t>(e.b_mat.size() * 4));
    wr(static_cast<std::uint32_t>(e.b_rot.size() * 4)); wr(static_cast<std::uint32_t>(e.b_uv2.size() * 4));
    wr(static_cast<std::uint32_t>(e.b_uv4.size() * 4));
    wr(static_cast<std::uint32_t>(t.max_count)); wr(static_cast<std::uint32_t>(t.count)); wr(t.dirty);
    wr(t.data_flags); wr(static_cast<std::uint32_t>(t.src_blend)); wr(static_cast<std::uint32_t>(t.dst_blend));
    wr(static_cast<std::uint32_t>(t.vtx_alpha)); wr(t.f_b8[0]); wr(t.f_b8[1]); wr(t.f_c0); wr(t.f_c4);
    for (auto x : t.f_d8) wr(x);
    for (auto x : t.f_f8) wr(x);
    wr(e.mat0.colour);
    const std::uint32_t* tmpl = reinterpret_cast<const std::uint32_t*>(0x00692558u);
    for (int i = 0; i < 16; ++i) wr(tmpl[i]);
}

// The exe's B17 image-pad (exe_main.cpp), the same way: linked at /BASE:0x10000 /FIXED, this zero-init array is
// the image's .bss and covers the original's arena, so 0x006924d8.. and 0x007dc57c are ours, zero-filled, from load.
extern "C" char g_pad[0x00A00000];
char g_pad[0x00A00000];

static bool covered(std::uintptr_t a, std::size_t n) {
    const std::uintptr_t b = reinterpret_cast<std::uintptr_t>(g_pad);
    if (a >= b && a + n <= b + sizeof(g_pad)) return true;
    std::fprintf(stderr, "emit_port: 0x%08x not inside the pad [%p, +0x%x)\n", static_cast<unsigned>(a),
                 static_cast<void*>(g_pad), static_cast<unsigned>(sizeof(g_pad)));
    return false;
}

int main(int argc, char** argv) {
    if (argc < 3) return 2;
    if (!covered(0x006924d8u, 0xd0) || !covered(0x007dc57cu, 4)) return 3;
    g_in = std::fopen(argv[1], "r");
    g_out = std::fopen(argv[2], "w");
    if (!g_in || !g_out) return 2;
    char tag[8];
    static Emitter e;
    while (std::fscanf(g_in, "%7s", tag) == 1) {
        const std::string t = tag;
        if (t == "C") {                                  // create
            const std::uint32_t fl = rd(); const std::int32_t cap = static_cast<std::int32_t>(rd());
            Create(e, fl, cap, nullptr);
            std::fprintf(g_out, "C");
            dump_create(e);
            std::fprintf(g_out, "\n");
        } else if (t == "S") {                           // setter / submit / flush sequence
            const std::uint32_t fl = rd(); const std::int32_t cap = static_cast<std::int32_t>(rd());
            const int nops = static_cast<int>(rd());
            Create(e, fl, cap, nullptr);
            for (int k = 0; k < nops; ++k) {
                const std::uint32_t op = rd();
                std::uint32_t a[16];
                switch (op) {
                case 1: for (int i = 0; i < 3; ++i) a[i] = rd(); SetPos(reinterpret_cast<float*>(a)); break;
                case 2: SetPos(nullptr); break;
                case 3: for (int i = 0; i < 2; ++i) a[i] = rd(); SetSize(reinterpret_cast<float*>(a)); break;
                case 4: SetSize(nullptr); break;
                case 5: a[0] = rd(); SetColour(a); break;
                case 6: SetColour(nullptr); break;
                case 7: for (int i = 0; i < 16; ++i) a[i] = rd(); SetMatrix(a); break;
                case 8: SetMatrix(nullptr); break;
                case 9: SetRotate(F(rd())); break;
                case 10: for (int i = 0; i < 4; ++i) a[i] = rd(); SetUv2(reinterpret_cast<float*>(a)); break;
                case 11: SetUv2(nullptr); break;
                case 12: for (int i = 0; i < 8; ++i) a[i] = rd(); SetUv4(reinterpret_cast<float*>(a)); break;
                case 13: SetUv4(nullptr); break;
                case 14: Submit(e); break;
                case 15: Flush(e); break;
                default: break;
                }
            }
            std::fprintf(g_out, "S");
            wr(static_cast<std::uint32_t>(e.batch.count));
            wrv(e.b_pos); wrv(e.b_size); wrv(e.b_col); wrv(e.b_mat); wrv(e.b_rot); wrv(e.b_uv2); wrv(e.b_uv4);
            wr(static_cast<std::uint32_t>(e.tank.count)); wr(e.tank.dirty);
            wrv(e.t_pos); wrv(e.t_size); wrv(e.t_col); wrv(e.t_mat); wrv(e.t_rot);
            wrv(e.t_uv2); wrv(e.t_uv4);
            std::fprintf(g_out, "\n");
        } else if (t == "L") {                           // selector
            const Callbacks c = SelectCallbacks(rd());
            std::fprintf(g_out, "L");
            wr(c.render); wr(c.pos); wr(c.uv); wr(c.col); wr(c.normal); wr(c.end);
            std::fprintf(g_out, "\n");
        } else if (t == "I") {                           // instance
            static PTank k;
            k = PTank{};
            k.data_flags = rd();
            k.count = static_cast<std::int32_t>(rd());
            float r[3], u[3];
            for (float& x : r) x = F(rd());
            for (float& x : u) x = F(rd());
            for (auto& x : k.f_d8) x = rd();
            k.f_c4 = rd();
            const std::size_t n = static_cast<std::size_t>(k.count);
            std::vector<std::uint32_t> pos(n * 3 + 3), size(n * 2 + 2), rot(n + 1), col(n + 1), uv2(n * 4 + 4);
            for (std::size_t i = 0; i < n; ++i) {
                for (int j = 0; j < 3; ++j) pos[i * 3 + j] = rd();
                for (int j = 0; j < 2; ++j) size[i * 2 + j] = rd();
                rot[i] = rd(); col[i] = rd();
                for (int j = 0; j < 4; ++j) uv2[i * 4 + j] = rd();
            }
            k.pos = {pos.data(), 12}; k.size = {size.data(), 8}; k.rot = {rot.data(), 4};
            k.col = {col.data(), 4}; k.uv2 = {uv2.data(), 16};
            Streams s;
            const bool ok = Instance(k, true, r, u, s);
            std::fprintf(g_out, "I");
            wr(ok ? 1u : 0u);
            for (float x : s.xyz) wr(U(x));
            for (float x : s.uv) wr(U(x));
            for (auto x : s.argb) wr(x);
            std::fprintf(g_out, "\n");
        }
    }
    std::fclose(g_out);
    return 0;
}
