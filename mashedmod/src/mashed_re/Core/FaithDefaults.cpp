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
    // [D5-KNOBS-D-SEEDED6 2026-10-30] POS_MODEL, ROUND_GRID_ORDER, PATHPROG_LATE, SPAWN_BASIS_RW, RING_DBUF and
    // STEER_ACOS_CRT left this list with their read sites: the seeded path is the only one in the source
    // (verify/d5knobsd_seeded6_20261030/RESULT_D5-KNOBS-D-SEEDED6.md).
    // [D5-KNOBS-AI-TICK-FIRST 2026-10-31] AI_TICK_FIRST left this list: it had no reader after D5-KNOBS-E1-PHYS-A4A5
    // made the AI tick phase unconditional before the player's step (verify/d5knobsaitf_20261031/RESULT_D5-KNOBS-AI-TICK-FIRST.md).
    // [D5-KNOBS-E5-REST 2026-10-31] ROUND_GRID, TGT3, SLOT_TYPE_LIVE, SUBSTEP_OUTER, SUBSTATE, PARTMORPH, CAR_STATE0 (the
    // MASHED_FLIP_D11077 block) and INTRO_MOVIES left this list with their read sites, and GRID_DIR, which had no reader
    // since D5-KNOBS-RUNARMS-STACKS: the seeded path is the only one in the source
    // (verify/d5knobse5rest_20261031/RESULT_D5-KNOBS-E5-REST.md).
    { "AI_COUNTDOWN_CLOCK", "1" },
    { "AI_CONTACT_EVENT", "1" },
    // [D5-KNOBS-E3-GEOM-AI 2026-10-31] RW_SSE_POINTS, FWD_AT, ROTATE_LUT, ROTATE_PC24, HEADING_FWD_LIVE, AI_LUT (the mask is
    // the constant 7), AI_FWD_ATROW, STEER_SCALE_RDATA, WCS_TRINORMAL (trinflip) and WCS_COMBINE_PC24 (cpcflip) left this list
    // with their read sites: the seeded path is the only one in the source
    // (verify/d5knobse3geom_20261031/RESULT_D5-KNOBS-E3-GEOM-AI.md).
    // [D5-KNOBS-E4-MODE-REG 2026-10-31] PATHPROG (with arm R's shadow-row tick, MASHED_PP_PUBLISH and the race_pct bridge),
    // REASSERT_KEEP_NORMAL, REGISTRY, PICKUPS_ORIG, REG_CARS, REG_CALLBACKS, MODE7, REG_LATE, MODE_LIVE and P3_RESET_PAIR
    // left this list with their read sites: the seeded path is the only one in the source
    // (verify/d5knobse4mode_20261031/RESULT_D5-KNOBS-E4-MODE-REG.md).
    { "CC_PC24", "1" },
    { "A5_DRAFT_LUT", "1" },
    // [p2flip 2026-10-22, Mariano's decision] dcphys2's P2 stack (verify/dcphys2_20261022/RESULT_dcphys2.md): with it the
    // shared A6b / A5 / A4 bodies are bit-identical to the original on every sampled call (402 / 3214 / 5403).
    { "A6B_ACOS_CRT", "1" },       { "A6B_NORM_LUT", "1" },
    // [D5-KNOBS-E2-PHYS-A6FIX 2026-10-31] FIX_5CEA60 and UP_TRINORMAL (f60flip / lk21), A6A_GEAR_ORDER, FIX_5CEA44, u9774's
    // five (f5flip: A6A_SPIN_ORDER, TER_SPIN_F32, FIXUP_F32, FIXUP_SUM_ORDER, FIXUP_SLIDE_ORDER) and A6A_DOT_ORDER (dotflip)
    // left this list with their read sites: the seeded path is the only one in the source
    // (verify/d5knobse2phys_20261031/RESULT_D5-KNOBS-E2-PHYS-A6FIX.md). MASHED_COLL_LUT is not seeded here: it is default
    // ON at its own site since L-26 (Collision/ContactDeps.h, MASHED_NO_COLL_LUT=1 opts out).
    // [D5-KNOBS-E1-PHYS-A4A5 2026-10-30] A6A_LC0, A4_ORDER, A5_FD0_LIVE, A5_RUBBER_LIVE (rbflip) and P2's A5_RING_ORDER,
    // A5_DOT_ORDER, A5_AIR_F32, A5_CONTACT_INT, A5_SPIN_FILD, A4_DOT_ORDER left this list with their read sites: the
    // seeded path is the only one in the source (verify/d5knobse1phys_20261030/RESULT_D5-KNOBS-E1-PHYS-A4A5.md).
    // [D5-FLIP-RFLAME-SORT 2026-10-30, Mariano's decision L-66 (a)] the R_FLAME per-group sorter FUN_0045ac40 ahead of each
    // group's 5-spark scan, as the original's tick FUN_0045ae80 runs it (CALL 0x0045ac40 @0x0045af41), Powerup/PowerupEffects.cpp
    // (verify/d3rflame_20261024/RESULT_D3-CF-RFLAME.md). MASHED_NO_RFLAME_SORT=1 or MASHED_RFLAME_SORT=0 opts out.
    // Measured ON: verify/d5fliprflame_20261030/RESULT_D5-FLIP-RFLAME-SORT.md.
    { "RFLAME_SORT", "1" },
    // [D5-FLIP-MUSIC-NOCDAUDIO 2026-10-30, Mariano's decision L-66 a] no cdaudio.rws stream in Race / Results: the original's
    // race tick FUN_00466b50 (case 3/7) starts no music voice (verify/wsjmusic_20261026/RESULT_D4-WSJ-MUSIC.md). Measured ON:
    // verify/d5flipmusic_20261030/RESULT_D5-FLIP-MUSIC-NOCDAUDIO.md. MASHED_NO_MUSIC_NO_RACE_CDAUDIO=1 or
    // MASHED_MUSIC_NO_RACE_CDAUDIO=0 opts out.
    { "MUSIC_NO_RACE_CDAUDIO", "1" },
    // [D5-FLIP-SAVE-FIRSTRUN 2026-10-30, Mariano's decision L-66 a] the two first-run save writers of Race/GameFlow.cpp
    // (0x00409e30 counter 1 on a missing file, FUN_00496530's gather-byte clear 0x00496568 per active slot), see
    // verify/spanwriters_20261027/RESULT_D4-FU-SPANINIT-WRITERS.md. Measured ON:
    // verify/d5flipsavefirstrun_20261030/RESULT_D5-FLIP-SAVE-FIRSTRUN.md. MASHED_NO_SAVE_FIRSTRUN_WRITERS=1 or
    // MASHED_SAVE_FIRSTRUN_WRITERS=0 opts out.
    { "SAVE_FIRSTRUN_WRITERS", "1" },
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
