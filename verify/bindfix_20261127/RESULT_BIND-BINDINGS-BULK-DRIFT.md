0 BINDINGS.tsv rows disagree with `bindings --profile bulk` (4 at the start), selftest checks the bulk BINDINGS output, PASS 0 every profile

# RESULT BIND-BINDINGS-BULK-DRIFT (run 57, binder tool row, base orch/integration c880ab12e)

## Measured numbers

| | start (c880ab12e) | this branch |
|---|---|---|
| `bindings --profile bulk --dry` | updated 4 added 429 | **updated 0 added 0** (after the write) |
| `bindings --profile audio --dry` | updated 0 added 0 | updated 0 added 0 |
| selftest BINDINGS.tsv checks | audio only | **audio IDENTICAL (0/0), bulk IDENTICAL (0/0)** |
| binder selftest | PASS | **PASS** (every profile: raceload, audio, bulk, raceframe, renderframe, gates) |
| unit test `test_porttop_bind_lib.py` | 11 cases | **12/12 PASS** (new case `bindings_foreign_stop`) |

## Owning profile of the 4 rows (decision)

The row that wrote a named stop first owns it. Bulk lists all 4 as plain stops (plan.json `stop`) and has its own stop bodies for
them (Match/PortTop_BulkStops.cpp:30, :726, :744, :772), but it did not write these rows:

| rva | owner | row text kept |
|---|---|---|
| 0x0057c210 | **audio** (PORT-TOP-AUDIOTICK-STOPS) | missing PortTop_AStop_0057c210, level 6, caller 0x0047d150 |
| 0x00401da0 | **raceframe** (PORT-TOP-06) | missing PortTop_RFStop_00401da0, level 4, caller 0x00401570 |
| 0x00535700 | **raceframe** (PORT-TOP-06) | missing PortTop_RFStop_00535700, level 5, caller 0x00476df0 |
| 0x0055dff0 | **raceframe** (PORT-TOP-06) | missing PortTop_RFStop_0055dff0, level 5, caller 0x0047d180 |

0x0057c210 belongs to the audio profile. **No audio state changed** (re/tools/bind/state/audio/ untouched, no state file of any
profile touched); the audio row is kept byte-identical, so nothing goes through REACH-6. The 4 rows are byte-identical to the base
(`git diff` shows 0 `-`/`+` lines for them).

## Change

- `re/tools/bind/porttop_bind.py`:
  - `cmd_bindings`: a `stop_sym_foreign` profile (only bulk sets it, state/bulk/profile.json:21) keeps a `missing` row whose
    exe_symbol carries another profile's `stop_prefix` (new `foreign_stop_owner`). Rows no profile prefix names
    (e.g. `PortTop_Missing_*`) are still relabelled. Profiles without `stop_sym_foreign` behave as before.
  - selftest: `SELFTEST_BINDINGS_TOOL = ('bulk',)` adds bulk to the BINDINGS.tsv rebuild-and-compare check (audio keeps its
    profile.json `selftest_bindings`). Kept in the tool so the row changes no profile state.
- `re/tools/bind/test_porttop_bind_lib.py`: case `bindings_foreign_stop` (bulk keeps a `PortTop_AStop_` row, relabels a
  `PortTop_Missing_` row, `foreign_stop_owner` answers audio / None).
- `verify/porttop_20261108/BINDINGS.tsv`: regenerated with `bindings --profile bulk` (tool, not by hand): +429 rows
  (244 matched, 185 missing), 0 rows changed. 1790 -> 2219 lines. The 429 are bulk's own plan entries no row had written yet;
  the selftest's bulk check cannot read IDENTICAL without them.
- `re/orchestrator/UNCERTAIN_REGISTER.md`: +1 line (the 429 adds change no build: a code-reading claim).

## Checks run

| check | result |
|---|---|
| `mashed_lock.py status` before the selftest | free |
| `test-slot --status` | 8 slots free on LIVING-PC; C: 82 GB free |
| `bindings --profile <p> --dry` all 6 profiles, before and after | table above and "Not done" below |
| `py -3.12 re/tools/bind/test_porttop_bind_lib.py` | 12/12 PASS |
| `py -3.12 re/tools/bind/porttop_bind.py selftest` | PASS, log `BIND-BINDINGS-BULK-DRIFT_selftest.txt` |
| not run | builds, ddkobj check, G-CTRL, B, captures, game arms (tool row, text kind) |
| frida-<hash> folders in %LOCALAPPDATA%\Temp | 0 before, 0 after (no harness run) |

## [UNCERTAIN]

- The 429 added rows change no build: by reading, ddkobj.py uses BINDINGS.tsv only for kind `provisional` (ddkobj.py:505) and
  under `--matched-only` (ddkobj.py:576, not passed by build.bat), and bulk writes no `provisional` row. Test: a ddkobj check of
  this branch against the integration head. Register line added.

## Not done (other profiles' drift, not owned here)

`bindings --dry` on this branch still rewrites rows for profiles outside this row:
- raceframe: updated 4 added 14 (its own RFStop rows 0x004103a0, 0x00420870, 0x00448220, 0x0046b540; 18 adds before the bulk write).
- raceload: updated 14 added 282 (relabels audio / raceframe stops, e.g. 0x004a34b0 PortTop_AStop_, 0x0057c210, 0x0055dff0, and
  0x00495780 PortTop_Missing_; 12 / 349 at the start). raceload has no `stop_sym_foreign` or `bindings_keep_missing`, so the new
  rule does not cover it.
- gates: updated 2 added 9 (0x004103a0, 0x00448220 raceframe stops turned into gates rows).
- renderframe, audio: 0 / 0.
None of these is selftest-checked. A follow-up tool row can extend the selftest to them once each profile's owner decides.
