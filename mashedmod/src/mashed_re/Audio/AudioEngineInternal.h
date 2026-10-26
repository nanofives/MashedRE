// AudioEngine internals shared with Audio/AudioWire.cpp ([D4-WSJ-WIRE]). Not a public API.
#pragma once
#include "RwsBank.h"

namespace mashed_re {
namespace Audio {
namespace detail {

// One row of the MASHED_AUDIO_EVENT_LOG CSV (no-op when the log knob is unset).
void EvLogRow(const char* kind, const char* arg, const char* resolved, double val);
// Exact, case-insensitive wave-name lookup in the loaded permdict SFX bank; nullptr if absent or not loaded.
const RwsWave* SfxBankWaveExact(const char* name);
// Play `w` on a new DirectSound buffer at `freqHz` and linear volume `vol01` (SFX master and focus gain applied).
// Returns the buffer (opaque) or nullptr when there is no device (MASHED_MUTE=1) or creation fails.
void* VoicePlay(const RwsWave& w, float freqHz, float vol01, bool loop);
void  VoiceStop(void* buf);
// Live frequency / volume update; a negative argument leaves that control alone.
void  VoiceSet(void* buf, float freqHz, float vol01);

}  // namespace detail
}  // namespace Audio
}  // namespace mashed_re
