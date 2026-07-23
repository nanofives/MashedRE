// Mashed RE — standalone-exe HookSystem no-op stubs (Phase C2).
//
// When mashed_re.exe is built as a standalone (no MASHED.exe to hook), the
// RH_ScopedInstall(...) global initializers still emit calls to
// HookSystem::Register. The dev-only Core/HookSystem.cpp implementation is
// not linked into the exe target, so these calls would be unresolved.
//
// This file provides empty implementations so the standalone links cleanly.
// The hook registry is unused in the standalone — we ARE the implementation,
// not a runtime patcher.
//
// Phase B's audit (re/STANDALONE_DEPS.md) confirms this is the only
// HookSystem symbol called from the reimpl set; the rest of the namespace
// (Install/Uninstall/Count/At) is only referenced by HookSystem.cpp itself.
#include "../Core/HookSystem.h"

namespace HookSystem {

void Register(std::uintptr_t /*target_rva*/, void* /*replacement*/, const char* /*name*/) {
    // No-op: the standalone exe is the implementation; there is nothing to patch.
}

// Phase C2: referenced by the exe-side .cpp set as of 2026-07-23 (the B5c
// in-process C4 self-test in Collision/RwpIntegrator.cpp references
// Count/At/Install/Uninstall; that code is runtime-gated OFF on the exe by
// MASHED_PHYS_C4_SELFTEST, but the symbols must resolve at link time). No-op here.
bool Install  (std::size_t /*index*/) { return false; }
bool Uninstall(std::size_t /*index*/) { return false; }
void InstallAll  () { /* no-op */ }
void UninstallAll() { /* no-op */ }
std::size_t       Count() { return 0; }
const HookEntry&  At(std::size_t /*index*/) {
    static HookEntry s_empty{0, nullptr, "", {0,0,0,0,0}, false};
    return s_empty;
}

} // namespace HookSystem
