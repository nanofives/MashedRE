# HANDOFF RO-MENU-REPLAY (2026-11-12 ~12:00, reboot-coordinator stop)

Worktree `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-MENU-REPLAY`, branch `orch/RO-MENU-REPLAY`.
Tooling commit `54d3aab25`, plus a later uncommitted-at-the-time edit, the boot/menu split of the call counters
(`calls_boot`, detorig.js `PV.atTick1`), committed with this file. Ledger row: `working`.

## Done
- Boot mode of the replay oracle (`re/frida/detorig.py --boot`, `js/detorig.js` cfg.boot):
  - frida-spawn
  - install at the PE entry `0x004a4bb7` on the main thread
  - the clock advances per call until the first main-loop tick, then tick-only
  - per-tick hashes from tick 0, warp at `--warp-tick 600`
  - menu checkpoint k=100 (`--menu-ckpt`)
  - negative control at a tick (`--neg-tick`)
  - phase bytes per tick (`meta.ticks.phases`)
- Arm runner: `verify/romenu_20261112/run_arm.sh` (takes `mashed_lock` itself; wrap it in `test-slot`). Env:
  `WT=600 CAP=4000 TMO=700`.
- Segments: the first tick is already phase 1 (all of boot runs before the first tick).
  - menu = ticks 0..599
  - race = ticks 600..3323 (the load runs inside one tick)
- References r1 / r2 (`D:/MashedWT/RO-MENU-REPLAY_data/r1`, `r2`):
  - `prov_compare.py` strict, 0 allowed blocks: IDENTICAL-FOR-N 3324 of 3324 (menu 600, race 2724)
  - this uses the TIMER-PHASE rule, now applied at the first race tick
  - every dword that differs at the menu checkpoint is a MASK / RULE row of `re/frida/warp_normalize.tsv`
- provlink self-test: `provlink_selftest.py` ok=40 bad=0.
- Census (`D:/MashedWT/RO-MENU-REPLAY_data/census/prov.json`): 24 of the 113 PROV-INSITU NOT-EXECUTED bodies run.
  - boot only: 13
  - menu: 9, `0x0043dfd0` 599 calls
  - race: 2, `0x00429bd0` 18 calls and `0x004427c0` 19 calls
- Bisect (`prov_bisect.py --any-segment --tag m`): state in `D:/MashedWT/RO-MENU-REPLAY_data/m_state.jsonl`, 16 of 24.
  - IDENTICAL-FOR-3324: 12 bodies: `0x00429bd0`, `0x00433f40`, `0x004627f0`, `0x00473c20`, `0x004d4f00`, `0x00475a00`,
    `0x004938e0`, `0x00493fd0`, `0x00555360`, `0x005a6b70`, `0x004022a0`, `0x0042e5b0`
  - CRASH in boot (single runs, AV `0xc0000005` before the first tick, 1 run each, not yet repeated): `0x0045d460`,
    `0x0048e820`, `0x00499400`
  - DIVERGED@16 (menu), block `0x898800`: `0x0043d2a0`
  - Run `m_090404_019_4` was killed mid-run for the reboot. It is not in the state file and the resumed driver
    re-runs those bodies.

## Next
1. Resume the bisect (it skips the 16 done):
   ```
   cd <worktree>; export WT=600 CAP=4000 TMO=700; D=D:/MashedWT/RO-MENU-REPLAY_data
   test-slot py -3.12 re/tools/prov/prov_bisect.py --bodies verify/provinsitu_20261111/bodies_all.json --census $D/census \
     --ref $D/r1 --ref2 $D/r2 --tag m --rvas $(paste -sd, $D/notexec.txt) --any-segment --group 4 --max-runs 40 \
     --data $D --arm verify/romenu_20261112/run_arm.sh --arm-locks | tee -a $D/bisect_m.log
   ```
   8 bodies remain, among them `0x0043dfd0`.
2. Repeat each CRASH and DIVERGED single once to confirm the verdict, as PROV-INSITU did (the `rep` runs).
3. Negative control in the menu: `run_arm.sh negm --negctl 1 --neg-tick 300`. Expected: divergence at tick 300 or
   later.
4. Optional: a third reference, r3, to check that the reference stays stable.
5. Write `verify/romenu_20261112/RESULT_RO-MENU-REPLAY.md`:
   - counts per verdict for the 113 rows (24 run, 89 still NOT-EXECUTED in this scenario)
   - the self-tests: references 3324/3324 strict, provlink 40/40, negative control
   - raw file SHA-256 list
   - no build input changed, so no builds, `ddkobj` check or gates
6. `roadmap.ps1 set RO-MENU-REPLAY review -Evidence verify/romenu_20261112/RESULT_RO-MENU-REPLAY.md`, then send the
   REPORT block.
