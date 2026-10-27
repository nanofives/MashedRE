# PREREG D4-WSB-SHADOW-PRIVATE, addendum 1 (committed alone, before arm N)

The verdict batch (asi `b0ab9507...`) gave HEAP-CLEAN for `0x0055fea0` (heap o 1..1), `0x0055ff90` (4..4) and
`0x00565260` (2..4), each 24/24. `0x00564040` came back CLEAN-NO-HEAP: `pages=o:0,n:0` and `heap=o:0` in 24/24.
Its only stores go to `param_3` (`RwpSolverLeaves1.cpp:201`, sort block below it), and both callers pass
`*(param_1 + 0x80)` (`RwpSolverBroadphase3.cpp:182`, `:472`). A call that collects 0 entries writes nothing.

## Arm N (`0x00564040` only)

Same recipe as the verdict arms plus `MASHED_SHADOW_NEED_PAGES=1` (new, opt-in): a sample in which neither run wrote a
tracked page, and stack and return agree, is discarded (`[..] EMPTY rv=` line, first 64 logged, at most 2000, then
sampling stops). Every sample line now also carries `rv=` (the return, here the collected count).

Read-out:
- **HEAP-CLEAN**: CLEAN on the counted samples and `heap=o:` > 0 in every one of them. Then the row has the leg.
- **Counted samples with rv > 0 and 0 heap pages are impossible under NEED_PAGES** (they are discarded). So if the
  EMPTY lines show `rv` > 0, the buffer is outside the tracked heap regions. Reported as **BUFFER-UNTRACKED**, row stays
  C3 for this leg, and the buffer address goes to the next step.
- **EMPTY only, all rv = 0**: the sampled calls write nothing. Reported as **NO-WRITES-SAMPLED**, row stays C3.
- Hang / crash / no samples: re-run once, as in the PREREG.

The tracker rule of the PREREG is unchanged.
