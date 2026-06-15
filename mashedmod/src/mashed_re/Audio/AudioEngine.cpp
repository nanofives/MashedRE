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
