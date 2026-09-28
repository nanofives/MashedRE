// Powerup/PowerupAim.h — FUN_00459620, the TARGET-ACQUISITION routine that
// MORTAR, GUN and MISSILE share (D3 criterion (c)).
//
// Every offset, constant and branch below was disassembled from
// original/MASHED.exe.unpatched (SHA-256 BDCAE093...3C0E, the pinned anchor)
// with re/tools/disasm_va.py, or read from a read-only Ghidra pool clone with
// re/tools/decomp_pc.py.
//
// WHAT IT IS, and what the prior note got wrong. D3_CONTACT_PORT_2026-09-28.md
// §5.2 calls FUN_00459620 "the SHARED projectile routine" and says the flight
// that drives MORTAR's, GUN's and MISSILE's contact sites runs inside it. It
// does not. The routine acquires a TARGET: it scores every other car in a cone,
// picks the one at the smallest angle, probes line of sight to it, and writes an
// aim record. No projectile position is integrated anywhere in it.
//
// WHY IT GATES THREE TYPES. Its two world-query sites sit on opposite sides of
// one branch, `if (candidate_count == 0)`:
//
//   0x459c19  the vertical FALLBACK probe, at `origin + at*range`  -- == 0 only
//   0x459d54  the LINE-OF-SIGHT probe, origin -> endpoint          -- always
//
// so a port that gets the candidate count wrong gets one site's call count wrong
// on every call. Measured in verify/d3_contact_20260928 (pu_contact_report.py),
// the branch goes both ways and no stub can be right:
//   g3 GUN     0x459c19 163/163 calls    g2 MISSILE 0x459c19  0/32  calls
//   g4 GUN     0x459c19 163/163 calls    g2 MORTAR  0x459c19 96/153 calls
//
// THE RECORD. 0x0068b9f8 + slot*0x58, from the prologue: `IMUL EBP,EBP,0x58`
// @0x00459632 and `ADD EBP,0x68b9f8` @0x00459638. The decompiler's
// `&DAT_0068b9fc` names the +4 field, not the base.
//
// PORT BOUNDARY, stated plainly:
//   - The DECISION STRUCTURE -- the candidate test, the selection rule, which
//     query fires on which side, and what each outcome writes -- is verbatim.
//   - Three live-subsystem reads are INPUTS here rather than calls, because the
//     standalone and the replay reach them differently: the four car positions
//     (FUN_0041f030), their active flags (FUN_0040e370) and the firing car's aim
//     matrix (FUN_0041f220). See Inputs below.
//   - The SECOND candidate list (FUN_004075a0 / FUN_004075b0) is NOT ported.
//     It is a MEASURED no-op on the only capture that records it: list_n == 0 on
//     all 348 acquisition calls of verify/d3_contact_20260928b/m1.msd. `Inputs::
//     listCount` carries the number so a future capture that is non-zero fails
//     loudly instead of silently taking the wrong branch.
//   - The THIRD loop (FUN_0047ce70 / FUN_0047d130 over RW atomics, decompiled at
//     0x00459f5e..0x0045a0dc) is NOT ported. It makes NO contact call, so it
//     cannot move a criterion-(c) count; it can only move the written endpoint.
//   - The locked branch's INTERCEPT PREDICTION (FUN_0041f2c0, and the
//     FUN_00558b40 adjust on its refusal) is not ported -- see [UNCERTAIN] in
//     the .cpp.
//   This is C2-grade. Do NOT mark C4.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Powerup {
namespace Aim {

// The acquisition record. Offsets are from the record base 0x0068b9f8; only the
// fields FUN_00459620 itself writes are modelled.
struct Record {
    float cone;         // +0x04  `MOV [EBP+4],EAX` @0x0045966a  = arg4
    int   targetCar;    // +0x08  car index, or -1
    int   objIndex;     // +0x0c  third-loop index, or -1
    int   hit;          // +0x14  `MOV [EBP+0x14],0` @0x00459673; 1 = endpoint is a hit
    float range;        // +0x18  `MOV [EBP+0x18],ECX` @0x0045966d = arg3
    float dist;         // +0x1c
    float aimPoint[3];  // +0x24..0x2c
    float endpoint[3];  // +0x3c..0x44
    float origin[3];    // +0x48..0x50  = arg2[0..2] @0x00459685..0x00459693
};

// The live-subsystem reads, hoisted to inputs. The standalone fills these from
// its own car field; re/tools/pu_replay fills them from the capture's
// `--puhook-aim` channel, which records exactly these addresses:
//   carPos[i]   4 dwords at 0x0063dc38 + i*0x2ac        (FUN_0041f030)
//   carActive[i] [[0x005f2770] + i*4 + 0x34] != 0       (FUN_0040e370)
//   at          the aim matrix `at` row, +0x20..0x28    (FUN_0041f220 fills the
//               record's +0x38 buffer from the car's own matrix)
//   listCount   [0x0063a5d0]                            (FUN_004075a0)
struct Inputs {
    float origin[3];
    float range;        // arg3
    float cone;         // arg4, in DEGREES (_DAT_005cc98c = 57.29578 = 180/pi)
    float at[3];
    int   carActive[4];
    float carPos[4][3];
    int   listCount;
};

// FUN_00459620. `slot` is arg1 -- the FIRING car's index, which the candidate
// loop skips (`CMP EAX,EBX` @0x004596cd).
void Acquire(int slot, const Inputs& in, Record* rec);

// How many of the last Acquire's candidates passed the cone/range test. Exposed
// only so a harness can assert the branch it expected; not part of the original.
int  LastCandidateCount();

}  // namespace Aim
}  // namespace Powerup
}  // namespace mashed_re
