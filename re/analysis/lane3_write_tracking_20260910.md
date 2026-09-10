# Lane 3 — page-level write tracking for the shadow A/B (2026-09-10)

Companion to `promotion_lanes_assessment_20260910.md` (Lane 3) and `Core/ShadowTrack.h`.
**Moves no C-level**; it is a verifier. Results feed `re-classify` like every shadow verdict.

## What it is

`ShadowAB::RunTracked(counter, impl, SHADOW_STACK_WINDOW(), args...)`: an A/B that compares
**effects**, not a return or a hand-declared region. Per sampled call:

1. every committed, writable, non-executable page that is private (heaps, `.bss`) or part of
   `MASHED.exe`'s image (`.data`) is set `PAGE_READONLY` — excluding this `.asi`'s image, the
   tracking pools, the current thread's stack and TEB, and guard pages;
2. a vectored exception handler catches the first write to each page, saves the pre-image,
   re-enables the page, resumes; faults from other threads are counted as noise;
3. the ORIGINAL runs (hook uninstalled); post-images are taken; pre-images are written back,
   so memory is as it was before the call;
4. the PORT runs under the same tracking;
5. pages the original touched: `post_orig` vs live; pages only the port touched: pre vs live
   (writing the same value is not a difference); the caller's 4 KB stack window above the
   wrapper's return address is snapshotted/restored/compared the same way (out-params in
   the caller's frame).

Log detail per sample: `pages=o:N,n:N,diff:D stack=B noise=X ovf=Y ret=R @page+off stk+off`.

## Why it exists

The first sweep (`shadow_lane_20260910.md`) hit two walls: 7 of 10 DIVERGENT rows were
non-idempotent functions whose second execution legitimately returned something else, and 348
void ports (55 C2 + 293 C3) had no verified output region. Restoring the pre-state between the
two executions removes the first; comparing whatever was written removes the second.

## Limits (stated up front so nobody overclaims)

- `.asi` only. Effects outside tracked memory are invisible: files, handles, GPU, other
  modules' `.data` (system DLL globals are untracked on purpose), writes below the caller's
  frame window. Irreversible side effects execute twice.
- Noise pages (another thread wrote during the window) are excluded and can hide a real
  difference on that page; the sample reports the count.
- Allocation-heavy functions diverge by construction (different heap blocks); read offsets.
- The hook is re-installed only after the tracked window (the registry entry lives in a
  tracked heap page), so a recursive call from inside the port reaches the original.
- Heavier than `Run()`: two protect/unprotect passes per sample; budget 24 samples per site.

## Wiring

- `re/tools/decomp2port.py` emits `RunTracked` for every generated Lane 2 port (void or not).
- `re/tools/shadow_gen.py --tracked` converts `NEEDS_REGION` void ports (55 C2 today).
- `shadow_batch.py` / `shadow_ab_report.py` unchanged: ndiff counts differing pages + stack +
  return.

## First live runs

**Run 1 (20 tracked Lane 2 void ports, `batch_lane3_l2void.txt`)**: the 7-site group died 4 s
into boot; `0x00421980`+`0x00421960` died 26 s in, mid-race. **Control run** (same hooks
installed LIVE, A/B disarmed via the new `--no-shadow` / `MASHED_NO_SELFTEST=1`,
`batch_control1.txt`): identical crashes at the same times, so these are **defects in the
generated Lane 2 ports**, not in the tracker. Single-site bisect (`batch_control2.txt`):
`0x00421960`, `0x004219c0`, `0x00495fe0` crash alone; `0x00421980`, `0x0041f290`, `0x0041f060`
run a full race alone. CORRECTION to an earlier reading in this session: the "audio-frame
crash dumps" I cited are dated 2026-09-09 and belong to another session; today's deaths wrote
no dump. Nothing about the crash SITE is established from dumps.

**Run 2 (55 tracked hand-ported C2 void sites, `batch_lane3_c2void.txt`)**: every site that
actually FIRED under tracking crashed at its first sample (`0x00423b00 FrontendInputDispatch`,
`0x0047e9c0` physics-scene init root, both ~10 s = race start); every site that never fired
survived the boot. These ports run in every default boot, so this is the **tracker**. No dump,
no log line (the crash precedes `Record`). Breadcrumbs (`MASHED_SHADOW_TRACE=1`, one line per
stage outside the protected windows) were added to find the dying stage; see below.

Lane 2 consequences (already applied to `decomp2port.py`): a callee whose own decompilation
shows FEWER stack parameters than the call site passes is refused (`CALLEE_REG_ARG`: the extra
argument travels in a register a cdecl thunk cannot set); a loop that advances a pointer it
never passes to a zero-arg call is refused (`HIDDEN_REG_ARG`). Neither rule flagged
`0x00421980`/`0x00421960` or the boot-crash group, so a further hazard class is open; the
single-site bisection (`batch_control2.txt`) names the exact ports.

**Diagnosis of the run-2 crashes (breadcrumbs, 12 traced boots)**: the process died *silently*
(no dump, no fault ever reaching the handler) inside the original's protected run. Per-stage
crumbs written with `WriteFile` only narrowed it to: pages read-only, handler NOT yet marked
active. `Protect()` ran before `s.active = true`, so a write by **any other thread** in that
gap was an unhandled access violation and terminated the process. With private memory (heaps)
protected that window was hit on the first sample every time; with exe-image pages only, three
samples survived by luck and the fourth died. **Fix**: arm the handler before the first
`VirtualProtect`, disarm after the last restore. Two further findings from the same traces:
the ntdll region holding the vectored-handler node must never be protected (the dispatcher
increments its refcount before calling any handler); and stop-the-world suspension deadlocked
the physics-scene init root (59 threads suspended, CPU flat, window unresponsive), so it is
now opt-in (`MASHED_SHADOW_STW=1`) and the default keeps threads running and **discards any
sample where another thread faulted on a tracked page** (`NOISY`, not counted).

**Run 3 (`batch_trace23.txt`, exe-image pages only, threads running)**: `0x0047e9c0`
**24/24 CLEAN** (`pages=o:2,n:2,diff:0 stack=0 ret=0`), 13 further samples discarded as NOISY
(`noise=2`: two other threads write exe `.data` during the window). The one earlier sample that
had shown `diff:1 @00624048` was a NOISY sample — contamination, not a port defect, and the
discard rule is what caught it. This is the first evidence the lane produces: effects of a void
physics-init function compared page-for-page against the original at its real call site.

Private memory (heaps) is tracked only with `MASHED_SHADOW_PRIVATE=1` (optionally bounded by
`MASHED_SHADOW_PRIV_LO/HI`); regions below the exe image base are never protected (loader /
WoW64 bookkeeping killed the process the instant it went read-only). Results of the private
probe and the 55-site batch: see the run log names in the next section.

## Results — 55 tracked hand-ported C2 void sites (exe-image pages, threads running)

Runs `batch_lane3_c2void2.txt` + `batch_lane3_c2void3.txt`, 18 boots, 4 cars, 60 s:

| verdict | n | reading |
|---|---|---|
| CLEAN | **18** | 24/24 tracked samples each, touched pages + caller stack window + return identical |
| DIVERGENT | 2 | `0x0055c2d0` 5/24: 1–2 bytes at caller-frame `+0x3c` (a float out-param written through a pointer; precision-class, same family as the float10 limit — not a logic verdict). `0x0047e9c0` 1/24 with `noise=0` at `.data+0x624048` while 23/24 and a separate 24/24 run were identical: contamination by a non-faulting second writer (the one case the discard rule cannot see) or a data-dependent path; **intermittent, unclassified** |
| CRASH | 2 | `0x0055bd80` dies during load (phase 2, before any sample); `0x00560260` completes 24 clean samples then the game dies — its effects go to the HEAP, which is not restored while private tracking is off, so the double execution leaks state |
| NO_SAMPLES | 11 | never fired in this scenario |
| not booted | 22 | budget |

The private-memory probe (`MASHED_SHADOW_PRIVATE=1`, `batch_trace24_private.txt`) still dies
silently on the first sample; it stays opt-in and unexplained. That is the next thing to trace
(`MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_TRACE=1`, one boot), because heap-writing functions
like `0x00560260` need it to be safe.

**Promotion (done 2026-09-10, later the same session)**: gate check `log/shadow_ab/c3_gate_check_tracked.tsv`
— 7 PASS, 11 LEAF (zero callees, sole caller C2; leaf exemption CONFIDENCE.md L24 satisfied by
the 24/24 effect-identical samples), 0 FAIL. **18 rows C2→C3** via re-classify; 10 naming/intent
markers filed as U-9116..U-9125 (the shared `font_text_d3` report of `0x004c51a0` was not swept:
its markers are not this row's). C3 total 966 → 984.

