# B5e cluster plan — account2 worker draft output (2026-07-16), verified coverage 128+8+1=137


All data derived from `island_dag.tsv` edges + `hooks.csv` status + the two thunk files. No decomp disambiguation was needed — the TSV `internal_callees` edges fully determine every grouping, so no forward dependencies arise.

## DONE-ALREADY

Island members with `hooks.csv` status ∈ {ported, impl, verified} (excluded from clusters). 8 functions:

| rva | name | C-level | status |
|---|---|---|---|
| 004c3ac0 | Vec3Magnitude | C4 | verified |
| 004c3b30 | FastSqrt | C4 | verified |
| 004c45f0 | FUN_004c45f0 | C3 | impl |
| 004d7ff0 | RwIdentityPassthrough | C3 | impl |
| 004d8480 | RwErrSlotWrite | C3 | impl |
| 0055ac00 | RwpShapeActiveBitSet | C3 | ported |
| 0055c000 | RwpGjkSupportMap | C3 | ported |
| 0055e200 | RwpSolverContextSet | C3 | ported |

## INTRINSICS

| rva | name | note |
|---|---|---|
| 004a3440 | __chkstk / CrtStackProbe (61 B, C3 impl) | compiler intrinsic, not a port item |

## CLUSTER PLAN

Ordering: K1 pinned; then remaining leaves; then upward by dependency depth. **No cluster has any forward dep** — every cluster's island callees land in an earlier cluster or the same cluster (or DONE/INTRINSIC/K1). Caps: ≤~8 KB body, ≤15 fns (K12 is the mandated 10,500 B own-cluster; K1's 19 grandfathered).

| id | members (rva:size) | bytes | fns | earlier-cluster deps | fwd deps | indirect_calls=1 members |
|---|---|---|---|---|---|---|
| K1 | 005601f0:109, 00563e70:239, 00563f60:221, 00564040:187, 005641b0:351, 00564310:663, 00565120:60, 00565160:67, 005651b0:70, 00565200:94, 00565550:22, 00565ef0:171, 00565fa0:167, 00566200:411, 005667c0:109, 00566830:163, 005675d0:91, 005684c0:149, 00568560:143 | 3487 | 19 | — (pinned) | none | — |
| K2 | 004c4600:111, 004c51a0:323, 004c52f0:378, 00546b10:214, 00546bf0:93, 00546c50:93, 00546cb0:93 | 1305 | 7 | DONE | none | 004c4600, 004c52f0, 00546b10 |
| K3 | 0055a1f0:1957, 0055a9a0:397, 0055abb0:73, 0055ae50:27, 0055b750:173, 0055bae0:133, 0055bd70:14, 0055c0f0:318, 0055c2d0:175 | 3267 | 9 | K1 | none | 0055a1f0, 0055bae0, 0055bd70, 0055c2d0 |
| K4 | 005646c0:1465, 00564c80:1179, 00565260:747 | 3391 | 3 | K1, INTRINSIC | none | — |
| K5 | 0055ab30:117, 0055e050:44, 0055bb70:115, 0055bd80:85, 0055c230:154 | 515 | 5 | K2, K3, K4 | none | 0055bd80, 0055c230, 0055e050 |
| K6 | 0056bb80:337, 0056bce0:260, 0056bdf0:135, 0056be80:534, 0056c0a0:618, 0056c310:614 | 2498 | 6 | DONE | none | — |
| K7 | 0056c580:861, 0056c8e0:434, 0056caa0:1262, 0056cf90:216, 0056d070:724 | 3497 | 5 | — | none | — |
| K8 | 0056d350:158, 0056d3f0:2375, 0056dd40:2357, 0056ed60:463, 0056e680:1756 | 7109 | 5 | — | none | — |
| K9 | 0056ef30:133, 0056efc0:83, 0056f020:113, 0056f0a0:328, 0056f1f0:338, 0056fad0:186, 00567c00:49 | 1230 | 7 | — | none | — |
| K10 | 0056f350:1918, 0056fb90:770, 0056fea0:483 | 3171 | 3 | K2, K3, K9 | none | 0056f350 |
| K11 | 00567f00:1460, 00567c60:670, 005685f0:431, 00568dd0:505, 00568fd0:354 | 3420 | 5 | — | none | — |
| K12 | 00570090:10500 | 10500 | 1 | K1, K9, K10 | none | — |
| K13 | 00560260:3537 | 3537 | 1 | K1, K6, K7, K8, K9, K10, K12 | none | 00560260 |
| K14 | 00577be0:196, 00577cb0:513, 00577ec0:1497, 005784a0:366 | 2572 | 4 | — | none | — |
| K15 | 00576880:4958 | 4958 | 1 | K1, K14 | none | — |
| K16 | 005735f0:126, 00575120:208, 005751f0:181, 00576640:563, 00579b50:165, 00579c00:328, 00579d50:243, 00579e50:134, 00579ee0:866, 0057ae20:8 | 2822 | 10 | — | none | — |
| K17 | 00575b60:250, 00578b20:176, 00578bd0:213, 00578cb0:212, 00578d90:185, 0057a250:1038, 0057a660:824, 00575fe0:1632, 00578ff0:2899 | 7429 | 9 | K1, K2, K3, K16 | none | 00575fe0, 0057a660 |
| K18 | 005757d0:170, 00575c60:890, 00574ad0:1615, 00578610:1287, 0057a9a0:1035, 0056b7a0:546 | 5543 | 6 | DONE, K1, K2, K3, K5, K16, K17 | none | 00575c60, 0057a9a0 |
| K19 | 00578e50:105, 0057adb0:105, 005752b0:680 | 890 | 3 | K15, K18 | none | — |
| K20 | 00575880:732, 00575560:618 | 1350 | 2 | K3, K17, K19 | none | — |
| K21 | 00561280:265, 00568990:675, 005729a0:3138, 0056ba30:244, 0056bb30:67 | 4389 | 5 | K1, K2, K3, K5, K6, K15, K16, K18, K20 | none | 00561280, 005729a0 |
| K22 | 00573670:544, 0056b9d0:95 | 639 | 2 | K3, K5, K16, K18, K21 | none | — |
| K23 | 0055fe50:77, 0055fea0:203, 0055ff70:28, 0055ff90:603, 00561040:566, 00561390:2239, 00561c50:517, 00561e60:31, 00561e80:31 | 4295 | 9 | DONE, K1, K3, K5, K6, K11, K13, K21, K22 | none | 0055fe50, 00561040, 00561390 |
| K24 | 0047e9c0:118 | 118 | 1 | DONE, K23 | none | — |

### Per-cluster dependency notes (island-callee → owning cluster)

- **K2** (RW matrix/quat math): `00546b10`→`00546bf0/00546c50/00546cb0` (within) + `004c3b30`(DONE); `004c51a0`,`004c52f0`→`004d7ff0`,`004d8480`(DONE); `004c4600` leaf.
- **K3** (RWP-3.7 leaves): `0055a1f0`,`0055a9a0`→`00564040`(K1); `0055abb0`→`00565200`(K1); `0055ae50`→`00565550`(K1); rest leaves.
- **K4** (00564/565 builder): `00564c80`→`005646c0`(within) + `005641b0`/`00564310`/`00565120`/`00565160`/`005651b0`(K1); `005646c0`→`00563e70`(K1) + `004a3440`(INTRINSIC); `00565260`→`00563f60`(K1).
- **K5** (0055 L2/L3 helpers): `0055ab30`→`00564c80`,`00565260`(K4); `0055e050`→`0055ab30`(within); `0055bb70`→`004c4600`(K2),`0055bae0`(K3); `0055bd80`→`004c4600`(K2); `0055c230`→`0055c0f0`(K3).
- **K6** (hull group A): `0056be80`/`0056c0a0`/`0056c310`→`004c45f0`(DONE),`0056bb80`,`0056bce0`(within); `0056bb80`→`004c3b30`(DONE).
- **K7** (0056c group B): `0056caa0`→`0056c8e0`(within); `0056d070`→`0056cf90`(within).
- **K8** (0056d/e group C): `0056d3f0`→`0056d350`(within); `0056e680`→`0056ed60`(within).
- **K9** (0056e/f group D): `0056f0a0`→`0056f1f0`(within); `00567c00`→`0056ef30`(within).
- **K10** (group E): `0056f350`→`0055b750`(K3),`0056f0a0`/`0056f1f0`/`0056fad0`(K9); `0056fb90`→`00546b10`(K2).
- **K11** (00567/00568 leaves): `00567c60`→`00567f00`(within); rest leaves (incl. `005685f0`, island_callers=0).
- **K12** (`00570090`, own): →`005667c0`(K1),`0056f0a0`/`0056f1f0`(K9),`0056fb90`/`0056fea0`(K10).
- **K13** (`00560260`, own): 16-callee fan-in — `005601f0`/`005667c0`/`005675d0`(K1), `00567c00`/`0056efc0`/`0056f020`(K9), `0056bdf0`/`0056c310`(K6), `0056c580`/`0056caa0`/`0056d070`(K7), `0056d3f0`/`0056dd40`/`0056e680`(K8), `0056f350`(K10), `00570090`(K12).
- **K14** (0057 leaves P1): `00577cb0`→`00577be0`(within).
- **K15** (`00576880`, own): →`005667c0`(K1),`00577be0`/`00577cb0`/`00577ec0`/`005784a0`(K14).
- **K16** (0057 leaves P3): all leaves.
- **K17** (P4): `0057a250`→`00579c00`/`00579d50`/`00579e50`/`00579ee0`(K16); `0057a660`→`00579d50`/`0057ae20`(K16),`0055bd70`(K3); `00575fe0`→`004c4600`(K2),`0055bd70`(K3),`00566200`/`005667c0`(K1) + self-recursion; `00575b60`/`00578b20`/`00578bd0`/`00578cb0`→`0055c2d0`(K3); `00578ff0`→`00579b50`(K16); `00578d90`→`005667c0`/`00566830`(K1).
- **K18** (P5): `005757d0`→`0055c230`(K5); `00575c60`→`00575fe0`(K17),`004c4600`(K2); `00574ad0`→`00575120`/`005751f0`(K16); `00578610`→`00578b20`/`00578bd0`/`00578cb0`/`00578d90`(K17); `0057a9a0`→`0057a250`/`0057a660`(K17),`0055c000`(DONE); `0056b7a0`→`0055abb0`/`0055ae50`(K3),`0055bd80`(K5).
- **K19** (P6): `00578e50`→`005757d0`/`00578610`(K18); `0057adb0`→`005757d0`/`0057a9a0`(K18); `005752b0`→`00574ad0`(K18),`00576880`(K15).
- **K20** (P7): `00575880`→`00575b60`(K17),`00578e50`/`0057adb0`(K19),`0055c2d0`(K3); `00575560`→`005752b0`(K19),`00578ff0`(K17).
- **K21** (P8): `00561280`→`0055bb70`(K5); `00568990`→`00575560`/`00575880`(K20),`00575c60`(K18),`00576640`(K16),`0055abb0`(K3),`0055bd80`(K5); `005729a0`→`00561280`(within),`00575880`(K20),`00576880`(K15),`0056be80`(K6),`004c51a0`/`004c52f0`(K2),`0055bae0`(K3),`005735f0`(K16),`005667c0`(K1); `0056ba30`→`00568990`(within),`0056b7a0`(K18); `0056bb30`→`0056ba30`(within).
- **K22** (P9): `00573670`→`005729a0`(K21),`00575c60`(K18),`00576640`(K16),`0055abb0`(K3),`0055bd80`(K5); `0056b9d0`→`00573670`(within),`0056b7a0`(K18).
- **K23** (stage drivers): `0055fe50`→`0055a1f0`(K3); `0055fea0`→`00568990`(K21); `0055ff70`→`0056bb30`(K21); `0055ff90`→`00567c60`(K11); `00561040`→`00560260`(K13); `00561390`→`0055a9a0`(K3),`0055ab30`/`0055bb70`/`0055e050`(K5),`00561280`(K21),`00565ef0`/`00565fa0`(K1),`0056b9d0`/`00573670`(K22),`0056be80`/`0056c0a0`(K6),`004c3ac0`(DONE); `00561c50`→`0055ac00`(DONE); `00561e60`→`00568fd0`(K11); `00561e80`→`00568dd0`(K11).
- **K24** (`0047e9c0`, root): →`0055e200`(DONE) + the 9 K23 stage drivers.

## THUNK-RETIREMENT MAP

Grepped both files for `FUN_` RVAs that are island members. Only 5 of the ~24 thunks/externs target island RVAs; the rest (`0057c500/300/220/550`, `0055c810/4f0/4a0`, `0055bab0/b940/ae70/dec0`, `00482730/140`, `004c0b30`, `004e7e30`, `0047d240`, `00559c40`, `00426c00`, `00563810/940`) point at **non-island** RVAs and are **not** retired by this plan.

| thunk/extern (file:line) | RVA | retired by |
|---|---|---|
| `FUN_004c52f0` — RwpBuildExterns.cpp:62 | 004c52f0 | **K2** |
| `FUN_004c51a0` — RwpBuildExterns.cpp:63 | 004c51a0 | **K2** |
| `FUN_00546b10` — RwpBuildExterns.cpp:64 | 00546b10 | **K2** |
| `FUN_0047e9c0` — RwpBuildExterns.cpp:65 | 0047e9c0 | **K24** |
| `FUN_0055c000` — CollisionBuildDeps.h:62 (extern) | 0055c000 | already retired by **DONE** (RwpGjkSupportMap, C3 ported) |

## COVERAGE CHECK

Cluster members (by count):
```
K1  19   K7   5   K13  1   K19  3
K2   7   K8   5   K14  4   K20  2
K3   9   K9   7   K15  1   K21  5
K4   3   K10  3   K16 10   K22  2
K5   5   K11  5   K17  9   K23  9
K6   6   K12  1   K18  6   K24  1
```
Sum of clusters = 19+7+9+3+5+6+5+5+7+3+5+1+1+4+1+10+9+6+3+2+5+2+9+1 = **128**

**128** (clustered) **+ 8** (DONE-ALREADY) **+ 1** (INTRINSIC `004a3440`) = **137** ✓ = TSV member count (rows 2–138).
(raw JSON: C:\Users\maria\AppData\Local\Temp\fleet-delegate-e9e2b1294b724e0f935cbbb2fa379f61.json)
