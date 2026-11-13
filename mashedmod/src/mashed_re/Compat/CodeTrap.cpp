// RVA-LINT: ORIGTOP-ONLY
// Mashed RE - trap on the original's code range (IMG-3-TEXT-TRAP, decision L-149 (a) step 3, Mariano 2026-11-13).
// Exe only. Spec: verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md section 4.
//
// The image (IMG-2, Compat/PortStateImage.cpp) leaves every code pointer it does not bind at its original value, and
// the original's .text range of the B17 pad holds zeros. A call through such a pointer used to run pad bytes and fault
// far from the cause. With MASHED_CODE_TRAP=1 (default OFF, MASHED_ORIG_TOP path only) the range 0x00401000..0x005cbfff
// (.text 0x00401000..0x005ca8e0 and _rwcseg 0x005cb000..0x005cb451, page rounded, 459 pages; .rdata starts at
// 0x005cc000) is PAGE_NOACCESS, and a vectored handler reports each fault in it as one "[code-trap]" line:
//   kind execute | read | write, the original VA, and
//   execute: the return address [esp] and, when the call site before it decodes (FF 15 abs, FF /2 with a register
//            base, FF /2 through a register loaded by A1 / 8B just before, E8 rel32), the slot the call read the target
//            from, checked against memory (the slot must still hold the target);
//   read / write: Eip.
// Addresses are module+offset (mashed_re.exe+0x..., read against mashed_re.map); names are joined offline by
// re/tools/image/code_trap_report.py.
// An execute AV at an address below 0x10000 is reported too (kind execute-null): the image zeroes the original IAT
// 0x005cc000..0x005cc2e8, so a call through an IAT slot lands there (spec section 3).
//
// Mode 1 then calls ExitProcess(<original VA>) (the exit convention of PortTopCodePtr.cpp CodePtrStop). Mode 2 (a
// diagnostic for listing the unbound pointers one boot reaches) logs each new (target, return address) pair once and,
// for a decoded call, simulates the return: eax = 0, Eip = [esp], esp + 4. A callee that pops its own arguments
// (stdcall / thiscall with arguments) leaves esp off by its argument bytes after that, so mode 2 reports are exact
// only up to the first such call. Read / write traps and undecoded execute traps exit in both modes.
//
// The B16 thunk installer (StandaloneRvaThunks.cpp) is the only writer into this range in the tree: it calls
// CodeTrap_AllowPage per thunk page (RWX, 0xCC-filled once) and clips its granule mapping to stay out of the range.

#include "CodeTrap.h"

#include <windows.h>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

extern "C" char g_b17_low_arena_pad[0x00A00000];   // exe_main.cpp B17: the original-address pad

namespace {

constexpr std::uintptr_t kLo = 0x00401000u;          // .text start
constexpr std::uintptr_t kHi = 0x005cc000u;          // .rdata start (end of _rwcseg 0x005cb451, page rounded)
constexpr std::uintptr_t kPages = (kHi - kLo) >> 12; // 459

int  s_mode = -1;
bool s_armed = false;
unsigned char s_carved[(kPages + 7) / 8];

const char* s_logPath = "mashed_re.log";
char s_pend[8192];
std::size_t s_pendLen = 0;
bool s_hold = true;
unsigned s_reports = 0;

void Emit(const char* line) {
    if (s_hold) {
        const int m = std::snprintf(s_pend + s_pendLen, sizeof(s_pend) - s_pendLen, "[code-trap] %s\n", line);
        if (m > 0 && s_pendLen + static_cast<std::size_t>(m) < sizeof(s_pend)) s_pendLen += static_cast<std::size_t>(m);
        return;
    }
    if (std::FILE* f = std::fopen(s_logPath, "a")) {
        std::fprintf(f, "[code-trap] %s\n", line);
        std::fclose(f);
    }
}

void TrapLog(const char* fmt, ...) {
    char line[700];
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    if (n >= 0) Emit(line);
}

// Before an exit: the held lines go out too (the process may end before the exe's log truncate).
void FlushHeld() {
    if (!s_hold) return;
    s_hold = false;
    if (!s_pendLen) return;
    if (std::FILE* f = std::fopen(s_logPath, "a")) {
        std::fwrite(s_pend, 1, s_pendLen, f);
        std::fclose(f);
    }
    s_pendLen = 0;
}

bool Readable(std::uintptr_t a, std::size_t n) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(reinterpret_cast<LPCVOID>(a), &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT) return false;
    const DWORD p = mbi.Protect & 0xFFu;
    if (p == PAGE_NOACCESS || p == PAGE_EXECUTE || (mbi.Protect & PAGE_GUARD)) return false;
    const std::uintptr_t end = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    return a + n <= end || Readable(end, a + n - end);
}

bool ReadU32(std::uintptr_t a, std::uint32_t* out) {
    if (!Readable(a, 4)) return false;
    std::memcpy(out, reinterpret_cast<const void*>(a), 4);
    return true;
}

void ModOff(std::uintptr_t a, char* buf, std::size_t n) {
    HMODULE hm = nullptr;
    char path[MAX_PATH];
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCSTR>(a), &hm) && hm && GetModuleFileNameA(hm, path, MAX_PATH)) {
        const char* s = std::strrchr(path, '\\');
        std::snprintf(buf, n, "%s+0x%X", s ? s + 1 : path, static_cast<unsigned>(a - reinterpret_cast<std::uintptr_t>(hm)));
        return;
    }
    std::snprintf(buf, n, "?");
}

std::uint32_t Reg(const CONTEXT* c, unsigned r) {
    switch (r & 7u) {
    case 0: return c->Eax; case 1: return c->Ecx; case 2: return c->Edx; case 3: return c->Ebx;
    case 4: return c->Esp + 4u;   // esp at the call site: the call pushed the return address
    case 5: return c->Ebp; case 6: return c->Esi; default: return c->Edi;
    }
}
const char* RegName(unsigned r) {
    static const char* const k[8] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
    return k[r & 7u];
}

// SIB address with the register file at the call site. mod 0 with base 5 = disp32 only.
std::uint32_t SibAddr(const CONTEXT* c, unsigned char sib, unsigned mod, std::int32_t disp) {
    const unsigned scale = 1u << (sib >> 6), idx = (sib >> 3) & 7u, base = sib & 7u;
    std::uint32_t a = static_cast<std::uint32_t>(disp);
    if (idx != 4) a += Reg(c, idx) * scale;
    if (!(mod == 0 && base == 5)) a += Reg(c, base);
    return a;
}

struct Site { char form[48]; std::uint32_t slot; bool hasSlot; bool decoded; };

// Decodes the call instruction that ends at ret and computes the slot it read the target from. A form is accepted only
// when its slot (or register) still holds the target, so overlapping byte patterns cannot name a wrong slot.
Site DecodeCall(const CONTEXT* c, std::uintptr_t ret, std::uint32_t target) {
    Site s{};
    unsigned char b[32];
    if (ret < 32 || !Readable(ret - 32, 32)) return s;
    std::memcpy(b, reinterpret_cast<const void*>(ret - 32), 32);
    auto at = [&](int back) { return b[32 - back]; };   // at(1) = the byte just before ret
    auto d32 = [&](int back) { std::int32_t v; std::memcpy(&v, &b[32 - back], 4); return v; };
    auto ok = [&](std::uint32_t slot) { std::uint32_t v = 0; return ReadU32(slot, &v) && v == target; };
    // E8 rel32
    if (at(5) == 0xE8 && static_cast<std::uint32_t>(ret + d32(4)) == target) {
        std::snprintf(s.form, sizeof(s.form), "call rel32");
        s.decoded = true;
        return s;
    }
    // FF 15 abs32
    if (at(6) == 0xFF && at(5) == 0x15 && ok(static_cast<std::uint32_t>(d32(4)))) {
        s.slot = static_cast<std::uint32_t>(d32(4)); s.hasSlot = s.decoded = true;
        std::snprintf(s.form, sizeof(s.form), "call [abs32]");
        return s;
    }
    // FF /2 mod 10 disp32 (rm != 4), FF /2 mod 10 SIB disp32
    if (at(6) == 0xFF && (at(5) & 0xF8) == 0x90 && (at(5) & 7) != 4 &&
        ok(Reg(c, at(5)) + static_cast<std::uint32_t>(d32(4)))) {
        s.slot = Reg(c, at(5)) + static_cast<std::uint32_t>(d32(4)); s.hasSlot = s.decoded = true;
        std::snprintf(s.form, sizeof(s.form), "call [%s+disp32]", RegName(at(5)));
        return s;
    }
    if (at(7) == 0xFF && at(6) == 0x94 && ok(SibAddr(c, at(5), 2, d32(4)))) {
        s.slot = SibAddr(c, at(5), 2, d32(4)); s.hasSlot = s.decoded = true;
        std::snprintf(s.form, sizeof(s.form), "call [sib+disp32]");
        return s;
    }
    // FF /2 mod 01 disp8, SIB disp8
    if (at(3) == 0xFF && (at(2) & 0xF8) == 0x50 && (at(2) & 7) != 4 &&
        ok(Reg(c, at(2)) + static_cast<std::uint32_t>(static_cast<std::int8_t>(at(1))))) {
        s.slot = Reg(c, at(2)) + static_cast<std::uint32_t>(static_cast<std::int8_t>(at(1))); s.hasSlot = s.decoded = true;
        std::snprintf(s.form, sizeof(s.form), "call [%s+disp8]", RegName(at(2)));
        return s;
    }
    if (at(4) == 0xFF && at(3) == 0x54 && ok(SibAddr(c, at(2), 1, static_cast<std::int8_t>(at(1))))) {
        s.slot = SibAddr(c, at(2), 1, static_cast<std::int8_t>(at(1))); s.hasSlot = s.decoded = true;
        std::snprintf(s.form, sizeof(s.form), "call [sib+disp8]");
        return s;
    }
    // FF /2 mod 00 (rm != 4, 5), SIB mod 00
    if (at(2) == 0xFF && (at(1) & 0xF8) == 0x10 && (at(1) & 7) != 4 && (at(1) & 7) != 5 && ok(Reg(c, at(1)))) {
        s.slot = Reg(c, at(1)); s.hasSlot = s.decoded = true;
        std::snprintf(s.form, sizeof(s.form), "call [%s]", RegName(at(1)));
        return s;
    }
    if (at(3) == 0xFF && at(2) == 0x14 && (at(1) & 7) != 5 && ok(SibAddr(c, at(1), 0, 0))) {
        s.slot = SibAddr(c, at(1), 0, 0); s.hasSlot = s.decoded = true;
        std::snprintf(s.form, sizeof(s.form), "call [sib]");
        return s;
    }
    // FF D0+r: call reg. The slot is named when the instruction just before loads that register from memory.
    if (at(2) == 0xFF && (at(1) & 0xF8) == 0xD0 && Reg(c, at(1)) == target) {
        const unsigned r = at(1) & 7u;
        s.decoded = true;
        std::snprintf(s.form, sizeof(s.form), "call %s", RegName(r));
        // The load of r ends `e` bytes before ret; `test r,r` (85 /r) and a short jcc between it and the call are skipped.
        int e = 2;
        if (at(e + 2) >= 0x70 && at(e + 2) <= 0x7F) e += 2;
        if (at(e + 2) == 0x85 && at(e + 1) == (0xC0 | (r << 3) | r)) e += 2;
        const char* skip = e > 2 ? "test/jcc; " : "";
        // mov r,[abs32] ending at back-offset `end`: A1 imm32 (eax only) or 8B 05+r*8 imm32. Returns the length or 0.
        auto absLoad = [&](int end, std::uint32_t* abs) -> int {
            if (end + 5 <= 32 && r == 0 && at(end + 5) == 0xA1) { *abs = static_cast<std::uint32_t>(d32(end + 4)); return 5; }
            if (end + 6 <= 32 && at(end + 6) == 0x8B && at(end + 5) == (0x05 | (r << 3))) {
                *abs = static_cast<std::uint32_t>(d32(end + 4)); return 6;
            }
            return 0;
        };
        // Register x at the load: unchanged since then unless x == r; r is recovered when the instruction before the
        // load is mov r,[abs32] (the dword is read now, so it must not have changed in between).
        bool regOk = true;
        auto regAt = [&](unsigned x, int loadEnd) -> std::uint32_t {
            if ((x & 7u) != r) return Reg(c, x);
            std::uint32_t abs = 0, v = 0;
            if (absLoad(loadEnd, &abs) && ReadU32(abs, &v)) return v;
            regOk = false;
            return 0;
        };
        std::uint32_t abs = 0;
        std::uint32_t slot = 0;
        bool have = false;
        const char* lf = "";
        if (absLoad(e, &abs) && ok(abs)) {
            slot = abs; have = true; lf = "mov r,[abs32]";
        } else {
            for (int len = 2; len <= 7 && !have; ++len) {   // 8B modrm [sib] [disp]
                const int st = e + len;                         // back-offset of the 8B byte
                if (st > 32 || at(st) != 0x8B || ((at(st - 1) >> 3) & 7u) != r) continue;
                const unsigned char m = at(st - 1);
                const unsigned mod = m >> 6, rm = m & 7u;
                const bool sib = rm == 4 && mod != 3;
                const int want = 2 + (sib ? 1 : 0) + (mod == 1 ? 1 : mod == 2 ? 4 : (mod == 0 && !sib && rm == 5) ? 4 : 0);
                if (mod == 3 || want != len || (mod == 0 && !sib && rm == 5)) continue;
                std::int32_t disp = mod == 1 ? static_cast<std::int8_t>(at(e + 1)) : mod == 2 ? d32(e + 4) : 0;
                regOk = true;
                std::uint32_t a = static_cast<std::uint32_t>(disp);
                if (sib) {
                    const unsigned char sb = at(st - 2);
                    const unsigned scale = 1u << (sb >> 6), idx = (sb >> 3) & 7u, base = sb & 7u;
                    if (mod == 0 && base == 5) continue;   // [idx*s+disp32] form not handled
                    if (idx != 4) a += regAt(idx, st) * scale;
                    a += regAt(base, st);
                    lf = "mov r,[sib]";
                } else {
                    a += regAt(rm, st);
                    lf = "mov r,[reg+disp]";
                }
                if (regOk && ok(a)) { slot = a; have = true; }
            }
        }
        if (have && ok(slot)) {
            s.slot = slot; s.hasSlot = true;
            std::snprintf(s.form, sizeof(s.form), "%s; %scall %s", lf, skip, RegName(r));
        }
        return s;
    }
    return s;
}

struct Seen { std::uint32_t target, ret; };
Seen s_seen[256];
unsigned s_seenN = 0;
bool SeenBefore(std::uint32_t target, std::uint32_t ret) {
    for (unsigned i = 0; i < s_seenN; ++i)
        if (s_seen[i].target == target && s_seen[i].ret == ret) return true;
    if (s_seenN < 256) s_seen[s_seenN++] = {target, ret};
    return false;
}

// Self-test probe state (CodeTrap_Arm): while active the handler records instead of reporting.
struct Probe { bool active; char kind[16]; std::uint32_t va, slot, ret; bool hasSlot; };
Probe s_probe;
typedef void(__cdecl* ProbeFn)(void);
ProbeFn volatile s_probeSlot = nullptr;

[[noreturn]] void Stop(std::uint32_t va) {
    FlushHeld();
    ExitProcess(va);
}

LONG CALLBACK CodeTrapVeh(PEXCEPTION_POINTERS ep) {
    if (!s_armed) return EXCEPTION_CONTINUE_SEARCH;
    const EXCEPTION_RECORD* er = ep->ExceptionRecord;
    CONTEXT* c = ep->ContextRecord;
    const char* kind = nullptr;
    std::uintptr_t va = 0;
    if (er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && er->NumberParameters >= 2) {
        va = static_cast<std::uintptr_t>(er->ExceptionInformation[1]);
        const ULONG_PTR op = er->ExceptionInformation[0];
        if (va >= kLo && va < kHi) kind = op == 8 ? "execute" : op == 1 ? "write" : "read";
        else if (op == 8 && va < 0x10000u) kind = "execute-null";
    } else if (er->ExceptionCode == EXCEPTION_BREAKPOINT) {
        va = reinterpret_cast<std::uintptr_t>(er->ExceptionAddress);
        if (va >= kLo && va < kHi) kind = "execute";   // a 0xCC byte of a carved page
    }
    if (!kind) return EXCEPTION_CONTINUE_SEARCH;
    if (s_probe.active) {   // self-test probe: record what the report would say, let the probe's __except take it
        std::uint32_t r = 0;
        const bool hr = kind[0] == 'e' && ReadU32(c->Esp, &r);
        const Site st = hr ? DecodeCall(c, r, static_cast<std::uint32_t>(va)) : Site{};
        std::snprintf(s_probe.kind, sizeof(s_probe.kind), "%s", kind);
        s_probe.va = static_cast<std::uint32_t>(va);
        s_probe.slot = st.slot;
        s_probe.hasSlot = st.hasSlot;
        s_probe.ret = r;
        return EXCEPTION_CONTINUE_SEARCH;
    }

    char eipBuf[96], retBuf[96];
    ModOff(c->Eip, eipBuf, sizeof(eipBuf));
    if (kind[0] != 'e') {   // read / write: always fatal
        ++s_reports;
        TrapLog("%s original 0x%08X by eip %08X (%s); exit 0x%08X", kind, static_cast<unsigned>(va),
                static_cast<unsigned>(c->Eip), eipBuf, static_cast<unsigned>(va));
        Stop(static_cast<std::uint32_t>(va));
    }
    std::uint32_t ret = 0;
    const bool haveRet = ReadU32(c->Esp, &ret);
    ModOff(ret, retBuf, sizeof(retBuf));
    const Site site = haveRet ? DecodeCall(c, ret, static_cast<std::uint32_t>(va)) : Site{};
    char slotBuf[64];
    if (site.hasSlot) std::snprintf(slotBuf, sizeof(slotBuf), "slot 0x%08X", site.slot);
    else std::snprintf(slotBuf, sizeof(slotBuf), "slot ?");
    const bool cont = s_mode == 2 && site.decoded;
    if (!cont || !SeenBefore(static_cast<std::uint32_t>(va), ret)) {
        ++s_reports;
        TrapLog("%s original 0x%08X %s via \"%s\" ret %08X (%s) esp %08X; %s", kind, static_cast<unsigned>(va), slotBuf,
                site.decoded ? site.form : "no call site decoded", static_cast<unsigned>(ret), retBuf,
                static_cast<unsigned>(c->Esp),
                cont ? "mode 2: return simulated (eax 0)" : "exit");
    }
    if (!cont) Stop(static_cast<std::uint32_t>(va));
    c->Eax = 0;
    c->Eip = ret;
    c->Esp += 4;
    return EXCEPTION_CONTINUE_EXECUTION;
}

// One probe: kind 0 read, 1 write, 2 call through s_probeSlot. True when the exception reached the probe's __except.
bool RunProbe(int what, std::uintptr_t addr) {
    std::memset(&s_probe, 0, sizeof(s_probe));
    s_probe.active = true;
    bool faulted = false;
    __try {
        if (what == 0) { volatile std::uint32_t v = *reinterpret_cast<volatile std::uint32_t*>(addr); (void)v; }
        else if (what == 1) { *reinterpret_cast<volatile std::uint32_t*>(addr) = 0; }
        else { s_probeSlot = reinterpret_cast<ProbeFn>(addr); s_probeSlot(); }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        faulted = true;
    }
    s_probe.active = false;
    s_probeSlot = nullptr;
    return faulted;
}

bool InExe(std::uintptr_t a) {
    const std::uintptr_t img = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    HMODULE hm = nullptr;
    return GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              reinterpret_cast<LPCSTR>(a), &hm) && reinterpret_cast<std::uintptr_t>(hm) == img;
}

// In-process self-test after arming (no game code has run): every page protected, and one probe per report kind
// through the real handler. The breakpoint probe carves the last page 0x005cb000 and restores it (zeros, NOACCESS).
void SelfTest() {
    unsigned fail = 0, checks = 0;
    unsigned noacc = 0;
    for (std::uintptr_t p = kLo; p < kHi; p += 0x1000u) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<LPCVOID>(p), &mbi, sizeof(mbi)) && mbi.State == MEM_COMMIT &&
            mbi.Protect == PAGE_NOACCESS) ++noacc;
    }
    ++checks; if (noacc != kPages) ++fail;
    TrapLog("self-test pages PAGE_NOACCESS %u/%u", noacc, static_cast<unsigned>(kPages));

    const std::uintptr_t slot = reinterpret_cast<std::uintptr_t>(&s_probeSlot);
    struct Case { const char* name; int what; std::uintptr_t addr; const char* kind; bool wantSlot; };
    const Case cases[] = {
        {"read", 0, 0x00500000u, "read", false},
        {"write", 1, 0x00500004u, "write", false},
        {"execute", 2, 0x00401000u, "execute", true},
        {"execute-null", 2, 0x00000000u, "execute-null", true},
    };
    for (const Case& k : cases) {
        const bool f = RunProbe(k.what, k.addr);
        const bool ok = f && std::strcmp(s_probe.kind, k.kind) == 0 && s_probe.va == k.addr &&
                        (!k.wantSlot || (s_probe.hasSlot && s_probe.slot == slot && InExe(s_probe.ret)));
        ++checks; if (!ok) ++fail;
        TrapLog("self-test %-12s 0x%08X: faulted %d kind %s va 0x%08X slot %s0x%08X (want 0x%08X) ret %08X %s", k.name,
                static_cast<unsigned>(k.addr), f, s_probe.kind[0] ? s_probe.kind : "-", s_probe.va,
                s_probe.hasSlot ? "" : "none ", s_probe.slot, k.wantSlot ? static_cast<unsigned>(slot) : 0u, s_probe.ret,
                ok ? "ok" : "FAIL");
    }
    // Breakpoint: a carved page reports a call into its 0xCC bytes as execute.
    const std::uintptr_t pg = kHi - 0x1000u;
    bool ok = mashed_re::Compat::CodeTrap_AllowPage(pg);
    const bool f = ok && RunProbe(2, pg + 0x10u);
    ok = ok && f && std::strcmp(s_probe.kind, "execute") == 0 && s_probe.va == pg + 0x10u && s_probe.hasSlot &&
         s_probe.slot == slot;
    const std::uintptr_t i = (pg - kLo) >> 12;
    std::memset(reinterpret_cast<void*>(pg), 0, 0x1000);
    s_carved[i >> 3] &= static_cast<unsigned char>(~(1u << (i & 7)));
    DWORD old = 0;
    const bool back = VirtualProtect(reinterpret_cast<LPVOID>(pg), 0x1000, PAGE_NOACCESS, &old) != 0;
    ++checks; if (!ok || !back) ++fail;
    TrapLog("self-test breakpoint   0x%08X: faulted %d kind %s va 0x%08X slot %s0x%08X; page restored %d %s",
            static_cast<unsigned>(pg + 0x10u), f, s_probe.kind[0] ? s_probe.kind : "-", s_probe.va,
            s_probe.hasSlot ? "" : "none ", s_probe.slot, back, ok && back ? "ok" : "FAIL");
    TrapLog("self-test %s (%u of %u checks failed)", fail ? "FAIL" : "PASS", fail, checks);
}

} // namespace

namespace mashed_re {
namespace Compat {

int CodeTrap_Mode() {
    if (s_mode < 0) {
        char v[8] = {};
        s_mode = 0;
        if (GetEnvironmentVariableA("MASHED_CODE_TRAP", v, sizeof(v)) > 0 && v[0] && v[0] != '0')
            s_mode = v[0] == '2' ? 2 : 1;
    }
    return s_mode;
}

bool CodeTrap_Armed() { return s_armed; }

bool CodeTrap_InRange(std::uintptr_t addr) { return s_armed && addr >= kLo && addr < kHi; }

bool CodeTrap_Arm() {
    if (s_armed || CodeTrap_Mode() == 0) return s_armed;
    const std::uintptr_t lo = reinterpret_cast<std::uintptr_t>(&g_b17_low_arena_pad[0]);
    const std::uintptr_t hi = lo + sizeof(g_b17_low_arena_pad);
    if (kLo < lo || kHi > hi) {
        TrapLog("NOT ARMED: range 0x%08X..0x%08X is outside the B17 pad 0x%08X..0x%08X", static_cast<unsigned>(kLo),
                static_cast<unsigned>(kHi), static_cast<unsigned>(lo), static_cast<unsigned>(hi));
        return false;
    }
    if (!AddVectoredExceptionHandler(1, CodeTrapVeh)) {
        TrapLog("NOT ARMED: AddVectoredExceptionHandler failed (%lu)", GetLastError());
        return false;
    }
    DWORD old = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(kLo), kHi - kLo, PAGE_NOACCESS, &old)) {
        TrapLog("NOT ARMED: VirtualProtect(0x%08X, 0x%X, PAGE_NOACCESS) failed (%lu)", static_cast<unsigned>(kLo),
                static_cast<unsigned>(kHi - kLo), GetLastError());
        return false;
    }
    s_armed = true;
    TrapLog("armed: 0x%08X..0x%08X (%u pages) PAGE_NOACCESS, was 0x%X; mode %d (%s)", static_cast<unsigned>(kLo),
            static_cast<unsigned>(kHi - 1), static_cast<unsigned>(kPages), static_cast<unsigned>(old), s_mode,
            s_mode == 2 ? "report every decoded call, simulate the return" : "report the first trap, exit");
    SelfTest();
    return true;
}

bool CodeTrap_AllowPage(std::uintptr_t page) {
    page &= ~static_cast<std::uintptr_t>(0xFFFu);
    if (!s_armed || page < kLo || page >= kHi) return false;
    const std::uintptr_t i = (page - kLo) >> 12;
    DWORD old = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(page), 0x1000, PAGE_EXECUTE_READWRITE, &old)) {
        TrapLog("carve 0x%08X: VirtualProtect failed (%lu)", static_cast<unsigned>(page), GetLastError());
        return false;
    }
    if (!(s_carved[i >> 3] & (1u << (i & 7)))) {
        s_carved[i >> 3] |= static_cast<unsigned char>(1u << (i & 7));
        std::memset(reinterpret_cast<void*>(page), 0xCC, 0x1000);
        TrapLog("carved page 0x%08X (RWX, 0xCC-filled) for a B16 thunk", static_cast<unsigned>(page));
    }
    return true;
}

void CodeTrap_Log(const char* logPath) {
    s_logPath = logPath;
    FlushHeld();
}

} // namespace Compat
} // namespace mashed_re
