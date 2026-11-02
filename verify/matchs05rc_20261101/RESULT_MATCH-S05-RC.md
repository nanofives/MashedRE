# RESULT MATCH-S05-RC (text-only re-classify leg)

**Verdict: GREEN.** 0x0041f710 `FUN_0041f710` C2 -> C4, route `match` (c4=match), through re-classify.
Branch `orch/MATCH-S05-RC` off `orch/integration` 6d9d01a7f (MATCH-S05 merged at 6e07aee91, log 87a4527d2,
MATCH-S07 merged after it). No source file changed: the diff is `hooks.csv` (one row) and one
`re/analysis/CHANGELOG.md` line below `<!-- ENTRIES -->`.

## Gate check (re-classify, "Evidence types match and libid")

| field | value |
|---|---|
| tool command | `MSYS_NO_PATHCONV=1 py -3.12 re/tools/match/match.py re/match/s05/0041f710.c --bytes` |
| result (re-run at this head) | `0x0041f710 _FUN_0041f710: 100.0% (regs 100.0%, ops 100.0%) 22/22 ins flags=/O2 /GS /Ob1 tc=ddk`, 88 B whole symbol, 0 differing unmasked bytes, BYTE-IDENTICAL |
| relocheck | `0x0041f710 OK checked=1 unchecked=0` |
| matched source + sha | `re/match/s05/0041f710.c`, sha256 (committed blob, LF) `226667aa4d05a1de321387e0121574472e14a0deeb1a95ea7b3964dbac7e2532` (the CRLF checkout hashes `4297c77a...`) |
| port body | `mashedmod/src/mashed_re/Match/MatchS05_0041f710.cpp` includes the matched source (`#define FUN_0041f710 CR_41f710Body`). Exe only, the only body. `CR_41f710` is a 4-instruction naked EAX -> cdecl adapter with no RVA comment |
| stubs in body | none. Callee `FUN_00484cf0` is `RG_484cf0` (verbatim naked, `Race/WorldRegistry_naked.inc`). U-8047 (its identity, Blocks=none) stays attached |
| gates after the swap | MATCH-S05 merged build `c8c2220a`: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3, equal to `merge_train.json` refs. The train re-ran them on merge (no change) |

Raw match output: `match_rerun.txt` (this folder).

## Exe caller on the merged head (L-68)

Built this head (`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, Build OK, .asi not deployed): exe sha256
`9861ec9fc9571ccab3758b74e3c78b725a9b3622217cefab10f3ca61b4b4863f`, asi
`b591f134f048c4033b1bbd2f4b9793169a2a61c309f96080234e85c81763f973`.
`py -3.12 verify/matchs05_20261101/tools/exe_callsites.py mashed_re.exe mashed_re.map ...` (`exe_callsites.txt`):

| symbol | exe call sites |
|---|---|
| `_CR_41f710Body` | 1 (`_CR_41f710+0x1`) |
| `_CR_41f710` | 1 (`Race::CarsReg_FrameTick+0x67`) |
| `Race::CarsReg_FrameTick` | 2 (`TrackRenderer::UpdateCar+0x73a`, `+0x3065`) |

Source chain: `D3d9Render/TrackRenderer.cpp:270` `PreGoRegistryTick` calls `Race::CarsReg_FrameTick()` with no knob
(the pre-GO registry chain always runs since D5-PREGO-CUBE-FLIP), and `TrackRenderer.cpp:6101` calls it when
`gates_` is non-empty. exe-callers=1 holds.

## Not moved (notes only, L-68)

The other 58 S05 swaps keep their level. Each `hooks.csv` row gets a `2026-11-01 MATCH-S05-RC: match evidence: ...`
note (the text RESULT_MATCH-S05.md prepared), from a re-run at this head (`match_rerun_all.txt`: 54 sources, all 59
RVAs BYTE-IDENTICAL over the whole symbol, all relocheck OK):

- 0x00423270, 0x00423320: `exe-callers=1 from FrontendInputDispatch (0x00423b00), which has exe-callers=0; C4 when a
  live caller lands (L-68 ruling)`.
- the other 27 exe+asi swaps and the 29 asi-only swaps: `exe-callers=0; C4 when a caller lands (L-68 ruling)`.
- 0x00421080 and 0x00423480 are scored by bytes (insn score 80.0% / 96.8%, whole symbol BYTE-IDENTICAL), as in
  `swaps.tsv` column `by`. Their notes say so.
- relocheck lists unchecked CRT names on three rows, written into their notes: 0x00422470 (`__ftol,_floor`),
  0x00423480 and 0x00425c00 (`@__security_check_cookie@4,___security_cookie`).

hooks.csv diff: 59 rows (1 level move + 58 notes), no other row touched. CHANGELOG: one line (the C2->C4 move).
