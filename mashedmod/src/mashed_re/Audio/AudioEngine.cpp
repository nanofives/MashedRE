// AudioEngine impl — DirectSound 8 secondary-buffer playback. See AudioEngine.h.
#include "AudioEngine.h"
#include "RwsBank.h"

#include <windows.h>
#include <dsound.h>
#include <mmreg.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cmath>
#include <vector>

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

IDirectSound8* g_ds = nullptr;
IDirectSoundBuffer* g_engine = nullptr;   // procedural engine voice
DWORD               g_engineBaseHz = 22050;
IDirectSoundBuffer* g_music = nullptr;    // the single music voice
MusicState          g_musicState = MusicState::Silent;
std::vector<RwsWave> g_sfxBank;           // cached 0x809 SFX bank (permdict)
IDirectSoundBuffer*  g_sfxRing[8] = {};   // one-shot SFX voices (round-robin)
int                  g_sfxNext = 0;

struct Voice {
    IDirectSoundBuffer* buf = nullptr;
    bool                loop = false;
    bool                used = false;
};
std::vector<Voice> g_voices;

// 0..1 linear -> DirectSound hundredths-of-dB (DSBVOLUME_MIN..MAX = -10000..0).
LONG VolToDs(float v) {
    if (v <= 0.0001f) return DSBVOLUME_MIN;
    if (v >= 1.0f)    return DSBVOLUME_MAX;     // 0
    LONG db = static_cast<LONG>(2000.0 * std::log10(static_cast<double>(v)));
    if (db < DSBVOLUME_MIN) db = DSBVOLUME_MIN;
    if (db > DSBVOLUME_MAX) db = DSBVOLUME_MAX;
    return db;
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
    buf->SetVolume(VolToDs(volume));
    if (FAILED(buf->Play(0, 0, loop ? DSBPLAY_LOOPING : 0))) {
        buf->Release(); logf("PlayLoop: Play failed"); return -1;
    }

    int id = -1;
    for (size_t i = 0; i < g_voices.size(); ++i)
        if (!g_voices[i].used) { id = static_cast<int>(i); break; }
    if (id < 0) { id = static_cast<int>(g_voices.size()); g_voices.push_back(Voice{}); }
    g_voices[id].buf = buf; g_voices[id].loop = loop; g_voices[id].used = true;

    DWORD st = 0; buf->GetStatus(&st);
    logf("PlayLoop voice=%d '%s' rate=%u ch=%d bytes=%u loop=%d status=%s",
         id, w->name.c_str(), w->rate, w->channels, desc.dwBufferBytes, (int)loop,
         (st & DSBSTATUS_PLAYING) ? "PLAYING" : "stopped");
    return id;
}
}  // namespace

bool Init(void* hwnd) {
    if (g_ds) return true;
    HRESULT hr = DirectSoundCreate8(nullptr, &g_ds, nullptr);
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
    return CreateAndPlay(rwsPath, waveName, volume, true);
}
int PlayOnce(const char* rwsPath, const char* waveName, float volume) {
    return CreateAndPlay(rwsPath, waveName, volume, false);
}

void SetVolume(int voice, float volume) {
    if (voice >= 0 && voice < (int)g_voices.size() && g_voices[voice].used && g_voices[voice].buf)
        g_voices[voice].buf->SetVolume(VolToDs(volume));
}

void Stop(int voice) {
    if (voice >= 0 && voice < (int)g_voices.size() && g_voices[voice].used) {
        if (g_voices[voice].buf) { g_voices[voice].buf->Stop(); g_voices[voice].buf->Release(); }
        g_voices[voice] = Voice{};
    }
}

void StopAll() {
    for (size_t i = 0; i < g_voices.size(); ++i)
        if (g_voices[i].used && g_voices[i].buf) {
            g_voices[i].buf->Stop(); g_voices[i].buf->Release(); g_voices[i] = Voice{};
        }
}

void EngineStart(float volume) {
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
    g_engine->SetVolume(VolToDs(volume));
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
    if (g_engine) { g_engine->Stop(); g_engine->Release(); g_engine = nullptr; }
}

void MusicStart(const char* rwsPath, float volume, int maxSeconds) {
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
    g_music->SetVolume(VolToDs(volume));
    g_music->Play(0, 0, DSBPLAY_LOOPING);
    DWORD st = 0; g_music->GetStatus(&st);
    logf("MusicStart: %s %u samples @%u Hz status=%s", rwsPath,
         (unsigned)pcm.size(), rate, (st & DSBSTATUS_PLAYING) ? "PLAYING" : "stopped");
}

void MusicStop() {
    if (g_music) { g_music->Stop(); g_music->Release(); g_music = nullptr; }
}

// --- character voice banks (cited map; see the REmap note §2) ------------------
const char* CharacterBankName(int charIndex) {
    // DAT_006041f0 order (0x006041f0 stride 0x80): RED/BLUEJAY/MELON/GOLD/PINK/
    // SHADOW. Lowercased to match the on-disk english/<name>.rws files.
    static const char* kNames[6] = { "red", "bluejay", "melon", "gold", "pink", "shadow" };
    if (charIndex < 0 || charIndex > 5) return nullptr;
    return kNames[charIndex];
}

bool CharacterBankPath(int charIndex, int langCode, char* buf, int cap) {
    const char* name = CharacterBankName(charIndex);
    if (!name || !buf || cap <= 0) return false;
    const char* lang;          // FUN_004625b0 language switch
    switch (langCode) {
        case 1:  lang = "french";  break;
        case 2:  lang = "german";  break;
        case 3:  lang = "spanish"; break;
        case 4:  lang = "italian"; break;
        default: lang = "english"; break;
    }
    std::snprintf(buf, static_cast<std::size_t>(cap),
                  "original/toastaudio/pc/audio/pcdics/%s/%s.rws", lang, name);
    return true;
}

// --- music state controller ---------------------------------------------------
namespace {
// Loop a named 16-bit-mono wave from the loaded SFX bank (permdict) on the music
// voice — used for the menu music (`musicloop1`). No-op if already playing.
void MusicStartBankWaveLoop(const char* waveSub, float volume) {
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
    g_music->SetVolume(VolToDs(volume));
    g_music->Play(0, 0, DSBPLAY_LOOPING);
    logf("MusicBankLoop '%s' (%u bytes @%u) looping", waveSub, (unsigned)w->pcm.size(), w->rate);
}
}  // namespace

void MusicSetState(MusicState s) {
    if (s == g_musicState) return;          // idempotent: never restart per-frame
    const char* kCdaudio = "original/toastaudio/pc/audio/pcdics/cdaudio.rws";
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
                g_music->SetVolume(VolToDs(0.25f));
            } else {
                MusicStop();
                MusicStart(kCdaudio, 0.25f, 120);
            }
            break;
    }
    g_musicState = s;
    logf("MusicSetState -> %d", static_cast<int>(s));
}

void SfxLoadBank(const char* rwsPath) {
    if (!g_sfxBank.empty()) return;
    RwsBankLoad(rwsPath, g_sfxBank, kLog);
}

void SfxPlay(const char* waveNameSub, float volume) {
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
    buf->SetVolume(VolToDs(volume));
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
