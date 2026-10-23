// Mashed RE — Save subsystem: VFS file-exists router (0x00550b00) and autosave trigger (0x004099a0).
// One body of each, in BOTH rsp files (lane savelink2, 2026-10-23). Read from the disassembly of
// original/MASHED.exe.unpatched (re/tools/disasm_fn.py 00550b00 00550bbf / 004099a0 004099d6).
//
// Corrections against the previous asi-only copy (session c3-batch-e-s2, 2026-05-15):
//   VfsFileExists
//     - the drive prefix keeps its ':'. FUN_004a34b0 (_strncpy) copies i + 1 bytes and 0x00550b4b writes the terminator
//       at buf[i + 1] ([esp+edi+0x11], buf at esp+0x10); the old copy terminated at buf[i] and compared "X", not "X:".
//     - a colon whose prefix matches no list node (or an empty list, 0x00550b50) resumes the scan for the next colon
//       (0x00550b74); the old copy stopped at the first colon.
//     - a NULL default object (0x00550b79..0x00550b81) falls into the error-callback branch: cb(6), return 0
//       (0x00550b83..0x00550b9c); the old copy returned 0 without the callback.
//   AutosaveTrigger
//     - the message is printf (0x004a2cbd) of the narrow string at 0x005ccb34 "starting autosave\n"; the old copy
//       passed 0x005cc7dc ("...heel\0\0\0\0EarthBuWheel") to wprintf.
// The no-colon VfsFileExists path and AutosaveTrigger's three stores were already right.
//
// Per-target state (#ifdef MASHED_STANDALONE), the GameSaveBuffer.cpp pattern:
//   exe: no RtFS manager exists, so the VFS words are private zeros: VfsFileExists takes the inactive branch with no
//        callback and returns 0. It does NOT replace Save/GameSave.cpp's fopen stand-in for the save path. The autosave
//        guard binds by name to the live save counter (GameFlow_SaveCounterPtr, the DAT_008a95ac model); the three
//        flags it sets get private storage. Both functions are dead code in the exe today (no exe caller).
//   asi: the original absolute addresses, the original printf and its string.
//
// Uncertainties carried: U-2330 (RwGlobals +0xf4 used as the scan length), U-2331 (meaning of the callback code 6),
// U-1550 (meaning of autosave state 4).
//
// Anchor: MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (on .unpatched)
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifndef MASHED_STANDALONE
#include <windows.h>
#include "../Core/HookSystem.h"
#endif

namespace mashed_re { namespace Race { std::uint32_t* GameFlow_SaveCounterPtr(); } }

namespace {

typedef std::uint32_t (__cdecl* ScanFn_t)(const char*);                     // *(RwGlobals + 0xf4)
typedef int           (__cdecl* CmpFn_t)(const char*, std::uint32_t);       // *(RwGlobals + 0xf0)
typedef int           (__cdecl* ExistsFn_t)(std::uint32_t* obj, char* name); // obj word 0x13 (+0x4c)
typedef void          (__cdecl* ErrCb_t)(int);

#ifdef MASHED_STANDALONE
std::uint32_t s_vfs[4];        // active, list head, default obj, error callback: all 0
std::uint32_t s_autosave[3];   // state, write request, offset
std::uint32_t* VfsActive()   { return &s_vfs[0]; }
std::uint32_t* VfsListHead() { return &s_vfs[1]; }
std::uint32_t* VfsDefault()  { return &s_vfs[2]; }
std::uint32_t* VfsErrCb()    { return &s_vfs[3]; }
std::uint32_t  RwGlobals()   { return 0; }   // read only on the active branch, which the exe never takes
std::uint32_t* SaveCounter() { return mashed_re::Race::GameFlow_SaveCounterPtr(); }
std::uint32_t* AutosaveState()    { return &s_autosave[0]; }
std::uint32_t* AutosaveWriteReq() { return &s_autosave[1]; }
std::uint32_t* AutosaveOffset()   { return &s_autosave[2]; }
void AutosaveMessage() { std::printf("starting autosave\n"); }   // the string at 0x005ccb34
#else
std::uint32_t* VfsActive()   { return reinterpret_cast<std::uint32_t*>(0x007dc75cu); }
std::uint32_t* VfsListHead() { return reinterpret_cast<std::uint32_t*>(0x007dc754u); }
std::uint32_t* VfsDefault()  { return reinterpret_cast<std::uint32_t*>(0x007dc76cu); }
std::uint32_t* VfsErrCb()    { return reinterpret_cast<std::uint32_t*>(0x007dc768u); }
std::uint32_t  RwGlobals()   { return *reinterpret_cast<std::uint32_t*>(0x007d3ff8u); }
std::uint32_t* SaveCounter() { return reinterpret_cast<std::uint32_t*>(0x008a95acu); }
std::uint32_t* AutosaveState()    { return reinterpret_cast<std::uint32_t*>(0x008a9584u); }
std::uint32_t* AutosaveWriteReq() { return reinterpret_cast<std::uint32_t*>(0x008a9594u); }
std::uint32_t* AutosaveOffset()   { return reinterpret_cast<std::uint32_t*>(0x008a95b0u); }
void AutosaveMessage() {
    reinterpret_cast<int (__cdecl*)(const char*, ...)>(0x004a2cbdu)(reinterpret_cast<const char*>(0x005ccb34u));
}
#endif

int Dispatch(std::uint32_t* obj, char* name) {
    return reinterpret_cast<ExistsFn_t>(obj[0x13])(obj, name);   // 0x00550bab..0x00550bad call [esi+0x4c]
}

} // namespace

// 0x00550b00
int VfsFileExistsCore(char* name)
{
    if (*VfsActive() != 0) {                                                   // 0x00550b00..0x00550b0e
        const std::uint32_t len = (*reinterpret_cast<ScanFn_t*>(RwGlobals() + 0xf4))(name);   // 0x00550b14..0x00550b1a
        for (std::uint32_t i = 0; i < len; ++i) {                             // 0x00550b25..0x00550b77, unsigned
            if (name[i] != ':') continue;                                      // 0x00550b2b
            // 100-byte stack buffer (0x00550b05 sub esp,0x64). A colon at index 99 or later overruns it in the
            // original as well.
            char buf[100];
            std::strncpy(buf, name, i + 1);                                    // 0x00550b31..0x00550b3b FUN_004a34b0
            buf[i + 1] = 0;                                                    // 0x00550b4b
            for (std::uint32_t* n = reinterpret_cast<std::uint32_t*>(*VfsListHead()); n != nullptr;
                 n = reinterpret_cast<std::uint32_t*>(n[0])) {                 // 0x00550b40..0x00550b72
                if ((*reinterpret_cast<CmpFn_t*>(RwGlobals() + 0xf0))(buf, n[3]) == 0)   // 0x00550b52..0x00550b6c
                    return Dispatch(n, name);                                  // 0x00550b9d..0x00550b9f
            }
        }
        std::uint32_t* obj = reinterpret_cast<std::uint32_t*>(*VfsDefault()); // 0x00550b79
        if (obj != nullptr) return Dispatch(obj, name);                       // 0x00550b7f..0x00550b81
    }
    const std::uint32_t cb = *VfsErrCb();                                      // 0x00550b83
    if (cb != 0) reinterpret_cast<ErrCb_t>(cb)(6);                             // 0x00550b8c..0x00550b8e [U-2331]
    return 0;
}

// 0x004099a0
void AutosaveTriggerCore()
{
    if (*SaveCounter() == 0) return;                                          // 0x004099a0..0x004099a7
    AutosaveMessage();                                                        // 0x004099a9..0x004099ae
    *AutosaveState()    = 4;                                                  // 0x004099b6 [U-1550]
    *AutosaveWriteReq() = 1;                                                  // 0x004099c0
    *AutosaveOffset()   = 0;                                                  // 0x004099ca
}

#ifndef MASHED_STANDALONE
// =====================================================================================================================
// .asi install + in-process A/B (MASHED_LEAF_SELFTEST=1; log leaf_selftest.log in the CWD, shared with the other leaves).
//   VfsFileExists, live: the original twice through a trampoline (noo), then the shared body (ndiff); the return and the
//     fs-object exists calls are compared, the call is classed by whether the name holds a ':'.
//   MASHED_LEAF_SELFTEST_SWEEP=1, once at the first hooked VfsFileExists: the four VFS words are swapped for synthetic
//     fs objects (recording exists stubs) and an error-callback recorder, then restored. The RwGlobals scan / compare
//     functions are the game's own. Then AutosaveTrigger with the guard word 0 and non-zero over sentinel flag values;
//     its four words are restored after.
// =====================================================================================================================
namespace {
void* g_vfsResume = reinterpret_cast<void*>(0x00550b05);
void* g_autoResume = reinterpret_cast<void*>(0x004099a5);
// Both open with a 5-byte MOV EAX,[abs] (A1 5C C7 7D 00 / A1 AC 95 8A 00).
__declspec(naked) int __cdecl VfsOrig(char*) {
    __asm {
        mov eax, dword ptr ds:[007dc75ch]
        jmp dword ptr [g_vfsResume]
    }
}
__declspec(naked) void __cdecl AutoOrig() {
    __asm {
        mov eax, dword ptr ds:[008a95ach]
        jmp dword ptr [g_autoResume]
    }
}

int SelfTestOn() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST"); v = (s && s[0] && s[0] != '0') ? 1 : 0; }
    return v;
}
void Log(const char* s) {
    HANDLE h = CreateFileA("leaf_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

// Records every exists / error-callback call of one run.
struct Rec { std::uint32_t n, obj[4], name[4], cbn, cbarg; };
Rec g_rec;
int __cdecl RecExists(std::uint32_t* obj, char* name) {
    if (g_rec.n < 4) { g_rec.obj[g_rec.n] = reinterpret_cast<std::uint32_t>(obj); g_rec.name[g_rec.n] =
        reinterpret_cast<std::uint32_t>(name); }
    ++g_rec.n;
    return static_cast<int>(obj[1]);                                         // a per-object tag
}
void __cdecl RecErrCb(int code) { ++g_rec.cbn; g_rec.cbarg = static_cast<std::uint32_t>(code); }

// Live calls: forward to the real exists functions, recording them.
struct LiveRec { std::uint32_t n, obj, name; int ret; };
LiveRec g_live;
volatile LONG g_vCalls[2], g_vNoo[2], g_vNd[2], g_vTotal;

// Synthetic fs objects: [0] next, [1] tag (the exists result), [3] prefix-name pointer, [0x13] exists fn.
std::uint32_t g_obj[5][0x14];
const char* const kObjName[5] = { "a:", "a", "x:y:", "q", "default" };
void ObjInit() {
    for (int k = 0; k < 5; ++k) {
        std::memset(g_obj[k], 0, sizeof g_obj[k]);
        g_obj[k][1] = 0x100u + k;
        g_obj[k][3] = reinterpret_cast<std::uint32_t>(kObjName[k]);
        g_obj[k][0x13] = reinterpret_cast<std::uint32_t>(&RecExists);
    }
}
struct VfsOut { int ret; Rec rec; };
// which: 0 = original, 1 = shared
void VfsRun(int which, char* name, VfsOut* o) {
    std::memset(&g_rec, 0, sizeof g_rec);
    o->ret = which == 0 ? VfsOrig(name) : VfsFileExistsCore(name);
    o->rec = g_rec;
}
long g_vsCases, g_vsNoo, g_vsNd, g_vsCb, g_vsDisp;
void VfsCase(const char* tag, std::uint32_t active, std::uint32_t head, std::uint32_t dflt, std::uint32_t cb,
             const char* nm) {
    char name[64]; std::strncpy(name, nm, sizeof name - 1); name[sizeof name - 1] = 0;
    *VfsActive() = active; *VfsListHead() = head; *VfsDefault() = dflt; *VfsErrCb() = cb;
    VfsOut o1, o2, om;
    VfsRun(0, name, &o1); VfsRun(0, name, &o2); VfsRun(1, name, &om);
    const int noo = (o1.ret != o2.ret) + (std::memcmp(&o1.rec, &o2.rec, sizeof o1.rec) != 0);
    const int nd = (o1.ret != om.ret) + (std::memcmp(&o1.rec, &om.rec, sizeof o1.rec) != 0);
    ++g_vsCases; g_vsNoo += (noo != 0); g_vsNd += (nd != 0); g_vsCb += (o1.rec.cbn != 0); g_vsDisp += (o1.rec.n != 0);
    if (noo || nd) {
        char line[300];
        wsprintfA(line, "[vs%ld] fn=550b00 %s name=\"%s\" o=%x/%u/%u m=%x/%u/%u noo=%d ndiff=%d\r\n", g_vsCases, tag, nm,
                  o1.ret, o1.rec.n, o1.rec.cbn, om.ret, om.rec.n, om.rec.cbn, noo, nd);
        Log(line);
    }
}
void VfsSweep() {
    std::uint32_t keep[4] = { *VfsActive(), *VfsListHead(), *VfsDefault(), *VfsErrCb() };
    ObjInit();
    // list: obj1 ("a") -> obj0 ("a:") -> obj2 ("x:y:"); obj3 ("q") alone; obj4 = default
    g_obj[1][0] = reinterpret_cast<std::uint32_t>(g_obj[0]);
    g_obj[0][0] = reinterpret_cast<std::uint32_t>(g_obj[2]);
    const std::uint32_t L = reinterpret_cast<std::uint32_t>(g_obj[1]);
    const std::uint32_t Q = reinterpret_cast<std::uint32_t>(g_obj[3]);
    const std::uint32_t D = reinterpret_cast<std::uint32_t>(g_obj[4]);
    const std::uint32_t C = reinterpret_cast<std::uint32_t>(&RecErrCb);
    const char* names[] = { "abc", "a:b", "a:", ":", "", "x:y:z", "x:y", "q:r", "zz:a:b", "b:x:y:z", "a:b:c" };
    const std::uint32_t heads[] = { L, Q, 0 };
    const std::uint32_t dflts[] = { D, 0 };
    const std::uint32_t cbs[] = { C, 0 };
    for (std::uint32_t h : heads) for (std::uint32_t d : dflts) for (std::uint32_t c : cbs)
        for (const char* nm : names) VfsCase("active", 1, h, d, c, nm);
    for (std::uint32_t c : cbs) for (const char* nm : names) VfsCase("inactive", 0, L, D, c, nm);
    *VfsActive() = keep[0]; *VfsListHead() = keep[1]; *VfsDefault() = keep[2]; *VfsErrCb() = keep[3];
    char line[200];
    wsprintfA(line, "SWEEP fn=550b00 cases=%ld noo=%ld ndiff=%ld dispatched=%ld errcb=%ld\r\n", g_vsCases, g_vsNoo, g_vsNd,
              g_vsDisp, g_vsCb);
    Log(line);
}

long g_asCases, g_asNoo, g_asNd;
void AutoCase(std::uint32_t guard) {
    std::uint32_t* w[4] = { SaveCounter(), AutosaveState(), AutosaveWriteReq(), AutosaveOffset() };
    const std::uint32_t pre[4] = { guard, 0xa1a1a1a1u, 0xb2b2b2b2u, 0xc3c3c3c3u };
    std::uint32_t o1[4], o2[4], om[4];
    for (int k = 0; k < 4; ++k) *w[k] = pre[k];
    AutoOrig();
    for (int k = 0; k < 4; ++k) { o1[k] = *w[k]; *w[k] = pre[k]; }
    AutoOrig();
    for (int k = 0; k < 4; ++k) { o2[k] = *w[k]; *w[k] = pre[k]; }
    AutosaveTriggerCore();
    for (int k = 0; k < 4; ++k) om[k] = *w[k];
    const int noo = std::memcmp(o1, o2, sizeof o1) != 0, nd = std::memcmp(o1, om, sizeof o1) != 0;
    ++g_asCases; g_asNoo += noo; g_asNd += nd;
    char line[200];
    wsprintfA(line, "[as%ld] fn=4099a0 guard=%x o=%x/%x/%x m=%x/%x/%x noo=%d ndiff=%d\r\n", g_asCases, guard, o1[1], o1[2],
              o1[3], om[1], om[2], om[3], noo, nd);
    Log(line);
}
void AutoSweep() {
    std::uint32_t* w[4] = { SaveCounter(), AutosaveState(), AutosaveWriteReq(), AutosaveOffset() };
    std::uint32_t keep[4];
    for (int k = 0; k < 4; ++k) keep[k] = *w[k];
    AutoCase(0); AutoCase(1); AutoCase(0x80000000u); AutoCase(0xffffffffu);
    for (int k = 0; k < 4; ++k) *w[k] = keep[k];
    char line[120];
    wsprintfA(line, "SWEEP fn=4099a0 cases=%ld noo=%ld ndiff=%ld\r\n", g_asCases, g_asNoo, g_asNd);
    Log(line);
}

void Sweeps() {
    const char* e = std::getenv("MASHED_LEAF_SELFTEST_SWEEP");
    if (!e || !e[0] || e[0] == '0') return;
    VfsSweep();
    AutoSweep();
}

__declspec(noinline) int __cdecl VfsDispatch(char* name) {
    if (!SelfTestOn() || !name) return VfsFileExistsCore(name);
    static LONG s_swept = 0;
    if (InterlockedExchange(&s_swept, 1) == 0) Sweeps();
    const int cls = std::strchr(name, ':') != nullptr ? 1 : 0;
    const int r1 = VfsOrig(name);
    const int r2 = VfsOrig(name);
    const int rm = VfsFileExistsCore(name);
    const int noo = r1 != r2, nd = r1 != rm;
    const LONG c = InterlockedIncrement(&g_vCalls[cls]);
    if (noo) InterlockedIncrement(&g_vNoo[cls]);
    if (nd) InterlockedIncrement(&g_vNd[cls]);
    const LONG t = InterlockedIncrement(&g_vTotal);
    char line[300];
    if (noo || nd || c <= 4) {
        wsprintfA(line, "[%ld] fn=550b00 colon=%d name=\"%.80s\" r=%x m=%x noo=%d ndiff=%d\r\n", t, cls, name, r1, rm, noo,
                  nd);
        Log(line);
    }
    if (t <= 16 || (t & 0xff) == 0) {
        wsprintfA(line, "SUM fn=550b00 calls=%ld plain=%ld/%ld/%ld colon=%ld/%ld/%ld\r\n", t, g_vCalls[0], g_vNoo[0],
                  g_vNd[0], g_vCalls[1], g_vNoo[1], g_vNd[1]);
        Log(line);
    }
    return rm;
}
}  // namespace

// The installed entries and the path1 exports (names kept: re/frida/hooks_registry.py, re/frida/bisect_save_hooks.py).
extern "C" __declspec(dllexport) int __cdecl VfsFileExists(char* name) { return VfsDispatch(name); }
RH_ScopedInstall(VfsFileExists, 0x00550b00);
extern "C" __declspec(dllexport) void __cdecl AutosaveTrigger() { AutosaveTriggerCore(); }
RH_ScopedInstall(AutosaveTrigger, 0x004099a0);
#endif
