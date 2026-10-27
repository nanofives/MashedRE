# PREREG D4-WSB-BVH-WRITESET (committed before any arm)

Base: `orch/integration` @ `1158f5c1c`. Prior evidence: `verify/wsbbvh_20261026/RESULT_D4-WSB-C4-BVH.md`.
Rows: the 9 BVH rows that stayed C3 there.

## Change under test

Harness only, no knob, no default. The C3 shadow wrappers of `0x0055fea0`, `0x0055ff90`, `0x00561040`,
`0x00561390`, `0x00564040`, `0x00564c80` move from `ShadowAB::Run` (return only) to
`ShadowAB::RunTracked` (page-level write tracking + the caller's 4 KB stack window, pre-state restored
between original and port, `Core/ShadowTrack.h`). Unarmed (`MASHED_SHADOW_AB` unset) both call only
`_impl`, so no computed value changes. The wrappers are compiled into the exe too, so the default
gates are re-run on this branch's exe and must equal `merge_train.json` `gctrl.ref` / `b.ref`.

## Arms and verdict rules (fixed now)

1. **Gates.** `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`. Any hash off the
   ref = RED for the step (code-layout move), no promotion, report it.
2. **Write-set shadow** (census scenario `scenario_launch.py --track 0 --mode 10 --cars 4 --hold 60
   --hooks 0x<rva>`, `MASHED_SHADOW_AB=1`, one hook per boot) for the 6 rows above plus `0x00565260`
   (already RunTracked), `0x00563e70` (RunTracked), `0x00565200` (RunRegion 0x1c at `param_3`).
   CLEAN = n >= 1 sample, 0 divergent, `A/B-IS-REAL installed=e9`. Any divergent sample = the row stays
   C3 with the page detail recorded. n = 0 after one re-run = the row stays C3 (not observed).
3. **Racediff re-reads, no new comparator latitude beyond these two, each backed by a static proof
   in the RESULT:**
   - `0x00564c80`: arg 3 of the call to `0x005646c0` is compared at 16 bits, because the original
     `0x005646c0` reads that slot at exactly 4 sites (`0x005647e3`, `0x00564830`, `0x00564aab`,
     `0x00564af8`, recursive descent, esp-delta tracked, no conflict) and each is followed by
     `and reg, 0xffff` before any use. Every other arg and call stays full width.
   - `0x00565260`: return not compared (declared `void`, tool v5 rule) only if the EAX consumers of all
     3 call sites are shown dead: the one that reads EAX (`push eax` at `0x0055af06`) feeds `param_2`
     of `0x00564c80`, whose high half must be shown never read (`0x00564c80` + `0x005646c0`).
4. **Census racediff** for `0x00563e70` / `0x00565200`: the v5 capture agent attached to the PID that
   `scenario_launch.py` prints (same scenario as 2), ON boot `--hooks 0x<rva>`, OFF boot no `--hooks`
   (`MASHED_RE_NO_AUTO_HOOK=1`, `.asi` loaded, nothing installed). Both boots `MASHED_NO_SELFTEST=1`,
   so neither the shadow A/B nor `MASHED_PHYS_C4_SELFTEST` re-executes a body under capture.
   Window = A4 (`0x00470670`) calls 1-3000, budget 6.
   C4-CLEAN rule unchanged from the v5 tool. Not observed on both sides = stays C3.

A row goes C3 -> C4 only if it has a C4-CLEAN racediff (old or re-run per 3/4) AND a CLEAN write-set
leg (2). Promotions through the re-classify skill only.
