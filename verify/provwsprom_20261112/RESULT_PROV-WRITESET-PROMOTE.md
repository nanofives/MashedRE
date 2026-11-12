# RESULT PROV-WRITESET-PROMOTE (run 39, 2026-11-12): L-128 frida-replay C4 promotions from the 114 write-set bodies

Inputs: `verify/provinsitu_20261111/WRITESET_TABLE.tsv` (PROV-WRITESET-114), `PROV_VERDICTS.tsv` (PROV-INSITU).
Scenario for every row: one race, track 0, QuickRace, 4 cars, 2,724 ticks, replay reference race, DDK body live in the
original through an inline JMP (`re/tools/prov/provlink.py`). Tracker path: HB-TRACKER-AT-MERGE is stage `claimed`, not
merged, so promotion goes through `c4_reclass.py batch` (route `frida-replay`), not a promotions TSV.

## Answer to the count question

The 22 caller-stack-only bodies sit inside the **45 escapes**, not inside the 69 covered (22 caller-stack only of 45 escapes).
By L-40 they stay C3 (or their current level) until a scenario makes them write to compared memory. No decision row.

## Counts

| class | bodies |
|---|---|
| 114 bodies in PROV-WRITESET-114 | 114 |
| covered (frame or image stores only, observed) | 69 |
| escapes | 45 |
| escapes: caller-stack only (L-40, stay) | 22 |
| escapes: other classes (heap or non-image static, image-masked; stay) | 23 |
| covered with at least one store into the compared image | 36 |
| covered with no store outside the frame in the stepped calls | 33 |
| **promoted to C4, `c4=frida-replay`** | **30** |
| L-54 exception (statically empty write-set, promoted on the clean replay alone) | 0 |
| byte-matched (`c4=match` / `c4=libid`) promoted here | 0 |

Why the cut is the 36 with image stores and not all 69 covered. L-40 says writers whose sampled calls write nothing
to compared memory stay C3 until a scenario makes them write there. The 33 covered bodies with no store beyond the
frame in the stepped calls have a static leg of `not-covered` (stores through registers or absolute addresses the
capstone pass could not bound), and the first 10 calls observed none outside the frame, so the compare never saw them
write compared memory. They stay where they are. The 36 with image stores wrote into `0x005ea000..0x00915008`
outside the masked blocks, which the per-tick hash compares, and that compare was IDENTICAL-FOR-2724.
The lead may re-cut: the 33 are listed below.

L-54: no body in the 114 has a statically empty write-set (the static leg was `not-covered` for all 114 by construction),
so the exception applies to none. Bodies with an empty static write-set were already promoted by PROV-PROMOTE-44.

install-check: ok (install column = ok in PROV_VERDICTS.tsv, inline JMP installed at each RVA)
body-executed: yes, stores observed in stepped calls inside the race window (calls_stepped > 0, calls_run > 0)
IDENTICAL-FOR-2724: per-tick compare of the PROV-INSITU runs against the 2,724-tick reference, verdict column of PROV_VERDICTS.tsv
write-set: covered (observed stores are frame or compared image only, image > 0; PROV-WRITESET-114 WRITESET_TABLE.tsv, L-38 / L-40 / L-54)

## Legs (all hold for the 30 promoted rows)

1. install check: `install` = ok in PROV_VERDICTS.tsv for all 114.
2. body executed: stores observed in stepped calls of the race window (`calls_stepped` > 0, `calls_run` > 0).
3. no STUB: the body has no row in `STUBS.md` (column 2 is not the body's RVA). 5 covered-with-image bodies have a STUB row and are excluded.
4. Write-set leg: observed stores are frame or compared image only (WRITESET_TABLE verdict `covered`, image > 0).
5. IDENTICAL-FOR-2724: compare tool = `re/tools/prov/prov_compare` per-tick hash of the PROV-INSITU runs, reference = the 2,724-tick
   replay reference race, N = 2724, result `verify/provinsitu_20261111/RESULT_PROV-INSITU.md` (verdict column of PROV_VERDICTS.tsv).
   Per-tick identity of the TF-stepped write-set runs was not re-scored in PROV-WRITESET-114 (stated there).

## Promoted (30)

| rva | level before | calls_stepped | stores | frame | image | calls_run (INSITU) |
|---|---|---|---|---|---|---|
| 0x00408610 | C2 | 10 | 210 | 70 | 140 | 10876 |
| 0x0041af50 | C2 | 10 | 300 | 230 | 70 | 10876 |
| 0x0041c410 | C3 | 10 | 520 | 210 | 310 | 10876 |
| 0x0045c030 | C2 | 10 | 60 | 30 | 30 | 1048 |
| 0x00467350 | C2 | 10 | 232 | 0 | 232 | 2719 |
| 0x004744a0 | C2 | 10 | 15670 | 7690 | 7980 | 118 |
| 0x00477f00 | C2 | 10 | 30 | 0 | 30 | 4559 |
| 0x0047a320 | C2 | 4 | 37 | 0 | 37 | 4 |
| 0x00408590 | C3 | 10 | 10 | 0 | 10 | 10673 |
| 0x0040b290 | C2 | 8 | 80 | 0 | 80 | 8 |
| 0x00410d10 | C2 | 10 | 20 | 10 | 10 | 2279 |
| 0x00412f30 | C2 | 10 | 420 | 350 | 70 | 2719 |
| 0x00413cb0 | C2 | 10 | 101 | 60 | 41 | 2719 |
| 0x00421d20 | C2 | 10 | 250 | 180 | 70 | 12 |
| 0x004252c0 | C2 | 10 | 510 | 350 | 160 | 2719 |
| 0x004704c0 | C2 | 10 | 580 | 50 | 530 | 16 |
| 0x004864f0 | C2 | 10 | 70 | 40 | 30 | 12 |
| 0x00415220 | C2 | 10 | 30 | 0 | 30 | 1917 |
| 0x00417180 | C3 | 10 | 27 | 0 | 27 | 5083 |
| 0x0041f590 | C2 | 10 | 153 | 60 | 93 | 10876 |
| 0x004212b0 | C2 | 10 | 2560 | 1280 | 1280 | 2719 |
| 0x00442440 | C2 | 10 | 110 | 90 | 20 | 886 |
| 0x00459620 | C2 | 10 | 400 | 230 | 170 | 699 |
| 0x0045cd30 | C2 | 1 | 87 | 81 | 6 | 1 |
| 0x0046cc40 | C2 | 10 | 1128 | 808 | 320 | 21596 |
| 0x0046da80 | C2 | 10 | 210 | 70 | 140 | 238 |
| 0x00490500 | C2 | 10 | 550 | 500 | 50 | 5438 |
| 0x00415880 | C2 | 10 | 30 | 10 | 20 | 4929 |
| 0x00486830 | C2 | 10 | 7020 | 6020 | 1000 | 2719 |
| 0x0048ab70 | C2 | 10 | 10 | 0 | 10 | 2719 |

## Covered with image stores, not promoted (6)

| rva | reason |
|---|---|
| 0x00420050 | STUB row in STUBS.md (leg 3) |
| 0x0040e590 | STUB row in STUBS.md (leg 3) |
| 0x00416250 | STUB row in STUBS.md (leg 3) |
| 0x00424b80 | STUB row in STUBS.md (leg 3) |
| 0x00499d90 | STUB row in STUBS.md (leg 3) |
| 0x004c4eb0 | write-set covers 3 stepped calls only, N=10 crashed with 0x80000004 (see below) |

## Covered, no store beyond the frame in stepped calls: stay (33)

`0x00454350`, `0x004809e0`, `0x004818d0`, `0x00491340`, `0x00405540`, `0x00422ba0`, `0x00459000`, `0x0045efe0`, `0x0047def0`, `0x0048d360`, `0x0048d540`, `0x004b42c0`, `0x004c4dc0`, `0x005ace70`, `0x005b3a00`, `0x004228f0`, `0x004568d0`, `0x00480d60`, `0x004c4a50`, `0x005ab410`, `0x005abd30`, `0x004694e0`, `0x00471ec0`, `0x0047d640`, `0x00485ef0`, `0x0048a780`, `0x0048b7c0`, `0x004c52f0`, `0x0040aef0`, `0x00412620`, `0x00414570`, `0x00414c30`, `0x00489500`


## Escapes: stay (45)

Caller-stack only (22): `0x00426810`, `0x004752f0`, `0x00475f90`, `0x00408b00`, `0x00420290`, `0x00441b30`, `0x00450b10`, `0x00463640`, `0x00477be0`, `0x004b5080`, `0x00546e70`, `0x005a8750`, `0x005aa7e0`, `0x00407e20`, `0x00420870`, `0x0047ba20`, `0x0047bb10`, `0x004b4550`, `0x005ab380`, `0x005acaa0`, `0x005b3760`, `0x005ac770`


Other escapes: `0x004967e0`, `0x00556e90`, `0x005ae920`, `0x00401da0`, `0x0047d3c0`, `0x0047eb30`, `0x00490020`, `0x004d40d0`, `0x004d8480`, `0x005a8890`, `0x005baf00`, `0x0045d7a0`, `0x0045dd60`, `0x00482140`, `0x005a9e60`, `0x005aa560`, `0x005ae250`, `0x005ae650`, `0x005a6ea0`, `0x005ae4c0`, `0x005ae800`, `0x005b0cf0`, `0x00488e70`


Note: 5 bodies in the escape or no-image sets already read C4 in hooks.csv (0x004d8480, 0x005a8750, 0x005aa7e0, 0x005ac770, 0x005ace70), reached by an earlier route. Not touched here.
