// SubstepInterleave.cpp — [u9778 2026-10-21] MASHED_SUBSTEP_INTERLEAVE=1, default OFF (verify/u9778_20261021/PREREG_u9778.md).
//
// The original's substep order is substep-outer, car-inner (MASHED.exe.unpatched, capstone):
//   FUN_00470c70  0x00471110..0x00471141  sub = min(rem, 0x19); FUN_004709a0((float)sub, ..) at 0x00471137; rem -= sub
//   FUN_004709a0  0x004709e5 / 0x00470c47..0x00470c53  for car = 0..15: the substep body, the car<->car loop
//                 0x00470b23..0x00470c1f (0x00469df0 at 0x00470bcd, other cars j > car), the selector swap 0x00470c31..41
// so car i's car<->car contact in substep 2 reads car j > i after j's substep 1. The port's StepCar runs one car's whole
// frame (all substeps, then its tail) before the next car, so there car j is still at its frame-start pose.
//
// Mechanism: each car's StepCar runs in its own fiber. StepCar calls VehiclePhysics_IlYield (VehiclePhysicsRun.cpp) at
// the end of every substep, which switches back here. InterleaveRun resumes the fibers in slot order once per round
// until every StepCar has returned: round 0 = each car's pre-part + substep 1, round 1 = substep 2, round 2 = the tails.
// Fibers carry the x87 / SSE control state (FIBER_FLAG_FLOAT_SWITCH).
#include "VehiclePhysicsRun.h"

#include <cstdio>
#include <cstdlib>
#include <windows.h>

namespace mashed_re {
namespace Vehicle {
namespace {

struct IlArg { int slot; float dt; PlayerCarIO* io; };

void*  g_main = nullptr;
void*  g_fiber[16] = {};
IlArg  g_arg[16];
bool   g_done[16] = {};
int    g_cur = -1;   // slot whose fiber is running, -1 = main

VOID CALLBACK IlEntry(LPVOID p) {
    IlArg* a = static_cast<IlArg*>(p);
    for (;;) {
        VehiclePhysics_StepCar(a->slot, a->dt, *a->io);
        g_done[a->slot] = true;
        SwitchToFiber(g_main);   // resumed next frame with a new g_arg[slot]
    }
}

}  // namespace

bool VehiclePhysics_InterleaveEnabled() {
    static const bool s = [] { const char* e = std::getenv("MASHED_SUBSTEP_INTERLEAVE"); return e && e[0] == '1'; }();
    return s;
}

bool VehiclePhysics_IlInFiber() { return g_cur >= 0; }

void VehiclePhysics_IlSwitchToMain() {
    if (g_cur >= 0) SwitchToFiber(g_main);
}

bool VehiclePhysics_InterleaveRun(int n, const int* slots, float dt, PlayerCarIO* ios) {
    if (!g_main) {
        g_main = ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
        if (!g_main && GetLastError() == ERROR_ALREADY_FIBER) g_main = GetCurrentFiber();
        if (!g_main) return false;
    }
    for (int k = 0; k < n; ++k) {
        const int s = slots[k];
        if (s < 0 || s >= 16) return false;
        if (!g_fiber[s]) {
            g_fiber[s] = CreateFiberEx(64 * 1024, 4 * 1024 * 1024, FIBER_FLAG_FLOAT_SWITCH, IlEntry, &g_arg[s]);
            if (!g_fiber[s]) return false;
        }
    }
    for (int k = 0; k < n; ++k) {
        const int s = slots[k];
        g_arg[s] = { s, dt, &ios[k] };
        g_done[s] = false;
    }
    // MASHED_SUBSTEP_INTERLEAVE_ORDER=car (diagnostic): run each fiber to completion in slot order, i.e. the per-car
    // order through the fiber path; it must reproduce the knob-off default bit for bit (the mechanism control).
    static const bool s_carOrder = [] { const char* e = std::getenv("MASHED_SUBSTEP_INTERLEAVE_ORDER"); return e && e[0] == 'c'; }();
    for (int round = 0; round < 64; ++round) {
        bool any = false;
        for (int k = 0; k < n; ++k) {
            const int s = slots[k];
            if (g_done[s]) continue;
            any = true;
            g_cur = s;
            do { SwitchToFiber(g_fiber[s]); } while (s_carOrder && !g_done[s]);
            g_cur = -1;
        }
        if (!any) return true;
    }
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "[u9778] InterleaveRun: 64 rounds without every car finishing\n");
        std::fclose(lf);
    }
    return true;
}

}  // namespace Vehicle
}  // namespace mashed_re
