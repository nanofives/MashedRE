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
into boot; `0x00421980` alone died 26 s in, mid-race. Both crash dumps end at the same audio
frame (`eip 0x005bbf59`, `ecx` null or garbage). First reading: the pre-image restore reverted
another thread's write on a shared page (contamination). **Control run** (same hooks installed
LIVE, A/B disarmed via the new `--no-shadow` / `MASHED_NO_SELFTEST=1`): **identical crashes**
at the same times. So both are **defects in the generated ports**, not in the tracker — the
`L2_` opt-in guard exists for exactly this. The contamination reasoning still stands as a
design hazard, and the tracker now suspends every other thread for the whole compare window
(`SuspendOthers`/`ResumeOthers` in `ShadowTrack.h`, deadlock caveat documented there).

Lane 2 consequences (already applied to `decomp2port.py`): a callee whose own decompilation
shows FEWER stack parameters than the call site passes is refused (`CALLEE_REG_ARG`: the extra
argument travels in a register a cdecl thunk cannot set); a loop that advances a pointer it
never passes to a zero-arg call is refused (`HIDDEN_REG_ARG`). Neither rule flagged
`0x00421980`/`0x00421960` or the boot-crash group, so a further hazard class is open; the
single-site bisection (`batch_control2.txt`) names the exact ports.
