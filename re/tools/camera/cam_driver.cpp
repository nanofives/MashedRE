// Offline driver for the verbatim race-camera port (FUN_00446520 family).
//
// WHY: RaceCamera (Race/RaceCamera.cpp, RVAs 0x00446520 / 0x00441820 / 0x0040e180
// / 0x00410d10) runs every frame in the standalone at TrackRenderer.cpp:3488, and its
// pose is then discarded -- `race_cam_.pos()` and `.target()` have ZERO call sites in
// the tree. So the port has never been checked against the original at all, and its
// three RVAs sit at C2 with camera C4 = 0 of 14 rows.
//
// This driver feeds the port the ORIGINAL's own per-frame inputs, captured live by
// re/frida/camera_probe.py, and prints the port's outputs so they can be diffed
// against the original's. No game run, no wiring, no renderer: it isolates "is the
// verbatim port faithful?" from "is its output the world camera?", which is a
// separate and still-open question (verify/d1_carproj/RESULT.md).
//
// Build:  re/tools/camera/build_cam_driver.bat
// Run:    cam_driver.exe <trace_v2.csv> <nodes.txt> <LED.piz> > port_out.csv
//
// Two adapter substitutions, stated because they are NOT captured:
//   * `active` -- the original calls IsCarSlotActive (0x0040e370), a table walk we
//     do not replicate offline. Derived as (alive != -1). Sound for these captures
//     because every row has alive == 1 on all four cars; assert it rather than
//     assume it.
//   * `dead_ms` -- the original holds an INT tick count and does `fild` then
//     `fdiv 1000.0` (0x00447273); our port's field is a float. Passed straight
//     through. Every captured row has dead == 0, so this path is not exercised
//     here; asserted too.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>

#include "../../../mashedmod/src/mashed_re/Race/RaceCamera.h"

using mashed_re::Race::RaceCamera;
using mashed_re::Race::RaceCamCar;
using mashed_re::Race::RaceCamNode;

// ---------------------------------------------------------------- CSV by header

struct Csv {
    std::unordered_map<std::string, int> col;
    std::vector<std::vector<double>> rows;

    static void split(char* line, std::vector<char*>& out) {
        out.clear();
        char* p = line;
        out.push_back(p);
        for (; *p; ++p)
            if (*p == ',') { *p = 0; out.push_back(p + 1); }
    }

    bool load(const char* path) {
        std::FILE* f = std::fopen(path, "r");
        if (!f) { std::fprintf(stderr, "cannot open %s\n", path); return false; }
        char line[8192];
        std::vector<char*> f2;
        if (!std::fgets(line, sizeof(line), f)) { std::fclose(f); return false; }
        // strip EOL
        for (char* p = line; *p; ++p) if (*p == '\r' || *p == '\n') { *p = 0; break; }
        split(line, f2);
        for (int i = 0; i < (int)f2.size(); ++i) col[f2[i]] = i;
        while (std::fgets(line, sizeof(line), f)) {
            for (char* p = line; *p; ++p) if (*p == '\r' || *p == '\n') { *p = 0; break; }
            if (!line[0]) continue;
            split(line, f2);
            std::vector<double> r;
            r.reserve(f2.size());
            for (char* s : f2) r.push_back(std::atof(s));
            rows.push_back(std::move(r));
        }
        std::fclose(f);
        return true;
    }

    int idx(const char* name) const {
        auto it = col.find(name);
        if (it == col.end()) {
            std::fprintf(stderr, "trace is missing required column '%s'\n", name);
            std::exit(2);
        }
        return it->second;
    }
};

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr,
            "usage: %s <trace_v2.csv> <nodes.txt> <LED.piz>\n", argv[0]);
        return 2;
    }

    Csv t;
    if (!t.load(argv[1])) return 2;
    if (t.rows.empty()) { std::fprintf(stderr, "trace has no rows\n"); return 2; }

    // nodes.txt: one node per line, 9 floats -- dir xyz, c0 xyz, c3 xyz.
    std::vector<RaceCamNode> nodes;
    {
        std::FILE* f = std::fopen(argv[2], "r");
        if (!f) { std::fprintf(stderr, "cannot open %s\n", argv[2]); return 2; }
        RaceCamNode n{};
        while (std::fscanf(f, "%f %f %f %f %f %f %f %f %f",
                           &n.dir[0], &n.dir[1], &n.dir[2],
                           &n.c0[0], &n.c0[1], &n.c0[2],
                           &n.c3[0], &n.c3[1], &n.c3[2]) == 9)
            nodes.push_back(n);
        std::fclose(f);
    }
    if (nodes.empty()) { std::fprintf(stderr, "no nodes parsed\n"); return 2; }

    const int i_tt = t.idx("track_type");
    const int track_type = (int)t.rows[0][i_tt];

    RaceCamera cam;
    const bool led = cam.LoadLed(argv[3], track_type);
    cam.SetNodes(nodes.data(), (int)nodes.size());
    std::fprintf(stderr,
        "driver: %zu rows, %zu nodes, track_type %d, LoadLed(LE%d.LED) = %s\n",
        t.rows.size(), nodes.size(), track_type, track_type, led ? "OK" : "FAILED");
    if (!led)
        std::fprintf(stderr,
            "  NOTE: no LED entries -> every node takes the -25deg fallback branch\n"
            "  (RaceCamera.cpp:118-125). The original's override table WAS populated\n"
            "  in this capture (elev 15.0), so a FAILED load means the port and the\n"
            "  original are on different branches and the diff measures that, not the\n"
            "  camera law.\n");

    // column indices
    const int i_dtb = t.idx("dtb"), i_jit = t.idx("jit"), i_tk = t.idx("ticks");
    const int i_rst = t.idx("reset"), i_oh = t.idx("overhead"), i_mode = t.idx("mode");
    int i_cx[4], i_cy[4], i_cz[4], i_vx[4], i_vy[4], i_vz[4];
    int i_alive[4], i_dead[4], i_deadt[4], i_prog[4], i_pct[4];
    for (int i = 0; i < 4; ++i) {
        char b[32];
#define COL(dst, suffix) \
        std::snprintf(b, sizeof(b), "c%d" suffix, i); dst[i] = t.idx(b)
        COL(i_cx, "x");     COL(i_cy, "y");     COL(i_cz, "z");
        COL(i_vx, "vx");    COL(i_vy, "vy");    COL(i_vz, "vz");
        COL(i_alive, "alive"); COL(i_dead, "dead"); COL(i_deadt, "deadt");
        COL(i_prog, "prog");   COL(i_pct, "pct");
#undef COL
    }

    std::printf("row,ppx,ppy,ppz,pdx,pdy,pdz,pzoom,pvw,ppairA,ppairB,mode,reset\n");

    long adapter_active_violations = 0, adapter_dead_violations = 0;

    for (size_t r = 0; r < t.rows.size(); ++r) {
        const std::vector<double>& v = t.rows[r];
        RaceCamCar cars[4];
        for (int i = 0; i < 4; ++i) {
            cars[i].pos[0] = (float)v[i_cx[i]];
            cars[i].pos[1] = (float)v[i_cy[i]];
            cars[i].pos[2] = (float)v[i_cz[i]];
            cars[i].vel[0] = (float)v[i_vx[i]];
            cars[i].vel[1] = (float)v[i_vy[i]];
            cars[i].vel[2] = (float)v[i_vz[i]];
            const int alive = (int)v[i_alive[i]];
            cars[i].active    = (alive != -1);
            cars[i].alive     = (alive == 1);
            cars[i].dead_flag = ((int)v[i_dead[i]] != 0);
            cars[i].dead_ms   = (float)v[i_deadt[i]];
            cars[i].path_prog = (float)v[i_prog[i]];
            cars[i].race_pct  = (float)v[i_pct[i]];
            if (alive != 1) ++adapter_active_violations;
            if ((int)v[i_dead[i]] != 0) ++adapter_dead_violations;
        }

        cam.Update(cars, track_type, (float)v[i_dtb],
                   (std::uint32_t)v[i_tk], (float)v[i_jit],
                   (int)v[i_rst] != 0, (int)v[i_oh] != 0);

        int pa = -1, pb = -1;
        RaceCamera::MostSeparatedPair(cars, &pa, &pb);

        const float* p = cam.pos();
        const float* g = cam.target();
        std::printf("%zu,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d,%d\n",
                    r, p[0], p[1], p[2], g[0], g[1], g[2],
                    cam.required_zoom(), cam.view_window(), pa, pb,
                    (int)v[i_mode], (int)v[i_rst]);
    }

    if (adapter_active_violations)
        std::fprintf(stderr,
            "*** %ld car-rows had alive != 1. The `active` adapter substitution\n"
            "    (active = alive != -1) is only sound when every row is alive == 1.\n"
            "    Treat this run's pair selection as unverified.\n",
            adapter_active_violations);
    if (adapter_dead_violations)
        std::fprintf(stderr,
            "*** %ld car-rows had dead != 0, so the int-vs-float dead_ms adapter\n"
            "    divergence IS exercised in this run. Those rows are suspect.\n",
            adapter_dead_violations);
    if (!adapter_active_violations && !adapter_dead_violations)
        std::fprintf(stderr, "adapter assumptions hold: all rows alive==1, dead==0\n");
    return 0;
}
