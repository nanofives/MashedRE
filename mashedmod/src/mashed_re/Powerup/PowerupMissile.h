// Powerup/PowerupMissile.h — MISSILE's own projectile chain (D3 criterion (c)).
//
// The tick is FUN_00455c90 (0x00455c90). Ghidra's auto-analysis never defined
// it, so it was read with the `--create` mode added to re/tools/decomp_pc.py:
// a TRANSIENT function definition against a -readOnly pool clone, discarded on
// exit. That is not a master write, and none was needed.
//
// TWO POOLS, walked in lockstep in one loop, both bounds disassembled from
// original/MASHED.exe.unpatched (SHA-256 BDCAE093...3C0E):
//   aim records  0x006885d0 stride 0x2c, FIVE entries
//     `MOV EDI,0x6886ac` @0x00455c9a, `SUB EDI,0x2c` @0x00455ca9
//   projectiles  0x006883b0 stride 0x6c, FIVE entries
//     `MOV EBP,0x688620` @0x00455c9f, `SUB EBP,0x6c` @0x00455caf, and the loop
//     exits at 0x00688404 — so the record BASES are 0x6883b0/41c/488/4f4/560.
// The older note's "pool DAT_006883bc stride 0x6c" named the record's POSITION
// field (base+0x0c), not its base — the same off-by-a-field as the acquisition
// record's &DAT_0068b9fc gloss.
//
// THE LOOP WALKS DOWNWARD. EBP starts at the HIGHEST record and decrements, so
// record index 4 is stepped first and index 0 last. That ordering is load-bearing
// for anything that zips per-frame contact rows against records.
//
// WHAT THE TICK DOES per live record (0x00455cf9..0x00455e9f):
//   flight step:  (+0x54 == -1 && +0x58 == -1) ? FUN_00455610 : FUN_004556f0
//   age += DAT_007f100c
//   if (age > 3.0)  -> FUN_00455910 terminal (RA 0x455d37), done
//   EVEN FRAMES ONLY (`DAT_007f101c & 0x80000001`, 0x00455d83..0x00455d99):
//       sphere query FUN_004b4d10, radius |delta|^2 * 1.5 + 0.05   (RA 0x455de0)
//       on a hit: gate FUN_0045c350 (RA 0x455df9); a ZERO gate detonates
//       via FUN_00455910 (RA 0x455e07)
//   EVERY frame: ground probe FUN_004b4cd0, pos -> pos - (0, 3.0, 0) (RA 0x455e59)
//       miss -> bias = -0.0005 ; hit -> bias = (t*3 - 0.4)*2.5*-0.5, floored at -0.05
//
// The parity gate is why every capture shows 0x455de0 at about half of 0x455e59
// (m1 15 vs 31, m2 6 vs 12, s1 98 vs 195).
//
// PORT BOUNDARY, stated plainly and it is WIDER than MORTAR's:
//   - The CONTACT HALF is verbatim: the lifetime gate, the frame-parity gate, the
//     sphere radius, both query branches, the terminal, and the +0x28 ground
//     bias — which is a pure function of the query verdict, so the port owns it
//     outright.
//   - The FLIGHT INTEGRATION is NOT ported. FUN_00455610 (unguided) and
//     FUN_004556f0 (homing) both read AND write the projectile's RenderWare frame
//     matrix through FUN_004c1520 / FUN_004c1340 / FUN_004c15c0, a closed loop the
//     replay cannot reproduce without porting those RwMatrix ops. Position and
//     delta are INPUTS here. Consequence, stated so it is not mistaken for
//     evidence: the port does not decide WHERE a missile is, only what it does
//     about it.
//   - FUN_00455100 (RA 0x455cd9) belongs to the AIM half, not the projectile
//     half, and is not ported.
//   This is C2-grade. Do NOT mark C4.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Powerup {
namespace Missile {

// One pool record's live fields, by byte offset from the record base.
struct Record {
    int   live;       // +0x50
    float pos[3];     // +0x0c..0x14
    float delta[3];   // +0x1c..0x24   this frame's travel
    float bias;       // +0x28         ground-follow term added to delta.y
    float age;        // +0x18
    float speed;      // +0x30
    int   tgt0;       // +0x54         both -1 = unguided (FUN_00455610)
    int   tgt1;       // +0x58
};

// 0x00455c90's per-record body. Returns:
//   0 stepped normally, 1 detonated (sphere hit + gate allowed), 2 aged out.
// `frameCtr` is DAT_007f101c, whose parity gates the sphere query.
int Step(Record& r, float dt, int frameCtr);

}  // namespace Missile
}  // namespace Powerup
}  // namespace mashed_re
