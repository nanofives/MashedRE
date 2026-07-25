# B5e per-field C4 campaign + A3 audit prep (2026-07-24)

## Part 1 ÔÇö Lane-end per-field C4 campaign for K1..K24

### Acceptance model (from the sources, not run_diff)

Per `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md` ┬º0/┬º4: lane-end acceptance is a **whole-loop per-field body-state bit-identity diff** (the deferred B5d ┬º4 diff, commit 08595848) ÔÇö a canonical bridge-driven QuickRace with the port installed (inline-JMP live), per-field comparing the proxy body state via `body = FUN_0057c210((&DAT_006c9a78)[i])`. **Linear fields must be bit-identical (ndiff=0); angular fields carry the accepted Ôëñ1-ULP x87 floor** (`project_phys_chain_float10_methodology`), tracked as **U-9020** (K7 note, line 289: "deferred-associativity = U-9020 ÔÇª to settle at the lane-end per-field diff"). This is the same in-process self-test pattern already proven on the B5c subset (┬º`0055b800` row: snapshot 0x40 matrix slot + 0x10 quat slot ÔåÆ `HookSystem::Uninstall/Install` original ÔåÆ restore ÔåÆ run port ÔåÆ per-32-bit-field compare; 64/64 ndiff=0 GREEN).

**Consequence:** the K1..K24 members are internal `__cdecl` solver functions (contiguous-stack-buffer ABIs, mixed-type struct params, float10 ST0 returns). They are **not** in the `run_diff.py` registry and cannot be promoted per-function through it ÔÇö the per-field whole-loop self-test is the harness for every one of them.

### B5c integrator subset ÔÇö DONE (C4, 2026-07-24)

Six fns reached C4 today via the in-process self-test (`log/phys_c4_b5c_ALL6_GREEN_20260724.log`, `0055b800` earlier via `log/phys_c4_b5c_matrixrefresh_GREEN.log`). These are the integrator dependency ("B5c8"), not numbered K-clusters; two of them (`0055ac00`, `0055e200`) also appear in the island ┬º3 DONE-ALREADY list.

| RVA | name | C-level | evidence |
|---|---|---|---|
| 0057c210 | RwpBodyTableLookup | **C4 Ô£à** | phys_c4_b5c_ALL6_GREEN_20260724.log |
| 0055ac00 | RwpShapeActiveBitSet | **C4 Ô£à** | phys_c4_b5c_ALL6_GREEN_20260724.log |
| 0055b800 | RwpBodyMatrixRefresh | **C4 Ô£à** | phys_c4_b5c_matrixrefresh_GREEN.log |
| 0055deb0 | RwpWorldSolverHandle | **C4 Ô£à** | phys_c4_b5c_ALL6_GREEN_20260724.log |
| 0055dff0 | RwpBodyRefreshGate | **C4 Ô£à** | phys_c4_b5c_ALL6_GREEN_20260724.log |
| 0055e200 | RwpSolverContextSet | **C4 Ô£à** | phys_c4_b5c_ALL6_GREEN_20260724.log |

### K1..K24 ÔÇö all C2, awaiting lane-end per-field C4

Per B5e ┬º5 (`K24` hooks.csv row line 1789: "B5e solver-island port COMPLETE (K1..K24 all C2)"). Cluster-level table (per-fn `run_diff` arg_type is **N/A** for all ÔÇö see acceptance model; the only per-fn distinction that matters is the float10-ST0 blocker, itemised in the blocker column). C-level = C2 for every member unless noted.

| K | members (RVA) | #fns | arg_type status | blocker | batch |
|---|---|---|---|---|---|
| K1 | 005601f0 00563e70 00563f60 00564040 005641b0 00564310 00565120 00565160 005651b0 00565200 00565550 00565ef0 00565fa0 00566200 005667c0 00566830 005675d0 005684c0 00568560 | 19 | N/A (whole-loop) | 005667c0 = float10 ST0 (┬º4 note 4; K17 line 494) ÔåÆ no 80-bit ST0 run_diff handler; folded into whole-loop | B1 |
| K6 | 0056bb80 0056bce0 0056bdf0 0056be80 0056c0a0 0056c310 | 6 | N/A | x87 FSIN/FCOS + FSQRT (line 253); whole-loop | B1 |
| K7 | 0056c580 0056c8e0 0056caa0 0056cf90 0056d070 | 5 | N/A | deep interleaved x87 sums ÔåÆ **U-9020** carve-out (lines 287-289) | B1 |
| K8 | 0056d350 0056d3f0 0056dd40 0056ed60 0056e680 | 5 | N/A | dd40 = SSE2 twin (needs SSE-state compare, not ST0); U-9020 (line 323); whole-loop | B1 |
| K9 | 0056ef30 0056efc0 0056f020 0056f0a0 0056f1f0 0056fad0 00567c00 | 7 | N/A | fad0 float body only; rest integer; whole-loop | B1 |
| K11 | 00567f00 00567c60 005685f0 00568dd0 00568fd0 | 5 | N/A | whole-loop | B1 |
| K14 | 00577be0 00577cb0 00577ec0 005784a0 | 4 | N/A | 00577ec0 calls float10 005667c0 (line 462); whole-loop | B1 |
| K16 | 005735f0 00575120 005751f0 00576640 00579b50 00579c00 00579d50 00579e50 00579ee0 0057ae20 | 10 | N/A | **005751f0, 00579d50 = float10 ST0** (lines 472-473) ÔåÆ no 80-bit handler | B1 |
| K2 | 004c4600 004c51a0 004c52f0 00546b10 00546bf0 00546c50 00546cb0 | 7 | N/A | RwMatrix module-slot indirect; whole-loop | B2 |
| K3 | 0055a1f0 0055a9a0 0055abb0 0055ae50 0055b750 0055bae0 0055bd70 0055c0f0 0055c2d0 | 9 | N/A | vtable/body-obj dispatch (routes to REAL tables pre-KV); whole-loop | B2 |
| K4 | 005646c0 00564c80 00565260 | 3 | N/A | whole-loop | B2 |
| K10 | 0056f350 0056fb90 0056fea0 | 3 | N/A | whole-loop | B2 |
| K12 | 00570090 | 1 | N/A | float10 fVar19/20 = FLT_MAX/-FLT_MAX artifacts (ported as consts, lines 425-430); U-9020; whole-loop | B2 |
| K15 | 00576880 | 1 | N/A | x87 verbatim; float10 area; whole-loop | B2 |
| K17 | 00575b60 00575fe0 00578b20 00578bd0 00578cb0 00578d90 00578ff0 0057a250 0057a660 | 9 | N/A | **00578b20/bd0/cb0, 0057a660 = float10 ST0** (lines 489-492) ÔåÆ no 80-bit handler | B2 |
| K5 | 0055ab30 0055e050 0055bb70 0055bd80 0055c230 | 5 | N/A | obj/desc indirect dispatch; whole-loop | B3 |
| K13 | 00560260 | 1 | N/A | KV callbacks reproduced as by-value structs (Kv11/Kv13Frame); whole-loop | B3 |
| K18 | 005757d0 0056b7a0 00574ad0 00575c60 00578610 0057a9a0 | 6 | N/A | **00574ad0/00578610/0057a9a0 = float10** (lines 515-517); U-9020; whole-loop | B3 |
| K19 | 00578e50 0057adb0 005752b0 | 3 | N/A | whole-loop | B3 |
| K20 | 00575880 00575560 | 2 | N/A | whole-loop | B4 |
| K21 | 00561280 00568990 005729a0 0056ba30 0056bb30 | 5 | N/A | whole-loop | B4 |
| K22 | 00573670 0056b9d0 | 2 | N/A | whole-loop | B4 |
| K23 | 0055fe50 0055fea0 0055ff70 0055ff90 00561040 00561390 00561c50 00561e60 00561e80 | 9 | N/A | 3 indirect dispatch; whole-loop | B4 |
| K24 | 0047e9c0 | 1 | N/A (SIM-HEALTH only, line 1789) | root world-step; retires RwpBuildExterns thunk; whole-loop | B4 |

**128 members total** (matches ┬º3: 128 clustered + 8 DONE + 1 `__chkstk` = 137).

### arg_type finding (ARG_TYPES.md, do-not-open-diff_template confirmed)

- **No 80-bit x87 ST0 float-return handler exists.** The closest handlers (`float_scalar` L224, `float3_scalar_ret` L232, `float_2ptr_ret` early-window L160) compare the return **as a 32-bit u32 bit pattern**, not an 80-bit long double. So any per-function `run_diff` on the float10-ST0 fns listed above (K1 005667c0; K16 005751f0/00579d50; K17 00578b20/bd0/cb0/0057a660; K18 area/SAT/TOI drivers) would be blocked. **Mitigation is structural, not a new handler:** these ST0 values are x87-chain intermediates; the final body-state fields are float32, so the whole-loop per-field diff sidesteps them (angular residual ÔåÆ U-9020).
- **Other "new handler" shapes** that a per-fn path would need but the whole-loop diff avoids: SSE2 lane-state compare (K8 dd40), by-value KV-frame struct compare (K13). None are needed for the lane-end diff.

### Batch plan (account3 Frida sessions ÔÇö leaf-first, dependency order)

All batches use the **canonical-scenario in-process per-field body-state self-test** (B5c precedent, `MASHED_PHYS_C4_SELFTEST`-style, extended to the whole loop) ÔÇö **not** `run_diff.py`. GREEN = linear fields ndiff=0, angular within Ôëñ1-ULP (U-9020). Order follows ┬º3 cluster deps ("no cluster has a forward dep"); attribution on RED is by toggling `RH_ScopedInstall` clusters (bisection, as used all lane).

- **B1 ÔÇö leaf clusters (deps `ÔÇö`/`DONE`):** K1, K6, K7, K8, K9, K11, K14, K16. Establish the per-field diff harness + baseline GREEN with only leaves installed.
- **B2 ÔÇö layer-2 (deps into B1/DONE):** K2, K3, K4, K10, K12, K15, K17.
- **B3 ÔÇö layer-3:** K5, K13, K18, K19.
- **B4 ÔÇö layer-4 (deepest deps) + root:** K20, K21, K22, K23, K24. K24 root closes the loop; retires the `FUN_0047e9c0` RwpBuildExterns thunk.
- **Final whole-loop GREEN run:** full 136-hook canonical race (bridge + B5c8 + K1..K24 root) with per-field diff live ÔÇö the single artifact that promotes the lane C2ÔåÆC4. Precedent install-observe already at 135/136 (sole miss 0x5752b0 flake, line 1789).

**Prerequisite note:** per-field standalone *truth* also needs KV1..KV3 (┬º3 lines 122-129) ÔÇö the island reads REAL vtables via absolute address until KV lands, so `.asi` A/B per-field diff is valid now, but standalone-exe per-field parity is gated on KV.

---

## Part 2 ÔÇö A3 audit prep (16 C4 flags across T1.2/T1.3 = 9 distinct rows)

Source: `re/analysis/plans/tracker_health_2026-07-24.md` T1.2 (7 rows C4 with early-stage status) + T1.3 (9 rows C4 with blank `frida_diff` pointer). The health doc flags the **blank pointer column**, not a bad on-disk path ÔÇö so verdicts below hinge on whether the artifact cited in the row body exists.

| RVA | name | flag(s) | claimed evidence (row body) | verdict | path |
|---|---|---|---|---|---|
| 00404320 | PerModeRenderMachine | T1.3 | R1-B install-observe (hook 0xE9 live, 25s survival) | **EVIDENCE-FOUND** | `log/install_observe_r1b_20260609.txt` (batch 5, line 24) |
| 004c5800 | RwTexDictionarySetCurrent | T1.2+T1.3 | R1-B install-observe | **EVIDENCE-FOUND** | `log/install_observe_r1b_20260609.txt` (batch 5, line 25) |
| 004c5820 | RwTexDictionaryGetCurrent | T1.2+T1.3 | R1-B install-observe | **EVIDENCE-FOUND** | `log/install_observe_r1b_20260609.txt` (batch 5, line 25) |
| 00431ae0 | FUN_00431ae0 | T1.2+T1.3 | R1-B install-observe | **EVIDENCE-FOUND** | `log/install_observe_r1b_20260609.txt` (batch 6, line 27) |
| 00431af0 | FUN_00431af0 | T1.2+T1.3 | R1-B install-observe | **EVIDENCE-FOUND** | `log/install_observe_r1b_20260609.txt` (batch 6, line 27) |
| 00431b00 | FUN_00431b00 | T1.2+T1.3 | R1-B install-observe | **EVIDENCE-FOUND** | `log/install_observe_r1b_20260609.txt` (batch 6, line 27) |
| 00492770 | MainLoopInit | T1.3 | R1-B install-observe | **EVIDENCE-FOUND** | `log/install_observe_r1b_20260609.txt` (batch 6, line 27) |
| 0040b6d0 | FUN_0040b6d0 | T1.2+T1.3 | c3_batch_ac: "Frida bit-identical GREEN run_diff_warm + integration 8/8" | **MISSING** | no artifact on disk ÔÇö only prompt `log/batches/c3_batch_ac.txt`; not in install_observe_r1b; no run_diff_warm log exists |
| 0046bce0 | FUN_0046bce0 | T1.2+T1.3 | c3_batch_ad: "run_diff_warm + integration 12/12"; `frida_diff`=`batch-y-s2` (tag) | **MISSING** | no artifact on disk ÔÇö only prompt `log/batches/c3_batch_ad.txt`; `batch-y-s2` is a batch tag, not a path |

### Notes / caveats for account3

- **7 EVIDENCE-FOUND** all point to the same file, `log/install_observe_r1b_20260609.txt` (verified: file exists; grep confirms each RVA present in batch 5 / batch 6). **Fix = repoint the blank `frida_diff` column to that path**; the artifact was never lost, only unwired. Caveat: this file is **install-observe survival** (inline-JMP live + manifest installed=1 + 25 s no-crash), *not* a per-field behavioral diff ÔÇö the rows themselves note "canonical DIFF still pre-fix ÔÇª C4 RETAINED pending ÔÇª re-validation." If A3 wants diff-grade C4, these need a fresh installed-hook canonical diff, not just repointing.
- **2 MISSING** (0040b6d0, 0046bce0): the cited `run_diff_warm`/integration-build evidence was never saved as a standalone artifact (only the batch *prompt* files survive). **account3 must re-run** the diff to produce a citable log before these hold at C4.
- No row classifies as **STALE** ÔÇö the health doc flags blank pointer *columns*, and none of the on-disk paths that do exist mismatch their claim.
- T1.2/T1.3 status-vocab fixes (`mapped`/`new` ÔåÆ `verified` or `impl`) are orthogonal to evidence and route through `re-classify` on account3.

**Read-only run ÔÇö no files were modified. All fixes (frida_diff repoint, MISSING re-runs, status normalisation) route through the `re-classify` skill on account3.**
