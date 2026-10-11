// Mashed RE — negative-control mutation kit (DEV .asi ONLY; never in exe_sources.rsp).
//
// Purpose: prove that a Frida A/B handler (arg_type) can read RED. A handler that
// stays GREEN while the reimpl arm runs a deliberately WRONG port is BLIND to that
// class of defect, and every C3/C4 row whose evidence rests on that handler +
// registry entry is at risk (SimGolf's *_negctl.cpp discipline, made generic).
//
// One naked forwarder, NegCtl_Entry, stands in for the reimpl export. It wraps the
// REAL port (any calling convention: it never touches a register or stack slot
// before forwarding, it stashes only the return address) and corrupts it in exactly
// one way, chosen per run by NegCtl_Arm:
//
//   1 null        no body: return 0 in EAX (and 0.0 in ST0 for a float return); the
//                 port is never called. Stack-balanced for stdcall via stdcall_bytes.
//   2 ret_int     forward, then EAX ^= 1.
//   3 ret_float   forward, then ST0 = ST0*2 + 0.75 (fixed point only at -0.75).
//   4 field_first forward, then XOR 0x5A into the FIRST byte the port changed.
//   5 field_last  forward, then XOR 0x5A into the LAST byte the port changed.
//   6 heap_extra  one extra game-CRT _malloc(0x20) (0x004a45fb, leaked), then forward.
//   7 import_extra one extra kernel32!GetTickCount call through this module's IAT,
//                 then forward.
//   8 field_none  CONTROL for 4/5: the same freeze + snapshot + diff, no flip. If it reads RED the
//                 vector is sensitive to the freeze latency (the game rewrote a seeded global
//                 meanwhile) and 4/5's verdicts for that hook mean nothing.
//
// "Changed" (4/5) = bytes that differ before/after the forwarded call in
//   (a) a <=0x400-byte window at every stack arg 0..7 and every entry register
//       (ECX, EAX, EDX, ESI, EDI, EBX) that points at committed writable memory
//       outside MASHED.exe's image and outside this thread's stack, scanned in that
//       order, then
//   (b) MASHED.exe's writable sections (.data, _rwdseg), ascending, minus a noise
//       mask: every byte seen changing while NegCtl_Arm samples them 20x over
//       ~400 ms with nothing called (the running game's per-frame writes).
// For the span snapshot .. diff the game's own threads (Win32 start address inside MASHED.exe) are
// suspended, so the running game cannot write a byte between the two looks (the 400 ms noise mask alone
// missed timer-like globals such as 0x007d70cc..0x007d70d8, first run 2026-10-11).
// The flip is undone at the next NegCtl_Arm if the byte still holds the flipped
// value, so one warm process can run many jobs. If the port changed nothing in
// (a)+(b), the field classes forward unchanged and NegCtl_Stats reports flips=0
// (the class is then NOT APPLICABLE to that vector, not "blind").
//
// Gate: NegCtl_Arm returns NULL unless the process env has MASHED_NEGCTL=1, which
// only re/frida/run_negctl.py sets. Not thread-safe (single A/B thread by design).

#ifdef MASHED_STANDALONE
#error "NegCtl.cpp is a dev-.asi-only harness; it must never be linked into mashed_re.exe"
#endif

#include <windows.h>
#include <intrin.h>
#include <tlhelp32.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
// Plain globals so the naked forwarder can address them by name.
void*    g_ncTarget       = nullptr;
void*    g_ncRet          = nullptr;
int      g_ncCls          = 0;
int      g_ncFloatRet     = 0;
unsigned g_ncStdcallBytes = 0;
float    g_ncThreeQuarters = 0.75f;
}

namespace {

enum { kNull = 1, kRetInt = 2, kRetFloat = 3, kFieldFirst = 4, kFieldLast = 5, kHeapExtra = 6, kImportExtra = 7,
       kFieldNone = 8 };

const unsigned kArgWindow = 0x400;
const int      kMaxWins   = 14;          // 8 stack args + 6 registers
const int      kCalSamples = 20;

struct Region { uint8_t* base; size_t len; };

Region   s_glob[4];
int      s_nGlob = 0;
size_t   s_globTotal = 0;
uint8_t* s_globPre = nullptr;            // snapshot of all global regions, concatenated
uint8_t* s_noise   = nullptr;            // 1 byte per global byte (0/1), accumulated

uint8_t* s_win[kMaxWins];
size_t   s_winLen[kMaxWins];
uint8_t  s_winPre[kMaxWins][kArgWindow];
int      s_nWin = 0;

unsigned s_calls = 0, s_flips = 0, s_lastChanged = 0;
uint8_t* s_lastFlip = nullptr;
uint8_t  s_lastOld = 0, s_lastNew = 0;
bool     s_pending = false;
char     s_stats[256];

void FindGlobalRegions() {
    if (s_nGlob) return;
    uint8_t* img = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(img);
    auto* nt  = reinterpret_cast<IMAGE_NT_HEADERS32*>(img + dos->e_lfanew);
    IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections && s_nGlob < 4; ++i) {
        if (!(sec[i].Characteristics & IMAGE_SCN_MEM_WRITE)) continue;
        s_glob[s_nGlob].base = img + sec[i].VirtualAddress;
        s_glob[s_nGlob].len  = sec[i].Misc.VirtualSize;
        s_globTotal += sec[i].Misc.VirtualSize;
        ++s_nGlob;
    }
    s_globPre = static_cast<uint8_t*>(VirtualAlloc(nullptr, s_globTotal, MEM_COMMIT, PAGE_READWRITE));
    s_noise   = static_cast<uint8_t*>(VirtualAlloc(nullptr, s_globTotal, MEM_COMMIT, PAGE_READWRITE));
}

void SnapGlobals(uint8_t* dst) {
    for (int i = 0; i < s_nGlob; ++i) {
        memcpy(dst, s_glob[i].base, s_glob[i].len);
        dst += s_glob[i].len;
    }
}

// Accumulate every byte that changes with nothing called (the game's own writes).
void Calibrate() {
    uint8_t* a = s_globPre;
    SnapGlobals(a);
    for (int k = 0; k < kCalSamples; ++k) {
        Sleep(20);
        size_t off = 0;
        for (int i = 0; i < s_nGlob; ++i) {
            const uint8_t* cur = s_glob[i].base;
            for (size_t j = 0; j < s_glob[i].len; ++j)
                if (cur[j] != a[off + j]) { s_noise[off + j] = 1; a[off + j] = cur[j]; }
            off += s_glob[i].len;
        }
    }
}

bool InImage(uintptr_t p) {
    uintptr_t img = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(img + reinterpret_cast<IMAGE_DOS_HEADER*>(img)->e_lfanew);
    return p >= img && p < img + nt->OptionalHeader.SizeOfImage;
}

size_t WritableSpan(uintptr_t p) {
    if (p < 0x10000) return 0;
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(reinterpret_cast<void*>(p), &mbi, sizeof mbi)) return 0;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return 0;
    const DWORD w = PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    if (!(mbi.Protect & w)) return 0;
    uintptr_t end = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    size_t n = end - p;
    return n < kArgWindow ? n : kArgWindow;
}

void AddWindow(uintptr_t p) {
    if (s_nWin >= kMaxWins || InImage(p)) return;
    uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);   // NT_TIB StackLimit / StackBase
    if (p >= lo && p < hi) return;
    for (int i = 0; i < s_nWin; ++i)
        if (reinterpret_cast<uintptr_t>(s_win[i]) == p) return;
    size_t n = WritableSpan(p);
    if (!n) return;
    s_win[s_nWin] = reinterpret_cast<uint8_t*>(p);
    s_winLen[s_nWin] = n;
    memcpy(s_winPre[s_nWin], s_win[s_nWin], n);
    ++s_nWin;
}

bool IsField() { return g_ncCls == kFieldFirst || g_ncCls == kFieldLast || g_ncCls == kFieldNone; }

// Freeze the game's own threads (Win32 start address inside MASHED.exe) for the span Pre-snapshot ..
// Post-diff, so a byte the running game writes in that window is never taken for the port's write.
// Frida's and the drivers' threads keep running. A frozen thread holding a lock the port needs makes
// the call hang: the runner then times out to ERROR, never to a verdict.
HANDLE   s_game[64];                 // opened once per Arm, so Pre only pays SuspendThread
int      s_nGame = 0;
unsigned s_lastFrozen = 0;
typedef LONG(NTAPI* NtQit_t)(HANDLE, int, PVOID, ULONG, PULONG);

void OpenGameThreads() {
    for (int i = 0; i < s_nGame; ++i) CloseHandle(s_game[i]);
    s_nGame = 0;
    static NtQit_t qit = reinterpret_cast<NtQit_t>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationThread"));
    if (!qit) return;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    THREADENTRY32 te;
    te.dwSize = sizeof te;
    const DWORD pid = GetCurrentProcessId(), self = GetCurrentThreadId();
    for (BOOL ok = Thread32First(snap, &te); ok && s_nGame < 64; ok = Thread32Next(snap, &te)) {
        if (te.th32OwnerProcessID != pid || te.th32ThreadID == self) continue;
        HANDLE h = OpenThread(THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT, FALSE,
                              te.th32ThreadID);
        if (!h) continue;
        uintptr_t start = 0;
        if (qit(h, 9 /* ThreadQuerySetWin32StartAddress */, &start, sizeof start, nullptr) < 0 || !InImage(start)) {
            CloseHandle(h);
            continue;
        }
        s_game[s_nGame++] = h;
    }
    CloseHandle(snap);
}

void FreezeGameThreads() {
    unsigned n = 0;
    for (int i = 0; i < s_nGame; ++i)
        if (SuspendThread(s_game[i]) != static_cast<DWORD>(-1)) {
            CONTEXT c;
            c.ContextFlags = CONTEXT_CONTROL;
            GetThreadContext(s_game[i], &c);    // SuspendThread is asynchronous; this waits for it to land
            ++n;
        }
    s_lastFrozen = n;
}

void ThawGameThreads() {
    for (int i = 0; i < s_nGame; ++i) ResumeThread(s_game[i]);
}

}  // namespace

// regs = [EFLAGS][EDI][ESI][EBP][ESP][EBX][EDX][ECX][EAX] (pushad then pushfd);
// the saved ESP points at stack arg 0 (the return address was already popped).
extern "C" void __cdecl NegCtlPre(const uint32_t* regs) {
    ++s_calls;
    if (g_ncCls == kHeapExtra) {
        reinterpret_cast<void*(__cdecl*)(size_t)>(0x004a45fb)(0x20);   // game CRT _malloc, leaked
    } else if (g_ncCls == kImportExtra) {
        volatile DWORD t = GetTickCount();
        (void)t;
    } else if (IsField()) {
        FreezeGameThreads();
        s_nWin = 0;
        const uint32_t* args = reinterpret_cast<const uint32_t*>(regs[4]);
        for (int i = 0; i < 8; ++i) {
            if (WritableSpan(reinterpret_cast<uintptr_t>(args + i)) < 4) break;
            AddWindow(args[i]);
        }
        const int order[6] = {7, 8, 6, 2, 1, 5};   // ECX, EAX, EDX, ESI, EDI, EBX
        for (int r : order) AddWindow(regs[r]);
        SnapGlobals(s_globPre);
    }
}

extern "C" void __cdecl NegCtlPost() {
    if (!IsField()) return;
    uint8_t* first = nullptr;
    uint8_t* last  = nullptr;
    unsigned changed = 0;
    for (int i = 0; i < s_nWin; ++i)
        for (size_t j = 0; j < s_winLen[i]; ++j)
            if (s_win[i][j] != s_winPre[i][j]) {
                ++changed;
                if (!first) first = s_win[i] + j;
                last = s_win[i] + j;
            }
    size_t off = 0;
    for (int i = 0; i < s_nGlob; ++i) {
        const uint8_t* cur = s_glob[i].base;
        for (size_t j = 0; j < s_glob[i].len; ++j)
            if (cur[j] != s_globPre[off + j] && !s_noise[off + j]) {
                ++changed;
                if (!first) first = s_glob[i].base + j;
                last = s_glob[i].base + j;
            }
        off += s_glob[i].len;
    }
    s_lastChanged = changed;
    ThawGameThreads();
    uint8_t* hit = (g_ncCls == kFieldFirst) ? first : last;
    if (!hit || g_ncCls == kFieldNone) return;     // field_none: the freeze/snapshot control, never flips
    s_lastOld = *hit;
    s_lastNew = static_cast<uint8_t>(s_lastOld ^ 0x5A);
    *hit = s_lastNew;
    s_lastFlip = hit;
    s_pending = true;
    ++s_flips;
}

extern "C" __declspec(naked) void NegCtl_Entry() {
    __asm {
        pop  dword ptr [g_ncRet]
        cmp  dword ptr [g_ncCls], 1
        je   do_null
        pushad
        pushfd
        push esp
        call NegCtlPre
        add  esp, 4
        popfd
        popad
        call dword ptr [g_ncTarget]
        pushad
        pushfd
        call NegCtlPost
        popfd
        popad
        cmp  dword ptr [g_ncCls], 2
        jne  not_ret_int
        xor  eax, 1
    not_ret_int:
        cmp  dword ptr [g_ncCls], 3
        jne  done
        fadd st(0), st(0)
        fadd dword ptr [g_ncThreeQuarters]
    done:
        push dword ptr [g_ncRet]
        ret
    do_null:
        cmp  dword ptr [g_ncFloatRet], 0
        je   null_int
        fldz
    null_int:
        xor  eax, eax
        xor  edx, edx
        add  esp, dword ptr [g_ncStdcallBytes]
        jmp  dword ptr [g_ncRet]
    }
}

// Arm the forwarder for one run. Returns NegCtl_Entry, or NULL when MASHED_NEGCTL != 1
// or the class is unknown. Undoes the previous run's flip first.
extern "C" __declspec(dllexport) void* __cdecl NegCtl_Arm(void* target, int cls, unsigned stdcallBytes, int floatRet) {
    char env[8] = {0};
    if (!GetEnvironmentVariableA("MASHED_NEGCTL", env, sizeof env) || strcmp(env, "1") != 0) return nullptr;
    if (cls < kNull || cls > kFieldNone || !target) return nullptr;
    if (s_pending && s_lastFlip && *s_lastFlip == s_lastNew) *s_lastFlip = s_lastOld;
    s_pending = false;
    g_ncTarget = target;
    g_ncCls = cls;
    g_ncStdcallBytes = stdcallBytes;
    g_ncFloatRet = floatRet;
    s_calls = s_flips = s_lastChanged = 0;
    s_lastFlip = nullptr;
    if (cls == kFieldFirst || cls == kFieldLast || cls == kFieldNone) {
        FindGlobalRegions();
        Calibrate();
        OpenGameThreads();
    }
    return reinterpret_cast<void*>(&NegCtl_Entry);
}

// Undo the last field flip (if the byte still holds the flipped value) and disarm. run_negctl.py calls this after
// every field job so a warm process never carries a corrupted byte into the next hook's real/baseline run.
extern "C" __declspec(dllexport) int __cdecl NegCtl_Restore() {
    int undone = 0;
    if (s_pending && s_lastFlip && *s_lastFlip == s_lastNew) { *s_lastFlip = s_lastOld; undone = 1; }
    s_pending = false;
    g_ncCls = 0;
    return undone;
}

// "calls=N flips=N changed=N flip=0xADDR noise=N" for the runner's applicability check.
extern "C" __declspec(dllexport) const char* __cdecl NegCtl_Stats() {
    size_t noise = 0;
    if (s_noise)
        for (size_t i = 0; i < s_globTotal; ++i) noise += s_noise[i];
    _snprintf_s(s_stats, sizeof s_stats, _TRUNCATE, "calls=%u flips=%u changed=%u flip=0x%08x noise=%u frozen=%u",
                s_calls, s_flips, s_lastChanged, static_cast<unsigned>(reinterpret_cast<uintptr_t>(s_lastFlip)),
                static_cast<unsigned>(noise), s_lastFrozen);
    return s_stats;
}
