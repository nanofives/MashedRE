// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body: selectors of the direct callees of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe).
// RwCore/RwCore_00492e90.cpp is the one exe body of 0x00492e90 (re/match/s15/00492e90.c). Each callee below whose walked binding
// differs from that TU's own symbol goes through a selector: MASHED_OT_RENDERFRAME=1 calls the walked body (the frame-render
// closure of 0x00492e90, Match/PortTop_<rva>.cpp, or its named stop in Match/PortTop_RenderFrameStops.cpp); unset or 0 calls the TU's
// own symbol, so the knob-off path is unchanged. The TU runs only on the RW core frame route (RwCore_FrameRoute, MASHED_RWC_FRAME).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <windows.h>
extern "C" void RwCore_Log(const char* fmt, ...);   // C linkage (RwCore/RwCore_Knob.cpp:28-53)
static int s_knob = -1;
static bool PortTop_RRSelOn(void) {
    if (s_knob < 0) {
        char v[8] = {};
        s_knob = (GetEnvironmentVariableA("MASHED_OT_RENDERFRAME", v, sizeof(v)) > 0 && v[0] && v[0] != '0') ? 1 : 0;
        RwCore_Log("[PORT-TOP-RENDERFRAME-492E90] MASHED_OT_RENDERFRAME %s: the direct callees of 0x00492e90 %s", s_knob ? "on" : "off",
                   s_knob ? "call the walked frame-render closure" : "keep the RW core TU's own symbols");
    }
    return s_knob == 1;
}
extern "C" {
void RwCoreMissing_00404320(void);
void PortTop_RRStop_00404320(void);
void RwCoreMissing_0040de30(int p0);
void PortTop_Bound_0040de30(int p0);
void RwCoreMissing_0040df20(int p0);
void PortTop_RRStop_0040df20(int p0);
void RwCoreMissing_0040df60(void);
void PortTop_Bound_0040df60(void);
void RwCoreMissing_0040dfc0(void);
void PortTop_Bound_0040dfc0(void);
void RwCoreMissing_00410b30(void);
void PortTop_Bound_00410b30(void);
void RwCoreMissing_00426030(void);
void PortTop_RRStop_00426030(void);
void RwCoreMissing_00426670(int p0);
void PortTop_Bound_00426670(int p0);
void RwCoreMissing_004266b0(int p0);
void PortTop_Bound_004266b0(int p0);
int RwCoreMissing_0042a9f0(void);
int PortTop_Bound_0042a9f0(void);
int RwCoreMissing_0042b8f0(void);
int StatePhaseIsFinal(void);
int RwCoreMissing_0042d390(void);
int PortTop_Bound_0042d390(void);
int RwCoreMissing_0042f510(void);
int Vehicle0HandleGet(void);
void RwCoreMissing_00430b90(void);
void PortTop_Bound_00430b90(void);
int RwCoreMissing_004671c0(void);
int GetOverlayCamera(void);
void PortTop_RRSel_00404320(void) { if (PortTop_RRSelOn()) { PortTop_RRStop_00404320(); return; } RwCoreMissing_00404320(); }   // 0x00404320
void PortTop_RRSel_0040de30(int p0) { if (PortTop_RRSelOn()) { PortTop_Bound_0040de30(p0); return; } RwCoreMissing_0040de30(p0); }   // 0x0040de30
void PortTop_RRSel_0040df20(int p0) { if (PortTop_RRSelOn()) { PortTop_RRStop_0040df20(p0); return; } RwCoreMissing_0040df20(p0); }   // 0x0040df20
void PortTop_RRSel_0040df60(void) { if (PortTop_RRSelOn()) { PortTop_Bound_0040df60(); return; } RwCoreMissing_0040df60(); }   // 0x0040df60
void PortTop_RRSel_0040dfc0(void) { if (PortTop_RRSelOn()) { PortTop_Bound_0040dfc0(); return; } RwCoreMissing_0040dfc0(); }   // 0x0040dfc0
void PortTop_RRSel_00410b30(void) { if (PortTop_RRSelOn()) { PortTop_Bound_00410b30(); return; } RwCoreMissing_00410b30(); }   // 0x00410b30
void PortTop_RRSel_00426030(void) { if (PortTop_RRSelOn()) { PortTop_RRStop_00426030(); return; } RwCoreMissing_00426030(); }   // 0x00426030
void PortTop_RRSel_00426670(int p0) { if (PortTop_RRSelOn()) { PortTop_Bound_00426670(p0); return; } RwCoreMissing_00426670(p0); }   // 0x00426670
void PortTop_RRSel_004266b0(int p0) { if (PortTop_RRSelOn()) { PortTop_Bound_004266b0(p0); return; } RwCoreMissing_004266b0(p0); }   // 0x004266b0
int PortTop_RRSel_0042a9f0(void) { if (PortTop_RRSelOn()) return PortTop_Bound_0042a9f0(); return RwCoreMissing_0042a9f0(); }   // 0x0042a9f0
int PortTop_RRSel_0042b8f0(void) { if (PortTop_RRSelOn()) return StatePhaseIsFinal(); return RwCoreMissing_0042b8f0(); }   // 0x0042b8f0
int PortTop_RRSel_0042d390(void) { if (PortTop_RRSelOn()) return PortTop_Bound_0042d390(); return RwCoreMissing_0042d390(); }   // 0x0042d390
int PortTop_RRSel_0042f510(void) { if (PortTop_RRSelOn()) return Vehicle0HandleGet(); return RwCoreMissing_0042f510(); }   // 0x0042f510
void PortTop_RRSel_00430b90(void) { if (PortTop_RRSelOn()) { PortTop_Bound_00430b90(); return; } RwCoreMissing_00430b90(); }   // 0x00430b90
int PortTop_RRSel_004671c0(void) { if (PortTop_RRSelOn()) return GetOverlayCamera(); return RwCoreMissing_004671c0(); }   // 0x004671c0
}
#endif // MASHED_STANDALONE
