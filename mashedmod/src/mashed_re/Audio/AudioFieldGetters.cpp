// Mashed RE — Audio/AudioFieldGetters.cpp
// Four pure field getters split out of Audio/TimerGetters_ah5.cpp (audiolink slice 5) so they link into the exe. In both
// rsp files. No address, no callee. Bytes (verify/audiolink_20261023/disasm_slice5.txt): each is
// mov eax,[esp+4] / mov eax,[eax+off] / ret, off = 0xd6c / 0xd70 / 0xd74 / 0x10.
#include "../Core/HookSystem.h"
#include <cstdint>

// Voice field-getter triplet. __cdecl, single struct/this pointer on the stack
// (disasm: MOV EAX,[ESP+4]; MOV EAX,[EAX+off]; RET), NOT an ECX thiscall.
// Verbatim leaf: return *(uint32_t*)((char*)this + off).
// (harness/scenario-attach 2026-06-08; verified pool14.)

// 0x005a89a0  FUN_005a89a0  (11 bytes)  undefined4(this)  -> *(this+0xD6C)
extern "C" __declspec(dllexport) uint32_t __cdecl VoiceFieldD6cGet(void* self) {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(self) + 0xD6C);
}
RH_ScopedInstall(VoiceFieldD6cGet, 0x005a89a0);

// 0x005a89b0  FUN_005a89b0  (11 bytes)  undefined4(this)  -> *(this+0xD70)
extern "C" __declspec(dllexport) uint32_t __cdecl VoiceFieldD70Get(void* self) {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(self) + 0xD70);
}
RH_ScopedInstall(VoiceFieldD70Get, 0x005a89b0);

// 0x005a89c0  FUN_005a89c0  (11 bytes)  undefined4(this)  -> *(this+0xD74)
extern "C" __declspec(dllexport) uint32_t __cdecl VoiceFieldD74Get(void* self) {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(self) + 0xD74);
}
RH_ScopedInstall(VoiceFieldD74Get, 0x005a89c0);

// 0x005ab370  FUN_005ab370  (7 bytes)  undefined4(this)  -> *(this+0x10)
// Same __cdecl single-stack-ptr field-getter shape (MOV EAX,[ESP+4];
// MOV EAX,[EAX+0x10]; RET, pool14). Audio channel-meta accessor.
extern "C" __declspec(dllexport) uint32_t __cdecl ChannelField10Get(void* self) {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(self) + 0x10);
}
RH_ScopedInstall(ChannelField10Get, 0x005ab370);
