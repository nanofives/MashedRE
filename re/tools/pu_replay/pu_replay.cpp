// pu_replay.cpp -- D3 WS-D decision-logic replay (2026-09-26).
//
// Feeds the ORIGINAL's per-frame power-up inputs, as captured by
// re/frida/scenario_launch.py --statediff-puhook, into the PORTED dispatcher
// (mashedmod/src/mashed_re/Powerup/PowerupSystem.cpp + PowerupEffects.cpp, the
// same TUs the shipping exe links) and writes the port's per-frame state through
// MASHED_PU_STEPDUMP. re/tools/pu_diff.py then compares the two CSVs.
//
// Inputs replayed per dispatcher call (state == 6 only, one slot):
//   activation (act column, the original's FUN_0045c010(slot,code) call),
//   ctrl byte +7 (cur3) and +8 (cur4) -- prev is NOT fed: the port derives it
//   with SetInput's cook-shadow latch, and pu_diff checks it against prev3/prev4,
//   dt (DAT_007f100c).
// One input is INJECTED, not measured: OIL's distance gate (orig trail
// DAT_0068a290, IPowerupBackend::OilDropDue) returns "due" exactly on the calls
// where the original's supply decreased. The replay therefore tests the OIL
// decrement amount and deactivation timing, not the distance rule.
//
// Usage: pu_replay.exe <orig.puhook.csv> <slot>   (env MASHED_PU_STEPDUMP=<out.csv>)
#include "Powerup/PowerupSystem.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

using namespace mashed_re::Powerup;

namespace {

struct Row {
    int state = 0, slot = 0, cur3 = 0, cur4 = 0, codePre = -1;
    float dt = 0.f;
    int act = -1;
    bool oilDue = false;
};

std::vector<std::string> Split(const std::string& s) {
    std::vector<std::string> out; std::string cur;
    for (char c : s) { if (c == ',') { out.push_back(cur); cur.clear(); } else if (c != '\r' && c != '\n') cur += c; }
    out.push_back(cur);
    return out;
}

float Word0(const std::string& hex) {   // first dword of a pool-record hex dump, as float
    if (hex.size() < 8) return 0.f;
    unsigned char b[4];
    for (int i = 0; i < 4; ++i) b[i] = static_cast<unsigned char>(std::strtoul(hex.substr(i * 2, 2).c_str(), nullptr, 16));
    float f; std::memcpy(&f, b, 4); return f;
}

struct Backend : IPowerupBackend {
    HostCar car[4];
    bool due = false;
    const HostCar& Player() const override { return car[0]; }
    int AiCount() const override { return 3; }
    HostCar& Ai(int i) override { return car[1 + i]; }
    void SpawnMissile(const float*, const float*, int) override {}
    void SpawnMortar(const float*, const float*, int) override {}
    void DropHazard(const float*, bool) override {}
    void HitscanForward(const float*, const float*) override {}
    void SpreadCone(const float*, const float*) override {}
    void FlameJet(const float*, const float*, bool) override {}
    void BlindFlash(const float*) override {}
    void DropOilSlick(const float*) override {}
    void EffectEnd(int, const float*) override {}
    void SfxByName(const char*, float) override {}
    bool OilDropDue(int, const float*) override { return due; }
};

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) { std::fprintf(stderr, "usage: pu_replay <orig.puhook.csv> <slot>\n"); return 2; }
    const int want = std::atoi(argv[2]);
    std::FILE* f = std::fopen(argv[1], "r");
    if (!f) { std::fprintf(stderr, "cannot open %s\n", argv[1]); return 2; }
    char line[8192];
    std::vector<std::string> hdr;
    std::vector<Row> rows;
    // Dispatcher calls where the slot held nothing produce no row in the capture;
    // they still ran (the slot was idle), so replay a quiet frame for each gap.
    long lastCall = -1;
    while (std::fgets(line, sizeof line, f)) {
        auto c = Split(line);
        if (hdr.empty()) { hdr = c; continue; }
        auto col = [&](const char* n) -> std::string {
            for (std::size_t i = 0; i < hdr.size(); ++i) if (hdr[i] == n) return i < c.size() ? c[i] : "";
            return "";
        };
        if (std::atoi(col("slot").c_str()) != want) continue;
        Row r;
        r.state = std::atoi(col("state").c_str());
        if (r.state != 6) continue;
        const long call = std::atol(col("call").c_str());
        if (lastCall >= 0) for (long g = lastCall + 1; g < call; ++g) { Row q; q.state = 6; q.dt = rows.empty() ? 1.f / 60.f : rows.back().dt; rows.push_back(q); }
        lastCall = call;
        r.slot = want;
        r.cur3 = std::atoi(col("cur3").c_str());
        r.cur4 = std::atoi(col("cur4").c_str());
        {   // dt: exact float bits "0x..." (captures from 2026-09-26 on) or decimal
            const std::string d = col("dt");
            if (d.size() > 2 && d[0] == '0' && d[1] == 'x') {
                const unsigned long u = std::strtoul(d.c_str() + 2, nullptr, 16);
                std::memcpy(&r.dt, &u, 4);
            } else r.dt = static_cast<float>(std::atof(d.c_str()));
        }
        r.codePre = std::atoi(col("code_pre").c_str());
        const std::string act = col("act");
        if (act.size() > 1 && act[0] == 'A') r.act = std::atoi(act.c_str() + 1);
        if (r.codePre == kOil && Word0(col("rec_post")) < Word0(col("rec_pre"))) r.oilDue = true;
        rows.push_back(r);
    }
    std::fclose(f);

    Backend be;
    PowerupSystem sys;
    sys.Init(&be);
    for (const Row& r : rows) {
        if (r.act >= 0) sys.Activate(want, r.act);
        sys.SetInput(want, r.cur3 != 0, r.cur4 != 0);
        be.due = r.oilDue;
        sys.Tick(r.dt, be.car, r.state);
    }
    std::printf("replayed %zu frames on slot %d\n", rows.size(), want);
    return 0;
}
