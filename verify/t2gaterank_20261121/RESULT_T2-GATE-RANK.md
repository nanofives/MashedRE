# RESULT T2-GATE-RANK (run 53, 2026-11-21, text-only)

Branch `orch/T2-GATE-RANK` from `orch/integration` `a9e5b4c8a`. Row kind: text-only analysis. No build, no game run, no
gates, no tracker edit. One attempt. Worker claude2, Opus 5.5 medium, lead cmwpwu4745v0nnz1ci4gqg2l2.

## 0. Verdict: GREEN (table produced, top 10 named)

140 gates on the executed top-down path have T2 census functions behind them. The top 40 are in section 3, all 140 in
`T2_GATE_RANK.tsv` (25.7 KB). Two hand stand-ins of the run loop dominate everything else:
**0x004111c0** (release 312 census functions, 7.5 M T2 calls; it dominates 721) and **0x00492e90** (release 151, 5.5 M
calls; dominates 114). The next gate releases 40.

## 1. Method (`t2_gate_rank.py`, output `t2_gate_rank.out`)

- **Graph:** `verify/portorigtop_20261109/origtop_size.py` edges of the original (`MASHED.exe.unpatched`): direct E8
  calls plus relocation edges (address-taken functions and pointer tables, 78 tables). Restricted to the 3377 functions
  the original executed in the census (`verify/census_20261116/census_T2-RACETICK-CENSUS.tsv`, any phase).
- **T2 census function:** calls > 0 in phase `race-load` or `race-tick`: 2450 functions, 54,357,823 calls.
- **Route state at this head**, per rva, first source that names it:
  1. the plans: `verify/ro_20261116/rltools/plan_rl.json` (PORT-OT-RACE-BIND-PROV4, 139 bind / 75 reuse / 46 stop) and
     `re/tools/bind/state/audio/plan.json`. bind / reuse = `run`, stop = `stop`.
  2. `verify/porttop_20261108/BINDINGS.tsv`: matched / near-match = `run`; bound / crt / shim = `lib` (supplied, not
     followed); port = `hand`; stand-in / under-stand-in = `stand`; missing / rw = `unbound`.
  3. the `#define FUN_<rva> <target>` aliases of every `PortTop_*` TU in `mashedmod/exe_sources.rsp` (1301 rvas, 117 not
     named above): target with `Stop` = stop, `StandIn` = stand, `PortTop_Bound_` / `Orig_` / `PortTop_RL_` / `RwCore_`
     = run, any other exe name = hand. 134 rvas are aliased differently by different TUs; the highest state is kept.
  4. knob-on view: `MASHED_OT_RACELOAD=1`, so 0x0040d270 = `Orig_0040d270`
     (`Match/PortTop_StandInsRL.cpp`, `PortTop_Bound_0040d270`).
- **Executed path R:** closure from 0x00492370 following `run` nodes only: 770 functions, 513 of them T2 census (13.5 M
  calls). **Gate** = a node R reaches in state stop / stand / unbound: 168 (49 stop, 42 stand, 77 unbound).
- **Per gate:**
  - `rel_tool` (the ranking key): T2 census functions outside R that run once this gate alone is bound, following
    run bodies and every node a `porttop_bind.py write` binds by itself (C4 `c4=match` with a re/match source, not in a
    `cpp_unclean.txt`, or an `ADMITTED.tsv` row). Ties broken by their T2 calls, then `behind`.
  - `rel_alone`: the same following today's run bodies only.
  - `behind`: T2 census functions the gate dominates (dominator tree from 0x00492370 following every node except
    lib / hand): every static path to them passes through it.
  - `next_stops`: the gates the release itself hits (first 12 listed).
- **Class** of each gate: hand stand-in (run-loop piece = one of origtop_size's RACE nodes) / c4=match bindable /
  c4=match cpp-unclean (first compile error in the TSV's source of `cpp_unclean.txt`) / C4 no bindable source (route
  tag) / C4 frida-replay with source / C1-C3 with a source (best_pct from `re/analysis/match_ledger.tsv`, in-situ verdict
  from `re/census/t2_route_table.tsv`) / no source.

## 2. Limits ([UNCERTAIN], one line in `re/orchestrator/UNCERTAIN_REGISTER.md`)

- **[UNCERTAIN]** static reach, not a run: 622 of the 2450 T2 census functions have no static path from 0x00492370 in
  this graph (calls through pointers computed at run time), so no gate is credited with them. Test: a count-mode run of
  the knob-on route, compared against `next_stop_list`.
- **[UNCERTAIN]** the 134 alias conflicts take the highest state, so a gate whose callers are wired differently per TU
  may be counted as run.
- 15 T2 census functions sit only behind hand exe bodies (`hand`) and are not ranked. The 31 hand bodies on R are
  listed in `t2_gate_rank.out`.
- The runtime first stop of PORT-OT-RACE-BIND-PROV4, 0x0047b860 (c4=match cpp-unclean, stop), ranks 61: it releases 2
  and dominates 37.

## 3. Top 40

Columns: rank, rva, name, class, rel_tool (functions / T2 calls), rel_alone, behind (functions / T2 calls), route state,
first caller on R.

| # | rva | name | class | rel_tool | tool calls | rel_alone | behind | behind calls | state | caller |
|---|---|---|---|---|---|---|---|---|---|---|
|1|`0x004111c0`|FUN_004111c0|hand stand-in (run-loop piece)|312|7513205|52|721|6490635|stand|0x00492d30|
|2|`0x00492e90`|FUN_00492e90|hand stand-in (run-loop piece)|151|5500959|12|114|3435520|stand|0x00492290|
|3|`0x00429290`|FUN_00429290|hand stand-in|40|2011829|1|8|832|stand|0x00402750|
|4|`0x0040d110`|FUN_0040d110|hand stand-in|36|38192|6|47|63434|stand|0x0040d270|
|5|`0x00440bc0`|FUN_00440bc0|c4=match cpp-unclean|30|241|27|32|367|stop|0x0045bae0|
|6|`0x00409710`|FUN_00409710|c4=match cpp-unclean|21|36219|1|2|2|stop|0x00426e10|
|7|`0x004d8570`|RwEngineRegisterStringFunctions|c4=match bindable|19|135794|9|4|51338|unbound|0x004c32b0|
|8|`0x0048ae00`|FUN_0048ae00|hand stand-in|18|1897601|5|0|0|stand|0x0040bbb0|
|9|`0x00425bf0`|FUN_00425bf0|c4=match cpp-unclean|17|26050|0|1|1|stop|0x004262f0|
|10|`0x00487280`|FUN_00487280|no source (C2)|15|3102578|10|5|851|stop|0x00480100|
|11|`0x0055f800`|FUN_0055f800|C3 with a source (65.3%)|15|31504|13|26|68237|stop|0x0055f450|
|12|`0x005a7b60`|FUN_005a7b60|C2 with a source (53.5%, in-situ CRASH)|15|1891|10|42|3291|stop|0x0045d460|
|13|`0x004e7420`|RpClumpStreamRead|C2 with a source (37.9%)|14|160047|8|8|1846|unbound|0x004781b0|
|14|`0x0041def0`|FUN_0041def0|hand stand-in|13|1895823|2|0|0|stand|0x00402750|
|15|`0x004775b0`|FUN_004775b0|hand stand-in|11|1895983|1|0|0|stand|0x0040bbb0|
|16|`0x004e99b0`|FUN_004e99b0|C2 with a source (45.5%)|11|367213|8|5|100|stop|0x004b3c60|
|17|`0x00402b70`|FUN_00402b70|c4=match bindable|11|25904|1|2|35|unbound|0x00495280|
|18|`0x00411d90`|Replay::CreateOrLoad|hand stand-in|11|25877|1|2|2|stand|0x0040d270|
|19|`0x004a504f`|FUN_004a504f|c4=match bindable|10|25876|9|5|7000|unbound|0x004a2b60|
|20|`0x004113b0`|FUN_004113b0|hand stand-in|10|25876|0|0|0|stand|0x00402750|
|21|`0x00402240`|ClumpGeom401fe0Create|hand stand-in|9|1885601|2|0|0|stand|0x004025f0|
|22|`0x00402590`|ClumpGeom4022a0Create|hand stand-in|9|1885601|2|0|0|stand|0x004025f0|
|23|`0x004cce20`|FUN_004cce20|c4=match bindable|9|23260|6|3|6294|unbound|0x004c32b0|
|24|`0x004568d0`|FUN_004568d0|C4 frida-replay with source|8|2201588|5|11|5641|unbound|0x0045bae0|
|25|`0x004e5850`|RpWorldCreate|C2 with a source (57.4%)|7|350454|5|0|0|unbound|0x00401000|
|26|`0x0047b8d0`|FUN_0047b8d0|c4=match cpp-unclean|7|73597|6|14|73611|stop|0x0047b9b0|
|27|`0x0041bec0`|FUN_0041bec0|hand stand-in|7|13300|0|0|0|stand|0x00402750|
|28|`0x0041d6e0`|FUN_0041d6e0|hand stand-in|7|13300|0|0|0|stand|0x00402750|
|29|`0x004275d0`|FUN_004275d0|hand stand-in|6|1986276|2|1|616|stand|0x00402750|
|30|`0x0041eaa0`|FUN_0041eaa0|hand stand-in|6|89649|1|1|66|stand|0x00402750|
|31|`0x004967e0`|FUN_004967e0|hand stand-in|6|31014|5|7|33843|stand|0x00492d20|
|32|`0x0041a1e0`|FUN_0041a1e0|hand stand-in|6|10264|2|0|0|stand|0x00402750|
|33|`0x0041d8b0`|FUN_0041d8b0|hand stand-in|6|10264|2|0|0|stand|0x00402750|
|34|`0x0041db90`|FUN_0041db90|hand stand-in|6|10264|2|0|0|stand|0x00402750|
|35|`0x004b6570`|FUN_004b6570|c4=match bindable|6|146|1|7|153|unbound|0x00495280|
|36|`0x004783f0`|FUN_004783f0|C2 with a source (90.8%)|5|335123|4|2|32|stop|0x00478660|
|37|`0x004a5f4a`|FUN_004a5f4a|C4 no bindable source (libid)|5|16965|1|1|86|unbound|0x004a2cbd|
|38|`0x00427880`|FUN_00427880|hand stand-in|5|10852|0|2|684|stand|0x00427ca0|
|39|`0x004e67b0`|RpAtomicCreate|c4=match bindable|4|1885003|2|3|1562822|unbound|0x004e69a0|
|40|`0x0045d7a0`|FUN_0045d7a0|C2 with a source (52.6%, in-situ IDENTICAL-FOR-2724)|4|308153|3|4|62216|stop|0x00466b50|


Class tally of the 140: hand stand-in 36, c4=match bindable 39, c4=match cpp-unclean 8, C2 with a source 17, C3 with a
source 14, C1 with a source 3, none with a source 1, C4 no bindable source 12, C4 frida-replay with source 3, no source 7.

## 4. Top 10 (staffing order)

1. `0x004111c0` FUN_004111c0, hand stand-in of the race tick (run-loop piece, caller 0x00492d30): 312 released, 721 behind.
2. `0x00492e90` FUN_00492e90, hand stand-in RenderFrame (run-loop piece, caller 0x00492290): 151 released, 114 behind.
3. `0x00429290` FUN_00429290, hand stand-in (boot init piece of 0x00402750): 40 released.
4. `0x0040d110` FUN_0040d110, RL stand-in under 0x0040d270: 36 released, 47 behind.
5. `0x00440bc0` FUN_00440bc0, c4=match cpp-unclean stop (COURSE.LUA handler): 30 released.
6. `0x00409710` FUN_00409710, c4=match cpp-unclean stop (C2632 char/bool): 21 released.
7. `0x004d8570` RwEngineRegisterStringFunctions, c4=match bindable, unbound: 19 released.
8. `0x0048ae00` FUN_0048ae00, hand stand-in: 18 released.
9. `0x00425bf0` FUN_00425bf0, c4=match cpp-unclean stop (C2664 PizFs_Mount arg): 17 released.
10. `0x00487280` FUN_00487280, no source (C2) stop: 15 released, 3.1 M T2 calls.

## 5. Files

- `verify/t2gaterank_20261121/t2_gate_rank.py` (the script), `t2_gate_rank.out` (its stdout), `T2_GATE_RANK.tsv` (all 140
  gates), `top40.md` (the generated table rows).
- `re/orchestrator/UNCERTAIN_REGISTER.md` +1 line.
- Regenerate: `py -3.12 verify/t2gaterank_20261121/t2_gate_rank.py`.
