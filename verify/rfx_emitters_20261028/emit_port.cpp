// emit_port.cpp — port side of arm A1 (PREREG_D5-RFX-EMITTERS.md section 3). Reads the case file
// emit_unicorn.py writes, runs mashedmod/src/mashed_re/Render/RfxEmitter.cpp on it, writes one line of hex
// dwords per case. Build: cl /nologo /EHsc /std:c++17 /O2 emit_port.cpp <RfxEmitter.cpp> (vcvars32, x86).
#include "../../mashedmod/src/mashed_re/Render/RfxEmitter.h"

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
    wr(e.flags); wr(static_cast<std::uint32_t>(e.capacity)); wr(static_cast<std::uint32_t>(e.count));
    // allocated sizes in bytes, the FUN_00474db0 n * size of each array (0 = not allocated)
    wr(static_cast<std::uint32_t>(e.pos.size() * 4)); wr(static_cast<std::uint32_t>(e.size.size() * 4));
    wr(static_cast<std::uint32_t>(e.col.size() * 4)); wr(static_cast<std::uint32_t>(e.mat.size() * 4));
    wr(static_cast<std::uint32_t>(e.rot.size() * 4)); wr(static_cast<std::uint32_t>(e.uv2.size() * 4));
    wr(static_cast<std::uint32_t>(e.uv4.size() * 4));
    wr(static_cast<std::uint32_t>(t.max_count)); wr(static_cast<std::uint32_t>(t.count)); wr(t.dirty);
    wr(t.data_flags); wr(static_cast<std::uint32_t>(t.src_blend)); wr(static_cast<std::uint32_t>(t.dst_blend));
    wr(static_cast<std::uint32_t>(t.vtx_alpha)); wr(t.f_b8[0]); wr(t.f_b8[1]); wr(t.f_c0); wr(t.f_c4);
    for (auto x : t.f_d8) wr(x);
    for (auto x : t.f_f8) wr(x);
    wr(t.material_colour);
    for (auto x : Stage().tmpl) wr(x);
}

int main(int argc, char** argv) {
    if (argc < 3) return 2;
    g_in = std::fopen(argv[1], "r");
    g_out = std::fopen(argv[2], "w");
    if (!g_in || !g_out) return 2;
    char tag[8];
    Emitter e;
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
            wr(static_cast<std::uint32_t>(e.count));
            wrv(e.pos); wrv(e.size); wrv(e.col); wrv(e.mat); wrv(e.rot); wrv(e.uv2); wrv(e.uv4);
            wr(static_cast<std::uint32_t>(e.tank.count)); wr(e.tank.dirty);
            wrv(e.tank.pos); wrv(e.tank.size); wrv(e.tank.col); wrv(e.tank.mat); wrv(e.tank.rot);
            wrv(e.tank.uv2); wrv(e.tank.uv4);
            std::fprintf(g_out, "\n");
        } else if (t == "L") {                           // selector
            const Callbacks c = SelectCallbacks(rd());
            std::fprintf(g_out, "L");
            wr(c.render); wr(c.pos); wr(c.uv); wr(c.col); wr(c.normal); wr(c.end);
            std::fprintf(g_out, "\n");
        } else if (t == "I") {                           // instance
            PTank k;
            k.data_flags = rd();
            k.count = static_cast<std::int32_t>(rd());
            float r[3], u[3];
            for (float& x : r) x = F(rd());
            for (float& x : u) x = F(rd());
            for (auto& x : k.f_d8) x = rd();
            k.f_c4 = rd(); k.f_c4_set = true;
            const std::size_t n = static_cast<std::size_t>(k.count);
            k.pos.resize(n * 3); k.size.resize(n * 2); k.rot.resize(n); k.col.resize(n); k.uv2.resize(n * 4);
            for (std::size_t i = 0; i < n; ++i) {
                for (int j = 0; j < 3; ++j) k.pos[i * 3 + j] = rd();
                for (int j = 0; j < 2; ++j) k.size[i * 2 + j] = rd();
                k.rot[i] = rd(); k.col[i] = rd();
                for (int j = 0; j < 4; ++j) k.uv2[i * 4 + j] = rd();
            }
            Streams s;
            const bool ok = Instance(k, r, u, s);
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
