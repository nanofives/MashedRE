#include "HookSystem.h"

#include <cstring>
#include <vector>

namespace HookSystem {

// File-scoped registry. Populated by static ctors before DllMain runs.
static std::vector<HookEntry>& Registry() {
    static std::vector<HookEntry> r;
    return r;
}

void Register(std::uintptr_t target_rva, void* replacement, const char* name) {
    HookEntry e{};
    e.target_rva  = target_rva;
    e.replacement = replacement;
    e.name        = name;
    e.installed   = false;
    Registry().push_back(e);
}

static bool PatchJmp(std::uint8_t* target, std::uint8_t* replacement,
                     std::uint8_t saved_out[5]) {
    DWORD old_prot;
    if (!VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &old_prot)) {
        return false;
    }
    std::memcpy(saved_out, target, 5);
    const std::int32_t rel = static_cast<std::int32_t>(
        replacement - target - 5);
    target[0] = 0xE9;
    std::memcpy(target + 1, &rel, 4);
    DWORD tmp;
    VirtualProtect(target, 5, old_prot, &tmp);
    return true;
}

static bool RestoreBytes(std::uint8_t* target, const std::uint8_t saved[5]) {
    DWORD old_prot;
    if (!VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &old_prot)) {
        return false;
    }
    std::memcpy(target, saved, 5);
    DWORD tmp;
    VirtualProtect(target, 5, old_prot, &tmp);
    return true;
}

bool Install(std::size_t index) {
    auto& reg = Registry();
    if (index >= reg.size()) return false;
    HookEntry& h = reg[index];
    if (h.installed) return true;
    auto* target      = reinterpret_cast<std::uint8_t*>(h.target_rva);
    auto* replacement = reinterpret_cast<std::uint8_t*>(h.replacement);
    if (!PatchJmp(target, replacement, h.saved_prologue)) return false;
    h.installed = true;
    return true;
}

bool Uninstall(std::size_t index) {
    auto& reg = Registry();
    if (index >= reg.size()) return false;
    HookEntry& h = reg[index];
    if (!h.installed) return true;
    auto* target = reinterpret_cast<std::uint8_t*>(h.target_rva);
    if (!RestoreBytes(target, h.saved_prologue)) return false;
    h.installed = false;
    return true;
}

void InstallAll() {
    for (std::size_t i = 0; i < Registry().size(); ++i) Install(i);
}

void UninstallAll() {
    for (std::size_t i = 0; i < Registry().size(); ++i) Uninstall(i);
}

std::size_t Count() { return Registry().size(); }

const HookEntry& At(std::size_t index) { return Registry()[index]; }

} // namespace HookSystem
