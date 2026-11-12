#pragma once
// PORT-OT-SEED-CODEPTR: code-pointer bindings for the MASHED_ORIG_TOP path (exe only). See PortTopCodePtr.cpp.
namespace mashed_re {
namespace Compat {

// MASHED_ORIG_TOP=1 only, before PortTop_WinMain: writes the exe address of each code-pointer slot's target
// (Compat/PortTopCodePtr.inc) into the original-address pad, seeds the data dwords of the same entries, then
// checks every write and logs one [OT_CODEPTR] line to logPath. Returns the number of failed checks.
unsigned PortTopCodePtr_Apply(const char* logPath);

} // namespace Compat
} // namespace mashed_re
