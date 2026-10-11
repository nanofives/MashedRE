// re/tools/veccap/port_image.cpp
//
// Host for the replay-as-C3 lane (replay_c3.py). Links the ported TUs named in
// veccap_registry.FUNCS UNCHANGED, with the same per-TU flags the .asi uses
// (/EHsc /W3 /O2, plus /arch:IA32 for TUs in mashedmod/x87_tus.txt), into an
// image linked /FIXED at 0x30000000. replay_c3.py maps that image into the SAME
// Unicorn address space as original/MASHED.exe and runs the port's machine code
// on the exact memory the original sees: write sets, stack cleanup and reads are
// then measured identically for both bodies.
//
// Nothing here executes natively except the no-op main. Imports the ports reach
// are trapped by replay_c3.py (VirtualQuery is modelled over Unicorn's mapped
// regions; any other import stops the run and is reported, never silently passed).
#include <cstdint>

#include "../../../mashedmod/src/mashed_re/Core/HookSystem.h"
// Stubbed registry: the image is linked /OPT:NOREF, so the ShadowAB templates the
// TUs instantiate are kept and need these symbols. None runs during a replay.
namespace HookSystem {
static HookEntry s_none{};
void Register(std::uintptr_t, void*, const char*) {}
bool Install(std::size_t) { return false; }
bool Uninstall(std::size_t) { return false; }
std::size_t Count() { return 0; }
const HookEntry& At(std::size_t) { return s_none; }
}

int main() { return 0; }
