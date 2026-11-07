// Mashed RE — port-state migration 3/3 (L-78 (c)): the original game-mode machine's state kept alive in the exe.
// Exe only. PREREG: verify/portstategameflow_20261102/PREREG_MATCH-PORTSTATE-GAMEFLOW.md.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// The original runs its session from the 7-case machine 0x004929d0 on DAT_00771968
// (re/analysis/game_state/0x004929d0.md) and keeps the race's course in four index words. The exe runs the session
// from Race::GameFlow / RaceSession and the dev-race route in exe_main, and kept none of these words: at course load
// DAT_0067e9fc was 0 (written after the load), DAT_0067f17c / DAT_0067ea94 / DAT_0063ba7c / DAT_0063ba78 had no exe
// writer, and DAT_00771968 was written once (1, Boot/FrameDispatch.cpp). So the matched selector chain of 0x0046b540
// (0x00430790 / 0x0042f6a0 / 0x00431d70 -> 0x0040ce80) read row 0 (Training) on every track
// (verify/portstatecourse_20261101/RESULT_MATCH-PORTSTATE-COURSE.md, T13 row 0x0040ce80).
//
// With MASHED_PORTSTATE_GAMEFLOW the words are the exe's state at the original addresses, written at the points the
// original writes them. The original's writers, decomp_pc.py on a read-only pool clone 2026-11-02:
//   DAT_00771968 = 1      0x00492770 (main-loop init), 0x00493590
//   DAT_00771968 = 2/3/4/5/6/7   0x004929d0 cases (see GameModeState_SetPhase callers)
//   DAT_0067e9fc          0x0043df00 (= 0xb, attract), 0x0043dfd0 (the frontend machine, before the launch)
//   DAT_0067f17c          0x0043dfd0 (the challenge row); the launch stores it: FUN_0040e170(DAT_0067f17c)
//   DAT_0067ea94          0x00431d00 (= 0), 0x0043dfd0 screens 0x12 / 0x18 (cursor 0..0xc over FUN_00430830)
//   DAT_0063ba7c          0x0040e170 only (callers 0x0043df00 with 0, 0x0043dfd0 with DAT_0067f17c)
//   DAT_0063ba78          0x0040d45b (0x0040d440 = case 2 of 0x004929d0)
// Readers of the index: 0x0046b540 (0x0046b56c..0x0046b585), 0x0040d040 / 0x0040d110 (both the same
// `DAT_0067e9fc == 10 -> DAT_0067ea94` arm, plus `== 6`), 0x0040d250 / 0x0040d440 (DAT_0063ba7c).
//
// The course index k is the row of the original's course table whose record's dword 0 is the loaded COURSE.LUA
// Course_Id (0x0040d250 returns exactly that dword for row DAT_0063ba7c). In the original's mode 10 the course
// loaded is row DAT_0067f17c (0x0043dfd0 case 10 -> FUN_0040e170(DAT_0067f17c)) and the handling row is
// DAT_0067ea94 (0x0046b573). DAT_0067f17c / DAT_0063ba7c / DAT_0063ba78 = k is what is published.
//
// [D4-UNC-PHYS-2 2026-11-07, U-9783] DAT_0067ea94 is NOT published from the course any more. The original never
// writes it at course load: its writers are 0x00431d00 (= 0) and the setup-screen option of 0x0043dfd0 (screen 0x12
// row 4, 0x0044024d..0x00440268 / 0x0044079d..0x004407b8; screen 0x18 row 3), stepped by the left / right
// controls and filtered by the mode-10 availability words 0x007f0a44 + i*0x30 (FUN_00430830, 0x00430852). Measured
// on the original (verify/d4uncphys_20261107/RESULT_D4-UNC-PHYS.md): on the harness route (tracks 0/3/10/11) and on
// the menu Quick Battle route DAT_0067ea94 stays 0 on every course and A3 runs type 6 (_DAT_00613108 = 105.0); only
// a pick of the setup option moves it (row 3 picked on a Training race: type 18, 100.0 / 42000.0, with
// DAT_0063ba7c = DAT_0063ba78 = 0). So the handling row and the course row are separate selections, and the
// previous `DAT_0067ea94 = k` gave non-Training courses a handling row the original does not run there. The word
// keeps whatever the exe's frontend wrote (Frontend/GameModeCarSelect.cpp, the 0x00431d00 port, writes 0).
#include "GameModeState.h"
#include "../Track/CourseTableState.h"

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

extern "C" std::uint32_t __cdecl PtrTable5f2770Get(std::uint32_t param_1);   // 0x0040ce80 (Util/PromoLoop_round49.cpp)
extern "C" int __cdecl GetRaceSubMode();                                    // 0x0042f6a0 (Match/MatchS07_0042f6a0.cpp)

namespace mashed_re {
namespace Race {
namespace {

constexpr std::uintptr_t kPhase  = 0x00771968u;   // DAT_00771968, 0x004929d0's switch word
constexpr std::uintptr_t kMode   = 0x0067e9fcu;   // DAT_0067e9fc, FUN_0042f6a0
constexpr std::uintptr_t kRow    = 0x0067f17cu;   // DAT_0067f17c, FUN_00430790
constexpr std::uintptr_t kQbRow  = 0x0067ea94u;   // DAT_0067ea94, FUN_00431d70
constexpr std::uintptr_t kSelCrs = 0x0063ba7cu;   // DAT_0063ba7c, FUN_0040e170 store
constexpr std::uintptr_t kLdCrs  = 0x0063ba78u;   // DAT_0063ba78, 0x0040d45b store
constexpr std::uintptr_t kTable  = 0x005f2770u;   // PTR_PTR_005f2770 (0x005f2728 once seeded)

std::int32_t& I32(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }

void Log(const char* fmt, ...) {
    if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::vfprintf(fp, fmt, ap);
        va_end(ap);
        std::fclose(fp);
    }
}

// Row k of the seeded table, or 0 when the table or the row is not seeded (MASHED_PORTSTATE_COURSE OFF).
std::uint32_t Row(int k) {
    const std::uint32_t tab = static_cast<std::uint32_t>(I32(kTable));
    if (!tab || k < 0 || k >= 13) return 0;
    return static_cast<std::uint32_t>(I32(tab + 4u * static_cast<std::uint32_t>(k)));
}

} // namespace

bool GameModeState_On() {
    static const bool s_on = (std::getenv("MASHED_PORTSTATE_GAMEFLOW") != nullptr);
    return s_on;
}

void GameModeState_SetPhase(int phase) {
    if (!GameModeState_On()) return;
    const int was = I32(kPhase);
    I32(kPhase) = phase;
    Log("[portstate-gameflow] phase %d -> %d (mode=%d row=%d qbrow=%d sel=%d ld=%d)\n",
        was, phase, I32(kMode), I32(kRow), I32(kQbRow), I32(kSelCrs), I32(kLdCrs));
}

void GameModeState_PreLoad(int gameMode) {
    if (!GameModeState_On()) return;
    I32(kMode) = gameMode;
}

void GameModeState_OnCourseLoad(int course_id) {
    if (!GameModeState_On()) return;
    const int k = Track::CourseTableState_IndexOf(course_id);
    if (k >= 0) {
        I32(kRow) = k;                               // the launch's source (0x0043dfd0 -> FUN_0040e170(DAT_0067f17c))
        // DAT_0067ea94 (kQbRow, the mode-10 handling row read at 0x0046b573) is left alone: see the U-9783 note above.
        I32(kSelCrs) = k;                          // FUN_0040e170 (0x0040e170)
        I32(kLdCrs)  = I32(kSelCrs);                 // 0x0040d45b: DAT_0063ba78 = DAT_0063ba7c
    }
    // Witness: the row the original's 0x0046b540 selector would read now, and its handling key (+4, 0x0040ce80).
    const int v = (I32(kMode) == 10) ? I32(kQbRow) : I32(kRow);
    const std::uint32_t row = Row(v);
    const int key = row ? I32(row + 4u) : -1;
    const int rid = row ? I32(row) : -1;
    Log("[portstate-gameflow] course=%d k=%d mode=%d row=%d qbrow=%d sel=%d ld=%d v_rec_id=%d v_key=%d\n",
        course_id, k, I32(kMode), I32(kRow), I32(kQbRow), I32(kSelCrs), I32(kLdCrs), rid, key);
}

bool GameModeState_HandlingType(int* out) {
    if (!GameModeState_On()) return false;
    int v = I32(kRow);                                // 0x0046b56c  FUN_00430790
    if (GetRaceSubMode() == 10) v = I32(kQbRow);      // 0x0046b573..0x0046b57d  FUN_0042f6a0 / FUN_00431d70
    if (!Row(v)) return false;                        // table not seeded: 0x0040ce80 would dereference 0
    *out = static_cast<int>(PtrTable5f2770Get(static_cast<std::uint32_t>(v)));   // 0x0046b585  FUN_0040ce80
    static int s_logged = 0;
    if (s_logged < 1) {
        ++s_logged;
        Log("[portstate-gameflow] a3 mode=%d v=%d type=%d\n", GetRaceSubMode(), v, *out);
    }
    return true;
}

} // namespace Race
} // namespace mashed_re
