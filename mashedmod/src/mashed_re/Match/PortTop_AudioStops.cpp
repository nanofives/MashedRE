// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): named stops of the audio-tick closure (0x00466b50 / 0x0045d3f0 / 0x0045d430).
// Each is a callee with no matched (c4=match) or L-126 admitted body yet. A stop logs and exits with code = rva
// (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so a reached stop names the next body to bind.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <intrin.h>
extern "C" {
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
void PortTop_AStop_004075a0(void) { PortTop_AudioCalleeStop(0x004075a0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0042b8d0(void) { PortTop_AudioCalleeStop(0x0042b8d0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0042b900(void) { PortTop_AudioCalleeStop(0x0042b900u, 0u, _ReturnAddress()); }
void PortTop_AStop_0045caf0(void) { PortTop_AudioCalleeStop(0x0045caf0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0045d7a0(void) { PortTop_AudioCalleeStop(0x0045d7a0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0045dd60(void) { PortTop_AudioCalleeStop(0x0045dd60u, 0u, _ReturnAddress()); }
void PortTop_AStop_0045e990(void) { PortTop_AudioCalleeStop(0x0045e990u, 0u, _ReturnAddress()); }
void PortTop_AStop_0045efe0(void) { PortTop_AudioCalleeStop(0x0045efe0u, 0u, _ReturnAddress()); }
void PortTop_AStop_00461e90(void) { PortTop_AudioCalleeStop(0x00461e90u, 0u, _ReturnAddress()); }
void PortTop_AStop_00463640(void) { PortTop_AudioCalleeStop(0x00463640u, 0u, _ReturnAddress()); }
void PortTop_AStop_00464ca0(void) { PortTop_AudioCalleeStop(0x00464ca0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0046c7b0(void) { PortTop_AudioCalleeStop(0x0046c7b0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0046cbb0(void) { PortTop_AudioCalleeStop(0x0046cbb0u, 0u, _ReturnAddress()); }
void PortTop_AStop_00493f70(void) { PortTop_AudioCalleeStop(0x00493f70u, 0u, _ReturnAddress()); }
void PortTop_AStop_004a34b0(void) { PortTop_AudioCalleeStop(0x004a34b0u, 0u, _ReturnAddress()); }
void PortTop_AStop_004c3ac0(void) { PortTop_AudioCalleeStop(0x004c3ac0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005459c0(void) { PortTop_AudioCalleeStop(0x005459c0u, 0u, _ReturnAddress()); }
void PortTop_AStop_00550f20(void) { PortTop_AudioCalleeStop(0x00550f20u, 0u, _ReturnAddress()); }
void PortTop_AStop_00551460(void) { PortTop_AudioCalleeStop(0x00551460u, 0u, _ReturnAddress()); }
void PortTop_AStop_0057c210(void) { PortTop_AudioCalleeStop(0x0057c210u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a5fd0(void) { PortTop_AudioCalleeStop(0x005a5fd0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a6010(void) { PortTop_AudioCalleeStop(0x005a6010u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a75b0(void) { PortTop_AudioCalleeStop(0x005a75b0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a9f60(void) { PortTop_AudioCalleeStop(0x005a9f60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aa560(void) { PortTop_AudioCalleeStop(0x005aa560u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aac20(void) { PortTop_AudioCalleeStop(0x005aac20u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b0cf0(void) { PortTop_AudioCalleeStop(0x005b0cf0u, 0u, _ReturnAddress()); }
}
#endif // MASHED_STANDALONE
