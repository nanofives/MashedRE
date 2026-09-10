# Next session — kickoff prompt

## ⇒ CURRENT STATE (2026-09-10, second session — merge + open-item drain) — READ THIS FIRST

Branch `race/first-frame-parity`, tree clean. Trackers: hooks.csv 5,930 rows
(C4 184, **C3 1,009**, C2 3,885, C1 821) · DEFERRED 677 · UNCERTAINTIES 3,083.

**Landed this session:**
1. **Merges drained.** `docs/reconcile`, `race/nav-champ`, `race/arctic-cap` merged (the first two
   needed conflict adjudication — HEAD's 2026-08-31 measurement SUPERSEDES nav-champ's
   2026-08-30 "codes 11/12 are inert" claim, and `verify/nav_shots/FINDINGS.md` now carries a
   dated header saying so). 47 unprotected original-side reference captures under
   `verify/arctic_ref/` + `verify/geomlight_broadcheck/` force-added (they were untracked because
   `verify/**/*.bmp` is gitignored; re-capture is not bit-reproducible). The uncommitted
   Challenge-Select `dot` work was rescued off the stale agent worktree (37 commits behind) by
   re-applying onto HEAD. **`area/frontend` is the ONLY branch still unmerged** — deliberately: it
   is a WIP checkpoint whose `PanelSortInit` hook (`0x00420d00`) would go live in the dev ASI
   unverified.
2. **PAL4 was rejected by `QuadRenderer`**, so every BADGES 16x16 sprite silently never drew —
   the whole Finding-38 status-glyph family and the detail-panel checklist have been dead since
   they landed. Fixed; 15/17 screens byte-identical scope control. **U-9130** files the honest
   limit: `verify/orig_screens/s6.bmp` is NOT state-matched (4 unlocked rows vs our 1) so it
   cannot adjudicate the restored draws. `re/analysis/pal4_quad_upload_20260910.md`.
3. **Item B CLOSED — `0x0056bce0` C2→C3, 48/48 CLEAN.** Its hypothesis was wrong (args ARE plain
   cdecl); the real cause is an **implicit caller-saved-register contract**: callers
   `0x0056c310`/`0x0056c0a0` deref EDX at `0x0056c3e7`/`0x0056c180` immediately after the call,
   the original never writes EDX, and MSVC does. Fixed with a naked EDX-preserving shim, plus two
   independent precision corrections. **This is a lane-wide class** —
   `re/analysis/bce0_edx_contract_20260910.md`, and the screen below.
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
- **Crashes to bisect — BISECTED, 5 of 9 resolved** (`re/analysis/shadow_crasher_bisect_20260910.md`).
  The "group" `0x0056f350 0x0056fea0 0x00570090` had never been split; one site per boot plus a
  `--no-shadow` control each gives:
  | RVA | armed alone | control | verdict |
  |---|---|---|---|
  | `0x0056bce0` | CLEAN 48/48 | — | **C3, closed** |
  | `0x0056fea0` | RACE_OK **NO_SAMPLES** | RACE_OK | **not a crasher** → NO_SAMPLES bucket |
  | `0x00570090` | RACE_OK **NO_SAMPLES** | RACE_OK | **not a crasher** → NO_SAMPLES bucket |
  | `0x0056f0a0` | CRASH 30s | **RACE_OK** | crash is in the **A/B window**, not the port |
  | `0x0056f350` | CRASH 27s | CRASH 26s | crash is in the **installed port** |
  - `0x0056f350`: **argument shape CHECKED and CORRECT** — the original's three cdecl args
    (`E+4`, `E+8`, `E+0xc` float) map exactly onto `(int, float*, float)`. Base (+0x10), field
    (+0xc), stride (0x28), re-read bound (+0xac) and the `local_78[27]` sizing against both
    callees all verified faithful. The fault is at **iteration k = 5** (computed `EAX−EDI`),
    reading `[param_2+0x14C]` — the same address the original reads on the same iteration. Found
    and fixed one real transcription defect (loop counter declared `float`; the original's is an
    int at `[esp+0x50]`), but the crash reproduced and **the fault MOVED** to a `cmov`-ised
    default-pointer ternary yielding 0 (`divss xmm0,[eax]`, `EAX=0`). So there is more than one
    divergent path and whack-a-mole is low yield. Standalone regression-checked: no-op there.
    **NEXT: a runtime probe, not more static reading** — log `k`, `[param_2+0xac]` and
    `[param_2+0xc+k*0x28+0x10]` per iteration from the port and diff against a Frida trace of the
    original's loop on the same call. Full detail in the analysis note.
  - `0x0056f0a0`: control boots clean, so the port is fine; the armed boot NULL-writes at
    `0x0056caf4` inside a **third** function, `FUN_0056caa0`. Suspect the A/B window: a side
    effect outside the tracked span that the restore cannot undo (`ShadowAB.h` LIMITS names this
    exactly). NEXT: read the body for an out-of-span side effect — if it has one this site is not
    a RunRegion candidate at all and should leave the lane.
  - **The caller-saved-register class is RULED OUT for the other eight**: all 14 of their callers
    decompiled and grepped for `extraout_EAX/ECX/EDX` — zero hits. One `extraout_ST1` /
    `extraout_ST1_00` in `FUN_00570090` (x87 stack depth) is now moot, since neither site it
    calls is a crasher.
  - Still untriaged: `0x0055bd80` (at load), `0x00560260` (24 clean samples then heap effects
    unrestored), Lane 2 generated `0x00421960 0x004219c0 0x00495fe0`. (B/H/I)
  - **Reusable recipe — this is the part that mattered:** (1) `--no-shadow` control, to decide
    port-vs-witness; (2) zero-hook baseline, for a passing control; (3)
    `re/frida/poll_attach_catch_crash.py` in a background job **alongside** `shadow_batch.py`,
    then resolve the EIP against `mashed_re_dev.map` at preferred base `0x10000000`. WER produced
    **no** minidump for any of these, so waiting on one finds nothing.
  - **Harness caveat:** `--no-shadow` sets `MASHED_NO_SELFTEST=1`, so a control logs no samples
    by construction. Only an **armed** RACE_OK with n=0 means "never fired".
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

### B. DONE — `0x0056bce0` is C3 (48/48 CLEAN). The hypothesis below was WRONG; kept for the record.

~~Disassemble the original's prologue/argument use: if `param_3` (float) is not a plain cdecl stack
arg …~~ **Args ARE plain cdecl** (`0x0056bce3`/`0x0056bce7`/`[esp+0x1c]`, `add esp,0x10 / ret`).
The crash was the caller's EDX, not the callee's signature. See
`re/analysis/bce0_edx_contract_20260910.md` and the crash-triage recipe under "Open items".

### B2. Chase the `extraout_ST1` lead on `0x0056fea0` / `0x0056f0a0` **[the natural successor]**
Same shape as B but one register class over — see the bullet under "Open items". Confirm or refute
whether `FUN_0056fea0`'s original leaves the x87 stack at a depth the port does not.

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
