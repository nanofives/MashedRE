# RESULT HB-CHUNK: chunked PHASE 2 install in c4_navigate_batch.py (`--on-chunk`)

Verdict: **GREEN.** Harness-only change. Default gates unchanged on the branch exe.

Branch `orch/HB-CHUNK` from `orch/integration` @ `4ff9c8468`. Backlog source: `re/HARNESS_BACKLOG.md`
(entry now marked LANDED).

## What changed

- `re/frida/c4_navigate_batch.py`
  - PHASE 2 installs candidates in `chunked(rvas, on_chunk)` groups, one boot per group. A crashing
    group goes to `bisect_bad(group)` alone. The group's good set is re-installed once to read the
    JMP bytes. `--on-chunk N` (default 16, `0` = the old all-in-one-boot path).
  - `run_on()` wraps every ON boot: re-runs once on a piz-collision crash
    (`MASHED.exe+0x1509b4` / `+0xe66db`, U-9677), and applies `--force-bad`.
  - `--force-bad 0xRVA`: harness self-test only. Any ON boot that installs that RVA overwrites its
    installed JMP with `mov eax,[0]` (`A1 00 00 00 00`) once the nav completes, so the next call
    faults for real inside MASHED.exe. The run prints a banner that its verdicts are not evidence.
  - `--out-tag X` writes `log/c4_nav_batch_resultX.{json,md}`. `_meta` records `on_chunk`,
    `force_bad`, and a per-group log (`rvas`, `crashed`, `eip`, `bad`).
- `re/frida/canonical_c4_navigate.py`
  - Game root honours `MASHED_ROOT` (a root that contains `original\`, the `scenario_launch.py`
    convention). Unset = the old `<tree>/original`.
  - `run(..., poison=None)` and agent rpc `poison(addr)` (fires at `COUNT_GATE`, after the nav).
  - Prints the spawned PID per boot. Kill path unchanged: `dev.kill(pid)` on its own PID only.

## Live verification (booted original, private root, own PIDs)

Root `D:\MashedWT\HB-CHUNK_root` (robocopy of `original\`, 265 files), `MASHED_ROOT` set, branch
`mashed_re_dev.asi` (sha256 `35716f41f6aee4b1...`) copied into that root only. `original\` untouched,
no redeploy. All boots under `mashed_lock.py run --label HB-CHUNK`.

RVA set (6, frontend, all exercised on nav `4,4`): `0x0042e3a0 0x00472f40 0x004730b0 0x0042aae0
0x00493fc0 0x004c19f0`.

### Run 1: `--on-chunk 3 --force-bad 0x00472f40` (10 boots)

| step | installed | PID | outcome |
|---|---|---|---|
| PHASE 1 OFF | none (count 6) | 30096 | counts 679/679/679/1353/1358/2032 |
| group 0 | 0x0042e3a0,0x00472f40,0x004730b0 | 23988 | crash eip `0x472f40` |
| bisect 3 | same | 23616 | crash |
| bisect 1 | 0x0042e3a0 | 2336 | survive |
| bisect 2 | 0x00472f40,0x004730b0 | 24500 | crash |
| bisect 1 | 0x00472f40 | 9660 | crash alone, eip `0x472f40` |
| bisect 1 | 0x004730b0 | 25536 | survive |
| good re-install | 0x0042e3a0,0x004730b0 | 18568 | survive, JMP read |
| group 1 | 0x0042aae0,0x00493fc0,0x004c19f0 | 11492 | survive (navTaps=2) |

`_meta.groups` = g0 crashed, bad `[0x00472f40]`. g1 not crashed, bad `[]`. Verdicts: 5/6 trio-OK,
`0x00472f40` HOLD `no-JMP,crashes`. Group 1 was never re-booted or bisected: the blast radius is the
crashing group only. No spawned PID was left alive after the run.

### Run 2 (control): `--on-chunk 0`, no `--force-bad` (2 boots)

One group of 6 (PID 24656) installed and survived, 6/6 trio-OK, `0x00472f40` included. So the
run-1 failure of `0x00472f40` comes from the injection alone, and the `0` path still gives the old
single-boot behaviour.

Extracts: `c4nav_forcebad.md`, `c4nav_control.md` (verdict tables) in this folder.

## Gates (branch exe `mashed_re.exe` sha8 `44dda7a3`, `MASHED_NO_DEPLOY=1` build)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT` = the private root.

| gate | got | ref (`merge_train.json`) |
|---|---|---|
| G-CTRL H_DEF step/gates/ptrace | 5943c5f1 / b493262b / 09551e6a | 5943c5f1 / b493262b / 09551e6a |
| B K0 / K1 / K26 | 3a03670c / 2516d903 / 53778664 | 3a03670c / 2516d903 / 53778664 |

All self-exited, exit 0.

## Raw files (not committed, `D:\MashedWT\HB-CHUNK_data`)

| file | sha256 |
|---|---|
| chunkrun_forcebad.log | 3cf2a795f9eee5a2dd70c96a8cadadec5439dd3e5c5dc84cbe36c5fee08a5abf |
| chunkrun_control.log | 41fa640b7ebf1870f1e386068b5e95e0bb88c6a1b1ab960e0f886fd6b3ef4acd |
| gates.log | b85f04d0923563171db070495bed278063e6a556f8adc971e7ee0adb7af243b3 |
| arms/H_DEF.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| arms/H_DEF.gates.csv | b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f |
| arms/H_DEF.ptrace.log | 09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 |
| arms/B_DEF_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| arms/B_DEF_K1.step.csv | 2516d9034a1774893bf2f59edd368c3783ad6cfe0ed491dc68b3d84a0afec7f6 |
| arms/B_DEF_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |

## Open

- [UNCERTAIN] `--on-chunk` default 16 is untuned. No large-batch timing exists yet, so the boot
  saving against the old full-range bisect is reasoned, not measured.
- `bisect_bad` re-boots the full crashing group once before splitting (a flake guard). That costs one
  boot per crashing group. Kept deliberately.
- No tracker rows changed. Run-1 verdicts are self-test output, not evidence for any hook.
