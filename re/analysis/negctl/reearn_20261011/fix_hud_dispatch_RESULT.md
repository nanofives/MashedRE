# fix/hud-dispatch: the 4 HUD dispatch rows, ported from the bytes (2026-10-12)

Branch `work/hud-dispatch`. Brief: port 0x0041a3e0, 0x0041c0c0, 0x0041c300, 0x00426ba0 (all C2, demoted 2026-10-11
because no body existed: `HUD/HudDispatch.cpp` only called the original RVAs). No tracker edits, no re-classify,
no path2 here: the parent does those after merging.

## Verdicts

| RVA | name | registry key | verdict |
|---|---|---|---|
| 0x0041a3e0 | HudDispatchMode10 | hud_dispatch_mode10 | **PORTED-GREEN** |
| 0x0041c0c0 | HudDispatchSlot2 | hud_dispatch_slot2 | **PORTED-GREEN** |
| 0x0041c300 | HudDispatchMode5 | hud_dispatch_mode5 | **PORTED-GREEN** |
| 0x00426ba0 | HudDrawEnabled | hud_draw_enabled | **PORTED-GREEN** |

All four bodies live in `mashedmod/src/mashed_re/HUD/HudDispatch.cpp` (one body per RVA, `extern "C"
__declspec(dllexport)`, `RH_ScopedInstall`). The file is in both `asi_sources.rsp` and `exe_sources.rsp`. No other
TU references these RVAs (grep of `mashedmod/src`), so there is no duplicate shim to delete. The existing
`Call0041a3e0()` / `Call0041c300()` / `Call0041c0c0()` / `CallHudDrawEnabled()` helpers in HudIngameDispatch call the
RVA, which now lands in these ports once installed.

## Ports and their RVA citations (from `original/MASHED.exe.unpatched`, capstone)

**0x0041a3e0 HudDispatchMode10** (20 bytes)
`0x0041a3e0 mov eax,[0x0063c628]` / `0x0041a3e5 test eax,eax` / `0x0041a3e7 je 0x0041a3f3` /
`0x0041a3e9 mov eax,0x0063c610` / `0x0041a3ee jmp 0x0041c2d0` / `0x0041a3f3 ret`.
Port: read the dword flag, and when non-zero load EAX = 0x0063c610 and call 0x0041c2d0 (inline asm). Returns the
EAX the original leaves: the flag (0) on the skip path, else 0x0041c2d0's EAX.

**0x0041c300 HudDispatchMode5** (20 bytes): same shape, flag `[0x0063cdbc]` (0x0041c300), block `0x0063cda4`
(0x0041c309), tail `jmp 0x0041c2d0` (0x0041c30e).

**0x0041c0c0 HudDispatchSlot2** (29 bytes)
`0x0041c0c0 push esi` / `0x0041c0c1 mov esi,0x0063cab8` / `0x0041c0c6 mov eax,esi` / `0x0041c0c8 call 0x0041bc50` /
`0x0041c0cd add esi,0x16c` / `0x0041c0d3 cmp esi,0x0063cd90` / `0x0041c0d9 jl 0x0041c0c6` / `0x0041c0db pop esi` /
`0x0041c0dc ret`. Two records (0x0063cab8, 0x0063cc24), no per-record guard, signed compare. Port: do/while with
EAX = record loaded by inline asm before each call. Returns the last 0x0041bc50 EAX.

**0x00426ba0 HudDrawEnabled** (6 bytes): `0x00426ba0 mov eax,[0x0066d704]` / `0x00426ba5 ret`. The compiled export
is byte-identical (`mov eax,[0x66d704]; ret`).

Register contract: both callees take their object in EAX (0x0041c2d1 / 0x0041bc51 `mov esi,eax`). Neither callee is
ported (both C2), so both are called at their original RVA. Call-site disassembly of every caller (2026-10-12): no
caller reads ECX or EDX after any of the four calls; only 0x00426ba0's EAX is consumed (`test eax,eax` /
`mov ebx,eax`). The compiled ports touch only EAX on their skip paths, as the originals do.

## Fixture design (`re/frida/negctl_fixtures/z_fix_hud_dispatch.py`)

Why the old vectors measured nothing: all four were `arg_type: none` / `read_global` entries for exports that did
not exist ("export not found in .asi", negctl 2026-10-11). Even with an export, a `none` call at the menu only takes
the early-outs (flags 0) or walks HUD records the menu never filled.

New entries, `scalars_to_scattered_globals` + `freeze_main` + `fold_ret`, ret `uint32`:
* `recorder` (1 arg, mscdecl; caller-cleaned per `0x0041c2ee add esp,0x10` / `0x0041bc64 add esp,4`) logs each
  dispatched object pointer to scratch+0, up to 64 calls. Fake objects at scratch+0x200+k*0x50 carry only
  `[obj+0x48]` = recorder (the one field read: `0x0041c2d7 call [eax+0x48]`, `0x0041bc61` the same). The log is the
  dispatch ORDER and the object set. 0x0041c2d0 and 0x0041bc50 run for real on both arms.
* mode10 / mode5 (8 vectors each): seed BOTH rows' flags and BOTH 4-dword blocks with disjoint objects, so a port
  that reads the other row's flag or passes the other row's block records a different log. Flags 0, 1, 0xdeadbeef,
  0x80000000, 0x00000100 (low byte 0: catches a byte test), 0xffffffff (with aliased members), 0/0, 1/1.
* slot2 (9 vectors): seed all 29 flag dwords of both records (flag offsets X from 0x0041bc53..0x0041be95, object at
  rec+X+0xa0) with per-record bit masks: none, all, one record only (each way), alternating, single first/last slot,
  flag value 0x80000000 and 0x100.
* hud_draw_enabled (10 vectors): seed 0x0066d704, fold EAX, observe the global.

Fingerprints are non-constant: distinct values per CSV = 10/10 (draw_enabled), 7/8 (mode10, mode5: the two flag-0
vectors coincide by design), 8/9 (slot2: the two all-flags vectors differ only in flag value, which 0x0041bc50 only
tests for non-zero).

## Evidence (`re/analysis/negctl/reearn_20261011/evidence/`)

| key | run_diff | --self-baseline | negctl real / baseline / null / ret / field_none | mutation |
|---|---|---|---|---|
| hud_dispatch_mode10 | GREEN 0/8 | GREEN 0/8 | GREEN / GREEN / RED 6/8 / RED 8/8 / CTRL-GREEN | RED 6/8 |
| hud_dispatch_mode5 | GREEN 0/8 | GREEN 0/8 | GREEN / GREEN / RED 6/8 / RED 8/8 / CTRL-GREEN | RED 4/8 |
| hud_dispatch_slot2 | GREEN 0/9 | GREEN 0/9 | GREEN / GREEN / RED 8/9 / RED 9/9 / CTRL-GREEN | RED 7/9 |
| hud_draw_enabled | GREEN 0/10 | GREEN 0/10 | GREEN / GREEN / RED 9/10 / RED 10/10 / CTRL-GREEN | RED 9/10 |

field_first / field_last read NA on all four (flips=0): these bodies write no memory themselves (the only writes are
the harness recorder's), so the field mutants have nothing to flip. null and ret are the discriminating mutants.

Files: `fix_diff_<key>.csv`, `fix_diff_<key>_selfbaseline.csv`, `fix_mutation_<key>.csv`,
`fix_negctl_hud_dispatch.tsv` (copy of `runs/fix_hud_dispatch/negctl_20261012_140159.tsv`).

**Deliberate mutation** (one build, one single-point change per row, then restored, touched, rebuilt with
"(source newer)" for both targets, and run_diff + self-baseline re-run for the committed CSVs):

| key | mutation | result | expected mismatches |
|---|---|---|---|
| hud_dispatch_mode10 | block `0x0063c610` -> `0x0063cda4` (mode5's block) | RED 6/8 | every flag != 0 vector (6) |
| hud_dispatch_mode5 | flag `0x0063cdbc` -> `0x0063c628` (mode10's flag) | RED 4/8 | vectors where the two flags differ in zero-ness (4) |
| hud_dispatch_slot2 | loop bound `0x0063cd90` -> `0x0063cc24` (one record) | RED 7/9 | every vector with record 1 non-empty (7) |
| hud_draw_enabled | global `0x0066d704` -> `0x0066d708` | RED 9/10 | all but the seed-0 vector (live 0x0066d708 read 0) |

Each RED count equals the count derived from the vector set (the mismatching vectors are exactly the ones the mutation changes).

**Other C3 rows in the touched file, re-run after the restore** (`fix_rerun_hud_dispatch_<key>.csv`):
hud_ingame_dispatch (0x0040dfc0) GREEN 0/7, hud_slot_loop_b630 (0x0041b630) GREEN 0/14, hud_slot_loop_ccc0
(0x0041ccc0) GREEN 0/14, hud_slot_update_cc50 (0x0041cc50) GREEN 0/3.

## C3 gate: callers and callees (callers by Ghidra function boundary, `decomp_pc.py --callers --no-decomp`)

| row | callers (level) | callees (level) |
|---|---|---|
| 0x0041a3e0 | FUN_0040dfc0 HudIngameDispatch **C3** (sites 0x0040e043 jmp, 0x0040e0bd call) | 0x0041c2d0 FUN_0041c2d0 **C2** |
| 0x0041c300 | FUN_0040dfc0 **C3** (0x0040e028 jmp, 0x0040e09e call) | 0x0041c2d0 **C2** |
| 0x0041c0c0 | FUN_0040dfc0 **C3** (0x0040e0d4 call) | 0x0041bc50 FUN_0041bc50 **C2** |
| 0x00426ba0 | FUN_00402a40 AppDestroy **C2**, FUN_0040d270 Course::Finish **C2**, FUN_0040dfc0 **C3**, FUN_00410b30 **C2**, FUN_004111c0 **C2** | none (leaf) |

All callees are C2+, so none blocks C3 under the callee gate.

## Plate corrections (made in this branch)

`re/analysis/hud_ingame/` and `re/analysis/hud_ingame_promote_c2/` plates for 0x0041a3e0 / 0x0041c300 / 0x0041c0c0
omitted the EAX argument (0x0063c610 / 0x0063cda4 / the record pointer) and the tail jmp, and gave the two mode
routers as 19 bytes (they are 20). Appended "Correction 2026-10-12" sections with the bytes. 0x00426ba0's plates were
right; appended the caller list.

## Findings outside the four rows (not fixed here; for the parent)

1. **`FUN_0041c2d0` port (same file, install MASS-DISABLED, row C2) does not match the bytes.** The original
   (0x0041c2d0..0x0041c2f2) is `push esi; mov esi,eax`, then for m = [esi+0xc], [esi+4], [esi], [esi+8]:
   `push m; call [m+0x48]`, then `add esp,0x10; pop esi; ret`. The port (a) dereferences a vtable
   (`mov edx,[ecx]; call [edx+0x48]`) where the original calls a function-pointer field of the member directly
   (one dereference), (b) never pops its four pushed args, and (c) keeps using EAX as the base after the first
   call, which the callee clobbers. Its hooks.csv note and plate ("vtable[0x48]") carry the same misreading.
   Installing it as written would crash. This branch does not call it: the two mode ports call 0x0041c2d0's
   original bytes.
2. The 0x0041c0c0 plate's caller condition said "DAT_007f0fd0 != 2"; from FUN_0040dfc0 that test applies only on the
   sub-mode 6/10 arm (the default arm calls 0x0041c0c0 whenever FUN_0042f500() != 0). Corrected in the plate note.

No [UNCERTAIN] items for the four rows.
