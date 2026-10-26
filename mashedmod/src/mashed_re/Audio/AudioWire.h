// AudioWire — [D4-WSJ-WIRE] the race SFX of the original wired into the AudioEngine stand-in by data,
// behind ONE default-OFF knob MASHED_AUDIO_WIRE=1 (PREREG verify/wsjwire_20261026/PREREG_D4-WSJ-WIRE.md).
// With the knob unset every entry point below returns at once and no call site changes behaviour.
//
// Sources (L-24 = (a), Mariano 2026-10-26):
//   wave table + voice counts  verify/wsj80e_20261026/wave_tables.tsv (permdict 0x006044f0, stride 0x94, +0x84 count)
//   trigger sites              verify/wsjsfxmap_20261026/sfx_caller_table.csv
//   engine class               Audio/EngineClassTable.h (verify/wsjeng_20261026/)
//   no race cdaudio            verify/wsjmusic_20261026/RESULT_D4-WSJ-MUSIC.md
#pragma once

namespace mashed_re {
namespace Audio {

bool WireOn();   // MASHED_AUDIO_WIRE=1, read once

// Race lifecycle. vehicleByCar[c] = the ORIGINAL vehicle index 0..12 (EngineClassTable.h order), -1 = unknown.
void WireRaceBegin(int numCars, const int* vehicleByCar);
void WireRaceTick(float playerNorm01);   // once per race frame: event-ring consumer (FUN_004648b0) + engine tick (FUN_00463640)
void WireRaceEnd();

// The port's DFF base name ("ADVANTAGE") -> original vehicle index 0..12, -1 if not in the table.
int  WireVehicleIndexByName(const char* dffBase);
// Engine class 0..3 (eng1..eng4) for an original vehicle index; FUN_00463640's default arm (0) when out of range.
int  WireEngineClass(int vehicleIdx);

// Powerup FX events (PowerupEffects.cpp sites; the original's FX calls in the SFXMAP table).
enum PowerupSfx {
    kPuSfxGunFire = 0,   // FUN_004561c0 -> FUN_00465ca0(0x13 machineg)            0x004566b8
    kPuSfxDrumFire,      // FUN_00454740 -> FUN_00465ca0(0x12 rocket)              0x0045477b
    kPuSfxMissileFire,   // FUN_00455150 -> FUN_00465e80(0x12) + FUN_00465e80(0x14) 0x0045532f / 0x0045533b
    kPuSfxPMineFire,     // FUN_00457ef0 -> FUN_00465ca0(0x1b drop mine)          0x00457f1c
    kPuSfxShotgunFire,   // FUN_0045b390 -> FUN_00465ca0(0x15 shotgun)            0x0045b3aa
    kPuSfxFlashFire,     // FUN_00454db0 -> FUN_00465ca0(0x18 flash)              0x00454df0
    kPuSfxMortarFire,    // FUN_004533b0 -> FUN_00465ca0(0x12) + FUN_00465ca0(0x1e) 0x004535ed / 0x004535f5
    kPuSfxOilFire,       // FUN_00457800 -> FUN_00465e80(0x17 oilslick, owner)    0x00457a06
    kPuSfxFlameArm,      // FUN_0045a7c0 -> FUN_00465e80(0x16 flame thrower, owner) 0x0045a83c
    kPuSfxFlameDeact,    // FUN_0045a8b0 -> FUN_004661a0(0x16, owner)             0x0045a8fc
};
void WirePowerup(int ev, int owner);

void WirePickup();                                // FUN_00458920 -> FUN_00465ca0(0x1c) 0x00458a26
void WireRingPush(int type, int car, int other);  // audio-side copy of the 0x007e9de0 ring (types 3 and 4)
// Race SFX the stand-in already fires by name (countdown, spin-out explosion). Returns true when the knob
// handled it (the caller then skips the legacy one-shot ring).
bool WireSfxByName(const char* name);
// Re-apply live wire voice volumes after a focus / master-volume change (AudioEngine::SetOutputActive).
void WireReapplyVolumes();
// MASHED_AUDIO_WIRE_SELFTEST=1: boot-time checks of the voice model, guard and ring on the loaded permdict.
void WireSelfTest();

}  // namespace Audio
}  // namespace mashed_re
