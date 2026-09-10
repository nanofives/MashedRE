# Next session — kickoff prompt

## ⇒ CURRENT STATE (2026-09-10 session close) — READ THIS FIRST

Branch `race/first-frame-parity`, tree clean, no stray processes. Trackers: hooks.csv 5,930 rows
(C4 184, **C3 1,008** (was 922), C2 3,886, C1 821) · DEFERRED 677 · UNCERTAINTIES 3,082.
**86 rows moved C2→C3 today**, none resting on a synthetic Frida call. Full write-ups:
`re/analysis/shadow_lane_20260910.md` (Run/Region lanes), `lane2_decomp2port_design_20260910.md`
(transcriber), `lane3_write_tracking_20260910.md` (write tracking), `promotion_lanes_assessment_20260910.md`
(where the remaining volume is).

### Built today (`re/tools/`, `Core/`)
| tool | one line |
|---|---|
| `shadow_gen.py` | installed port → shadow site (`Run` / `--region` / `--tracked`); manifest `re/parity/shadow_sites.tsv` |
| `shadow_batch.py` | boots site groups, retries harness VOIDs, bisects CRASH/HUNG to one site, kills only its own PIDs; `--no-shadow` control |
| `shadow_ab_report.py` | log → CLEAN / DIVERGENT / DIVERGENT_FLOAT10 / UNPROVEN / SKIPPED / NO_SAMPLES |
| `decomp2port.py` + `DecompPC.java --port` | Ghidra decomp → verbatim C++ TU per function, `L2_` opt-in hooks, `INDIRECT_IDIOMS` table, `--prune-failed` |
| `Core/ShadowTrack.h` | `RunTracked`: page-level write tracking + caller stack window; `MASHED_SHADOW_TRACE=1` breadcrumbs + hang watchdog |

### Promotions today
44 (Run lane, 48/48) + 30 (tracked, 24/24) + 5 (region, 48/48) + **7 decompiler-generated ports**
(tracked 24/24) = 86. Every row cites its gate file under `log/shadow_ab/c3_gate_check*.tsv`.

### Open items (all saved; none blocking)
- **Private-memory tracking hangs** on its first sample: the tracked thread waits on a critical
  section that is not an NT heap lock (CRT `_HEAP_LOCK` or Frida interceptor lock). Watchdog log
  shows `EIP 0x77a8b9dc`, stack `77a9ff5c 77b49a54 … 77b4d3c8`. Next: read the CS address from
  the `RtlpWaitOnCriticalSection` frame and its owner thread id. (H)
- **`0x0047e9c0` first-call write at `.data+0x624048`** the port omits — reproducible 3 boots,
  candidate real defect in the K24 root port. (H)
- **Crashes to bisect**: tracked `0x0056f350 0x0056fea0 0x00570090` (group), `0x0055bd80` (at
  load), `0x00560260` (after 24 clean samples, heap effects unrestored), `0x0056f0a0`; Lane 2
  generated `0x00421960 0x004219c0 0x00495fe0`; RunRegion `0x0056bce0`. (B/H/I)
- **Precision-class divergences**: `0x0055c2d0` (5/24 caller-frame bytes), `0x0055b750`
  (13/48 region), 5 float10 returns. (C)
- **Lane 1 C3→C4 (387 sites)** waits on the rubric wording decision (G).
- **Lane 2**: 15 indirect-call refusals; next table entries `*DAT_007d4110+off`, `*DAT_007d4108+0x28`;
  register-argument hazard class open. (I)
- **29 NO_SAMPLES** sites need a richer scenario (D). **U-9087**, **D-11069**, `TransformMatrixUpdate`
  (D-10793), `main` 270+ commits behind, 6 orphaned pool locks (F).

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
