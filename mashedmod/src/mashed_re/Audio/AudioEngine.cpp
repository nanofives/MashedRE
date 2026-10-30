// AudioEngine impl — DirectSound 8 secondary-buffer playback. See AudioEngine.h.
#include "AudioEngine.h"
#include "RwsBank.h"
#include "AudioCharacterBankPaths.h"
#include "AudioWire.h"
#include "AudioEngineInternal.h"
#include "../DllGuard.h"

#include <windows.h>
#include <dsound.h>
#include <mmreg.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>

// The original's linear-to-log leaf, Audio/AudioLinearToLog.cpp (both targets). VolToDs's only body.
extern "C" uint32_t __cdecl AudioLinearToLog(float param_1);

namespace mashed_re {
namespace Audio {

namespace {
const char* kLog = "log/mashed_re.log";
void logf(const char* fmt, ...) {
    if (std::FILE* f = std::fopen(kLog, "a")) {
        va_list ap; va_start(ap, fmt);
        std::fprintf(f, "[audio] "); std::vfprintf(f, fmt, ap); std::fprintf(f, "\n");
        va_end(ap); std::fclose(f);
    }
}

// [D4-WSJ-REF] MASHED_AUDIO_EVENT_LOG=<path>, default OFF: one CSV row per audio request at the API entry,
// written before the device check so a MASHED_MUTE=1 run still logs. Logging only, nothing played changes.
// Columns: ms (QPC since the first row), kind, arg (wave / bank), resolved (the bank wave name the
// request resolves to, empty when none), val. verify/wsjref_20261026/RESULT_D4-WSJ-REF.md.
std::FILE*    s_evLog = nullptr;
bool          s_evLogInit = false;
LARGE_INTEGER s_evQ0 = {}, s_evQf = {};
void EvLog(const char* kind, const char* arg, const char* resolved, double val) {
    if (!s_evLogInit) {
        s_evLogInit = true;
        char p[MAX_PATH];
        const DWORD n = GetEnvironmentVariableA("MASHED_AUDIO_EVENT_LOG", p, sizeof(p));
        if (n > 0 && n < sizeof(p)) {
            s_evLog = std::fopen(p, "w");
            QueryPerformanceFrequency(&s_evQf); QueryPerformanceCounter(&s_evQ0);
            if (s_evLog) std::fputs("ms,kind,arg,resolved,val\n", s_evLog);
        }
    }
    if (!s_evLog) return;
    LARGE_INTEGER q; QueryPerformanceCounter(&q);
    std::fprintf(s_evLog, "%.1f,%s,%s,%s,%.3f\n",
                 static_cast<double>(q.QuadPart - s_evQ0.QuadPart) * 1000.0 / static_cast<double>(s_evQf.QuadPart),
                 kind, arg ? arg : "", resolved ? resolved : "", val);
    std::fflush(s_evLog);
}

IDirectSound8* g_ds = nullptr;
IDirectSoundBuffer* g_engine = nullptr;   // procedural engine voice
DWORD               g_engineBaseHz = 22050;
IDirectSoundBuffer* g_music = nullptr;    // the single music voice
MusicState          g_musicState = MusicState::Silent;
std::vector<RwsWave> g_sfxBank;           // cached 0x809 SFX bank (permdict)
IDirectSoundBuffer*  g_sfxRing[8] = {};   // one-shot SFX voices (round-robin)
int                  g_sfxNext = 0;

// [WS-G4] master category volumes (0..1), driven by the Sound options screen
// (Music / SFX Volume sliders). All playback scales its per-voice volume by the
// category master so the sliders actually attenuate audio. Base (pre-master)
// volumes of the persistent music/engine voices are tracked so a slider change
// re-applies live. Default 1.0 (full) until the loaded settings push real values.
float g_masterMusic   = 1.0f;
float g_masterSfx     = 1.0f;
float g_musicBaseVol  = 0.0f;
float g_engineBaseVol = 0.0f;
// Global OUTPUT gain for window focus/minimize muting (SetOutputActive). Folded
// into VolToDs so EVERY voice — music, engine, ambience, one-shots created while
// unfocused — is silenced; SEPARATE from the user's Music/SFX category sliders so
// muting on focus loss never clobbers their saved volume settings. 1=audible, 0=muted.
float g_focusGain     = 1.0f;
inline float Clamp01f(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }

struct Voice {
    IDirectSoundBuffer* buf = nullptr;
    bool                loop = false;
    bool                used = false;
    float               vol  = 0.0f;   // intended (linear x category-master) for focus re-apply
};
std::vector<Voice> g_voices;

// 0..1 linear -> DirectSound hundredths-of-dB through the original's AudioLinearToLog 0x005aeb90
// (Audio/AudioLinearToLog.cpp): floor -10000 below 1e-5f [0x005cc990], f32 2000*log10 truncated, positives clamped to 0.
// Scaled by g_focusGain (port-side) so a focus-loss mute applies to every SetVolume.
// [D4-AUDIO-VOLTODS-FLIP 2026-10-24] L-6 = A (log/orch/DECISIONS.md): the original's law is the only body. The port's
// log10 body, MASHED_VOLTODS_ORIG and MASHED_VOLTODS_LOG are deleted. verify/audiovoltods_20261024/RESULT_voltods.md S2.
LONG VolToDs(float v) {
    v *= g_focusGain;
    return static_cast<LONG>(static_cast<int32_t>(AudioLinearToLog(v)));
}

int CreateAndPlay(const char* rwsPath, const char* waveName, float volume, bool loop) {
    if (!g_ds) return -1;
    std::vector<RwsWave> waves;
    if (!RwsBankLoad(rwsPath, waves, kLog)) { logf("PlayLoop: bank load failed %s", rwsPath); return -1; }
    const RwsWave* w = RwsBankFind(waves, waveName);
    if (!w || !w->valid()) { logf("PlayLoop: wave '%s' not found in %s", waveName, rwsPath); return -1; }

    WAVEFORMATEX wf = {};
    wf.wFormatTag      = WAVE_FORMAT_PCM;
    wf.nChannels       = static_cast<WORD>(w->channels);
    wf.nSamplesPerSec  = w->rate;
    wf.wBitsPerSample  = static_cast<WORD>(w->bits);
    wf.nBlockAlign     = static_cast<WORD>(w->channels * w->bits / 8);
    wf.nAvgBytesPerSec = w->rate * wf.nBlockAlign;

    DSBUFFERDESC desc = {};
    desc.dwSize        = sizeof(desc);
    desc.dwFlags       = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
                         DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;
    desc.dwBufferBytes = static_cast<DWORD>(w->pcm.size());
    desc.lpwfxFormat   = &wf;

    IDirectSoundBuffer* buf = nullptr;
    if (FAILED(g_ds->CreateSoundBuffer(&desc, &buf, nullptr)) || !buf) {
        logf("PlayLoop: CreateSoundBuffer failed (%u bytes)", desc.dwBufferBytes);
        return -1;
    }
    void* p1 = nullptr; void* p2 = nullptr; DWORD s1 = 0, s2 = 0;
    if (FAILED(buf->Lock(0, desc.dwBufferBytes, &p1, &s1, &p2, &s2, 0))) {
        buf->Release(); logf("PlayLoop: Lock failed"); return -1;
    }
    std::memcpy(p1, w->pcm.data(), s1);
    if (p2 && s2) std::memcpy(p2, w->pcm.data() + s1, s2);
    buf->Unlock(p1, s1, p2, s2);
    buf->SetVolume(VolToDs(volume * g_masterSfx));   // ambience = SFX category
    if (FAILED(buf->Play(0, 0, loop ? DSBPLAY_LOOPING : 0))) {
        buf->Release(); logf("PlayLoop: Play failed"); return -1;
    }

    int id = -1;
    for (size_t i = 0; i < g_voices.size(); ++i)
        if (!g_voices[i].used) { id = static_cast<int>(i); break; }
    if (id < 0) { id = static_cast<int>(g_voices.size()); g_voices.push_back(Voice{}); }
    g_voices[id].buf = buf; g_voices[id].loop = loop; g_voices[id].used = true;
    g_voices[id].vol = volume * g_masterSfx;   // intended level for focus re-apply

    DWORD st = 0; buf->GetStatus(&st);
    logf("PlayLoop voice=%d '%s' rate=%u ch=%d bytes=%u loop=%d status=%s",
         id, w->name.c_str(), w->rate, w->channels, desc.dwBufferBytes, (int)loop,
         (st & DSBSTATUS_PLAYING) ? "PLAYING" : "stopped");
    return id;
}
}  // namespace

// --- [D4-WSJ-WIRE] internals for Audio/AudioWire.cpp (AudioEngineInternal.h) -------------
namespace detail {
void EvLogRow(const char* kind, const char* arg, const char* resolved, double val) { EvLog(kind, arg, resolved, val); }
const RwsWave* SfxBankWaveExact(const char* name) {
    if (!name) return nullptr;
    for (const RwsWave& w : g_sfxBank) if (_stricmp(w.name.c_str(), name) == 0) return &w;
    return nullptr;
}
static DWORD ClampHz(float hz) {
    if (hz < static_cast<float>(DSBFREQUENCY_MIN)) return DSBFREQUENCY_MIN;
    if (hz > static_cast<float>(DSBFREQUENCY_MAX)) return DSBFREQUENCY_MAX;
    return static_cast<DWORD>(hz);
}
void* VoicePlay(const RwsWave& w, float freqHz, float vol01, bool loop) {
    if (!g_ds || !w.valid()) return nullptr;
    WAVEFORMATEX wf = {};
    wf.wFormatTag = WAVE_FORMAT_PCM; wf.nChannels = static_cast<WORD>(w.channels);
    wf.nSamplesPerSec = w.rate; wf.wBitsPerSample = static_cast<WORD>(w.bits);
    wf.nBlockAlign = static_cast<WORD>(w.channels * w.bits / 8);
    wf.nAvgBytesPerSec = w.rate * wf.nBlockAlign;
    DSBUFFERDESC d = {};
    d.dwSize = sizeof(d);
    d.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS;
    d.dwBufferBytes = static_cast<DWORD>(w.pcm.size());
    d.lpwfxFormat = &wf;
    IDirectSoundBuffer* buf = nullptr;
    if (FAILED(g_ds->CreateSoundBuffer(&d, &buf, nullptr)) || !buf) return nullptr;
    void* p1 = nullptr; DWORD s1 = 0;
    if (SUCCEEDED(buf->Lock(0, d.dwBufferBytes, &p1, &s1, nullptr, nullptr, 0))) {
        std::memcpy(p1, w.pcm.data(), s1);
        buf->Unlock(p1, s1, nullptr, 0);
    }
    buf->SetFrequency(ClampHz(freqHz));
    buf->SetVolume(VolToDs(vol01 * g_masterSfx));
    buf->Play(0, 0, loop ? DSBPLAY_LOOPING : 0);
    return buf;
}
void VoiceStop(void* b) {
    if (IDirectSoundBuffer* buf = static_cast<IDirectSoundBuffer*>(b)) { buf->Stop(); buf->Release(); }
}
void VoiceSet(void* b, float freqHz, float vol01) {
    IDirectSoundBuffer* buf = static_cast<IDirectSoundBuffer*>(b);
    if (!buf) return;
    if (freqHz >= 0.0f) buf->SetFrequency(ClampHz(freqHz));
    if (vol01 >= 0.0f)  buf->SetVolume(VolToDs(vol01 * g_masterSfx));
}
}  // namespace detail

bool Init(void* hwnd) {
    if (g_ds) return true;
    // MASHED_MUTE=1 -> skip DirectSound init entirely. g_ds stays null and every
    // playback fn (CreateAndPlay/EngineStart/MusicStart) already early-returns on
    // !g_ds, so the whole audio system silently no-ops. For headless/screenshot
    // runs so the game doesn't blast sound.
    {
        char muteBuf[8] = {};
        if (GetEnvironmentVariableA("MASHED_MUTE", muteBuf, sizeof(muteBuf)) > 0 && muteBuf[0] == '1') {
            logf("Init: MASHED_MUTE=1 — audio disabled (no DirectSound)");
            return false;
        }
    }
    // D5-SHIM-GUARD-DSOUND: no static DSOUND.dll import (a dsound.dll beside the
    // exe was bound by name at process load). Resolve DirectSoundCreate8 from the
    // System32 copy only. On failure audio stays off: no fallback to the default
    // search.
    using DirectSoundCreate8Fn = HRESULT(WINAPI*)(LPCGUID, LPDIRECTSOUND8*, LPUNKNOWN);
    HMODULE dsmod = mashed_re::DllGuardLoadSystem32("dsound.dll");
    auto dsCreate8 = dsmod ? reinterpret_cast<DirectSoundCreate8Fn>(GetProcAddress(dsmod, "DirectSoundCreate8"))
                           : nullptr;
    if (!dsCreate8) { logf("Init: dsound.dll System32 load failed - audio disabled"); return false; }
    HRESULT hr = dsCreate8(nullptr, &g_ds, nullptr);
    if (FAILED(hr) || !g_ds) { g_ds = nullptr; logf("Init: DirectSoundCreate8 failed hr=%#lx", hr); return false; }
    // PRIORITY lets us set the primary format; the window may be null in odd
    // headless cases — fall back to the desktop window.
    HWND h = static_cast<HWND>(hwnd);
    if (!h) h = GetDesktopWindow();
    g_ds->SetCooperativeLevel(h, DSSCL_PRIORITY);
    logf("Init: DirectSound ready");
    return true;
}

void Shutdown() {
    EngineStop();
    MusicStop();
    for (auto& b : g_sfxRing) if (b) { b->Stop(); b->Release(); b = nullptr; }
    g_sfxBank.clear();
    StopAll();
    for (auto& v : g_voices) if (v.buf) { v.buf->Release(); v.buf = nullptr; }
    g_voices.clear();
    if (g_ds) { g_ds->Release(); g_ds = nullptr; }
}

bool Available() { return g_ds != nullptr; }

int PlayLoop(const char* rwsPath, const char* waveName, float volume) {
    EvLog("loop", waveName, rwsPath, volume);
    return CreateAndPlay(rwsPath, waveName, volume, true);
}
int PlayOnce(const char* rwsPath, const char* waveName, float volume) {
    EvLog("once", waveName, rwsPath, volume);
    return CreateAndPlay(rwsPath, waveName, volume, false);
}

void SetVolume(int voice, float volume) {
    if (voice >= 0 && voice < (int)g_voices.size() && g_voices[voice].used && g_voices[voice].buf) {
        g_voices[voice].vol = volume;                 // track intended for focus re-apply
        g_voices[voice].buf->SetVolume(VolToDs(volume));
    }
}

void Stop(int voice) {
    if (voice >= 0 && voice < (int)g_voices.size() && g_voices[voice].used) {
        if (g_voices[voice].buf) { g_voices[voice].buf->Stop(); g_voices[voice].buf->Release(); }
        g_voices[voice] = Voice{};
    }
}

void StopAll() {
    EvLog("stop_all", "", "", 0.0);
    for (size_t i = 0; i < g_voices.size(); ++i)
        if (g_voices[i].used && g_voices[i].buf) {
            g_voices[i].buf->Stop(); g_voices[i].buf->Release(); g_voices[i] = Voice{};
        }
}

void EngineStart(float volume, int vehicleIdx) {
    // [D4-WSJ-WIRE] knob ON: the per-car engine voices of Audio/AudioWire.cpp (FUN_00463640) are the engine, with
    // the class of `vehicleIdx` (EngineClassTable.h); the single legacy voice below is not created.
    if (WireOn()) {
        static const char* const kEng[4] = {"eng1", "eng2", "eng3", "eng4"};
        EvLog("engine_start", kEng[WireEngineClass(vehicleIdx)], "wire", volume);
        return;
    }
    EvLog("engine_start", "eng1", "", volume);
    if (!g_ds || g_engine) return;
    DWORD rate = 22050;
    std::vector<std::int16_t> pcm;
    // Prefer the REAL engine loop: `eng1` from the permdict SFX bank (0x809,
    // 16-bit mono @22050; already loaded by SfxLoadBank at boot). permdict has
    // eng1..eng4 engine classes; per-vehicle class assignment is unresolved
    // (REmap note §4) so eng1 is used for all cars.
    if (!g_sfxBank.empty()) {
        const RwsWave* w = RwsBankFind(g_sfxBank, "eng1");
        if (w && w->valid() && w->pcm.size() > 1000) {
            rate = w->rate ? w->rate : 22050;
            pcm.resize(w->pcm.size() / 2);
            std::memcpy(pcm.data(), w->pcm.data(), pcm.size() * 2);
            logf("EngineStart: real engine 'eng1' from permdict (%u samples @%u)",
                 (unsigned)pcm.size(), rate);
        }
    }
    if (pcm.empty()) {
        // procedural fallback: 1 s seamless low buzz (sawtooth + octave grit).
        rate = 22050;
        const int N = static_cast<int>(rate);
        pcm.resize(static_cast<size_t>(N));
        const double baseCyc = 55.0;
        for (int i = 0; i < N; ++i) {
            const double ph = std::fmod(static_cast<double>(i) * baseCyc / N, 1.0);
            double s = (2.0 * ph - 1.0) * 0.6;
            s += 0.25 * (2.0 * std::fmod(ph * 2.0, 1.0) - 1.0);
            if (s > 1.0) s = 1.0; if (s < -1.0) s = -1.0;
            pcm[static_cast<size_t>(i)] = static_cast<std::int16_t>(s * 9000.0);
        }
        logf("EngineStart: procedural engine voice");
    }
    g_engineBaseHz = rate;
    WAVEFORMATEX wf = {};
    wf.wFormatTag = WAVE_FORMAT_PCM; wf.nChannels = 1; wf.nSamplesPerSec = rate;
    wf.wBitsPerSample = 16; wf.nBlockAlign = 2; wf.nAvgBytesPerSec = rate * 2;
    DSBUFFERDESC d = {};
    d.dwSize = sizeof(d);
    d.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS;
    d.dwBufferBytes = static_cast<DWORD>(pcm.size() * 2);
    d.lpwfxFormat = &wf;
    if (FAILED(g_ds->CreateSoundBuffer(&d, &g_engine, nullptr)) || !g_engine) {
        g_engine = nullptr; return;
    }
    void* p1 = nullptr; DWORD s1 = 0;
    if (SUCCEEDED(g_engine->Lock(0, d.dwBufferBytes, &p1, &s1, nullptr, nullptr, 0))) {
        std::memcpy(p1, pcm.data(), s1);
        g_engine->Unlock(p1, s1, nullptr, 0);
    }
    g_engineBaseVol = volume;
    g_engine->SetVolume(VolToDs(volume * g_masterSfx));   // engine = SFX category
    g_engine->Play(0, 0, DSBPLAY_LOOPING);
    logf("EngineStart: engine voice playing (@%u Hz)", g_engineBaseHz);
}

void EngineSetRpm(float norm01) {
    if (!g_engine) return;
    if (norm01 < 0.f) norm01 = 0.f; if (norm01 > 1.f) norm01 = 1.f;
    // idle..redline: 0.7x .. 2.2x base rate.
    const DWORD hz = static_cast<DWORD>(g_engineBaseHz * (0.7f + norm01 * 1.5f));
    g_engine->SetFrequency(hz);
}

void EngineStop() {
    EvLog("engine_stop", "", "", 0.0);
    if (g_engine) { g_engine->Stop(); g_engine->Release(); g_engine = nullptr; }
}

void MusicStart(const char* rwsPath, float volume, int maxSeconds) {
    EvLog("music_start", rwsPath, "", maxSeconds);
    if (!g_ds || g_music) return;
    std::uint32_t rate = 44100;
    std::vector<std::int16_t> pcm;
    const std::size_t cap = (maxSeconds > 0)
        ? static_cast<std::size_t>(maxSeconds) * 44100u : 0u;
    if (!RwsStreamDecode(rwsPath, pcm, rate, cap, kLog) || pcm.empty()) {
        logf("MusicStart: decode failed %s", rwsPath); return;
    }
    WAVEFORMATEX wf = {};
    wf.wFormatTag = WAVE_FORMAT_PCM; wf.nChannels = 1; wf.nSamplesPerSec = rate;
    wf.wBitsPerSample = 16; wf.nBlockAlign = 2; wf.nAvgBytesPerSec = rate * 2;
    DSBUFFERDESC d = {};
    d.dwSize = sizeof(d);
    d.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;
    d.dwBufferBytes = static_cast<DWORD>(pcm.size() * 2);
    d.lpwfxFormat = &wf;
    if (FAILED(g_ds->CreateSoundBuffer(&d, &g_music, nullptr)) || !g_music) {
        g_music = nullptr; logf("MusicStart: CreateSoundBuffer failed"); return;
    }
    void* p1 = nullptr; void* p2 = nullptr; DWORD s1 = 0, s2 = 0;
    if (SUCCEEDED(g_music->Lock(0, d.dwBufferBytes, &p1, &s1, &p2, &s2, 0))) {
        std::memcpy(p1, pcm.data(), s1);
        if (p2 && s2) std::memcpy(p2, reinterpret_cast<const std::uint8_t*>(pcm.data()) + s1, s2);
        g_music->Unlock(p1, s1, p2, s2);
    }
    g_musicBaseVol = volume;
    g_music->SetVolume(VolToDs(volume * g_masterMusic));
    g_music->Play(0, 0, DSBPLAY_LOOPING);
    DWORD st = 0; g_music->GetStatus(&st);
    logf("MusicStart: %s %u samples @%u Hz status=%s", rwsPath,
         (unsigned)pcm.size(), rate, (st & DSBSTATUS_PLAYING) ? "PLAYING" : "stopped");
}

void MusicStop() {
    if (g_music) { g_music->Stop(); g_music->Release(); g_music = nullptr; }
}

// --- character voice banks (cited map; see the REmap note §2) ------------------
// Thin wrapper over the shared 0x004625b0 slot core (Audio/AudioCharacterBankPaths.cpp, decision W0-1 / L-3,
// verify/bankpath_20261024/RESULT_bankpath.md): uppercase, backslashes, no root prefix (the caller adds original/).
bool CharacterBankPath(int charIndex, int langCode, char* buf, int cap) {
    return CharacterBankPathCore(charIndex, langCode, buf, cap);
}

// --- music state controller ---------------------------------------------------
namespace {
// Loop a named 16-bit-mono wave from the loaded SFX bank (permdict) on the music
// voice — used for the menu music (`musicloop1`). No-op if already playing.
void MusicStartBankWaveLoop(const char* waveSub, float volume) {
    {
        const RwsWave* r = g_sfxBank.empty() ? nullptr : RwsBankFind(g_sfxBank, waveSub);
        EvLog("music_bankloop", waveSub, r ? r->name.c_str() : "", volume);
    }
    if (!g_ds || g_music || g_sfxBank.empty()) return;
    const RwsWave* w = RwsBankFind(g_sfxBank, waveSub);
    if (!w || !w->valid()) { logf("MusicBankLoop: '%s' not in SFX bank", waveSub); return; }
    WAVEFORMATEX wf = {};
    wf.wFormatTag = WAVE_FORMAT_PCM; wf.nChannels = 1; wf.nSamplesPerSec = w->rate;
    wf.wBitsPerSample = 16; wf.nBlockAlign = 2; wf.nAvgBytesPerSec = w->rate * 2;
    DSBUFFERDESC d = {};
    d.dwSize = sizeof(d);
    d.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;
    d.dwBufferBytes = static_cast<DWORD>(w->pcm.size());
    d.lpwfxFormat = &wf;
    if (FAILED(g_ds->CreateSoundBuffer(&d, &g_music, nullptr)) || !g_music) { g_music = nullptr; return; }
    void* p1 = nullptr; DWORD s1 = 0;
    if (SUCCEEDED(g_music->Lock(0, d.dwBufferBytes, &p1, &s1, nullptr, nullptr, 0))) {
        std::memcpy(p1, w->pcm.data(), s1);
        g_music->Unlock(p1, s1, nullptr, 0);
    }
    g_musicBaseVol = volume;
    g_music->SetVolume(VolToDs(volume * g_masterMusic));
    g_music->Play(0, 0, DSBPLAY_LOOPING);
    logf("MusicBankLoop '%s' (%u bytes @%u) looping", waveSub, (unsigned)w->pcm.size(), w->rate);
}
}  // namespace

// [D4-WSJ-MUSIC] MASHED_MUSIC_NO_RACE_CDAUDIO=1, default OFF: no cdaudio.rws stream in Race / Results.
// The original has no reader of the `CDAUDIO` row string (0x00604170) and no path builder that can yield
// cdaudio.rws (0x004625ec / 0x00462991 / 0x0045d470 only); its race tick (FUN_00466b50 case 3/7) starts no music
// voice. verify/wsjmusic_20261026/RESULT_D4-WSJ-MUSIC.md.
static bool NoRaceCdaudio() {
    static const bool on = [] {
        char b[8];
        return GetEnvironmentVariableA("MASHED_MUSIC_NO_RACE_CDAUDIO", b, sizeof(b)) > 0 && b[0] == '1';
    }();
    return on;
}

void MusicSetState(MusicState s) {
    if (s == g_musicState) return;          // idempotent: never restart per-frame
    EvLog("music_state", "", "", static_cast<int>(s));
    const char* kCdaudio = "original/toastaudio/pc/audio/pcdics/cdaudio.rws";
    if ((NoRaceCdaudio() || WireOn()) && (s == MusicState::Race || s == MusicState::Results)) {
        MusicStop();
        g_musicState = s;
        logf("MusicSetState -> %d (no cdaudio)", static_cast<int>(s));
        return;
    }
    switch (s) {
        case MusicState::Silent:
            MusicStop();
            break;
        case MusicState::Menu:
            MusicStop();
            MusicStartBankWaveLoop("musicloop1", 0.60f);   // permdict menu loop
            break;
        case MusicState::Race:
            MusicStop();
            MusicStart(kCdaudio, 0.45f, 120);              // cdaudio race music
            break;
        case MusicState::Results:
            // Keep the race stream running but duck it; start a ducked race
            // stream if none is playing (e.g. Results entered cold).
            if (g_music && g_musicState == MusicState::Race) {
                g_musicBaseVol = 0.25f;
                g_music->SetVolume(VolToDs(0.25f * g_masterMusic));
            } else {
                MusicStop();
                MusicStart(kCdaudio, 0.25f, 120);
            }
            break;
    }
    g_musicState = s;
    logf("MusicSetState -> %d", static_cast<int>(s));
}

// [WS-G4] Sound-options master volumes. Setting one re-applies live to the
// persistent voice in that category (music / engine) so the slider audibly moves
// the level while you hold it; one-shot SFX + ambience pick the master up at
// their next play.
void SetMasterMusicVolume(float v) {
    g_masterMusic = Clamp01f(v);
    if (g_music) g_music->SetVolume(VolToDs(g_musicBaseVol * g_masterMusic));
}
void SetMasterSfxVolume(float v) {
    g_masterSfx = Clamp01f(v);
    if (g_engine) g_engine->SetVolume(VolToDs(g_engineBaseVol * g_masterSfx));
    WireReapplyVolumes();                            // [D4-WSJ-WIRE] no-op with the knob unset
}
float MasterMusicVolume() { return g_masterMusic; }
float MasterSfxVolume()   { return g_masterSfx; }

// Window focus / minimize mute. Called from the WinMain WM_ACTIVATE handler with
// the window's active state: when the app loses focus or is minimized the entire
// output is silenced; on refocus the live looping voices (music / engine / ambience)
// are restored to their intended levels. Idempotent. Independent of the user's
// Music/SFX category sliders (those are preserved). One-shot SFX fired while muted
// are silent automatically (g_focusGain is folded into VolToDs at creation).
void SetOutputActive(bool active) {
    const float g = active ? 1.0f : 0.0f;
    if (g == g_focusGain) return;            // no change
    g_focusGain = g;
    // Re-apply the persistent + looping voices so the new gain takes effect on
    // sounds that are ALREADY playing (DirectSound buffer volume is latched, not
    // continuously read). New/one-shot sounds pick g_focusGain up via VolToDs.
    if (g_music)  g_music->SetVolume(VolToDs(g_musicBaseVol  * g_masterMusic));
    if (g_engine) g_engine->SetVolume(VolToDs(g_engineBaseVol * g_masterSfx));
    for (auto& v : g_voices)
        if (v.used && v.buf) v.buf->SetVolume(VolToDs(v.vol));
    for (auto* b : g_sfxRing)
        if (b) b->SetVolume(VolToDs(g_masterSfx));   // transient; mute any still ringing
    WireReapplyVolumes();                            // [D4-WSJ-WIRE] no-op with the knob unset
    logf("SetOutputActive(%d) -> focusGain=%.0f", (int)active, g_focusGain);
}

void SfxLoadBank(const char* rwsPath) {
    if (!g_sfxBank.empty()) return;
    RwsBankLoad(rwsPath, g_sfxBank, kLog);
    WireSelfTest();                                  // [D4-WSJ-WIRE] MASHED_AUDIO_WIRE + _SELFTEST only
}

void SfxPlay(const char* waveNameSub, float volume) {
    {
        const RwsWave* r = g_sfxBank.empty() ? nullptr : RwsBankFind(g_sfxBank, waveNameSub);
        EvLog("sfx", waveNameSub, r ? r->name.c_str() : "", volume);
    }
    // [D4-WSJ-WIRE] knob ON: countdown and explosion go through the permdict voice model (Audio/AudioWire.cpp).
    if (WireSfxByName(waveNameSub)) return;
    if (!g_ds || g_sfxBank.empty()) return;
    const RwsWave* w = RwsBankFind(g_sfxBank, waveNameSub);
    if (!w || !w->valid()) return;
    WAVEFORMATEX wf = {};
    wf.wFormatTag = WAVE_FORMAT_PCM; wf.nChannels = static_cast<WORD>(w->channels);
    wf.nSamplesPerSec = w->rate; wf.wBitsPerSample = 16;
    wf.nBlockAlign = static_cast<WORD>(w->channels * 2);
    wf.nAvgBytesPerSec = w->rate * wf.nBlockAlign;
    DSBUFFERDESC d = {};
    d.dwSize = sizeof(d);
    d.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
    d.dwBufferBytes = static_cast<DWORD>(w->pcm.size());
    d.lpwfxFormat = &wf;
    int slot = g_sfxNext; g_sfxNext = (g_sfxNext + 1) & 7;
    if (g_sfxRing[slot]) { g_sfxRing[slot]->Stop(); g_sfxRing[slot]->Release(); g_sfxRing[slot] = nullptr; }
    IDirectSoundBuffer* buf = nullptr;
    if (FAILED(g_ds->CreateSoundBuffer(&d, &buf, nullptr)) || !buf) return;
    void* p1 = nullptr; DWORD s1 = 0;
    if (SUCCEEDED(buf->Lock(0, d.dwBufferBytes, &p1, &s1, nullptr, nullptr, 0))) {
        std::memcpy(p1, w->pcm.data(), s1);
        buf->Unlock(p1, s1, nullptr, 0);
    }
    buf->SetVolume(VolToDs(volume * g_masterSfx));
    buf->Play(0, 0, 0);          // one-shot
    g_sfxRing[slot] = buf;
}

bool IsPlaying(int voice) {
    if (voice < 0 || voice >= (int)g_voices.size() || !g_voices[voice].used || !g_voices[voice].buf)
        return false;
    DWORD st = 0; g_voices[voice].buf->GetStatus(&st);
    return (st & DSBSTATUS_PLAYING) != 0;
}

unsigned PlayCursor(int voice) {
    if (voice < 0 || voice >= (int)g_voices.size() || !g_voices[voice].used || !g_voices[voice].buf)
        return 0xFFFFFFFFu;
    DWORD play = 0, write = 0;
    if (FAILED(g_voices[voice].buf->GetCurrentPosition(&play, &write))) return 0xFFFFFFFFu;
    return play;
}

}  // namespace Audio
}  // namespace mashed_re
