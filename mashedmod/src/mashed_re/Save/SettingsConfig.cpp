// Mashed RE — Save/SettingsConfig.cpp: the video-config cluster (lane savelink2, 2026-10-23; matched bodies PORT-SAVE-01,
// 2026-11-08). Save/SettingsCfg.cpp (ConfigSave) is folded in and removed.
//
//   0x004963e0  ConfigLogError     void(const char*)      fputs to the log handle 0x00772fbc
//   0x00496400  ConfigLogDebug     void(const char*, ...) vsprintf into 0x200 bytes, fputs to the log handle
//   0x00498910  ConfigFilenameGet  char*()                "videocfg.bin" (0x005d012c) copied to 0x007731e8
//   0x00498950  ConfigLoad         int()                  fopen "rb", fread 0x200 into 0x00773208
//   0x004989b0  ConfigSave         int()                  fopen "wb", fwrite 0x200 from 0x00773208
//
// PORT-SAVE-01 (2026-11-08): the five bodies are the byte-matched sources in re/match/s16/ (MATCH-S16, XP DDK cl
// 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, match.py 100%, relocheck OK), compiled unchanged in both targets:
//   0x004963e0 re/match/s16/004963e0.c   here, at the end of this TU
//   0x00496400 re/match/s16/00496400.c   Match/PortSave01_Config.cpp
//   0x00498910 / 0x00498950 / 0x004989b0 re/match/s16/00498910.c (the same three functions as 00498950.c and
//                                        004989b0.c), Match/PortSave01_Config.cpp
// 004963e0.c defines FUN_004963e0(char*) while 00498910.c declares it (const char*, ...), as the original's separate C
// TUs did, so the two cannot share one C++ TU. The call is cdecl with one pointer argument either way.
// This TU supplies what the matched sources bind to, per target:
//   * callees (fputs and the CRT entries 0x004a42c5 / 0x004a4541 / 0x004a49cf / 0x004a4b22 / 0x004a4368) -> the
//     extern "C" SettingsCfg* functions below, which call the same Host* functions the replaced bodies called;
//   * globals (MGLOBAL / MGLOBALA in the matched preamble) -> SettingsCfgGlobal(address): the original address in the
//     .asi, and in the exe the same private storage and string copies the replaced bodies used (the exe's mapping of
//     the original .rdata is zero-filled, so the strings at 0x005d013c / 0x005cf010 / ... are empty there).
// The file name is the matched body's own literal "videocfg.bin", the name the replaced body used in both targets.
//
// Read from the disassembly of original/MASHED.exe.unpatched (re/tools/disasm_fn.py 004963e0 0049646f / 00498910
// 004989fd). FUN_004a4541 is fopen(path, mode): push 0x40, call _fsopen 0x004a44e5, 0x004a4541..0x004a4553.
//
// The bodies follow the UNPATCHED binary. The boot patch disable_log makes 0x00496400 a RET, 0x004963e7 a JMP and the
// log opener 0x00496490 return 0 (re/analysis/BOOT_PATCHES.md), so in the patched reference the handle stays NULL and
// the installed hooks return early exactly as the patched bytes do. Installing them does override the two patched
// sites. With the opener patched that is unobservable (recorded in RESULT_settingsconfig.md).
//
// Per-target host functions (#ifdef MASHED_STANDALONE):
//   exe: the exe's own CRT. The log handle, the filename copy and the 0x200-byte settings buffer are private. The exe
//        never reads videocfg.bin (its window is fixed, exe_main.cpp). ConfigLoad / ConfigSave run only under
//        MASHED_VIDEOCFG_WRITE (default ON since L-29 (a), =0 opts out): Boot/VideoCfgFirstRun.cpp, the first-run default write.
//   asi: the game's CRT (cross-CRT FILE* is not shareable) through push/ret thunks, the original strings and globals.
//
// OPTIMIZER NOTE (kept from the previous copies): MSVC /O2 + LTCG replaced every typed call of the game's fputs by
// address with its own CRT symbol, so the host functions and the sweep are compiled with optimization off and the game
// CRT is reached through naked push/ret thunks. The matched body at the end is compiled with optimization on.
//
// Anchor: MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (on .unpatched)
#pragma optimize("", off)

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <share.h>
#ifndef MASHED_STANDALONE
#include <windows.h>
#include "../Core/HookSystem.h"
#else
#include "../Compat/PortStateSeed.h"
#endif

namespace {

#ifdef MASHED_STANDALONE
void*         s_logHandle = nullptr;
char          s_filenameBuf[16];
unsigned char s_settings[0x200];
void**        LogHandle()   { return &s_logHandle; }
char*         FilenameBuf() { return s_filenameBuf; }
void*         SettingsBuf() { return s_settings; }
// PORT-SEED-OPTOUT (verify/portstate_20261109/RESULT_PORT-SEED-OPTOUT.md): the five .rdata strings the matched config
// bodies read, as the original holds them (MASHED.exe.unpatched). Used only when the port-state seed is OFF
// (MASHED_NO_PORTSTATE_SEED=1). With the seed ON (default) the bodies read the seeded original addresses.
char s_cfgLoading[27] = "Loading video cfg from %s
";   // 0x005d013c
char s_cfgSaving[24]  = "Saving video cfg to %s
";      // 0x005d0158
char s_cfgFailed[9]   = "	FAILED
";                     // 0x005cfe70
char s_cfgModeRb[3]   = "rb";                             // 0x005cf010
char s_cfgModeWb[3]   = "wb";                             // 0x005d00d8
int      HostFputs(const char* s, void* f)                     { return std::fputs(s, static_cast<std::FILE*>(f)); }
int      HostVsprintf(char* b, const char* f, va_list a)       { return std::vsprintf(b, f, a); }
void*    HostFopen(const char* p, const char* m)               { return _fsopen(p, m, _SH_DENYNO); }
unsigned HostFread(void* b, unsigned e, unsigned c, void* f)   {
    return static_cast<unsigned>(std::fread(b, e, c, static_cast<std::FILE*>(f)));
}
unsigned HostFwrite(const void* b, unsigned e, unsigned c, void* f) {
    return static_cast<unsigned>(std::fwrite(b, e, c, static_cast<std::FILE*>(f)));
}
int      HostFclose(void* f)                                   { return std::fclose(static_cast<std::FILE*>(f)); }
#else
void**        LogHandle()   { return reinterpret_cast<void**>(0x00772fbcu); }
char*         FilenameBuf() { return reinterpret_cast<char*>(0x007731e8u); }
void*         SettingsBuf() { return reinterpret_cast<void*>(0x00773208u); }
const char* const kFmtLoading = reinterpret_cast<const char*>(0x005d013cu);
const char* const kFmtSaving  = reinterpret_cast<const char*>(0x005d0158u);
const char* const kFailed     = reinterpret_cast<const char*>(0x005cfe70u);
const char* const kModeRead   = reinterpret_cast<const char*>(0x005cf010u);
const char* const kModeWrite  = reinterpret_cast<const char*>(0x005d00d8u);
__declspec(naked) int __cdecl HostFputs(const char*, void*)              { __asm { push 0x004a4254
                                                                                   ret } }
__declspec(naked) int __cdecl HostVsprintf(char*, const char*, va_list)  { __asm { push 0x004a42c5
                                                                                   ret } }
__declspec(naked) void* __cdecl HostFopen(const char*, const char*)      { __asm { push 0x004a4541
                                                                                   ret } }
__declspec(naked) unsigned __cdecl HostFread(void*, unsigned, unsigned, void*)        { __asm { push 0x004a49cf
                                                                                                ret } }
__declspec(naked) unsigned __cdecl HostFwrite(const void*, unsigned, unsigned, void*) { __asm { push 0x004a4b22
                                                                                                ret } }
__declspec(naked) int __cdecl HostFclose(void*)                          { __asm { push 0x004a4368
                                                                                   ret } }
#endif

} // namespace

// What the matched sources bind to (both targets). Callee signatures are the matched sources' own declarations.
extern "C" {
int SettingsCfgFputs(const char* s, FILE* f) { return HostFputs(s, f); }
int SettingsCfgVsprintf(char* buf, const char* fmt, char* args) {
    return HostVsprintf(buf, fmt, reinterpret_cast<va_list>(args));
}
FILE* SettingsCfgFopen(const char* name, const char* mode) { return static_cast<FILE*>(HostFopen(name, mode)); }
unsigned int SettingsCfgFread(void* buf, unsigned int size, unsigned int n, FILE* f) {
    return HostFread(buf, size, n, f);
}
unsigned int SettingsCfgFwrite(const void* buf, unsigned int size, unsigned int n, FILE* f) {
    return HostFwrite(buf, size, n, f);
}
int SettingsCfgFclose(FILE* f) { return HostFclose(f); }

// Original address -> the storage the matched body reads and writes in this target.
void* SettingsCfgGlobal(unsigned int a) {
#ifdef MASHED_STANDALONE
    switch (a) {
    case 0x00772fbcu: return LogHandle();
    case 0x007731e8u: return FilenameBuf();
    case 0x00773208u: return SettingsBuf();
    default: break;
    }
    // PORT-STATE-02 (L-101): the five .rdata strings (0x005d013c, 0x005d0158, 0x005cfe70, 0x005cf010, 0x005d00d8)
    // fall through to their original addresses: the port-state seed (Compat/PortStateSeed.cpp, default ON) fills them.
    // PORT-SEED-OPTOUT: with MASHED_NO_PORTSTATE_SEED=1 nothing fills them (zeros), and the first-run load passed an
    // empty fopen mode from 0x005cf010 to UCRT _fsopen, whose invalid-parameter handler ended the process with
    // 0xC0000409 (common_fsopen -> _invalid_parameter_noinfo, from ConfigLoad+0x20). The opt-out binds the bodies to
    // the private copies above instead. The binding is made by the bodies' MGLOBALA dynamic initializers, before
    // WinMain's PortStateSeed_Apply, so it follows the knob, not the apply result (a refused seed, FNV mismatch, is a
    // build fault the seed --check lint stops).
    if (!mashed_re::Compat::PortStateSeed_On()) {
        switch (a) {
        case 0x005d013cu: return s_cfgLoading;
        case 0x005d0158u: return s_cfgSaving;
        case 0x005cfe70u: return s_cfgFailed;
        case 0x005cf010u: return s_cfgModeRb;
        case 0x005d00d8u: return s_cfgModeWb;
        default: break;
        }
    }
#endif
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(a));
}

// The matched bodies (exports kept from the replaced copies).
__declspec(dllexport) void __cdecl ConfigLogError(char* msg);              // here, re/match/s16/004963e0.c
__declspec(dllexport) void __cdecl ConfigLogDebug(const char* fmt, ...);   // Match/PortSave01_Config.cpp
__declspec(dllexport) char* __cdecl ConfigFilenameGet(void);               // Match/PortSave01_Config.cpp
__declspec(dllexport) int __cdecl ConfigLoad(void);                        // Match/PortSave01_Config.cpp
__declspec(dllexport) int __cdecl ConfigSave(void);                        // Match/PortSave01_Config.cpp
}  // extern "C"

#ifdef MASHED_STANDALONE
// The exe's private 0x200-byte buffer (0x00773208 in the original), filled by Boot/VideoCfgFirstRun.cpp's port of
// FUN_00498ea0 before ConfigSave (MASHED_VIDEOCFG_WRITE, default ON, =0 opts out).
void* ConfigSettingsBuf() { return SettingsBuf(); }
#endif

#ifndef MASHED_STANDALONE
// =====================================================================================================================
// .asi install + A/B. The five installed entries count their live calls. SettingsSweep() (exported; called through
// Frida by re/frida/nav_champ_probe.py --plan settingssweep at the mode-select screen, private root only) runs each case
// through a trampoline that replays the UNPATCHED original's first bytes (twice, noo) and then the shared body (ndiff).
// Compared per case: the return, the 0x200-byte settings buffer, the 16-byte filename copy, the log text (a temp file
// opened through the game's CRT and set as the log handle), and for ConfigSave the bytes written to videocfg.bin. Every
// case restores the handle, both buffers and videocfg.bin (bytes and attributes). Log: leaf_selftest.log in the CWD.
// =====================================================================================================================
namespace {
void* g_errResume  = reinterpret_cast<void*>(0x004963e9);
void* g_dbgResume  = reinterpret_cast<void*>(0x00496406);
void* g_nameResume = reinterpret_cast<void*>(0x00498915);
void* g_loadResume = reinterpret_cast<void*>(0x00498956);
void* g_saveResume = reinterpret_cast<void*>(0x004989b6);
// Original entry replay: 0x004963e0..0x004963e7 with the original JE (the patched site is 0x004963e7), body resumed at
// 0x004963e9.
__declspec(naked) void __cdecl LogErrorOrig(const char*) {
    __asm {
        mov  eax, dword ptr ds:[00772fbch]
        test eax, eax
        jz   skip
        jmp  dword ptr [g_errResume]
    skip:
        ret
    }
}
// Original entry replay: SUB ESP,0x204 of 0x00496400 (the patched RET), body resumed at 0x00496406.
__declspec(naked) void __cdecl LogDebugOrig(const char*, ...) {
    __asm {
        sub  esp, 204h
        jmp  dword ptr [g_dbgResume]
    }
}
__declspec(naked) char* __cdecl FilenameOrig() {
    __asm {
        mov  eax, dword ptr ds:[005d012ch]
        jmp  dword ptr [g_nameResume]
    }
}
// PUSH ESI + CALL 0x00498910 (relative in the original, re-encoded absolute here), resumed after the call.
__declspec(naked) int __cdecl LoadOrig() {
    __asm {
        push esi
        mov  eax, 00498910h
        call eax
        jmp  dword ptr [g_loadResume]
    }
}
__declspec(naked) int __cdecl SaveOrig() {
    __asm {
        push esi
        mov  eax, 00498910h
        call eax
        jmp  dword ptr [g_saveResume]
    }
}

void Log(const char* s) {
    HANDLE h = CreateFileA("leaf_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
DWORD ReadAll(const char* path, unsigned char* out, DWORD cap) {
    HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return 0xffffffffu;
    DWORD n = 0; ReadFile(h, out, cap, &n, nullptr); CloseHandle(h);
    return n;
}
void WriteAll(const char* path, const unsigned char* in, DWORD n) {
    HANDLE h = CreateFileA(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD w; WriteFile(h, in, n, &w, nullptr); CloseHandle(h);
}

const char* const kCfg = "videocfg.bin";
const char* const kLogTmp = "leaf_cfglog.tmp";
const char* const kHidden = "videocfg.bin.sweep_hidden";

struct Out { int ret; unsigned char set[0x200]; char name[16]; DWORD logN; unsigned char log[600]; DWORD cfgN;
             unsigned char cfg[0x200]; };
enum Fn { kErr, kDbg, kName, kLoad, kSave };
// env: bit 0 = a log handle (temp file), bit 1 = videocfg.bin hidden, bit 2 = videocfg.bin read-only
void Run(Fn fn, int orig, int env, const char* msg, const unsigned char* set0, Out* o) {
    std::memcpy(SettingsBuf(), set0, 0x200);
    std::memset(FilenameBuf(), 0x5a, 16);
    void* lf = nullptr;
    if (env & 1) { lf = HostFopen(kLogTmp, kModeWrite); *LogHandle() = lf; } else *LogHandle() = nullptr;
    switch (fn) {
    case kErr:  orig ? LogErrorOrig(msg) : ConfigLogError(const_cast<char*>(msg)); o->ret = 0; break;
    case kDbg:  orig ? LogDebugOrig(msg, 0x1234, "zq", 'k') : ConfigLogDebug(msg, 0x1234, "zq", 'k'); o->ret = 0;
                break;
    case kName: { char* r = orig ? FilenameOrig() : ConfigFilenameGet();
                  o->ret = (r == FilenameBuf()) ? 1 : 2; } break;
    case kLoad: o->ret = orig ? LoadOrig() : ConfigLoad(); break;
    case kSave: o->ret = orig ? SaveOrig() : ConfigSave(); break;
    }
    *LogHandle() = nullptr;
    if (lf) HostFclose(lf);
    std::memcpy(o->set, SettingsBuf(), 0x200);
    std::memcpy(o->name, FilenameBuf(), 16);
    std::memset(o->log, 0, sizeof o->log);
    o->logN = lf ? ReadAll(kLogTmp, o->log, sizeof o->log) : 0;
    if (lf) DeleteFileA(kLogTmp);
    std::memset(o->cfg, 0, sizeof o->cfg);
    o->cfgN = (fn == kSave) ? ReadAll(kCfg, o->cfg, sizeof o->cfg) : 0;
}
int Diff(const Out& a, const Out& b) {
    return (a.ret != b.ret) + (std::memcmp(a.set, b.set, sizeof a.set) != 0) + (std::memcmp(a.name, b.name, 16) != 0) +
           (a.logN != b.logN) + (std::memcmp(a.log, b.log, sizeof a.log) != 0) + (a.cfgN != b.cfgN) +
           (std::memcmp(a.cfg, b.cfg, sizeof a.cfg) != 0);
}
long g_cases, g_noo, g_nd, g_logged;
unsigned char g_cfg0[0x200]; DWORD g_cfg0N;
void RestoreCfg() {
    SetFileAttributesA(kCfg, FILE_ATTRIBUTE_NORMAL);
    if (GetFileAttributesA(kHidden) != INVALID_FILE_ATTRIBUTES) { DeleteFileA(kCfg); MoveFileA(kHidden, kCfg); }
    if (g_cfg0N != 0xffffffffu) WriteAll(kCfg, g_cfg0, g_cfg0N);
}
void Case(Fn fn, int env, const char* msg, const unsigned char* set0) {
    Out* o = static_cast<Out*>(std::malloc(3 * sizeof(Out)));
    if (!o) return;
    for (int k = 0; k < 3; ++k) {
        if (env & 2) MoveFileA(kCfg, kHidden);
        if (env & 4) SetFileAttributesA(kCfg, FILE_ATTRIBUTE_READONLY);
        Run(fn, k < 2 ? 1 : 0, env, msg, set0, &o[k]);
        RestoreCfg();
    }
    const int noo = Diff(o[0], o[1]), nd = Diff(o[0], o[2]);
    ++g_cases; g_noo += (noo != 0); g_nd += (nd != 0); g_logged += (o[0].logN != 0 && o[0].logN != 0xffffffffu);
    char line[300];
    wsprintfA(line, "[cs%ld] fn=%d env=%d ret=%d/%d log=%lu/%lu cfg=%lu/%lu noo=%d ndiff=%d\r\n", g_cases, (int)fn, env,
              o[0].ret, o[2].ret, o[0].logN, o[2].logN, o[0].cfgN, o[2].cfgN, noo, nd);
    Log(line);
    std::free(o);
}
}  // namespace

extern "C" __declspec(dllexport) int __cdecl SettingsSweep()
{
    void* keepLog = *LogHandle();
    unsigned char keepSet[0x200]; std::memcpy(keepSet, SettingsBuf(), 0x200);
    char keepName[16]; std::memcpy(keepName, FilenameBuf(), 16);
    g_cfg0N = ReadAll(kCfg, g_cfg0, sizeof g_cfg0);
    unsigned char patt[0x200];
    for (int i = 0; i < 0x200; ++i) patt[i] = static_cast<unsigned char>(i * 13 + 7);
    char line[200];
    // PORT-SAVE-01: the installed entries are the matched bodies, which carry no live-call counters.
    wsprintfA(line, "LIVE fn=settings cfg0=%lu\r\n", g_cfg0N);
    Log(line);
    const char* msgs[] = { "x\n", "", "\tFAILED\n" };
    const char* fmts[] = { "plain\n", "a %d %s %c\n", "%08x|%s|%c|%%\n" };
    for (int env = 0; env < 2; ++env) {
        for (const char* m : msgs) Case(kErr, env, m, patt);
        for (const char* f : fmts) Case(kDbg, env, f, patt);
        Case(kName, env, nullptr, patt);
        Case(kLoad, env, nullptr, patt);          // file present
        Case(kLoad, env | 2, nullptr, patt);      // file missing: "\tFAILED\n"
        Case(kSave, env, nullptr, patt);          // writes patt
        Case(kSave, env | 4, nullptr, patt);      // read-only: open fails, no log
    }
    *LogHandle() = keepLog;
    std::memcpy(SettingsBuf(), keepSet, 0x200);
    std::memcpy(FilenameBuf(), keepName, 16);
    RestoreCfg();
    unsigned char now[0x200]; const DWORD nowN = ReadAll(kCfg, now, sizeof now);
    const int cfgSame = (nowN == g_cfg0N) && std::memcmp(now, g_cfg0, nowN == 0xffffffffu ? 0 : nowN) == 0;
    wsprintfA(line, "SWEEP fn=settings cases=%ld noo=%ld ndiff=%ld logged=%ld cfg_restored=%d\r\n", g_cases, g_noo, g_nd,
              g_logged, cfgSame);
    Log(line);
    return static_cast<int>(g_nd + g_noo);
}

RH_ScopedInstall(ConfigLogError, 0x004963e0);
RH_ScopedInstall(ConfigLogDebug, 0x00496400);
RH_ScopedInstall(ConfigFilenameGet, 0x00498910);
RH_ScopedInstall(ConfigLoad, 0x00498950);
RH_ScopedInstall(ConfigSave, 0x004989b0);
#endif

// =====================================================================================================================
// 0x004963e0: re/match/s16/004963e0.c, compiled unchanged (both targets). The preamble's MGLOBAL is defined here so the
// log handle binds through SettingsCfgGlobal. fputs binds to SettingsCfgFputs.
// =====================================================================================================================
#pragma optimize("", on)
#define MATCH_PREAMBLE
#define MGLOBAL(T, n, a) static T &n = *(T *)SettingsCfgGlobal(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])SettingsCfgGlobal(a)
extern "C" {
#define FUN_004963e0 ConfigLogError
#define fputs SettingsCfgFputs
#include "../../../../re/match/s16/004963e0.c"
#undef fputs
}  // extern "C"
