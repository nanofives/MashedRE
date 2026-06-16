// Runtime check of the WS-F C++ parsers against known Arctic asset values
// (the Python re/tools/rw_track_data.py is the byte-truth reference). Build:
//   cl /EHsc /I.. trackdata_test.cpp ..\TrackData.cpp
//   trackdata_test.exe <dir-of-extracted-arctic-assets>
#include "../TrackData.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

using namespace mashed_re::Track;

static int g_fail = 0;
static std::vector<std::uint8_t> slurp(const std::string& p) {
    std::FILE* f = std::fopen(p.c_str(), "rb");
    if (!f) { std::printf("  [skip] cannot open %s\n", p.c_str()); return {}; }
    std::fseek(f, 0, SEEK_END); long n = std::ftell(f); std::fseek(f, 0, SEEK_SET);
    std::vector<std::uint8_t> b(n > 0 ? n : 0);
    if (n > 0) { size_t rd = std::fread(b.data(), 1, n, f); b.resize(rd); }
    std::fclose(f); return b;
}
static void check(const char* what, bool ok) {
    std::printf("  %s %s\n", ok ? "[ok]  " : "[FAIL]", what);
    if (!ok) ++g_fail;
}
static bool near(float a, float b, float e = 1e-2f) { return std::fabs(a - b) < e; }

int main(int argc, char** argv) {
    std::string dir = argc > 1 ? argv[1] : ".";
    if (!dir.empty() && dir.back() != '/' && dir.back() != '\\') dir += "/";

    std::printf("F1 .SPL WAVE1.SPL\n");
    { auto d = slurp(dir + "WAVE1.SPL"); Spline s;
      bool ok = !d.empty() && s.Parse(d.data(), d.size());
      check("parses", ok);
      check("40 points", ok && s.num_points == 40);
      check("constant-Y water (~-3.618)", ok && s.constant_y && near(s.y, -3.618f));
    }

    std::printf("F2 .ANM H_ANIM_1.ANM\n");
    { auto d = slurp(dir + "H_ANIM_1.ANM"); HAnim a;
      bool ok = !d.empty() && a.Parse(d.data(), d.size());
      check("parses", ok);
      check("31 frames", ok && a.frames.size() == 31);
      check("duration ~39.98s", ok && near(a.duration, 39.9833f));
      check("frame0 t=0", ok && near(a.frames[0].time, 0.f));
      // sampler: at t=0 returns frame0 translation
      float p[3], q[4]; if (ok) a.Sample(0.f, p, q);
      check("Sample(0) == frame0.t", ok && near(p[0], a.frames[0].t[0]) &&
            near(p[2], a.frames[0].t[2]));
      // sampler: quaternion stays unit
      float qn = ok ? std::sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]) : 0.f;
      check("Sample quat unit", ok && near(qn, 1.f, 2e-3f));
    }

    std::printf("F3 .UVA SEA.UVA\n");
    { auto d = slurp(dir + "SEA.UVA"); UVDict u;
      bool ok = !d.empty() && u.Parse(d.data(), d.size());
      check("parses", ok);
      check("2 entries", ok && u.anims.size() == 2);
      check("entry0 name bmp_Sea_M", ok && u.anims.size() > 0 && u.anims[0].name == "bmp_Sea_M");
      check("Sea stem -> 'Sea'", ok && u.anims.size() > 0 && u.anims[0].TextureStem() == "Sea");
      check("Sea dv/dt ~0.1818", ok && u.anims.size() > 0 && near(u.anims[0].dv_dt, 0.1818f));
      check("Sky dv/dt ~-0.0303", ok && u.anims.size() > 1 && near(u.anims[1].dv_dt, -0.0303f));
    }

    std::printf("F5 .MTS CRATE.MTS\n");
    { auto d = slurp(dir + "CRATE.MTS"); MtxList m;
      bool ok = !d.empty() && m.Parse(d.data(), d.size());
      check("parses", ok);
      check("5 matrices", ok && m.items.size() == 5);
      check("crate0 pos (-28.26,0.31,5.09)", ok && m.items.size() > 0 &&
            near(m.items[0].m[9], -28.2608f) && near(m.items[0].m[11], 5.0897f));
      check("crate0 type=3 (orthonormal)", ok && m.items.size() > 0 && m.items[0].type == 3);
    }

    std::printf("F4 LAPDATA.LUA\n");
    { auto d = slurp(dir + "LAPDATA.LUA"); LapData l;
      bool ok = !d.empty() && l.Parse(reinterpret_cast<const char*>(d.data()), d.size());
      check("parses", ok);
      check("lap_lines [0,93]", ok && l.lap_lines.size() == 2 &&
            l.lap_lines[0] == 0 && l.lap_lines[1] == 93);
      check("finish_gate()==0", ok && l.finish_gate() == 0);
      check("2 split sectors", ok && l.split_sectors.size() == 2);
      check("7 safe-start ranges", ok && l.safe_start_lines.size() == 7);
    }

    std::printf("\n%s (%d failures)\n", g_fail ? "FAILED" : "ALL PASS", g_fail);
    return g_fail ? 1 : 0;
}
