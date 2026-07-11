// Mashed RE - character voice-bank path builder (WS-J M1 slice).
// Builds the 6 localized "toastaudio\pc\audio\pcdics\<lang>\<NAME>.rws"
// paths for the racer-colour voice banks, all in one call, keyed only by a
// single language code (NOT per-character -- the original loops all 6 names
// internally from the same DAT_006041f0 table every call).
//
// 0x004625b0  FUN_004625b0  (0x18f bytes, 0x004625b0..0x0046273f)
// void __cdecl FUN_004625b0(int langCode)  -- cdecl confirmed: single stack
// arg read at [ESP+0x14] (past 4 pushed regs + retaddr), trailing `RET` with
// no operand (caller-cleans -> cdecl, not stdcall).
//
// Per iteration (EBP = 0, 0x80, ... 0x280; loop while EBP < 0x300, i.e. 6
// entries of stride 0x80 into DAT_00690158):
//   1. FUN_004a2b60(&DAT_00690158 + EBP, 0x5cc4de) -- zero/init the 0x80-byte
//      slot (0x5cc4de not read as data; treated as an init-size/flag arg to
//      FUN_004a2b60, not cited further here -- out of scope for this hook).
//   2. Copies the 28-byte literal "toastaudio\pc\audio\pcdics\" (7 dwords,
//      0x005cc4e0-ish region) into the slot.
//   3. switch(langCode) appends one of 5 literals (with trailing '\'):
//        0/default = "english\"  (0x005ce764, 9 bytes incl. null)
//        1         = "french\"   (0x005ce75c, 8 bytes incl. null)
//        2         = "german\"   (0x005ce748, 8 bytes incl. null)
//        3         = "spanish\"  (0x005ce73c, 9 bytes incl. null)
//        4         = "italian\"  (0x005ce750, 9 bytes incl. null)
//   4. Appends the character name string read from DAT_006041f0 + EBP
//      (confirmed via memory_read on the live master/pool14 slot):
//        EBP=0x000 -> "RED"      EBP=0x080 -> "BLUEJAY"
//        EBP=0x100 -> "MELON"    EBP=0x180 -> "GOLD"
//        EBP=0x200 -> "PINK"     EBP=0x280 -> "SHADOW"
//   5. Appends the literal ".rws" + null (0x005ce734, 5 bytes).
//
// Result per slot: "toastaudio\pc\audio\pcdics\<lang>\<NAME>.rws".
//
// Only caller: FUN_004669b0 (SoundEngine init hub), passing DAT_007f0f60
// (the live language-code global) as langCode.
//
// Reused by the standalone greenfield target as
// mashed_re::Audio::CharacterBankName / CharacterBankPath (per-character,
// on demand); this hook instead matches the original's exact
// build-all-6-at-once, fixed-address-buffer behavior for diff-original A/B.
#include "../Core/HookSystem.h"
#include <cstdio>
#include <cstring>

namespace {

constexpr int kSlotStride = 0x80;
constexpr int kNumBanks = 6;
// Original per-slot buffer, absolute address (written in-process by the .asi
// hook; only meaningful while injected into the live MASHED.exe image).
constexpr uintptr_t kBankPathBuf = 0x00690158u;

const char* const kBankName[kNumBanks] = {
    "RED", "BLUEJAY", "MELON", "GOLD", "PINK", "SHADOW",
};

const char* LangDir(int langCode) {
    switch (langCode) {
        case 1: return "french";
        case 2: return "german";
        case 3: return "spanish";
        case 4: return "italian";
        case 0:
        default: return "english";
    }
}

}  // namespace

// 0x004625b0
extern "C" __declspec(dllexport) void __cdecl AudioCharacterBankPathsBuild(int langCode) {
    char* const base = reinterpret_cast<char*>(kBankPathBuf);
    for (int i = 0; i < kNumBanks; ++i) {
        char* const slot = base + static_cast<ptrdiff_t>(i) * kSlotStride;
        std::snprintf(slot, kSlotStride,
                      "toastaudio\\pc\\audio\\pcdics\\%s\\%s.rws",
                      LangDir(langCode), kBankName[i]);
    }
}

RH_ScopedInstall(AudioCharacterBankPathsBuild, 0x004625b0);  // WS-J M1 slice
