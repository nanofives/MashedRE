// AudioWire impl — see AudioWire.h. [D4-WSJ-WIRE], knob MASHED_AUDIO_WIRE=1 (default OFF).
//
// Model: the original's 139 permdict virtual voices (DAT_0068f640[v*2], cumulative bases from the +0x84 counts).
// A voice is "busy" ((voice+0x34 & 2) != 0 in the original) while its one-shot plays (PCM length / start frequency,
// QPC clock, so a MASHED_MUTE=1 run takes the same decisions) or while a loop runs. Every start is logged to
// MASHED_AUDIO_EVENT_LOG as `vstart,<wave>,v<voice>,<car>` so an arm can be scored against the D4-WSJ-REF tables.
#include "AudioWire.h"
#include "AudioEngineInternal.h"
#include "EngineClassTable.h"

#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cstdint>

namespace mashed_re {
namespace Vehicle {
int   Fi_ParticipantCount();                       // DAT_008a94d0 (FUN_0040e340), Vehicle/ForceIntegratorStubs.cpp
float VehiclePhysics_RecordF32(int slot, int off); // Vehicle/VehiclePhysicsRun.cpp (0 before the sim is up)
}  // namespace Vehicle

namespace Audio {

namespace {

// ---- W1: permdict wave table (verify/wsj80e_20261026/wave_tables.tsv rows 0..42, row VA 0x006044f0 + row*0x94) ----
// name = row +0x00, count = row +0x84, flag = row +0x88. The +0x80 base is 0 in the image and filled at load as the
// running sum of the counts (80E RESULT; sum 139 = DAT_0068fc84 measured in D4-WSJ-REF).
struct WaveRow { const char* name; int count; int flag88; };
constexpr WaveRow kPerm[] = {
    {"eng1", 4, 1}, {"eng1d", 4, 1}, {"eng2", 4, 1}, {"eng2d", 4, 1},
    {"eng3", 4, 1}, {"eng3d", 4, 1}, {"eng4", 4, 1}, {"eng4d", 4, 1},
    {"explosion1", 8, 0}, {"impact with barrier", 4, 0}, {"impact with other vehicle 1", 4, 0},
    {"impact with crate", 4, 0}, {"impact with snowplough", 4, 0}, {"mushroomhit", 4, 0},
    {"helicopter", 3, 1}, {"menu navigation", 1, 0}, {"menu selection", 1, 0}, {"threetwoone", 1, 0},
    {"rocket", 4, 0}, {"machineg", 4, 0}, {"missile exhaust", 4, 1}, {"shotgun", 4, 0},
    {"flame thrower", 4, 1}, {"oilslick", 4, 0}, {"flash", 4, 0}, {"timingbeam", 1, 0},
    {"go", 1, 0}, {"drop mine", 4, 0}, {"collision with pod", 4, 0}, {"SPLASH", 4, 0},
    {"mortarexhaust", 4, 0}, {"carlands", 4, 0}, {"fire", 4, 1}, {"debris", 4, 0},
    {"beatthebomb", 1, 0}, {"beatthebomblasercut", 1, 0}, {"beatthebombexplode", 1, 0}, {"bullethitscar", 4, 0},
    {"helicopterKC", 1, 1}, {"noise burst", 1, 0}, {"backfire", 4, 0}, {"laugh", 1, 0},
    {"musicloop1", 1, 1},
};
constexpr int kPermRows   = static_cast<int>(sizeof(kPerm) / sizeof(kPerm[0]));   // 43 = DAT_0068fc88
constexpr int kPermVoices = 139;                                                 // DAT_0068fc84
static_assert(kPermRows == 43, "permdict rows");

// Wave indices used below (permdict row numbers; the original pushes these as FUN_00465ca0's first argument).
enum : int {
    kWEng1 = 0, kWExplosion1 = 8, kWImpactBarrier = 9, kWThreeTwoOne = 17, kWRocket = 18, kWMachineG = 19,
    kWMissileExhaust = 20, kWShotgun = 21, kWFlameThrower = 22, kWOilslick = 23, kWFlash = 24, kWGo = 26,
    kWDropMine = 27, kWPod = 28, kWMortarExhaust = 30, kWFire = 32,
};

// ---- constants (D4-WSJ-SFXMAP raw consts.txt / dis_465ca0.txt / dis_464670.txt; ENGCLASS RESULT) ----
constexpr float kFreq22050     = 22050.0f;      // 0x46ac4400 (0x00465d48, 0x005ce6b8)
constexpr float kVolOne        = 1.0f;          // 0x3f800000 (0x00465d72)
constexpr float kCountdownVol  = 0.6f;          // 0x3f19999a (FUN_00462f50)
constexpr float kExplStep      = 2000.0f;       // 0x005cd0b8
constexpr float kImpactAdd     = 500.0f;        // 0x005ccd04 (0x0046471d)
constexpr float kImpactScaleK1 = 1.8181818e-4f; // 0x005ce9bc (0x00464730, kind 1)
constexpr float kImpactScale   = 2.5e-4f;       // 0x005ce9b8 (0x00464738)
constexpr float kImpactPitchR  = 2205.0f;       // 0x4509d000 (0x00464770)
constexpr float kRpmToPitch    = 3.5f;          // _DAT_005ce26c 0x40600000
constexpr float kCarPitchStep  = 200.0f;        // _DAT_005ce194 0x43480000
constexpr float kRpmToVol      = 1.0e-5f;       // _DAT_005cc990 0x3727c5ac
constexpr float kRpmMax        = 9500.0f;       // DAT_006130c0
constexpr float kMinRpmJitter  = 250.0f;        // FUN_00472650(0, 0x437a0000) 0x004636ed
constexpr int   kPinDat0068f4c8 = 1;            // [W3] pinned: FUN_004627f0 wrote 1 in every D4-WSJ-REF capture
constexpr int   kSpeedOff      = 0x9e4;         // FUN_0046d6d0 = record +0x9e4 (Ai/AiPostStepBrake.cpp)

struct VVoice {
    bool   loop = false;
    double busyUntilMs = 0.0;
    void*  buf = nullptr;
    float  vol = 0.0f;
};
VVoice s_v[kPermVoices];
int    s_base[kPermRows] = {};
bool   s_baseInit = false;
const RwsWave* s_wave[kPermRows] = {};
bool   s_waveInit = false;

int    s_numCars = 0;
int    s_class[4] = {0, 0, 0, 0};
int    s_partOverride = -1;          // self-test only
bool   s_speedOverrideOn = false;    // self-test only
float  s_speedOverride[4] = {};
int    s_podCount = 0;               // DAT_008aa2d0
int    s_guardClosed[kPermRows] = {};
std::uint32_t s_rng = 0x2545f491u;   // audio-private: the original's FUN_00472650 is the shared RNG (not taken here)

struct RingEv { int type, car, other; };
RingEv s_ring[256];
int    s_ringN = 0;

double NowMs() {
    static LARGE_INTEGER f = {};
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    LARGE_INTEGER q; QueryPerformanceCounter(&q);
    return static_cast<double>(q.QuadPart) * 1000.0 / static_cast<double>(f.QuadPart);
}
float Rand01() {
    s_rng = s_rng * 1664525u + 1013904223u;
    return static_cast<float>(s_rng >> 8) * (1.0f / 16777216.0f);
}
void InitTables() {
    if (!s_baseInit) {
        int b = 0;
        for (int i = 0; i < kPermRows; ++i) { s_base[i] = b; b += kPerm[i].count; }
        s_baseInit = true;
    }
    if (!s_waveInit) {
        const RwsWave* any = detail::SfxBankWaveExact(kPerm[0].name);
        if (any) {   // the bank is loaded: resolve every row once
            for (int i = 0; i < kPermRows; ++i) s_wave[i] = detail::SfxBankWaveExact(kPerm[i].name);
            s_waveInit = true;
        }
    }
}
int  Base(int wave) { InitTables(); return s_base[wave]; }
bool Loops(int wave) {
    // flag +0x88 (FUN_005a6dc0(v,8,1,flag) at load, 80E RESULT) on the rows whose only stops are explicit
    // (engines via race end, flame thrower via FUN_004661a0). [UNCERTAIN] the 80E RESULT names the field, not its
    // meaning. missile exhaust plays one-shot: its stop (FUN_00455910 terminal, FUN_004661a0 0x00455985) is not ported.
    return kPerm[wave].flag88 != 0 && wave != kWMissileExhaust;
}
bool Busy(int v) { return s_v[v].loop || NowMs() < s_v[v].busyUntilMs; }
int  Participants() {
    if (s_partOverride >= 0) return s_partOverride;
    return Vehicle::Fi_ParticipantCount();             // DAT_008aa254 = FUN_0040e340() (0x00466b68)
}
bool GuardClosed() { return kPinDat0068f4c8 != 0 && Participants() > 1; }
void NoteGuard(int wave) {
    if (s_guardClosed[wave]++ == 0) detail::EvLogRow("guard_closed", kPerm[wave].name, "first", Participants());
}
float CarSpeed(int car) {
    if (s_speedOverrideOn) return (car >= 0 && car < 4) ? s_speedOverride[car] : 0.0f;
    return Vehicle::VehiclePhysics_RecordF32(car, kSpeedOff);
}

void StopVoice(int v, int wave) {
    if (s_v[v].buf) detail::VoiceStop(s_v[v].buf);
    const bool was = Busy(v);
    s_v[v] = VVoice{};
    if (was) { char r[16]; std::snprintf(r, sizeof r, "v%d", v); detail::EvLogRow("vstop", kPerm[wave].name, r, -1); }
}
void StartVoice(int wave, int v, float hz, float vol, int car) {
    InitTables();
    const RwsWave* w = s_wave[wave];
    if (s_v[v].buf) detail::VoiceStop(s_v[v].buf);
    s_v[v] = VVoice{};
    s_v[v].loop = Loops(wave);
    s_v[v].vol  = vol;
    if (w && w->valid() && hz > 0.0f) {
        const int bps = w->channels * (w->bits / 8);
        const double samples = bps > 0 ? static_cast<double>(w->pcm.size()) / bps : 0.0;
        s_v[v].busyUntilMs = NowMs() + samples * 1000.0 / hz;
        s_v[v].buf = detail::VoicePlay(*w, hz, vol, s_v[v].loop);
    }
    char r[16]; std::snprintf(r, sizeof r, "v%d", v);
    detail::EvLogRow("vstart", kPerm[wave].name, r, car);
}

// FUN_00465ca0 (0x00465ca0..0x00465d9b): first free voice of `wave`.
int FirstFree(int wave) {
    if (wave == kWFire && GuardClosed()) { NoteGuard(wave); return -1; }   // 0x00465ca0..0x00465cb7
    // FUN_00492d10()==3 (0x00465cbd..0x00465cc5): every wire caller is a race-frame site.
    const int base = Base(wave);
    int got = -1;
    for (int i = 0; i < kPerm[wave].count; ++i) {                           // 0x00465cf0..0x00465d86
        if (!Busy(base + i)) { StartVoice(wave, base + i, kFreq22050, kVolOne, -1); got = base + i; break; }
    }
    if (got < 0) detail::EvLogRow("vfull", kPerm[wave].name, "", kPerm[wave].count);
    if (wave == kWPod) ++s_podCount;                                        // 0x00465d8d..0x00465d95 INC [0x008aa2d0]
    return got;
}
// FUN_00465c10 / FUN_00465e80: fixed slot base+slot, only if free. [UNCERTAIN] their start frequency / volume were
// not read (SFXMAP lists the slot rule only); FUN_00465ca0's 22050 / 1.0 are used.
int FixedSlot(int wave, int slot) {
    if (slot < 0 || slot >= kPerm[wave].count) return -1;
    const int v = Base(wave) + slot;
    if (Busy(v)) { detail::EvLogRow("vbusy", kPerm[wave].name, "", slot); return -1; }
    StartVoice(wave, v, kFreq22050, kVolOne, slot);
    return v;
}
void StopSlot(int wave, int slot) {   // FUN_004661a0
    if (slot < 0 || slot >= kPerm[wave].count) return;
    StopVoice(Base(wave) + slot, wave);
}
// FUN_00465940 (decomp 00465940.c): explosion1 voices base+4..base+7, pitch k*2000 + 22050 for k = 4,3,2,1.
int ExplosionScan() {
    const int base = Base(kWExplosion1);
    int k = 4;
    for (int v = base + 4; v < base + 8; ++v, --k) {
        if (!Busy(v)) {
            StartVoice(kWExplosion1, v, static_cast<float>(k) * kExplStep + kFreq22050, kVolOne, -1);
            return v;
        }
    }
    detail::EvLogRow("vfull", kPerm[kWExplosion1].name, "", 4);
    return -1;
}
// FUN_00464670(car, kind, other, override) (dis_464670.txt). Wave = 9 + kind (0x00464686: [kind*0x94 + 0x604aa4]).
int Impact(int car, int kind, int other, float ovr) {
    const int wave = kWImpactBarrier + kind;
    if (GuardClosed()) { NoteGuard(wave); return -1; }                  // 0x00464671..0x00464695
    if (car < 0 || car >= kPerm[wave].count || other < 0 || other >= kPerm[wave].count) return -1;
    const int base = Base(wave);
    if (Busy(base + car) || Busy(base + other)) return -1;              // 0x004646a5..0x004646c9
    const float a = CarSpeed(car), b = CarSpeed(other);                 // FUN_0046d6d0 x2
    float val = (ovr != 0.0f) ? ovr + kImpactAdd : (a < b ? b : a);     // 0x004646ec..0x00464723
    float vol = val * (kind == 1 ? kImpactScaleK1 : kImpactScale);      // 0x00464727..0x00464738
    if (vol > 1.0f) vol = 1.0f;                                          // 0x00464744..0x00464757
    const float hz = Rand01() * kImpactPitchR + kFreq22050;             // FUN_00472650(0, 2205.0) + 22050
    StartVoice(wave, base + car, hz, vol, car);
    return base + car;
}
void RingDrain() {   // FUN_004648b0: newest first, types 3 and 4 (8 and 0xb have no port producer)
    for (int n = s_ringN - 1; n >= 0; --n) {
        const RingEv& e = s_ring[n];
        if (e.type == 3)      Impact(e.car, 0, 0, 0.0f);
        else if (e.type == 4) Impact(e.car, 1, e.other, 0.0f);
    }
    s_ringN = 0;
}
void StopAllWire() {
    for (int v = 0; v < kPermVoices; ++v) if (s_v[v].buf) detail::VoiceStop(s_v[v].buf);
    for (auto& x : s_v) x = VVoice{};
    s_ringN = 0;
}
bool EnvOn(const char* name) {
    char b[8];
    return GetEnvironmentVariableA(name, b, sizeof(b)) > 0 && b[0] == '1';
}
}  // namespace

bool WireOn() {
    static const bool on = EnvOn("MASHED_AUDIO_WIRE");
    return on;
}

int WireVehicleIndexByName(const char* dffBase) {
    if (!dffBase) return -1;
    for (int i = 0; i < 13; ++i) if (_stricmp(dffBase, ::Audio::kVehicleNames[i]) == 0) return i;
    return -1;
}
int WireEngineClass(int vehicleIdx) {
    if (vehicleIdx < 0 || vehicleIdx > 0xb) return 0;   // JA 0x004636d9 at 0x00463687 (and the unknown case)
    return ::Audio::kEngineClassByVehicle[vehicleIdx];
}

void WireRaceBegin(int numCars, const int* vehicleByCar) {
    if (!WireOn()) return;
    StopAllWire();
    for (auto& g : s_guardClosed) g = 0;
    s_podCount = 0;
    s_numCars = numCars;
    for (int c = 0; c < 4; ++c) {
        const int vi = (vehicleByCar && c < numCars) ? vehicleByCar[c] : -1;
        s_class[c] = WireEngineClass(vi);
        if (c < numCars)
            detail::EvLogRow("engine_class", (vi >= 0 && vi < 13) ? ::Audio::kVehicleNames[vi] : "?",
                             kPerm[kWEng1 + 2 * s_class[c]].name, c);
    }
    detail::EvLogRow("wire_race", "", "", numCars);
}

// FUN_00466b50 race case (decomp 00466b50.c): FUN_004648b0 then, per car, FUN_00463640.
void WireRaceTick(float playerNorm01) {
    if (!WireOn()) return;
    RingDrain();
    int cars = Participants();
    if (cars <= 0) cars = s_numCars;
    if (cars > 4) cars = 4;
    if (playerNorm01 < 0.f) playerNorm01 = 0.f;
    if (playerNorm01 > 1.f) playerNorm01 = 1.f;
    for (int c = 0; c < cars; ++c) {
        const int cls  = s_class[c];
        const int wave = kWEng1 + 2 * cls;                                    // row = class*2 (0x0046370b)
        const int v    = Base(wave) + c;                                      // 0x00463725 + 0x00463731
        const int vd   = Base(wave + 1) + c;                                  // d layer = slot + 4 (0x00463768)
        const float minRpm = ::Audio::kEngineMinRpm[cls] + Rand01() * kMinRpmJitter;   // 0x004636ed..0x004636f9
        // [UNCERTAIN] the rpm state machine (DAT_0069054c states 0..4, tables 0x00613030..0x006130c8) is not ported:
        // car 0 maps the stand-in's speed norm onto [min rpm, 9500], cars 1..3 hold min rpm.
        float rpm = (c == 0) ? minRpm + playerNorm01 * (kRpmMax - minRpm) : minRpm;
        if (rpm > kRpmMax) rpm = kRpmMax;
        if (rpm < minRpm)  rpm = minRpm;
        const float hz  = rpm * kRpmToPitch + static_cast<float>(c) * kCarPitchStep
                        + ::Audio::kEngineBasePitch[cls];                     // 0x0046371f, 0x0046372b
        const float vol = rpm * kRpmToVol + ::Audio::kEngineVolBias[cls];    // 0x00463b5a / 0x00463b63
        if (!Busy(v)) {                                                       // (voice+0x34 & 2) == 0
            StartVoice(wave, v, hz, vol, c);
            StartVoice(wave + 1, vd, hz, vol, c);                             // DAT_006130bc = 1 (0x00463682)
        }
        // [UNCERTAIN] the d voice's own pitch / volume law (0x00463b39) was not read: the main voice's is used.
        s_v[v].vol = vol; s_v[vd].vol = vol;
        detail::VoiceSet(s_v[v].buf, hz, vol);
        detail::VoiceSet(s_v[vd].buf, hz, vol);
    }
}

void WireRaceEnd() {
    if (!WireOn()) return;
    for (int w = 0; w < kPermRows; ++w)
        if (s_guardClosed[w]) detail::EvLogRow("guard_closed_total", kPerm[w].name, "", s_guardClosed[w]);
    detail::EvLogRow("pod_count", "", "", s_podCount);
    StopAllWire();
}

void WirePowerup(int ev, int owner) {
    if (!WireOn()) return;
    detail::EvLogRow("pu_event", "", "", ev);
    switch (ev) {
        case kPuSfxGunFire:     FirstFree(kWMachineG); break;
        case kPuSfxDrumFire:    FirstFree(kWRocket); break;
        // [UNCERTAIN] the original's slot is DAT_006883ec[record] (the missile record); the port has no record index
        // at this site, so the owner is used.
        case kPuSfxMissileFire: FixedSlot(kWRocket, owner); FixedSlot(kWMissileExhaust, owner); break;
        case kPuSfxPMineFire:   FirstFree(kWDropMine); break;
        case kPuSfxShotgunFire: FirstFree(kWShotgun); break;
        case kPuSfxFlashFire:   FirstFree(kWFlash); break;
        case kPuSfxMortarFire:  FirstFree(kWRocket); FirstFree(kWMortarExhaust); break;
        case kPuSfxOilFire:     FixedSlot(kWOilslick, owner); break;
        case kPuSfxFlameArm:    FixedSlot(kWFlameThrower, owner); break;
        case kPuSfxFlameDeact:  StopSlot(kWFlameThrower, owner); break;
        default: break;
    }
}

void WirePickup() {
    if (!WireOn()) return;
    FirstFree(kWPod);
}

void WireRingPush(int type, int car, int other) {
    if (!WireOn()) return;
    if (s_ringN < 256) s_ring[s_ringN++] = RingEv{type, car, other};
}

bool WireSfxByName(const char* name) {
    if (!WireOn() || !name) return false;
    int wave = -1;
    if (_stricmp(name, "threetwoone") == 0) wave = kWThreeTwoOne;        // FUN_00462f50 0x00463014
    else if (_stricmp(name, "go") == 0)    wave = kWGo;                  // FUN_00462f50 0x004630cb
    else if (_stricmp(name, "explosion1") == 0) { ExplosionScan(); return true; }   // FUN_00465940
    if (wave < 0) return false;
    const int v = Base(wave);                                            // single voice; started only if not playing
    if (!Busy(v)) StartVoice(wave, v, kFreq22050, kCountdownVol, -1);
    return true;
}

void WireReapplyVolumes() {
    if (!WireOn()) return;
    for (auto& x : s_v) if (x.buf) detail::VoiceSet(x.buf, -1.0f, x.vol);
}

// ---- self-test (MASHED_AUDIO_WIRE_SELFTEST=1). One `selftest,<name>,PASS|FAIL,<value>` row per check. ----
void WireSelfTest() {
    if (!WireOn() || !EnvOn("MASHED_AUDIO_WIRE_SELFTEST")) return;
    auto row = [](const char* n, bool ok, double val) { detail::EvLogRow("selftest", n, ok ? "PASS" : "FAIL", val); };
    InitTables();
    StopAllWire();
    {   // W1: 139 voices, every row name present in the loaded permdict
        int sum = 0, found = 0;
        for (int i = 0; i < kPermRows; ++i) { sum += kPerm[i].count; if (s_wave[i]) ++found; }
        row("table_voices_139", sum == kPermVoices, sum);
        row("table_names_in_bank_43", found == kPermRows, found);
        row("base_fire_116", Base(kWFire) == 116, Base(kWFire));
        row("base_musicloop1_138", Base(42) == 138, Base(42));
    }
    s_partOverride = 4;
    {   // W2: first-free on machineg (base 70, 4 voices), then full
        bool ok = true;
        for (int i = 0; i < 4; ++i) ok = ok && FirstFree(kWMachineG) == 70 + i;
        row("firstfree_70_71_72_73", ok, 4);
        row("firstfree_full_rejects", FirstFree(kWMachineG) == -1, -1);
    }
    {   // W3: fire guard
        row("fire_rejected_parts4", FirstFree(kWFire) == -1, 4);
        s_partOverride = 1;
        row("fire_accepted_parts1_v116", FirstFree(kWFire) == 116, 1);
        s_partOverride = 4;
    }
    {   // W2: explosion scan pitches 30050 / 28050 / 26050 / 24050 on voices 36..39
        bool ok = true;
        for (int i = 0; i < 4; ++i) ok = ok && ExplosionScan() == 36 + i;
        row("explosion_36_37_38_39", ok, 4);
        row("explosion_full_rejects", ExplosionScan() == -1, -1);
    }
    {   // W6: ring type 3, car 2 -> impact with barrier voice 42 only with the guard open
        s_speedOverrideOn = true;
        s_speedOverride[0] = 1000.0f; s_speedOverride[2] = 3000.0f;
        WireRingPush(3, 2, 0); RingDrain();
        row("impact_guard_closed_parts4", !Busy(42), 4);
        s_partOverride = 1;
        WireRingPush(3, 2, 0); RingDrain();
        row("impact_guard_open_v42", Busy(42), 1);
        WireRingPush(3, 2, 0); RingDrain();   // busy -> no second start (the voice still plays)
        row("impact_busy_no_restart", Busy(42), 1);
        s_speedOverrideOn = false;
        s_partOverride = 4;
    }
    {   // W2: fixed slot oilslick owner 1 -> voice 87, then busy
        row("fixedslot_oil_owner1_v87", FixedSlot(kWOilslick, 1) == 87, 87);
        row("fixedslot_busy_rejects", FixedSlot(kWOilslick, 1) == -1, -1);
    }
    {   // W7: engine class by the port's DFF base names
        row("class_ADVANTAGE_1", WireEngineClass(WireVehicleIndexByName("ADVANTAGE")) == 1, 1);
        row("class_STALLION_1", WireEngineClass(WireVehicleIndexByName("STALLION")) == 1, 1);
        row("class_SHURIKEN_3", WireEngineClass(WireVehicleIndexByName("SHURIKEN")) == 3, 3);
        row("class_FORMULA_0", WireEngineClass(WireVehicleIndexByName("FORMULA")) == 0, 0);
        row("class_unknown_0", WireEngineClass(WireVehicleIndexByName("NOPE")) == 0, 0);
    }
    s_partOverride = -1;
    StopAllWire();
    s_podCount = 0;
    for (auto& g : s_guardClosed) g = 0;
}

}  // namespace Audio
}  // namespace mashed_re
