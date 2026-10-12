// Mashed RE — challenge-row movers and the leaf queries they call.
//
// One body per RVA, two bindings. Each core below is the single port of its
// RVA. The hook build binds it to the original image's globals (the dllexport
// in the same .cpp); the standalone binds it to its own menu state
// (Frontend/MenuNavSM.cpp). The table at 0x007f0a40 is read live in both: the
// standalone mirrors the save span to that address (Nav_GameStateLoadSave).
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#pragma once
#include <cstdint>

namespace mashed_re {
namespace Frontend {

// 0x00430830 (FUN_00430830, SplitScreenTrackAssignment): one dword of the
// 0x007f0a40 table, row `p` (stride 0x30), column chosen by `mode`
// (DAT_0067e9fc). Modes outside 2..10 return 0. Body: MenuScoreSort.cpp.
std::uint32_t SplitScreenTrackAssignmentFor(std::int32_t mode, int p);

// 0x00430910 (FUN_00430910, MenuOptionSlotGet): `mode` = DAT_0067e9fc,
// `sel` = DAT_0067f184, `row` = DAT_0067f17c. Body: FrontendState.cpp.
int MenuOptionSlotGetFor(std::int32_t mode, std::int32_t sel, std::int32_t row);

// 0x004309b0 (FUN_004309b0, FrontendModeIndex): game mode -> table column.
// Body: FrontendMode.cpp.
int FrontendModeIndexFor(std::int32_t mode);

// State and callees the two row movers touch. The pointers name the four
// globals by their original addresses; the callees are FUN_00430830,
// FUN_00430910 and FUN_0042f6b0 as the binding sees them.
struct RowMoverIo {
    std::int32_t* row;    // DAT_0067f17c
    std::int32_t* sel;    // DAT_0067f184
    std::int32_t* mode;   // DAT_0067e9fc (read by the DOWN mover only)
    std::int32_t* flag;   // DAT_0067f1a4
    std::int32_t* ea68;   // DAT_0067ea68 (read once at entry)
    int  (*avail)(int row);   // FUN_00430830
    int  (*optValid)();       // FUN_00430910
    void (*modeSync)();       // FUN_0042f6b0
};

// 0x004323c0 FUN_004323c0 (MenuCursorBack), whole body. Body: FrontendNav.cpp.
void RowMoveBack(const RowMoverIo& io);

// 0x004322c0 FUN_004322c0 from 0x004322ea, i.e. the part after
// FUN_0042aa00(1) and the slot-kind 6/7 test (0x004322c0..0x004322e4), which
// the caller owns. Body: FrontendNav.cpp.
void RowMoveFwd(const RowMoverIo& io);

}  // namespace Frontend
}  // namespace mashed_re
