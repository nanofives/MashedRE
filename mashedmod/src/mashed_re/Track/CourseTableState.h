// Mashed RE — port-state migration 1/3 (L-78 (c)): the original's course table kept alive in the exe.
// Exe only. PREREG and arms: verify/portstatecourse_20261101/PREREG_MATCH-PORTSTATE-COURSE.md, RESULT_*.md.
#pragma once

namespace mashed_re {
namespace Track {

// MASHED_PORTSTATE_COURSE (default OFF). At the exe's course load: seed the original's static course tables
// (0x005f2570 records, 0x005f2728 pointers, 0x005f33f8 hook table, 0x005f37a0 count) from the bytes of
// MASHED.exe.unpatched, then run the matched selector 0x0041e870 with course_id so 0x0063d7e4 holds the
// original's hook record. Knob OFF it returns before touching anything.
void CourseTableState_OnCourseLoad(int course_id);

// [MATCH-PORTSTATE-GAMEFLOW] The row k (0..12) of the original's course pointer table 0x005f2728 whose record's
// dword 0 (the value 0x0040d250 returns for row k) equals course_id, read from the transcribed tables below, so
// it holds with either knob value. -1 when no row matches.
int CourseTableState_IndexOf(int course_id);

} // namespace Track
} // namespace mashed_re
