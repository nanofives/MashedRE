# RESULT MATCH-T01G-FU, 2026-11-02

**Verdict: GREEN (partial match, gates unchanged).** Of the four call sites the T01 / T02 / T09 / T12 swaps need,
two are byte-identical and swapped in as port bodies: Race::Tick **0x0040fc00** and the race frame **0x004111c0**
(plus the held static 0x004102f0). **0x0040e590** (84.6%) and **0x00416250** (88.9%) are not matched within the
budget (class b). The 0x00409a80 "installer" turned out to be a static call chain through five functions Ghidra never
created; nothing in it has a source yet, so 0x00409a80 is not swapped.

Gates on the swapped build (exe sha256/16 `07715e94e17ad53a`): G-CTRL `8284be46/ba407a16/31fe043e`, B
`1d5bf4f9/1d5bf4f9/1d5bf4f9` = `re/orchestrator/merge_train.json` refs (**unchanged**). All four arms self-exited.

## Match (static leg)

Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87 (`re/tools/match/match.py`).

| rva | size | file | result | relocheck | C++ |
|---|---|---|---|---|---|
| 0x0040fc00 | 621 B (684 B symbol) | `re/match/t01g/004111c0.c` | 100.0% 158/158, whole symbol BYTE-IDENTICAL (jump + index tables) | OK 75 checked, unchecked `__ftol` | ok |
| 0x004111c0 | 400 B | `re/match/t01g/004111c0.c` | whole symbol BYTE-IDENTICAL (match.py prints 84.7% by instruction because it disassembles the 11-entry jump table at 0x00411324 as code) | OK 37/0 | ok |
| 0x004102f0 | 172 B | `re/match/t01g/004111c0.c` (held static, EBX arg) | 100.0%, BYTE-IDENTICAL (the S02 source) | OK 14/0 | ok |
| 0x004103a0 | 360 B | same file, `// HELD:` (EAX arg; body elsewhere) | not compared (HELD); 100% in `re/match/s02/004103a0.c` | n/a | ok |
| 0x0040e590 | 2360 B | `re/match/t01g/0040e590.c` | **84.6%** (regs 89.1, ops 99.0), 786 vs 783 ins; raw g2s 22.0% | n/a (<100%) | ok |
| 0x00416250 | 2015 B | `re/match/t01g/00416250.c` | **88.9%** (regs 97.7, ops 98.5), 605/605 ins; previous best 58.2% (`re/match/s03/h4_00416250.c`) | BAD only at sites the residual shifts | ok |

Source sha256/16: `004111c0.c` 0b76ac553380b797, `0040e590.c` 4222faf5b7fa1c13, `00416250.c` b68a678bf568ec7e.

What closed 0x0040fc00 / 0x004111c0 (helper h1): 0x0040fc00 is `static void FUN_0040fc00(uint delta)` with the
argument in ESI (pilot finding 1, defined before 0x004111c0 in the same file); unsigned (fild + 2^32 fix-up); the
1/3000 factor is the global float at 0x005cc948 (a literal let the compiler reuse the first product); `(float)delta`
written at each use (named local 82.4%); one tail statement Ghidra dropped (`DAT_007f1030 += (float)delta * 1000.0f`);
outer switch order 2, 6/10, 3/4/5, 11 -> default, inner case 5 before 4/7/8/9 (only the jump-table bytes showed it).
0x004111c0: first block is if / else-if on the 5 / 6 result of 0x0055dec0; `DAT_007f0fc8` is a float stored as 0.0f.

Residuals (both class b, budget spent, permuter run):
- **0x0040e590** (h2): (1) the last loop strength-reduces `i*4+0x34` in every natural form; `(short)i` is the only
  form found that keeps the original's addressing (one extra movsx; not the real source); (2) case 2 of the first
  switch tests -1 before 1; (3) register colouring in the mode-4 post-pass. A lead permuter pass on the final file
  (1365 compiles) reached 87.3%, but **both of its gains change behaviour** (it moved `FUN_0046b540(i)` before
  `FUN_0041f000`, and its `mulswap` turned `DAT_0063b9b8[a*4 + k]` into `4 * DAT_0063b9b8[a + k]`), so it was
  rejected and the 84.6% file kept.
- **0x00416250** (h3): `volatile float steer` is the only form found that gives the original's memory home for the
  steer value, and volatile loads it first at 3 sites per history block (`fld steer; fcomp b` vs the original's
  `fld b; fcomp [steer]`); EBX/EDI swap (out vs car*0x74). Found on the way: `AiCar74.state` is at +0x68
  (0x0089a52c), not +0x64 as in the s03 struct (relocheck caught it). The same tail fixes (block-scoped `cand[2]`,
  explicit float casts on sub-products, `volatile int DAT_007f0ff4`) likely apply to siblings 0x00416a30 / 0x00417da0.

## Swap (port leg)

`mashedmod/src/mashed_re/Match/MatchT01G_004111c0.cpp` compiles `re/match/t01g/004111c0.c` as C++ (MATCH_PORT) in
**both targets**, `install=0` (no old body, no old install; installing a new hook would change the .asi default).

| rva | port symbol | targets | install | note |
|---|---|---|---|---|
| 0x004111c0 | `FUN_004111c0` | exe + asi | 0 | new body, no hooks.csv name |
| 0x0040fc00 | `FUN_0040fc00` | exe + asi | 0 | new body, `MATCH_STATIC` (extern in C++) |
| 0x004102f0 | `FUN_004102f0` | exe + asi | 0 | new body, `MATCH_STATIC` |
| 0x004103a0 | HELD -> `SubStateHeldCountdown5_C` | | | existing port body (Race/SubStateMachine.cpp) |

Binds: 21 to port bodies present in both targets (`SubStateEnter4`, `GetRenderSubMode`, `HudSlotUpdateCc50`,
`HudSlideBillboardTick`, `HudDrawEnabled`, `UtilWritePtr`, `FUN_004292d0`, `GetDat0067ea64`, `GetRaceSubMode`,
`VehicleSeedWritePair`, `FollowTargetField10`, `VehiclePhysicsStateReset`, `VehicleSlotGetter`, `UtilDeref55dec0`,
`SubStateHeldCountdown5_C`, and six used only by the HELD copy); 55 raw-address forwarders
`MatchT01G_004111c0_Call<va8>` for callees with no body in both targets, **including 0x0040e590** (rebind when it
matches). Callee body map: `D:/MashedWT/MATCH-T01G-FU_data/bodies_004111c0.tsv`.

- `mashedmod/exe_sources.rsp`, `asi_sources.rsp`: + `Match\MatchT01G_004111c0.cpp` (T01G block after
  `Race\CameraEntries.cpp`). `mashedmod/x87_tus.txt`: + `MatchT01G_004111c0` (own header line).
- `py -3.12 scripts/lint_rva_bodies.py`: `2 finding(s) ... allowlisted=2 NEW=0`. `dual_copy_allowlist.txt` unchanged.
- **exe-callers = 0 for all three new bodies.** The port's race frame is RVA-less code (TrackRenderer.cpp /
  AiStandalone.cpp); nothing in the exe calls `FUN_004111c0`, and `FUN_0040fc00` / `FUN_004102f0` are called only from
  it. So gate (3) shows only that the swap changes nothing the default build runs (L-68).
- Build: `MASHED_NO_DEPLOY=1 mashedmod\build.bat` exit 0, `=== Build OK ===`.

## Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, A4_ROOT / MASHED_ROOT
`D:/MashedWT/MATCH-T01G-FU_root` (robocopy of `original/`), A4_EXE = build copied out first, wrapped in
`mashed_lock.py run --label MATCH-T01G-FU` (queued behind the merge train's solo window).

| arm | result | vs ref |
|---|---|---|
| H_DEF | self-exited; step `8284be46`, gates `ba407a16`, ptrace `31fe043e` | **equal** |
| B_DEF_K0 / K1 / K26 | self-exited, 762 lines each, `1d5bf4f9` x3 | **equal** |

The two unmatched sources (`0040e590.c`, `00416250.c`) are committed as work in progress and are **not** compiled
into either target, so the build the gates ran on is the branch's build.

## 0x00409a80 (T06): the installer is a static chain through Ghidra-missed functions

T06 recorded 0x00409a80 as "no static caller, installer indirect". A rel32 scan of `.text` finds 10 direct sites
(`call` + tail `jmp` in each of 0x00409b00, 0x00409e90, 0x0040a130, 0x0040a3a0, 0x0040a650: calls at 0x409cfc,
0x40a0af, 0x40a339, 0x40a537, 0x40a8ef; jmps at 0x409e3b, 0x40a0ea, 0x40a363, 0x40a578, 0x40a93e). None of the five
has a hooks.csv row or a decomp. Their only entries are the tail jumps of the switch at **0x0040acd0**
(`mov eax,[0x008a9584]; dec eax; cmp eax,5; ja; jmp [eax*4+0x0040ad04]`, cases 0x409b00 / 0x40a130 / 0x40a650 /
0x40a3a0 / 0x409e90 / 0x409ad0), called once at 0x0043e069 (inside 0x0043dfd0, port `RaceRuleFromEvent`). Sibling
switches on the same state: 0x0040ab40 (-> 0x40aa60 / 0x40aa30 / 0x40a9a0 / 0x40aad0) and 0x0040ac80.
Chain: 0x0043dfd0 -> 0x0040acd0 -> {five case functions, ~3.7 KB} -> 0x00409a80. No source exists for any of the
five, so nothing here matches yet. Detail: `D:/MashedWT/MATCH-T01G-FU_data/notes/lead_00409a80.md`.

## Re-classify

None to C4: the three new bodies have exe-callers = 0 (L-68: matched and swapped, pending a caller). Proposed
`notes` text for 0x0040fc00, 0x004111c0, 0x004102f0 when the lead files it through re-classify:
`match evidence: identical (re/match/t01g/004111c0.c), swapped (Match/MatchT01G_004111c0.cpp), exe-callers=0; C4 when a caller lands (L-68 ruling)`.

## Next

1. Wire the exe race frame to `FUN_004111c0` (the T01 title's expected gate move: default-OFF knob + PREREG, then a
   decided flip + L-n).
2. 0x0040e590 and 0x00416250: a second hand pass (the residuals above), then rebind `FUN_0040e590` in the wrapper.
3. T06: g2s + match the five case functions 0x00409b00..0x0040a650 and 0x0040acd0 (Ghidra must create them first;
   `re/tools/decomp_pc.py --create` reads missed functions, memory `transient-create-reads-ghidra-missed-functions`).

## Raw data (`D:/MashedWT/MATCH-T01G-FU_data`, not committed)

```
07715e94e17ad53af9988af6626485b6b15732e158d9242c3bc4966d34cd14e1  exe_g1/mashed_re.exe
0ad1825b91623b611fb755770e8b0a45da2cd066f97f7b59d665a3921d5ab28f  build1.log
d4c6773fce1241f1a04a3fd8ac9a382d6b3483ed0425ce9e70b45d89e055ed54  gates_g1.log
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log
eb59b3eb09cf8fd0fd88c82f87fefe4983f3c1fceff6fb4d57382355f45bc616  arms_g1/H_DEF.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K0.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K1.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K26.step.csv
219d14f8f44de7fbfe02ea79f4f1ab66b689c5109670b3f070752c6ed53ef976  arms_g1/B_DEF_K0.log
4ef5c0ea6c99cbd017767f1f4c38a07d2a6cbc1a5e7841288840762e6f328277  arms_g1/B_DEF_K1.log
bf5190040b4afbcddc9c40eeba96ea238645c3add7c3de5c494586339991bfb8  arms_g1/B_DEF_K26.log
231460008f418ae3f3217f20d36391dd51cdcf44c1830312121c4e384bba3f69  decomp.json
14a8e6e513a0add66dde2d95ac89583a5c59f347f66068dc5bb8726ce39412a7  notes/h1.md
65e84fa9167c5be88cebd5aa95eaa938783dfd47ae656b3778f9694a4ca7dfc4  notes/h2.md
08d78d27c43f01ba6f0319cea14c10eda7c0e302bdf971556ddde75e1c6ca169  notes/h3.md
4b580abdcd03c36af020018f7f7641b2a2835d89924277b40f2bff935a097fea  notes/w1.md
1621ca1ccae32338052f7256ced49e6f6c4b49a69ffa69a3c79739b0e656f92f  notes/lead_00409a80.md
51f25b0089e5dfb94dcadfabb457bfd05bcaf02009728cc49252c7de39052de6  scratch/0040e590_perm.c  (rejected permuter variant)
```
