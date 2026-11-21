# PREREG PROV-INSITU-REF1 (run 53, 2026-11-21)

Written and committed before the first body run. Race-path evidence row: the replay runs inside the original MASHED.exe,
binds nothing into the exe, no build, no G-CTRL, no B, no 12x4 captures.

## Scope
`sweep309.tsv` (from `t2_nonc4.tsv`, log/orch/audit_20261121, by `select309.py`): every T2 function with no C4 body and a
source (`best_pct` not None), classes game 196, RW-Physics 59, RW core 54 = 309.

## Scenario and reference
ref1 = `verify/ro_20261116/REFERENCE_RO-REF-NAME.json` (D:/MashedWT/RO-REF-NAME_data/ref1), control pair ref2. Arms use
the ref1 argv exactly (`run_arm.py`), plus `--prov` / `--prov-rvas` (bodies) or `--prov-count` (census). Private root
`D:\MashedWT\PROV-INSITU-REF1_root`, robocopy of `original\`; MASHED.exe `b9977dab...`, unpatched `bdcae093...`,
gamesave.bin `bd187881...`, d3d9.dll `47901951...`, mashed_re_dev.asi `906f8601...`, all equal to the ref1 root.

## Bodies
`re/tools/prov/provlink.py --rvas <309>`: XP DDK object of the source (match.py compile path, the file's FLAGS / LANG /
TOOLCHAIN), linked against the original image. UNRESOLVED -> NOT-WIRED. Install: inline `E9` at the RVA to a counting
stub then the body; the install check (`prov_install.json`, `jmp_ok`) per site.

## Compare and verdicts
`re/frida/prov_compare.py <run> ref1 ref2`: per-tick compare from tick 0 (boot), allowed blocks = blocks differing
between ref1 and ref2, warp_normalize masks.
- IDENTICAL-FOR-600: every compared tick equal, body installed (`jmp_ok`) and executed (calls > 0 in the run).
- DIVERGED@tick: first differing tick and blocks, confirmed by a second single-body run (same tick).
- CRASH: run ends without full ticks, confirmed by a second run; first fault address when `--crash-catch` gives one.
  Piz-collision signatures (`MASHED.exe+0x1509b4`, `+0xe66db`) are re-runs.
- NOT-EXECUTED: 0 calls in the census run (counters only) over the whole 600 ticks.
- NOT-WIRED: provlink UNRESOLVED.
Executed bodies are run in groups with `prov_bisect.py --any-segment` (a group that replays identically gives each member
IDENTICAL; a diverging group is split until each wrong body is alone).

## Controls (must pass before body verdicts count)
- c0: no body, IDENTICAL-FOR-N 600 of 600 against ref1/ref2, dn_warp 108, bootlen qpc 222.
- xneg: `--negctl 1` (RW RNG word +1 at the race anchor) must be DIVERGED in the race segment.

## Promotions (L-128)
`re/promotions/PROV-INSITU-REF1.tsv`, route `frida-replay`, only for IDENTICAL-FOR-600 bodies with: install check ok,
body executed in the race window or the run, no STUB marker in hooks.csv notes, and a write-set leg that is not
`not-covered` (static provlink store list empty or only absolute stores into the compared image, or a dynamic
`--prov-wlog` leg that observes only frame / compared-image stores). Checked with `c4_reclass.py batch --tsv` dry run only.
