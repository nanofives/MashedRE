// Mashed RE -- the game thread's x87 precision control (PORT-X87-PC24, L-103 (a)).
//
// The original's game thread runs at CW 0x007f (PC_24) from D3D9 device creation on: IDirect3D9::CreateDevice without
// D3DCREATE_FPU_PRESERVE sets it on the calling thread, which in MASHED.exe is the game thread (SubsystemInit 0x00492270).
// mashed_re.exe creates its device on a worker (CreateDeviceWatched, exe_main.cpp), so its game thread keeps the CRT's
// 0x027f (PC_53) unless InitD3D9 applies PC_24, which it does by default since L-105 (a)
// (2026-11-09); MASHED_NO_X87_PC24=1 opts out. See verify/portstate_20261109/PREREG_X87PC24.md and
// RESULT_PORT-X87-PC24-AUDIT.md (orch/PORT-X87-PC24-AUDIT).
//
// GameThreadPc24() is true only after the exe's InitD3D9 applied the knob. The dev .asi never sets it (MASHED.exe's own
// D3D9 CW applies there), so code shared by both targets keeps its old behaviour in the .asi.
#pragma once
#include <float.h>
#include <cstdlib>

namespace mashed_re {
namespace X87 {

inline bool& GameThreadPc24() { static bool s = false; return s; }

// The precision a callback that reproduces "the exe thread's precision" should run at: PC_24 once the knob is applied,
// else PC_53. MASHED_X87_PC24_PK53=1 keeps PC_53 (interaction arm H_X24PK only).
inline unsigned int ThreadBaselinePc() {
    static const bool s_pk53 = [] { const char* e = std::getenv("MASHED_X87_PC24_PK53"); return e && e[0] && e[0] != '0'; }();
    return (GameThreadPc24() && !s_pk53) ? _PC_24 : _PC_53;
}

}  // namespace X87
}  // namespace mashed_re
