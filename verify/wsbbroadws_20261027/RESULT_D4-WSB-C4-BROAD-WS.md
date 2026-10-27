# RESULT D4-WSB-C4-BROAD-WS: write-set C4 for 5 BROAD rows

Verdict: **RED**. No row reaches C4. Default gates are unchanged on this branch's exe, and the harness change is
hash-neutral. The PREREG (`PREREG_D4-WSB-C4-BROAD-WS.md`, `1ac9ddef2`, committed alone before any arm) was applied
as written. No tracker changed.

| row | write-set shadow (leg 2, census) | racediff v6 (leg 3) | level |
|---|---|---|---|
| `0x0055c000` | **CLEAN 24/24** | Quick Battle: not observed (2 attempts). Census: **NOT RUN** (OFF boot fault) | stays C3 |
| `0x0055c0f0` | **CLEAN 24/24** | Quick Battle: not observed (2 attempts). Census: **NOT RUN** (OFF boot fault) | stays C3 |
| `0x0055bae0` | **CLEAN 24/24** | Quick Battle: not observed (2 attempts). Census: **NOT RUN** (OFF boot fault) | stays C3 |
| `0x0055ab30` | **CLEAN 24/24** | **BEHAVIOR-DIFF** (callee arg 2 high half, see below) | stays C3 |
| `0x0055a1f0` | **DIVERGENT 1/24** at `0x00624048` | **BEHAVIOR-DIFF** (callee arg 2 high half, proven dead) | stays C3 |

Branch `orch/D4-WSB-C4-BROAD-WS` off `orch/integration` @ `cb1ba12de`, then merged `orch/integration` @ `fc09a9e04`
(the `D4-WSB-NARROW-FLIP` merge commit; `git merge-base --is-ancestor fc09a9e04 HEAD` = true) before any arm.
Every arm below ran on that tree. Scope: `verify/wsbc4scope_20261027/RESULT_D4-WSB-C4-SCOPE.md` (group BROAD-WS).

## Harness change (`879e1ad65`)

- `0x0055a1f0`, `0x0055bae0` (`RwpSolverBroadphase3.cpp`) and `0x0055ab30` (`RwpSolverGlue5.cpp`): wrapper
  `ShadowAB::Run` -> `ShadowAB::RunTracked`.
- `0x0055c000` (`RwpIntegrator.cpp`): the body becomes `static FUN_0055c000_impl`, and a new `extern "C" FUN_0055c000`
  wrapper calls `ShadowAB::RunTracked` (`kPhaseRace`). The extern name stays, so `CollisionBodyCreate.cpp`,
  `RwpSolverCore16.cpp` and `RwpSolverCore18.cpp` link unchanged.
- `0x0055c0f0`: already `RunTracked`.

Unarmed, every wrapper calls only `_impl`.

## Leg 1: default gates (this branch's exe `mashed_re.exe` `879c1dd6...a751`)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D4-WSB-C4-BROAD-WS_root`,
under the test slot and `mashed_lock.py run --label D4-WSB-C4-BROAD-WS`. All four arms `exit=0 self_exited=True`.

| arm | sha8 | ref (`merge_train.json`) |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

## Leg 2: write-set shadow (`run_shadow.py`, census track 0 / mode 10 / 4 cars / 60 s, one hook per boot)

`shadow_summary.tsv`. Every row `PATCHBYTE installed=e9`, `A/B-IS-REAL`, 24 samples.

| row | n | divergent | page counts over the 24 samples |
|---|---|---|---|
| `0x0055c000` | 24 | 0 | `o:0,n:0` x24 |
| `0x0055c0f0` | 24 | 0 | `o:0,n:0` x24 |
| `0x0055bae0` | 24 | 0 | `o:0,n:0` x24 |
| `0x0055ab30` | 24 | 0 | `o:1,n:1,diff:0` x24 |
| `0x0055a1f0` | 24 | **1** | sample 0 `o:2,n:0,diff:1` at `0x00624000+0x48`, samples 1-23 `o:0,n:0` |

What `o:0,n:0` covers (`Core/ShadowTrack.h` header): every committed writable non-executable private or
`MASHED.exe` `.data` page is tracked, except the current thread's stack. The caller's 4 KB stack window above the
wrapper's return address is snapshotted and compared on its own (`stack=0` on all 24 samples). So for
`0x0055c000` / `0x0055c0f0` / `0x0055bae0`, no write landed outside that window, and the window matched. The log
does not record whether the scope note's pointer stores (`fstp [eax]` x3 in `0x0055bae0` and `0x0055c000`, 8 stores
in `0x0055c0f0`) executed in those samples. Each runs only behind a NULL test (`0x0055bae0` and `0x0055c000`: getter
result and matrix non-NULL). This is a coverage limit of the leg, not a divergence.

`0x0055bae0` executed in the census scenario (24 samples with the JMP live), though `wsb_census.tsv` lists it
NOT-EXEC. Its direct callers in the original (`callers.txt`): `0x0055bba8`/`0x0055bbbe`/`0x0055bbd6` (inside
`0x0055bb70`) and `0x00572a84`/`0x00572ba0` (inside `0x005729a0`). `0x0055bb70` is called from `0x00459f0c`,
`0x0048185f`, `0x005612b8` (`0x00561280`) and `0x00561480` (`0x00561390`).

**`0x0055a1f0` at `0x00624048`.** In sample 0 only, the original wrote 2 pages and the port wrote none. One of
them differs, first at `0x00624048`. The byte-anchored reference scan (`scan_refs.py 0x00623f00 0x00624060`) finds
no instruction with `0x00624048` as a displacement or immediate. The nearest are `mov dword ptr [esi], 0x62403c` at
`0x0057c31a` (`FUN_0057c300` stores `&DAT_0062403c` as the first dword of a 0x34-byte allocation) and
`mov [0x624044], ecx` at `0x0057c4ef` (`FUN_0057c4b0`). `0x00624048` = `0x0062403c + 0xc`.
[UNCERTAIN] what writes `0x00624048` on the original's first call, and whether it is this body or another thread.
Missing: the dword values (`ShadowTrack` logs only the offset; D4-WSB-BVH-FU's `823aadb1e` adds the value log) and an
STW re-run. The STW diagnostic boot here did not run: its driver's 900 s timeout expired while waiting for the
machine lock (held by D4-WSB-BVH-FU's run). No game was spawned for it.

## Leg 3: racediff, tool v6 unmodified (`run_bvh2.py` -> `verify/wsbbvh_20261027/c4_racediff_v6.py`)

`racediff_summary.tsv`. `installed=True` on every ON boot.

**Quick Battle window (frames 300-1200).**

| row | verdict | calls off/on | sigs off/on | PIDs (off/on, last attempt) |
|---|---|---|---|---|
| `0x0055c000` | not observed, 2 attempts | 0 / 0 | 0 / 0 | 4852 / 8200 |
| `0x0055c0f0` | not observed, 2 attempts | 0 / 0 | 0 / 0 | 17048 / 15776 |
| `0x0055bae0` | not observed, 2 attempts | 0 / 0 | 0 / 0 | 13832 / 19536 |
| `0x0055ab30` | **BEHAVIOR-DIFF** | 6 / 6 | 1 / 4 | 10700 / 11184 |
| `0x0055a1f0` | **BEHAVIOR-DIFF** | 6 / 6 | 1 / 1 | 17964 / 12760 |

Both diffs are in one callee argument, and only in its high half:

- `0x0055a1f0` -> `0x00564040` (4 calls per call). OFF first call `(h0, h1, h2, h3, 0)`, ON `(h0, 0x18, h1, h2, 0)`.
  Calls 2-4: `0x19`/`0x1a`/`0x1b` on both sides. So arg 2 of the first call is a pointer-range dword on OFF and
  `0x18` on ON, and the heap tokens after it shift by one.
  **Static: `0x00564040` reads only the low 16 bits of `param_2`** (`argwidth_00564040.txt`, 67 instructions, one
  `ret` at `0x005640fa`, delta 0, no conflict): one load `mov ebp, [esp+0x14]` at `0x00564047`. `ebp` is then read
  only by `mov ecx, ebp` / `and ecx, 0xffff` (`0x0056404e`/`0x00564050`) and the 16-bit `cmp cx, bp`
  (`0x00564063`, `0x005640b5`) before `pop ebp` (`0x005640f7`). Ghidra types it `ushort param_2`. So the diff is
  unobservable, but the PREREG did not mask it, and the row stays C3 (it also fails leg 2).
- `0x0055ab30` -> `0x00565260` and `0x00564c80`. OFF arg 2 = `Sloc` (a stack-range dword) on both callees in all 6
  calls; ON arg 2 = `0x18`/`0x19`/`0x1a`/`0x1b` (2/2/1/1). Arg 1 and arg 3 match. For `0x00564c80`, D4-WSB-BVH-WRITESET
  proved that only the low 16 bits of `param_2` are read (`verify/wsbbvh_20261027/argwidth.txt` item 2). For
  `0x00565260` `param_2` there is no proof yet. The walk (`scan_argwidth.py 0x00565260 ... 8`) shows the slot loaded
  into `edi` at `0x0056526b`, rewritten at `0x005652a1`/`0x005652d5`, address-taken at `0x005652c3`/`0x0056532a`, and
  reloaded into `ebx` at `0x00565497`. The `edi` flow from `0x0056526b` was not followed.
  [UNCERTAIN] whether `0x00565260` reads the high half of its incoming `param_2`. Missing: that `edi` flow.

**Census window (frames 1-3000, `C4_SCENARIO=census`), the 3 rows Quick Battle did not observe.** 2 attempts per
row. No OFF boot completed. `0x0055c000` attempt 1: `attach failed` on both the ON and the OFF boot. The other 5
attempts: the ON boot reached `armed: asi=True jmp=[233]` and frame 3001-3012, then the OFF boot failed the same way
as in D4-WSB-BVH-WRITESET, `frida.InvalidOperationError: script has been destroyed` (5/5).
Summary rows: `0x0055c000_census` HOLD (from attempt 1's result file: not observed, no-JMP), `0x0055c0f0_census` and
`0x0055bae0_census` NO-RESULT.
Per the PREREG the census racediff is NOT RUN for all three. D4-WSB-BVH-FU's v7 tool (`823aadb1e`: "census OFF loads
the asi (--hooks __none__)") targets this fault. It is not merged and not used here.

## Next

- `0x0055c000`, `0x0055c0f0`, `0x0055bae0`: the write-set leg is CLEAN on this build. Each needs only a census
  racediff once the OFF boot works (BVH-FU's v7 after it merges), under a PREREG addendum naming the tool.
- `0x0055ab30`: write-set CLEAN. Prove the width of `0x00565260` `param_2` (follow `edi` from `0x0056526b`). Then a
  PREREG addendum with declared-width masks for `0x00565260` arg 2 (if proven) and `0x00564c80` arg 2 (proven), and
  a racediff re-run.
- `0x0055a1f0`: log the `0x00624048` values (BVH-FU's ShadowTrack value log) and run an STW boot. Mask `0x00564040`
  arg 2 (proven 16-bit here) in the same addendum.
- The harness commit `879e1ad65` is hash-neutral (gates above) and is what every follow-up above needs.

## Files

Committed (`verify/wsbbroadws_20261027/`): `PREREG_D4-WSB-C4-BROAD-WS.md`, `run_shadow.py` and `run_bvh2.py` (copies of
the BVH-WRITESET drivers with label and paths changed, plus `BROADWS_DATA` / `BROADWS_SUMMARY` env overrides),
`scan_callers.py`, `callers.txt`, `argwidth_00564040.txt`, `shadow_summary.tsv`, `racediff_summary.tsv`, this file.

Raw (not committed), `D:/MashedWT/D4-WSB-C4-BROAD-WS_data/`:

| file | SHA-256 |
|---|---|
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates/gates_run.log` | `487b121fbdab55b0c4c0ed6ad20a1aaafb38bc9d009d977b6f31f7b31db80443` |
| `0055a1f0/shadow_ab.log` | `6ce88a6f3c89d005cbe53a335e75284fef684fd57169216ff42b405fe1270073` |
| `0055ab30/shadow_ab.log` | `031669708a0b76433985859be60d71cdd4f3883ec9406da4ddfc516a2e53c3eb` |
| `0055bae0/shadow_ab.log` | `36a0c9ce63b05efd6f73b23322ce0007a85ef220d93b1ccb899423fe7b1a22ba` |
| `0055c000/shadow_ab.log` | `1db453524e75fffd82aa990dd95c54f75e42b6fced26357e473bbd9eb03c2aa8` |
| `0055c0f0/shadow_ab.log` | `979f6e10fd62e507bfeef24095f6e138b26a4a38821a886e43210122d7cebfa2` |
| `shadow_batch1.log` | `d5f1aace25a932169fa016a4ea1cd3f2b797786a7a6740a6b08892b41904672f` |
| `0055a1f0/c4_racediff_samples.json` | `4c6c2410975ccfd1c97f332b3ae9635267a9a90a3f40099f3b01bcd8afd5a2b9` |
| `0055ab30/c4_racediff_samples.json` | `f021b83e2c2e8ae228239b9b21148713ccddf06603a26cde5c32e12c3a67b45e` |
| `0055c000` / `0055c0f0` / `0055bae0` `c4_racediff_samples.json` (empty, identical) | `3968b8b8d5257bbb2c9c5e8ea42e559746d0513df7123f0e1d3c214ebd50c9a8` |
| `racediff_leg3.log` | `d3401edc91ee19480936c59d8eaa06607046421e02bc92da5638836a05b45910` |
| `racediff_leg3b.log` (census + the STW attempt) | `a2d571ea7166f8baa5f37ecdb602ad0851b93687ce779e3738c42be87589782e` |
| `0055c000_census/transcript.txt` | `73cc9ba0c8e148c2b92d25446514a438ddc68e9ef6ebe8e5975efa1905e80b00` |
| `0055c0f0_census/transcript.txt` | `1c6b8783d46039888937caa94793724b06424b16b0f6fd2f221d8b73f5efff52` |
| `0055bae0_census/transcript.txt` | `06b170a918c8940fe56d306e1d45eb8e419ac43c76b27e9b6148fb5f0b3910c8` |

Build: `mashed_re.exe` `879c1dd6c5806e763e89b13fdc62ead3075fdc1eca5f25e678c804af0bb1a751`, `mashed_re_dev.asi`
`19768c2e271d26d9e29af54713c9f9edb811a8e874ea6f9b87c76f19bba69502` (private root only, hash checked after copy),
`mashed_re_dev.map` `125c7e3227f37c226a2c5066b6f1dac09cadcfdab9095eef6aa66a4fd2ffb00c`. `original/mashed_re_dev.asi` not
touched. Every game PID was spawned and killed by the drivers. No process of this step is left running. No watcher
was started (Monitor tasks ended with their streams).
