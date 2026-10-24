#pragma once
// Mashed RE - shared core of the 0x004625b0 character voice-bank path builder.
// In both rsp files (verify/bankpath_20261024/PREREG_bankpath.md). The .asi
// export AudioCharacterBankPathsBuild loops the six DAT_00690158 slots over it.

namespace mashed_re {
namespace Audio {

// One slot of FUN_004625b0: "toastaudio\pc\audio\pcdics\<lang>\<NAME>.rws",
// NAME uppercase from the DAT_006041f0 order (0=RED 1=BLUEJAY 2=MELON 3=GOLD
// 4=PINK 5=SHADOW), lang from the 0x004625b0 switch (1=french 2=german
// 3=spanish 4=italian, else english). No root prefix. One snprintf(buf, cap).
// Returns false if charIndex is outside 0..5, buf is null or cap <= 0.
bool CharacterBankPathCore(int charIndex, int langCode, char* buf, int cap);

// MASHED_BANKPATH_CORE=1 (default OFF): CharacterBankPath becomes the thin
// wrapper over the core, and its caller adds the original/ root prefix.
bool BankPathCoreOn();

}  // namespace Audio
}  // namespace mashed_re
