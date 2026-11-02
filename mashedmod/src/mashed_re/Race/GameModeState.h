// Mashed RE — port-state migration 3/3 (L-78 (c)): the original game-mode machine's state kept alive in the exe.
// Exe only. PREREG and arms: verify/portstategameflow_20261102/PREREG_MATCH-PORTSTATE-GAMEFLOW.md, RESULT_*.md.
#pragma once

namespace mashed_re {
namespace Race {

// MASHED_PORTSTATE_GAMEFLOW (default OFF, read once). Every function below returns at once with the knob OFF.
bool GameModeState_On();

// The phase word DAT_00771968 that 0x004929d0 switches on. The exe's GameFlow / dev-race route publish the
// original's value at the transition the original makes (0x00492770 / 0x00493590 = 1, 0x004929d0 case 1 -> 2,
// case 2 -> 3 at 0x00492a5b, case 3 -> 7 pause, case 7 -> 3 resume, case 3 -> 5 round end).
void GameModeState_SetPhase(int phase);

// Before the course load: the game mode word DAT_0067e9fc, which the original's frontend (0x0043dfd0) writes before
// the race launch and therefore before course load and before 0x0046b540 reads it.
void GameModeState_PreLoad(int gameMode);

// At the exe's course load (after CourseTableState_OnCourseLoad): the course index k of course_id in the original's
// course table (0x005f2728 row whose record dword 0 = course_id), published where the original keeps it:
// 0x0067f17c (the launch's source for 0x0040e170, 0x0043dfd0), 0x0067ea94 when DAT_0067e9fc == 10 (0x0046b573),
// 0x0063ba7c (0x0040e170) and 0x0063ba78 (0x0040d45b, case 2 of 0x004929d0).
void GameModeState_OnCourseLoad(int course_id);

// 0x0046b540's selector input, the original's form (0x0046b56c..0x0046b585): 0x0040ce80(DAT_0067e9fc == 10 ?
// DAT_0067ea94 : DAT_0067f17c). Returns false (out untouched) when the knob is OFF or the course table row is
// not seeded (MASHED_PORTSTATE_COURSE OFF), so the caller keeps its own selector.
bool GameModeState_HandlingType(int* out);

} // namespace Race
} // namespace mashed_re
