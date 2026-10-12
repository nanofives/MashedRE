# negctl re-earn, cluster fstate (2026-10-12)

Branch `reearn/fstate` (from 606a8bbd). Five rows demoted by the 2026-10-11 negative-control
re-check, each re-ported from the disassembly of `original/MASHED.exe.unpatched`.
Code commit: b0acc28a. Build: `MASHED_NO_DEPLOY=1 mashedmod\build.bat` (worktree .asi only).

| RVA | hook key | verdict | run_diff | self-baseline | witness |
|---|---|---|---|---|---|
| 0x0042ebe0 | frontend_player_slot_check | FIXED | GREEN 18/18 | GREEN 18/18 | RED 1/18 |
| 0x00430290 | championship_complete | FIXED | GREEN 14/14 | GREEN 14/14 | RED 5/14 |
| 0x00430830 | split_screen_track_assignment | FIXED | GREEN 15/15 | GREEN 15/15 | RED 8/15 |
| 0x0046be10 | write_46be10 | FIXED | GREEN 6/6 | GREEN 6/6 | RED 4/6 |
| 0x0046cbe0 | vehicle_car_state_set | FIXED | GREEN 7/7 | GREEN 7/7 | RED 4/7 |

All five use arg_type `scalars_to_scattered_globals` (b03/b09 fixtures, plus the overlay
`re/frida/negctl_fixtures/z_fix_fstate.py`).

Negative control (`runs/fix_fstate/negctl_20261012_045830.tsv`, copied to
`evidence/fix_negctl_fstate.tsv`), `--classes null,ret,field_first,field_last,field_none --slots 1`:

| hook | real | baseline | null | ret | field_first | field_last | field_none |
|---|---|---|---|---|---|---|---|
| frontend_player_slot_check | GREEN 0/18 | GREEN 0/18 | RED 13/18 | RED 18/18 | NA | NA | CTRL-GREEN |
| championship_complete | GREEN 0/14 | GREEN 0/14 | RED 12/14 | SKIP (void) | RED 8/14 | RED 12/14 | CTRL-GREEN |
| split_screen_track_assignment | GREEN 0/15 | GREEN 0/15 | RED 11/15 | RED 15/15 | NA | NA | CTRL-GREEN |
| write_46be10 | GREEN 0/6 | GREEN 0/6 | RED 4/6 | RED 6/6 | RED 4/6 | RED 4/6 | CTRL-GREEN |
| vehicle_car_state_set | GREEN 0/7 | GREEN 0/7 | RED 4/7 | RED 7/7 | RED 4/7 | RED 4/7 | CTRL-GREEN |

Witness protocol: the five principal defects were reintroduced together in one build, one per
function (each run_diff measures only its own export), run_diff taken (RED, counts above), then
the five sources restored with `git checkout` + `touch`. The restore build log
(`log/build_fstate_restore.txt`) shows `(source newer)` and a compile line for all five TUs,
and run_diff was re-run on that build: GREEN 18/18, 14/14, 15/15, 6/6, 7/7. The
`evidence/fix_diff_<hook>.csv` files are from that post-restore run, not the witness run.

---

## 0x0042ebe0 FrontendPlayerSlotCheck (frontend_player_slot_check)

**Verdict: FIXED.** Source `mashedmod/src/mashed_re/Frontend/MenuHelpers.cpp`.

Defects (old port vs bytes):
1. The 9-row checks never set the flag. Any zero cell sets it: 0x0042ece8 `mov ecx,1`
   (reached from every `je 0x42ece8` in 0x0042ec18..0x0042ed51).
2. Wrong cells. p == 10 reads [0x7f0a48-4] (0x0042ebf5) and +0x2c,0x5c,...,0x1ac
   (0x0042ec18..0x0042ec7a); p == 11 reads [0x7f0a48] (0x0042ebff) and +0x30..0x1b0
   (0x0042ec8a..0x0042ecd4); p == 12 reads [0x7f0a48+0xc] (0x0042ec08) and +0x3c..0x1bc
   (0x0042ece1..0x0042ed49). That is columns 0x7f0a44 / 0x7f0a48 / 0x7f0a54, rows 0..9, stride
   0x30. The old port used stride 0x18 (p 10) and dword-index stride 3 from wrong bases (11/12).
3. Enable flag stride: `lea eax,[edx+edx*2] ; shl eax,4` (0x0042eced..0x0042ecf0), so
   [0x7f0a44 + p*0x30] (0x0042ecf3), not + p*0xc.
4. Return width: `xor eax,eax ; sete al` (0x0042ecf9, 0x0042ed02) gives a full dword 0/1. The
   old port returned `bool` (AL only).
5. Loop count: base 0x7f0a48, step 0x1e0 (0x0042ed53), `cmp eax,0x7f0c28 ; jl` (0x0042ed58).
   0x7f0a48 + 0x1e0 == 0x7f0c28, so the body runs exactly once. Kept as a literal loop.

Witness: rows 1..9 dropped (row 0 only), the "never sets bVar1 from the 8-field check" defect:
RED 1/18 (`evidence/fix_witness_row0only_frontend_player_slot_check.csv`), the demotion's count.

Confidence gate: leaf (no calls in 0x0042ebe0..0x0042ed64). Callers: 0x00439210 FUN_00439210
C4 (four sites 0x00439736, 0x00439a71, 0x00439e08, 0x00439e31).

Plate corrected: `re/analysis/promote_c2_frontend_menus/0x0042ebe0.md` (correction section).

## 0x00430290 Championship::Complete (championship_complete)

**Verdict: FIXED.** Source `mashedmod/src/mashed_re/Save/CareerEvents.cpp`. The full byte-cited
walk is the comment block above `ChampionshipComplete`.

Defects (old port vs bytes; cells are 0x007f0a40 + row*0x30 + col, R = DAT_0067f17c*0x30):
1. Mode 3 count read 6 consecutive dwords per 0x48 step. Bytes: column 0x7f0a44, rows 0..11
   (eax 0x7f0a74 step 0x120 < 0x7f0cb4, six cells per pass at eax-0x30..eax+0xc0),
   0x004302e2..0x0043031a.
2. Mode 3 post: FUN_0042a920 is called unconditionally inside the guard with id 0x264 / 0x267 /
   0 (0x004302de, 0x00430320, 0x0043032c, 0x00430332). The old port posted only on n == 2 / 9.
3. Mode 3 marks: +0x30, +0x2c, +0x5c of the record (0x0043035f [R+0x7f0a70], 0x00430365
   [R+0x7f0a6c], 0x0043036b [R+0x7f0a9c]); the old port wrote +0x1c, +0x18, +0x24.
4. Mode 3 next rows: a second count c of the same column (0x00430371..0x004303ab), then rows
   1..c with `p < 0x7f0c54` (0x004303c0 signed jge) and [p] != 2 get [p]=1, [p+8]=2
   (0x004303cb, 0x004303cd). The old port marked one next record with other fields.
5. Mode 4 did not reach FUN_004099a0. Bytes: after the 0x268 block it falls to the mode-5 test
   (0x004304d2) and the tail-jmp at 0x00430665.
6. Mode 4 prints: printf 0x004a2cbd with 0x5cd8c8 "silver add=%d\n" (0x0043048a), 0x5cd8b0
   "TOTALNUMTRACKS-3=%d\n", 10 (0x00430496), and 0x5cd8a8 "HERE!!\n" when n == 10 (0x004304a8).
   The old port omitted them. Now called through the original printf with the original format
   pointers (same pattern as Frontend/MenuMixed.cpp FrontendC2RoundI).
7. Mode 5: [R+0x7f0a54] = 2 is written BEFORE the all-done scan (0x004304fb). The scan is 18
   cells per pass, columns 0x7f0a44/0x7f0a48/0x7f0a54, rows 0..11 (0x00430503..0x004305aa);
   the old port read 3 fields of rows 0..9 with no early write. When all are 2, 0x26a is posted
   twice (0x004305c0 then 0x004305c9 with esi = 0x26a).
8. Mode 5 tail: [R+0x7f0a54]=2 and [R+0x7f0a5c]=2 (0x004305d1, 0x004305d7); count column
   0x7f0a54 rows 0..9 (0x004305dd..0x00430639). The old port wrote +0x14/+0x18 and counted +0x14.
9. Mode is re-read after FUN_0040dd60 (0x004302b5) and again at 0x004304d2, as in the bytes.

Fixture: z_fix_fstate.py adds five vectors b03 did not reach: n == 2 (0x264), n == 9 (0x267),
c == 12 hitting the 0x7f0c54 bound, gold n == 10 with [0x7f0c84] != 2 (0x269), mode-4 n == 10
with [0x7f0c54] == 2. 14 vectors, 13 distinct folded outputs.

Witness: mode-3 marks back at +0x1c/+0x18/+0x24 alone: RED 5/14
(`evidence/fix_witness_mode3marks_championship_complete.csv`).

Confidence gate: callees 0x0040dd60 Race::GuardConcludedAndP1Won C4, 0x0042a920
Frontend::PostTrophyEvent C4, 0x004099a0 Save::AutosaveTrigger C4, 0x004a2cbd CRT printf
(hooks.csv `FID_conflict:_wprintf`) C2. Caller: 0x00411170 TimeTrialRecordPlayback C3
(site 0x004111a1).

[UNCERTAIN] none new. hooks.csv row 0x004a2cbd is named `_wprintf`; the bytes at the three
format addresses are ASCII and Frontend/MenuMixed.cpp already records it as the narrow printf.
That is a naming item on the 0x004a2cbd row, not a gate for this row.

Plate corrected: `re/analysis/profile_career/00430290.md` (correction section).

## 0x00430830 SplitScreenTrackAssignment (split_screen_track_assignment)

**Verdict: FIXED.** Source `mashedmod/src/mashed_re/Frontend/MenuScoreSort.cpp`.

Defects:
1. Stride: every jump-table case computes `lea r,[eax+eax*2] ; shl r,4` = p*0x30 (e.g. mode 2
   0x0043084c..0x0043084f). The old port used 0xc for modes 2/3/4/5/6/9/10.
2. Columns from the jump table at 0x004308e4: 2 -> 0x7f0a44 (0x00430852), 3 -> 0x7f0a44
   (0x00430863), 4 -> 0x7f0a48 (0x00430874), 5 -> 0x7f0a54 (0x00430885), 6 -> 0x7f0a4c
   (0x00430896), 7 -> 0x7f0a60 (0x004308a7), 8 -> 0x7f0a64 (0x004308b8), 9 -> 0x7f0a68
   (0x004308c9), 10 -> 0x7f0a44 (0x004308da). These matched.
3. Mode test is unsigned (0x00430835 add eax,-2 ; cmp eax,8 ; ja 0x004308e1). Now read as uint32.

Fixture: z_fix_fstate.py adds mode 0xffffffff, p = -1 (mode 2) and mode 9 p = 9.

Witness: stride 0xc for every mode except 7/8: RED 8/15
(`evidence/fix_witness_stride0xc_split_screen_track_assignment.csv`).

Confidence gate: leaf. Callers: 0x004323c0 MenuCursorBack C3; 0x004322c0, 0x00432800,
0x0043af10, 0x0043dfd0 all C2.

Plate corrected: `re/analysis/frontend_promote_menus_b/00430830.md`.

## 0x0046be10 Write46be10 (write_46be10)

**Verdict: FIXED.** Source `mashedmod/src/mashed_re/Util/PromoLoop_sessionB.cpp`.

Defect: return value. 0x0046be41 `mov eax,1 ; ret`; the old port returned
`(idx*0x341+off)*4`. Guard (0x0046be14 cmp 0x10 ; jb, unsigned) and both stores
(0x0046be35 [off+0x88219c] = *src, 0x0046be3b [off+0x88221c] = val) re-read and matched.

Witness: `return off` alone: RED 4/6 (`evidence/fix_witness_retoffset_write_46be10.csv`), the
demotion's count.

Confidence gate: leaf. Callers: 0x00472560 and 0x004725f0 C3 (naked ports call 0x46be10 by
address); 0x0044cb00, 0x00451820, 0x00451ad0 C2.

Plate: `re/analysis/bucket_gameplay_0045dff0_0046dd90/0x0046be10.md` already says "Returns 1";
no correction needed.

## 0x0046cbe0 VehicleCarStateSet (vehicle_car_state_set)

**Verdict: FIXED.** Source `mashedmod/src/mashed_re/Vehicle/SmallLeaves_o5.cpp`.

Defects:
1. Store width: 0x0046cbfa `mov [eax+0x881f90],ecx` is a DWORD; the old port stored one byte.
2. Guard: 0x0046cbe4 `cmp eax,0x10` ; 0x0046cbe7 `jb` is unsigned; the old port compared
   `int > 0xf`, so a negative idx wrote below the table.
3. Duplicate implementation: Vehicle/Physics.cpp also installed 0x0046cbe0
   (VehicleSpinoutStateSet). It sits earlier in asi_sources.rsp, so it registered first and was
   the copy the .asi installed, while run_diff measured VehicleCarStateSet. Physics.cpp's body
   and RH_ScopedInstall are removed; the file keeps a comment pointing to SmallLeaves_o5.cpp.
   No registry key referenced VehicleSpinoutStateSet.

Fixture: z_fix_fstate.py adds idx = -1 and 0x80000000 and observes the "record -1" cells at
0x00881f90 - 0xd04 (pre-filled), which b09 had to leave out.

Witness: byte store alone: RED 4/7
(`evidence/fix_witness_bytestore_vehicle_car_state_set.csv`).

Confidence gate: leaf. Callers: 0x00422ba0 (sites 0x00422d6d, 0x00422d9a) and 0x00424eb0
(five sites) both C2.

Plate corrected: `re/analysis/util_c0_promote/0x0046cbe0.md` (unsigned guard, dword stores,
duplicate removed).

---

## Regression: other C3/C4 hooks in the touched files

Every hooks.csv C3/C4 row with a live `RH_ScopedInstall` in the six touched TUs, other than the
five above: 205 registry keys (199 in Util/PromoLoop_sessionB.cpp, 3 MenuHelpers.cpp,
1 MenuScoreSort.cpp, 1 CareerEvents.cpp, 1 SmallLeaves_o5.cpp; Vehicle/Physics.cpp now has none).
**205/205 GREEN** on the restored build. Table: `evidence/fix_siblings_fstate.tsv`.

- Lanes: the six non-PromoLoop keys via `run_diff.py`; the PromoLoop keys via
  `early_window_leaf_diff.py` (their recorded lane), except `delta_5c6b60`, which errors on both
  arms in the early-window lane ("expected a pointer", 2 boots, so a harness item, not a
  regression) and is GREEN 8/8 under `run_diff.py`, the lane of its re-earn evidence.
- Includes the two direct callers of 0x0046be10 that are ported: `build_472560` (0x00472560)
  and `initrange_4725f0` (0x004725f0), both GREEN.
- `run_diff_warm.py` was tried first and is not usable for this set: it ran early-window
  arg_types through the menu lane and reported RED on almost every key (`log/fstate_siblings_warm.txt`).
  Not used as evidence.
- Two transient infrastructure events, neither from this work: the host disk hit 0 bytes free
  mid-sweep (76 spawns failed "No space left on device", 60 result rows lost; all 136 re-run
  GREEN after space returned), and one `championship_complete` run_diff hit a Frida
  TransportError timeout (re-run GREEN).

## Not done here (by the brief)

No path2 (`run_verify_hook.py`), no tracker edits, no re-classify. path2 for 0x0046cbe0 should
confirm the installed copy is now VehicleCarStateSet (the Physics.cpp duplicate is gone).
