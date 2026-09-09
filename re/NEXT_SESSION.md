# Next session — kickoff prompt

## ⇒ CURRENT STATE (2026-09-09 session close) — READ THIS FIRST

Branch `race/first-frame-parity` @ `466e66f3`. **Working tree clean, no stray processes.**
21 commits this session, all pushed to the branch. Trackers: hooks.csv 5,930 rows
(C4 **184**, C3 **922**, C2 3,972, C1 821) · DEFERRED 677 · UNCERTAINTIES 3,040.

This was a **tooling + evidence-integrity** session, not a feature session. It built three
instruments, then used them to find four defects — two of which were in evidence the trackers
had already accepted.

### What was built

| | what it does | state |
|---|---|---|
| `re/tools/matchdiff*.py` | static `.data` operand correspondence, port vs original — no Frida, no Ghidra, offline | swept C3/C4 (932/1086 PASS) + C2 pre-screen (176/185); wired into `promote-c3-batch` step 6 |
| `mashedmod/src/mashed_re/Core/ShadowAB.h` | in-process A/B at the REAL call site — no `arg_type`, no synthetic call, N functions per boot | 7 sites live; 304 samples / 6 fns / 1 boot |
| `scripts/ttd/ttd_reimpl_diff.py --reimpl asi:<Export>` | replays TTD-captured original inputs through the `.asi` export | works (FastSqrt 128/128); capture side blocked |
| `mashedmod/build_objs.ps1` | per-TU object cache | 1-file edit ~18 s, was ~2 min |
| `re/tools/dualinstall_audit.py` | classifies `DUAL-INSTALL REFUSED` and names the dead copy | 39 refusals triaged |

### What was found (the part that matters)

1. **U-9086 — a C4 row's port was wrong.** `0x00404320 PerModeRenderMachine` treated
   `0x007d3ff8` as an array when the original dereferences it as a pointer. Fixed; row
   **demoted C4→C3**, because its evidence was a boot-to-menu survival run and the row's own
   note says the function is *void at the main menu* — the affected arms never executed.
2. **U-9087 — 10 hooks NEVER install.** `HookSystem` refuses any RVA whose first byte is `E9`,
   but those 10 originals *are* compiler jump thunks that begin with `E9` in the pristine
   binary. **4 are C4.** None can hold installed-hook evidence. **Fix not applied — decision
   needed** (§A below).
3. **28 duplicate-RVA implementations in one boot**, 25 with the tracker naming the dead copy —
   and 23 of those had the dead symbol's *name* too, so the rows were authored end-to-end
   against code that never runs. 21 repointed; 4 deferred as **D-11069** (§B).
4. **D-10793 promotion REFUSED on evidence.** `0x00442440 TransformMatrixUpdate` diverged
   48/48 in-race, at `pos.x` and `pos.z`, while `pos.y` matched in all 48 (§C).

Corrections I had to make to my own earlier claims this session, recorded so they are not
re-quoted: "only 14 C2 rows have a reimplementation" (wrong — the `file` column holds analysis
notes for most C2 rows; ~185 have compiled reimpls), and "39 competing implementations" (wrong —
28; the rest were guard false positives and one boot-patch collision).

---

## PICK ONE — the two decisions are blocking, the rest is ordinary work

### A. U-9087 — the `E9`-thunk guard **[DECISION, blocking 10 hooks]**
The guard cannot distinguish our JMP from the original's. Proposed fix: branch on the JMP
**target** — inside `mashed_re_dev.asi` = ours (refuse), inside `MASHED.exe` = the original's own
thunk (safe to hook). Applying it re-enables **10 hooks that have never once run**, 4 of them
C4, so it changes core install behaviour and wants your call, not a drive-by change.

### B. D-11069 — 4 duplicate rows whose copies live in **different targets** **[DECISION]**
For `0x0046cbe0`, `0x00431f30`, `0x004f8660`, `0x004f8690` the winner is `.asi`-only while the
loser is in exe+asi — so in the exe the "loser" is the ONLY implementation. Either delete the
redundant copy so one implementation serves both targets, or extend `hooks.csv` to record
per-target implementations. Note each earned its C-level against *one* of the two bodies.

### C. Fix `TransformMatrixUpdate` (D-10793) **[ready to work, well localised]**
`pos.y` is already correct, `pos.x`/`pos.z` are not — that localises it. Scaffolding is in
place (`TransformMatrixUpdate_impl` + shadow wrapper over `param_1+0x4c..+0xcb`); the
`RH_ScopedInstall` is deliberately left commented so a defective hook does not ship. Re-enable,
fix, re-run — C3 when 48/48 come back clean on the non-PAD fields. **No Frida arg_type needed.**

    MASHED_SHADOW_AB=1 py -3.12 re/frida/scenario_launch.py --hooks 0x00442440 --hold 30
    # then: original/shadow_ab.log

### D. Grow the shadow lane — adoption is the only throughput limit
6 functions/boot is not the ceiling, it is *every shadow site that exists*. Adding one is two
lines. Best next targets: the arg_type-blocked C2 rows in `DEFERRED` (19 found; check
reachability first with `MASHED_COUNT_RVAS`, which costs nothing and installs no hook —
`0x00412cf0` fires **0** times in a race and would have been a wasted session).

### E. Hand-review the 52 operand candidates
`re/parity/matchdiff_triage.csv`. Start with the **9 at size ratio < 0.5** — deeper delegation
chains or genuinely partial ports. Six artifact classes already explain 98 of 155; a class
label EXPLAINS a row, it does not clear it.

### F. Carried over, untouched this session
- **Desktop verification** of the wave-2/3 playtest commits (`playtest_feedback_20260906.md`).
- **D-11065/67/68** still Ghidra-gated. D-11066's premise was answered (`0x0067eaf0` IS the
  cursor); D-11067's premise was *corrected* — `RaceRankThreePlayers` exists and passes the
  operand check, its install is merely commented out, so that task is wire+verify, not port.
- **`main` is 272+ commits behind** this branch and 12 worktrees are stale. This is what makes
  `Agent(isolation:"worktree")` fork from a stale base. Removal ONLY via `diag.py wt-remove`.
- **TTD recording** needs a Defender ASR path exclusion for `tools\ttd_x86\` from the device
  admin (machine is now org-managed). Until then the lane has ONE capture, 8 distinct inputs.
- **Housekeeping:** 6 orphaned Ghidra pool locks (`mashed_pool/Mashed_pool{0,1,10,11,12,13}.lock`,
  dated 07-30..08-31). Not mine — I released slot 14 cleanly — and left alone because I could not
  verify no other session holds them. Clear with `ghidra_pool.sh` if you know none is live.

---

## Ready-to-paste kickoff

> Resume the Mashed RE lane. Branch `race/first-frame-parity` @ `466e66f3`, tree clean.
> Read `re/NEXT_SESSION.md` first, then pick ONE of A–F.
>
> Two items are decisions only you can make: **U-9087** (fixing the `E9`-thunk guard re-enables
> 10 hooks that have never run, 4 of them C4) and **D-11069** (4 duplicate RVAs whose two copies
> live in different build targets).
>
> If you want ordinary progress instead, take **C**: `0x00442440 TransformMatrixUpdate` diverges
> at `pos.x`/`pos.z` while `pos.y` is correct, the scaffolding is in place, and it needs no Frida
> `arg_type` — run
> `MASHED_SHADOW_AB=1 py -3.12 re/frida/scenario_launch.py --hooks 0x00442440 --hold 30`
> and read `original/shadow_ab.log`.
>
> Standing rules that bit this session: never `--hooks all` (phase-2 wedge, 0 samples); check
> reachability with `MASHED_COUNT_RVAS` before spending a boot; and treat a clean shadow run as
> evidence for `re-classify`, never an automatic C-level.
