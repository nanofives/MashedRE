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
void PortTop_AStop_005459c0(void) { PortTop_AudioCalleeStop(0x005459c0u, 0u, _ReturnAddress()); }
void PortTop_AStop_00550f20(void) { PortTop_AudioCalleeStop(0x00550f20u, 0u, _ReturnAddress()); }
void PortTop_AStop_0057c210(void) { PortTop_AudioCalleeStop(0x0057c210u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a6640(void) { PortTop_AudioCalleeStop(0x005a6640u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a6cb0(void) { PortTop_AudioCalleeStop(0x005a6cb0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a75b0(void) { PortTop_AudioCalleeStop(0x005a75b0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a8460(void) { PortTop_AudioCalleeStop(0x005a8460u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a84d0(void) { PortTop_AudioCalleeStop(0x005a84d0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a8640(void) { PortTop_AudioCalleeStop(0x005a8640u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a8680(void) { PortTop_AudioCalleeStop(0x005a8680u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a9280(void) { PortTop_AudioCalleeStop(0x005a9280u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a9d00(void) { PortTop_AudioCalleeStop(0x005a9d00u, 0u, _ReturnAddress()); }
void PortTop_AStop_005a9f60(void) { PortTop_AudioCalleeStop(0x005a9f60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aac20(void) { PortTop_AudioCalleeStop(0x005aac20u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aae10(void) { PortTop_AudioCalleeStop(0x005aae10u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aae30(void) { PortTop_AudioCalleeStop(0x005aae30u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aae60(void) { PortTop_AudioCalleeStop(0x005aae60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005ab010(void) { PortTop_AudioCalleeStop(0x005ab010u, 0u, _ReturnAddress()); }
void PortTop_AStop_005ab620(void) { PortTop_AudioCalleeStop(0x005ab620u, 0u, _ReturnAddress()); }
void PortTop_AStop_005abe30(void) { PortTop_AudioCalleeStop(0x005abe30u, 0u, _ReturnAddress()); }
void PortTop_AStop_005ac540(void) { PortTop_AudioCalleeStop(0x005ac540u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aead0(void) { PortTop_AudioCalleeStop(0x005aead0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005aef30(void) { PortTop_AudioCalleeStop(0x005aef30u, 0u, _ReturnAddress()); }
void PortTop_AStop_005af140(void) { PortTop_AudioCalleeStop(0x005af140u, 0u, _ReturnAddress()); }
void PortTop_AStop_005af2a0(void) { PortTop_AudioCalleeStop(0x005af2a0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005af670(void) { PortTop_AudioCalleeStop(0x005af670u, 0u, _ReturnAddress()); }
void PortTop_AStop_005af690(void) { PortTop_AudioCalleeStop(0x005af690u, 0u, _ReturnAddress()); }
void PortTop_AStop_005af780(void) { PortTop_AudioCalleeStop(0x005af780u, 0u, _ReturnAddress()); }
void PortTop_AStop_005af7d0(void) { PortTop_AudioCalleeStop(0x005af7d0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005af8f0(void) { PortTop_AudioCalleeStop(0x005af8f0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b0cf0(void) { PortTop_AudioCalleeStop(0x005b0cf0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b0f40(void) { PortTop_AudioCalleeStop(0x005b0f40u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b3300(void) { PortTop_AudioCalleeStop(0x005b3300u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b35e0(void) { PortTop_AudioCalleeStop(0x005b35e0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b6e60(void) { PortTop_AudioCalleeStop(0x005b6e60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b7040(void) { PortTop_AudioCalleeStop(0x005b7040u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b7750(void) { PortTop_AudioCalleeStop(0x005b7750u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b7980(void) { PortTop_AudioCalleeStop(0x005b7980u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b7a90(void) { PortTop_AudioCalleeStop(0x005b7a90u, 0u, _ReturnAddress()); }
void PortTop_AStop_005b92e0(void) { PortTop_AudioCalleeStop(0x005b92e0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005ba7f0(void) { PortTop_AudioCalleeStop(0x005ba7f0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005baa60(void) { PortTop_AudioCalleeStop(0x005baa60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005bb380(void) { PortTop_AudioCalleeStop(0x005bb380u, 0u, _ReturnAddress()); }
void PortTop_AStop_005bbf60(void) { PortTop_AudioCalleeStop(0x005bbf60u, 0u, _ReturnAddress()); }
void PortTop_AStop_005bc050(void) { PortTop_AudioCalleeStop(0x005bc050u, 0u, _ReturnAddress()); }
void PortTop_AStop_005bc0f0(void) { PortTop_AudioCalleeStop(0x005bc0f0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005bcef0(void) { PortTop_AudioCalleeStop(0x005bcef0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005bd640(void) { PortTop_AudioCalleeStop(0x005bd640u, 0u, _ReturnAddress()); }
void PortTop_AStop_005bd6f0(void) { PortTop_AudioCalleeStop(0x005bd6f0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c2dd9(void) { PortTop_AudioCalleeStop(0x005c2dd9u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c2e79(void) { PortTop_AudioCalleeStop(0x005c2e79u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c70d0(void) { PortTop_AudioCalleeStop(0x005c70d0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c8010(void) { PortTop_AudioCalleeStop(0x005c8010u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c83a0(void) { PortTop_AudioCalleeStop(0x005c83a0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c8810(void) { PortTop_AudioCalleeStop(0x005c8810u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c89a0(void) { PortTop_AudioCalleeStop(0x005c89a0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c9770(void) { PortTop_AudioCalleeStop(0x005c9770u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c98d0(void) { PortTop_AudioCalleeStop(0x005c98d0u, 0u, _ReturnAddress()); }
void PortTop_AStop_005c9d10(void) { PortTop_AudioCalleeStop(0x005c9d10u, 0u, _ReturnAddress()); }
void PortTop_AStop_00635850(void) { PortTop_AudioCalleeStop(0x00635850u, 0u, _ReturnAddress()); }
void PortTop_AStop_00635a28(void) { PortTop_AudioCalleeStop(0x00635a28u, 0u, _ReturnAddress()); }
}
#endif // MASHED_STANDALONE
