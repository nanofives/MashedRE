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
// D3 CONTACT (2026-09-28): a SECOND input is now injected, and the same way --
// the contact chain's two query leaves. With <orig.puhook.csv>'s sibling
// <orig.pucontact.csv> present, the replay feeds the ORIGINAL's MEASURED verdict
// for FUN_004b4cd0 (hit count) and FUN_0045c110 (surface gate) back into the
// ported Powerup/PowerupContact.cpp, keyed on (dispatcher call, call-site return
// address). The standalone has no collision world here, so what this measures is
// the CALL STRUCTURE and the STATE EFFECT of each outcome -- not the query. Every
// port call with no matching original row, and every original row the port never
// consumed, is counted and printed; that difference IS the criterion (c) verdict.
// The port's own rows go to MASHED_PU_CONTACTDUMP in the capture's exact column
// shape, so re/tools/pu_contact_report.py reads both sides identically.
//
// Usage: pu_replay.exe <orig.puhook.csv> <slot>   (env MASHED_PU_STEPDUMP=<out.csv>,
//        MASHED_PU_CONTACTDUMP=<out.pucontact.csv>)
#include "Powerup/PowerupSystem.h"
#include "Powerup/PowerupContact.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace mashed_re::Powerup;

namespace {

struct Row {
    int state = 0, slot = 0, cur3 = 0, cur4 = 0, codePre = -1;
    float dt = 0.f;
    int act = -1;
    bool oilDue = false;
    long call = -1;     // the ORIGINAL dispatcher call number (contact key)
};

// ---- contact-verdict injector ---------------------------------------------
// Keyed on (original dispatcher call, call-site return address), because a type
// can reach the same RVA from several sites in one call and the report's whole
// attribution rule is "by call site, not by RVA".
struct Inject {
    std::map<std::pair<long, unsigned>, std::vector<int> > q;   // 0x004b4cd0 sites
    std::map<std::pair<long, unsigned>, std::vector<int> > g;   // 0x0045c110 sites
    std::map<long, std::vector<int> > sq, sc;   // armed sweep, subject slot only
    std::map<unsigned, long> origCalls, portCalls, matched, unmatched;
    long curCall = -1;

    int TakeSweep(std::map<long, std::vector<int> >& m, unsigned ra) {
        portCalls[ra]++;
        auto it = m.find(curCall);
        if (it == m.end() || it->second.empty()) { unmatched[ra]++; return ra == 0x45bceau; }
        const int v = it->second.front();
        it->second.erase(it->second.begin());
        matched[ra]++;
        return v;
    }

    int Take(std::map<std::pair<long, unsigned>, std::vector<int> >& m, unsigned ra) {
        portCalls[ra]++;
        auto it = m.find(std::make_pair(curCall, ra));
        if (it == m.end() || it->second.empty()) { unmatched[ra]++; return 0; }
        const int v = it->second.front();
        it->second.erase(it->second.begin());
        matched[ra]++;
        return v;
    }
};
Inject g_inj;

int QueryInject(void*, const float*, mashed_re::Powerup::Contact::WorldHit* out,
                std::uint32_t ra) {
    const int n = g_inj.Take(g_inj.q, ra);
    // The replay has no triangles, so only the branch-deciding value is real.
    // t = 0.5 keeps the lerp finite; nothing downstream of it is measured here.
    if (n) { out->t = 0.5f; out->normal[1] = 1.f; }
    return n;
}
int GateInject(void*, const mashed_re::Powerup::Contact::WorldHit*, std::uint32_t ra) {
    return g_inj.Take(g_inj.g, ra);
}
int SweepInject(void*, int /*slot*/, int which) {   // 0 = query, 1 = confirm
    return which == 0 ? g_inj.TakeSweep(g_inj.sq, 0x0045bcd8u)
                      : g_inj.TakeSweep(g_inj.sc, 0x0045bceau);
}

// pu_contact_report.py's slot rule, verbatim: arg2 == 0x0088fbe0 + slot*0xb4 + 0x80.
int SweepSlotOf(unsigned a2) {
    const long off = static_cast<long>(a2) - 0x0088fbe0L - 0x80L;
    if (off < 0 || off % 0xb4 || off / 0xb4 > 15) return -1;
    return static_cast<int>(off / 0xb4);
}

// Call sites the injector owns. Every one was READ OFF the original: the RVA of
// the call + its instruction length gives the return address the capture records.
//   OIL    FUN_00457800: CALL 0x004b4cd0 @0x004578cc -> 0x004578d1
//                        CALL 0x0045c110 @0x00457909 -> 0x0045790e
//   P_MINE FUN_00457c10: CALL 0x004b4cd0 @0x00457ca0 -> 0x00457ca5
//                        CALL 0x0045c110 @0x00457cf4 -> 0x00457cf9
// A .pucontact.csv row carries no slot, and these sites are per-TYPE code that
// every slot runs, so a row is the SUBJECT slot's only when the subject slot held
// that type on that dispatcher call. (pu_contact_report.py hits the same problem
// and solves the dispatcher-sweep case with the arg2 = slot_base+0x80 rule; there
// is no such channel for the per-type sites.) MEASURED consequence of getting this
// wrong: verify/d3_contact_20260928/g2 has three 0x457ca5 rows, only two of which
// are slot 0's -- the third lands inside slot 0's MORTAR window and belongs to
// another slot's P_MINE.
const unsigned kQuerySites[] = { 0x004578d1u, 0x00457ca5u, 0x0045b4c2u };
const unsigned kGateSites[]  = { 0x0045790eu, 0x00457cf9u };
const int      kQueryOwner[] = { 19 /*OIL*/,  12 /*P_MINE*/, 17 /*SHOTGUN*/ };
const int      kGateOwner[]  = { 19,          12 };
const int      kQueryN = 3, kGateN = 2;

// Every contact call site the ported OIL/P_MINE FIRE path makes, in original
// order, with the type whose window it belongs to. The last two of each group
// are NOT injected -- they are placement leaves that always run once the gates
// pass, so their count is a pure consequence of the ported control flow.
// `gatedBy` = the site whose nonzero return is what lets this one run. If THAT
// site was not instrumented, this one cannot be tested either: with no injected
// verdict the port takes the refuse arm and never reaches here.
struct Site { unsigned ra; int owner; unsigned rva; unsigned gatedBy; const char* what; };
const Site kSites[] = {
    { 0x004578d1u, 19, 0x004b4cd0u, 0u, "OIL    query 0x004b4cd0" },   // CALL @0x004578cc
    { 0x0045790eu, 19, 0x0045c110u, 0x004578d1u, "OIL    gate  0x0045c110" },   // CALL @0x00457909
    { 0x00457932u, 19, 0x004b4650u, 0x0045790eu, "OIL    lerp  0x004b4650" },   // CALL @0x0045792d
    { 0x0045797fu, 19, 0x004b5080u, 0x0045790eu, "OIL    basis 0x004b5080" },   // CALL @0x0045797a
    { 0x00457ca5u, 12, 0x004b4cd0u, 0u, "P_MINE query 0x004b4cd0" },   // CALL @0x00457ca0
    { 0x00457cf9u, 12, 0x0045c110u, 0x00457ca5u, "P_MINE gate  0x0045c110" },   // CALL @0x00457cf4
    { 0x00457d1fu, 12, 0x004b4650u, 0x00457cf9u, "P_MINE lerp  0x004b4650" },   // CALL @0x00457d1a
    { 0x00457db2u, 12, 0x004b5080u, 0x00457cf9u, "P_MINE basis 0x004b5080" },   // CALL @0x00457dad
    { 0x0045b4c2u, 17, 0x004b4b20u, 0u, "SHOTGN query 0x004b4b20" },   // CALL @0x0045b4bd
    { 0x0045b582u, 17, 0x004b5080u, 0x0045b4c2u, "SHOTGN basis 0x004b5080" },   // CALL @0x0045b57d
    { 0x0045bcd8u, -1, 0x004b4b60u, 0u, "SWEEP  query 0x004b4b60" },   // CALL @0x0045bcd3, slot by arg2
    { 0x0045bceau, -1, 0x0045c350u, 0x0045bcd8u, "SWEEP  confirm 0x45c350" },   // CALL @0x0045bce5, slot by arg2
};
const int kSiteCount = static_cast<int>(sizeof(kSites) / sizeof(kSites[0]));

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
        r.call = call;
        rows.push_back(r);
    }
    std::fclose(f);
    // gap rows carry no original call number; give them the run of calls they
    // stand for so the contact key still lines up.
    {
        long next = rows.empty() ? 0 : rows.back().call;
        for (std::size_t i = rows.size(); i-- > 0; ) {
            if (rows[i].call >= 0) next = rows[i].call;
            else rows[i].call = --next;
        }
    }

    // ---- load <base>.pucontact.csv, if present ----------------------------
    std::string base(argv[1]);
    const std::string suffix = ".puhook.csv";
    if (base.size() > suffix.size() && base.compare(base.size() - suffix.size(),
                                                    suffix.size(), suffix) == 0)
        base.erase(base.size() - suffix.size());
    const std::string cxPath = base + ".pucontact.csv";
    bool haveCx = false;
    // Which RVAs the capture's Frida listeners were armed for. A site whose RVA
    // appears NOWHERE in the capture was not instrumented when it was taken (the
    // PU_CONTACT table grew on 2026-09-28), so its zero original rows are a
    // missing input, not a port divergence. The distinction is load-bearing: the
    // 2026-09-27 captures predate `surface_gate` and `g3` predates `query_4b4b20`.
    std::set<unsigned> armedRva;
    if (std::FILE* cf = std::fopen(cxPath.c_str(), "r")) {
        haveCx = true;
        std::vector<std::string> ch;
        while (std::fgets(line, sizeof line, cf)) {
            auto c = Split(line);
            if (ch.empty()) { ch = c; continue; }
            auto col = [&](const char* n) -> std::string {
                for (std::size_t i = 0; i < ch.size(); ++i) if (ch[i] == n) return i < c.size() ? c[i] : "";
                return "";
            };
            armedRva.insert(static_cast<unsigned>(
                std::strtoul(col("rva").c_str(), nullptr, 16)));
            const long cl = std::atol(col("call").c_str());
            // Only the calls this replay actually re-runs. The capture spans the
            // whole race (all 4 slots, plus the frames before the subject slot's
            // first state==6 row); rows outside that range have no port
            // counterpart by construction and are not a divergence.
            if (rows.empty() || cl < rows.front().call || cl > rows.back().call) continue;
            const std::size_t ri = static_cast<std::size_t>(cl - rows.front().call);
            const int held = (ri < rows.size()) ? rows[ri].codePre : -1;
            const unsigned ra = static_cast<unsigned>(std::strtoul(col("ret_addr").c_str(), nullptr, 16));
            const int ret = std::atoi(col("ret").c_str());
            if (ra == 0x0045bcd8u || ra == 0x0045bceau) {
                const unsigned a2 = static_cast<unsigned>(std::strtoul(col("a2").c_str(), nullptr, 16));
                if (SweepSlotOf(a2) != want) continue;
                g_inj.origCalls[ra]++;
                (ra == 0x0045bcd8u ? g_inj.sq : g_inj.sc)[cl].push_back(ret);
                continue;
            }
            for (int i = 0; i < kSiteCount; ++i)
                if (kSites[i].ra == ra && held == kSites[i].owner) g_inj.origCalls[ra]++;
            for (int i = 0; i < kQueryN; ++i) if (kQuerySites[i] == ra && held == kQueryOwner[i])
                g_inj.q[std::make_pair(cl, ra)].push_back(ret);
            for (int i = 0; i < kGateN; ++i) if (kGateSites[i] == ra && held == kGateOwner[i])
                g_inj.g[std::make_pair(cl, ra)].push_back(ret);
        }
        std::fclose(cf);
        mashed_re::Powerup::Contact::SetInjectors(QueryInject, GateInject, nullptr);
        mashed_re::Powerup::Contact::SetSweepInjector(SweepInject, nullptr);
    }

    if (!haveCx) {
        // Captures taken before --puhook-contacts existed (verify/d3_pu_20260926/*)
        // carry no query verdicts. The replay has no collision world either, so a
        // non-injected SegmentQuery would return 0 and every drop would refuse --
        // which would read as a regression when it is only a missing input. Run
        // those under the permissive verdict (query hits, surface allows), i.e.
        // exactly the model the port had before 2026-09-28, and say so.
        mashed_re::Powerup::Contact::SetInjectors(
            [](void*, const float*, mashed_re::Powerup::Contact::WorldHit* o, std::uint32_t) {
                o->t = 0.5f; o->normal[1] = 1.f; return 1; },
            [](void*, const mashed_re::Powerup::Contact::WorldHit*, std::uint32_t) { return 0; },
            nullptr);
    }

    Backend be;
    PowerupSystem sys;
    sys.Init(&be);
    for (const Row& r : rows) {
        g_inj.curCall = r.call;
        if (r.act >= 0) sys.Activate(want, r.act);
        sys.SetInput(want, r.cur3 != 0, r.cur4 != 0);
        be.due = r.oilDue;
        sys.Tick(r.dt, be.car, r.state);
    }
    mashed_re::Powerup::Contact::CloseDump();
    std::printf("replayed %zu frames on slot %d\n", rows.size(), want);
    if (!haveCx) {
        std::printf("contact: %s absent -- pre-2026-09-28 capture; the query leaves ran\n"
                    "         under the permissive verdict (hit / allow), so the DECISION\n"
                    "         half below is comparable and criterion (c) is NOT tested here.\n",
                    cxPath.c_str());
        return 0;
    }
    // Count what the PORT emitted, from its own MASHED_PU_CONTACTDUMP rows -- the
    // same file, in the same schema, that re/tools/pu_contact_report.py reads.
    std::map<unsigned, long> portRows;
    if (const char* pd = std::getenv("MASHED_PU_CONTACTDUMP")) {
        if (std::FILE* pf = std::fopen(pd, "r")) {
            std::vector<std::string> ph;
            while (std::fgets(line, sizeof line, pf)) {
                auto c = Split(line);
                if (ph.empty()) { ph = c; continue; }
                for (std::size_t i = 0; i < ph.size() && i < c.size(); ++i)
                    if (ph[i] == "ret_addr")
                        portRows[static_cast<unsigned>(std::strtoul(c[i].c_str(), nullptr, 16))]++;
            }
            std::fclose(pf);
        }
    }

    std::printf("\nCONTACT CALL SITES (original capture vs port), slot %d\n", want);
    std::printf("  %-24s %-12s %6s %6s %9s %9s  %s\n",
                "site", "ret_addr", "orig", "port", "port-only", "orig-only", "status");
    int bad = 0, notArmed = 0;
    for (int i = 0; i < kSiteCount; ++i) {
        const unsigned ra = kSites[i].ra;
        long leftover = 0;
        for (auto& kv : g_inj.q) if (kv.first.second == ra) leftover += (long)kv.second.size();
        for (auto& kv : g_inj.g) if (kv.first.second == ra) leftover += (long)kv.second.size();
        if (ra == 0x0045bcd8u) for (auto& kv : g_inj.sq) leftover += (long)kv.second.size();
        if (ra == 0x0045bceau) for (auto& kv : g_inj.sc) leftover += (long)kv.second.size();
        const long port = portRows.count(ra) ? portRows[ra] : g_inj.portCalls[ra];
        bool armed = armedRva.count(kSites[i].rva) != 0;
        for (unsigned g = kSites[i].gatedBy; armed && g; ) {
            int gi = -1;
            for (int j = 0; j < kSiteCount; ++j) if (kSites[j].ra == g) { gi = j; break; }
            if (gi < 0) break;
            if (!armedRva.count(kSites[gi].rva)) armed = false;
            g = kSites[gi].gatedBy;
        }
        const bool diff  = (g_inj.origCalls[ra] != port) || g_inj.unmatched[ra] || leftover;
        std::printf("  %-24s 0x%-10x %6ld %6ld %9ld %9ld  %s\n",
                    kSites[i].what, ra, g_inj.origCalls[ra], port,
                    g_inj.unmatched[ra], leftover,
                    !armed ? "not-armed" : (diff ? "DIVERGES" : "clean"));
        if (armed && diff) bad = 1;
        if (!armed && (port || g_inj.origCalls[ra])) notArmed = 1;
    }
    std::printf("CONTACT VERDICT: %s%s\n", bad ? "DIVERGES" : "CLEAN",
                notArmed ? "  (not-armed rows had no Frida listener when this capture"
                           " was taken and are excluded)" : "");
    return 0;
}
