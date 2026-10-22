// Mashed RE — the FAITH default set (faithflip 2026-10-20, Mariano's decision; verify/faithflip_20261020/PREREG_faithflip.md).
//
// The 38 knobs of the full faithful stack FAITH (verify/d2off_20261020/RESULT_d2off.md) and u9774's five (f5flip) are default ON in the standalone. Each one
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
    // [f5flip 2026-10-20, Mariano's decision] u9774's five (verify/u9774_20261020/RESULT_u9774.md); MASHED_COLL_LUT stays OFF.
    { "A6A_SPIN_ORDER", "1" },     { "TER_SPIN_F32", "1" },        { "FIXUP_F32", "1" },         { "FIXUP_SUM_ORDER", "1" },
    { "FIXUP_SLIDE_ORDER", "1" },
    // [trinflip 2026-10-20, Mariano's decision, after wheelcite C27b] u9773's 3-wheel alignment (verify/u9773_20261020/RESULT_u9773.md).
    { "WCS_TRINORMAL", "1" },
    // [cpcflip 2026-10-20, Mariano's decision] u9776's PC_24 scope around the alignment's FUN_004c52f0 (verify/u9776_20261020/RESULT_u9776.md).
    { "WCS_COMBINE_PC24", "1" },
    // [rbflip 2026-10-21, Mariano's decision] u9777's live rubber-band leaves for A5 (verify/u9777_20261020/RESULT_u9777.md).
    { "A5_RUBBER_LIVE", "1" },
    // [fflip 2026-10-21, Mariano's decision] brecipe's live mode word / fd0 and live slot table for the AI host
    // (verify/brecipe_20261021/RESULT_brecipe.md), flipped with the canonical recipe swap to rule 0 / game mode 10.
    { "MODE_LIVE", "1" },          { "SLOT_TYPE_LIVE", "1" },
    // [soflip 2026-10-21, Mariano's decision] substep's FUN_004709a0 order, substep-outer / car-inner
    // (verify/substep_20261021/RESULT_substep.md); steps only the cars the seeded A4_ORDER pre-phase stepped.
    { "SUBSTEP_OUTER", "1" },
    // [dotflip 2026-10-22, Mariano's decision] dcphys's A6a dot association (verify/dcphys_20261021/RESULT_dcphys.md):
    // the shared A6a body is bit-identical to the original on 6551/6551 sampled calls with it, and misses on 25..174 per
    // race without it.
    { "A6A_DOT_ORDER", "1" },
    // [p2flip 2026-10-22, Mariano's decision] dcphys2's P2 stack (verify/dcphys2_20261022/RESULT_dcphys2.md): with it the
    // shared A6b / A5 / A4 bodies are bit-identical to the original on every sampled call (402 / 3214 / 5403).
    { "A6B_ACOS_CRT", "1" },       { "A6B_NORM_LUT", "1" },        { "A5_RING_ORDER", "1" },     { "A5_DOT_ORDER", "1" },
    { "A5_AIR_F32", "1" },         { "A5_CONTACT_INT", "1" },      { "A5_SPIN_FILD", "1" },      { "A4_DOT_ORDER", "1" },
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
