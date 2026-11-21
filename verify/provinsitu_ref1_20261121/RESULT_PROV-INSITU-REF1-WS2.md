# RESULT PROV-INSITU-REF1-WS2 (run 54, 2026-11-21)

Worker cmwqebtl072f9nz1cfoy3lc9k (claude2, claude-opus-5-5 medium), lead REPLAY-AUDIO (cmwqea6x671wxnz1cgusoou57).
Branch `orch/PROV-INSITU-REF1-WS2` from `orch/integration` `77dedfece`. Checkpoint `a1085d935` was taken before the
12:17 reboot and the work resumed on L0's GO.

Row kind: replay, match-only. Nothing is bound into the exe. Not run: build, ddkobj check, G-CTRL, B and the 12x4
captures. No ddkobj unit changed, so no DIVERGENT-unit classification applies.

## Verdict: GREEN

- All four legs of the brief ran:
  - **Class table:** why the 203 non-promoted IDENTICAL bodies fail the write-set leg, with counts.
  - **Group splits:** all 3 diverging groups (ws_g01, ws_g03, ws_g05) were split to their members.
  - **Singles:** the 3 covered+image bodies and the 28 not-run bodies were re-stepped one per run.
  - **Wider sample:** the 31 frame-only bodies with race calls were re-stepped over the race window.
- **2 bodies are promoted** under L-128 through `re/promotions/PROV-INSITU-REF1-WS2.tsv`: `0x0046ddb0`
  VehicleWheelForceIntegrator and `0x00459000`. `c4_reclass.py batch --tsv` dry run: 2 pass, 0 refused.
- **3 more bodies pass the write-set leg but fail another leg.** Two have an open STUB row. One has a standing
  long-race DIVERGED.
- C4 counts, reported apart: 2 replay-verified (`c4=frida-replay`), 0 byte-matched (`c4=match` / `c4=libid`).

## Legs for the 2 promoted bodies

```
install-check: ok (prov_install.json jmp_ok true at the RVA in each cited run: s_0046ddb0, st_g02)
body-executed: yes (0x0046ddb0: 10 calls stepped in s_0046ddb0, 10263 calls in its verdict run; 0x00459000: 1248 race calls stepped in st_g02, 2719 calls in its verdict run)
no STUB: no open STUBS.md row names either RVA in column 2 (0x0046ddb0 appears only in resolved rows S-2625 / S-2631 / S-2632)
write-set: covered (every observed store is frame or compared image, image > 0, in a stepped run that itself replays IDENTICAL-FOR-600 against ref1 / ref2, and no stepped run of the body shows an escape; L-38 / L-40)
IDENTICAL-FOR-600: re/frida/prov_compare.py against ref1 / ref2 (verify/ro_20261116/REFERENCE_RO-REF-NAME.json), body live, no stepping, PROV-INSITU-REF1 verdict runs (b1_023950_003_6 for 0x0046ddb0, b1_023950_005_6 for 0x00459000), VERDICTS.tsv of RESULT_PROV-INSITU-REF1.md
```

| rva | name | evidence run | replay of that run | calls stepped | stores | frame | image | long race (11-11) |
|---|---|---|---|---|---|---|---|---|
| 0x0046ddb0 | VehicleWheelForceIntegrator | s_0046ddb0 (single, first 10 calls) | IDENTICAL-FOR-600 | 10 | 660 | 200 | 460 | not run |
| 0x00459000 | FUN_00459000 | st_g02 (7 bodies, every call from the race anchor) | IDENTICAL-FOR-600 | 1248 | 60864 | 60858 | 6 | IDENTICAL-FOR-2724 |

- `0x0046ddb0` is a thin row. Its stepped calls are the first 10, all from one single run. Its other ref1 stepped
  run, ws_g01, diverged because of the member `0x00420050` (see "Splits").
- Its earlier hooks.csv row is C3 (`impl`, ForceIntegrator.cpp). That row's hand-path port status is not touched
  (L-93: C4 certifies the RE).

## Why the 203 bodies fail the write-set leg (`WS2_CLASSES.tsv`, one row per body)

257 IDENTICAL-FOR-600 bodies, minus the 54 promoted by PROV-INSITU-REF1, leaves 203. Class and subclass come from
PROV-INSITU-REF1's first-10-call sample, re-decoded here (`ws2_classes.py`). The last column is this row's result.

| class | n | subclass (n) | after WS2 |
|---|---|---|---|
| A long-race hold | 3 | standing long-race DIVERGED on the current source: `0x00469aa0`, `0x0046e9e0`, `0x004709a0` (3) | not re-stepped, stay |
| B escapes | 131 | heap/other only (47) | stay: L-38 needs a heap compare |
| | | caller stack through a pointer, e.g. `mov [ecx], eax` (26) | stay: L-40 |
| | | own argument slot + heap/other (24) | stay |
| | | own argument slot only, esp/ebp-relative at base_esp+4..+0x1c (15) | stay (DECISION 1) |
| | | through a pointer + heap/other (6) | stay |
| | | heap/other + image-masked (5) | stay |
| | | image-masked only (3) | stay: masked blocks |
| | | own slot + heap/other + masked (2) | stay |
| | | both caller-stack forms + heap/other (2) | stay |
| | | both caller-stack forms (1) | stay |
| C frame only (L-40) | 38 | race calls: stride re-step (31) | 3 pass, 1 covered+image in a diverged run, 6 now escape, 21 still frame only |
| | | all calls before the race anchor, or every call already stepped (7) | not re-stepped, stay |
| D covered+image, stepped run diverged | 3 | `0x00420050`, `0x0046ddb0`, `0x00448220` (3) | 1 pass (`0x0046ddb0`), 2 stepping-sensitive |
| E not-run | 28 | ws_g05 write log full (16) | 1 pass, 13 escape, 2 frame only |
| | | no main-thread call stepped (11) | 8 run on a non-main thread (0 main-thread frames), 2 escape, 1 frame only |
| | | not in a ws group (`0x005185a0`) (1) | escapes |

- **Heap/other** (86 bodies in B) means a store outside the image, the 4 MB stack window and the blob. Every such
  target in the re-decode is below 0x70000000. None lands in the exe's code or .rdata or in the prov stub area.
  Under L-38 a heap writer needs a heap-tracked compare with more than 0 pages compared. The replay compare hashes
  the image only, so **no wider sample of this harness covers them**. Covering them needs a heap-tracked replay
  compare, which would be a tool row.
- **Image-masked** (10 bodies) means a store into a 1 KiB block that ref1 and ref2 already disagree on. No compare
  of these two references covers it.
- **Caller stack.** Two forms were found:
  - **through a pointer (35 bodies):** a store through a pointer argument into the caller's frame. Under L-40 these
    stay until a scenario makes them write compared memory.
  - **own argument slot (41 bodies):** `[esp+k]` / `[ebp+k]` landing at base_esp+4..+0x1c, the body's own incoming
    parameter slots. Example: `0x00463640` +0x101 `mov dword ptr [esp + 0x34], ecx` stores into base_esp+0x4.
  - Of the 15 own-slot-only bodies, 11 also store into the compared image. [UNCERTAIN] Whether a caller reads such a
    slot after the call, and whether the original stores the same slot. This rests on reading the decode only. Test:
    a register line.
- **Non-main thread** (8 bodies): `0x005b6b00`, `0x005b2820`, `0x005b2b00`, `0x005ab620`, `0x005b1180`, `0x005b29e0`,
  `0x005b3300`, `0x005c8910`. Each single stepped 0 main-thread calls and logged 1 to 10 other-thread steps. The
  write log records main-thread steps only (detorig.js wsInstall). These are the 8 not-run rows of `WS2_CUT.tsv`.
- **ws_g05 overflow.** In PROV-INSITU-REF1 the write log of ws_g05 filled at 2,097,152 records and dropped 826,682.
  `0x004df120` alone took 2,083,048 steps in 6 calls, so 16 later members got no stepped call. All 16 were
  re-stepped as singles here: 1 pass (`0x0046ef70`, held, below), 13 escape, 2 frame only.

## Passes that are not promoted

| rva | write-set run | why not promoted |
|---|---|---|
| 0x0047def0 | st_g03, 1069 race calls, 336 image stores | open STUB S-3728 names it in column 2 (no-STUB leg). Same finding as PROV-WRITESET-SCENARIO |
| 0x00469df0 | st_g04, 1487 race calls, 2273 image stores | open STUB S-2622 names it in column 2 (no-STUB leg) |
| 0x0046ef70 | s_0046ef70, 10 calls, 10 image stores | standing long-race DIVERGED@408 on the unchanged source (`re/match/PROVISIONAL.tsv`, refused). Held by the same rule as class A |

## Splits of the 3 diverging --prov-wlog groups (`ws2_state.jsonl`, all stepped `--prov-wlog 10`)

| group | repeat | path to the member | member | first tick, block | member single | member unstepped |
|---|---|---|---|---|---|---|
| ws_g01 (first) | DIVERGED@300 | a -> aa -> aab -> aabb -> aabbb | `0x00420050` PerPlayerViewportRender | 300, `0x692400` | s_00420050 DIVERGED@300 `0x692400` | IDENTICAL-FOR-600 in its ref1 verdict run b1_023950_002_6 |
| ws_g01 (second) | | b -> ba -> baa -> baab | `0x00407e20` | 302, `0x896400` | (escapes, not a single) | IDENTICAL-FOR-600 in its ref1 verdict run |
| ws_g03 | DIVERGED@302 | a -> aa -> aab -> aabb -> aabba | `0x00448220` Frontend::PostRaceResultCamera | 302, `0x896400` | s_00448220 DIVERGED@302 `0x896400` | nw_s_00448220 (`--prov-wlog 0`) IDENTICAL-FOR-600 |
| ws_g05 | DIVERGED@302 | a -> aa -> aaa -> aaaa | `0x00445aa0` CamEntryType0Step445aa0 | 302, `0x896400` | s_00445aa0 DIVERGED@302 `0x896400` | IDENTICAL-FOR-600 in its ref1 verdict run |

- Every sibling on each path is IDENTICAL-FOR-600, down to the single member. The member labels are in
  `ws2_state.jsonl`.
- **One residue.** ws_g01bab (`0x004b42c0`, `0x005b7980`, `0x00468980`) DIVERGED@535 in `0x7d5000`, `0x7d6000`. Both
  of its halves are IDENTICAL-FOR-600 (baba: `0x004b42c0`; babb: the other two). So that divergence has no single
  member.
- **Conclusion.** The three group divergences come from 4 bodies, and each diverges only when single-stepped. The
  cut rule needs an IDENTICAL stepped run, so none of the 4 passes. The rest of each group replays IDENTICAL.
  [UNCERTAIN] What the single-step changes. It rests on runs, not on a mechanism. Test: a register line.
- st_g00, a stride group (`0x004c4a50`, `0x004c52f0`, `0x004c4dc0`, `0x0045ac40`, `0x00454350`, `0x00468d80`,
  `0x004694e0`), also diverged at 302 in `0x896400`, and its write log overflowed (16,274,798 records dropped). It
  was not split. Its covered+image body `0x00468d80` is left as "covered+image, run diverged". 5 of its 7 bodies
  show an escape or frame-only stores in the logged prefix.

## Wider sample: stride re-step of the frame-only bodies (`ws2_STRIDE_PLAN.tsv`, `ws2_wstride.json`)

- **Method.** This is the PROV-WRITESET-SCENARIO method on the ref1 recipe: `--prov-wstride` with `--prov-warm anchor`,
  up to 100000 stepped calls per body, every K-th race call, in groups of 7.
- **K was underestimated.** It was set from the census and the 190/2724 race window. 4 of 5 groups filled the write
  log (2,097,152 records), so only a prefix is decoded. The calls stepped per body are in `WS2_WRITESET.tsv`.
- st_g01..st_g04 replay IDENTICAL-FOR-600. st_g00 DIVERGED@302 (above).
- Result over the 31 bodies:

  | result | n | bodies |
  |---|---|---|
  | pass | 3 | `0x00459000` promoted, `0x0047def0` and `0x00469df0` STUB-held |
  | covered+image in the diverged run | 1 | `0x00468d80` |
  | now escape | 6 | `0x004c52f0`, `0x004c4dc0`, `0x00564040`, `0x00414570`, `0x0047d640`, `0x005b3a00` |
  | still frame only (L-40 stays) | 21 | |

- **Cut fix.** An escape seen in any stepped run now makes the body escape (`ws2_decode.py`). Before this fix the
  first-10-call "frame only" verdict hid 4 of these 6 escapes.

## No-step controls (`--prov-wlog 0`)

- `nw_ws_g01` / `nw_ws_g03` / `nw_ws_g05` (20 bodies) and `nw_s_00420050` (1 body) report DIVERGED@0 in block
  `0x00912800`. At tick 1 (checkpoint k=201) the dwords 0x00912b34 / b44 / b54 / bb4 / bc4 / bd4 read 0 against
  ref1's 4 / 0x1b / 0x3e (twice).
- `nw_s_00448220` with the same option is IDENTICAL-FOR-600.
- [UNCERTAIN] Why those six dwords stay 0 in 4 of these 5 runs. Test: a register line.
- So the four failed runs are not controls. The only clean control is `nw_s_00448220`, and it confirms that
  `0x00448220` diverges because of the stepping.

## DECISION text for the lead (not decided here)

1. **Own argument slots.** Should a store into the body's own incoming parameter slots (esp/ebp-relative, base_esp+4
   .. +4*nargs) count as frame in the write-set leg? Options:
   - (a) keep it as caller stack, under L-40. Today's rule, 0 bodies move.
   - (b) count it as frame when a static scan shows no caller reads the slot after the call. That makes the 11
     own-slot-only bodies with image stores into cut candidates, if their stepped runs are IDENTICAL.

   Evidence: `WS2_CLASSES.tsv`, `D:/MashedWT/PROV-INSITU-REF1-WS2_data/esc_detail_ref1groups.json`.
   Recommendation: (b), behind its own tool row for the caller scan.
2. **Stepping-sensitive bodies.** `0x00420050`, `0x00407e20`, `0x00448220` and `0x00445aa0` diverge only when
   stepped. Options:
   - (a) keep the rule that the stepped run must replay IDENTICAL. The 4 stay.
   - (b) accept stores from calls that end before the first diverging tick.

   Recommendation: (a), plus a harness row that finds what the trap changes.

## Not covered

- Heap and masked escapes (89 of the 131 B bodies have at least one). There is no heap-tracked replay compare.
- The 8 non-main-thread bodies. The write log is main-thread only.
- The 21 + 7 frame-only bodies. They stay under L-40. Their race sample is the logged prefix only, because the log
  overflowed in 4 groups.
- st_g00 was not split.
- The extra-batch / NOSRC bodies belong to PROV-INSITU-REF1-NOSRC, not to this row.

## Runs, locks, roots

- Every arm runs through `ws2_arm.py`: the ref1 argv unchanged, test-slot outside and `mashed_lock.py run` inside.
- Before every launch it loops on `mashed_lock.py status` and sleeps 60 s while any line shows PRIORITY or SOLO.
  `lockstatus.log` records the waits: 11:03 to 11:09, and the merge-train hold 13:00 to 13:12.
- The PC rebooted at 12:17. All drivers had been stopped first, and only this row's PIDs were killed. The 3 runs in
  flight re-ran.
- Root `D:\MashedWT\PROV-INSITU-REF1_root` was reused read-only. Its MASHED.exe `b9977dab...fe87`, unpatched
  `bdcae093...3c0e`, gamesave.bin `bd187881...2a70`, d3d9.dll `47901951...c7be` and mashed_re_dev.asi
  `906f8601...a8ad` are equal to the ref1 root.
- 80 runs with a state line: 31 singles, 39 split runs (repeats included), 5 stride groups and 5 no-step controls. The 3 runs killed for the reboot re-ran and are counted once.
  The notebook hub was not used. Game runs stay local by profile.

## Evidence

- In this folder:
  - Tables: `WS2_CLASSES.tsv`, `WS2_CUT.tsv` (per body, all stepped runs listed), `WS2_WRITESET.tsv` (per body and
    run), `ws2_STRIDE_PLAN.tsv`, `ws2_wstride.json`.
  - Tools: `ws2_arm.py`, `ws2_driver.py`, `ws2_classes.py`, `ws2_decode.py`, `ws2_table.py`.
- Promotions: `re/promotions/PROV-INSITU-REF1-WS2.tsv`.
- Raw data: `D:/MashedWT/PROV-INSITU-REF1-WS2_data`. It holds the run folders, `ws2_state.jsonl`,
  `esc_detail_ref1groups.json`, the `wsdec_*.tsv` decodes, `lockstatus.log` and the `driver_*.log` files.
  `WS2_RAW_SHA256.txt` lists the SHA-256 of each of those files and of every run's `ticks.bin`, `wlog.bin`,
  `meta.json` and `prov_install.json`.
