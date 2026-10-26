# RESULT D4-WSJ-SFXMAP: race SFX events mapped to the original's call sites

**Verdict: GREEN (text-only deliverable).** Branch `orch/D4-WSJ-SFXMAP` off `orch/integration` @ `86bdd5121`.
L-24 = (a) (Mariano 2026-10-26): make the AudioEngine stand-in faithful by data. This row is the caller table.
Inputs: `verify/d4scope_20261026/SCOPING_D4-WSJ.md` G5 G6, `re/analysis/audio_character_banks_REmap_20260616.md:164-165`,
`verify/wsjref_20261026/RESULT_D4-WSJ-REF.md` + `ref_tables.md` (wave names, voice slices, measured return addresses).

**Deliverable: `verify/wsjsfxmap_20261026/sfx_caller_table.csv`** (46 rows, 14 columns). One row per (event, call site):
wave bank + index + name + voice slice, the play function, the call-site RVA, the containing function, the trigger
read off the decompilation, the guard, the REF-measured return address where one exists, and the port site with a status.

Solo window respected: no build, no boot of either exe, nothing under `original\` opened. Every fact below is from
headless Ghidra on pool slot `Mashed_pool11` (`-readOnly -noanalysis`, the anchored MASHED.exe), released after.

## 1. Method (static, reproducible)

- `re/tools/ghidra_scripts/SfxSites.java` (new, committed): every reference to each SFX play/stop/update function plus the
  10 instructions before the call, so the pushed wave index is read off the listing; plus every instruction referencing the
  permdict descriptor table `[0x006044f0, 0x006044f0 + 43*0x94)`, decoded to (wave index, field).
- `re/tools/ghidra_scripts/EntryBefore.java` (new): candidate entry for call sites Ghidra places in no function (9 sites,
  all in powerup bodies). Each candidate was confirmed with `decomp_pc.py --create` (transient, discarded).
- `re/tools/ghidra_scripts/DumpData.java` (new): raw dword / float of the thresholds cited below.
- Existing `XrefRange.java` / `DisasmPC.java` / `decomp_pc.py` for the guard globals, the event ring and the disassembly.

Wave index decode: `FUN_00465ca0(idx, pos)` reads `(&DAT_00604570)[idx*0x25]` = `0x006044f0 + idx*0x94 + 0x80` (voice base)
and `+0x84` (voice count) (`0x00465ccb..0x00465cdc`). Direct `FUN_005a66d0` callers read a fixed descriptor
(`DAT_00604f44` = idx 17, `DAT_00605478` = idx 26, `DAT_0060575c` = idx 31, and so on: the permdict census in the raw data
lists every one). Course-bank users read `*(DAT_0088e670) + idx*0x90 + 0x80`.

**Cross-check with REF:** every return address REF measured on a race-phase start (`0x0046375c`, `0x00463777`,
`0x00464b6b`, `0x00463019`, `0x004630d0`, `0x0046316c`, `0x00465d41` via `0x00458a2b` / `0x004535f2` / `0x004535fa` /
`0x004566bd`, `0x00465c54` via `0x00422239` / `0x0042227a`, `0x004659c1`, `0x00464619`, `0x0046322f`) is the instruction
after a call site in the table. The wave index each site pushes equals the wave REF decoded from the voice slice. 0 misses.

## 2. The voice-budget guard (resolves REF's `fire` [UNCERTAIN] and explains G6's empty captures)

`DAT_008aa254 = FUN_0040e340()` at `0x00466b68`, every audio tick (`FUN_0040e340` = hooks.csv `GetLiveCarCount`, C4).
`DAT_0068f4c8` is written only in `FUN_004627f0` (SoundEngine HW init): `0` at `0x004628c4`, then `1` at `0x004628d8` when
`DAT_006904a0` (= word `[ESP+0x22]` − 4, `0x004628b1..0x004628bf`, the structure filled by `FUN_005a9e40` at `0x00462855`)
is `< 0x30` (`0x004628bc` / `0x004628ce`), or when dword `[ESP+0xc]` != 0 (`0x004628d0..0x004628d6`).
REF measured `DAT_006904a0` = 28 and `DAT_0068f4c8` = 1 on this machine, `DAT_008aa254` = 2 (o_train1) / 4 (o_train2, o_pu1).

With `DAT_0068f4c8 != 0` and `DAT_008aa254 > 1` the original skips:

| what | where the guard is | effect |
|---|---|---|
| `fire` (perm 32) starts | `FUN_00465ca0` `0x00465ca0..0x00465cb7` (`CMP [ESP+4],0x20` / `JZ 0x00465d9b`), same test in `FUN_00466000` / `FUN_00466100` | asked 3720..12520 times per capture, 0 starts. The REF [UNCERTAIN] (state gate vs free-voice test) is answered: neither, this guard |
| all impact one-shots, perm 9..13 | `FUN_00464670` `0x00464671..0x00464695` (`JG 0x004647de`) | 0 impact starts in all captures |
| skid (course 0..3) | `FUN_00463c80` entry | 0 skid starts |
| tyre roll (course 4..6) | `FUN_00463f40` entry (inverted test returns) | 0 tyre starts |
| per-car 3-state impact (course) | `FUN_004642f0` entry | 0 starts |

So on the reference hardware every multi-car race is silent for collisions, skids and tyres, by design of the original,
not by a missing contact. G6's "were contacts filtered or absent" is answered statically: filtered, at these guards.
`carlands` (`FUN_004644a0`) is NOT behind the guard. Neither is the explosion, powerup or pickup path.

## 3. Event families (full rows in the CSV)

**Countdown (`FUN_00462f50`, gated `FUN_0040e350()==5` and `FUN_0042f6a0()!=0xb`).** State `DAT_006904e4` on the value
`FUN_0041da90` writes: `threetwoone` (perm 17, voice 65) attempted at 0.34 / 0.82 / 1.3 (`0x005cd238` / `0x005cd234` /
`0x005cd230`) from `0x00463014`, only if the voice is not playing. `go` (perm 26, voice 95) at 1.86 (`0x005ccdf4`) from
`0x004630cb`. Both freq 22050.0 (`0x46ac4400`), vol 0x3f19999a. REF's T+91 spacing matches 1.86 − 0.34 = 1.52 at 60 per
unit [UNCERTAIN: the unit of the `FUN_0041da90` value was not read this step]. Port: `threetwoone` at round start
(`TrackRenderer.cpp:7313`), `go` on `ba8c==6` (`:5021`, `:5095`).

**Powerups (fn names from the port's own table, `Powerup/PowerupSystem.cpp:21-40`).**

| type | original start(s) | port today |
|---|---|---|
| GUN FIRE `0x004561c0` | machineg (19) `0x004566b8` | `"shotgun"` (`PowerupEffects.cpp:128`): wrong wave |
| DRUM FIRE `0x00454740` | rocket (18) `0x0045477b` | `"drop mine"` (`:191`): wrong wave |
| MISSILE FIRE `0x00455150` | rocket (18) `0x0045532f` + missile exhaust (20) `0x0045533b`, fixed slot `FUN_00465e80`; in-flight pos `FUN_0045dce0` `0x00455f1e`; stop `FUN_004661a0` `0x00455985` | `"missile exhaust"` only (`:255`), no update, no stop |
| P_MINE FIRE `0x00457ef0` | drop mine (27), tail `JMP` at `0x00457f1c`, arg written at `0x00457f14` | `"drop mine"` (`:333`): matches |
| R_FLAME ARM / TICK / DEACT | flame thrower (22): start `0x0045a83c`, pos+vol `0x0045a647`, stop `0x0045a8fc`, slot = player | nothing |
| SHOTGUN FIRE `0x0045b6e0` | shotgun (21) `0x0045b3aa` in `FUN_0045b390` | `"shotgun"` (`:581`): matches |
| FLASH FIRE `0x00454db0` | flash (24) `0x00454df0` | `"flash"` (`:637`): matches |
| OIL FIRE `0x00457800` | oilslick (23) `0x00457a06`, slot = player | nothing |
| MORTAR FIRE `0x004533b0` | rocket (18) `0x004535ed` + mortarexhaust (30) `0x004535f5` | `"explosion1"` (`:731`): wrong wave |

**Explosions.** `FUN_00453eb0` (damage sweep) → `FUN_00465940` at `0x00453f11`: explosion1 voices 36..39 (base+4..7),
pitch `i*2000 + 22050`. Its callers include the mortar shell `FUN_00453210`, drum tick `FUN_00454350`, missile terminal
`FUN_00455910`, P_MINE tick (`FUN_00458080` from `0x00458339`), `FUN_004818d0`, and the copter / launcher code.
Separately `FUN_004645c0` starts explosion1 voice 32+car (`0x00464614`) while `[0x00881f90+car*0xd04]==2` and
`[0x00881f94+car*0xd04]>0`. REF o_pu1 saw 3 + 1 of these. Port: one `explosion1` on spin-out (`TrackRenderer.cpp:5947`).

**Pickup.** `FUN_00458920` (registered as a callback from `FUN_00459000`, data ref `0x0045925c`, dispatcher `FUN_0045bba0`)
starts `collision with pod` (28) at `0x00458a26` and `FUN_00465ca0` then increments `DAT_008aa2d0`. The port runs it as a
counted no-op (`Gameplay/PickupTick.cpp:31`).

**Collisions (behind the §2 guard).** The producer is the 256-slot event ring `DAT_007e9de0` (head) / `0x007e9de4` (type) /
`0x007ea1e4` (frame stamp) / `0x007ea5e4` (car + 0xff00). `FUN_004648b0` (from the audio tick) plays only events stamped with
the current `DAT_007f1030`: type 3 → `FUN_00464670(car,0)` impact with barrier (push `0x00469cc8`, `VehicleContactScanUpdate`);
type 4 → kind 1 impact with other vehicle 1 (pushes `0x0046aed4` / `0x0046b089`, `VehicleVehicleCollisionImpulse`);
type 8 → kinds 2/3/4 crate / snowplough / mushroomhit (pushes `0x0047e5f1` / `0x0047e7b2`, `FUN_0047def0`);
type 0xb → kind 3 (pushes `0x0044bb5a` / `0x0044c413`). Wave = 9 + kind (`0x00604aa4 + kind*0x94`), voice = base + car.
**The port omits every one of these ring pushes as "dev-viz only"** (`Collision/CarWorldContacts.cpp:486`,
`Collision/CarCarContacts.cpp:297`, `:509`; `Collision/RwpIntegrator.cpp:16-19`). The ring is not dev-viz: audio reads it.

**Other race-frame sources.** helicopter (14) 3 nearest of `FUN_00425ef0()` entities, `0x00464b66`, every race frame (REF:
3 starts at tick 0, port has none); helicopterKC (38) when `FUN_004075a0()==1`; noise burst (39) `0x00463167`, started by
`FUN_004656e0` right after each character stream; carlands (31) `0x00464558`; debris (33) `FUN_00465c10` `0x00422234` /
`0x00422275` from Race::Tick `0x0040fcf3` (REF: 4 starts per capture, no port site found by grep for `004222d0` /
`0040fcf3`); SPLASH (29) `0x0048b427`; copter gun / missile / explosion (`FUN_00405ab0`, `FUN_00406070`, `FUN_004064c0`);
beat-the-bomb set (34/35/36) from `FUN_004039f0`; timingbeam (25) only with `DAT_008aa254<2`.

## 4. Port status summary (CSV `port_status`)

30 MISSING, 4 WIRED, 3 WRONG-WAVE (GUN, DRUM, MORTAR), 2 PARTIAL (MISSILE, damage-sweep explosion), 1 WRONG-CLASS (engine),
1 timing and 1 trigger difference (countdown), 1 EQUAL (fire, silent on both sides), 3 OUT-OF-SCOPE (mode 6 launchers,
Egypt push columns, frontend menu SFX).

## 5. For D4-WSJ-WIRE (not done here: behaviour changes need a PREREG and a default-OFF knob)

- The guard in §2 must be part of the stand-in, keyed on the same two inputs. Open choice for the WIRE PREREG: the
  standalone has no `FUN_005a9e40` capability struct, so `DAT_0068f4c8` must either be pinned to the reference value 1
  (diffable against every REF capture) or derived from the stand-in's own voice budget. Recommendation: pin to 1 by default
  under the knob, since every reference capture was taken with 1.
- First-free scan per wave: `FUN_00465ca0` (perm) and `FUN_00465da0` (course twin, new transient fn at `0x00465da0`)
  scan `count` voices from `base` and start the first with `(voice+0x34 & 2) == 0`. `FUN_00465c10` / `FUN_00465e80`
  start a fixed slot (`base + arg`). `FUN_0045dce0` / `FUN_00465f40` / `FUN_00466000` / `FUN_00466100` only update a
  voice that is already playing.
- Highest-yield rows measured by REF: helicopter (3), debris (4), collision with pod (2..4), noise burst (1), mortar rocket +
  mortarexhaust (3 + 3), machineg (4), explosion1 (4), engine class + d-layer (ENGCLASS row).

## 6. Side result for D4-WSJ-ENGCLASS (not claimed, that row owns it)

`FUN_00463640` (per car) sets `DAT_0069055c = switch(FUN_0041efc0(car) / 6)`: {0,4,5,0xb} → 3, {1,3} → 1, {2,6,8,9} → 2,
default → 0 (writes `0x00463690..0x004636d9`). Engine wave = `2*DAT_0069055c` (stride 0x4a dwords at `0x00463725`), voice =
base + car (`0x00463757`); the d-layer is voice + 4 (`0x00463772`, `DAT_0068f660`) when `DAT_006130bc != 0` (set 1 at
`0x00463682`). `FUN_0041efc0(car)` reads `*(*(0x0063dc48 + car*0x2ac) + 0x80)`. REF's eng2 means `DAT_0069055c` = 1 there.

## 7. Uncertainties

- Type 0xb ring event: producer meaning not read; it plays only when `FUN_004a2c48()` (an `__ftol` of ST0) returns 0 or 1.
- Countdown value unit (`FUN_0041da90` output).
- Mode-6 dispatch of the launcher family taken from the hooks.csv row for `0x004189a0`, not re-read.
- Port sites marked "none found" are from grep over `mashedmod/src/mashed_re` for the RVA and the wave name. A port body
  that implements the trigger under another name without citing the RVA would be missed.

## Gates

Not run: solo window (no build, no boot). Nothing in `mashedmod/` changed, so the exe is the `orch/integration` exe.

## Raw data (not committed), SHA-256

Root `D:/MashedWT/D4-WSJ-SFXMAP_data/` (decompiled bodies under `decomp/`, 680 KB total):

| file | SHA-256 |
|---|---|
| sites.txt | 107602defd702e38b88e9ffe87a693de970ff630273735ce0b29a441c51a2d09 |
| sites2.txt | 913671bdac782865b3fba0f638f3a351aae5dbf7ca96cfcb165b239437c20342 |
| sites3.txt | 5d83e8f720cd1f878c918043e03cd26495345fd3e2ad58bbe4738005c0f775a5 |
| sites4.txt | a1d2c58ef71124d6e7499bac79def4d7bf762a59d174644e5feee6337372eaaa |
| sites5.txt | a657fe7ac36c4b8dbd1809317e552ebe80049201a75ed372a22154f2e63bcd03 |
| sites_summ.txt | 76c6470d46a3b49a4fb56d904466cd5605ea3aaa224a9dbd68657c64ef679226 |
| entries.txt | e69c4ffc5e253750fa2e6f91c206aa36b23e9f52b55ca4de7aeedf461dde8c20 |
| entries2.txt | 2a5d61a7db71505cf7ce40e099b09ccd6baa83030be4e1516c972992631f41b6 |
| x_ringtype.txt | 5e831d6b2be7c5b8af3dd835cf5715fcf2b4b611c0554ff596a18b9a1077b7df |
| x_ringall.txt | 760ed1222dd53706df0349b63d2dc64a8d36cf757f271d8f5029d4cd9311b981 |
| x_8aa254.txt | bf7103c288b403a9ff48a85c139a3d02ac07e49c8594d445b3f374eda4293d20 |
| x_68f4c8.txt | 1097c57489850f5df64335d7efa10496aa539a7c9561634a957145d0bb8b29f0 |
| x_69055c.txt | 23bbe671ab411f27ff1f4671fb34bd0a6664c3aab427404e42c5d461bab5fc93 |
| x_6130bc.txt | 0cb18413fdc8a76a061ca2b10ec54f28b9bdade3c01672ef4dc2f9cb5d484133 |
| dis_465ca0.txt | 6fe6b71362379097058e972acc9fadba3c6fac105ef9381094e8bdb1b4040cea |
| dis_464670.txt | 770bd07a54fae3a95efa00b23143eb0992b02995fd32e70a5bed2d1e125d1caa |
| dis_4627f0.txt | 567e727ac660bcfa809f8d79fdab06ab0bb37d6e225a31832a1bf4070b58e163 |
| consts.txt | c8303472544136ac341c84c6511be3422561c6d68bbb2cec4b6a861ec3d54963 |
| d1.json | 937979ba4f8d2c58f3483c2a7982e66fc254b15c127a725f99cb062ada29b21c |
| d2.json | 0ab42257c3f8d3125d20fcc1b2fb934431cb18f759264019b1f492436b4776ca |
| d3.json | d2a72661fc941dcdd0774deff7d34aa0fb67c9b49ca9fb8295926d61c18ec4be |
