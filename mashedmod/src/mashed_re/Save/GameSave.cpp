// Mashed RE - Save/GameSave reimplementation.
// Four functions in the gamesave cluster. All C2→C3 session c3-batch-e-s1.
#include "../Core/HookSystem.h"

#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// DAT_008a95a0 — save-status global. Written to 0 after both load and write.
// 0x004099e4: MOV [0x008a95a0], EAX
static constexpr std::uintptr_t kSaveStatusGlobal = 0x008a95a0;

// Callee stubs — referenced by address only; not linked into reimpl DLL.
// File-read wrapper  0x004b3b70: __cdecl int FUN_004b3b70(const char*, void*, uint32_t)
// File-write wrapper 0x004b3bb0: __cdecl int FUN_004b3bb0(const char*, void*, uint32_t)
// File-exists check  0x00550b00: __cdecl int FUN_00550b00(const char*)
using FileReadFn_t  = int(__cdecl*)(const char*, void*, std::uint32_t);
using FileWriteFn_t = int(__cdecl*)(const char*, void*, std::uint32_t);
using FileExistsFn_t = int(__cdecl*)(const char*);

// reinterpret_cast from integer is not constexpr in MSVC; use plain statics.
static FileReadFn_t  const gFileRead   = reinterpret_cast<FileReadFn_t >(0x004b3b70u);  // 0x004b3b70
static FileWriteFn_t const gFileWrite  = reinterpret_cast<FileWriteFn_t>(0x004b3bb0u);  // 0x004b3bb0
static FileExistsFn_t const gFileExists = reinterpret_cast<FileExistsFn_t>(0x00550b00u); // 0x00550b00

// 0x005cc8e0 — "gamesave.bin" string address (0x00404e5a / 0x00404f5a / 0x00404f80)
static const char* const kGameSaveFilename = reinterpret_cast<const char*>(0x005cc8e0u);

// 0x00803358 — gamesave buffer address (0x00404e55 / 0x00404f55)
static void* const kGameSaveBuf = reinterpret_cast<void*>(0x00803358u);

// 0x24fa0 — gamesave buffer size 150,432 bytes (0x00404e50 / 0x00404f50)
static constexpr std::uint32_t kGameSaveSize = 0x24fa0u;

// ─────────────────────────────────────────────────────────────────────────────
// 0x004099e0  SaveStatusClear
// Writes param_1 to DAT_008a95a0. Called with 0 after both save and load.
// Disasm: MOV EAX,[ESP+4]; MOV [0x008a95a0],EAX; RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl SaveStatusClear(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(kSaveStatusGlobal) = param_1;
}

RH_ScopedInstall(SaveStatusClear, 0x004099e0);  // re-enabled 2026-05-24 batch-save-b

// ─────────────────────────────────────────────────────────────────────────────
// 0x00404e50  SaveLoad
// SAVE_LOAD_FN. Calls FUN_004b3b70(filename, buf, size) then SaveStatusClear(0).
// Always returns 0 regardless of I/O result.
// Disasm (0x00404e50..0x00404e70):
//   PUSH 0x24fa0     @00404e50
//   PUSH 0x803358    @00404e55
//   PUSH 0x5cc8e0    @00404e5a
//   CALL 004b3b70    @00404e5f
//   PUSH 0           @00404e64
//   CALL 004099e0    @00404e66
//   ADD ESP,0x10     @00404e6b
//   XOR EAX,EAX      @00404e6e
//   RET              @00404e70
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl SaveLoad() {
    gFileRead(kGameSaveFilename, kGameSaveBuf, kGameSaveSize);
    SaveStatusClear(0);
    return 0;
}

RH_ScopedInstall(SaveLoad, 0x00404e50);  // re-enabled 2026-05-24 batch-save-a

// ─────────────────────────────────────────────────────────────────────────────
// 0x00404f50  SaveWrite
// SAVE_WRITE_FN. Mirror of SaveLoad but calls FUN_004b3bb0 (write wrapper).
// Disasm (0x00404f50..0x00404f70):
//   PUSH 0x24fa0     @00404f50
//   PUSH 0x803358    @00404f55
//   PUSH 0x5cc8e0    @00404f5a
//   CALL 004b3bb0    @00404f5f
//   PUSH 0           @00404f64
//   CALL 004099e0    @00404f66
//   ADD ESP,0x10     @00404f6b
//   XOR EAX,EAX      @00404f6e
//   RET              @00404f70
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl SaveWrite() {
    gFileWrite(kGameSaveFilename, kGameSaveBuf, kGameSaveSize);
    SaveStatusClear(0);
    return 0;
}

RH_ScopedInstall(SaveWrite, 0x00404f50);  // re-enabled 2026-05-24 batch-save-a

// ─────────────────────────────────────────────────────────────────────────────
// 0x00404f80  SaveFileExists
// Calls FUN_00550b00(filename); normalizes result to 0 or 1 via NEG/SBB/NEG.
// Disasm (0x00404f80..0x00404f93):
//   PUSH 0x5cc8e0    @00404f80
//   CALL 00550b00    @00404f85
//   ADD ESP,0x4      @00404f8a
//   NEG EAX          @00404f8d
//   SBB EAX,EAX      @00404f8f
//   NEG EAX          @00404f91
//   RET              @00404f93
// NEG/SBB/NEG idiom: any non-zero → 1; zero → 0.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl SaveFileExists() {
    const int raw = gFileExists(kGameSaveFilename);
    // NEG/SBB/NEG: normalize to {0,1}
    return (raw != 0) ? 1 : 0;
}

RH_ScopedInstall(SaveFileExists, 0x00404f80);  // re-enabled 2026-05-24 batch-save-a
