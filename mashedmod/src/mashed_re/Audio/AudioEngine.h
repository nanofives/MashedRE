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

// Master category volumes (0..1) for the Sound options screen. All playback
// scales by the category master; setting one re-applies live to the persistent
// music / engine voice. Music slider -> SetMasterMusicVolume, SFX slider ->
// SetMasterSfxVolume. Default 1.0. (WS-G4)
void  SetMasterMusicVolume(float v);
void  SetMasterSfxVolume(float v);
float MasterMusicVolume();
float MasterSfxVolume();

// Window focus / minimize mute (driven from WinMain's WM_ACTIVATE). active=false
// silences ALL output (music/engine/ambience/SFX); active=true restores the live
// looping voices. Independent of the Music/SFX category sliders (those persist).
void  SetOutputActive(bool active);

// Engine voice — loops the REAL engine sample `eng1` from the permdict SFX bank
// (0x809; loaded once via SfxLoadBank), pitched by RPM. permdict carries 4
// engine classes (eng1..eng4 + `d` variants); which class each of the 12
// vehicles uses is vehicle stat-data not yet RE'd — eng1 is used for all cars
// for now (see re/analysis/audio_character_banks_REmap_20260616.md §4). Falls
// back to a procedural buzz only if permdict isn't loaded. Pitched by
// EngineSetRpm.
void EngineStart(float volume);
void EngineSetRpm(float norm01);             // 0..1 -> playback pitch
void EngineStop();

// Character voice banks (driver taunts/commentary). The 6 localized
// english/<name>.rws banks are per-CHARACTER (NOT engine, NOT per-vehicle):
// the bank follows the racer's Player Colour index 0..5. Cited map (DAT_006041f0
// / FUN_004625b0; see the REmap note §2):
//   0=RED 1=BLUEJAY 2=MELON 3=GOLD 4=PINK 5=SHADOW
// CharacterBankName returns the base name (nullptr if out of range);
// CharacterBankPath builds the localized file path
//   original/toastaudio/pc/audio/pcdics/<lang>/<name>.rws
// langCode follows FUN_004625b0 (0=english 1=french 2=german 3=spanish
// 4=italian; default english). Returns false if charIndex is out of range.
const char* CharacterBankName(int charIndex);
bool        CharacterBankPath(int charIndex, int langCode, char* buf, int cap);

// Music — one dedicated music voice, switched on game-state edges. The state
// setter is idempotent (re-setting the current state is a no-op; it never
// restarts the voice per-frame). Menu=permdict `musicloop1`, Race=cdaudio.rws
// (streamed IMA), Results=ducked race music, Silent=stopped.
enum class MusicState { Silent, Menu, Race, Results };
void MusicSetState(MusicState s);

// Lower-level music controls (MusicSetState uses these). MusicStart decodes a
// 0x80d streamed RWS (e.g. cdaudio.rws) and loops it; `maxSeconds` caps the
// decode (0 = whole track).
void MusicStart(const char* rwsPath, float volume, int maxSeconds);
void MusicStop();

// SFX bank — preload a 0x809 wave bank (e.g. permdict.rws: "menu navigation",
// "menu selection", "threetwoone", "go", "rocket", "drop mine", "explosion1"...)
// once, then fire named one-shots from the cache (8-voice ring). Cheap to call
// per UI event / race event.
void SfxLoadBank(const char* rwsPath);
void SfxPlay(const char* waveNameSub, float volume);

// Verification aid: true if `voice`'s buffer is actually in the PLAYING state
// (DSBSTATUS_PLAYING) — used to prove audio is live in a headless run.
bool IsPlaying(int voice);
// Play cursor (bytes) for `voice`, or 0xFFFFFFFF if invalid — advancing across
// two reads proves the buffer is really being rendered.
unsigned PlayCursor(int voice);

}  // namespace Audio
}  // namespace mashed_re
