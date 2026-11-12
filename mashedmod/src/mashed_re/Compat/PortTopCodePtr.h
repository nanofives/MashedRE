#pragma once
// PORT-OT-SEED-CODEPTR: code-pointer bindings for the MASHED_ORIG_TOP path (exe only). See PortTopCodePtr.cpp.
namespace mashed_re {
namespace Compat {

// MASHED_ORIG_TOP=1 and MASHED_OT_CODEPTR=1 (default OFF), before PortTop_WinMain: writes the exe address of each code-pointer slot's target
// (Compat/PortTopCodePtr.inc) into the original-address pad, seeds the data dwords of the same entries, then
// checks every write and logs one [OT_CODEPTR] line to logPath. Returns the number of failed checks.
unsigned PortTopCodePtr_Apply(const char* logPath);

// Writes the lines held since Apply (Apply runs before the exe clears mashed_re.log). No-op when nothing is held.
void PortTopCodePtr_Log(const char* logPath);

} // namespace Compat
} // namespace mashed_re
