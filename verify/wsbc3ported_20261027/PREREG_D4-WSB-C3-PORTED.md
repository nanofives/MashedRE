# PREREG D4-WSB-C3-PORTED: shadow A/B for the 20 census-EXEC C2/ported RW-Physics rows

Committed alone, before any arm. Branch `orch/D4-WSB-C3-PORTED` off `orch/integration` @ `37383a4bb`.

## Scope

Of the 49 C2/ported rows of `third-party-library[RenderWare-Physics-3.7]`, the census
(`verify/wsbcensus_20261026/wsb_census.tsv`) marks 20 EXEC and 29 NOT-EXEC. This step arms the 20 EXEC
rows. The 29 NOT-EXEC rows produce no samples in the census scenario and stay C2 with a note.

`0x0055bd70` is a `__declspec(naked)` argument-forwarding tail JMP (`RwpSolverBroadphase3.cpp:599-604`)
with no ShadowAB wrapper and no own stores (`scan_stores.txt`: 0 stores). It is not armed and stays C2.

## Source change (.asi wrapper only, unarmed path unchanged)

Original-body store scan (`scan_stores.py` -> `scan_stores.txt`, capstone, non-ESP write operands):

- 13 rows wrapped with `ShadowAB::Run` (return only) store through pointers: `00564310 005667c0
  00574ad0 00575120 005752b0 00575560 00576880 00577be0 00577cb0 00578b20 00578bd0 00578cb0 00578ff0`.
  Their wrapper moves `Run` -> `RunTracked(..., SHADOW_STACK_WINDOW(), ...)` so the writes are compared.
- `0055b750` moves `RunRegion` (0xc at param_3) -> `RunTracked`, so every write is compared, not only
  the declared span.
- `005751f0` has 0 stores and keeps `Run`. `0055bd80 0055c2d0 0056f0a0 0056f350` already use `RunTracked`.

Unarmed (`MASHED_SHADOW_AB` unset) `RunTracked` calls only `_impl`, the same as `Run`/`RunRegion`
(`Core/ShadowTrack.h:739`). No `_impl` body, knob or default changes. Expected: G-CTRL and B hashes
equal the `merge_train.json` refs (`5943c5f1/364350ae/52052890`, `3a03670c/e85ea6fb/53778664`).

## Arm

Per RVA, one boot: `scenario_launch.py --track 0 --mode 10 --cars 4 --hold 60 --hooks 0x<rva>`
(the census scenario, only that hook installed), `MASHED_SHADOW_AB=1`, private root
`D:\MashedWT\D4-WSB-C3-PORTED_root`, under `mashed_lock.py run --label D4-WSB-C3-PORTED`.
Runner `run_shadow.py` (copy of `verify/wsbbroadws_20261027/run_shadow3.py`, paths and label changed).
Report `re/tools/shadow_ab_report.py`. Raw to `D:/MashedWT/D4-WSB-C3-PORTED_data/<rva>/`.

## Pass criterion per row (C2 -> C3)

All of: verdict CLEAN, `ndiff = 0`, `n >= 24` (RunTracked budget) or `n >= 48` (Run), PATCHBYTE proof
`A/B-IS-REAL` (installed `e9`, uninstalled not `e9`) or `A/B-IS-REAL(hook-not-installed)`, no crash
attributable to the hook. Plus the static C3 halves already on file: caller and callee at C2+ or leaf
(`antiisland.txt`, all 20 pass), `RH_ScopedInstall`, both targets build clean.

- DIVERGENT: stays C2, the diff detail goes in the RESULT and a note via re-classify.
- n below the floor or NO_LOG after one re-run: stays C2 with a note (not exercised enough).
- Piz-collision crash signatures (U-9677) are re-runs, not findings.

No row is claimed C4 here. The C4 band rows take the C3 rows afterwards (L-38: a C4 needs > 0 heap pages
compared).
