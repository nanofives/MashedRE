# PREREG RACE-LOAD-004E99B0 (run 57, race-path row, L-184)

Committed alone, before any arm of this row.

Change: the race-load stop 0x004E99B0 (caller 0x004b3c60) and its callees with no exe body get bodies on the knob path only
(MASHED_ORIG_TOP + MASHED_OT_RACELOAD, raceload profile, no new knob, no default change):
- 0x004e99b0, 0x004e9e40, 0x004ea220: re/match/rwl02 sources rewritten from the disassembly (the old raw bodies read their
  chunk headers through separate locals), 0x004e51e0: source unchanged; all four NOT-PROVEN under equiv.py, listed in
  re/match/PROVISIONAL.tsv (class P3), `prov_bind` + roots in re/tools/bind/state/raceload/.
- 0x004e5280 (C4 match) bound as a root; 0x004f3e90 aliased to the lifted RW core body the exe already compiles
  (RwCoreLift_004f3e90, RwCore/RwCore_Lifted_04.cpp).

Arms (object build, staged image 81cc7775, private root D:\MashedWT\RACE-LOAD-004E99B0_root):
RO_ROLOADRWC-<tag> (stack verify/savelink_20261023/stacks/ROLOADRWC.txt), checkpoints 0,1,17,18,299,301,600.
Knob boot on the VS2022 exe: MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=600, re-run once on a short exit.

Measured, decision B: G1 met only if the exe load length under MASHED_ORIG_TOP on the object build is above 3 (ref1 108) or
race.n above 0 on a ROLOADRWC arm. Otherwise the RESULT's first line is "not met, stop moved from 0x004E99B0 to X".
Also reported: the CUT wall count re-cut with verify/racepiz_20261127/cut_join.py on RACE-PIZ-DEVICE's census
(prov_anchor.json sha256 4043240e...; head control 108), and the 0x00420420 execution count on a knob-on arm (read only).

Expected direction (not a done-means): the stop leaves 0x004E99B0. No G-CTRL / B / 12x4 in the worker (row kind).
