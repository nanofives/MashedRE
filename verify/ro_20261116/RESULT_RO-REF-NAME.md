# RESULT RO-REF-NAME (run 51, 2026-11-16)

Branch `orch/RO-REF-NAME` from `orch/RO-REF-FIXED-LOAD` `bf833a562`. Row kind: original-side runs plus text. No build, no
G-CTRL, no B, no tracker, mask, hooks.csv or ROADMAP change. No harness or gate code change. Nothing pushed or enqueued.

## 0. Verdict: GREEN

Two fresh plain runs on the fixed-load recipe (`--read-wait --read-wait-rtfs`), ref1 and ref2, new launches in this row:

| run | race.dn_warp | bootlen.qpc | load calls | N (boot) |
|---|---|---|---|---|
| ref1 | 108 | 222 | 108 | 234 |
| ref2 | 108 | 222 | 108 | 235 |

**ref1:ref2 is IDENTICAL-FOR-N 600 of 600, N aligned** (no `--n-ok`), control ignore set
(`--masks re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv --ignore 0x00614400-0x00614800,0x00771800-0x00771c00`).
The one block listed is the ignored `0x00771800` (from tick 9); the raw word `0x006147d8` (MASK) differs from tick 9 and
is dropped by the mask route (`refname_pair_ref1_ref2.txt`). Supporting only: ref1:s1 (RO-REF-FIXED-LOAD) is also 600 of 600
(`refname_pair_ref1_s1.txt`). Masks-only `0x00771800` residue stays under the control ignore set. RO-REF-TICKRAW-771800 not claimed.

## 1. Reference recorded

ref1 is the replay oracle reference. `detorig_firstdiv.py` takes the reference as its `DIR_REF` argument and has no fixed
reference path, so nothing in the gate was changed: the reference is named in
`verify/ro_20261116/REFERENCE_RO-REF-NAME.json` (dir, argv, root, exe SHA-256s, harness commit `bf833a562`, usage line).
Checkpoints 0,1,17,18,299,301,600 and the call logs are in the run dir. Raw stays in `D:/MashedWT/RO-REF-NAME_data`
(166 MB); SHA-256 of all 54 files: `raw_sha256_RO-REF-NAME.txt`.

Root: `D:\MashedWT\RO-REF-FIXED-LOAD_root` reused (MASHED.exe `b9977dab...fe87`, unpatched `BDCAE093...3C0E`). Driver
`refname_arms.py` (a copy of `fixload_arms.py` with this row's data folder and lock label). Under test-slot and mashed_lock;
waited for memory and the merge-train solo window, no crash.

## 2. How an exe replay arm gets the same load length and k (item 4)

Not measured on an exe arm. Read from code, so [UNCERTAIN] (register line added):
- The original's load length (108) and k (QPC 222) come from the piz poll loop: callback `FUN_00429290` once per
  GetOverlappedResult poll at `0x004b688f`, polls forced to one per read by `--read-wait` (`0x004b68a4`) and
  `--read-wait-rtfs` (`0x005510f5`). ref1/ref2 `meta.json` `bootlen.rw_polls` = 93 in both.
- On the ORIG_TOP exe the piz layer is PIZ_FS (`mashedmod/src/mashed_re/Match/PortTop_00402750_Pieces.cpp`).
  `PortTop_PizFs_SetIdle_004b6560` records the `0x00429290` callback and its comment and log say the exe reader does not call
  it ("loads synchronously", "not called by the exe reader"). So the exe makes no per-read callback, and its synchronous reads
  already need no wait, but they also do not produce the original's 93 callback chains. Whether the exe's clock count at tick 1
  and race anchor then equal 222 / 108 is untested.
- Test: one exe replay arm, compare its clock counts with the reference meta.json. Register line:
  `re/orchestrator/UNCERTAIN_REGISTER.md` "RO-REF-NAME (exe read wait)".

## Evidence (`verify/ro_20261116/`)
This RESULT, `REFERENCE_RO-REF-NAME.json`, `refname_arms.py`, `refname_pair_ref1_ref2.txt`, `refname_pair_ref1_s1.txt`,
`raw_sha256_RO-REF-NAME.txt`.
