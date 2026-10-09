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

// Loud failure line: raw WriteFile append (same loader-lock-safe pattern as
// ManifestLine below) to a fixed cwd-relative log, plus OutputDebugStringA so
// it also shows under a debugger/Frida.
static void LoudLog(const char* line) {
    OutputDebugStringA(line);
    HANDLE h = CreateFileA("hook_dual_install.log", FILE_APPEND_DATA,
                           FILE_SHARE_READ, nullptr, OPEN_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote = 0;
    WriteFile(h, line, static_cast<DWORD>(std::strlen(line)), &wrote, nullptr);
    CloseHandle(h);
}

// Boot-patch guard (2026-10-09, re/diag/KNOWN_ISSUES.md FULL-AUTOHOOK-BOOT-AV).
// original\MASHED.exe carries ten on-disk boot patches (CLAUDE.md "Runtime state").
// A hook whose 5-byte JMP overlaps a patched byte would silently UNDO that patch:
// VideoGetRender{Width,Height} at the fix_camera_res sites restored
// `mov eax,[0x00616028]` (1680x1050 live) over `mov eax,640`, and the full .asi set
// AV'd at boot (0x004c7785). These ranges are every byte where MASHED.exe differs
// from MASHED.exe.unpatched (17 clusters, 112 bytes, diffed 2026-10-09; incl. the two
// code caves). If the patch set changes, re-diff and update this table.
// MASHED_HOOK_ALLOW_BOOTPATCH=1 disables the guard (deliberate experiments only).
struct PatchedRange { std::uintptr_t va; std::uint32_t len; const char* patch; };
static const PatchedRange kBootPatched[] = {
    {0x0040283f,  5, "skip_movies"},
    {0x004951aa,  1, "skip_controller_dialog"},
    {0x004951ac,  3, "skip_controller_dialog"},
    {0x004951f0,  5, "skip_selector"},
    {0x00495870,  5, "fix_joypad"},
    {0x004963e7,  1, "disable_log"},
    {0x00496400,  1, "disable_log"},
    {0x00496490,  6, "disable_log"},
    {0x00498bc0,  4, "fix_camera_res"},
    {0x00498bd0,  4, "fix_camera_res"},
    {0x00498dbc,  2, "show_windowed"},
    {0x004996d3,  1, "no_focus_pause"},
    {0x004a4541,  5, "fix_fopen"},
    {0x00508bd5, 31, "fix_joypad cave"},
    {0x00508bf5,  7, "fix_joypad cave"},
    {0x0050f6c4, 28, "fix_fopen cave"},
    {0x005bc750,  3, "skip_audio_com"},
};

static const PatchedRange* BootPatchOverlap(std::uintptr_t rva) {
    for (const PatchedRange& r : kBootPatched) {
        if (rva < r.va + r.len && r.va < rva + 5) return &r;
    }
    return nullptr;
}

bool Install(std::size_t index) {
    auto& reg = Registry();
    if (index >= reg.size()) return false;
    HookEntry& h = reg[index];
    if (h.installed) return true;
    auto* target      = reinterpret_cast<std::uint8_t*>(h.target_rva);
    auto* replacement = reinterpret_cast<std::uint8_t*>(h.replacement);
    static const bool allowBootPatch = [] {
        const char* v = std::getenv("MASHED_HOOK_ALLOW_BOOTPATCH");
        return v && v[0] && v[0] != '0';
    }();
    if (!allowBootPatch) {
        if (const PatchedRange* p = BootPatchOverlap(h.target_rva)) {
            char buf[256];
            wsprintfA(buf,
                      "[HookSystem] BOOT-PATCH REFUSED at 0x%08X: JMP would overwrite the "
                      "'%s' patch at 0x%08X; NOT installing '%s'\r\n",
                      static_cast<unsigned>(h.target_rva), p->patch,
                      static_cast<unsigned>(p->va), h.name ? h.name : "?");
            LoudLog(buf);
            return false;
        }
    }
    // Dual-install guard: if the target already starts with an E9 JMP, another
    // hook owns this RVA (two registry entries at one RVA, e.g. a port hook +
    // an A/B driver). Saving that E9 as "original prologue" would corrupt
    // uninstall/trampolines — refuse the second install, loudly. (None of our
    // registered targets legitimately begins with E9; prologues are anchored
    // per hook file.)
    if (target[0] == 0xE9) {
        const char* first = "<not in registry: foreign/unknown E9>";
        for (const HookEntry& other : reg) {
            if (&other != &h && other.installed &&
                other.target_rva == h.target_rva) {
                first = other.name ? other.name : "?";
                break;
            }
        }
        char buf[256];
        wsprintfA(buf,
                  "[HookSystem] DUAL-INSTALL REFUSED at 0x%08X: already hooked "
                  "by '%s'; NOT installing '%s' (would save E9 as prologue)\r\n",
                  static_cast<unsigned>(h.target_rva), first,
                  h.name ? h.name : "?");
        LoudLog(buf);
        return false;
    }
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

// Install hooks, honoring optional env-var gates. With NO env set this is
// identical to "install every registered hook" (unchanged default behavior).
//
// Crash bisection:
//   MASHED_HOOK_LO / MASHED_HOOK_HI  install only registry indices [LO, HI)
//   MASHED_HOOK_SKIP                 names/0xRVAs to skip (exact-token denylist)
//   MASHED_HOOK_MANIFEST=<path>      append an index->rva->name manifest (opt-in)
//
// C3->C4 canonical verification (the important one):
//   MASHED_HOOK_ONLY=<list>          install ONLY hooks whose registered name or
//                                    "0x%08x" RVA token EQUALS a token of this
//                                    comma/space list (allowlist; exact-token
//                                    match — names case-sensitive, RVA tokens
//                                    case-insensitive; substring matching removed
//                                    2026-07-03 after "AiPreTick" inside
//                                    "AbAiPreTick" co-installed two hooks at
//                                    0x004177b0). This is how a C4 batch installs
//                                    just its candidate set live (inline-JMP)
//                                    while every other function stays original ->
//                                    a clean per-hook "modded vs original"
//                                    canonical scenario that sidesteps the
//                                    full-install multi-bug field.
// Precedence: ONLY (if set) wins; otherwise LO/HI range; SKIP applies on top.

// Exact-token list match for MASHED_HOOK_ONLY / MASHED_HOOK_SKIP. `list` is
// split on commas/spaces/tabs; a hook matches iff some token EQUALS its
// registered name (strcmp, case-sensitive) or its "0x%08x" RVA token
// (case-insensitive, so 0X.../uppercase hex CLI input still selects).
// Sentinels like "__none__" / "ZZ_NO_HOOK_ZZ" equal no token -> match nothing.
static bool TokenMatch(const char* list, const char* name, const char* rvatok) {
    const std::size_t name_len  = name ? std::strlen(name) : 0;
    const std::size_t rvatok_len = std::strlen(rvatok);
    for (const char* p = list; *p;) {
        while (*p == ',' || *p == ' ' || *p == '\t') ++p;
        const char* tok = p;
        while (*p && *p != ',' && *p != ' ' && *p != '\t') ++p;
        const std::size_t len = static_cast<std::size_t>(p - tok);
        if (!len) continue;
        if (name_len == len && std::memcmp(tok, name, len) == 0) return true;
        if (rvatok_len == len && _strnicmp(tok, rvatok, len) == 0) return true;
    }
    return false;
}

void InstallAll() {
    const std::size_t n = Registry().size();
    std::size_t lo = 0, hi = n;
    if (const char* s = std::getenv("MASHED_HOOK_LO")) lo = std::strtoul(s, nullptr, 0);
    if (const char* s = std::getenv("MASHED_HOOK_HI")) {
        std::size_t v = std::strtoul(s, nullptr, 0);
        if (v < hi) hi = v;
    }
    const char* only     = std::getenv("MASHED_HOOK_ONLY");
    const char* skip     = std::getenv("MASHED_HOOK_SKIP");
    const char* manifest = std::getenv("MASHED_HOOK_MANIFEST");

    for (std::size_t i = 0; i < n; ++i) {
        const HookEntry& e = Registry()[i];
        char rvatok[16];
        wsprintfA(rvatok, "0x%08x", static_cast<unsigned>(e.target_rva));
        bool want;
        if (only && only[0]) {
            // allowlist: exact name token or exact "0x%08x" RVA token
            want = TokenMatch(only, e.name, rvatok);
        } else {
            want = (i >= lo && i < hi);
            // Lane 2 (re/tools/decomp2port.py): generated, UNVERIFIED ports register under an
            // L2_ prefix and are opt-in -- installed only through MASHED_HOOK_ONLY (above) or
            // MASHED_HOOK_LANE2=1. A default .asi run must never carry a port nobody has
            // A/B'd; the shadow batch names its candidates explicitly, so it is unaffected.
            static const bool lane2 = [] {
                const char* v = std::getenv("MASHED_HOOK_LANE2");
                return v && v[0] && v[0] != '0';
            }();
            if (want && !lane2 && e.name && std::strncmp(e.name, "L2_", 3) == 0) want = false;
        }
        if (want && skip && skip[0] && TokenMatch(skip, e.name, rvatok)) {
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
