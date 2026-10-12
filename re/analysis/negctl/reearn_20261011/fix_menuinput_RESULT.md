# negctl re-earn fix: cluster MENUINPUT (2026-10-12)

Rows: 0x0042aff0, 0x0042b180, 0x0042b310, 0x0042b540, 0x0042b960, plus follow-up 0x0042b770. All six are FIXED.

Branch `reearn/menuinput`, fix commit `249aa785`. Every body was re-ported from a full disassembly of
`original/MASHED.exe.unpatched`, not only from the defect the decision row named. Built with
`MASHED_NO_DEPLOY=1`. path2 (`run_verify_hook.py`) was not run, and no tracker was edited. Both are left for the parent.

Shared evidence:
- Fixture: `re/frida/negctl_fixtures/z_fix_menuinput.py`. It keeps the b03/b08 vectors verbatim and adds
  discriminating vectors for pad 7 held alone, type 0x13 sub 1, and 4-record slot walks. The dwords between
  the ids at `0x007f1a18..0x007f1a50` are zeroed, and `0x005f2770` points at a scratch block whose `+0x34 + i*4`
  is the `FUN_0040e470(i)` result (disasm `0x0040e470`).
- negctl: `evidence/fix_negctl_menuinput.tsv` (run `runs/fix_menuinput/negctl_20261012_043151.tsv`), handler
  `scalars_to_scattered_globals`, slots 1.
- Witness 1 (principal old defect alone, one per function, one build): `evidence/fix_witness_principal_<hook>.csv`.
- Witness 2 (4-byte slot stride alone, A to D, one build): `evidence/fix_witness_stride4_<hook>.csv`.
- After both witnesses the fix was restored, rebuilt, and run_diff + self-baseline were re-run. The
  `fix_diff_*.csv` files come from that post-restore run.
- Callers: one call site each, all inside the region whose nearest preceding hooks.csv start is
  `0x0043dfd0` FUN_0043dfd0 (**C2**). Call sites: `0x0043fca4` (A), `0x0043fedd` (B), `0x0043ff29` (C), `0x00440461` (D),
  `0x0043fc50` (CarSlotInit1P). This was attributed by the nearest hooks.csv start, not by a Ghidra function
  boundary. No absolute (pointer-table) references.
- Callees: `0x0040e470` CarSlotStateGet **C3**, `0x0040e480` CarSlotStateSet **C3**.

## 0x0042aff0 MenuButtonDetectA (`menu_button_detect_a`, arg_type `scalars_to_scattered_globals`)

**Verdict: FIXED**

Defects:
1. Pad 7 held alone was not counted as held. In the original, `0x0042b062` `mov al,[0x7f125a]` / `0x0042b069` `jne 0x0042b0ad`
   goes to the hold check, which skips the timer clear at `0x0042b0f7`. The port ORed only pads 0..6, so it wrote
   0.0 to `_DAT_0067f1b0`. This is the decision-row defect.
2. Slot walk stride. Both walks step `add esi, 0x10` (`0x0042b09c`, `0x0042b163`) over 4 records up to `0x007f1a54`
   (`0x0042b0a0`, `0x0042b167`), with index EBX/EDI 0..3. The port stepped 4 bytes (16 reads, index 0..15).
3. The hold compare is `fld [0x7f1004]` / `fadd [0x67f1b0]` / `fst [0x67f1b0]` / `fcomp [0x5ccac0]` / `test ah,0x41`
   (`0x0042b0c6..0x0042b0e3`). It compares the unrounded ST0 sum. The port compared the float-rounded timer in an
   SSE2 TU. It is now an inline x87 block, which is exact at any live control word.
   This was not exercised by a vector: it only differs when the exact sum is above the threshold but rounds to it.

Checked and matching: the screen gate is signed `[6,7]` (`0x0042b0bc` jl / `0x0042b0c1` jg), the rearm value is
`0x3e4ccccd` (`0x0042b0e5`), and the phase-3 bound is `0x7f1766` = 8 pads (`0x0042b124`).

Evidence:
- run_diff GREEN 13/13 (6 distinct fingerprints): `evidence/fix_diff_menu_button_detect_a.csv`
- self-baseline GREEN 13/13: `evidence/fix_diff_menu_button_detect_a_selfbaseline.csv`
- negctl: real GREEN 0/13, baseline GREEN 0/13, null RED 11/13, ret RED 13/13, field_first RED 9/13,
  field_last RED 9/13, field_none CTRL-GREEN 0/13
- Witness 1 (pad 7 dropped): RED 3/13 (idx 5, 7, 8)
- Witness 2 (stride 4): RED 2/13 (idx 9, 10)

## 0x0042b180 MenuButtonDetectB (`menu_button_detect_b`, arg_type `scalars_to_scattered_globals`)

**Verdict: FIXED**

This is the same instruction stream as 0x0042aff0, with held `0x007f1047` (`0x0042b190`), previous `0x007f1507`
(`0x0042b29d`), timer `0x0067f1b4` (`0x0042b25c`) and phase-3 bound `0x7f1767` (`0x0042b2b4`). It had the same three defects:
- pad 7 `0x0042b1f2` / `0x0042b1f9` jne `0x0042b23d`
- stride `0x0042b22c` / `0x0042b2f3` `add esi, 0x10`
- x87 compare `0x0042b256..0x0042b273`

The body is now shared with A through `MenuButtonDetectHoldRepeat`.

Evidence:
- run_diff GREEN 13/13 (6 distinct): `evidence/fix_diff_menu_button_detect_b.csv`
- self-baseline GREEN 13/13
- negctl: real/baseline GREEN 0/13, null RED 11/13, ret RED 13/13, field_first/last RED 9/13,
  field_none CTRL-GREEN
- Witness 1: RED 3/13 (idx 5, 7, 8)
- Witness 2: RED 2/13 (idx 9, 10)

## 0x0042b310 MenuButtonDetectC (`menu_button_detect_c`, arg_type `scalars_to_scattered_globals`)

**Verdict: FIXED**

Defects:
1. For screen type 0x13 the original returns 1 for sub 0, 1 and 2. Sub 0 and 1 go `0x0042b3f9` je / `0x0042b3fe` je
   `0x0042b3e7`, and sub 2 goes `0x0042b400` cmp 2 / `0x0042b405`. Only other subs reach phase 3. The port returned 1 only
   for sub 2. This is the decision-row defect. The sub is read from `0x0067ed40 + DAT_0067e9f8 << 6` (`0x0042b3f1`).
2. Slot walk stride. The original steps `add esi, 0x10` (`0x0042b3bf`, `0x0042b521`) over 4 records. The port stepped 4 bytes
   over 8 iterations.

Checked and matching:
- type 0x1e returns 1 (`0x0042b3e2`)
- when nothing is held, `_DAT_0067f1b8 = 0` (`0x0042b40f`) and execution goes to phase 3 without the type check
- the phase-3 edge detect has no early exit on the slot path (all 4 `FUN_0040e470` calls run)
- the pad-7 shortcut at `0x0042b4ca` returns the same value as accumulating

The body is now shared with D through `MenuButtonDetectScreenGate`.

Evidence:
- run_diff GREEN 14/14: `evidence/fix_diff_menu_button_detect_c.csv`. There are 3 distinct fingerprints, which is every
  reachable (ret, cleared-dword) combination.
- self-baseline GREEN 14/14
- negctl: real/baseline GREEN 0/14, null RED 11/14, ret RED 14/14, field_first/last RED 2/14,
  field_none CTRL-GREEN
- Witness 1 (sub 2 only): RED 4/14 (idx 1, 8, 9, 13). idx 1 is the decision row's vector.
- Witness 2: RED 2/14 (idx 10, 11)

## 0x0042b540 MenuButtonDetectD (`menu_button_detect_d`, arg_type `scalars_to_scattered_globals`)

**Verdict: FIXED**

This is the same stream as 0x0042b310, with held `0x007f1045` (`0x0042b554`), previous `0x007f1505` (`0x0042b662`) and cleared
dword `0x0067f1bc` (`0x0042b63f`). It had the same two defects:
- sub 0/1 at `0x0042b629` / `0x0042b62e` je `0x0042b617` return 1 (decision-row defect)
- stride `0x0042b5ef` / `0x0042b751` `add esi, 0x10`

Evidence:
- run_diff GREEN 17/17 (3 distinct): `evidence/fix_diff_menu_button_detect_d.csv`
- self-baseline GREEN 17/17
- negctl: real/baseline GREEN 0/17, null RED 13/17, ret RED 17/17, field_first/last RED 3/17,
  field_none CTRL-GREEN
- Witness 1: RED 4/17 (idx 1, 11, 12, 16)
- Witness 2: RED 2/17 (idx 13, 14)

## 0x0042b960 CarSlotInit1P (`car_slot_init_1p`, arg_type `scalars_to_scattered_globals`)

**Verdict: FIXED**

Defect: `xor ecx,ecx` (`0x0042b960`). ECX is incremented on every pass (`0x0042b982`) of the 8-pad scan (bound `0x7f1762`,
`0x0042b983`). It is stored once after the loop: `mov [0x7f1a14], ecx` (`0x0042b991`). A miss therefore leaves 8. The port
stored 0 first and wrote the index only on a hit, so a miss left 0. This is the decision-row defect.

Checked and matching:
- `0x007f1a24/34/44 = -1` (`0x0042b98a` `or eax,-1`, stores `0x0042b997..a1`)
- `0x007f1a0c = 1` (`0x0042b9a6`)
- `FUN_0040e480` called as (0,1), (1,0), (2,0), (3,0) (`0x0042b9b0..cb`, cdecl, one `add esp,0x20`)

No fixture change was needed: the b08 vectors 3 and 5 already discriminate.

Evidence:
- run_diff GREEN 6/6 (4 distinct): `evidence/fix_diff_car_slot_init_1p.csv`
- self-baseline GREEN 6/6
- negctl: real/baseline GREEN 0/6, null RED 6/6, ret SKIP (void), field_first/last RED 6/6,
  field_none CTRL-GREEN
- Witness 1 (miss -> 0): RED 2/6 (idx 3, 5). These are the decision row's two vectors.

## 0x0042b770 MenuButtonDetectE (`menu_button_detect_e`, arg_type `scalars_to_scattered_globals`, old level C4)

**Verdict: FIXED** (follow-up requested by the parent after the first five rows)

Defect: slot walk stride. The original steps `add esi, 0x10` (`0x0042b877`) over 4 records up to `0x007f1a54`
(`0x0042b87b`), with index EDI 0..3 passed to `FUN_0040e470` (`0x0042b84c`). The port stepped `piVar3++` (4 bytes) and capped
at 8 iterations, reading `0x007f1a14..0x007f1a30` as ids with index 0..7. The b03 fixture had one slot vector,
(0,-1,-1,-1), which a 4-byte walk reads correctly. That is why the row stayed GREEN at C4.

Checked against the bytes and matching:
- 0x1000 path: pads 0..6 OR `held && !previous` into EBX (`0x0042b783..0x0042b81f`, held `0x007f1049`,
  previous `0x007f1509`, stride 0x4c)
- pad 7 (`0x007f125d` / `0x007f171d`) returns 1 directly (`0x0042b836`), otherwise `mov eax, ebx` (`0x0042b885`)
- slot path: id != -1 (`0x0042b846`), `FUN_0040e470(i) != 2` (`0x0042b854`), held / previous on pad id
  (`0x0042b85e` / `0x0042b868`), no early exit
- no timer, no screen-type check

The slot-walk helpers (`SlotRec` / `PadAt`) now sit above E and are shared with C and D.

Evidence:
- Demotion witness: `evidence/fix_finding_stride4_menu_button_detect_e.csv`, the old port against a 4-record probe, RED 2/4.
  idx 0 is all ids -1 (the original returns 0, the old port 1). idx 2 is id 5 in record 2 (the original returns 1, the old port 0).
- Fixture: `z_fix_menuinput.py` keeps the 7 b03 vectors and adds 6 four-record vectors (state-2 skips in records 2 and 3,
  edges in records 0, 2 and 3, all ids -1).
- run_diff GREEN 13/13 (2 distinct, ret 0/1 is the only output): `evidence/fix_diff_menu_button_detect_e.csv`
- self-baseline GREEN 13/13: `evidence/fix_diff_menu_button_detect_e_selfbaseline.csv`
- negctl (`runs/fix_menuinput/negctl_20261012_044251.tsv`, rows appended to `evidence/fix_negctl_menuinput.tsv`):
  - real GREEN 0/13, baseline GREEN 0/13
  - null RED 7/13, ret RED 13/13
  - field_first / field_last NA (E writes no field, observe set empty)
  - field_none CTRL-GREEN
- Witness (4-byte stride restored alone): RED 3/13 (idx 7, 8, 12): `evidence/fix_witness_stride4_menu_button_detect_e.csv`.
  After restore and rebuild, run_diff and self-baseline were re-run, and the `fix_diff_*` files come from that run.
- Plate `frontend_promote_menus_a/0x0042b770.md` corrected: path B is 4 records of 0x10, not `iVar4 = 0..7`.

## Same-file regression check

These are other C3/C4 hooks whose bodies live in `Frontend/MenuNav.cpp` or `Frontend/MenuButtonDetect.cpp`. Each was
re-run after the restore build (`evidence/fix_regress_diff_<hook>.csv`). After the E re-port, screen_width_get, screen_height_get and car_slot_assign were re-run GREEN again, and C, D and CarSlotInit1P were re-run GREEN with self-baseline (their `fix_diff_*` files are from that run). MenuNav.cpp was not touched by the E follow-up:

| hook | RVA | conf | result |
|---|---|---|---|
| menu_button_detect_e | 0x0042b770 | C4 | GREEN 7/7 before its own re-port (b03 vectors only), superseded by the E section |
| screen_width_get | 0x0042b8b0 | C3 | GREEN 10/10 |
| screen_height_get | 0x0042b8c0 | C4 | GREEN 10/10 |
| car_slot_assign | 0x0042b9e0 | C3 | GREEN 10/10 |
| menu_team_balance | 0x0042bb60 | C3 | GREEN 10/10 |
| menu_entry_get | 0x0042ac90 | C3 | VOID 10/10: both arms AV at 0x0 (original included), 2 boots |

menu_entry_get: my diff does not touch its body or the constants it reads (no hunk above line 41 of `MenuNav.cpp`).
Its registry vectors are int args 0..9 against the live `0x0067ed38` table, and in this boot both arms fault at 0x0.
This is a pre-existing fixture void, not a regression. It needs a negctl fixture before its C3 evidence can be re-measured.

## Findings outside the row list

- MenuButtonDetectE 0x0042b770 had the same slot-stride defect. The parent asked for it as a follow-up, and it is now FIXED
  (see its section above).
- menu_entry_get 0x0042ac90: left alone, the parent is filing it.

## Plate corrections

- `frontend_promote_menus_a/0x0042aff0.md`:
  - pad 7 jumps to the hold check and counts as held
  - path B is 4 records of 0x10 bytes (Ghidra `piVar4 += 4` on `int*`)
  - the compare reads the unrounded x87 sum
- `frontend_promote_menus_a/0x0042b310.md`: changed "cursor 0/1 fall through to phase 3" to "return 1". That claim was
  the source of the C/D defect. Path B is now 4 records of 0x10.
- `frontend_promote_menus_a/0x0042b960.md`: a miss leaves index 8, and step 1's 0 is the `xor ecx,ecx`, not a separate store.

## [UNCERTAIN] items

None new. The carried U-3445 (FUN_0040e470 semantics) and U-3556 (screen IDs 0x13 / 0x1e) are naming-only. They
do not affect the mechanical bodies.
