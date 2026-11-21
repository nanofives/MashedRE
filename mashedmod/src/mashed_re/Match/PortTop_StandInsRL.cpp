// RVA-LINT: ORIGTOP-ONLY
// PORT-OT-RACE (L-111 step 3): the MASHED_OT_RACELOAD knob (default OFF, exe only, MASHED_ORIG_TOP path only) and the
// named stand-ins of the matched 0x0040d270 course load's subtree (Match/PortTop_0040d270.cpp). Each stand-in logs its
// first call and its argument and counts its calls. None of them is a body of its RVA. PREREG
// verify/ro_20261116/PREREG_PORT-OT-RACE.md, RESULT verify/ro_20261116/RESULT_PORT-OT-RACE.md.
//
// PortTop_Bound_0040d270 is what the matched 0x0040d440 (Match/PortTop_0040d440.cpp) calls:
//   knob off                       -> PortTop_StandIn05_0040d270 (Match/PortTop_StandIns05.cpp), as before this row
//   knob on, [0x005f2770] == 0     -> the same stand-in, logged: 0x0040d270 reads **([0x005f2770] + track * 4) and there
//                                     is no table without the file-backed .data image (MASHED_PORTSTATE_IMAGE)
//   knob on                        -> Orig_0040d270, the matched body
#ifdef MASHED_STANDALONE
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace {
int RaceLoadKnob() {
    static int s = -1;
    if (s < 0) {
        const char* const v = std::getenv("MASHED_OT_RACELOAD");
        s = (v && v[0] == '1') ? 1 : 0;
    }
    return s;
}
void RlLog(const char* fmt, unsigned a, unsigned b, unsigned c) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] ");
        std::fprintf(f, fmt, a, b, c);
        std::fprintf(f, " (PORT-OT-RACE)\n");
        std::fclose(f);
    }
}
unsigned U32(unsigned long a) { return *reinterpret_cast<const volatile unsigned*>(a); }
struct Count { const char* rva; const char* what; unsigned n; };
void Hit(Count& c, unsigned arg) {
    if (++c.n == 1) {
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva %s: stand-in (PORT-OT-RACE) %s, first call arg %08X (PORT-OT-RACE)\n", c.rva, c.what, arg);
            std::fclose(f);
        }
    }
}
Count s_d040 = {"0x0040d040", "201 B, 4 callees, the B path of 0x0040d270", 0};
Count s_d110 = {"0x0040d110", "320 B, 11 callees (0x0040cea0 and 0x00420420 C2)", 0};
// Checkpoint 3: also what the bound 0x004264d0 calls (lead decision (b)): the matched body faulted in RwCore_004c15c0+0x85
// (original 0x004c1645), [RwGlobals+0xbc] = 0 on the librw RwGlobals record. No RW core call here.
Count s_8c70 = {"0x00458c70", "136 B, callee 0x004c15c0 (RW core, not called)", 0};
Count s_6340 = {"0x00426340", "280 B, 10 callees (rule 5 path)", 0};
Count s_7800 = {"0x00407800", "C2, 19 callees (rule 5 path)", 0};
Count s_74a0 = {"0x004074a0", "167 B, callees 0x0047f290 0x00489c60 0x004e6e00", 0};
Count s_64d0 = {"0x004264d0", "351 B, 14 callees", 0};
Count s_1d90 = {"0x00411d90", "410 B, 11 callees (0x00482900 C3)", 0};
Count s_6e10 = {"0x00426e10", "the course load proper (piz mount, COURSE.LUA, LAPDATA.LUA, AI data), 723 B, 35 callees, 4 C2", 0};
Count s_cde0 = {"0x004ccde0", "skipped: list head 0x007d45cc is 0 (RW resource list not initialised on this path)", 0};
} // namespace

extern "C" {
void PortTop_StandIn05_0040d270(int table, int row);   // Match/PortTop_StandIns05.cpp
void Orig_0040d270(int param_1, int param_2);          // Match/PortTop_0040d270.cpp
int  RwCore_004ccde0(void);                            // RwCore/RwCore_004ccde0.cpp
unsigned g_PortTopRL_LoadCalls = 0;

void PortTop_Bound_0040d270(int table, int row) {
    if (!RaceLoadKnob()) { PortTop_StandIn05_0040d270(table, row); return; }
    if (table == 0) {
        RlLog("rva 0x0040d270: MASHED_OT_RACELOAD=1 but [0x005f2770] = %08X (no file-backed .data image): stand-in, row %u%c",
              static_cast<unsigned>(table), static_cast<unsigned>(row), ' ');
        PortTop_StandIn05_0040d270(table, row);
        return;
    }
    ++g_PortTopRL_LoadCalls;
    RlLog("rva 0x0040d270: MASHED_OT_RACELOAD=1, matched body Orig_0040d270(table %08X, row %u) entered, call %u", static_cast<unsigned>(table),
          static_cast<unsigned>(row), g_PortTopRL_LoadCalls);
    RlLog("rva 0x0040d270: before: DAT_0063ba8c = %u, DAT_007f0fd0 (rule) = %u, DAT_0080332c = %08X", U32(0x0063ba8cu), U32(0x007f0fd0u),
          U32(0x0080332cu));
    Orig_0040d270(table, row);
    RlLog("rva 0x0040d270: matched body returned: DAT_0063ba8c = %u, DAT_0080332c = %08X, DAT_00803324 = %08X", U32(0x0063ba8cu),
          U32(0x0080332cu), U32(0x00803324u));
    RlLog("rva 0x0040d270: stand-in calls 0x00426e10 %u, 0x004264d0 %u, 0x004074a0 %u", s_6e10.n, s_64d0.n, s_74a0.n);
    RlLog("rva 0x0040d270: stand-in calls 0x0040d110 %u, 0x00411d90 %u, 0x0040d040 %u", s_d110.n, s_1d90.n, s_d040.n);
    RlLog("rva 0x0040d270: stand-in calls 0x00426340 %u, 0x00407800 %u, 0x004ccde0 skipped %u", s_6340.n, s_7800.n, s_cde0.n);
}

int  PortTop_StandInRL_0040d040(int p1, int p2) { Hit(s_d040, static_cast<unsigned>(p2)); (void)p1; return 0; }
void PortTop_StandInRL_0040d110(int p1, int p2) { Hit(s_d110, static_cast<unsigned>(p2)); (void)p1; }
void PortTop_StandInRL_00458c70(void) { Hit(s_8c70, 0); }
void PortTop_StandInRL_00426340(int p0) { Hit(s_6340, static_cast<unsigned>(p0)); }
void PortTop_StandInRL_00407800(int p0) { Hit(s_7800, static_cast<unsigned>(p0)); }
void PortTop_StandInRL_004074a0(void) { Hit(s_74a0, 0); }
void PortTop_StandInRL_004264d0(int p0) { Hit(s_64d0, static_cast<unsigned>(p0)); }
void PortTop_StandInRL_00411d90(int p0) { Hit(s_1d90, static_cast<unsigned>(p0)); }
void PortTop_StandInRL_00426e10(unsigned p0) { Hit(s_6e10, p0); }
// Checkpoint 2: what the matched 0x0040d020 calls. The matched body of 0x00426e10 (Match/PortTop_00426e10.cpp) and its
// closure (verify/ro_20261116/rltools/plan_rl.json); a callee with no matched body is a named stop that exits with code = rva.
void PortTop_Bound_00426e10(int track);
void PortTop_RL_00426e10(int track) {
    RlLog("rva 0x00426e10: matched course load PortTop_Bound_00426e10(track %u) entered%c%c", static_cast<unsigned>(track), ' ', ' ');
    PortTop_Bound_00426e10(track);
    RlLog("rva 0x00426e10: matched course load returned: DAT_0066d704 = %u, DAT_0080332c = %08X, DAT_00656ee8 = %08X", U32(0x0066d704u),
          U32(0x0080332cu), U32(0x00656ee8u));
}
// Checkpoint 3: what the matched 0x0040d270 calls for 0x004264d0 (was the stand-in PortTop_StandInRL_004264d0, which stays
// defined and is no longer called). The matched body (Match/PortTop_004264d0.cpp, re/match/s06/004264d0.c) and its closure
// (verify/ro_20261116/rltools/plan_rl.json). The body returns early when 0x0042fe30 returns 0 (read here as DAT_0067ea74).
void PortTop_Bound_004264d0(int param_1);
// A fault inside the bound closure is logged (exe offset of the faulting instruction, access kind and address, the RW
// globals record and its +0xbc word) and then passed on (EXCEPTION_CONTINUE_SEARCH): the process still ends as before.
static int RlFaultFilter(EXCEPTION_POINTERS* ep) {
    const EXCEPTION_RECORD* er = ep->ExceptionRecord;
    const unsigned base = static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr)));
    const unsigned at = static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(er->ExceptionAddress));
    const unsigned kind = er->NumberParameters >= 2 ? static_cast<unsigned>(er->ExceptionInformation[0]) : 9u;
    const unsigned addr = er->NumberParameters >= 2 ? static_cast<unsigned>(er->ExceptionInformation[1]) : 0u;
    RlLog("rva 0x004264d0: fault code %08X at exe+0x%X, access kind %u", static_cast<unsigned>(er->ExceptionCode), at - base, kind);
    RlLog("rva 0x004264d0: fault address %08X, eax %08X, ecx %08X", addr, static_cast<unsigned>(ep->ContextRecord->Eax),
          static_cast<unsigned>(ep->ContextRecord->Ecx));
    const unsigned g = U32(0x007d3ff8u);
    RlLog("rva 0x004264d0: at the fault [0x007d3ff8] = %08X, [+0xbc] = %08X, edx %08X", g, g ? U32(g + 0xbcu) : 0u,
          static_cast<unsigned>(ep->ContextRecord->Edx));
    return EXCEPTION_CONTINUE_SEARCH;
}
static void RlCall004264d0(int param_1) {
    __try {
        PortTop_Bound_004264d0(param_1);
    } __except (RlFaultFilter(GetExceptionInformation())) {
    }
}
// RACE-RWGLOBALS-FRAMELIST: what the matched 0x0040d270 and the bound 0x004264d0 call for 0x00458c70. With the RW core
// dirty-frame list on the record ([[0x007d3ff8]+0xbc] != 0, LibRw/RwApiBind.cpp RaceLoadFrameOpen under this knob) the
// matched body runs (Match/PortTop_00458c70.cpp); with it 0 the counting stand-in above runs, logged (the RwCore_004c15c0
// write [[+0xbc]+4] would fault, RESULT_PORT-OT-RACE.md 7.2).
void PortTop_Bound_00458c70(void);
void* RwCore_004c07b0(void* instance, int offset, int size);   // RwCore/RwCore_004c07b0.cpp
static unsigned s_8c70Bound = 0;
// LibRw/RwApiBind.cpp (RaceLoadFrameOpen, MASHED_OT_RACELOAD only): the RW core frame-module open on the librw RwGlobals
// record, offset 0x164. A fault inside it is logged with its exe-relative address and passed on (that TU has no windows.h).
static int RlFrameOpenFilter(EXCEPTION_POINTERS* ep) {
    const EXCEPTION_RECORD* er = ep->ExceptionRecord;
    const unsigned base = static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr)));
    const unsigned at = static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(er->ExceptionAddress));
    RlLog("RwGlobals 0x004c07b0 call: fault code %08X at exe+0x%X, access kind %u", static_cast<unsigned>(er->ExceptionCode), at - base,
          er->NumberParameters >= 2 ? static_cast<unsigned>(er->ExceptionInformation[0]) : 9u);
    RlLog("RwGlobals 0x004c07b0 call: fault address %08X, eax %08X, ecx %08X",
          er->NumberParameters >= 2 ? static_cast<unsigned>(er->ExceptionInformation[1]) : 0u, static_cast<unsigned>(ep->ContextRecord->Eax),
          static_cast<unsigned>(ep->ContextRecord->Ecx));
    RlLog("RwGlobals 0x004c07b0 call: edx %08X, esi %08X, edi %08X", static_cast<unsigned>(ep->ContextRecord->Edx),
          static_cast<unsigned>(ep->ContextRecord->Esi), static_cast<unsigned>(ep->ContextRecord->Edi));
    return EXCEPTION_CONTINUE_SEARCH;
}
void* PortTop_RL_FrameOpenSeh(void* record) {
    __try {
        return RwCore_004c07b0(record, 0x164, 4);
    } __except (RlFrameOpenFilter(GetExceptionInformation())) {
        return nullptr;
    }
}
void PortTop_RL_00458c70(void) {
    const unsigned g = U32(0x007d3ff8u);
    const unsigned head = g ? U32(g + 0xbcu) : 0u;
    if (head == 0) {
        RlLog("rva 0x00458c70: [0x007d3ff8] = %08X, [+0xbc] = %08X: no RW core frame list, stand-in%c", g, head, ' ');
        PortTop_StandInRL_00458c70();
        return;
    }
    ++s_8c70Bound;
    if (s_8c70Bound == 1)
        RlLog("rva 0x00458c70: matched body PortTop_Bound_00458c70 entered, [0x007d3ff8] = %08X, [+0xbc] = %08X, DAT_0068b9a8 = %u", g,
              head, U32(0x0068b9a8u));
    PortTop_Bound_00458c70();
    if (s_8c70Bound == 1)
        RlLog("rva 0x00458c70: matched body returned, [+0xbc] = %08X, [+0xc0] = %08X, DAT_0068b9a8 = %u", U32(g + 0xbcu),
              U32(g + 0xc0u), U32(0x0068b9a8u));
}
void PortTop_RL_004264d0(int param_1) {
    RlLog("rva 0x004264d0: matched body PortTop_Bound_004264d0(%08X) entered, DAT_0067ea74 = %u, DAT_007d3ff8 = %08X",
          static_cast<unsigned>(param_1), U32(0x0067ea74u), U32(0x007d3ff8u));
    RlCall004264d0(param_1);
    RlLog("rva 0x004264d0: matched body returned%c%c%c", ' ', ' ', ' ');
}
// Guard for the RwCore_004ccde0 call: that body walks the list at 0x007d45cc until it returns to &0x007d45cc (re/match source, RwCore/RwCore_004ccde0.cpp).
// A head of 0 is an uninitialised list: the walk would read address 0. Skipped and counted then.
void PortTop_RL_004ccde0(void) {
    if (U32(0x007d45ccu) == 0) { Hit(s_cde0, 0); return; }
    RwCore_004ccde0();
}
}
#endif // MASHED_STANDALONE
