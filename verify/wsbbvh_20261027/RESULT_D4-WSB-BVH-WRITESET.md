# RESULT D4-WSB-BVH-WRITESET: write-set compare for the 9 BVH rows that stayed C3

Verdict: **GREEN** (run 13, 2026-10-27, after the reboot). Default gates are unchanged and the PREREG rules were applied
as written. **5 rows go C3 -> C4.** 2 rows stay C3 on a write-set divergence. 2 rows stay C3 because the census racediff
did not run (the OFF boot fails every time, a harness fault). No source change in this run.

| row | write-set shadow (PREREG 2) | racediff | level |
|---|---|---|---|
| `0x0055fea0` | CLEAN 24/24 | C4-CLEAN (`verify/wsbbvh_20261026`) | **C3 -> C4** |
| `0x0055ff90` | CLEAN 24/24 | C4-CLEAN (`verify/wsbbvh_20261026`) | **C3 -> C4** |
| `0x00561040` | CLEAN 24/24 | C4-CLEAN (`verify/wsbbvh_20261026`) | **C3 -> C4** |
| `0x00564040` | CLEAN 24/24 | C4-CLEAN (`verify/wsbbvh_20261026`) | **C3 -> C4** |
| `0x00565260` | CLEAN 20/20 | C4-CLEAN, v6, PREREG 3 (6/6 calls, 4 sigs) | **C3 -> C4** |
| `0x00561390` | **DIVERGENT 2/24** at `0x007dce08` (STW diagnostic: CLEAN 24/24) | C4-CLEAN (`verify/wsbbvh_20261026`) | stays C3 |
| `0x00564c80` | **DIVERGENT 24/24** at `0x007dc8c8` | C4-CLEAN, v6, PREREG 3 (6/6 calls, 4 sigs) | stays C3 |
| `0x00563e70` | CLEAN 24/24 | census: **NOT RUN** (OFF boot 2/2 failed) | stays C3 |
| `0x00565200` | CLEAN 48/48 (RunRegion 0x1c) | census: **NOT RUN** (OFF boot 2/2 failed) | stays C3 |

Promotions went through re-classify (`apply_reclass.py`, one transaction: 5 `hooks.csv` rows + 5 CHANGELOG lines, no
`[UNCERTAIN]` unfiled in the 5 notes, no stub in the 5 bodies). C4 here = racediff with the hook installed alone AND a
write-tracked shadow with the hook installed alone (`PATCHBYTE installed=e9`, `A/B-IS-REAL`).

Branch `orch/D4-WSB-BVH-WRITESET` off `orch/integration` @ `1158f5c1c`. PREREG `PREREG_D4-WSB-BVH-WRITESET.md`
(`eec2fd746`, committed alone before any arm). Prior evidence `verify/wsbbvh_20261026/RESULT_D4-WSB-C4-BVH.md`.
The first attempt (run 12) was BLOCKED: the original AV'd at `0x005baf48` before the menu on LIVING-PC. After
Mariano's reboot on 2026-10-27 the original boots again. Run-12 crash detail: this file at `4b304fa75`.

## What changed

Harness only. The shadow wrappers of `0x0055fea0`, `0x0055ff90`, `0x00561040`, `0x00561390` (RwpSolverCore23.cpp),
`0x00564040` (RwpSolverLeaves1.cpp) and `0x00564c80` (RwpSolverCore4.cpp) call `ShadowAB::RunTracked` instead of
`ShadowAB::Run`. Unarmed both call only `_impl`. The wrappers are linked into the exe as well, so the default gates
were re-run on this branch's exe (below).

Tool v6 (`make_tool2.py` -> `c4_racediff_v6.py`, from the v5 tool with 4 asserted replacements): the declared-width
arg mask for `0x005646c0` arg 3, an `asiloaded` rpc, and a census-scenario driver (`C4_SCENARIO=census`).

## Default gates (this branch's exe, `mashed_re.exe` `7f3ce5aa...382a`)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D4-WSB-BVH-WRITESET_root`,
under the test slot and `mashed_lock.py run --label D4-WSB-BVH-WRITESET`. All four arms `exit=0 self_exited=True`.

| arm | sha8 | ref (`merge_train.json`) |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

The wrapper swap is hash-neutral on the default build. The first gate attempt hit the 40 min background limit while it
waited for a test slot, and was re-run whole (`gates_run.log`, then `gates_run2.log`).

## Static results (original binary `MASHED.exe.unpatched`, anchor `BDCAE093...3C0E`)

Script `scan_argwidth.py` (recursive descent from the entry, esp delta tracked per path, the stack probe
`0x004a3440` counted as `sub esp, eax`), output `argwidth.txt`.

1. **`0x005646c0` reads only the low 16 bits of `param_3`.** The walk (390 instructions, one `ret` at `0x00564c78`
   with delta 0, no delta conflict) finds 4 loads of the slot and no store: `0x005647e3` -> `and eax, 0xffff` at
   `0x005647ec`, `0x00564830` -> `and ecx, 0xffff` at `0x0056483d`, `0x00564aab` -> `and eax, 0xffff` at
   `0x00564ab4`, `0x00564af8` -> `and ecx, 0xffff` at `0x00564b05`. Each mask precedes any use.
2. **`0x00564c80` reads only the low 16 bits of `param_2`.** Two loads of the original value: `edx` at `0x00564c83`
   feeds `and eax, 0xffff` (`0x00564c96`) and the push at `0x00564cac` (arg 3 of `0x005646c0`, item 1), then is
   overwritten by `mov edx, 0xffff` at `0x00564d0f` with no read in between. `esi` at `0x00564e03` reaches memory only
   through `or ecx, esi` / `mov word [ebp+eax*4+0x881e], cx` (`0x00564e2d`/`0x00564e2f`, 16-bit store), `ecx` is
   overwritten at `0x00564e37`, `esi` at `0x00564e4b` + `and esi, 0x3ff` at `0x00564e54`. `0x00564eae` is a 16-bit load.
   The later slot accesses (`0x00564ed9`, `0x00564ffa` write it, `0x00564eee`..`0x005650e8` read it back) are a spill
   of other values. Ghidra's decomp of `0x00564c80` agrees: `param_2 & 0xffff`, `(ushort)param_2` x2, and the full
   value only as the `FUN_005646c0` argument.
3. **The EAX that `0x00565260` returns is dead at all 3 call sites.** `0x00559fb1` and `0x0055ab61` overwrite EAX
   before any read (`verify/wsbbvh_20261026/scan_eax_use.txt`). At `0x0055aef3`: `mov ax, [edi+0x20]` (`0x0055aeff`),
   `push eax` (`0x0055af06`) as `param_2` of `0x00564c80` (`call` at `0x0055af08`), then `mov eax, [esi+0x68]` at
   `0x0055af0d`. So only EAX's high half reaches `0x00564c80`, as `param_2`'s high half, which item 2 shows is never
   read. The v5 racediff's EAX difference on `0x00565260` (orig `0/1/5`, port `0x16/0xe`) is therefore unobservable.
4. **`0x00565200`'s `RunRegion` region covers every store.** 6 stores, all `fstp [edx+k]` with `edx = param_3`
   (`0x0056520e`), `k` = 0x10, 0x14, 0x18, 0, 4, 8; the last ends at 0x1c = the region size. No call in the body.

Consequence for `0x00564c80`'s v4 BEHAVIOR-DIFF: OFF recorded arg 3 as `Sloc` (the raw dword, high half included,
fell in the stack range), ON as `18`/`19`/`1a`/`1b`. With item 1 the high half is dead, so v6 compares that arg
masked to 16 bits on both sides (PREREG 3). No other arg or callee is masked.

## Runtime legs (run 13, 2026-10-27)

All from this worktree, `MASHED_ROOT=D:/MashedWT/D4-WSB-BVH-WRITESET_root`, sandbox off, every boot under the test slot
and `mashed_lock.py run --label D4-WSB-BVH-WRITESET`. Same exe and `.asi` as the gates below (no rebuild).

### 1. Write-set shadow (PREREG 2): `run_shadow.py`, one hook per boot, census scenario

Summary `shadow_summary.tsv`. Every row: `PATCHBYTE installed=e9`, `A/B-IS-REAL`.

| row | n | divergent | first differing page | note |
|---|---|---|---|---|
| `0x0055fea0` | 24 | 0 | | |
| `0x0055ff90` | 24 | 0 | | |
| `0x00561040` | 24 | 0 | | |
| `0x00561390` | 24 | **2** | `0x007dc000+0xe08` (samples 3, 13) | 22 OK |
| `0x00564040` | 24 | 0 | | |
| `0x00564c80` | 24 | **24** | `0x007dc000+0x8c8` (all) | first boot: `error: could not attach`, NO_LOG, re-run once per PREREG |
| `0x00565260` | 20 | 0 | | |
| `0x00563e70` | 24 | 0 | | |
| `0x00565200` | 48 | 0 | | RunRegion |

Each divergence is one page (`pages=o:1,n:1,diff:1`), `stack=0`, `ret=0`. The log names only the first differing
byte, not the values.

**`0x00561390` at `0x007dce08`.** The byte-anchored scan of the original (`scan_refs.py 0x007dce00 0x007dce10`) finds
`0x007dce08` only outside the BVH code: `mov [esi+0x30], 0x7dce08` at `0x005ab914`, `cmp [esi+0x30], 0x7dce08` at
`0x005ab768`, `lea esi, [esi+0x7dce08]` at `0x005ab9a8`. `0x005ab9a8` is inside `0x005ab980`, the ring-buffer copy that
the port's notes (`PromoLoop_sessionB.cpp:1947-1954`) record as the audio stream pump. Diagnostic re-run with
`MASHED_SHADOW_STW=1` (all other threads suspended during the window, `00561390_stw/`): **CLEAN 24/24**. The PREREG
fixed "any divergent sample = stays C3", so the row stays C3. The STW result is a diagnostic, not a verdict.
[UNCERTAIN] that the 2 samples are another thread's ring-buffer writes. Evidence: clean with STW, the address has no
reference in the BVH bodies. Missing: the writer's pc (`--watch-write` on `0x007dce08` during a 0x00561390 window).

**`0x00564c80` at `0x007dc8c8`, deterministic.** `scan_refs.py 0x007dc8c8 0x007dc8cc`: the only instructions touching
`0x007dc8c8` are in `0x005646c0`, two read-increment-write sites (`0x0056473d` load, `inc edx`, `0x00564764` store,
and `0x00564a0c` / `0x00564a2c`). No other instruction in the image reads it. The port has the same two increments
(`RwpSolverCore4.cpp:136`, `:205`, `DAT_007dc8c8` at `:64`). It is not contamination. In the `0x00561390` STW boot,
nested `0x00564c80` samples still differ at `0x007dc8c8`, 22 of 24, with every other thread stopped.
What is known and what is not:
- `0x005646c0` shadowed on its own (nested, RunTracked) matches 24/24, its write to `0x007dc8c8` included.
- The original `0x00564c80` body and the port make the same 9 calls in the same order: `0x005646c0` once at
  `0x00564cf0` (port `:273`), then `0x00564310` x3, `0x005641b0` x2, `0x00565120`, `0x005651b0`, `0x00565160`.
- The v6 racediff of `0x00564c80` is C4-CLEAN (callee args and returns equal, `0x005646c0` arg 3 at 16 bits).
- [UNCERTAIN] why the counter's final value differs. Missing: the two values. `ShadowTrack` logs only the offset.
  Next step: a value dump of the first differing dword (harness change in `Core/ShadowTrack.h`, then gates re-run).
- Side observation, not in this step's rows: nested `0x00564310` (return-only `Run`) returned `o=6` against `n=7/2/3` in
  24 of 48 samples in that boot. A return-only shadow does not restore pre-state between the two runs, so this is
  not attributed here.

Early exits: 4 of the 10 shadow boots (`0x0055fea0`, `0x00561040`, `0x00561390`, `0x00564c80`) left the race at
14-33 s of the 60 s hold. No WER event exists for any of them. Rows 1 and 3 logged a first-chance
`[trace] OTHER-EXC code=c0000005 info0=0 addr=00002008` at `eip=5cbb7683` / `5cbd7683`. That eip is outside
`MASHED.exe`, and the `.asi`'s preferred base is `0x10000000`. Each of the 4 still reached its sample cap (24) before
exiting, so no verdict is affected. [UNCERTAIN] which module that eip is in. Missing: the module map of a live boot
(one capture attempt missed the process).

### 2. Quick Battle racediff re-reads, tool v6 (PREREG 3): `run_bvh2.py`

| row | verdict | calls off/on | sigs off/on | spawned PIDs |
|---|---|---|---|---|
| `0x00564c80` | **C4-CLEAN** | 6 / 6 | 4 / 4 | 6156 / 19228 |
| `0x00565260` | **C4-CLEAN** | 6 / 6 | 4 / 4 | 1952 / 13140 |

`installed=True` on both ON boots. Static grounds for the two comparator rules are in the section below.

### 3. Census racediff (PREREG 4): NOT RUN

`C4_SCENARIO=census C4_WINDOW="1 3000" run_bvh2.py 00563e70 00565200`, 2 attempts per row. The ON boot reached
`armed: asi=True jmp=[233]` and frame 3001-3006 every time. **Every OFF boot (4/4) failed**:
`frida.InvalidOperationError: script has been destroyed` from `E.asiloaded()` (`c4_racediff_v6.py:453`), in the poll
that waits for the `.asi` before arming. Summary rows `NO-RESULT`, 0 calls on both sides (`racediff_summary.tsv`).
The OFF launch passes no `--hooks`, and `scenario_launch.py:5036` then sets `MASHED_RE_NO_AUTO_HOOK=1`. So the env
the PREREG asks for is in place, and the fault is in the census driver's OFF path or in that boot. [UNCERTAIN] which
one: the launcher's own output in that boot is not kept (`run_census` keeps it in memory only). Next step: write
`box["lines"]` to `<out>/off_launcher.txt` and re-run one OFF boot. The census driver has still never completed an OFF
boot, so it is not yet piloted.

A first leg-3 attempt died in the driver on a console encoding error (`UnicodeEncodeError` cp1252 at
`run_bvh2.py:51`). Its own queued lock waiter (PID 18996) was killed by PID and the leg was re-run with
`PYTHONIOENCODING=utf-8`. Raw data from that attempt: `failed_census1/`.

## Where the same blind spot hits BROAD and SOLV (lead ask)

`scan_blindspot.py` -> `blindspot.tsv`: every C3/C4 row in the three bands, the ShadowAB template its wrapper uses,
and the count of non-ESP stores in the original body. `blind = YES` = wrapper is `Run` (return only) and the body
stores outside its frame, so a return-only shadow A/B never compared that row's out-memory.

| band | blind rows (C3, `Run`, stores / string stores) |
|---|---|
| BROAD (D4-WSB-C4-BROAD) | `0x0055a1f0` 31/1, `0x0055ab30` 1/0, `0x0055bae0` 3/0, `0x0055e050` 45/0 |
| SOLV (D4-WSB-C4-SOLV) | `0x00568990` 9/0, `0x00568dd0` 12/0, `0x00568fd0` 30/0, `0x0056b7a0` 6/0, `0x0056ba30` 4/0 |
| BVH | none left after this branch's swap |

Also to re-scope, not blind by this rule but not fully covered either:

- BROAD has **7 C3 rows with no shadow wrapper at all** (`wrapper = none`); their C3 does not rest on a shadow A/B, so
  the C4 step must establish out-memory some other way (store scan category A, or add a RunTracked wrapper).
- `RunRegion` rows (SOLV 4, BVH 1) compare one hand-declared region only. Each needs the check done above for
  `0x00565200`: every store of the body inside that region.
- Of those 7, 5 bodies store outside their frame (`0x00558100` 4, `0x00558140` 3, `0x0055c000` 3, `0x0055c4f0` 1,
  `0x0055c810` 38). BROAD also has 2 rows **already C4** with stores and no shadow wrapper (`0x0055ac00` 2, `0x0055e200` 7).
  Their promotion evidence should be checked for an out-memory leg.
- BROAD 4 and SOLV 1 rows are `Run` with no store outside their frame: return-only is complete for them.

Re-scope suggestion for D4-WSB-C4-BROAD / SOLV: swap the 9 blind rows to `RunTracked` first (same harness change as
here, gates re-run), then run the census write-set shadow next to the racediff, as this step does.

## Next

- `0x00564c80`: dump the values of the first differing dword in `ShadowTrack` (or `--watch-write` on `0x007dc8c8`),
  then decide defect vs harness. Racediff is already C4-CLEAN, so the write-set leg alone holds it.
- `0x00561390`: write a PREREG for an STW write-set re-run (or name the `0x007dce08` writer). The STW diagnostic here
  is already 24/24 CLEAN.
- `0x00563e70` / `0x00565200`: fix the census OFF boot (keep the launcher log, find why the script is destroyed),
  then run PREREG 4. Both write-set legs are CLEAN already.
- BROAD / SOLV: the 9 blind rows above, same `RunTracked` swap.

## Files

Committed: `PREREG_D4-WSB-BVH-WRITESET.md`, `make_tool2.py`, `c4_racediff_v6.py` (generated), `run_bvh2.py`,
`run_shadow.py`, `scan_argwidth.py`, `disasm_lin.py`, `argwidth.txt`, `scan_blindspot.py`, `blindspot.tsv`,
`scan_refs.py` (byte-anchored reference scan), `shadow_summary.tsv`, `racediff_summary.tsv`, `apply_reclass.py`
(the re-classify transaction), this file. Trackers: `hooks.csv` (5 rows), `re/analysis/CHANGELOG.md` (5 lines).

Raw (not committed), `D:/MashedWT/D4-WSB-BVH-WRITESET_data/`. Run-12 failed boots are in `failed_boot1/`,
`failed_boot2/`, `failed_boot3/` (the per-RVA folders of run 12). Every spawned PID was killed by PID by the tools.
No MASHED or `mashed_re.exe` from this step is left running.

| file | SHA-256 |
|---|---|
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates/gates_run2.log` | `25a0f7efdffcc083ddec1e49b8af95a0aa26d6da8e7ea79e04e4d825bac22f77` |
| `0055fea0/shadow_ab.log` | `e7a7002af7688c268ad9c11e7751f4c4fd77e08253ec752c7ce8fd90f5d1b50a` |
| `0055ff90/shadow_ab.log` | `b69a3c0b5c1751c0ac5765b4fc960b79a0b41cf5a1f69730a1761670b1f43fa9` |
| `00561040/shadow_ab.log` | `cb243e57c7d47eb36e5a14026ab3e2f4b2913a9937368c7aba4a2a2544ba1ca8` |
| `00561390/shadow_ab.log` | `13ce691db78a6c412dedfc8ddc6baa4e6ba32ddad04b710a19af144d3697618b` |
| `00561390_stw/shadow_ab.log` | `49da92b82b4cbba496731f70c7d5f6f102b847464e46035fea9966d54dfa0805` |
| `00564040/shadow_ab.log` | `b5cc3ba9a086ddf22ffcb36fdb91b09be89684d793bcadc74d1b5da706da4036` |
| `00564c80_r1/shadow_transcript.txt` | `1188828fef9836dda7a6853cd79a72877c58e7f908bf404b027995209e80cd4b` |
| `00564c80/shadow_ab.log` | `bf61cbf358e30305cefe621d535a1ce1d26b858b3b898f3f1457acbb2e3afb7a` |
| `00565260/shadow_ab.log` | `d73630ab5c9a005c2024600290389a7f3d3b7ba003923d068b73d56e097c5022` |
| `00563e70/shadow_ab.log` | `4693e91c73a0fd4f9d6929d9fe3581abc52328b413a6bc1c3af83715ab045d3e` |
| `00565200/shadow_ab.log` | `cb14e1e14eed81696e3032b56dfa6a7ed70188c57a985d44ac4928dd5393cbfa` |
| `shadow_batch3.log` / `shadow_batch4.log` / `shadow_batch5_stw.log` | `1c1a0dc6...125cc` / `86c27f54...124b` / `273e61c6...f711` |
| `00564c80/c4_racediff_samples.json` | `5174ddd9634aa7bbf6642c203ee7a99f100312bdaa4cc2debaddf8ff8ccfedb7` |
| `00565260/c4_racediff_samples.json` | `ce55a9085698fdda6f745e04cc9d8c988d39c13f42aaf049881c3bf14480d2c2` |
| `00563e70_census/transcript.txt` | `92aff82120941e79a002b11b479a7cae2b66780172c02761ade57df1d14f6ecb` |
| `00565200_census/transcript.txt` | `1c597319f0b2de8657acd78e9a42c238cae1bcaff0de33d3fc1258b0bb20c37e` |
| `racediff_leg2.log` / `racediff_leg3.log` | `c9ae828d...3f13` / `12c75c6a...a2a8` |

Build: `mashed_re.exe` `7f3ce5aa920230482cb3bfe6146f2a818edae3d28b26a3ffb917969a62af382a`, `mashed_re_dev.asi`
`5d18fbc80373bf23b40c347db517e788ee6a70a4cae734bcb4d2bb02b9c2503a` (in the private root only, hash checked before the
run), `mashed_re_dev.map` `ba20e81816abed13485db63f04f11adb89f1321ac99c80dbea37b8e1c3b786f5`.
`original/mashed_re_dev.asi` not touched.
