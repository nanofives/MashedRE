// Mashed RE - whole data image loader (IMG-2-LOADER, decision L-149 (a)). Exe only, MASHED_ORIG_TOP path only.
// Spec: verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md section 2. Format: re/tools/image/mashed_image.py.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Compat {

// MASHED_PORTSTATE_IMAGE=1 (default OFF, read once). Only WinMain's MASHED_ORIG_TOP branch consults it.
bool PortStateImage_On();

// WinMain, MASHED_ORIG_TOP branch, before any game code: finds mashed_image.bin (MASHED_IMAGE, the exe's folder,
// MASHED_ROOT\original, then original\ under each parent of the exe's folder), checks magic, version, source SHA-256
// (the anchor), trailer SHA-256, each range FNV-1a and each range's bounds (inside the B17 pad, at or above
// 0x005cc000), then copies the three payloads into the pad and zeroes the IAT range 0x005cc000..0x005cc2e8.
// Any failure writes nothing. Returns true when the image was copied.
bool PortStateImage_Apply();

// True after a successful PortStateImage_Apply: the per-address seeds are skipped and only witnessed.
bool PortStateImage_Applied();

// After the per-run log truncate: the "[portstate-image]" lines held since Apply, plus the in-process self-test.
void PortStateImage_Log(const char* logPath);

} // namespace Compat
} // namespace mashed_re

// C linkage for the knob-path wrappers (Match/PortTop_*.cpp) that skip their own seed writes when the image is in.
extern "C" int PortStateImage_AppliedC(void);
