# RESULT D4-WSB-BVH-FU: BVH residue (0x00564c80, 0x00561390, 0x00563e70, 0x00565200)

Verdict: **GREEN.** Default gates unchanged on this branch's exe (built with the D4-WSB-NARROW-FLIP default). **1 row goes
C3 -> C4** (`0x00565200`). 3 rows stay C3 under the PREREG rules. The three asks of the ledger row are answered:
`0x007dc8c8` values logged, `0x00561390` STW re-run done under a PREREG, census racediff fixed and run.
One finding for the lead: in the write-set legs so far, heap memory was never tracked. That includes the legs behind 5
promotions of the prior step (section 4).

| row | write-set leg on this exe | racediff on this exe | level |
|---|---|---|---|
| `0x00565200` | W: RunRegion CLEAN 48/48 | C: census **C4-CLEAN** 6/6 calls | **C3 -> C4** |
| `0x00563e70` | W: CLEAN 24/24, but `o:0,n:0` in every sample. P: hung 2/2 | C: census **C4-CLEAN** 6/6 calls | stays C3 (addendum 1: needs CLEAN-P) |
| `0x00561390` | S (STW): CLEAN 24/24. S2 (no STW): CLEAN 24/24. P: hung 2/2 | R: Quick Battle **C4-CLEAN** 6/6 calls | stays C3 (addendum 1: needs CLEAN-P) |
| `0x00564c80` | V: DIVERGENT 24/24, values below. P: hung 2/2. PN: no read-out | R2: Quick Battle **C4-CLEAN** 6/6 calls, 4 sigs | stays C3 |

Branch `orch/D4-WSB-BVH-FU`, off `orch/integration` @ `cb1ba12de` (contains `orch/D4-WSB-BVH-WRITESET` @ `130929d9d`).
`orch/integration` was merged in again after the NARROW-FLIP merge landed: `git merge-base --is-ancestor fc09a9e04 HEAD`
true at `e00b64c5b`, before the build and before every arm. PREREG `PREREG_D4-WSB-BVH-FU.md` (`9cf1abb5e`, committed
alone), addendum 1 (`223b0d3c5`), addendum 2 (`56aac8cea`), each committed before the arms it names.

## What changed

1. `Core/ShadowTrack.h`: a divergent `RunTracked` sample also logs `ndw=` (differing dwords on the first differing page)
   and `v=off:pre/orig/port` for the first 4 of them. `Core/ShadowAB.h`: log line buffer 256 -> 512. Armed path only.
2. `c4_racediff_v7.py` (hand copy of v6): the census OFF boot passes `--hooks __none__`. Root cause of the v6 failure:
   with no `--hooks`, `scenario_launch.py:5036` sets `MASHED_RE_NO_AUTO_HOOK=1`, and `dinput8_shim.cpp` `LoadDevAsi`
   then returns before loading `mashed_re_dev.asi`. `asiloaded()` stayed false through the whole race, and the rpc
   died with the process ("script has been destroyed"). The launcher log was in fact kept by v6
   (`D:/MashedWT/D4-WSB-BVH-WRITESET_data/00563e70_census/launcher_off.txt`: a full 58 s race). The prior RESULT's
   "not kept" was wrong. With v7 both OFF boots log `armed: asi=True` with the unhooked byte at the RVA.
3. Drivers `run_bvh3.py`, `run_shadow2.py` (copies, this step's paths and label). `run_shadow2.py` gained guards: it
   kills its own game PID on samples-done, on 300 s without log growth, or on 360 s with no log after the PID line
   (`PYTHONUNBUFFERED=1` so that line arrives live). One driver syntax fix before any arm ran (`feecd4dbd`).

## Default gates (`mashed_re.exe` `8832afca...6e3e`)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D4-WSB-BVH-FU_root`,
test slot + `mashed_lock.py run --label D4-WSB-BVH-FU`. All four `exit=0 self_exited=True`.

| arm | sha8 | ref (`merge_train.json`) |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

No source change after this build (later commits touch only `verify/` scripts and docs).

## 1. `0x00564c80` at `0x007dc8c8` (leg V, diagnostic)

24/24 divergent, every sample `pages=o:1,n:1,diff:1`, `ndw=2`: the two differing dwords are `+8c8` and `+8cc` of page
`0x007dc000`. Sample 0: `8c8` pre `0x49e` / orig `0x4c1` / port `0x4c2`, `8cc` pre `0x158` / orig `0x15a` / port `0x15b`.
Sample 1: `8c8` `0x4e7` / `0x508` / `0x509`, `8cc` `0x15c` / `0x15c` / `0x15d`.
Over all 24: **port - orig = 1 on `+8c8` and 1 on `+8cc`, in every sample.** The original's own increment per call
ranges 33..62 (`+8c8`) and 0..14 (`+8cc`). In the port body `+8c8` is the pair-test increment
(`RwpSolverCore4.cpp:136`, `:205`) and `+8cc` the pair-accept increment (`:148`, `:219`), followed by `FUN_00563e70`.
So the port's run of `0x005646c0` tests one more pair and accepts one more pair than the original's run.

The sample also shows the original's run wrote one tracked page only, while `0x00564c80` writes the box at
`param_1 + (param_2 & 0xffff) * 0x20` (`RwpSolverCore4.cpp:267-272`) and links the primitive into the octree.
`ShadowTrack.h:382`, `:409-417`: heap pages are tracked only with `MASHED_SHADOW_PRIVATE=1`, which no write-set driver
has set. So those heap writes are not restored before the port runs.
[UNCERTAIN] that the +1/+1 is this restore gap (the primitive linked by the original's run meets itself once in the
port's query). For: exact +1 on both counters in 24/24, `o:1` while the body writes heap. Missing: the same compare
with heap pages tracked. Arm P (heap + STW) hung 2/2 before any sample. Arm PN (heap, no STW) got no read-out: the
launcher logged `error: could not attach`. Not a port defect claim, not cleared either.

Racediff R2 on this exe: C4-CLEAN (6/6 calls, 4 sigs each side, PIDs 15776/4032). The row stays C3 because it has no
CLEAN write-set leg.

## 2. `0x00561390` (legs S, S2, R)

S (`MASHED_SHADOW_STW=1`, PREREG): **CLEAN 24/24**, `A/B-IS-REAL installed=e9`. S2 (no STW, diagnostic): **CLEAN 24/24**.
The prior step's 2/24 divergences at `0x007dce08` did not recur in either boot, so no value was captured for them. R
(Quick Battle racediff, this exe): C4-CLEAN, 6/6 calls. The PREREG rule (S + R) is met. Addendum 1, committed after V
and before S ran, made CLEAN-P the verdict leg instead, because S tracks the image pages only: `o:1,n:1` in every
sample. P hung 2/2 (game PIDs 18688, 17824: race reached, then the launcher's `carinfo()` rpc failed, no shadow log,
killed by the no-log guard). **Stays C3.**
The S boot ran its 24 samples, then froze with 0 CPU and held the machine lock for 28 min. I killed my game PID 17412
by hand. That hang is why the guards were added.

## 3. `0x00563e70` / `0x00565200` (legs W, C)

C (census, tool v7, A4 calls 1-3000): `0x00563e70` **C4-CLEAN** 6/6 calls (PIDs 3984/11064), `0x00565200` **C4-CLEAN**
6/6 calls (PIDs 8584/12000). Both OFF boots `armed: asi=True`.
W: `0x00565200` CLEAN 48/48 (RunRegion 0x1c at `param_3`, all 6 stores of the body inside the region, prior RESULT
static item 4) -> **promoted**. The log is byte-identical to the prior step's (`cb14e1e1...`): it is a fresh file
(12:29 today, this step's folder), and RunRegion OK lines carry no values.
`0x00563e70` W: CLEAN 24/24 but `o:0,n:0` in every sample, so the compare covered no memory. P hung 2/2 (game PIDs 19624,
10192). **Stays C3.**

## 4. Finding for the lead: the write-set legs have never tracked heap memory

Tracked pages per sample in the prior step's logs (`D:/MashedWT/D4-WSB-BVH-WRITESET_data/<rva>/shadow_ab.log`):

| row (C4 since D4-WSB-BVH-WRITESET) | samples | tracked pages |
|---|---|---|
| `0x0055fea0` | 24 | `o:0,n:0` in all |
| `0x0055ff90` | 24 | `o:0,n:0` in all |
| `0x00564040` | 24 | `o:0,n:0` in all |
| `0x00565260` | 20 | `o:0,n:0` in all |
| `0x00561040` | 24 | `o:1,n:1` in all |

Their `hooks.csv` notes say "page-level writes + caller 4 KB stack window compared". For the first four, no page was
compared. The stack window was compared, and the return was compared. Their racediff legs (C4 rubric: canonical scenario
with the hook installed) are unaffected. Arm P could not produce the heap-tracked diagnostic the addendum planned for
these 5 rows: heap tracking hung in 6 of 6 boots, so the 5 diagnostic boots were not run.
Recommendation: correct those 5 notes through re-classify. Whether they keep C4 on racediff alone is the lead's call.
The BROAD / SOLV re-scope that the prior RESULT suggested ("swap to RunTracked, run the census write-set") inherits the
same gap until heap tracking works.

## 5. Harness observations (for `re/HARNESS_BACKLOG.md`, not acted on here)

- **Heap tracking (`MASHED_SHADOW_PRIVATE=1`) freezes the game at the first armed sample**: 6/6 with STW, plus 1 PN boot
  with no read-out. Without it, `RunTracked` only covers `MASHED.exe` image pages and the 4 KB caller stack window.
- **`RunTracked` installs hooks that were not installed.** When a wrapper is armed but its hook is absent, it runs the
  A/B and then calls `HookSystem::Install(idx)` (`ShadowTrack.h`, after `Compare`), so the hook goes live for the rest of
  the boot (S boot: `[--] fn=FUN_00564c80 PATCHBYTE installed=83 ... SUSPECT-no-restore`, then samples). Checked in all
  10 logs of this step and the prior one: every sample of the target row comes before the first sample of any other
  row. So no verdict above or in the prior step is affected. It changes the "hook installed alone" state after the
  target's samples, and it is a candidate cause for the post-sample freezes and early exits. Suggested fix: re-install
  only if the byte was `0xE9` before the window.

## Next

- Heap tracking in `ShadowTrack.h` is what blocks `0x00564c80`, `0x00561390` and `0x00563e70` (racediffs already
  C4-CLEAN on this exe). Fix the private-mode freeze, then re-run arm P for those 3 plus the 5 prior rows. One more
  available read-out for `0x00564c80`: `--watch-write` on `0x007dc8c8` inside one sample window.
- Lead: section 4 (5 prior C4 notes).

## Files

Committed (`verify/wsbbvhfu_20261027/`): `PREREG_D4-WSB-BVH-FU.md`, `PREREG_ADDENDUM_1.md`, `PREREG_ADDENDUM_2.md`,
`c4_racediff_v7.py`, `run_bvh3.py`, `run_shadow2.py`, `apply_reclass.py` (the re-classify transaction),
`racediff_summary.tsv`, `shadow_summary.tsv`, this file. Trackers: `hooks.csv` (1 row), `re/analysis/CHANGELOG.md`
(1 line). Source: `Core/ShadowTrack.h`, `Core/ShadowAB.h`.

Raw (not committed), `D:/MashedWT/D4-WSB-BVH-FU_data/`. Every PID killed by this step was spawned by this step
(6204/2088/664/3316, 6400/18228/1564/9608, 17412 by hand, the rest by the guard). No MASHED from this step is running.

| file | SHA-256 |
|---|---|
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates/gates_run.log` | `444fe690b5a08d5c5c408bc2ecdb53bffbdb783168a9c9a1b1b97db57cb01cb3` |
| `00564c80/shadow_ab.log` (V) | `f74fd59bef393ffd54b5341a1a16bc1c342bdd45c164faa71af831795fdcc501` |
| `00564c80/c4_racediff_samples.json` (R2) | `5174ddd9634aa7bbf6642c203ee7a99f100312bdaa4cc2debaddf8ff8ccfedb7` |
| `00563e70/shadow_ab.log` (W) | `1604716928a08e89e6462152f34729606793f718339b7820ca9f288bd854cd59` |
| `00565200/shadow_ab.log` (W) | `cb14e1e14eed81696e3032b56dfa6a7ed70188c57a985d44ac4928dd5393cbfa` |
| `00561390_stw/shadow_ab.log` (S) | `dba8463bd37540417810b8377c4b6f5d6e72eca699d05ef91c3272f76d1ee0fc` |
| `00561390_nostw/shadow_ab.log` (S2) | `bf56b6aaca77c4d8a7e49038f6feed88938f16afac3b9b63b6e7854a0d56be2c` |
| `00561390/c4_racediff_samples.json` (R) | `4d461d2e6a3dc6336dbfbded8091299f216a17276355645c4803ab95df9d3086` |
| `00563e70_census/c4_racediff_samples.json` (C) | `1c9dc5f0dda05218acda98998ae13973fe35bf1321a0760aac6b6e4f3cf3cd94` |
| `00565200_census/c4_racediff_samples.json` (C) | `43e3e4eb247fa2b0ed643bb3df3a97f39a090df37fa92132676aaa277a7ddf28` |
| `00563e70_census/launcher_off.txt` | `70dd369be0ba3cc830c76ce2f9b9233513f2b801be843fc9b0d5d9b2ff43bb31` |
| `00565200_census/launcher_off.txt` | `2dcc08170ce223b5f3a3deb6f90376915aff09480767e5bacf0f58b1e2dc4e11` |
| `00561390_priv/shadow_transcript.txt` / `_priv_r2/` | `ccb30da2...03ed4` / `5b229b18...e82f5` |
| `00563e70_priv/shadow_transcript.txt` / `_priv_r2/` | `c1e8d587...f40bd` / `8783afeb...52c50` |
| `00564c80_privnostw/shadow_transcript.txt` | `b0d3dd9ef37e0546bd12a82fd7d98392a0c9fcc74f20f67254d782d46f3266f4` |
| `shadow_batch2.log` / `shadow_batchP.log` / `batch3.log` / `batch4.log` / `batch5.log` | `487fb85e...ac34` / `fccc4b03...ac34` / `a17e9965...df9a` / `64cb27ee...1dcf` / `859ffced...e056` |

The two P attempts of `0x00564c80` left no transcript (the driver was killed by hand with the game).
Build: `mashed_re.exe` `8832afca185fa7ff37a619dc2ef080eac95da186f6b2f4059fa6189959a96e3e`, `mashed_re_dev.asi`
`aef1ed979adf826bac54acd0641b626fe01e04eb8ccf2f7b752f8da87e7f659c` (private root only, hash matched), `mashed_re_dev.map`
`e1558478853fa7a554f6d211bab83dae32ef0c79ca298866315a32e92726aa18`. `original/mashed_re_dev.asi` not touched.
