// Mashed RE - c3_batch_ah s4: gameplay score/grid thresholds + paired-resource release.
// Bit-faithful ports of three tiny gameplay leaves.  See
//   re/analysis/bucket_gameplay_0040ad90_00412010/0x0040b8{90,e0}.md
//   re/analysis/bucket_gameplay_0040ad90_00412010/0x0040cf40.md
//
// VERIFICATION NOTE (c3_batch_ah s4): all three DEFER, not promoted to C3.
//   - 0x0040b890 / 0x0040b8e0 are zero-arg getters gated on FUN_0040e340(),
//     FUN_0042f500() and the mode global DAT_007f0fd0, all fixed at the menu ->
//     constant observable (false-green under int_scalar). DEFER.
//   - 0x0040cf40 is a no-op at diff-attach (DAT_0063ba90/ba94 are both 0 at the
//     menu, so the non-null guards skip the FUN_004c5a60 destructor + the writes)
//     -> degenerate all-zero observable; cannot seed valid destructible handles
//     without risking a crash. DEFER.
// Ported faithfully so the exports exist; left for a runtime-state harness.
#include "../Core/HookSystem.h"
#include <cstdint>

// External getters / destructor (call into the original at their fixed RVAs;
// project convention is reinterpret_cast<FnType>(addr), not extern symbols).
typedef int  (__cdecl* GetterFn)(void);
typedef void (__cdecl* DtorFn)(int);
static const GetterFn FUN_0040e340 = reinterpret_cast<GetterFn>(0x0040e340u);  // (C4)
static const GetterFn FUN_0042f500 = reinterpret_cast<GetterFn>(0x0042f500u);  // (C4)
static const DtorFn   FUN_004c5a60 = reinterpret_cast<DtorFn>(0x004c5a60u);    // single-arg destructor

// Mode flag read by both threshold getters.
static int* const DAT_007f0fd0 = reinterpret_cast<int*>(0x007f0fd0);

// Paired global resource handles released by 0x0040cf40.
static int* const DAT_0063ba90 = reinterpret_cast<int*>(0x0063ba90);
static int* const DAT_0063ba94 = reinterpret_cast<int*>(0x0063ba94);

// 0x0040b890  FUN_0040b890  (65 bytes)  int(void)
// Returns 8 in every case except: FUN_0040e340()==4, FUN_0042f500()==0,
// DAT_007f0fd0 not in {1,2} -> returns 12 (0xc).
extern "C" __declspec(dllexport) int __cdecl GetGridCount8or12(void) {
    int iVar1 = FUN_0040e340();
    int iVar2 = (-(unsigned)(iVar1 != 4) & 0xfffffffc) + 0xc;   // ==4 -> 12 ; else 8
    iVar1 = FUN_0042f500();
    if (iVar1 != 0) iVar2 = 8;
    if (*DAT_007f0fd0 == 1) return 8;
    iVar1 = 8;
    if (*DAT_007f0fd0 != 2) iVar1 = iVar2;
    return iVar1;
}

RH_ScopedInstall(GetGridCount8or12, 0x0040b890);  // c3_batch_ah s4 (DEFER: degenerate)

// 0x0040b8e0  FUN_0040b8e0  (65 bytes)  int(void)
// Same shape as 0x0040b890 but returns 7 or 10.  Returns 7 in every case except:
// FUN_0040e340()==4, FUN_0042f500()==0, DAT_007f0fd0 not in {1,2} -> returns 10.
extern "C" __declspec(dllexport) int __cdecl GetScoreThreshold7or10(void) {
    int iVar1 = FUN_0040e340();
    int iVar2 = (-(unsigned)(iVar1 != 4) & 0xfffffffd) + 10;    // ==4 -> 10 ; else 7
    iVar1 = FUN_0042f500();
    if (iVar1 != 0) iVar2 = 7;
    if (*DAT_007f0fd0 == 1) return 7;
    iVar1 = 7;
    if (*DAT_007f0fd0 != 2) iVar1 = iVar2;
    return iVar1;
}

RH_ScopedInstall(GetScoreThreshold7or10, 0x0040b8e0);  // c3_batch_ah s4 (DEFER: degenerate)

// 0x0040cf40  FUN_0040cf40  (56 bytes)  void(void)
// For each of DAT_0063ba90 / DAT_0063ba94: if non-null, FUN_004c5a60(handle) and
// null it.  Idempotent paired-resource release.
extern "C" __declspec(dllexport) void __cdecl ReleaseResourcePair(void) {
    if (*DAT_0063ba90 != 0) { FUN_004c5a60(*DAT_0063ba90); *DAT_0063ba90 = 0; }
    if (*DAT_0063ba94 != 0) { FUN_004c5a60(*DAT_0063ba94); *DAT_0063ba94 = 0; }
}

RH_ScopedInstall(ReleaseResourcePair, 0x0040cf40);  // c3_batch_ah s4 (DEFER: no-op at menu)
