#include "HookSystem.h"

#include <cstring>
#include <cstdlib>
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

// Append "idx rva installed name\n" to the manifest path via raw WriteFile
// (loader-lock-safe; avoids CRT file I/O inside DllMain).
static void ManifestLine(const char* path, std::size_t idx,
                         std::uintptr_t rva, bool installed, const char* name) {
    HANDLE h = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                           OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    char buf[256];
    int n = wsprintfA(buf, "%u\t0x%08X\t%d\t%s\r\n",
                      static_cast<unsigned>(idx), static_cast<unsigned>(rva),
                      installed ? 1 : 0, name ? name : "?");
    DWORD wrote = 0;
    if (n > 0) WriteFile(h, buf, static_cast<DWORD>(n), &wrote, nullptr);
    CloseHandle(h);
}

// Install hooks, honoring optional env-var gates for crash bisection:
//   MASHED_HOOK_LO / MASHED_HOOK_HI  install only registry indices [LO, HI)
//   MASHED_HOOK_SKIP                 comma/space list of names to skip (substring)
//   MASHED_HOOK_MANIFEST=<path>      append an index->rva->name manifest (opt-in)
// With NO env set this is identical to "install every registered hook".
void InstallAll() {
    const std::size_t n = Registry().size();
    std::size_t lo = 0, hi = n;
    if (const char* s = std::getenv("MASHED_HOOK_LO")) lo = std::strtoul(s, nullptr, 0);
    if (const char* s = std::getenv("MASHED_HOOK_HI")) {
        std::size_t v = std::strtoul(s, nullptr, 0);
        if (v < hi) hi = v;
    }
    const char* skip     = std::getenv("MASHED_HOOK_SKIP");
    const char* manifest = std::getenv("MASHED_HOOK_MANIFEST");

    for (std::size_t i = 0; i < n; ++i) {
        const HookEntry& e = Registry()[i];
        bool want = (i >= lo && i < hi);
        if (want && skip && skip[0] && e.name && std::strstr(skip, e.name)) {
            want = false;
        }
        bool ok = want && Install(i);
        if (manifest && manifest[0]) {
            ManifestLine(manifest, i, e.target_rva, ok, e.name);
        }
    }
}

void UninstallAll() {
    for (std::size_t i = 0; i < Registry().size(); ++i) Uninstall(i);
}

std::size_t Count() { return Registry().size(); }

const HookEntry& At(std::size_t index) { return Registry()[index]; }

} // namespace HookSystem
