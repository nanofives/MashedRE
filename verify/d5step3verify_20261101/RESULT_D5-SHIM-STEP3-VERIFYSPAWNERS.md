# RESULT D5-SHIM-STEP3-VERIFYSPAWNERS (2026-11-01)

Text-only leg of D5 shim removal, step 3. Branch `orch/D5-SHIM-STEP3-VERIFYSPAWNERS` from `orch/integration`.
No game boot, no `.ps1`/`.bat` edit, no edit of any `verify/` file other than this RESULT.
Background: `verify/d5autoloadsweep_20261030/RESULT_D5-SHIM-AUTOLOAD-SWEEP.md`, and (branch
`orch/D5-SHIM-DINPUT8-SPAWNERS`) `verify/d5dinput8spawners_20261101/RESULT_D5-SHIM-DINPUT8-SPAWNERS.md` + `re/frida/dev_asi.py`.

**VERDICT: GREEN for the scoping leg.** 22/22 verify/ spawners classified: 15 stock-only (a), 7 hooks-ON that relied on the autoload (b), 0 retired (c).
`re/tools/spawner_audit.py` exits 0 on the `orch/D5-SHIM-DINPUT8-SPAWNERS` tree and exits 1 on today's `orch/integration`
(21 live files that branch still has to migrate), so it goes green when that branch merges.
One finding for Mariano: 5 live `re/frida` spawners are not covered by the DINPUT8 branch and need a migrate-or-retire call (sections 4 and 6).

## 1. Detection rule (reproduces the row's 47 / 25 / 22)

A file is a SPAWNER when it matches `\.spawn\(|\bPopen\b|Start-Process` AND `MASHED\.exe|original_exe|MASHED_EXE`.
It is COVERED when it mentions `MASHED_RE_NO_AUTO_HOOK` or `dev_asi`. On `verify/*.py|ps1|bat` this gives
47 spawners, 25 covered, **22 uncovered**, the counts in the ledger row. (A first, wider pattern with `subprocess.run` and
`CreateProcess` gave 72/24/48, because it also matched Ghidra and tasklist callers.)

## 2. The 22 verify/ spawners

(a) stock-only, harmless once `dinput8.dll` is gone. (b) hooks-ON arm that got the asi from the autoload. (c) retired by DINPUT8-SPAWNERS.
Why (a) for a file that only wraps another script: it `Popen`s `scenario_launch.py` / `camera_probe.py` without `--hooks`, i.e. the stock arm,
and after DINPUT8-SPAWNERS that stock arm sets `MASHED_RE_NO_AUTO_HOOK=1`.

| # | file | class | evidence |
|---|---|---|---|
| 1 | `capfix_20261016/run_capture.py` | a | Popen of the capture tool, original-side capture, no hook env |
| 2 | `dupinstall_20261016/install_observe.py` | b | header line 13: "Requires: original/ has the deployed dinput8 proxy + fresh mashed_re_dev.asi"; installs via `MASHED_HOOK_ONLY` |
| 3 | `dupinstall2_20261016/install_observe.py` | b | same header, same `device.spawn` at line 98 |
| 4 | `dupinstall2_20261016/c4_racediff.py` | b | ON run sets `MASHED_HOOK_ONLY` to the RVA list (line 151), asi supplied by proxy |
| 5 | `dupinstall2_20261016/c4_verify.py` | b | ON run `MASHED_HOOK_ONLY=<all candidates>` (line 83) |
| 6 | `fe_residuals_20261016/u9672/orig_hold.py` | a | `U.spawn()` of the original, Frida read only, no hook env |
| 7 | `fxgates_20261026/orig_fx_gates_count.py` | a | original-side counter via `scenario_launch.py`, stock arm |
| 8 | `fxinputs_20261026/orig_fx_inputs.py` | a | same |
| 9 | `fxrebase_20261027/orig_fx_rebase.py` | a | same |
| 10 | `fxslot2_20261027/orig_alive_series.py` | a | same |
| 11 | `fxslot2_20261027/orig_fx_gates_count.py` | a | same |
| 12 | `fxstate7_20261027/orig_fx_state7.py` | a | same |
| 13 | `intromovies_20261101/run_audio.py` | a | arm O = stock `<root>/original/MASHED.exe`, arm S = `mashed_re.exe` |
| 14 | `librwrest_checks_20261016/run_orig_arms.py` | a | Popen of the original-side capture, no hook env |
| 15 | `librwrest_checks_20261016/run_stand.py` | a | runs `mashed_re.exe` (standalone); `MASHED.exe` only appears as the root-copy check |
| 16 | `r10_20261016/bt_capture.py` | a | original-side backtrace capture, no hook env |
| 17 | `rres_20261016/probe_shadow.py` | a | original-side capture, no hook env |
| 18 | `rsky_20261016/capture_one.py` | a | original-side capture, no hook env |
| 19 | `wsbbvh_20261026/c4_racediff_bvh.py` | b | OFF=`__none__`, ON = reimpl subset by `MASHED_HOOK_ONLY` (lines 14, 23) |
| 20 | `wsbbvh_20261026/make_tool.py` | a | string-patches a racediff source (lines 76-78), does not itself run a hooks arm |
| 21 | `wsbbvh_20261027/c4_racediff_v6.py` | b | same ON/OFF recipe as 19 (lines 15, 24) |
| 22 | `wsbc4trk2_20261029/bootcheck.py` | b | `MASHED_HOOK_ONLY=__none__` (line 37): checks the game boots with the asi mapped and nothing installed. Without the proxy it boots the stock exe and the check changes meaning |

Counts: (a) = rows 1, 6-18, 20 = 15. (b) = rows 2, 3, 4, 5, 19, 21, 22 = 7. (c) = 0, because DINPUT8-SPAWNERS rewrote and retired only `re/` and `scripts/` files, none of these.

The 7 (b) files are the ones that would get a one-line header `# RETIRED-NEEDS-DEV-ASI: hooks-ON arm relied on the dinput8 autoload, use re/frida/dev_asi.py`
if the policy below is amended to add headers. Nothing was edited here.

## 3. Policy proposal

verify/ scripts are frozen evidence of past runs and are EXEMPT from migration. Rationale: each belongs to a RESULT whose arms already ran,
re-running one means re-deriving a historical hash, and the 7 (b) files document how that arm was produced at the time. A live need to repeat a (b) arm
goes through `re/frida/dev_asi.py` in a new script, not an edit of the old one. `spawner_audit.py` reports verify/ and never fails on it.

## 4. Live re/ and scripts/ spawners the guard found

Run on the DINPUT8 branch tree (`git archive orch/D5-SHIM-DINPUT8-SPAWNERS re scripts`): 141 spawners, 120 covered, **21 uncovered**.
This leg reviewed all 21 and put them in `re/tools/spawner_audit_allowlist.txt` with a reason each:

- stock-only (8): `probe_prompt_calls`, `run_trace_boot_crash`, `focus_pause_check`, `patch_skip_powerups` (retired), `fix_boot_elevated.ps1`, `mashed_shot.ps1`, `observe_runtime.ps1`, `ttd/record_menu.ps1`.
- self-loads-asi (4): `bisect_save_hooks`, `launch_with_crash_catch`, `test_bypass5`, `mass_canonical_observe_wave2`. They `Module.load` the asi themselves. Under the proxy without the var they could double map, after removal they are fine.
- not-an-exe-spawn (4): `run_diff_parallel` and `orig_rampwatch` (Popen of covered scripts), `diag.py` and `catch_standalone_phys_crash` (spawn `mashed_re.exe`).
- **NEEDS-DEV-ASI (5)**: `auto_count_at_menu`, `catch_crash_eip`, `canon_observe_refused`, `probe_005ab100`, `mass_canonical_observe_wave3`. Their hooks-ON behaviour came from the autoload, and the DINPUT8 branch missed them (its survey keyed on the var, and these never set it). Allowlisted only to keep the guard green.

On today's `orch/integration` the tool exits 1 with 21 flagged: the files DINPUT8-SPAWNERS migrates (19 `.py` plus `bisect_asi_boot.ps1`, `bisect_hook_index.ps1`).
That is expected and clears when the branch merges (verified by the archive run). `wave2` and the two `.ps1` are classified from the DINPUT8 tree, not from this branch.

## 5. Tool

- `re/tools/spawner_audit.py` (stdlib). Scans `re/` and `scripts/` `*.py|*.ps1` (not `re/prior_art`, `re/analysis`) as live, `verify/` `*.py|ps1|bat` as report-only.
  Options: `--root DIR` (a bare archive works, the allowlist falls back to the one beside the script), `--list-verify`, `--quiet`. Exit 1 on any non-allowlisted live hit.
- `re/tools/spawner_audit_allowlist.txt`: 21 entries, reasons inline.
- Limits: a text scan. A file that mentions `dev_asi` only in a comment counts as covered, and a spawner built from a helper without `.spawn(`/`Popen`/`Start-Process` is missed.

| tree | live spawners | covered | allowlisted | flagged | verify/ spawners / covered / uncovered | exit |
|---|---|---|---|---|---|---|
| `orch/D5-SHIM-DINPUT8-SPAWNERS` (re/ + scripts/ archive) | 141 | 120 | 21 | 0 | not archived | 0 |
| this branch (`orch/integration` base) | 159 | 117 | 21 | 21 | 47 / 25 / 22 | 1 (expected before the merge) |

## 6. Decision for Mariano

Migrate or retire the 5 NEEDS-DEV-ASI files (section 4)? Recommendation: retire `canon_observe_refused`, `probe_005ab100`, `mass_canonical_observe_wave3`
(same family as the 16 already retired), migrate `auto_count_at_menu` and `catch_crash_eip` to `dev_asi.spawn_original`. Follow-up step, not done here.
