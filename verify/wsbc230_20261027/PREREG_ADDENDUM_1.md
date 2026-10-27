# PREREG D4-WSB-C230-RECHECK, addendum 1: the leg 2 first-call diff

Committed alone, before arm S2. Written after leg 2 arm S1 (`run_shadow_c230.py`, game PID 2380) returned
**DIVERGENT**, n 24, ndiff 1. That result stands as recorded.

## What differed

Only sample `[0]`, the first call of the port body in the boot:
`pages=o:1,n:2,diff:1 stack=0 noise=0 ovf=0 ret=0 heap=o:1,n:2 rv=0f52ca60 @00b8f000+ae4 ndw=1
v=ae4:80000009/80000009/8000000a`. The original run wrote 1 heap page, the port run wrote that page (equal) plus one more.
In the extra page one dword differs: `0x80000009` before and after the original run, `0x8000000a` after the port run.
Samples `[1]`..`[23]`: ndiff 0, `heap=o:1,n:1`.

1. `FUN_0055c230_impl` calls `NarrowDotOrder()` (`RwpSolverGlue5.cpp:179`), which holds a function-local static
   (`?s@?1??NarrowDotOrder@...` and its guard `?$TSS0@...`, `.map` `0003:0008ffe0` / `0003:0008ffe4`). The first call
   runs the MSVC thread-safe static init, `__Init_thread_header` / `_getenv` / `__Init_thread_footer` (the same three
   call-outs BROAD-A addendum 3 recorded on its first ON sample).
2. `__Init_thread_footer` increments the per-thread `__Init_thread_epoch`. In this `.asi` it is the second dword of an
   8-byte TLS template (`.map`: `__tls_start` `0002:00018094`, `__Init_thread_epoch` `0002:00018098`, `__tls_end`
   `0002:0001809c`). Its live copy is in the thread's TLS block, which the loader allocates on the heap. The value
   pattern `0x80000000 + n` -> `+1` is that counter's.
3. [UNCERTAIN] that `0x00b8fae4` is the main thread's `.asi` TLS block + 4. Missing: a read of
   `TEB.ThreadLocalStoragePointer[_tls_index]` in that boot. Arm S2 tests the claim indirectly: if the dword is the epoch,
   moving the static init out of the tracked window removes the diff, and nothing else changes.

## Harness change (before S2)

The `0x0055c230` wrapper calls `(void)NarrowDotOrder();` once before `ShadowAB::RunTracked`, so the static is
initialised before the tracked window opens. `NarrowDotOrder()` is a cached `getenv` read and the impl calls it on every
call anyway, so the value the impl reads does not change. Leg 1 is re-run on the rebuilt exe and must still match.

## Arm S2

Leg 2 as in the PREREG, on the rebuilt `.asi`, one boot. Read-outs unchanged (HEAP-CLEAN needs ndiff 0 on every
counted sample and heap pages compared > 0).

- S2 HEAP-CLEAN: leg 2 passes. The RESULT reports S1 and S2.
- S2 diff on sample `[0]` only, same single-dword pattern: the epoch explanation is refuted or incomplete, leg 2
  fails.
- S2 any other diff: leg 2 fails.

Leg 3 arm T runs on the rebuilt `.asi`. Its transparent list adds `narrowdotorder` (the wrapper's warm-up call, if it is
not inlined). The W1 control boots (original only, no hook installed) do not depend on this change.
