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
Count s_8c70 = {"0x00458c70", "136 B, callee 0x004c15c0", 0};
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
// Guard for the RwCore_004ccde0 call: that body walks the list at 0x007d45cc until it returns to &0x007d45cc (re/match source, RwCore/RwCore_004ccde0.cpp).
// A head of 0 is an uninitialised list: the walk would read address 0. Skipped and counted then.
void PortTop_RL_004ccde0(void) {
    if (U32(0x007d45ccu) == 0) { Hit(s_cde0, 0); return; }
    RwCore_004ccde0();
}
}
#endif // MASHED_STANDALONE
