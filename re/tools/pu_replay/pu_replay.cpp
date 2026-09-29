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
#include "Powerup/PowerupAim.h"
#include "Powerup/PowerupMortar.h"
#include "Powerup/PowerupMissile.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace mashed_re::Powerup;

namespace {

struct Row {
    int state = 0, slot = 0, cur3 = 0, cur4 = 0, codePre = -1;
    // DAT_0068d1f0[slot], the dispatcher's box-state gate (0x0045bc6b). It is an
    // INPUT here: its five producers (FUN_004111c0 / FUN_00422fd0 / FUN_0040be50 /
    // FUN_0040e590 / FUN_00424eb0) are not ported, so the value comes from the
    // capture's `boxstate` column, carried forward across the quiet-gap rows the
    // capture does not emit.
    int box = 1;
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
    // The hit's SEGMENT PARAMETER, popped in lockstep with the count above. A
    // capture taken before the `hit_t` column existed leaves this empty, and
    // TakeT falls back to the old fixed 0.5 -- which is fine for every site whose
    // `t` nothing downstream consumes, and NOT fine for MISSILE's ground bias,
    // which is a pure function of it. `tPresent` says which case a run is in.
    std::map<std::pair<long, unsigned>, std::vector<float> > qt;
    bool tPresent = false;
    float TakeT(unsigned ra) {
        auto it = qt.find(std::make_pair(curCall, ra));
        if (it == qt.end() || it->second.empty()) return 0.5f;
        const float v = it->second.front();
        it->second.erase(it->second.begin());
        return v;
    }
};
Inject g_inj;

int QueryInject(void*, const float*, mashed_re::Powerup::Contact::WorldHit* out,
                std::uint32_t ra) {
    const int n = g_inj.Take(g_inj.q, ra);
    // The replay has no triangles, so only the capture's own values are real.
    if (n) { out->t = g_inj.TakeT(ra); out->normal[1] = 1.f; }
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
const unsigned kQuerySites[] = { 0x004578d1u, 0x00457ca5u, 0x0045b4c2u, 0x0045444fu,
                                 0x0045afccu };
const unsigned kGateSites[]  = { 0x0045790eu, 0x00457cf9u };
// The two ACQUISITION query sites (FUN_00459620). They are not in kQuerySites
// because they are attributed by the --puhook-aim channel's own `slot` column
// (mode 3), not by the subject slot's held code: three types reach them.
const unsigned kAimQuerySites[] = { 0x00459c19u, 0x00459d54u };
const int      kAimQueryN = 2;
// MORTAR's OWN chain (FUN_00453730). Mode 1: a mortar in flight outlives the slot
// that fired it, so code_pre cannot attribute it -- the same reason DRUM is mode 1.
const unsigned kMtrQuerySites[] = { 0x00453789u };
const unsigned kMtrGateSites[]  = { 0x004537bbu };
const int      kMtrQueryN = 1, kMtrGateN = 1;
// MISSILE's OWN chain (inside the tick FUN_00455c90). Mode 4: attributed by the
// --puhook-missile channel, which records every live projectile per frame.
const unsigned kMisQuerySites[] = { 0x00455de0u, 0x00455e59u };
const unsigned kMisGateSites[]  = { 0x00455df9u };
const int      kMisQueryN = 2, kMisGateN = 1;
const int      kQueryOwner[] = { 19 /*OIL*/,  12 /*P_MINE*/, 17 /*SHOTGUN*/, 10 /*DRUM*/,
                                 16 /*R_FLAME*/ };
const int      kGateOwner[]  = { 19,          12 };
const int      kQueryN = 5, kGateN = 2;

// Every contact call site the ported OIL/P_MINE FIRE path makes, in original
// order, with the type whose window it belongs to. The last two of each group
// are NOT injected -- they are placement leaves that always run once the gates
// pass, so their count is a pure consequence of the ported control flow.
// `gatedBy` = the site whose nonzero return is what lets this one run. If THAT
// site was not instrumented, this one cannot be tested either: with no injected
// verdict the port takes the refuse arm and never reaches here.
// `mode` says how a row is attributed to the subject slot:
//   0  by the subject slot's code_pre on that call -- the site is inside code only
//      the holding slot runs.
//   1  by call range ONLY. Used for PROJECTILE sites: MEASURED on
//      verify/d3_contact_20260928/g2, slot 0's DRUM window is calls 1181..1190 but
//      its 0x45444f queries run 1182..1242 -- the dropped drum outlives the slot by
//      52 calls, and two are in flight at once from 1189. code_pre cannot see it.
//      Sound only when NO OTHER SLOT held that type in the capture, which the loader
//      CHECKS (status `contested` otherwise, excluded from the verdict).
//   2  by arg2 = slot_base + 0x80 (the dispatcher sweep).
//   4  by the --puhook-missile channel: a row exists for every live projectile on
//      that call, so "the port stepped one" and "the capture had one" line up
//      without needing code_pre, which a missile in flight also outlives.
//   3  by the --puhook-aim channel's own `slot` column. Used for the ACQUISITION
//      sites inside FUN_00459620, which MORTAR, GUN and MISSILE all reach, so
//      neither code_pre nor call range can attribute them -- but the aim channel
//      records the firing car index on every one of its calls.
struct Site { unsigned ra; int owner; unsigned rva; unsigned gatedBy; int mode; const char* what; };
const Site kSites[] = {
    { 0x004578d1u, 19, 0x004b4cd0u, 0u, 0, "OIL    query 0x004b4cd0" },   // CALL @0x004578cc
    { 0x0045790eu, 19, 0x0045c110u, 0x004578d1u, 0, "OIL    gate  0x0045c110" },   // CALL @0x00457909
    { 0x00457932u, 19, 0x004b4650u, 0x0045790eu, 0, "OIL    lerp  0x004b4650" },   // CALL @0x0045792d
    { 0x0045797fu, 19, 0x004b5080u, 0x0045790eu, 0, "OIL    basis 0x004b5080" },   // CALL @0x0045797a
    { 0x00457ca5u, 12, 0x004b4cd0u, 0u, 0, "P_MINE query 0x004b4cd0" },   // CALL @0x00457ca0
    { 0x00457cf9u, 12, 0x0045c110u, 0x00457ca5u, 0, "P_MINE gate  0x0045c110" },   // CALL @0x00457cf4
    { 0x00457d1fu, 12, 0x004b4650u, 0x00457cf9u, 0, "P_MINE lerp  0x004b4650" },   // CALL @0x00457d1a
    { 0x00457db2u, 12, 0x004b5080u, 0x00457cf9u, 0, "P_MINE basis 0x004b5080" },   // CALL @0x00457dad
    { 0x0045b4c2u, 17, 0x004b4b20u, 0u, 0, "SHOTGN query 0x004b4b20" },   // CALL @0x0045b4bd
    { 0x0045b582u, 17, 0x004b5080u, 0x0045b4c2u, 0, "SHOTGN basis 0x004b5080" },   // CALL @0x0045b57d
    { 0x0045444fu, 10, 0x004b4cd0u, 0u,          1, "DRUM   query 0x004b4cd0" },   // CALL @0x0045444a
    { 0x0045448au, 10, 0x004b4650u, 0x0045444fu, 1, "DRUM   lerp  0x004b4650" },   // CALL @0x00454485
    { 0x004544e0u, 10, 0x004b5080u, 0x0045444fu, 1, "DRUM   basis 0x004b5080" },   // CALL @0x004544db
    { 0x0045afccu, 16, 0x004b4cd0u, 0u,          1, "RFLAME query 0x004b4cd0" },   // CALL @0x0045afc7
    { 0x0045aff3u, 16, 0x004b4650u, 0x0045afccu, 1, "RFLAME lerp  0x004b4650" },   // CALL @0x0045afee
    { 0x0045b04cu, 16, 0x004b5080u, 0x0045afccu, 1, "RFLAME basis 0x004b5080" },   // CALL @0x0045b047
    { 0x0045bcd8u, -1, 0x004b4b60u, 0u, 2, "SWEEP  query 0x004b4b60" },   // CALL @0x0045bcd3, slot by arg2
    { 0x0045bceau, -1, 0x0045c350u, 0x0045bcd8u, 2, "SWEEP  confirm 0x45c350" },   // CALL @0x0045bce5, slot by arg2
    // FUN_00459620, shared by MORTAR/GUN/MISSILE. The FALLBACK pair runs only
    // when the candidate count is 0; the LOS pair runs on every call.
    { 0x00459c19u, -1, 0x004b4cd0u, 0u,          3, "AIM    fallback query" },   // CALL @0x00459c14
    { 0x00459c3cu, -1, 0x004b4650u, 0x00459c19u, 3, "AIM    fallback lerp " },   // CALL @0x00459c37
    { 0x00459d54u, -1, 0x004b4cd0u, 0u,          3, "AIM    los      query" },   // CALL @0x00459d4f
    { 0x00459db5u, -1, 0x004b4650u, 0x00459d54u, 3, "AIM    los      lerp " },   // CALL @0x00459db0
    // MORTAR's own detonation test, FUN_00453730, one caller (0x004538fe).
    { 0x00453789u,  7, 0x004b4cd0u, 0u,          1, "MORTAR query 0x004b4cd0" },   // CALL @0x00453784
    { 0x004537bbu,  7, 0x0045c350u, 0x00453789u, 1, "MORTAR gate  0x0045c350" },   // CALL @0x004537b6
    { 0x004537dfu,  7, 0x004b4650u, 0x004537bbu, 1, "MORTAR lerp  0x004b4650" },   // CALL @0x004537da
    { 0x0045382cu,  7, 0x004b5080u, 0x004537bbu, 1, "MORTAR basis 0x004b5080" },   // CALL @0x00453827
    // MISSILE's own chain, inside FUN_00455c90. The sphere query runs on EVEN
    // frames only (DAT_007f101c parity, 0x00455d83..0x00455d99), the ground probe
    // every frame -- which is why the two counts differ by ~2x in every capture.
    { 0x00455de0u, 11, 0x004b4d10u, 0u,          4, "MISSIL sphere 0x004b4d10" },  // CALL @0x00455ddb
    { 0x00455df9u, 11, 0x0045c350u, 0x00455de0u, 4, "MISSIL gate   0x0045c350" },  // CALL @0x00455df4
    { 0x00455e59u, 11, 0x004b4cd0u, 0u,          4, "MISSIL ground 0x004b4cd0" },  // CALL @0x00455e54
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
    int lastBox = 1;
    // code -> the set of slots that ever held it. A mode-1 (projectile) site can
    // only be attributed by call range when this set is exactly {want}.
    std::map<int, std::set<int> > heldBy;
    while (std::fgets(line, sizeof line, f)) {
        auto c = Split(line);
        if (hdr.empty()) { hdr = c; continue; }
        auto col = [&](const char* n) -> std::string {
            for (std::size_t i = 0; i < hdr.size(); ++i) if (hdr[i] == n) return i < c.size() ? c[i] : "";
            return "";
        };
        {   // every (slot, code) pair the capture shows, for the mode-1 contested check
            const int sl = std::atoi(col("slot").c_str());
            const int cp = std::atoi(col("code_pre").c_str());
            if (cp != -1) heldBy[cp].insert(sl);
        }
        if (std::atoi(col("slot").c_str()) != want) continue;
        Row r;
        r.state = std::atoi(col("state").c_str());
        if (r.state != 6) continue;
        const long call = std::atol(col("call").c_str());
        if (lastCall >= 0) for (long g = lastCall + 1; g < call; ++g) { Row q; q.state = 6; q.box = lastBox; q.dt = rows.empty() ? 1.f / 60.f : rows.back().dt; rows.push_back(q); }
        lastCall = call;
        r.slot = want;
        {   // captures taken before the column existed leave it empty -> keep 1
            const std::string b = col("boxstate");
            if (!b.empty()) r.box = std::atoi(b.c_str());
            lastBox = r.box;
        }
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

    // <base> = argv[1] with its ".puhook.csv" suffix removed; the sibling
    // channels hang off it.
    std::string base(argv[1]);
    {
        const std::string suffix = ".puhook.csv";
        if (base.size() > suffix.size() && base.compare(base.size() - suffix.size(),
                                                        suffix.size(), suffix) == 0)
            base.erase(base.size() - suffix.size());
    }

    // ---- load <base>.puaim.csv, if present --------------------------------
    // One row per FUN_00459620 call: the call's own arguments plus the live
    // subsystem reads the acquisition loop makes (the four car positions and
    // active flags, the firing car's aim-matrix `at` row, the second list's
    // count). Without it the acquisition sites cannot be replayed at all -- the
    // replay's Backend::car[4] is default-constructed all-zero.
    //
    // WHAT THIS DOES AND DOES NOT TEST, stated plainly. The SCHEDULE comes from
    // the capture: Acquire runs on exactly the calls the original ran it on, so
    // the LOS site's count (one per aim row) is true by construction and is NOT
    // evidence. The FALLBACK site's count is NOT: it fires only when the ported
    // candidate loop finds nothing, so its 187-of-348 pattern is decided entirely
    // by the port. That row is the criterion-(c) measurement here.
    std::map<long, mashed_re::Powerup::Aim::Inputs> aimIn;
    std::map<long, int> aimSlot;
    bool haveAim = false;
    {
        std::string aimPath = base + ".puaim.csv";
        if (std::FILE* af = std::fopen(aimPath.c_str(), "r")) {
            haveAim = true;
            std::vector<std::string> ah;
            while (std::fgets(line, sizeof line, af)) {
                auto c = Split(line);
                if (ah.empty()) { ah = c; continue; }
                auto col = [&](const char* n) -> std::string {
                    for (std::size_t i = 0; i < ah.size(); ++i) if (ah[i] == n) return i < c.size() ? c[i] : "";
                    return "";
                };
                // every float is recorded as its exact bits; a decimal is not the
                // value the game computed with.
                auto fx = [&](const char* n) -> float {
                    const std::string s = col(n);
                    if (s.size() > 2 && s[0] == '0' && s[1] == 'x') {
                        const unsigned long u = std::strtoul(s.c_str() + 2, nullptr, 16);
                        float f; std::memcpy(&f, &u, 4); return f;
                    }
                    return static_cast<float>(std::atof(s.c_str()));
                };
                const long cl = std::atol(col("call").c_str());
                mashed_re::Powerup::Aim::Inputs in;
                in.origin[0] = fx("ox"); in.origin[1] = fx("oy"); in.origin[2] = fx("oz");
                in.range = fx("range"); in.cone = fx("cone");
                in.at[0] = fx("at_x"); in.at[1] = fx("at_y"); in.at[2] = fx("at_z");
                in.listCount = std::atoi(col("list_n").c_str());
                for (int i = 0; i < 4; ++i) {
                    char k[16];
                    std::snprintf(k, sizeof k, "c%d_act", i);
                    in.carActive[i] = std::atoi(col(k).c_str()) > 0 ? 1 : 0;
                    for (int a = 0; a < 3; ++a) {
                        std::snprintf(k, sizeof k, "c%d_%c", i, "xyz"[a]);
                        in.carPos[i][a] = fx(k);
                    }
                }
                aimIn[cl] = in;
                aimSlot[cl] = std::atoi(col("slot").c_str());
            }
            std::fclose(af);
        }
    }

    // ---- load <base>.pumissile.csv, if present ----------------------------
    // One row per LIVE MISSILE projectile per frame, PRE and POST, plus the frame
    // counter DAT_007f101c whose parity gates the sphere query.
    //
    // The port is seeded from PRE each frame for POSITION and DELTA -- the flight
    // integration is NOT ported (it is a closed loop through the projectile's RW
    // frame matrix) -- but it CARRIES its own `age`, `live` and `bias`. So the
    // lifetime gate, the parity gate and the ground-bias value are the port's,
    // and the two query counts follow from them rather than from the schedule.
    struct MisRow {
        long call; int rec; int framectr;
        mashed_re::Powerup::Missile::Record pre;
        float postBias; int postLive;
    };
    std::vector<MisRow> misRows;
    bool haveMis = false;
    {
        std::string misPath = base + ".pumissile.csv";
        if (std::FILE* mf = std::fopen(misPath.c_str(), "r")) {
            haveMis = true;
            std::vector<std::string> mh;
            while (std::fgets(line, sizeof line, mf)) {
                auto c = Split(line);
                if (mh.empty()) { mh = c; continue; }
                auto col = [&](const char* n) -> std::string {
                    for (std::size_t i = 0; i < mh.size(); ++i) if (mh[i] == n) return i < c.size() ? c[i] : "";
                    return "";
                };
                auto fx = [&](const char* n) -> float {
                    const std::string s2 = col(n);
                    if (s2.size() > 2 && s2[0] == '0' && s2[1] == 'x') {
                        const unsigned long u = std::strtoul(s2.c_str() + 2, nullptr, 16);
                        float f; std::memcpy(&f, &u, 4); return f;
                    }
                    return static_cast<float>(std::atof(s2.c_str()));
                };
                MisRow m; std::memset(&m, 0, sizeof m);
                m.call = std::atol(col("call").c_str());
                m.rec  = std::atoi(col("rec").c_str());
                m.framectr = std::atoi(col("framectr").c_str());
                m.pre.live = std::atoi(col("pre_live").c_str()) ? 1 : 0;
                const char* ax = "xyz";
                for (int i = 0; i < 3; ++i) {
                    char k[16];
                    std::snprintf(k, sizeof k, "pre_p%c", ax[i]); m.pre.pos[i]   = fx(k);
                    std::snprintf(k, sizeof k, "pre_d%c", ax[i]); m.pre.delta[i] = fx(k);
                }
                m.pre.bias  = fx("pre_bias");
                m.pre.age   = fx("pre_age");
                m.pre.speed = fx("pre_speed");
                m.pre.tgt0  = std::atoi(col("pre_tgt0").c_str());
                m.pre.tgt1  = std::atoi(col("pre_tgt1").c_str());
                m.postBias  = fx("post_bias");
                m.postLive  = std::atoi(col("post_live").c_str()) ? 1 : 0;
                misRows.push_back(m);
            }
            std::fclose(mf);
        }
    }

    // ---- load <base>.pumortar.csv, if present -----------------------------
    // One row per MORTAR projectile per frame, with the pool record's PRE and
    // POST state. The port is SEEDED from PRE on the first row of each
    // projectile's life and then CARRIES ITS OWN STATE forward, so what is
    // measured is the accumulated integration, not 327 independent one-step
    // checks. The capture's POST columns are the reference; the max drift is
    // reported alongside the contact counts.
    //
    // A new life is detected by the age going DOWN (or by the record index being
    // seen for the first time): pool slots are reused, and the only field that
    // resets is age.
    struct MtrRow {
        long call; int rec;
        mashed_re::Powerup::Mortar::Record pre;
        mashed_re::Powerup::Mortar::Target tgt;
        float post[3];              // the capture's post position, for the drift
        float postAge;
    };
    std::vector<MtrRow> mtrRows;
    bool haveMtr = false;
    {
        std::string mtrPath = base + ".pumortar.csv";
        if (std::FILE* mf = std::fopen(mtrPath.c_str(), "r")) {
            haveMtr = true;
            std::vector<std::string> mh;
            while (std::fgets(line, sizeof line, mf)) {
                auto c = Split(line);
                if (mh.empty()) { mh = c; continue; }
                auto col = [&](const char* n) -> std::string {
                    for (std::size_t i = 0; i < mh.size(); ++i) if (mh[i] == n) return i < c.size() ? c[i] : "";
                    return "";
                };
                auto fx = [&](const char* n) -> float {
                    const std::string s2 = col(n);
                    if (s2.size() > 2 && s2[0] == '0' && s2[1] == 'x') {
                        const unsigned long u = std::strtoul(s2.c_str() + 2, nullptr, 16);
                        float f; std::memcpy(&f, &u, 4); return f;
                    }
                    return static_cast<float>(std::atof(s2.c_str()));
                };
                MtrRow m; std::memset(&m, 0, sizeof m);
                m.call = std::atol(col("call").c_str());
                m.rec  = std::atoi(col("rec").c_str());
                m.pre.owner = std::atoi(col("owner").c_str());
                m.pre.aim[0] = fx("aim_x"); m.pre.aim[1] = fx("aim_y"); m.pre.aim[2] = fx("aim_z");
                m.pre.baseY  = fx("baseY");
                const char* ax = "xyz";
                for (int i = 0; i < 3; ++i) {
                    char k[16];
                    std::snprintf(k, sizeof k, "pre_p%c", ax[i]); m.pre.pos[i]   = fx(k);
                    std::snprintf(k, sizeof k, "pre_v%c", ax[i]); m.pre.vel[i]   = fx(k);
                    std::snprintf(k, sizeof k, "pre_d%c", ax[i]); m.pre.delta[i] = fx(k);
                    std::snprintf(k, sizeof k, "post_p%c", ax[i]); m.post[i]     = fx(k);
                }
                m.pre.homing = std::atoi(col("pre_homing").c_str());
                m.pre.age    = fx("pre_age");
                m.pre.ageN   = fx("pre_agen");
                m.postAge    = fx("post_age");
                const std::string tx = col("tgt_x");
                m.tgt.valid = (tx.size() > 2 && tx[0] == '0' && tx[1] == 'x') ? 1 : 0;
                if (m.tgt.valid) {
                    m.tgt.pos[0] = fx("tgt_x"); m.tgt.pos[1] = fx("tgt_y"); m.tgt.pos[2] = fx("tgt_z");
                }
                mtrRows.push_back(m);
            }
            std::fclose(mf);
        }
    }

    // ---- load <base>.pucontact.csv, if present ----------------------------
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
            // `hit_t` exists only on captures from 2026-09-28c on.
            float ht = 0.5f;
            {
                const std::string h = col("hit_t");
                if (h.size() > 2 && h[0] == '0' && h[1] == 'x') {
                    g_inj.tPresent = true;
                    const unsigned long u = std::strtoul(h.c_str() + 2, nullptr, 16);
                    std::memcpy(&ht, &u, 4);
                }
            }
            if (ra == 0x0045bcd8u || ra == 0x0045bceau) {
                const unsigned a2 = static_cast<unsigned>(std::strtoul(col("a2").c_str(), nullptr, 16));
                if (SweepSlotOf(a2) != want) continue;
                g_inj.origCalls[ra]++;
                (ra == 0x0045bcd8u ? g_inj.sq : g_inj.sc)[cl].push_back(ret);
                continue;
            }
            int mode = -1, owner = -1;
            for (int i = 0; i < kSiteCount; ++i)
                if (kSites[i].ra == ra) { mode = kSites[i].mode; owner = kSites[i].owner; }
            if (mode < 0) continue;
            const bool mine = (mode == 1) ? true
                            : (mode == 4) ? true
                            : (mode == 3) ? (aimSlot.count(cl) && aimSlot[cl] == want)
                                          : (held == owner);
            if (!mine) continue;
            g_inj.origCalls[ra]++;
            for (int i = 0; i < kQueryN; ++i) if (kQuerySites[i] == ra) {
                g_inj.q[std::make_pair(cl, ra)].push_back(ret);
                g_inj.qt[std::make_pair(cl, ra)].push_back(ht);
            }
            for (int i = 0; i < kAimQueryN; ++i) if (kAimQuerySites[i] == ra) {
                g_inj.q[std::make_pair(cl, ra)].push_back(ret);
                g_inj.qt[std::make_pair(cl, ra)].push_back(ht);
            }
            for (int i = 0; i < kMtrQueryN; ++i) if (kMtrQuerySites[i] == ra) {
                g_inj.q[std::make_pair(cl, ra)].push_back(ret);
                g_inj.qt[std::make_pair(cl, ra)].push_back(ht);
            }
            for (int i = 0; i < kMtrGateN; ++i) if (kMtrGateSites[i] == ra)
                g_inj.g[std::make_pair(cl, ra)].push_back(ret);
            for (int i = 0; i < kMisQueryN; ++i) if (kMisQuerySites[i] == ra) {
                g_inj.q[std::make_pair(cl, ra)].push_back(ret);
                g_inj.qt[std::make_pair(cl, ra)].push_back(ht);
            }
            for (int i = 0; i < kMisGateN; ++i) if (kMisGateSites[i] == ra)
                g_inj.g[std::make_pair(cl, ra)].push_back(ret);
            for (int i = 0; i < kGateN; ++i) if (kGateSites[i] == ra)
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
    std::size_t misNext = 0, misSteps = 0, misLives = 0, misBiasBad = 0, misLiveBad = 0;
    std::map<int, mashed_re::Powerup::Missile::Record> misLive;
    std::map<int, float> misLastAge;
    std::set<int> misSeen;
    std::size_t mtrNext = 0, mtrSteps = 0, mtrLives = 0;
    std::map<int, mashed_re::Powerup::Mortar::Record> mtrLive;
    std::map<int, float> mtrLast;
    std::set<int> mtrSeen;
    float mtrDrift = 0.f, mtrAgeDrift = 0.f;
    for (const Row& r : rows) {
        g_inj.curCall = r.call;
        sys.SetBoxState(want, r.box);       // FUN_0045ba00, the un-ported producers
        if (r.act >= 0) sys.Activate(want, r.act);
        sys.SetInput(want, r.cur3 != 0, r.cur4 != 0);
        be.due = r.oilDue;
        sys.Tick(r.dt, be.car, r.state);
        // FUN_00459620 runs once per frame while MORTAR, GUN or MISSILE has a
        // projectile up. The ported ticks do not yet call it (their projectile
        // pools are not landed), so the SCHEDULE is taken from the capture and
        // only the BRANCH is the port's -- see the loader comment above.
        auto ai = aimIn.find(r.call);
        if (ai != aimIn.end() && aimSlot[r.call] == want) {
            mashed_re::Powerup::Aim::Record arec;
            std::memset(&arec, 0, sizeof arec);
            mashed_re::Powerup::Aim::Acquire(want, ai->second, &arec);
        }
        // MISSILE's pool loop. The tick walks EBP DOWNWARD, so record index 4 is
        // stepped first; the capture emits rows in index order, so replay them
        // descending to reproduce the original's per-call contact ORDER.
        {
            std::size_t lo = misNext;
            while (misNext < misRows.size() && misRows[misNext].call == r.call) ++misNext;
            for (std::size_t k = misNext; k-- > lo; ) {
                MisRow& m = misRows[k];
                mashed_re::Powerup::Missile::Record& live = misLive[m.rec];
                // a new life: the slot is fresh, or its age went DOWN
                if (!misSeen.count(m.rec) || m.pre.age < misLastAge[m.rec]) {
                    live = m.pre;
                    misSeen.insert(m.rec);
                    ++misLives;
                }
                misLastAge[m.rec] = m.pre.age;
                // position and delta are INPUTS (the flight step is not ported);
                // age, live and bias are the port's and are carried forward.
                for (int c = 0; c < 3; ++c) {
                    live.pos[c]   = m.pre.pos[c];
                    live.delta[c] = m.pre.delta[c];
                }
                live.live = 1;
                mashed_re::Powerup::Missile::Step(live, r.dt, m.framectr);
                if (std::fabs(live.bias - m.postBias) > 1e-6f) ++misBiasBad;
                if (live.live != m.postLive) ++misLiveBad;
                ++misSteps;
            }
        }
        // MORTAR's pool loop, for the projectiles this call had in flight. Pool
        // order within a call is the capture's row order (the tick walks
        // 0x00684ea8 upward), so replaying the rows in file order reproduces the
        // original's call ORDER as well as its count.
        while (mtrNext < mtrRows.size() && mtrRows[mtrNext].call == r.call) {
            MtrRow& m = mtrRows[mtrNext++];
            mashed_re::Powerup::Mortar::Record& live = mtrLive[m.rec];
            // A new life: the record index is fresh, or the age went DOWN.
            if (!mtrSeen.count(m.rec) || m.pre.age < mtrLast[m.rec]) {
                live = m.pre;                       // seed once, from the capture
                mtrSeen.insert(m.rec);
                ++mtrLives;
            }
            mtrLast[m.rec] = m.pre.age;
            mashed_re::Powerup::Mortar::Step(live, m.tgt, r.dt);
            // drift of the port's CARRIED state against the capture's POST
            for (int c = 0; c < 3; ++c) {
                const float e = std::fabs(live.pos[c] - m.post[c]);
                if (e > mtrDrift) mtrDrift = e;
            }
            const float ae = std::fabs(live.age - m.postAge);
            if (ae > mtrAgeDrift) mtrAgeDrift = ae;
            ++mtrSteps;
        }
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

    bool misBad = false;
    if (haveMis) {
        // A bias mismatch only MEANS anything when the capture carries hit_t;
        // without it the port computes from the fallback 0.5 and every hit row
        // would 'diverge' on a missing input rather than on a defect.
        misBad = (misLiveBad != 0) || (g_inj.tPresent && misBiasBad != 0);
        std::printf("\nMISSILE PROJECTILE CONTACT HALF (port vs capture POST)\n"
                    "  updates replayed %zu of %zu   projectile lives seeded %zu\n"
                    "  ground-bias mismatches %zu   live-flag mismatches %zu   %s\n",
                    misSteps, misRows.size(), misLives, misBiasBad, misLiveBad,
                    misBad ? "DIVERGES" : "clean",
                    g_inj.tPresent ? "present"
                                   : "ABSENT -- pre-2026-09-28c capture, the bias "
                                     "below is NOT tested");
        if (misSteps != misRows.size())
            std::printf("  NOTE: %zu capture rows fell outside the replayed call range\n",
                        misRows.size() - misSteps);
    }

    // MORTAR's contact COUNTS are schedule-derived -- the port steps once per
    // captured row, and the first thing a step does is the query -- so they pass
    // even with the integrator broken. MEASURED: all three MASHED_MORTAR_FORCE
    // controls leave every MORTAR row `clean` while the drift moves from 4.8e-07
    // to 6.03 / 6.29 / 0.226. The drift is therefore part of the VERDICT, not a
    // footnote, or this table would report a broken integrator as CLEAN.
    const float kMtrDriftMax = 1e-4f;       // ~1e3 ULPs at these magnitudes; the
                                            // real run sits 200x under it
    bool mtrBad = false;
    if (haveMtr) {
        mtrBad = (mtrDrift > kMtrDriftMax) || (mtrAgeDrift > kMtrDriftMax);
        std::printf("\nMORTAR PROJECTILE INTEGRATION (port carried vs capture POST)\n"
                    "  updates replayed %zu of %zu   projectile lives seeded %zu\n"
                    "  max position drift %.3g   max age drift %.3g   (limit %g)  %s\n",
                    mtrSteps, mtrRows.size(), mtrLives, mtrDrift, mtrAgeDrift,
                    kMtrDriftMax, mtrBad ? "DIVERGES" : "clean");
        if (mtrSteps != mtrRows.size())
            std::printf("  NOTE: %zu capture rows fell outside the replayed call range\n",
                        mtrRows.size() - mtrSteps);
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
        const bool contested = kSites[i].mode == 1 &&
            (heldBy[kSites[i].owner].size() > 1 ||
             (heldBy[kSites[i].owner].size() == 1 && !heldBy[kSites[i].owner].count(want)));
        // A mode-3 row with no --puhook-aim channel has ZERO original rows and
        // zero port rows, which would print `clean` and mean nothing. That is the
        // same false green the `not-armed` handling exists to prevent, so it gets
        // the same treatment: a capture taken before 2026-09-28b simply cannot
        // test the acquisition sites.
        bool armed = armedRva.count(kSites[i].rva) != 0;
        if (kSites[i].mode == 3 && !haveAim) armed = false;
        // Same rule for MORTAR's own chain: without the projectile channel the
        // port never steps a projectile, so a 0-vs-0 row would be a missing
        // input printed as a match.
        if (kSites[i].owner == 7 && kSites[i].mode == 1 && !haveMtr) armed = false;
        if (kSites[i].mode == 4 && !haveMis) armed = false;
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
                    !armed ? "not-armed" : contested ? "contested"
                                                      : (diff ? "DIVERGES" : "clean"));
        if (armed && !contested && diff) bad = 1;
        if (!armed && (port || g_inj.origCalls[ra])) notArmed = 1;
    }
    std::printf("CONTACT VERDICT: %s%s%s\n",
                (bad || mtrBad || misBad) ? "DIVERGES" : "CLEAN",
                notArmed ? "  (not-armed rows had no Frida listener when this capture"
                           " was taken and are excluded)" : "",
                haveAim ? "" : "  (no --puhook-aim channel: the AIM rows are NOT"
                               " tested by this capture)");
    if (!haveMtr)
        std::printf("         (no --puhook-mortar channel: the MORTAR rows are NOT"
                    " tested by this capture)\n");
    if (!haveMis)
        std::printf("         (no --puhook-missile channel: the MISSILE rows are NOT"
                    " tested by this capture)\n");
    return 0;
}
