# PREREG D4-WSB-STACKONLY-C3

Committed alone, before any arm. Branch `orch/D4-WSB-STACKONLY-C3` off `orch/integration` @ `0d1285dc7`.
No source change on this branch. Build `MASHED_NO_DEPLOY=1` at `0d1285dc7`: `mashed_re.exe` `779bc9c3...c447`,
`mashed_re_dev.asi` `c6a264a3...3566` (copied to the private root `D:/MashedWT/D4-WSB-STACKONLY-C3_root` only).

## Rows and rule

User decision L-40 (a), 2026-10-27: `0x0055bae0`, `0x0055c000`, `0x00568990`, `0x0056b7a0` stay C3. The L-38 rule is
unchanged: a write-set leg counts only when it compared **more than 0 heap pages**.

- `0x0055bae0`, `0x0055c000`: every return is a caller-stack address (BROAD-WS, `RESULT_D4-WSB-C4-BROAD-WS.md` leg 2),
  so no scenario gives them heap pages while the caller's buffer is a local. **No arm. They stay C3.** This step only
  records that, with a dated note through re-classify.
- `0x00568990`, `0x0056b7a0`: in the census (track 0 Training) every sampled call returned 0 and wrote nothing
  (SOLV-WS section 1). `0x0056b7a0` appends a pair only when two RWP bodies' AABBs overlap
  (`RwpSolverCore18.cpp:137`), and `0x00568990` returns 0 before any store when `param_3 == 0`
  (`RwpSolverCore21.cpp:205`). RWP bodies are the track's `RWP_Object` props (tyre walls, crates,
  `re/analysis/formats/track_world_bsp.md:139`). This step looks for a race where cars push props into contact.

## Arms (one hook installed alone per boot, `run_shadow5.py`, under `mashed_lock.py` and the test slot)

Env on every arm: `MASHED_ROOT=D:/MashedWT/D4-WSB-STACKONLY-C3_root`, `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1
MASHED_SHADOW_STW=1 MASHED_SHADOW_NEED_PAGES=1`, `HOLD=150`, mode 10, 4 cars, `SAMPLES_DONE=24`.

1. **Arm S (scenario search), `0x0056b7a0`**: `TRACK=12` (City, 10 props / 52 instances). If it gives 0 counted samples,
   then `TRACK=3` (Arctic), then `TRACK=11` (Warzone), then `TRACK=10` (Dump). Stop at the first track with at least 1
   counted sample. At most 4 boots.
2. **Arm H, `0x00568990`**: on the track arm S picked. If arm S picked none, arm H is not run.
3. **Arm R, racediff on this build**: `verify/wsbsolvws_20261027/c4_racediff_v7.py` unchanged, through a copy of
   `run_bvh4.py` with data path / label changed, Quick Battle window (A4 300..1200), one OFF + one ON boot per row, for
   each row whose arm S/H passed.

Before each original launch: `Get-PnpDevice -Class AudioEndpoint -PresentOnly` must list an endpoint. If not, or if
the AV at `0x005baf48` comes back, the row is parked BLOCKED ("audio endpoint"), no retry. A crash with a U-9677
signature (`MASHED.exe+0x1509b4`, `+0xe66db`) is re-run once, not a finding.

## Read-out per row (fixed now)

- **PASS write-set**: at least 1 counted sample, heap pages compared (sum over samples of max(o,n) heap pages) > 0,
  ndiff 0 on every counted sample, `PATCHBYTE ... installed=e9 ... A/B-IS-REAL`, no NOISY / UNTRACKED / OTHER-EXC line
  for the row.
- **DIVERGENT**: any counted sample with ndiff > 0. Row stays C3, divergence reported with its page / offset.
- **NO-WRITES-SAMPLED**: 0 counted samples on every track tried. Row stays C3.
- **C3 -> C4** only if PASS write-set and arm R gives `C4-CLEAN` (installed `0xE9`, same signature set both sides).
  This meets the unchanged L-38 rule with new data, it does not relax it. Any other outcome: stays C3, dated note.

Default gates on this build: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, must match
`re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`.
