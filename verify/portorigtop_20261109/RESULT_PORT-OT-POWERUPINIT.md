# RESULT PORT-OT-POWERUPINIT (2026-11-10): SCOPE STOP, no swap. Bindings chain fixed. Proposed 4-way split

L2 worker `cmwb00r8i0tsrsc1bk4hkzlvs` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/PORT-OT-POWERUPINIT` from
orch/integration `5557d67b5`. No mashedmod change, so no build and no launch. The default path and the knob path are
byte for byte what `5557d67b5` builds.

This row hit the brief's stop condition: "the Lua init needing a large unmatched subtree". The matched body of 0x0045bae0
(re/match/s10/0045bae0.c) has three parts that need different work. Only one of them, the five builders, is port work
that one commit can do.

## What 0x0045bae0 calls (re/match/s10/0045bae0.c)

1. Two zero passes on 0x0088fbe0 / 0x0068d200, then `DAT_0068d4e0 = 0x0042a6b0("PowerUpModels.txd", 0, 0)` (exe body
   `FUN_0042a6b0`, matched, already on the path).
2. `DAT_005f9998[i].init()` for `i < DAT_005f9bd8`. That is 9 calls through the original's type table, stride 0x40,
   slot +0x14.
3. Builders 0x00459290, 0x004587a0, 0x00453f30, then `0x0047b9b0("powerups_all.lua", &tmp, 0x00440bc0)`, then
   0x00411f30 and the tail 0x004039c0.

### Part 2: the type table cannot run as it stands

The exe's image pad holds the original's .data range zero-filled. Only seeded ranges hold the original's bytes
(Track/CourseTableState.cpp header, U-9783). Compat/PortStateSeed.inc seeds only `0x005f99c4` (= 0x0045b350, entry 0
+0x2c) and `0x005f9bd8` (= 9; knob log line `DAT_005f9bd8=9` in RESULT_PORT-OT-PICKUPPOOL's knob_on_1.log:409). So in
the exe every +0x14 slot is 0. The matched loop would call address 0 on its first iteration. In the original the slots
hold .text addresses, and the exe has no thunk at them. These are the slots, read from `original\MASHED.exe.unpatched`
at 0x005f9998 + 0x40*i + 0x14:

| i | type (+0x00) | init (+0x14) | hooks.csv | match source | strings it pushes (capstone) |
|---|---|---|---|---|---|
| 0 | 0x09 | 0x00456760 | C4, port none | re/match/s09/00456760.c | `flash`, `GatlingGun.dff` |
| 1 | 0x0a | 0x004548e0 | C4, port none | re/match/s09/004548e0.c | `DepthCharge.dff`, `DepthChargeRound.dff` |
| 2 | 0x0b | 0x00455400 | **no row** | none | `smoke`, `MissileLauncher.dff`, `missile.dff` |
| 3 | 0x0c | 0x004583a0 | **no row** | none | `MineDropper.dff`, `Mine.dff`, `lensflare-fs8` |
| 4 | 0x10 | 0x0045a6b0 | **no row** | none | `FlameThrower.dff`, `exp_cloud2` |
| 5 | 0x11 | 0x0045b830 | **no row** | none | `shotgun.dff` (tail jmp 0x0045b810, no row) |
| 6 | 0x12 | 0x00454e70 | **no row** | none | `flashunit.dff`, `lensflare-fs8` (tail jmp 0x00454e40, no row) |
| 7 | 0x13 | 0x004576f0 | **no row** | none | `oil`, `oilShine`, `oilslicker.dff` |
| 8 | 0x07 | 0x00452f70 | **no row** | none | `smoke`, `mortar.dff`, `MortarRound.dff` |

Seven of the nine are C0: no hooks.csv row, no plate, no match source. Their two tail targets are C0 too. 0x004b3fc0
(C2, no match source) is called by 8 of the 9. That is RE-axis MATCH work, and it needs Ghidra, which this account
(claude2) cannot run (re/ACCOUNT2_CAPABILITIES.md). Sizes from a capstone walk to the last `ret`/tail jump: 0xb3..0x1c1
bytes each, 9..18 calls each.

### Part 3a: Lua

0x0047b9b0 = `0x0047b860(); cb(); 0x0047b8d0(name, out); 0x0047b880()` (re/match/s13/0047b9b0.c). 0x0047b860 calls
0x004b7330 (lua-4.0, C2) and 0x004c0510 (lua-4.0). 0x0047b8d0 calls 0x005507b0, 0x00550910, 0x00550950, 0x005509b0 and
0x00550bc0 (VFS / stream layer, port none except 0x00550bc0). The callback 0x00440bc0 (the command registrar,
0x00440bc0..0x00440d40) has no hooks.csv row. The exe has no Lua 4.0 VM. The default path reads `powerups_all.lua`
through the hand runner `Save::RunTrackPowerupsScript` (Race/GameFlow.cpp `ApplyBootPowerupsScript`, D4-FU-SETTRACKPU).
This is the "large unmatched subtree" case. Porting the Lua 4.0 library or binding 0x0047b9b0 to the hand runner is an
architecture-level choice, so it needs an L-decision.

### Part 3b: the five builders (reachable)

| rva | match source | callees not yet in BINDINGS.tsv |
|---|---|---|
| 0x004039c0 | re/match/s01/raw/004039c0.c | 0x0042a5d0 (exe body `MatchS06_0042a5d0.cpp`, port body) |
| 0x00453f30 | re/match/s09/00453f30.c | none (no calls) |
| 0x00459290 | re/match/s09/00459290.c | 0x00474d60 (C4, port none), 0x00476c10 / 0x00476cb0 (exe bodies, port called) |
| 0x004587a0 | re/match/s09/004587a0.c | 0x004b3e40 (C4, port none), 0x004b65a0 / 0x004b65b0 (C4, port none), 0x004b3b70 (exe body FileWrappers.cpp) |
| 0x00411f30 | re/match/s02/00411f30.c | 0x004b3e40, 0x004b68e0 / 0x004b68f0 (C4, port none), 0x004781b0 (C4, port none), 0x004b5580 (C4, port none), 0x004b3b70 |

0x004b3e40's own callees are already bound: 0x004cc230 / 0x004cc160 shims, 0x004cc5e0 bound, and 0x004e7420
`MashedRw_ClumpStreamRead_004e7420` (rw). 0x004781b0 adds `RpClumpStreamRead` and 0x004ce2d0 (bound). 0x004b3f90
(called by 0x00411f30) is already the shim `MashedRw_ClumpFirstAtomic_004b3f90`. So the builders, and with them goals
(a), (b) and (c), need about 9 new exe TUs and no RE.

## Proposed split

| row | what | needs | delivers |
|---|---|---|---|
| **POWERUPINIT-A** (port, claude2 OK) | Swap 0x0045bae0 onto its matched body under MASHED_ORIG_TOP. Before the call, write exe stand-ins into the nine +0x14 slots of 0x005f9998 in the image pad (knob path only, each logs "type i: init stand-in, <reason>"). Bind 0x0047b9b0 to a stand-in (Lua 4.0 library not ported, see 3a). Bind the five builders to matched bodies with the ~9 callee TUs above. Settle the 0x0068b198 pool owner: the matched builder 0x004587a0 owns +0x00 end to end, the hand tick `BindAll` skips an entry whose +0x00 is a librw clump, and `PickupsOrig_ReleasePool` frees only `s_obj[i]`. Then show (a) the two 0x004768c0 selectors taking the matched branch and (b) swap 0x00412010 | one subsystem-sized commit, the RWMOD arm for the DFF stream reads | goals (a), (b), (c) of this row |
| **POWERUPINIT-B** (RE / MATCH, claude3, Ghidra) | Discover and match the 7 C0 inits (0x00452f70, 0x00454e70, 0x00455400, 0x004576f0, 0x004583a0, 0x0045a6b0, 0x0045b830) + tails 0x00454e40, 0x0045b810, + 0x004b3fc0 (C2, no source), + 0x0045a3a0 (C2). Note: they are reached only through the table, so Ghidra may not have them as functions (memory "Ghidra missed functions") | Ghidra MCP, match.py | matched sources, C4 by match |
| **POWERUPINIT-C** (port) | Replace the nine table stand-ins with matched bodies. Callees not on the path yet: 0x004b5190 (8 users), 0x004b5240 (5), 0x00474d60 (4), 0x004c13e0 (2), 0x004b52f0, 0x004548a0, 0x00456d00, 0x00458360, 0x00489940, 0x004c0ed0, 0x0040cda0, 0x004c39b0 (exe RwV3dNormalize) | B | the 0x005f9998 init table |
| **POWERUPINIT-D** (decision) | Lua on the knob path: port the Lua 4.0 library (0x004b7330, 0x004c0510, 0x005507b0.. stream layer, registrar 0x00440bc0), or bind 0x0047b9b0 to the hand `Save::RunTrackPowerupsScript` as a declared deviation | an L-n from Mariano | 0x0047b9b0 off its stand-in |

A does not depend on B, C or D.

## Bindings chain fix (done, committed)

`update_bindings_otother2.py` and `update_bindings_otpickuppool.py` each start with `update_bindings_othud2.py`, so
they are siblings. Shown on this tree: running `update_bindings_otpickuppool.py` alone rewrites 3 rows of the committed
BINDINGS.tsv, and they are exactly the three OTHER-2 rows (0x0048a830 matched -> stand-in, 0x0048cf70, 0x0048ffd0;
matched 202 -> 201). The file was restored afterwards.

New `verify/portorigtop_20261109/update_bindings_otpowerupinit.py` runs `update_bindings_othud2.py` once, then applies
the OTHER-2 `FIXED` table, then the PICKUPPOOL `FIXED` table (both imported from their scripts, not re-run), then its
own `FIXED` table, which is empty for this row. `--check-chain` stops after the three inherited steps:

| check | result |
|---|---|
| `update_bindings_otpowerupinit.py --check-chain` | 452 rows, `matched 202, stand-in 67` |
| `cmp` against `git show HEAD:verify/porttop_20261108/BINDINGS.tsv` | BYTE-IDENTICAL, sha256 `a33192406e58ee226f93d061e8f360ded6367d7a28108d9fa069757f58e06580` both |

Later rows chain from this script.

## Acceptance items

| item | status |
|---|---|
| ORIG_TOP boot RWMOD off / on | not run: no code change on either path |
| 12 captures OFF/ON/OFF/ON | not run: same reason |
| G-CTRL / B | unchanged by construction: the commit touches only `verify/` |
| rva-lint | `OK -- no NEW duplicate bodies (29 known)`, `ORIGTOP-ONLY=27 (28 marked TU(s))` |
| port column | not regenerated: no exe source changed |
