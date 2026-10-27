# PREREG D4-WSB-C4-BROAD-A, addendum 3: 0x0055c230 control in the long census window

Committed before arms Z1 and Z2. Written after the long census racediff of `0x0055c230` (`C4_HOLD=400`, A4 calls
300-30000) returned HOLD `BEHAVIOR-DIFF`, 6 calls each side. That HOLD stands as recorded.

## What differed

OFF: `[c55c680] [c55c0f0,A4+0,A1+0] eax=A4+0` x3, `[c55cdd0] [c55c0f0,A4+0,A1+0] eax=A4+0` x3.
ON: `[c55cdd0] ...` x3, `[c57b420] ...` x2, and one sample that starts with
`[a:__Init_thread_header,..] [a:_getenv,..] [a:__Init_thread_footer,..]` then `[c57b420] [c55c0f0,A4+0,A1+0] eax=A4+0`.

1. The three extra call-outs are the first-call initialisation of the wrapper's function-local static counter
   (`SHADOW_AB_COUNTER`, `RwpSolverGlue5.cpp`), harness code that runs once per boot. They are the same class of call
   as `__security_check_cookie`, which v7 already treats as transparent.
2. The first token of every sample is the indirect call `call dword ptr [edx+0x18]` at `0x0055c2a8`, `edx` =
   `[param_1+0x5c]` (`0x0055c2a3`). Its target depends on which object the sampled call receives. The rest of every
   signature is the same on both sides. The row first runs late in the race (the abb0 run of the same window reached
   A4 call 15197 / 13379), after round changes, where run-to-run variation of the original is known
   (`round-level outcomes do not reproduce`).

[UNCERTAIN] whether two boots of the original give the same 6 samples in this window. Missing: an OFF-vs-OFF run.

## Arms (`C4_TOOL=c4_racediff_v10.py`, census, `C4_HOLD=400`, A4 calls 300-30000)

- **Z1** `C4_CONTROL=1`, `C4_TRANSP_SYMS=__init_thread_header,__init_thread_footer,_getenv`: two boots of the original.
- **Z2** same transparent set, hook installed alone vs original.

## Rules

- Z1 `CONTROL-DIFF`: this window does not reproduce for `0x0055c230` on the original. It stays C3 whatever Z2
  shows, and the RESULT names the missing evidence (a reproducible window that reaches the row).
- Z1 `CONTROL-SAME`: promoted only on Z2 `C4-CLEAN`.
- Re-run only for both-sides-empty or a U-9677 crash.
