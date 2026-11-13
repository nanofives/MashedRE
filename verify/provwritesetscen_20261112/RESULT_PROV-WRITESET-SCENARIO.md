# RESULT PROV-WRITESET-SCENARIO (run 40, 2026-11-12): race-path write-set leg for the 33 covered bodies with no compared store

Verdict: **GREEN.** All 33 bodies of PROV-WRITESET-PROMOTE's "covered, no store beyond the frame" list were re-stepped
over the whole race (T2 scenario), not only their first 10 calls. **8 now store into the compared image** with every
observed store frame or image, in a stepped race that is itself IDENTICAL-FOR-2724 against an unstepped control. They
go to C4 by `frida-replay` through `re/promotions/PROV-WRITESET-SCENARIO.tsv` (dry run OK, below). The other 25 stay:
17 still store into the frame only over the race sample, 5 are now `escapes` (stores outside compared memory found
later in the race), 1 stores into the image but has an open STUB row, 2 never run after the race anchor (non-T2).
The L-54 exception is unchanged and applies to none of the 33.

## Legs for the 8 promoted bodies

install-check: ok (inline JMP at the RVA lands on the stub and the stub's jmp on the DDK body entry, `prov_install.json` jmp_ok of each cited run)
body-executed: yes, 182 to 10,509 race calls single-stepped per body (`calls_stepped` in SCEN_TABLE.tsv), all after the race anchor
IDENTICAL-FOR-2724: PROV-INSITU per-tick compare with the body live, verdict column of `verify/provinsitu_20261111/PROV_VERDICTS.tsv` (reference = the 2,724-tick replay reference race)
write-set: covered (every observed store in the race sample is frame or compared image, image > 0; SCEN_TABLE.tsv, L-38 / L-40)

New here: each cited stepped run is also IDENTICAL-FOR-2724 against an unstepped control of the same harness
(`VS_CONTROL.txt`), so the stores were observed in the reference race, not a perturbed one.

| rva | name (hooks.csv) | level | stride | evidence run | calls stepped | stores | frame | image | image targets (first) |
|---|---|---|---|---|---|---|---|---|---|
| 0x00422ba0 | FUN_00422ba0 (ai) | C2 | 2 | wsc_s_00422ba0 | 1360 | 10947 | 10707 | 240 | 0x008995b0.. |
| 0x004568d0 | FUN_004568d0 (gameplay) | C2 | 2 | wsa_g03 | 1360 | 25563 | 24865 | 698 | see table |
| 0x004809e0 | VehicleRespawnTeleport (vehicle) | C2 | 2 | wsd_g06a | 5314 | 9 | 7 | 2 | 0x008822b8, 0x008822bc |
| 0x004818d0 | FUN_004818d0 (vehicle) | C2 | 1 (every race call) | wsc_s_004818d0 | 10509 | 1644 | 1236 | 408 | 0x006c943c.. |
| 0x00485ef0 | FUN_00485ef0 (particle) | C2 | 15 | wsa_g01 | 182 | 35788 | 33046 | 2742 | see table |
| 0x0048b7c0 | FUN_0048b7c0 (particle) | C2 | 8 | wsa_g04 | 340 | 1645 | 1615 | 30 | 0x0071fa4c.. |
| 0x0048d360 | FUN_0048d360 (particle) | C2 | 4 | wsc_s_0048d360 | 391 | 45508 | 45346 | 162 | 0x00722370.. |
| 0x0048d540 | FUN_0048d540 (particle) | C2 | 4 | wsa_g04 | 570 | 30617 | 30493 | 124 | 0x00722390.. |

No STUB row names any of the 8 in column 2 (open rows only).

## Why the first sample missed them

PROV-WRITESET-114 stepped the first 10 calls of each body. For bodies called once per race tick (calls_run 2719) those
are the first 10 ticks of the race (countdown), where these bodies take frame-only paths. Over the whole race the 8 above
reach their image stores. This answers L-139: for these 8 a T2 scenario (the replay reference race itself, sampled past
its first ticks) makes them write compared memory.

## Method

- `re/frida/js/detorig.js` stub, new mode (`--prov-wstride <json>`): `dec [kd]; jnz skip; mov [kd], K; cmp [ws], N; jae skip;
  inc [ws]; pushfd; or [esp], 0x100; popfd`. It steps calls 1, K+1, 2K+1, ... up to N per body. K per body
  (`wstride.json`, `STRIDE_PLAN.tsv`) = ceil(steps per call x calls / 150,000) from the PROV-WRITESET-114 sample, so
  each body gets about 150k stepped instructions spread over the race. 13 bodies have K = 1 (every call).
- `--prov-warm anchor`: a cmp on an arm word in front of the stride gate; `provSnap()` at the race anchor sets it.
  No call is stepped during the load. Reason, measured: stepping from install (first try, `wss_g04`) shortened the load
  (dn_warp 110 against 118 on the unstepped control `wss_ctl_g04`), and that race diverged from the references at tick 0
  in 28 to 30 blocks. With the arm, dn_warp is 118 to 120, the unstepped range.
- `--prov-wcap`: write-log capacity (default 2,097,152 records). Group g06 overflowed it (0x00489500 ran 2,080,130
  steps in 214 calls), so g06 was re-run split (g06a, g06b). A larger allocation (8,388,608 records) changes one block,
  0x00912800, on every tick, so the g06a evidence run (`wsd_g06a`) uses the default capacity.
- Decode: `re/tools/prov/prov_writeset.py` unchanged (same classes as PROV-WRITESET-114, allowed blocks from PROV-INSITU
  ref_a / ref_b).
- Per-tick check of each stepped run: `re/frida/prov_compare.py <run> wss_ctl_g04 wss_ctl_g04 --pair r1,r2 --pair x_ref,x_ref2`
  (same masks and allowed blocks as PROV-INSITU, the unstepped control as reference). `tickscore.py` gives the block-level
  view against r1.
- Every launch: `test-slot` outside, `mashed_lock.py run` inside, private root `D:\MashedWT\PROV-WRITESET-SCENARIO_root`
  (robocopy of `original\`, MASHED.exe SHA-256 equal to the main install). Scripts: `run_wss.sh`, `batch.sh`,
  `aggregate.sh`, group files `groups*.txt`.

## All 33 (`SCEN_TABLE.tsv`, one row per body)

| class | n | bodies |
|---|---|---|
| image, promote | 8 | listed above |
| image, open STUB (leg 3), stays | 1 | 0x0047def0 (S-3728) |
| escapes, stays (L-40) | 5 | 0x004c52f0 (caller-stack 3184, other 24672), 0x00414570 (caller-stack 17), 0x005b3a00 (caller-stack 4), 0x004c4dc0 (caller-stack 34720, image 17830), 0x0047d640 (other 261) |
| frame-only over the race sample, stays C2/C3 | 17 | 0x00405540, 0x0040aef0, 0x00412620, 0x00414c30, 0x004228f0, 0x00454350, 0x00459000, 0x0045efe0, 0x004694e0, 0x00471ec0, 0x00480d60, 0x00489500, 0x0048a780, 0x00491340, 0x004b42c0, 0x004c4a50, 0x005ace70 |
| not run after the race anchor (non-T2), stays | 2 | 0x005ab410, 0x005abd30 (all their calls are in the load) |

Notes on the stay rows:
- 0x005ace70 already reads C4 in hooks.csv (earlier route, noted in PROV-WRITESET-PROMOTE). Not touched.
- 0x004c4dc0 / 0x004c4a50 / 0x004c52f0 / 0x005ab410 read C3; the rest C2. None changes level here.
- Of the 17 frame-only bodies, 6 were stepped on every race call (stride 1): 0x00405540 (1 call), 0x00412620,
  0x0045efe0, 0x00471ec0, 0x00480d60, 0x00491340. For them no race call stores outside the frame. The other 11 are a
  stride sample (every K-th call), so an unsampled call can still store elsewhere.
- 0x0047def0 stores 2184 times into the image and nowhere else outside the frame, so only leg 3 (S-3728 open) holds it.

## Stepped runs against the unstepped control (`VS_CONTROL.txt`)

| run | bodies | verdict vs control |
|---|---|---|
| wsa_g01, wsa_g03, wsa_g04, wsa_g06, wsa_g08, wsd_g06a, wsc_s_0048d360, wsc_s_00422ba0, wsc_s_004818d0 | evidence for all 8 promoted | IDENTICAL-FOR-N 2724 of 2724 |
| wsa_g00 | 4 bodies (no promotion) | DIVERGED at tick 1 (car-record blocks 0x00881c00 for 4 ticks, later 0x00692400, 0x00883800) |
| wsa_g02, wsb_g02 | 4 bodies | DIVERGED at tick 1046, both runs (deterministic) |
| wsa_g05, wsb_g05 | 4 bodies | DIVERGED at tick 1007, both runs (deterministic) |
| wsa_g07 | 4 bodies | DIVERGED at tick 932 |

Stepping changes the race in some groups at a repeatable tick. Cause not found. Two of the three image writers in
diverging groups (0x0048d360, 0x00422ba0 from g05, 0x004818d0 from g02) were run one per process: each single run is
IDENTICAL-FOR-2724 against the control with the same store counts as in its group, so the cause sits in the other
bodies of those groups (0x004694e0 / 0x0047d640 for g05, 0x00414c30 / 0x0047def0 / 0x005ace70 for g02).
[UNCERTAIN] Which body and which mechanism (no measurement isolates it). It does not touch the 8 promoted rows.
The stay rows from diverging runs (g00, g02, g05, g07) were observed in a race that differs from the control after the
tick named; their class (frame-only, escapes) is from that run.

The control itself (`wss_ctl_g04`) is not identical to the 11-11 references r1 / r2: one raw block, 0x00773c00
(dword 0x006147d8 = 1 against 0), differs on every tick. This is harness drift between 11-11 and today on the
unstepped path, so the stepped runs are measured against the control rather than r1 / r2. [UNCERTAIN] origin of
0x006147d8, not investigated (no body under test is live in the difference: the control has bodies live, unstepped,
and the r1 / r2 references none).

## Limits

- The IDENTICAL-FOR-2724 leg is PROV-INSITU's (body live, no stepping), as in PROV-WRITESET-PROMOTE. The new per-tick
  check shows the stepped evidence runs equal the unstepped control; it is not a new reference race.
- Stride sampling: bodies with K > 1 are observed on every K-th race call only.
- Stores by other threads are not logged (0 to 4 other-thread steps per run, cleared without logging).
- Same 4 MB frame / caller-stack window as PROV-WRITESET-114.
- Not run, by row kind (replay-oracle tooling and evidence, no default-path change): builds, `ddkobj.py check`, G-CTRL, B.
  No file under `mashedmod/` changed.

## Promotion

`re/promotions/PROV-WRITESET-SCENARIO.tsv` (8 rows, route `frida-replay`, result = this file). Dry run:
`py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/PROV-WRITESET-SCENARIO.tsv` (no `--apply`), output in
`c4_dry.txt`. The merge train applies it. Counts: replay-verified (`c4=frida-replay`) +8, byte-matched +0.

## Evidence

Committed (this folder): `SCEN_TABLE.tsv`, `STRIDE_PLAN.tsv`, `wstride.json`, `VS_CONTROL.txt`, `RAW_SHA256.tsv`,
`c4_dry.txt`, `tickscore.py`, `run_wss.sh`, `batch.sh`, `aggregate.sh`, `groups.txt`, `groups_rerun.txt`,
`groups_rerun2.txt`, `groups_singles.txt`, `groups_g06a.txt`.
Tools changed: `re/frida/js/detorig.js` (stride stub, arm word, write-log capacity), `re/frida/detorig.py`
(`--prov-wstride`, `--prov-warm`, `--prov-wcap`). Default behaviour of both is unchanged without the new flags.
Raw (not committed): `D:/MashedWT/PROV-WRITESET-SCENARIO_data/` (`wss_ctl_g04`, `wss_g00..g02`, `wss_g04`, `wsa_g00..g08`,
`wsb_g02`, `wsb_g05`, `wsb_g06a`, `wsb_g06b`, `wsc_s_*`, `wsd_g06a`: `wlog.bin`, `ticks.bin`, `prov_install.json`,
`meta.json`; per-run decode `*.tsv`, `*.sites.tsv`). SHA-256 of each cited raw file in `RAW_SHA256.tsv`.
