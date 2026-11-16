// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): named stops of the audio-tick closure (0x00466b50 / 0x0045d3f0 / 0x0045d430).
// Each is a callee with no matched (c4=match) or L-126 admitted body yet. A stop logs and exits with code = rva
// (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so a reached stop names the next body to bind.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <intrin.h>
extern "C" {
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
void PortTop_AStop_004075a0(void) { PortTop_AudioCalleeStop(0x004075a0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0045d7a0(void) { PortTop_AudioCalleeStop(0x0045d7a0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0045dd60(void) { PortTop_AudioCalleeStop(0x0045dd60u, 0u, _ReturnAddress()); }
void PortTop_AStop_0045e990(void) { PortTop_AudioCalleeStop(0x0045e990u, 0u, _ReturnAddress()); }
void PortTop_AStop_0045efe0(void) { PortTop_AudioCalleeStop(0x0045efe0u, 0u, _ReturnAddress()); }
void PortTop_AStop_00461e90(void) { PortTop_AudioCalleeStop(0x00461e90u, 0u, _ReturnAddress()); }
void PortTop_AStop_00463640(void) { PortTop_AudioCalleeStop(0x00463640u, 0u, _ReturnAddress()); }
void PortTop_AStop_00464ca0(void) { PortTop_AudioCalleeStop(0x00464ca0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0046c7b0(void) { PortTop_AudioCalleeStop(0x0046c7b0u, 0u, _ReturnAddress()); }
void PortTop_AStop_0046cbb0(void) { PortTop_AudioCalleeStop(0x0046cbb0u, 0u, _ReturnAddress()); }
void PortTop_AStop_004a34b0(void) { PortTop_AudioCalleeStop(0x004a34b0u, 0u, _ReturnAddress()); }
void PortTop_AStop_004a8815(void) { PortTop_AudioCalleeStop(0x004a8815u, 0u, _ReturnAddress()); }
void PortTop_AStop_004ccef0(void) { PortTop_AudioCalleeStop(0x004ccef0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005459c0(void) { PortTop_AudioCalleeStop(0x005459c0u, 0u, _ReturnAddress()); }
void PortTop_AStop_00550f20(void) { PortTop_AudioCalleeStop(0x00550f20u, 0u, _ReturnAddress()); }
void PortTop_AStop_00551460(void) { PortTop_AudioCalleeStop(0x00551460u, 0u, _ReturnAddress()); }
void PortTop_AStop_0057c210(void) { PortTop_AudioCalleeStop(0x0057c210u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a65d0(void) { PortTop_AudioCalleeStop(0x005a65d0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a6640(void) { PortTop_AudioCalleeStop(0x005a6640u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a66c0(void) { PortTop_AudioCalleeStop(0x005a66c0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a6cb0(void) { PortTop_AudioCalleeStop(0x005a6cb0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a71f0(void) { PortTop_AudioCalleeStop(0x005a71f0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a75b0(void) { PortTop_AudioCalleeStop(0x005a75b0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a7b60(void) { PortTop_AudioCalleeStop(0x005a7b60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a91b0(void) { PortTop_AudioCalleeStop(0x005a91b0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a9d00(void) { PortTop_AudioCalleeStop(0x005a9d00u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a9e60(void) { PortTop_AudioCalleeStop(0x005a9e60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a9f10(void) { PortTop_AudioCalleeStop(0x005a9f10u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a9f60(void) { PortTop_AudioCalleeStop(0x005a9f60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aac00(void) { PortTop_AudioCalleeStop(0x005aac00u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aac20(void) { PortTop_AudioCalleeStop(0x005aac20u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aae10(void) { PortTop_AudioCalleeStop(0x005aae10u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aae30(void) { PortTop_AudioCalleeStop(0x005aae30u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aae60(void) { PortTop_AudioCalleeStop(0x005aae60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005ab010(void) { PortTop_AudioCalleeStop(0x005ab010u, 0u, _ReturnAddress()); }
void PortTop_AStop_005ae250(void) { PortTop_AudioCalleeStop(0x005ae250u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aead0(void) { PortTop_AudioCalleeStop(0x005aead0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b0cf0(void) { PortTop_AudioCalleeStop(0x005b0cf0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b35e0(void) { PortTop_AudioCalleeStop(0x005b35e0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005bf610(void) { PortTop_AudioCalleeStop(0x005bf610u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c2dd9(void) { PortTop_AudioCalleeStop(0x005c2dd9u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c2e79(void) { PortTop_AudioCalleeStop(0x005c2e79u, 0u, _ReturnAddress()); }
void PortTop_AStop_00635850(void) { PortTop_AudioCalleeStop(0x00635850u, 0u, _ReturnAddress()); }
}
#endif // MASHED_STANDALONE
