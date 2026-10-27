# RESULT D4-WSB-C4-NARROW: RW-Physics NARROW band C3 -> C4

Verdict: **RED for 8 of 10, GREEN for 2.** The canonical-scenario run with the hook installed and
the body executing was done for all 10 C3 rows in `0x00570000-0x0057bfff`, one function per boot.
`0x00576640` and `0x00579b50` match the original bit for bit on every call and are promoted to C4.
The other 8 execute and return the same value as the original on every call, but on 5% to 53% of
calls they write different bytes: floats 1 to a few ULP apart for six of them, and a missing
integer-shaped word in the pair arena for `0x00575c60` / `0x00575fe0`. They stay C3: the C3 evidence (return-only
shadow A/B, 2026-09-10) still holds as stated, and C4 is refused.

Branch `orch/D4-WSB-C4-NARROW` off `orch/integration` @ `b0174c80f`. No exe-compiled source
changed (`exe_sources.rsp` and every TU in it untouched). The default gates are unchanged by
construction and were not run.

## Why a new probe (and why the C3 evidence missed this)

The C3 rows were promoted on `ShadowAB::Run` (`Core/ShadowAB.h`), which compares the **return
value only** and runs the original and then the port on the same live state with no rollback. All
8 RED rows write their results through pointer arguments, and those outputs were never compared.

`mashedmod/src/mashed_re/Collision/NarrowC4Probe.cpp` (.asi only, listed in `asi_sources.rsp`)
adds one opt-in hook per row, `L2_C4N_<rva>`. Per call in race phase (`0x00771968 >= 3`) it:

1. snapshots the function's write set (region list per row, from the port body and its ported
   callees, cited in the file),
2. uninstalls its inline JMP and calls the ORIGINAL (return r1, bytes O1),
3. restores, calls the ORIGINAL again (r2, O2): the **no-op control**, `noo` if (r1,O1) != (r2,O2),
4. restores, reinstalls, calls the PORT (rp, P). The game keeps the port's results,
5. counts `ok` / `diff` (return or any region byte) / `noo`, logs the first 20 diffs.

So every sampled call is a real execution of the port body behind an installed inline JMP on
canonical-scenario state, compared on its full output. `noo = 0` on every boot means the
snapshot covered everything the original's result depends on, including the writes of the
original shape-method slots the port calls into.

Run conditions: `verify/wsbnarrow_20261026/run_c4n.py`. Scenario = the census one (track 0,
mode 10, 4 slots, slot 0 parked, 45 s hold), `scenario_launch.py --hooks L2_C4N_<rva>`, every call
sampled (stride 1), `MASHED_ROOT=D:\MashedWT\D4-WSB-C4-NARROW_root` (robocopy of `original\`,
`MASHED.exe.unpatched` SHA-256 = anchor `BDCAE093...3C0E`), every boot inside
`mashed_lock.py run --label D4-WSB-C4-NARROW`. `MASHED_NO_SELFTEST=1` on every boot. With the
ShadowAB lane armed, `ShadowAB::Run` calls `HookSystem::Install` on nested port hooks that were
never installed, which would put more than one function live. Each boot's scenario_launch spawned
and killed its own PID.

## Per-row result (probe leg P, first run, .asi `17f4567b...f59d`)

| RVA | calls = samples | ok | diff | noo | ret diff | original wrote | verdict |
|---|---:|---:|---:|---:|---:|---:|---|
| `0x005757d0` | 4289 | 3507 | 782 | 0 | 0 | 4289 | RED, stays C3 |
| `0x00575880` | 4993 | 4648 | 345 | 0 | 0 | 4910 | RED, stays C3 |
| `0x00575c60` | 4545 | 3247 | 1298 | 0 | 0 | 4545 | RED, stays C3 |
| `0x00575fe0` | 2625 | 1226 | 1399 | 0 | 0 | 2625 | RED, stays C3 |
| `0x00576640` | 769 | 769 | 0 | 0 | 0 | 756 | **GREEN, C4** |
| `0x00577ec0` | 10689 | 8882 | 1807 | 0 | 0 | 10636 | RED, stays C3 |
| `0x005784a0` | 9153 | 8236 | 917 | 0 | 0 | 7436 | RED, stays C3 |
| `0x00578610` | 4545 | 4152 | 393 | 0 | 0 | 4475 | RED, stays C3 |
| `0x00578e50` | 8257 | 6660 | 1597 | 0 | 0 | 8113 | RED, stays C3 |
| `0x00579b50` | 129 | 129 | 0 | 0 | 0 | 0 (no writes) | **GREEN, C4** |

Counts are the last `SUM` line, written every 64 samples. A diff after the last SUM still writes a
`DIFF` line while fewer than 20 are logged. The two GREEN logs contain **0 `DIFF` and 0 `NOO`
lines**, so no sample of the whole boot differed. Every boot logged
`PATCHBYTE installed=e9 uninstalled=<prologue> reinstalled=e9 A/B-IS-REAL`.
`0x00575880` attempt 1 was VOID (`could not attach`), attempt 2 is the row above. These first-round
boots did not write a hook manifest. The selection was `MASHED_HOOK_ONLY=L2_C4N_<rva>`, an exact-token
allowlist (`Core/HookSystem.cpp:205-245`), and the second-round manifests below show it installs
exactly one hook.

## Second witness, own-hook leg, attribution re-runs (.asi `e42797f2...eb4f`)

Same probe plus a per-region first-diff histogram and `MASHED_HOOK_MANIFEST` per boot (the list
of hooks HookSystem installed). Every boot in this batch installed **exactly one** hook. Run in
a machine test slot (L0 rule 2026-10-26) and under `mashed_lock`.

| leg | RVA | result |
|---|---|---|
| H (own `FUN_00576640` installed alone, no A/B) | `0x00576640` | race reached and held, `installed=1 0x00576640:FUN_00576640` |
| P repeat | `0x00576640` | 1793 / 1793 ok, 0 diff, 0 noo, original wrote on 1740 |
| H (own `FUN_00579b50` installed alone, no A/B) | `0x00579b50` | race reached and held, `installed=1 0x00579B50:FUN_00579b50` |
| P repeat | `0x00579b50` | 129 / 129 ok, 0 diff, 0 noo |
| P | `0x005757d0` | 1271 ok, 74 diff, all in region 0 (the record) at `+0x00`, `+0x80`, `+0x84`, `+0x88` |
| P | `0x00575880` | 6907 ok, 2950 diff, all in region 1 (the record) at `+0x110`, `+0x88`, `+0x84`, `+0x80` |
| P | `0x00575c60` | 3108 ok, 1053 diff, all in region 1 (PA from the current count) at `+0x0c` |
| P | `0x00575fe0` | 356 ok, 541 diff in 897 samples, region 1 `+0x0c`. The game stopped logging 50 s into the boot (22:28:43) and was still alive 27 min later. I killed my own PID 33644 and the driver chain. The first `0x00575fe0` boot ran to the end (2625 samples), so this is recorded, not diagnosed |

`0x00578e50` was not re-run (the hang stopped the batch before it). It is a composite of
`0x00578610` + `0x005757d0` (`RwpSolverCore19.cpp:79-82`), both RED on their own.

## Where the RED rows differ

No return value differs on any row. Six rows differ in float bytes 1 to a few ULP apart (for
example `0x3f354f20` vs `0x3f354f21`). RwpSolverCore14/16/17/18/19/20 are already compiled x87
(`mashedmod/x87_tus.txt`), so this is the port's arithmetic, not the codegen mode. The two arena
rows differ in a different way: every logged first diff is the pair record at `+0x0c`, where the
original writes `0x000100b6`, `0x000100be`, `0x00020147`, `0x000301c7`, `0x000301d1` (as logged)
and the port leaves `0x00000000`.

| RVA | first differing output | own arithmetic |
|---|---|---|
| `0x00577ec0` | `*param_9` (15 of 20 logged), `*param_10` (5) | yes: dots in Ghidra print order, `RwpSolverCore14.cpp:155-187` |
| `0x005784a0` | appended element `+0x4`/`+0x8` (`param_2 + n*0x10`) | yes: `param_3[2]*param_4[2] + *param_3**param_4 + param_3[1]*param_4[1]`, `RwpSolverCore14.cpp:321` |
| `0x00578610` | `*param_6` (always written) and `*param_5` | yes, plus ported callees `0x005667c0`, `0x00578b20`, `0x00578bd0`, `0x00578cb0`, `0x00578d90`, `0x0055c2d0` |
| `0x005757d0` | record `+0x00..+0x08` and `+0x80..+0x88` (support headers) | no float math of its own: these are the outputs of the ported callee `FUN_0055c0f0` (`RwpSolverBroadphase3.cpp:652-666`) |
| `0x00578e50` | record outputs | composes `0x00578610` + `0x005757d0` |
| `0x00575880` | record `+0x110`, `+0x100`, `+0x10c`, `+0x80`, `+0x00` | composes `0x00578e50` / TOI path + `0x005757d0` |
| `0x00575c60`, `0x00575fe0` | pair record `PA[first]+0x0c`: original nonzero, port 0 | `[UNCERTAIN]` which write produces it. The port writes pair records at `RwpSolverCore17.cpp:481-486`, `:583-586`, `:679-685` and `RwpSolverCore18.cpp:588-597`, and the original clip slot `+0x44` is called with `W+0xc`. Not resolved here |

The three leaves with their own arithmetic (`0x00577ec0`, `0x005784a0`, `0x00578610`) carry
three-term dot products written as Ghidra prints them, `c*z + a*x + b*y`. C++ groups that
`(c*z + a*x) + b*y`. The 2026-10-21 dcphys finding (memory
`phys-selftest-quotas-sample-only-the-early-race`) showed the original adds the first two products
it loads, and fixed A6a/A5/A4 that way. This is the same transcription habit. The fix is to take
each sum's association from the FADD order in the disassembly. That is a behaviour change to the
default build (the exe compiles these TUs), so it needs a default-OFF knob, a PREREG and the
D2/G-CTRL gates. It is out of scope for this step and is proposed as a follow-up.
`[UNCERTAIN]` for the composite rows: whether their own bodies are exact once the leaves are
fixed is not measured here. Each composite's port tree calls ported callees, so its diff can
come entirely from a callee.

## Tracker changes (re-classify)

Through `re-classify`, one transaction on this branch:
- `hooks.csv`: `0x00576640`, `0x00579b50` C3/impl -> **C4/verified**, scenario
  `c4n_probe:track0-mode10-cars4`, frida_diff `c4n:verify/wsbnarrow_20261026/RESULT_D4-WSB-C4-NARROW.md`,
  notes appended. `file` and `exe_file` unchanged.
- `re/analysis/CHANGELOG.md`: three entries below `<!-- ENTRIES -->`, two promotions and one
  "C3 kept, C4 refused" line for the 8 RED rows.
- Plate frontmatter `confidence:` C1 -> C4 on `bucket_00565cd0/0x00576640.md` and `0x00579b50.md`
  (it was stale at C1).
- No new U- or S- rows. Still attached: U-9110 on `0x00576640` (naming only).
- The 8 RED rows are unchanged at C3. Their C3 claim (return-only shadow A/B) is still true as
  stated. Whether they should be demoted is a lead decision, not made here.

## Files

Committed (summaries):
- `mashedmod/src/mashed_re/Collision/NarrowC4Probe.cpp`, `mashedmod/asi_sources.rsp` (+1 line)
- `verify/wsbnarrow_20261026/run_c4n.py`: the driver
- `verify/wsbnarrow_20261026/c4n_summary.tsv`: per-boot SUM / PATCHBYTE / installed lines
- this file

Raw (not committed), `D:\MashedWT\D4-WSB-C4-NARROW_data\`:

| file | SHA-256 |
|---|---|
| `c4n_P_005757d0_a1.log` | `6d18c3a68291117afbca02edca71ceea0f516d39d5240017dd66f5271642b13c` |
| `c4n_P_00575880_a2.log` | `513fc17a7ca33bd664a599f2a3d4db72cc012b0cb2b296f7dcef1ad2862ab3bd` |
| `c4n_P_00575c60_a1.log` | `82f82dd5170e0281461da5e542f052089c353765e841e615211df97ab61cc920` |
| `c4n_P_00575fe0_a1.log` | `99e1a8d2d408e19a27401619971ae8b1964c97b436347dc28107a5f1c55df975` |
| `c4n_P_00576640_a1.log` | `cafb5bf1a5196162278934f8fd65fee77079d1e592c0b41685323be4db61dbe8` |
| `c4n_P_00577ec0_a1.log` | `e3a477edc6d7f3edfe93b08baf8dabd3b6ec4b7775dc779322b2c2c0eb5aa954` |
| `c4n_P_005784a0_a1.log` | `c3f29e688f4c548a49f33219592e4ad4251b061c5d98c344acabcc9ff2ebd70e` |
| `c4n_P_00578610_a1.log` | `edc2ea5eb82ffcf1ea67b00a94d768fd08c49403cc6d07d72f34f9ad430b17ed` |
| `c4n_P_00578e50_a1.log` | `e42912cc5a0fc2c299e12f2c36b32ea4e6576319c9288ff5eac2ba27df20149f` |
| `c4n_P_00579b50_a1.log` | `cf480e674e12111c0d47908ccc1106d61879e0e0fc6a7bb11043c013d29a2f04` |
| `c4n_P_005757d0_a1_r2.log` | `2547786f5547121cfcddad7399056f6af2acaa4d77e11e65508918a581feb159` |
| `c4n_P_00575880_a1_r2.log` | `8c60443fd00882a561d1b1d2ea8ea0337ea477f8be27c5997bb6495e44e594cc` |
| `c4n_P_00575c60_a1_r2.log` | `c07946afb8dda7bd60196978982f2a2df4785dc61718d4f1941bb79be432a19d` |
| `c4n_P_00575fe0_a1_r2_hung.log` | `9a26d4a8f555c807e6d8b66c367e3da4f17e2858edac7d661f94d9299775629b` |
| `c4n_P_00576640_a1_r2.log` | `9afc612a2a0b58d3f02f8caca24dc5800e34db7c18c9f624bd861cb92210df3d` |
| `c4n_P_00579b50_a1_r2.log` | `b7b8a3552cbb80d25876141125e46693907e7d549b8e553db20241839aaca1fd` |
| `manifest_H_00576640_a1_r2.tsv` | `5c78c3c8f84fddf9e1d1a0447002cadd591883c54f620b9a867f7c715f8c76b4` |
| `manifest_H_00579b50_a1_r2.tsv` | `ae30d82cd084e3584c36cb28a00cacab42dca2e5952f88e7516ca7c9ccc97761` |
| `manifest_P_00576640_a1_r2.tsv` | `8d7ebfa3a0a438b3caacf13f817443208a35d1a6363c63fb592bb535cf633874` |
| `manifest_P_00579b50_a1_r2.tsv` | `57ca5eea42af37b860deefb158046724bb234f9a9b5b43e1a0a8b7830a177a68` |

Launcher transcripts `launch_<leg>_<rva>_a<n>[_r2].txt` and build logs `build0..4.log` are in the
same folder.

## Follow-up proposed

`D4-WSB-NARROW-FIX` (new step, behaviour change, needs PREREG + default-OFF knob + D2/G-CTRL gates):
1. Re-associate the three-term sums in `0x00577ec0`, `0x005784a0`, `0x00578610` (and its ported
   callees) from the FADD order in the disassembly. Re-run this probe per row.
2. Find the missing pair-record `+0x0c` write in `0x00575c60` / `0x00575fe0`.
3. Check `FUN_0055c0f0` (BROAD band) for the same dot-order habit, since it produces
   `0x005757d0`'s diffs.
4. Then re-probe the composites (`0x005757d0`, `0x00578e50`, `0x00575880`). The probe and driver
   on this branch run unchanged: `run_c4n.py --legs P,H --rvas <rva>`.

## Limits

- The probe compares only the regions it snapshots. A port that writes an extra location the
  original never touches is not seen. `noo = 0` shows the snapshot is complete for what the
  original's result depends on, not that the port writes nothing else.
- One scenario (track 0, mode 10). `0x00579b50` ran 129 times in it. Its compare against the
  threshold uses a print-order dot (`RwpSolverCore16.cpp:251-253`), so an input on the threshold
  edge could still flip the result. No call in this scenario did.
- .asi only (the probe is not in the exe). For the two C4 rows the measured port body is
  `RwpSolverCore16.cpp`, which is the row's `exe_file` and is in both `asi_sources.rsp` and
  `exe_sources.rsp`: one TU, one body, so the evidence covers the body that ships
  (`re/CONFIDENCE.md`, "Which copy the evidence covers").
