// Mashed RE — the FAITH default set (faithflip 2026-10-20, Mariano's decision; verify/faithflip_20261020/PREREG_faithflip.md).
//
// The 38 knobs of the full faithful stack FAITH (verify/d2off_20261020/RESULT_d2off.md) are default ON in the standalone. Each one
// is still read at its own site under its own MASHED_<name>; this TU seeds the process environment with the FAITH value before
// any of those reads, so the default equals the FAITH arm's environment exactly:
//   - MASHED_NO_FAITH=1          seeds nothing: the pre-flip default, exactly;
//   - MASHED_NO_<name>=1         skips that one knob (where a site already had MASHED_NO_<name>, it keeps its meaning);
//   - MASHED_<name> already set  is left as given (explicit values still win).
// init_seg(lib) runs this TU's initializer before every user TU's (.CRT$XCL before .CRT$XCU), so the namespace-scope and
// function-local statics that cache these knobs see the seeded values. _putenv_s updates the CRT copy and the process
// environment, so both std::getenv and GetEnvironmentVariableA read it. Standalone only: the dev .asi never compiles this TU.
#ifdef MASHED_STANDALONE
#pragma warning(disable : 4073)   // initializers put in library initialization area (intended)
#pragma init_seg(lib)

#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace {

struct FaithKnob { const char* name; const char* value; };

// Order and values = STACK['FAITH'] in verify/d2off_20261020/run_arms.py (bare names there are set to "1").
const FaithKnob kFaith[] = {
    { "AI_COUNTDOWN_CLOCK", "1" }, { "POS_MODEL", "1" },           { "PATHPROG", "1" },          { "ROUND_GRID", "1" },
    { "ROUND_GRID_ORDER", "1" },   { "TGT3", "1" },                { "GRID_DIR", "1" },          { "AI_CONTACT_EVENT", "1" },
    { "AI_TICK_FIRST", "1" },      { "PATHPROG_LATE", "1" },       { "REASSERT_KEEP_NORMAL", "1" }, { "SPAWN_BASIS_RW", "1" },
    { "FIX_5CEA60", "1" },         { "UP_TRINORMAL", "1" },        { "RW_SSE_POINTS", "1" },     { "RING_DBUF", "1" },
    { "FWD_AT", "1" },             { "ROTATE_LUT", "1" },          { "ORTHO_LUT", "1" },         { "A6A_LC0", "1" },
    { "ROTATE_PC24", "1" },        { "HEADING_FWD_LIVE", "1" },    { "AI_LUT", "7" },            { "STEER_ACOS_CRT", "1" },
    { "AI_FWD_ATROW", "1" },       { "STEER_SCALE_RDATA", "1" },   { "REGISTRY", "1" },          { "PICKUPS_ORIG", "1" },
    { "REG_CARS", "1" },           { "REG_CALLBACKS", "1" },       { "MODE7", "1" },             { "CC_PC24", "1" },
    { "A4_ORDER", "1" },           { "REG_LATE", "1" },            { "A6A_GEAR_ORDER", "1" },    { "A5_FD0_LIVE", "1" },
    { "A5_DRAFT_LUT", "1" },       { "FIX_5CEA44", "1" },
};

bool Set(const char* var) {
    const char* e = std::getenv(var);
    return e && e[0] && e[0] != '0';
}

struct FaithSeed {
    FaithSeed() {
        if (Set("MASHED_NO_FAITH")) return;
        char on[64], no[64];
        for (const FaithKnob& k : kFaith) {
            std::strcpy(on, "MASHED_");    std::strcat(on, k.name);
            std::strcpy(no, "MASHED_NO_"); std::strcat(no, k.name);
            if (std::getenv(on) || Set(no)) continue;
            _putenv_s(on, k.value);
        }
    }
};
const FaithSeed s_faithSeed;

}  // namespace
}  // namespace mashed_re
#endif  // MASHED_STANDALONE
