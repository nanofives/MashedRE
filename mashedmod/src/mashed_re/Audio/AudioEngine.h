// AudioEngine — minimal real DirectSound playback for the standalone race
// (2026-06-15). The original Mashed uses DirectSound (see re/analysis/
// audio_dsound/); this is a small faithful backend: a secondary buffer per
// voice, filled with PCM decoded from an RWS bank (Audio/RwsBank), played
// looping or one-shot. Replaces the IRaceAudio stub.
//
// Scope today: track AMBIENCE (the per-track <area>.rws environmental bed —
// wind/sea/rain) as one or more looping voices. Engine-RPM voices + the full
// RWS sound-pool dispatch (FUN_005a7b60 bank loader) are the next iteration.
#pragma once

namespace mashed_re {
namespace Audio {

// Create the DirectSound device against `hwnd` (HWND as void* to keep <dsound.h>
// out of the header). Safe to call once at startup; returns false if no audio
// device / DirectSound is unavailable (playback then becomes a no-op).
bool Init(void* hwnd);
void Shutdown();
bool Available();

// Decode `waveName` (case-insensitive substring) from the RWS bank at `rwsPath`
// and play it. Looping voices persist until Stop/StopAll; one-shots free
// themselves. `volume` is 0..1 (linear, mapped to DirectSound's dB scale).
// Returns a voice id (>=0) or -1 on failure.
int  PlayLoop(const char* rwsPath, const char* waveName, float volume);
int  PlayOnce(const char* rwsPath, const char* waveName, float volume);

void SetVolume(int voice, float volume);     // 0..1
void Stop(int voice);
void StopAll();

// Engine voice — a procedural looping buzz pitched by RPM. [SCAFFOLD] the real
// per-car engine samples live in the 0x80d streamed RWS banks (toastaudio
// english/<car>.rws), a format not yet cracked; this synth stand-in gives the
// race an engine note that rises with speed until those are decoded.
void EngineStart(float volume);              // start (idempotent)
void EngineSetRpm(float norm01);             // 0..1 -> playback pitch
void EngineStop();

// Verification aid: true if `voice`'s buffer is actually in the PLAYING state
// (DSBSTATUS_PLAYING) — used to prove audio is live in a headless run.
bool IsPlaying(int voice);
// Play cursor (bytes) for `voice`, or 0xFFFFFFFF if invalid — advancing across
// two reads proves the buffer is really being rendered.
unsigned PlayCursor(int voice);

}  // namespace Audio
}  // namespace mashed_re
