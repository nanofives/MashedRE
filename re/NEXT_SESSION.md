# Next session — kickoff prompt

## ⇒ CURRENT STATE (2026-09-10 session close) — READ THIS FIRST

Branch `race/first-frame-parity`. Previous close: `d9d7682a` (2026-09-09). This session built the
**shadow A/B mass lane** and ran its first sweep. Full write-up: `re/analysis/shadow_lane_20260910.md`.

### What was built (`re/tools/`)

| tool | one line |
|---|---|
| `shadow_gen.py` | installed C2 port -> `Name_impl` + `ShadowAB::Run`/`RunRegion` wrapper, mechanically; manifest `re/parity/shadow_sites.tsv` |
| `shadow_batch.py` | boots site groups via `scenario_launch.py --hooks`, retries harness VOIDs, bisects CRASHes to one site; `re/parity/shadow_results.tsv`; raw logs `log/shadow_ab/` |
| `shadow_ab_report.py` | `shadow_ab.log` -> CLEAN / DIVERGENT / DIVERGENT_FLOAT10 / UNPROVEN / SKIPPED / NO_SAMPLES |

### What it produced

- **94 sites generated** (79 return-value + 15 verified region) from the 153 installed C2 ports.
  23 boots. **44 rows C2->C3** (gate 32 PASS / 12 LEAF / 0 FAIL, `log/shadow_ab/c3_gate_check.tsv`).
- **10 DIVERGENT, 0 established defects**: 7 proven non-idempotent (`rand()`, contact counters,
  read-then-written axis fields — worker reviews at file:line), 2 unresolved aliasing cases
  (`0x00577be0`, `0x00577cb0`), 1 real candidate on the region lane (`0x0055b750`, 13/48 on the
  output vec3, RunRegion already restores the region).
- **5 DIVERGENT_FLOAT10**: x87 80-bit return vs MSVC 64-bit long double — lane limit.
- **1 CRASH**: `0x0056bce0` alone crashes the game at race start with the A/B armed (unarmed = default
  `.asi`, boots fine). Cause not established.
- **29 NO_SAMPLES** across both kinds even with `--cars 4 --hold 60`.
- Two region CLEAN rows (`0x00546c50`, `0x00565200`) plus three from run 5 (`0x0056cf90`,
  `0x0056ed60`, `0x0056fad0`) are **CLEAN but not yet gate-checked or promoted**.

Corrections this session made to its own first readings, so they are not re-quoted: the run-2
"CRASH" pair `0x00574ad0`+`0x00575120` did not reproduce in run 3; the first "14 SUSPECT" rows
were callee wrappers reached through our own ported callers (proof shape `hook-not-installed`,
still a real A/B); one worker region spec (`0x0056c8e0`) was wrong and was caught by the
mechanical LHS verifier (`log/shadow_ab/region_verify.txt`).

---

## PICK ONE

### A. DONE — the 5 region-lane rows are promoted (see CHANGELOG 2026-09-10 REGION LANE)

### B. `0x0056bce0` crash **[Ghidra + one boot]**
Disassemble the original's prologue/argument use: if `param_3` (float) is not a plain cdecl stack
arg (x87 args are invisible to Ghidra — memory `feedback_zero_arg_argtype_false_green`), the shadow
call through `void(__cdecl*)(float*,float*,float)` is the crash AND the installed port is ABI-wrong
(memory `feedback_installed_hook_abi_mismatch`). Repro: `py -3.12 re/tools/shadow_batch.py --rvas
0056bce0 --group 1 --max-boots 1 --launch-arg=--cars --launch-arg=4`.

### C. Settle the two aliasing rows + `0x0055b750` **[RunRegion re-test]**
Regenerate `0x00577be0`/`0x00577cb0` as `RunRegion` over their output buffer (worker review 2
names it), re-boot, and read `0x0055b750`'s body against the disasm for a precision/transcription
difference. Each is C3 on a clean re-run, or the first real defect this lane has found.

### D. Reach the 29 NO_SAMPLES **[scenario work]**
They are contact/broadphase paths a 4-car standing race does not hit. Try `--boost`, `--mode 2`,
a track with walls, or drive input (`--statediff-drive`). `MASHED_COUNT_RVAS` first — it costs nothing.

### E. Next slice of void ports **[RunRegion with two spans]**
12 MULTI_REGION rows in `log/shadow_ab/worker_void_regions.txt` need a `RunRegion2`; 18 INDIRECT
and 7 GLOBAL_WRITES need a snapshot of the pointed-to/global state instead. Verify every span with
the LHS scanner before applying — the worker's specs are claims.

### G. C3->C4 through the shadow lane **[DECISION first: rubric L37 wording]**
`re/analysis/promotion_lanes_assessment_20260910.md`: 387 C3 rows are shadow-generatable today
(315 outside audio), 293 more need a region. `re/CONFIDENCE.md` L37 names a Frida CSV as the C4
evidence; the shadow report is a stronger canonical-scenario diff but not a Frida CSV. Amend, or
keep C4 Frida-only. If amended: `shadow_gen.py --sweep re/parity/matchdiff_sweep_c3.csv --apply`
(exclude audio; frontend/hud with `--phase any`), then `shadow_batch.py --cars 4 --hold 60`.

### H. Lane 3 — pool closed; open defects and the private-memory mode **[investigation]**
55 converted void C2 ports: 30 CLEAN (all promoted, C3 = 1008), 13 unreached, 3 CRASH, 2
DIVERGENT, 7 unbooted (`re/analysis/lane3_write_tracking_20260910.md`). Open: (1) private-memory
tracking hangs on the first sample — the tracked thread waits on a critical section that is not an
NT heap lock (CRT `_HEAP_LOCK` or Frida's interceptor lock are the candidates); the hang watchdog
(`MASHED_SHADOW_TRACE=1`) logs EIP/stack; next step is resolving the CS address on the hung stack
and its owner thread. (2) `0x0047e9c0` reproducible first-call page diff at `.data+0x624048` = a
real candidate defect in the K24 root port. (3) crash bisect of `0x0056f350 0x0056fea0 0x00570090`
(`shadow_batch.py --rvas ... --group 1 --launch-arg=--cars --launch-arg=4`).

### I. Lane 2 transcriber — grow the accept set **[tooling]**
`re/tools/decomp2port.py` + `INDIRECT_IDIOMS` table (design: `re/analysis/lane2_decomp2port_design_20260910.md`).
29/53 reachable unported C2 rows compile; the 7 idiom-recovered ones are **promoted C3** (first
decompiler-generated ports verified effect-identical). 15 indirect-call refusals remain: next table
entries are `*DAT_007d4110+off` (4 rows) and `*DAT_007d4108+0x28` — add only with a hand port that
pins the convention. 3 generated ports crash alone (`0x00421960 0x004219c0 0x00495fe0`): open hazard
class (register args the callee prototypes miss). Re-run: `decomp_pc.py --file rvas.txt --callees
--port --json -o d.json` → `decomp2port.py d.json --emit-dir Lane2 --apply --report r.tsv` →
`build.bat` → `decomp2port.py --prune-failed log/build_lane2b.txt --report r.tsv`.

### F. Carried over from 2026-09-09, untouched
U-9087 E9-thunk guard decision (10 hooks never install, 4 C4); D-11069 (4 duplicate RVAs in two
targets); `TransformMatrixUpdate` pos.x/pos.z defect (D-10793); desktop verification of the
playtest commits; `main` 270+ commits behind; 6 orphaned pool locks (`Mashed_pool{0,1,10,11,12,13}`).

---

## Ready-to-paste kickoff

> Resume the Mashed RE lane on `race/first-frame-parity`. Read `re/NEXT_SESSION.md`, then pick ONE
> of A–I (G needs your rubric decision first). The shadow A/B mass lane is live (`re/tools/shadow_gen.py`, `shadow_batch.py`,
> `shadow_ab_report.py`; write-up `re/analysis/shadow_lane_20260910.md`). Standing rules that bit
> this session: run with `--cars 4 --hold 60` (a 1-car race never fires the contact solver); a
> `Run()` DIVERGENT is not a defect until the body has been read for state it also writes; float10
> returns cannot be compared bit-exactly through MSVC `long double`; verify every worker-supplied
> region spec against the body's assignment LHSs before applying it.
