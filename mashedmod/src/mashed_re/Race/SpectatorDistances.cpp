// Mashed RE — FUN_00442a60 / FUN_0040e180, STANDALONE bodies.
// [U-9186 leg H3, 2026-10-08 — DEFERRED leg B, unblocked by USER DECISION (Mariano)]
//
// EXE-ONLY TU. Neither RVA has an .asi body, so there is no dual copy and no
// RH_ScopedInstall here: the standalone calls these directly.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
// PREREG: verify/d3_u9186_20261008/PREREG_H3.md
//
// WHAT THIS PRODUCES. 0x008989b0[v] = the planar XZ distance from a reference
// car, scaled. Readers: the AI fire gates FUN_004148b0 / FUN_00414c30 /
// FUN_00415020 / FUN_00415200 / FUN_00415220 and LeaderInRange, via FUN_00442cc0
// (Ai/AiStandalone.cpp:711 RefDist). Until this leg they all read 0.
//
// It is a BRIDGE, not a C4 port: it inherits the 2026-10-06 USER DECISION
// (knob-gated, non-bit-identical substrate acceptable, NO C-level follows).
// Default-OFF behind MASHED_REFDIST.
//
// ---------------------------------------------------------------------------
// TUNING CONSTANTS — read from original/MASHED.exe.unpatched, NOT guessed.
// Gate H3-CONSTS, verify/d3_u9186_20261008/RESULT_H3.md:
//   0x005cc730  .rdata fileoff 0x1cc730  bits 0x42a00000  =  80.0f
//   0x005ccd6c  .rdata fileoff 0x1ccd6c  bits 0x41a00000  =  20.0f
//   0x005cc568  .rdata fileoff 0x1cc568  bits 0x42c80000  = 100.0f
//   0x005cc9bc  .rdata fileoff 0x1cc9bc  bits 0x3f4ccccd  =   0.8f
// They decode the wrap branch: race_pct is 0..100 per lap, so "one car above 80
// and the other below 20" is the pair straddling the start/finish line, and
// subtracting 100.0 un-wraps the one that already crossed.
//
// DEVIATION, REGISTERED. The original's vector length is FUN_004c3ac0, the
// RenderWare fast-sqrt lookup (table at DAT_007d3ff8), which the standalone does
// not own. std::sqrt is used instead — the same stand-in Ai/AiStandalone.cpp:770
// already uses for this exact RVA. So the magnitudes here are NOT bit-identical
// to the original's and no C-level follows from them. Pair SELECTION is
// unaffected in principle (both functions are monotone in the squared length, so
// the argmax is the same) but that is an argument, not a measurement, and gate
// H3-PAIR reports the selected indices so it can be checked.


#include "RaceCamera.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

// Readers made standalone-reachable by legs H1a / H1b.
extern "C" std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx);          // 0x0046c7b0
extern "C" int          __cdecl VehicleCarStateRead(std::uint32_t carIdx,              // 0x0046cbb0
                                                    std::uint32_t* outState,
                                                    std::uint32_t* outSecondary);
extern "C" std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out,                  // 0x0046d4a0
                                                  std::uint32_t idx);
extern "C" std::uint32_t __cdecl IsCarSlotActive(int param_1);                         // 0x0040e370

namespace mashed_re {
namespace Race {

namespace {

constexpr float kWrapHi    =  80.0f;   // _DAT_005cc730
constexpr float kWrapLo    =  20.0f;   // _DAT_005ccd6c
constexpr float kLapSpan   = 100.0f;   // _DAT_005cc568
constexpr float kDistScale =   0.8f;   // _DAT_005cc9bc

// FUN_00408ad0 (0x00408ad0, 17-byte leaf): race_pct of car i.
// `return (float10)*(float *)(&DAT_008a96ec + param_1 * 0x30c);`
// Supplied standalone by the D-11072 race_pct bridge (MASHED_RACEPCT_BRIDGE).
inline float RacePct(int i) {
    return *reinterpret_cast<const float*>(
        0x008a96ecu + static_cast<std::uintptr_t>(i) * 0x30cu);
}

// The three per-car predicates FUN_0040e180 and FUN_00442a60 both gate on.
inline bool SlotUsable(int i) {
    if (!IsCarSlotActive(i)) return false;                      // FUN_0040e370
    if (VehicleSlotGetter(static_cast<std::uint32_t>(i)) != 1u)  // FUN_0046c7b0
        return false;
    std::uint32_t st = 0u, sec = 0u;
    VehicleCarStateRead(static_cast<std::uint32_t>(i), &st, &sec);  // FUN_0046cbb0
    return st == 0u;
}

// World XZ of car i, from the body-matrix translation row that FUN_0046d4a0
// resolves (+0x30/+0x38 off the returned pointer).
inline bool CarXZ(int i, float* x, float* z) {
    std::uint32_t p = 0u;
    if (!PtrCompute881ec8(&p, static_cast<std::uint32_t>(i)) || !p) return false;
    *x = *reinterpret_cast<const float*>(static_cast<std::uintptr_t>(p) + 0x30u);
    *z = *reinterpret_cast<const float*>(static_cast<std::uintptr_t>(p) + 0x38u);
    return true;
}

}  // namespace

// ---------------------------------------------------------------------------
// NOTE (not a body anchor — this comment deliberately does NOT lead with the RVA
// token, because scripts/lint_rva_bodies.py binds a function to the first RVA it
// sees in the preceding comment, and an earlier draft of this block made the
// lint read FillCamCars as a second body for that RVA).
//
// The most-separated-pair function is NOT re-implemented here. The exe has one body
// (Race/RaceCamera.cpp:182 RaceCamera::MostSeparatedPair) and
// scripts/lint_rva_bodies.py enforces one body per RVA per target; a second was
// written here first and the lint correctly rejected it (DUP-IN-TARGET).
//
// Using the existing body is also the more faithful choice, not merely the
// permitted one: RaceCamCar's own field comments (RaceCamera.h) name exactly the
// sources FUN_0040e180 reads — pos = vehicle +0x30/+0x34/+0x38 via FUN_0046d4a0,
// active = IsCarSlotActive 0x0040e370, alive = FUN_0046c7b0 == 1, dead_flag =
// FUN_0046cbb0 out1 — and its loop, its `<=` tie rule and its -1 tail fixups are
// the transcription of 0x0040e2f3 .. 0x0040e330 (cited in RaceCamera.cpp). What H3 supplies is the ARRAY,
// filled from the readers legs H1a/H1b made live, so the function answers the
// original's question on the original's inputs.
//
// OUT-PARAM ORDER is source-verified at RaceCamera.cpp:206-207: first out = INNER
// index, second = OUTER. FUN_00442a60's `local_3c`/`local_38` take them in that
// order, so they are passed through unswapped.
//
// Residual, registered: that body is DEMOTED C4->C2 (2026-09-29, hooks.csv:877)
// because it uses std::sqrt where the .asi twin uses the RW fast-sqrt, a 7.8%
// magnitude disagreement. Only the ARGMAX matters here, and both functions are
// monotone in the squared length, so the selected pair should be unaffected —
// an argument, not a measurement. Gate H3-PAIR logs the selected indices.
// ---------------------------------------------------------------------------

// Fill the RaceCamCar array FUN_0040e180 consumes, from the live record.
void FillCamCars(RaceCamCar cars[4]) {
    for (int i = 0; i < 4; ++i) {
        cars[i] = RaceCamCar{};
        cars[i].active = IsCarSlotActive(i) != 0u;                          // 0x0040e370
        cars[i].alive  = VehicleSlotGetter(static_cast<std::uint32_t>(i)) == 1u;  // 0x0046c7b0
        std::uint32_t st = 0u, sec = 0u;
        VehicleCarStateRead(static_cast<std::uint32_t>(i), &st, &sec);      // 0x0046cbb0
        cars[i].dead_flag = (st != 0u);
        cars[i].dead_ms   = static_cast<float>(sec);
        std::uint32_t p = 0u;
        if (PtrCompute881ec8(&p, static_cast<std::uint32_t>(i)) && p) {     // 0x0046d4a0
            const char* r = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(p));
            cars[i].pos[0] = *reinterpret_cast<const float*>(r + 0x30);
            cars[i].pos[1] = *reinterpret_cast<const float*>(r + 0x34);
            cars[i].pos[2] = *reinterpret_cast<const float*>(r + 0x38);
        }
        cars[i].race_pct = RacePct(i);                                      // 0x00408ad0
    }
}

// The inputs FUN_00442a60 reads, one set per call. The live path fills them from
// the record readers (FillRefDistIn); the U-9219 replay fills them from a capture
// of the original (verify/d3_u9219_20261013, scenario_launch.py --refdist-probe).
struct RefDistIn {
    RaceCamCar cars[4];          // active / alive / dead_flag / pos / race_pct, as FillCamCars
    std::uint32_t st[4];         // FUN_0046cbb0 out1
    bool pos_ok[4];              // FUN_0046d4a0 returned a pointer
};
struct RefDistOut {
    float rd[4];
    bool stored;                 // the 0x008989a8 / 0x008989c8 stores ran
    int ref, other;
};

namespace {
void FillRefDistIn(RefDistIn* in) {
    FillCamCars(in->cars);
    for (int i = 0; i < 4; ++i) {
        std::uint32_t sec = 0u;
        in->st[i] = 0u;
        VehicleCarStateRead(static_cast<std::uint32_t>(i), &in->st[i], &sec);   // FUN_0046cbb0
        std::uint32_t p = 0u;
        in->pos_ok[i] = PtrCompute881ec8(&p, static_cast<std::uint32_t>(i)) && p;  // FUN_0046d4a0
    }
}
}  // namespace

// ---------------------------------------------------------------------------
// 0x00442a60  Spectator::ComputeDistances — the selection and distance body on
// explicit inputs. Value-identical to the pre-2026-10-13 live body (same
// expressions); G-CTRL in verify/d3_u9219_20261013/PREREG_U9219.md checks it.
// ---------------------------------------------------------------------------
void ComputeDistancesCore(const RefDistIn& in, RefDistOut* out) {
    for (int i = 0; i < 4; ++i) out->rd[i] = 0.0f;      // the entry zeroing
    out->stored = false; out->ref = -1; out->other = -1;

    int a = 4, b = 0;
    RaceCamera::MostSeparatedPair(in.cars, &a, &b);     // FUN_0040e180

    float pa = in.cars[a].race_pct;                     // FUN_00408ad0
    float pb = in.cars[b].race_pct;
    if ((pa <= kWrapHi) || (kWrapLo <= pb)) {
        if ((kWrapHi < pb) && (pa < kWrapLo)) pb -= kLapSpan;
    } else {
        pa -= kLapSpan;
    }

    const std::uint32_t sa = in.st[a], sb = in.st[b];   // FUN_0046cbb0 out1 (state)

    // [U-9205 fix, 2026-10-09; verify/d3_bscore_20261009/PREREG_REFDIST_FIX.md] The reference is the
    // pair's LEADING car (higher race_pct) unless its state is non-zero. This replaced
    // `if (pb < pa) ref = b` ("the rearmost"), which inverted every distance. From the bytes:
    //   0x00442b3a fld pa; fcomp pb; test ah,0x41; jne 0x00442b6a      (jump when pa <= pb)
    //   pa > pb : sa == 0 -> ref a (0x00442b88); else sb == 0 -> ref b (0x00442b60); else exit 0x00442c6e
    //   pa <= pb: sb == 0 -> ref b (0x00442b72); else sa == 0 -> ref a (0x00442b88); else exit 0x00442c6e
    // The exits skip the index stores at 0x00442b91 / 0x00442b97.
    int ref, other;
    if (pa > pb) {
        if (sa == 0u)      { ref = a; other = b; }
        else if (sb == 0u) { ref = b; other = a; }
        else return;
    } else {
        if (sb == 0u)      { ref = b; other = a; }
        else if (sa == 0u) { ref = a; other = b; }
        else return;
    }
    out->stored = true; out->ref = ref; out->other = other;   // 0x00442b91 / 0x00442b97
    if (!in.cars[ref].active) return;                               // FUN_0040e370
    if (!in.cars[ref].alive) return;                                // FUN_0046c7b0
    if (!in.pos_ok[ref]) return;                                    // FUN_0046d4a0
    const float rx = in.cars[ref].pos[0], rz = in.cars[ref].pos[2];

    for (int i = 0; i < 4; ++i) {
        if (!in.cars[i].active) continue;
        if (!in.cars[i].alive) continue;
        if (!in.pos_ok[i]) continue;
        const float cx = in.cars[i].pos[0], cz = in.cars[i].pos[2];
        const float dx = rx - cx;
        const float dz = rz - cz;                                   // y term is 0.0
        out->rd[i] = std::sqrt(dx * dx + dz * dz) * kDistScale;     // FUN_004c3ac0 * 0.8
    }
}

// ---------------------------------------------------------------------------
// Live entry for the body above (not a second body; the comment does not lead
// with the RVA token, see the lint note above FillCamCars): fill the inputs from
// the record, write the outputs to 0x008989b0 / 0x008989a8 / 0x008989c8.
// ---------------------------------------------------------------------------
void ComputeDistances() {
    RefDistIn in;
    FillRefDistIn(&in);
    RefDistOut out;
    ComputeDistancesCore(in, &out);
    float* const refdist = reinterpret_cast<float*>(0x008989b0u);
    for (int i = 0; i < 4; ++i) refdist[i] = out.rd[i];
    if (out.stored) {
        *reinterpret_cast<std::int32_t*>(0x008989a8u) = out.ref;    // 0x00442b91
        *reinterpret_cast<std::int32_t*>(0x008989c8u) = out.other;  // 0x00442b97
    }
}

// ---------------------------------------------------------------------------
// [U-9219] MEASUREMENT ONLY, env-gated (MASHED_REFDIST_REPLAY=<a.rd.tsv>[;...],
// report MASHED_REFDIST_REPLAY_OUT). Feeds each captured FUN_00442a60 call's
// inputs to ComputeDistancesCore and compares the outputs with the original's.
// Row: n clk ba8c a8_in c8_in cars a8 c8 rd, cars = 4 x act:alive:st:st2:pct:x:y:z:slot
// (u32 hex for st..z). Writes nothing into game state.
// ---------------------------------------------------------------------------
namespace {
std::vector<std::string> Split(const std::string& s, char d) {
    std::vector<std::string> r; std::size_t p = 0;
    for (;;) { const std::size_t e = s.find(d, p); r.push_back(s.substr(p, e == std::string::npos ? std::string::npos : e - p));
               if (e == std::string::npos) break; p = e + 1; }
    return r;
}
float HexF(const std::string& h) {
    const std::uint32_t u = static_cast<std::uint32_t>(std::strtoul(h.c_str(), nullptr, 16));
    float f; std::memcpy(&f, &u, 4); return f;
}
void MaybeReplay() {
    const char* inList = std::getenv("MASHED_REFDIST_REPLAY");
    if (!inList) return;
    const char* outp = std::getenv("MASHED_REFDIST_REPLAY_OUT");
    std::FILE* o = outp ? std::fopen(outp, "w") : nullptr;
    if (!o) return;
    std::fprintf(o, "file\tn\tclk\tba8c\torig_a8\torig_c8\tport_stored\tport_ref\tport_other\torig_rd\tport_rd\n");
    for (const std::string& path : Split(inList, ';')) {
        std::FILE* f = std::fopen(path.c_str(), "rb");
        if (!f) { std::fprintf(o, "#MISSING %s\n", path.c_str()); continue; }
        std::string all;
        { char buf[1 << 16]; std::size_t n; while ((n = std::fread(buf, 1, sizeof buf, f)) > 0) all.append(buf, n); }
        std::fclose(f);
        std::size_t p = all.find('\n') + 1;
        while (p < all.size()) {
            std::size_t e = all.find('\n', p); if (e == std::string::npos) e = all.size();
            std::string ln = all.substr(p, e - p); if (!ln.empty() && ln.back() == '\r') ln.pop_back();
            p = e + 1;
            const std::vector<std::string> c = Split(ln, '\t');
            if (c.size() != 9) continue;
            const std::vector<std::string> cars = Split(c[5], ';');
            if (cars.size() != 4) continue;
            RefDistIn in{};
            bool ok = true;
            for (int i = 0; i < 4 && ok; ++i) {
                const std::vector<std::string> q = Split(cars[i], ':');
                if (q.size() != 9) { ok = false; break; }
                in.cars[i].active    = (q[0] != "x") && std::atoi(q[0].c_str()) != 0;   // FUN_0040e370
                in.cars[i].alive     = std::atoi(q[1].c_str()) == 1;                      // FUN_0046c7b0
                in.st[i]             = static_cast<std::uint32_t>(std::strtoul(q[2].c_str(), nullptr, 16));
                in.cars[i].dead_flag = in.st[i] != 0u;
                in.cars[i].race_pct  = HexF(q[4]);
                in.cars[i].pos[0] = HexF(q[5]); in.cars[i].pos[1] = HexF(q[6]); in.cars[i].pos[2] = HexF(q[7]);
                in.pos_ok[i] = true;
            }
            if (!ok) continue;
            RefDistOut out;
            ComputeDistancesCore(in, &out);
            std::string prd;
            for (int i = 0; i < 4; ++i) {
                std::uint32_t u; std::memcpy(&u, &out.rd[i], 4);
                char b[12]; std::snprintf(b, sizeof b, "%x", u);
                if (i) prd += ';';
                prd += b;
            }
            std::fprintf(o, "%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\n", path.c_str(), c[0].c_str(), c[1].c_str(),
                         c[2].c_str(), c[6].c_str(), c[7].c_str(), out.stored ? 1 : 0, out.ref, out.other,
                         c[8].c_str(), prd.c_str());
        }
    }
    std::fclose(o);
}
}  // namespace

// [U-9683] Producer order vs the AI tick, verify/d3_u9683_20261016/PREREG_u9683.md.
// MASHED_REFDIST_AFTER_AI moves the producer call from UpdateRace (before the AI tick of the
// frame) to the end of UpdateCar (after every car's physics and the progress tick), which is the
// producer's place in the original's per-frame order. MASHED_REFDIST_TRACE is a measurement-only
// witness: one row per producer call (P) and per AI tick entry (T), no game state written.
namespace {
bool RefDistAfterAiOn() {
    static const bool s_on = [] {
        const char* e = std::getenv("MASHED_REFDIST_AFTER_AI");
        return e && e[0] && e[0] != '0';
    }();
    return s_on;
}

long s_rdFrame = 0;   // bumped once per UpdateCar race frame, before player physics (no dependence on the order under test)

std::FILE* RdTrace() {
    static std::FILE* f = nullptr;
    static bool init = false;
    if (!init) {
        init = true;
        if (const char* p = std::getenv("MASHED_REFDIST_TRACE")) {
            f = std::fopen(p, "w");
            if (f) std::fprintf(f, "kind\tord\tsite\tba8c\tclk\trd\tref\tother\n");
        }
    }
    return f;
}

void RdRow(char kind, int site) {
    std::FILE* const f = RdTrace();
    if (!f) return;
    const std::uint32_t* const rd = reinterpret_cast<const std::uint32_t*>(0x008989b0u);
    std::fprintf(f, "%c\t%ld\t%c\t%d\t%d\t%x;%x;%x;%x\t%d\t%d\n", kind, s_rdFrame, kind == 'T' ? '-' : (site ? 'A' : 'U'),
                 *reinterpret_cast<const std::int32_t*>(0x0063ba8cu), *reinterpret_cast<const std::int32_t*>(0x007f0ff4u),
                 rd[0], rd[1], rd[2], rd[3], *reinterpret_cast<const std::int32_t*>(0x008989a8u),
                 *reinterpret_cast<const std::int32_t*>(0x008989c8u));
    std::fflush(f);
}
}  // namespace

// Default-ON entry point. Called once per race tick from TrackRenderer, at the site named by
// `site` (0 = UpdateRace, 1 = after the AI block). extern "C" with a flat name: the caller lives
// in mashed_re::D3d9Render and cannot re-open mashed_re::Race from there.
void ComputeDistancesTickImpl(int site) {
    static bool s_replayed = false;
    if (!s_replayed) { s_replayed = true; MaybeReplay(); }   // [U-9219] env-gated, no state written
    static const bool s_on = (std::getenv("MASHED_NO_REFDIST") == nullptr);   // DEFAULT-ON 2026-10-09 (verify/d3_b2flip_20261009); MASHED_REFDIST is now a no-op
    if (!s_on) return;
    ComputeDistances();
    RdRow('P', site);
}

bool RefDistAfterAi() {
    static bool s_logged = false;
    const bool on = RefDistAfterAiOn();
    if (on && !s_logged) {
        s_logged = true;
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {   // the game's own witness that the knob took
            std::fprintf(lf, "[U-9683] MASHED_REFDIST_AFTER_AI: refdist producer runs after the AI block\n");
            std::fclose(lf);
        }
    }
    return on;
}

void RefDistTraceFrame() { ++s_rdFrame; }

void RefDistTraceAiEntry() { RdRow('T', 0); }

}  // namespace Race
}  // namespace mashed_re

extern "C" void __cdecl RaceComputeDistancesTick(int site) {
    mashed_re::Race::ComputeDistancesTickImpl(site);
}

extern "C" int __cdecl RaceRefDistAfterAi() { return mashed_re::Race::RefDistAfterAi() ? 1 : 0; }

extern "C" void __cdecl RaceRefDistTraceFrame() { mashed_re::Race::RefDistTraceFrame(); }

extern "C" void __cdecl RaceRefDistTraceAiEntry() { mashed_re::Race::RefDistTraceAiEntry(); }

