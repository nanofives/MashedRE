# RESULT D4-WSB-SOLV-KV-ARMU: Arm U GREEN on 1800 cases, M4 site now covered, U-9589..U-9592 closed, rows stay C2

PREREG `PREREG_D4-WSB-SOLV-KV-ARMU.md` committed alone at `6d8c67783`, before the arm ran. Branch
`orch/D4-WSB-SOLV-KV-ARMU` off `orch/integration` @ `1611794e7`. No source edit, no game build, no `MASHED.exe` boot,
no original-side run, no Ghidra MCP. Anchor checked: `MASHED.exe.unpatched` SHA-256 `bdcae093...efd3c0e`.

## Arm U

`uc_arm.py` (copy of `verify/d4wsbsolvkv_20261029/uc_arm.py` with the three PREREG changes: the `0056adb0B` set, the
`fistp` probe at `0x0056b264`, `0056adb0B` in the site map). Port text: `RwpVtableKV12.cpp` at the base, verbatim.
Run under the test slot, 0 Unicorn errors over 1800 cases.

| variant | 0056a7a0 | 0056aae0 | 0056ac40 | 0056adb0 | 0056adb0A | 0056adb0B | read-out |
|---|---|---|---|---|---|---|---|
| port | 300/300 | 300/300 | 300/300 | 300/300 | 300/300 | 300/300 | **PASS, 1800/1800** |
| M1 (a7a0 add order) | 156/300 | 211/300 | 300/300 | 68/300 | 70/300 | 26/300 | detected (969) |
| M2 (ac40 sum in double) | 300/300 | 300/300 | 216/300 | 300/300 | 299/300 | 300/300 | detected (85) |
| M3 (aae0 update in double) | 300/300 | 259/300 | 300/300 | 120/300 | 134/300 | 71/300 | detected (616) |
| M4 (adb0 limit: round -> trunc) | 300/300 | 300/300 | 300/300 | 300/300 | 300/300 | 297/300 | **detected (3)** |

Cells are matching cases. The first 1500 cases are the KV step's (same seeds, same order) and give the same cells as its
`uc_summary.tsv`. **Verdict as preregistered: GREEN.**

### Coverage probe at `0x0056b264` (original side)

| label | `fistp` executions | M4-sensitive (round != trunc) | cases with one | operand range |
|---|---|---|---|---|
| `0056adb0` (P1_4 1..6) | 5 | 0 | 0 | 1.3e-05 .. 0.1396 |
| `0056adb0A` (P1_4 1..6) | 10 | 0 | 0 | 2.7e-04 .. 0.2874 |
| `0056adb0B` (P1_4 8..20) | 15 | 4 | 4 | 0.0461 .. 2.6947 |

The `P1_4 <= 7` bound of the KV RESULT holds: every operand in the first two sets is under 0.5. With `P1_4` 8..20 the
operand reaches 2.69.

### M4 diverging cases (all `0056adb0B`)

| case | P1_4 | W68 | first diff | `p1[6]` (pass count at exit) port / M4 |
|---|---|---|---|---|
| 1661 | 19 | 136 | arena+0x18 | 26 / 25 |
| 1723 | 16 | 92 | arena+0x18 | 21 / 20 |
| 1779 | 16 | 106 | arena+0x18 | 45 / 46 |

`arena+0x18` is `p1[6]`, the pass count `0x0056adb0` stores on exit (`0x0056b2fa` path / `0x0056b2d5` count). In these
three cases the rounded limit moves the next adaptive-stop check, so the run ends one pass earlier or later. The fourth
M4-sensitive case gives equal output under M4. [Per-case probe output was not kept, so which case that is and why
its changed limit did not move the exit is not recorded. The read-out does not need it.]

## Trackers (re-classify)

- U-9589 / U-9590 / U-9591 / U-9592 resolved with `re/tools/uncertainty_resolve.py` (the skill's documented
  operation). Reason: the esp depth at every listed site follows from the call frames read from the disassembly in
  `verify/d4wsbsolvkv_20261029/RESULT_D4-WSB-PORT-SOLV-KV.md` ("Frames": 69 words into `0x0056adb0`, `add esp,0x114`
  at `0x0056b645`; 15 into `0x0056ac40`, `add esp,0x3c` at `0x0056af84` / `0x0056b185`; 56 into `0x0056aae0`,
  `add esp,0xe0` at `0x0056b0e3`; 39 into `0x0056a7a0`, inside `add esp,0x9c` at `0x0056ac13`), and a port built on
  those frames matches the original's bytes on all 1800 Arm U cases with every mutant detected (this RESULT).
- `hooks.csv`: the 4 rows stay **C2 impl**, dated note citing this RESULT appended. No C-level move: Arm U is offline
  evidence, not a Frida diff (`re/CONFIDENCE.md`).
- No new U or S rows.

## Default gates

Not run: no source change and no game build in this step, so `mashed_re.exe` and the `.asi` are the base's. The merge
train's gate run covers the merged tree.

## Next

Original-side shadow A/B of the four `L2_` hooks (census scenario, one hook alone per boot, as in `D4-WSB-PORT-SOLV`),
needs the audio endpoint. CLEAN -> C3 through re-classify. `0x0056ac40` returns float10 on the original side
(`DIVERGENT_FLOAT10` limits of `shadow_ab_report.py`).

## Files

Committed: `PREREG_D4-WSB-SOLV-KV-ARMU.md`, this RESULT, `uc_arm.py`, `uc_harness_main.inc` (unchanged copy),
`uc_summary.tsv`, `uc_summary.json`.

Raw (not committed), `D:/MashedWT/D4-WSB-SOLV-KV-ARMU_data/`:

| file | SHA-256 |
|---|---|
| `uc/cases.bin` | `fac29aba2345c138312359500bb1c9619933ecc71783b49f459d13f0326a0d20` |
| `uc/out_port.bin` | `b5b77f3c9b4ae3e9bf6624c27374dfbfd8b20f70142f37988e19b4570971792d` |
| `uc/out_M1.bin` | `25e74050179e25e8cd2a372b04de9b894f59cd78edf6919b4ec268b06de38d12` |
| `uc/out_M2.bin` | `b2bf15a600df64661f2afc9e6375656848e3b363a1fcd344f8ca9f9500571adb` |
| `uc/out_M3.bin` | `0c8a010d7b3fff0ce25dc8ec5fab8f0a4eeac08c20af2a0d731a48c0d7479ee9` |
| `uc/out_M4.bin` | `d1b102c185c6d58aee77e6fd23b939aacae0871f3ce1350f6380d310636b87f4` |
| `uc_run.log` | `9b8047c5eaecd192795e769c5b4e84936943e58ab1059b01acc3d936c28ea015` |

Processes spawned: the five Unicorn harness exes (suspended-launched and waited on by `uc_arm.py`) and the `cl` builds.
No `MASHED.exe` or `mashed_re.exe` started. No watcher left running.
