# RESULT IMG-4-GHIDRA-DATAREF: Ghidra data-reference export of the image's code pointers (L-149 a, step 4)

Row `IMG-4-GHIDRA-DATAREF` (D4-IMG, run 44). Worker cmwfgscjb2n4ulh1n83t14lt5 (claude3), lead cmwf9zx6d1ljulh1nd88fk6k9.
Row kind: top-down (tool + knob path). **Branch `orch/IMG-4-GHIDRA-DATAREF` was created from `orch/IMG-3-TEXT-TRAP` @
`2f44d17fe`** (IMG-3 was not yet in `orch/integration`). Merge IMG-1, IMG-2 and IMG-3 first, or merge them with this branch.
Spec: `RESULT_PORTSTATE-IMAGE.md` section 3 ("Sources of the list", item 2) and the IMG-4 row of section 5.

## Verdict: DECISION (the deliverables are done, the goal state is not reachable under this row's constraints)

- **Export: done.** 1,275 Ghidra references from the image ranges into code. All are pointer-typed and all equal the
  file dword. `CODEPTR_IMAGE.tsv` has 1,283 rows and **contains all 188 CODEPTR_SLOTS slots**: 180 come from a Ghidra
  reference and 8 have no Ghidra reference (section 2). The spec's IMG-4 proof holds.
- **The powerup table.** All 9 slots at 0x005f99b0 + 0x40*i are in the export. Slot 0 was already on CODEPTR_SLOTS. The
  other 8 are `pointer` / `undefined *` references. The 7 targets with no function entry decompile with
  `decomp_pc.py --create` (section 3).
- **Binding: done for every target the exe has a body for.** 67 new slots are bound to exe bodies (`CP_IMG_FIX`), with
  0 refused and the self-test PASS. 1,020 slots have no exe body and keep their original value, so the trap reports
  them. 8 slots are excluded because a wrapper rebinds them (section 4).
- **The goal state (3000 frames, then AppDestroy with no trap and no AV) is not reached, and cannot be reached in this
  row.** Teardown 0x0045b930 calls the 9 table pointers. None of the 9 targets has an exe body: `BINDINGS.tsv` has no
  row and `mashed_re.map` has no symbol. This row forbids new matched bodies. A stop thunk exits as surely as the
  trap does (the first knob pass proved it, section 5). So the image + trap boot still ends at
  `execute original 0x004568A0 slot 0x005F99B0`, as in IMG-3. With the binding ON it ends at the same point, as an
  `[OT_CODEPTR] STOP`.
- **ddkobj check: IDENTICAL 1192/1219, DIFFERENT 12, not-in-map 15, units linked 1167/1443, refusals 4
  (wrapper-redefines-globals=4).** The same tally and the same DIFFERENT list as IMG-3.
- **Self-tests: PASS on every arm.** `[OT_CODEPTR]` self-test PASS (0 of 327 checks failed), image slots 67 written and 0
  refused, `[portstate-image]` self-test PASS (0 of 8), `[code-trap]` self-test PASS (0 of 6).

Decision for the lead (options):
- **(a), recommended:** a body row for the 9 teardown targets.
  - 0x004568a0 and 0x004540c0 already have C4 matched sources: `re/match/s09/004568a0.c` and `re/match/s09/004540c0.c`,
    port `none`.
  - 0x004555d0, 0x00457b10, 0x0045a790, 0x0045b2d0, 0x00454af0, 0x00456ca0 and 0x004530b0 first need the function
    created in the master project (ghidra-sweep / `CreateMissedFunctions.java`) and a hooks.csv row (re-classify).
  - They are 39..93-byte RpClumpDestroy loops (section 3).
  - Once BINDINGS.tsv names the bodies, re-running `codeptr_image.py --write` binds them with no other change.
- (b) Logging stand-ins for the 9, like the PWI init stand-in in `PortTop_0045bae0.cpp`. With the image OFF the slots are
  0 and teardown skips them (`test eax,eax; je`). A stand-in that returns therefore matches the image-OFF behaviour,
  but it is a stand-in on the T1 path.

## 1. What changed

| File | Change |
|---|---|
| `re/tools/ghidra_scripts/DataRefExport.java` (new) | Headless Ghidra script. It lists every memory reference whose source is in the given ranges and whose destination is in an executable block. For each one it records the data type at the source, the data start, the component path, the label, the function at or containing the target, the listing kind at the target, and the file dword |
| `re/tools/image/ghidra_dataref_export.py` (new) | Runs DataRefExport on a read-only pool clone (`-readOnly -noanalysis`, nothing persisted) over 0x005cc000..0x00637000 and 0x00915000..0x00915008. Reuses `decomp_pc.py`'s pool acquire/release and resolves the main checkout's `mashed_pool` from a worktree |
| `re/tools/image/dataref_raw.tsv` (new, 1,275 rows, 115 KB) | The raw export |
| `re/tools/image/codeptr_image.py` (new) | Selection (positive evidence only): pointer-typed, 4-aligned, file dword == target. Joins CODEPTR_SLOTS.tsv and BINDINGS.tsv. `--write` writes `CODEPTR_IMAGE.tsv` and `PortTopCodePtrImage.inc` |
| `re/tools/image/CODEPTR_IMAGE.tsv` (new, 1,283 rows) | {slot, target, ghidra_type, src_label, target_fn, in_codeptr_slots, exe_symbol, bind, evidence}. `bind`: cp-slots 188, bound 67, trap 1,020, excluded 8 |
| `mashedmod/src/mashed_re/Compat/PortTopCodePtrImage.inc` (new, generated) | 67 `CP_IMG_FIX(slot, sym, orig)` entries: GetRaceEndTrigger, RwInitNullStub, UtilReturnOne4d8560 |
| `mashedmod/src/mashed_re/Compat/PortTopCodePtr.cpp` | When the image is loaded (and `MASHED_OT_CODEPTR=1`, as for the 188), each `CP_IMG_FIX` slot is rewritten only if its image dword equals the listed original target. A mismatch is refused, counted and logged. The read-back self-test covers these slots too, and one log line gives the image-slot counts. With the image OFF nothing is written |
| `verify/portstate_20261113/img4_create7_decomp.txt` | `decomp_pc.py --create --callees` output for the 7 targets with no function entry |
| `verify/portstate_20261113/img4_codeptr_image.txt` | `codeptr_image.py` summary output |

No new matched bodies, no new librw binding, no tracker edit, no promotion, no decision row filed by this worker. Knobs
are unchanged and default OFF. The new code runs only with `MASHED_ORIG_TOP=1`, `MASHED_PORTSTATE_IMAGE=1` and
`MASHED_OT_CODEPTR=1` all set.

## 2. The export (Ghidra pool slot Mashed_pool0, read-only; slot released)

`py -3.12 re/tools/image/ghidra_dataref_export.py --raw re/tools/image/dataref_raw.tsv` produced:
`DataRefExport: 1275 refs into code, 1002 refs to non-code skipped` (about 23 s).

| quantity | value |
|---|---|
| references from the image ranges into code | 1,275 (all `DATA`; target listing kind `instruction` for all 1,275) |
| Ghidra type at the source | `undefined *` 1,168, `pointer` 63, `action *` 43, `void *` 1 (all pointer types) |
| refused by the selection (non-pointer, unaligned, file dword != target) | 0 |
| targets that are not a Ghidra function entry | 754 rows |
| CODEPTR_SLOTS slots present as a Ghidra reference | 180 of 188, target equal for all 180 |
| CODEPTR_SLOTS slots with no Ghidra reference | 8 (below), kept in CODEPTR_IMAGE.tsv with evidence `codeptr-slots` |
| new slots (not on CODEPTR_SLOTS) | 1,095, 778 distinct targets |

The 8 slots with no Ghidra reference are 0x005f99cc→0x00456740, 0x00632c30→0x005aaef0, 0x00632c34→0x005aae90,
0x00632c44→0x005aaf40, 0x00632c48→0x005aaee0, 0x00634554→0x005b4dd0, 0x006351e0→0x005c9230 and
0x006351e8→0x005bb8f0. At each slot address the listing holds an undefined byte (`listing_code_unit_containing`: kind
DataDB, length 1; examples 0x005f99cc `?? 40h`, 0x00632c30 `?? F0h`, 0x00634554 `?? D0h`, 0x006351e0 `?? 30h`). Their
evidence remains the Ghidra labels of `SEED2_ADDED.tsv` (`codeptr_inventory.py`). For each, the file dword equals the
recorded target.

## 3. The powerup type table (input from IMG-3)

| i | slot | target | Ghidra type at slot | function entry in Ghidra | `--create` decomp (`img4_create7_decomp.txt`) |
|---|---|---|---|---|---|
| 0 | 0x005f99b0 | 0x004568a0 | pointer (PTR_FUN_005f99b0) | FUN_004568a0 | (existing, C4 match) |
| 1 | 0x005f99f0 | 0x004540c0 | pointer (PTR_FUN_005f99f0) | FUN_004540c0 | (existing, C4 match) |
| 2 | 0x005f9a30 | 0x004555d0 | undefined * | none | 61 B: loop RpClumpDestroy(*p) from &DAT_006885e8 step 0xb dwords, RpClumpDestroy(q[-3]), FUN_00489c60(*q) from &DAT_00688418 step 0x1b dwords, while q < 0x688634 |
| 3 | 0x005f9a70 | 0x00457b10 | undefined * | none | 93 B: RpClumpDestroy loops over &DAT_0068b0e4 (step 6, < 0x68b144) and &DAT_0068a368 (step 0x1b, < 0x68b0e8); RpClumpDestroy(DAT_0068b134), RpClumpDestroy(DAT_0068b130), FUN_004768c0(&DAT_0068b090) |
| 4 | 0x005f9ab0 | 0x0045a790 | undefined * | none | 39 B: loop from &DAT_0068bb70 step 0x1a: RpClumpDestroy(p[4]), FUN_004898d0(*p), while p < 0x68bd10 |
| 5 | 0x005f9af0 | 0x0045b2d0 | undefined * | none | 45 B: loop from &DAT_0068d174 step 9: RpClumpDestroy(*p), while p < 0x68d204; RpClumpDestroy(DAT_0068d1e8) |
| 6 | 0x005f9b30 | 0x00454af0 | undefined * | none | 55 B: loop from &DAT_00688368 step 5: RpClumpDestroy(*p), while p < 0x6883b8; RpClumpDestroy(DAT_006883a8); FUN_004768c0(&DAT_0088fec0) |
| 7 | 0x005f9b70 | 0x00456ca0 | undefined * | none | 53 B: loop from &DAT_0068a25c step 4: RpClumpDestroy(*p), while p < 0x68a29c; FUN_004768c0(&DAT_0068a2c0); FUN_004768c0(&DAT_0068a210) |
| 8 | 0x005f9bb0 | 0x004530b0 | undefined * | none | 69 B: loop from &DAT_00684e4c step 7: RpClumpDestroy(*p), while p < 0x684ebc; loop from &DAT_00684f00 step 0x44: RpClumpDestroy(p[-4]), FUN_004898d0(*p), while p < 0x687100 |

"step n" means n dwords (the pointer arithmetic in the decompilation). For each of the 7 rows the decompiler printed
"0x... is not a function entry; showing containing function". With `--create` the function is created transiently on
the read-only clone (not persisted), so the size shown is the created function's body. The 7 rows are `bind=trap` in
CODEPTR_IMAGE.tsv because no exe body exists. Making those functions permanent in the master project is ghidra-sweep
work, not done here (no master Ghidra writes).

## 4. Binding rule, and the wrapper-owned slots

- **Bound (67):** the target has an exe symbol in `BINDINGS.tsv` of kind matched / bound / port / rw / crt / near-match /
  shim. In practice: GetRaceEndTrigger ×26, RwInitNullStub ×30, UtilReturnOne4d8560 ×11.
- **Trap (1,020):** no exe body. The slot is not written: it keeps the original value, so a call through it is reported
  by `MASHED_CODE_TRAP` (spec section 3, choice (b)).
- **Excluded (8):** the slots 0x005f9998 + 0x40*i + 0x14 (i 0..8) that are not on CODEPTR_SLOTS.
  `PortTop_PWI_Call0045bae0` (`Match/PortTop_0045bae0.cpp:183-196`) stores its init forwarders there itself, but only
  while a slot holds 0 or its original target (`s_pwiOrig[i]`). Rewriting them earlier defeats that check. The 9th of
  these slots, 0x005f99ac (i=0), is on CODEPTR_SLOTS and has been bound to `Orig_00456760` by `PortTopCodePtr.inc` since
  PORT-OT-SEED-CODEPTR (section 5, last point).
- **Why the new no-body slots are not bound to stop thunks** (the brief named stops or bodies): the first knob pass did
  bind them to stops (`boots_v1`), with 1,021 stop slots and 706 new thunks. The boot then exited at 1.9 s:
  `[OT_CODEPTR] STOP: ... original 0x00455400 ... exe+0x1FD16B`. The return address falls in `_PortTop_PWI_Call0045bae0`
  (map), and the slot is 0x005f9a2c, entry 2's +0x14 init. That result found the exclusion above. A stop also ends the
  process before trap mode 2 can list later calls, and gives no information the trap does not. So unbound slots follow
  spec choice (b).

## 5. Knob boots

All boots: `MASHED_DET_FRAMES=3000`, private root `D:/MashedWT/IMG-4-GHIDRA-DATAREF_root` (robocopy of
IMG-3-TEXT-TRAP_root, with `original/mashed_image.bin`), test-slot outside (`PROYECTOS_TEST_HUB=off`: the root and the
exes are on this PC's D:, and the first attempt dispatched to the notebook and failed there on the missing paths), and
`mashed_lock.py run --label IMG-4-GHIDRA-DATAREF` inside.

| arm | exe | env | rc | result |
|---|---|---|---|---|
| def_off | default | none | 0 | knobs off |
| def_on_img0 | default | ORIG_TOP | 0 | `WinMain 0x00492370 returned 0 after 3000 frames`, `[OT_CODEPTR] off` |
| def_on_img1_trap1 | default | ORIG_TOP + IMAGE + CODE_TRAP=1 | 0x004568A0 | `execute original 0x004568A0 slot 0x005F99B0`, unchanged from IMG-3 (this row's code is not active without OT_CODEPTR) |
| def_on_img1_cp_trap1 | default | + OT_CODEPTR=1 | 0x004568A0 | `applied: 188 slots ... self-test PASS (0 of 327 checks failed)`, `image slots ...: 67 written (67 bound ...), 0 refused`, trap self-test PASS. Runs to AppDestroy, then `[OT_CODEPTR] STOP ... 0x004568A0 ... exe+0x171DBE` |
| def_on_img1_cp_trap2 | default | + OT_CODEPTR=1, CODE_TRAP=2 | 0x004568A0 | same; **zero `[code-trap]` execute/read/write reports before the stop** |
| ddk_on_img1_cp_trap1 | object | as def_on_img1_cp_trap1 | 0x004568A0 | same, site exe+0x5648 |
| ddk_on_img1_cp_trap2 | object | as def_on_img1_cp_trap2 | 0x004568A0 | same, zero trap reports |

- **Binding ON against OFF** (`def_on_img1_cp_trap1` against `def_on_img1_trap1`, 876 lines each). Masked and sorted,
  with wall-clock, heap-pointer and trap/image lines removed, only the POWERUPINIT-A lines differ. OFF says
  `9 init pointers stored ..., 0 already non-zero` and logs `type table entry 0: ... matched body returned`. ON says
  `8 init pointers stored ..., 1 already non-zero`. Cause: the pre-existing CODEPTR_SLOTS fix
  `CP_FIX(0x005f99ac, Orig_00456760, ...)` in `PortTopCodePtr.inc`, the same wrapper interaction as section 4. So entry
  0's init runs `Orig_00456760` directly, without the logging forwarder. This row did not cause it. A follow-up can
  apply the same EXCLUDE to `codeptr_inventory.py`.
- The sorted diff also shows one RW_BIND line as `+Frame::scale / -RpPTankAtomicCreate`. That is a filter artifact: the
  counts are equal in all three logs (8 `Frame::scale`, 87 `RpPTankAtomicCreate`).
- **Object build against default** (`ddk_on_img1_cp_trap1` against `def_on_img1_cp_trap1`): differences are only in the
  B17 region-map totals (commit 9880 vs 9884 KB). These are exe-size dependent and outside the masked categories.
- **Knobs off unchanged:** `def_off` against IMG-3's `def_off` (masked) differs only in the region-map totals and the
  x87 thread id.
- `original/mashed_image.bin` is absent in the real `original\` (untouched).

## 6. Checks

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, rva-lint OK (237 known, 0 new), asm-lint OK, portstate-seed OK (272, 3231 B, fnv 0x5ecb7d52) |
| object build (`MASHED_DDKOBJ=1`, NO_DEPLOY) | `=== Build OK ===`, `[ddkobj] link rc=0` |
| `py.exe -3.12 re/tools/ddkobj/ddkobj.py check` | IDENTICAL 1192/1219, DIFFERENT 12 `[0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498510 0x00498810 0x004c0c20 0x004c0f00]`, not-in-map 15, units linked 1167/1443, refusals 4 (wrapper-redefines-globals=4). Unchanged against IMG-3 |
| self-tests | OT_CODEPTR PASS (0/327), image slots 0 refused, portstate-image PASS (0/8), code-trap PASS (0/6), on both builds |
| G-CTRL / B / 12x4 captures / menu walk | not run: top-down tool row, no menu code changed. The merge train runs the default gates |

## 7. Raw files (not committed), sha256

```
74384a29b66f4a12766fb10649acad083f00397086442c50a277f8da7d90a31c  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/default/mashed_re.exe
d38bf9240a90a0773eef137014dae9aac8a40eccc12f0c96298804a93d293631  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/default/mashed_re.map
132e7be0398f309d8a769902ce35d67e3209bf194acdb6468ade548f1634be4d  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/ddkobj/mashed_re.exe
81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8  D:/MashedWT/IMG-4-GHIDRA-DATAREF_root/original/mashed_image.bin
88e7ffc5b3ed174928718295a74b12892180d48fbada445830753bdc0710eb0d  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots/def_off.log
6af3deceb50a13f99ec2131c7b58fc207697e25eba91a0e8a7d3e40ff137bc85  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots/def_on_img0.log
562f54f37e0d0ee40c8a3806ee924d9e4e803e4d0e89dcd4b5e39f77e54d38f5  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots/def_on_img1_trap1.log
e419feb779bfa138fd5342abf0b13e0e1fe47c2710c54d49aade40d62d37c6dc  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots/def_on_img1_cp_trap1.log
f10bb7a155b4df0ae6afe1f368d8489917bd5db9e9be8ad61a874d09cf6ae763  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots/def_on_img1_cp_trap2.log
e53dec6ae76243326749759665ebf582581324360c8f56f6ce82a6e970664c2e  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots/ddk_on_img1_cp_trap1.log
81d68a42a069e4c5e44154cb1b950711c3c857722c81e553079906cd0fdeff14  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots/ddk_on_img1_cp_trap2.log
7e7447a2bb6c282b931689d8b32e757ded2cb8c1288a33c6770a01a25372e5fe  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots_v1/def_on_img1_cp_trap1.log   (first pass, stops bound: STOP 0x00455400 at 1.9 s)
25f8ef414ebefde8330f2269becd9ad83948a6d27f34c44fb9b59ceba5ac06fe  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots.sh
74b28db8225a1df339d6322a1b63bde50f895f280fc803cfecf202b95e87cefa  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/boots.out
02b508458c57c6186b29f1281571a23324954baf40b34dbe1db89388cd4d673b  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/build_default.log
3076b8f5af9419d50d13918e0f212806b69ecdeca623c3d8e14db8d6e4e37573  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/build_ddkobj.log
c33cc634cf42c78984e5ab48d60295438a839a99d6baaf7aa69cd278fe33d720  D:/MashedWT/IMG-4-GHIDRA-DATAREF_data/ddkobj_check.txt
```

## Next

- Lead decision (a) or (b) above for the 9 teardown targets. With (a): a ghidra-sweep creates the 7 functions in the
  master project, re-classify adds their rows, a body row ports or matches them (two already have C4 sources), and then
  `py -3.12 re/tools/image/codeptr_image.py --write` binds them.
- Optional: apply the same EXCLUDE to `verify/portorigtop_20261109/codeptr_inventory.py` for slot 0x005f99ac, so that
  entry 0's init goes through the PWI forwarder again under OT_CODEPTR.
- IMG-5: the trap-mode-2 menu walk lists the pointers the menu screens reach. CODEPTR_IMAGE.tsv already names their
  slots.
- Merge order: IMG-1-EXTRACT, IMG-2-LOADER, IMG-3-TEXT-TRAP, then this branch.
