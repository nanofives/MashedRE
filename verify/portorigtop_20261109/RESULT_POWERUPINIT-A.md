# RESULT POWERUPINIT-A (2026-11-10): GREEN, 0x0045bae0 and its five builders on matched bodies under MASHED_ORIG_TOP, 0x00412010 swapped, 0x0047b9b0 bound by L-114

L2 worker `cmwb00r8i0tsrsc1bk4hkzlvs` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/POWERUPINIT-A` on
`orch/PORT-OT-POWERUPINIT` `a3f743d21` (the scope row, RESULT_PORT-OT-POWERUPINIT.md). Row POWERUPINIT-D (L-114 (b)) closes
with this branch. No queued branch merged in.

## What changed

| rva | before | now | TU |
|---|---|---|---|
| 0x0045bae0 | `PortTop_StandIn04E_0045bae0` | `Orig_0045bae0` via `PortTop_PWI_Call0045bae0` | new Match/PortTop_0045bae0.cpp = re/match/s10/0045bae0.c |
| 0x00459290 | (not reached) | `Orig_00459290` | new Match/PortTop_00459290.cpp = re/match/s09/00459290.c |
| 0x004587a0 | (not reached) | `Orig_004587a0` via `PortTop_PWI_Call004587a0` | new Match/PortTop_004587a0.cpp = re/match/s09/004587a0.c |
| 0x00453f30 | (not reached) | `Orig_00453f30` | new Match/PortTop_00453f30.cpp |
| 0x00411f30 | (not reached) | `Orig_00411f30` | new Match/PortTop_00411f30.cpp = re/match/s02/00411f30.c |
| 0x004039c0 | (not reached) | `Orig_004039c0` | new Match/PortTop_004039c0.cpp = re/match/s01/raw/004039c0.c |
| 0x0042a5d0 | exe body calling raw 0x004b3bf0 | exe body, 0x004b3bf0 -> `Orig_004b3bf0` (0 / 0 out pointers, the 0x0042a6b0 shape) | Match/MatchS06_0042a5d0.cpp (`#ifdef MASHED_STANDALONE`) |
| 0x004b3e40, 0x004b3b70, 0x004b65a0, 0x004b65b0, 0x004b68e0, 0x004781b0, 0x00474d60 | (not reached) | `Orig_<rva>` | new Match/PortTop_<rva>.cpp, one each |
| 0x004b68f0 | (not reached) | binding `PortTop_PizFs_EntryLength_004b68f0` | Match/PortTop_00402750_Pieces.cpp |
| 0x0047b9b0 | (not reached) | `PortTop_PWI_Call0047b9b0` -> `Save::RunTrackPowerupsScript`, **deviation L-114** | Match/PortTop_0045bae0.cpp |
| 0x004b5580 | (not reached) | stand-in `PortTop_PWI_StandIn_004b5580` | Match/PortTop_00411f30.cpp |
| the 9 `DAT_005f9998[i].init` slots (+0x14) | 0 (image pad) | stand-ins `PortTop_PWI_InitStandIn_0..8`, stored by the forwarder | Match/PortTop_0045bae0.cpp |
| 0x00412010 | `PortTop_StandIn04C_00412010` | `Orig_00412010` via `PortTop_0045b930_Call00412010` | new Match/PortTop_00412010.cpp = re/match/s02/00412010.c |

Every callee binds by name. No raw-address call is left on this path.

The group file s10/0045bae0.c also holds 0x0045ba10 / 0x0045bac0 (static), 0x0045bf30 and 0x0045bba0. They compile as
`Orig_0045bf30` / `Orig_0045bba0`, and nothing in the exe calls them: 0x0040cfd0 keeps the 0x0045bf30 stand-in, because
that body calls the +0x30 slots, which are 0. The MSVC linker refuses an unresolved external even inside an unreferenced
function (tested: LNK2019 with /OPT:REF). So the 11 callees of 0x0045bba0 and the registrar 0x00440bc0 bind to logging
traps `PortTop_PWI_NotRun_<rva>`, which no run reached.

### The type table (0x005f9998, slot +0x14)

The image pad holds 0x005f9998.. zero-filled, and only `DAT_005f9bd8 = 9` and `0x005f99c4` are seeded
(Compat/PortStateSeed.inc). So the matched loop would call address 0. When `DAT_005f9bd8 == 9`, the forwarder writes a
logging stand-in into each +0x14 slot that holds 0, then runs the matched body. Otherwise it logs and does not run.

| i | original +0x14 | reason it stays a stand-in |
|---|---|---|
| 0, 1 | 0x00456760, 0x004548e0 | matched sources exist, callees not on this path (0x004b3fc0 has no source). Row POWERUPINIT-C |
| 2..8 | 0x00455400, 0x004583a0, 0x0045a6b0, 0x0045b830, 0x00454e70, 0x004576f0, 0x00452f70 | no hooks.csv row, no matched source. Row POWERUPINIT-B |

No other exe code reads the +0x14 slots. 0x0045b930 calls the +0x18 slots (`PTR_FUN_005f99b0`), which stay 0.

### 0x0047b9b0: deviation L-114

The Lua 4.0 VM is not ported. 0x0047b860 opens a lua-4.0 state (0x004b7330, 0x004c0510), 0x0047b8d0 runs the chunk
through 0x005507b0..0x00550bc0, and the registrar 0x00440bc0 has no hooks.csv row. Decision L-114 (b) replaces that
call: the binding reads `powerups_all.lua` from the piz mount stack (`PortTop_Fs_ReadAll`) and runs the hand runner
`Save::RunTrackPowerupsScript` (Save/GameSaveFormat.h) on the span at 0x007f0a40. The script's `Set_Track_Powerups` calls
(0x0047b280) write that span's rows at 0x007f0cb0. The registrar is not called and the out argument is not written. The
comment at the binding site names L-114, and the BINDINGS row carries the deviation note. Porting the Lua library is row
LUA-PORT. Every knob-ON run logs `4925 bytes, 39 Set_Track_Powerups calls, 0 statements skipped, from piz`.

### 0x004b68f0: a piz file-system binding

0x004b65b0 -> 0x004b68f0 gives the read size for IconCube.dff / lazer.dff. The matched 0x004b68f0 returns entry +0x78 (the
length, Piz/PizReader.h) from the directory the original's piz reader copies to 0x0090dac0, count 0x008ad9a8. The copy
runs at 0x004b6a4a..0x004b6ab0 (capstone on MASHED.exe.unpatched). The exe's piz reader never fills that range, so the
binding answers from the mount stack: `PortTop_PizFs_Find` length, else 0. One difference: the original searches only the
directory it loaded last, while the binding searches newest mount first. Logs: `0x004b68f0("IconCube.dff") binding:
found, length 1076`, `("lazer.dff") ... 972`. Those are the piz_extract.py list lengths 0x434 / 0x3cc, inside the
0x800 / 0x400 buffers.

### 0x00412010 and RpAtomic +0x3c

0x00411f30 stores `0x004b3f90(clump)` in `DAT_0063bb38[i]`, and 0x00412010 destroys `*(record + 0x3c)`, which is
RpAtomic.clump in RW 3.x. The exe's 0x004b3f90 shim returns an RW-layout atomic view, `MashedRwAtomicView` in
LibRw/RwApiBind.cpp, which had no +0x3c field. The view gains `rw::Clump* clump` at +0x3c (static_assert), set to the clump
argument. The only other knob-path caller, 0x00401690, reads +0x04 / +0x18 only. The 0x0045b930 call-site forwarder runs
`Orig_00412010` when all six records are non-zero, and the stand-in otherwise.

### Pool 0x0068b198: one owner, and where it departs from the approved bullet

Owner end to end: the 25 clumps are created by the matched 0x004587a0 and destroyed by the matched 0x00458880. The hand
pickup code never reads or writes through them.

The approved bullet said BindAll skips librw clumps. I did not do that, because it would corrupt the clumps. The verbatim
tick `PK_459000` writes its frame through entry +0x00 -> +0x04 at RW 3.x RwFrame offsets: `FrameIdentity` zeroes
+0x10..+0x4f, and `PK_FrameTranslate` writes +0x40. librw's `rw::Frame` is `Object` (8), `inDirtyList` (8), `objectList`
at +0x10, `matrix` at +0x18 (deps/librw/src/rwobjects.h:48-51). So the tick would overwrite the clump frame's object list,
which `rw::Clump::destroy` walks. Instead (Gameplay/PickupTick.cpp):

- `PickupsOrig_SetPoolBuilt`, called by the 0x004587a0 forwarder, records each builder clump in `s_built[i]`.
- `BindAll` lends the word. It still binds `&s_obj[i]`, as on the default path, and marks the entry lent. A non-zero word
  that is neither the fake object nor `s_built[i]` is counted as foreign.
- `PickupsOrig_ReleasePool`, from the 0x0045b930 forwarder before the matched 0x00458880, puts `s_built[i]` back where
  the word holds `&s_obj[i]`. It frees nothing and counts any other word as foreign. The matched body runs only with 0
  foreign.
- The default path never sets `s_origPoolBuilt`, so `BindAll` there is the old code path.

Measured:
- **Boots, no nav:** `(released, foreign) (0, 0)`, `(clumps, lent) (25, 0)`.
- **Nav ON runs:** `(25, 0)` and `(25, 25)`. The hand tick borrowed all 25 entries, got them back, and the matched body
  destroyed the 25 builder clumps. Every run exited with WinMain 0. This is the state that AV'd in the GAMEPLAY first try.

### Asked for in the brief

- **(a) Both 0x004768c0 selectors take the matched branch:**
  - `rva 0x004768c0(0x0068B968): matched body (record +0x04 set)` in every knob-ON run, RWMOD off and on.
  - `rva 0x004768c0(0x0068B9B8): matched body (record +0x04 set)` in every RWMOD-on run. 0x004593b0 runs only with RWMOD.
  - Record +0x04 after the builders, for example: 0x0068b968 +0x04 = `090BFCF8`, 0x0068b9b8 +0x04 = `090C3958` (knob_on_1).
- **(b) 0x00412010 swapped:** `rva 0x00412010: matched body (records built, of 6) (6, 6)` in every knob-ON run.
- **(c) Pool owner:** settled as above.

### Source edit (DDK text unchanged)

re/match/s01/raw/004039c0.c: the g2s preamble is now `#ifndef __cplusplus` (the C++ branch defines `undefined4` only), and
`extern undefined4 DAT_00636c00` is `#ifndef MATCH_PORT`. The port defines `DAT_00636c00` as the original address, and
`FUN_0042a5d0(name, ...)` drops the two extra cdecl arguments the source pushes.

| source | match.py --bytes | relocheck |
|---|---|---|
| s10/0045bae0.c | 0x0045ba10 140 B, 0x0045bac0 28 B, 0x0045bae0 181 B, 0x0045bf30 98 B, 0x0045bba0 738 B: all BYTE-IDENTICAL | OK 1/0/19/8/33 |
| s09/00459290.c | 280 B BYTE-IDENTICAL | OK 18 |
| s09/004587a0.c | 214 B BYTE-IDENTICAL | OK 17 |
| s09/00453f30.c | 33 B BYTE-IDENTICAL | OK 1 |
| s02/00411f30.c | 216 B BYTE-IDENTICAL | OK 15 |
| s01/raw/004039c0.c (after the guards) | 23 B BYTE-IDENTICAL | OK 2 |
| s18/004b3e40.c, 004b3b70.c, 004b65a0.c, 004b65b0.c, 004b68e0.c | 110 / 61 / 5 / 5 / 6 B BYTE-IDENTICAL | OK 4 / 3 / 1 / 1 / 1 |
| s12/004781b0.c, s11/00474d60.c, s02/00412010.c | 80 / 28 / 61 B BYTE-IDENTICAL | OK 4 / 2 / 4 |

### rva-lint

5 new ORIGTOP-ONLY pairs, each TU carrying `// RVA-LINT: ORIGTOP-ONLY` and a tagged line in
re/tools/dual_copy_allowlist.txt:
- DUP-IN-TARGET 0x0045bac0 (vs Powerup/PowerupSystem.cpp)
- DUP-IN-TARGET 0x004b3b70 (vs Save/FileWrappers.cpp, which returns 0 in the exe)
- CROSS-TARGET 0x00453f30 (vs Match/MatchS09_00453f30.cpp)
- CROSS-TARGET 0x004b65a0 and 0x004b68e0 (vs Util/PromoLoop_round30.cpp)

The first build also flagged 0x004587a0 and 0x004b302f. Both were false call-through hits from comment lines that started
`// 0x00…` above a definition (scripts/rva_body_scan.py `RE_RVA_COMMENT`), and the comments were reworded. Final:
`D4 burn-down=2 ORIGTOP-ONLY=32 (33 marked TU(s))`, `OK -- no NEW duplicate bodies (34 known)`.

## Acceptance

Machine rules:
- **Builds:** each build ran only with no `%TEMP%\mashed_machine_priolinger_*.lock` and no SOLO lock. One exception to
  report: the first full build ran while `mashed_lock` was held by PORT-OT-RENDER-2's gate run (pid 5364). The build rule
  names only priolinger and SOLO locks, and every later build waited for `mashed_lock` to be free too.
- **Launches:** every launch was `test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label
  POWERUPINIT-A[-gates] -- ...'`.
- **Roots:** private roots `D:/MashedWT/POWERUPINIT-A_root` (boots, captures) and `_rootA` (gates), each with `original` =
  junction to `D:/MashedWT/PORT-TOP-04J_rootK/original`.

| check | result |
|---|---|
| syntax check | `cl /Zs /EHa /W3 /O2 /std:c++17 /DMASHED_STANDALONE` on the 14 new and 5 edited TUs: no error |
| build (`MASHED_NO_DEPLOY=1`, final tree) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed`. mashed_re.exe sha256 `c4caedded9e59d2cbe210bd4cba64c863dd5aa934df91603015ee4ba9d4f3274` |
| knob-ON boot, RWMOD off (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `exited rc=0 (0x00000000) after 52.5 s`. :382 25 of 25 pool clumps. :383 L-114 runner 39 calls. :414 DAT_0068d4e0 / DAT_0068bb58 / 0x0068b9b8 set. :574 0x00412010 matched (6, 6). :582-583 0x00458880 matched (0, 0), (25, 0). :609 selector 0x0068B968 matched. :617 0x004593b0 stand-in (RWMOD off). :952 `WinMain 0x00492370 returned 0 after 3000 frames` |
| knob-ON boot, RWMOD on (same + `MASHED_ORIG_TOP_RWMOD=1`) | `exited rc=0 (0x00000000) after 52.1 s`. :579 0x00412010 matched. :587 0x00458880 matched. :614 selector 0x0068B968 matched. :622 `0x004593b0: matched body (RWMOD on)`. :627 selector 0x0068B9B8 matched. :966 `WinMain ... returned 0 after 3000 frames` |
| 12 captures, RWMOD off (`CAPFLAKE_DATA=<data> CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. ON logs: 0x00458880 matched (25, 0), (25, 25), selector 0x0068B968 matched, `WinMain returned 0 after 578 frames` |
| 12 captures, RWMOD on (arm `nav_rwmod`, `MASHED_ORIG_TOP_RWMOD=1` in every run's env) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. ON logs add `0x004593b0: matched body (RWMOD on)` and selector 0x0068B9B8 matched |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows of runs.tsv. No other capture moved (eqE=12 in every run) |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint | OK, 34 known, 0 new |

Default path: every change runs only on the MASHED_ORIG_TOP path, so no default-OFF knob or PREREG applies:
- The new TUs are reached only from the matched 0x00402750 / 0x0045b930.
- `s_origPoolBuilt` is set only by the 0x004587a0 forwarder.
- `MatchS06_0042a5d0`'s exe callee used to be a raw call that the exe cannot run.
- The atomic view is returned only by the knob-path 0x004b3f90 shim.

The gates above are unchanged.

## BINDINGS and the port column

`verify/portorigtop_20261109/update_bindings_powerupinit_a.py` runs `update_bindings_otpowerupinit.py` (the chain tip:
HUD-2 once, then the OTHER-2 and PICKUPPOOL rows, byte for byte against the committed file at a3f743d21). Then it writes
this row's rows:
- 0x0045bae0 stand-in -> matched.
- New matched rows: 0x00459290, 0x004587a0, 0x00453f30, 0x00411f30, 0x004039c0, 0x004b3e40, 0x004b3b70, 0x004b65a0,
  0x004b68e0, 0x004b65b0, 0x004781b0, 0x00474d60.
- 0x0042a5d0 matched with its new binding.
- 0x004b68f0 bound.
- 0x0047b9b0 port, with the "deviation L-114" note.
- 0x004b5580 and the 9 init slots as stand-ins.
- 0x00412010 stand-in -> matched.
- Notes appended to 0x004768c0, 0x00458880 and 0x004593b0.

Result: 452 -> 477 rows, matched 202 -> 217, stand-in 67 -> 75, port 44 -> 45, bound 42 -> 43.

`backfill_exe_file.py`: 18 rows given an exe_file, 2 with more than one TU (0x0045bac0, 0x004b3b70), `old value changed:
2`. Then `backfill_port.py` on this build: `none=5111 body=552 called=492`, 18 rows changed, all this row's:
- none -> called: 0x004039c0, 0x00411f30, 0x00412010, 0x00453f30, 0x004587a0, 0x00459290, 0x0045ba10, 0x0045bae0,
  0x00474d60, 0x004781b0, 0x004b3e40, 0x004b65a0, 0x004b65b0, 0x004b68e0.
- body -> called: 0x0042a5d0, 0x004b3b70.
- none -> body: 0x0045bba0, 0x0045bf30 (compiled, not called).

No drift row.

## Raw files (D:/MashedWT/POWERUPINIT-A_data, not committed)

| file | sha256 |
|---|---|
| build.log (final build) | 0ba7a00751482e4c9c77e0ebc15e32af5f788afca7f72c62db902a6a3a83f0f0 |
| mashed_re.exe (copy of the measured build) | c4caedded9e59d2cbe210bd4cba64c863dd5aa934df91603015ee4ba9d4f3274 |
| knob_on_1.log / .stdout | 1c4c99dc4f8338ce660b5c3a2b1e00cb927e2c8d39558e742672a7870e9c96d2 / 2d4567e781d11fa796ad7c75cde2780eb650c542d9bcf602901b69baf3ce552d |
| knob_on_rwmod.log / .stdout | 36e78992717ec9d21fffb29e25aa01b1218f067c49b48dbcdafb64d1516d477f / 3843c73a75f4583fdd7b524380d5cbd346c845d1dc74c64f183cb76083e1bafe |
| runs.tsv | f560646e9825f27b38517ec0c4d851cd17931fc8944763b7fbaa1d1204796438 (same 8 rows as PORT-OT-PICKUPPOOL's) |
| capflake_nav1.stdout / capflake_rwmod.stdout | 26145dc42fb904ea55fde4e36869347a7d1cd28176874d5c6aa7ebf1e8c0af01 / 7da690b00dff287fd6de236f3189bf1213b2e6e2a64f60ca0a7ee30b437b3778 |
| nav1/r01..r04 logs | 125f3e97… / f28f7c63… / 1f7d3917… / 4586c494… |
| nav_rwmod/r01..r04 logs | 47e5b999… / 750fb6df… / 0d0a1389… / ac642bce… |
| arms.stdout | 887542b1d441a7461d40c946fdf7b6dbe68daa7521b21b132ba3e5f463192bf0 |
| arms/H_DEF.step.csv / gates.csv / ptrace.log | 8284be46b3c6… / 6793183492e0… / 31fe043e261d… |
| arms/B_DEF_K0, K1, K26 .step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| match.txt (match.py + relocheck, 14 sources) | 3e65c72b936b60e87e85fa1beecd8e65054aec3129f31e58ed58f06889b25c8c |

## Next

- POWERUPINIT-B (claude3, Ghidra): match the seven C0 inits and their tails 0x00454e40 / 0x0045b810, 0x004b3fc0 and
  0x0045a3a0.
- POWERUPINIT-C: replace the nine stand-ins in PortTop_PWI_Call0045bae0 with matched bodies. The seeding loop already
  skips a non-zero slot, so a matched pointer can be stored there instead.
- 0x004b5560 (the 0x004b5580 callback): no row and no source, same lane as B.
- LUA-PORT: the Lua 4.0 library, to retire the L-114 binding.
