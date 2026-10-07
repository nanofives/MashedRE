// Mashed RE — FUN_0046d4a0, the SINGLE SHARED body. [U-9186 leg H1b, 2026-10-08]
//
// SHARED TU (exe_sources.rsp AND asi_sources.rsp). H1b was first written as a
// second, exe-only copy beside the .asi copy in Util/PromoLoop_round58.cpp.
// `scripts/lint_rva_bodies.py` rejected that as CROSS-TARGET and its policy note
// says why: the allowlist is "a burn-down list, not a permanent exemption --
// ROADMAP D4 takes it to zero by consolidating each pair into one shared TU
// judged against the original." Adding a new pair would have created exactly the
// debt D4 is committed to removing, so the body was consolidated here instead
// and the PromoLoop_round58.cpp copy deleted.
//
// The consolidation is behaviour-preserving for the .asi: VehRecord() resolves to
// 0x008815a0 + idx*0xd04 when MASHED_STANDALONE is not defined, and
// kOff_WheelSetSel / kOff_WheelMatrices are 0x9a8 / 0x928, so the arithmetic is
// the same 0x00881f48 / 0x00881ec8 the deleted copy used. The .asi therefore
// keeps the behaviour its C3 Frida evidence measured.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
//
// 0x0046d4a0  PtrCompute881ec8 — `u32 fn(u32* out, u32 idx)`
// Original, from the .asi copy's disassembly transcription
// (Util/PromoLoop_round58.cpp:18-21):
//   idx [ESP+8], CMP 0x10 / JC ; IMUL EAX,0xd04 ; MOV ECX,[EAX+0x881f48]
//   SHL ECX,6 (t*0x40) ; LEA EDX,[ECX+EAX+0x881ec8] ; MOV [out],EDX ; MOV EAX,1
//   -> *out = 0x00881ec8 + idx*0xd04 + t*0x40,  t = *(u32*)(0x00881f48 + idx*0xd04)
//
// Standalone translation. Both absolutes are fields of the record array that
// /DMASHED_STANDALONE rebinds (verify/d3_u9186_20261008/RESULT_G1.md CORRECTION):
//   0x00881f48 = record + 0x9a8   (kWheelSetSel,   VehicleStruct.h:101)
//   0x00881ec8 = record + 0x928   (kWheelMatrices, VehicleStruct.h:99)
// The arithmetic is unchanged; only the base moves, so the returned pointer is
// into g_vehicleArrayBase where this build's world x/z at +0x30/+0x38 actually
// live.
//
// EVIDENCE STATUS. The .asi arm keeps the C3 evidence the deleted copy carried
// (log/diff_ptr_compute_881ec8.csv, 10/10 GREEN 2026-06-13) because its compiled
// arithmetic is unchanged — but that diff exercised the ABSOLUTE form, so it says
// nothing about the standalone arm. The standalone arm's own witness is the
// ptr_ok/ptr_x/ptr_z columns of TrackRenderer::U9186GateDump, which call this
// body and compare the x/z it yields against VehiclePhysics_RecordF32.
// PREREG: verify/d3_u9186_20261008/PREREG_H1.md section 1 (H1b).
#include "../Core/HookSystem.h"
#include "VehicleRecordBase.h"

#include <cstdint>

extern "C" __declspec(dllexport) std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out,
                                                                        std::uint32_t idx) {
    if (idx > 0xfu) return 0;                                   // CMP 0x10 / JC
    const char* rec = mashed_re::Vehicle::VehRecord(idx);       // IMUL EAX,0xd04
    if (!rec) return 0;                 // standalone pre-race: array not allocated
    const std::uint32_t t = *reinterpret_cast<const std::uint32_t*>(
                                rec + mashed_re::Vehicle::kOff_WheelSetSel);
    *out = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
               rec + mashed_re::Vehicle::kOff_WheelMatrices
                   + static_cast<std::size_t>(t) * 0x40u));     // SHL 6 ; LEA
    return 1;
}
// No-op in the exe build; installs the hook in the .asi, as the deleted
// PromoLoop_round58.cpp copy did.
RH_ScopedInstall(PtrCompute881ec8, 0x0046d4a0);
