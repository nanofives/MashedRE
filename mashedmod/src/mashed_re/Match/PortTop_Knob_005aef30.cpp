// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-AUDIO-5AEF30-KNOB (L-174 (c), 2026-11-16): what a bound caller of 0x005aef30 calls (generator KNOB,
// verify/portorigtop_20261113/tools/audiotick_bind_gen.py). Not a body of 0x005aef30, never C4.
// PREREG verify/portorigtop_20261113/PREREG_PORT-TOP-AUDIO-5AEF30-KNOB.md, RESULT verify/portorigtop_20261113/RESULT_PORT-TOP-AUDIO-5AEF30-KNOB.md.
//
//   MASHED_OT_AUDIOTHREAD_STANDIN unset (default) -> the named stop of 0x005aef30, as before this row: log + exit with
//                                                   code 0x005aef30, return address = the caller's (PortTop_AudioCalleeStop)
//   MASHED_OT_AUDIOTHREAD_STANDIN=1               -> return param_1, start no thread, write nothing
//
// Original 0x005aef30 (listing of MASHED.exe.unpatched): param_2 -> param_1+8 (0x005aef3d), __beginthread(0x005aef70,
// param_1[4], param_1) (0x005aef46 -> 0x005c2e79), return 0 on -1 (0x005aef4e / 0x005aef53), handle -> param_1[0]
// (0x005aef5a), SetThreadPriority(handle, param_1[3]) ([0x005cc0bc], 0x005aef5e), return param_1 (0x005aef64).
// The bound caller 0x005a8060 tests the return only for zero (test eax,eax / jne 0x005a8346 at 0x005a8330 / 0x005a8332).
// The +8 store is left out: its reader is the thread entry 0x005aef70, which the stand-in does not start, and the bound
// caller passes 0 (push 0 at 0x005a8321) over a +8 that 0x005aef00 already zeroed (0x005aef19).
#ifdef MASHED_STANDALONE
#include <intrin.h>
#include <cstdio>
#include <cstdlib>

namespace {
int AudioThreadStandInOn() {
    static int s = -1;
    if (s < 0) {
        const char* const v = std::getenv("MASHED_OT_AUDIOTHREAD_STANDIN");
        s = (v && v[0] == '1') ? 1 : 0;
    }
    return s;
}
unsigned s_calls = 0;
} // namespace

extern "C" {
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp

// Stand-in (not a body) of rva 0x005aef30 under the knob, the named stop otherwise
unsigned* PortTop_Knob_005aef30(unsigned* param_1, unsigned param_2) {
    if (!AudioThreadStandInOn()) {
        PortTop_AudioCalleeStop(0x005aef30u, 0u, _ReturnAddress());   // same call as PortTop_AStop_005aef30
        return 0;
    }
    if (++s_calls == 1) {
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x005aef30: stand-in (PORT-TOP-AUDIO-5AEF30-KNOB) returns param_1 %p, no thread, "
                            "param_2 %08X not stored, desc +0 %08X +4 %08X +8 %08X +c %08X +10 %08X, return address %p\n",
                         static_cast<void*>(param_1), param_2, param_1[0], param_1[1], param_1[2], param_1[3], param_1[4],
                         _ReturnAddress());
            std::fclose(f);
        }
    }
    return param_1;
}
}
#endif // MASHED_STANDALONE
