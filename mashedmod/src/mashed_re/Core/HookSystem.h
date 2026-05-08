// Mashed RE — gta-reversed-style inline-JMP hook installer.
// Patches a 5-byte E9 rel32 at the original RVA so the OS executes our reimpl.
// Used only inside mashed_re_dev.asi during development.
#pragma once

#include <windows.h>
#include <cstdint>
#include <cstddef>

namespace HookSystem {

struct HookEntry {
    std::uintptr_t target_rva;
    void*          replacement;
    const char*    name;
    std::uint8_t   saved_prologue[5];
    bool           installed;
};

// Add a hook to the registry. Called by RH_ScopedInstall during DLL load.
// Patching itself is deferred to InstallAll() so DllMain controls timing.
void Register(std::uintptr_t target_rva, void* replacement, const char* name);

bool Install(std::size_t index);
bool Uninstall(std::size_t index);
void InstallAll();
void UninstallAll();

std::size_t       Count();
const HookEntry&  At(std::size_t index);

} // namespace HookSystem

// Place at file-scope after the replacement function definition:
//   RH_ScopedInstall(MyFunc, 0x00xxxxxx);
// Registers a static ctor that adds the entry to the registry on DLL load.
#define RH_ScopedInstall(Method, RVA)                                       \
    static struct AutoRegister_##Method {                                   \
        AutoRegister_##Method() {                                           \
            ::HookSystem::Register(                                         \
                (RVA), reinterpret_cast<void*>(&Method), #Method);          \
        }                                                                   \
    } s_AutoReg_##Method
