# PREREG PORT-OT-RACE-BIND-PROV4 (run 53, 2026-11-21)

Committed alone, before any arm of this row.

## Change

Knob `MASHED_OT_RACELOAD=1` (default OFF, exe only, `MASHED_ORIG_TOP` only), unchanged knob and stacks
(`@ROLOAD`, `@ROLOADC`). The four checkpoint-2 named stops 0x0047a020, 0x00479330, 0x00478660, 0x00462950 bind their
L-126 provisional sources (`re/match/PROVISIONAL.tsv`, rows of RACE-COURSELUA-BODIES, all NOT-PROVEN, knob path only,
never C4) through `verify/ro_20261116/rltools/rl_bind_gen.py` (`PROV_BIND` extended in place). Their static closure adds
45 more matched (c4=match) binds; all 49 new binds are in `log/orch/run53_bindlists/race_owned.txt` and none in
`audio_owned.txt` or `bulk_cp1.tsv`. 0x00458c70 stays the counting stand-in (owned by RACE-RWGLOBALS-FRAMELIST).

## Predictions (arms on the object build, ref1 = `D:/MashedWT/RO-REF-NAME_data/ref1`)

1. Knob off (`RO_ROIMG-pr1`): unchanged, exit 0, 600 ticks, load length (`race.dn_warp`) 1, first divergence record 0
   `0x00604604`.
2. Stops-exit arm (`RO_ROLOAD-pr1`): the 0x0047a020 stop line no longer appears. The arm ends at a later stop in the
   source call order of 0x00426e10 (the next named stop reached), or reaches the race anchor.
3. If the anchor is reached, load length is above 3 (checkpoint 1) and at most 108 (ref1). If it is not reached, the load
   length is not measured and the reached stop is the next body to bind.
4. First divergence against ref1 stays record 0 (tick 1), `0x00604604` (the knob acts only at tick 300).
5. Self-tests (`[REPLAY]`, `[TRACE]`, `[OT_TIMER]`) PASS on every arm that prints them.
6. `ddkobj.py check`: no newly DIFFERENT unit among the 45 matched new binds; the four provisional units are kept out
   of the object build by `ddkobj.py load_provisional` (they keep their VS2022 bodies) or reported DIFFERENT and
   classified provisional.
