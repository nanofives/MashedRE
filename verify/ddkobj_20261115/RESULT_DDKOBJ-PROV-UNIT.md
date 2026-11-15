# RESULT DDKOBJ-PROV-MIXED-UNIT + DDKOBJ-PROV-SHARED-UNIT (GREEN)

Row kind: top-down tool change (`re/tools/ddkobj/ddkobj.py` only). Base: `orch/integration` 38a6f3ae5.
Self-test: `py.exe -3.12 re/tools/ddkobj/ddkobj.py selftest` OK, 34 PASS, 0 failures (3 new checks).

## Problem

`main_build` skipped a unit as `skip:provisional` only when every MATCH rva of the unit was provisional. A provisional
body that shares its source with a linked function was DDK-compiled, linked and byte-checked in the object build,
against L-126 (6a) (a provisional body runs as its VS2022 copy). The base object build linked 7 such provisional rvas,
all DIFFERENT, all with `vs_copy_in_map False`: 0x00401690, 0x0043c5b0, 0x0045cd30, 0x00498510, 0x00498810, 0x004c0c20,
0x004c0f00. Two of them (0x00401690, 0x00498810) were not in the known list.

## Option chosen: per-function skip (not a source split)

Rule now in `process_unit`: a MATCH function whose rva is in `PROVISIONAL.tsv` (or BINDINGS kind `provisional`) is
not a port match. Its DDK copy is handled like a port-excluded helper: renamed to a dead local, every in-object
reference repointed to the exe name, so the exe keeps the VS2022 body and the byte check never sees it. This applies
to static DDK copies too (a static copy left in place would still run from the unit's linked functions). A static copy
whose DDK convention is private is refused by the existing `static_convention_ok` check, then by the existing
forwarder fallback, so the unit keeps its VS2022 bodies (`fail:externalized-static-register-convention`). A unit where
every rva is provisional is still `skip:provisional`. `exe_names` no longer lists provisional names. `check` adds
`provisional_functions_vs2022_in_linked_units` to `ddkobj_summary.json` and to the summary line.

Why not a split. For the 5 units that link, the per-function skip works and leaves the source untouched. For the 2 units
where the provisional function is a static with a DDK-private convention, the tool's own tests show a split cannot keep
the matched function byte-identical: compiling the provisional function non-static changes it ("bytes differ when
non-static", 1280 vs 1296 B for 0x00401690), and the generated forwarder (the external call a split would produce)
changes the matched caller's bytes ("the forwarder changes _FUN_004025f0's bytes", "the forwarder changes
_FUN_00498860's bytes"). Source: `fail:externalized-static-register-convention` evidence in the AFTER
`ddkobj_build.json`, quoted in `bytecheck_delta.txt`. Splitting also edits byte-matched sources, which this row does
not own. Not tested as a separate arm: [UNCERTAIN] whether a split with the provisional body moved to its own file
keeps 0x004025f0 / 0x00498860 at 100%; the forwarder result above is the nearest test.

## Object build, BEFORE (38a6f3ae5) and AFTER (this branch), same base, same machine

```
BEFORE [ddkobj summary] IDENTICAL 1547/1590  DIFFERENT 28  not-in-map 15  units linked 1533/1741  refusals 9
       (externalized-static-register-convention=5, wrapper-redefines-globals=4)  skip:provisional 199 units
AFTER  [ddkobj summary] IDENTICAL 1545/1581  DIFFERENT 21  not-in-map 15  units linked 1531/1741  refusals 11
       (externalized-static-register-convention=7, wrapper-redefines-globals=4)
       provisional kept VS2022 199 units + 5 fns in linked units
```

Per-row delta (`bytecheck_delta.txt`), every change explained:

| rva | BEFORE | AFTER | reason |
|---|---|---|---|
| 0x0043c5b0 | DIFFERENT | VS2022 body | provisional, externalized, unit links 0x0042aad0 (DDK) |
| 0x0045cd30 | DIFFERENT | VS2022 body | provisional, externalized, unit links 0x0045c820 (DDK) |
| 0x00498510 | DIFFERENT | VS2022 body | provisional, externalized, unit links 0x00497190 + 0x004971b0 (DDK) |
| 0x004c0c20 | DIFFERENT | VS2022 body | provisional, externalized, unit links 0x004c0ad0 (DDK) |
| 0x004c0f00 | DIFFERENT | VS2022 body | provisional, externalized, unit links 0x004c0ad0 (DDK) |
| 0x00401690 | DIFFERENT | VS2022 body | provisional static, DDK-private convention: unit refused |
| 0x004025f0 | IDENTICAL | VS2022 body | same unit as 0x00401690 (re/match/s01/00401690.c), refused |
| 0x00498810 | DIFFERENT | VS2022 body | provisional static, DDK-private convention: unit refused |
| 0x00498860 | IDENTICAL | VS2022 body | same unit as 0x00498810 (re/match/s16/00498810.c), refused |

No newly DIFFERENT row: the AFTER DIFFERENT set is the BEFORE set minus the 7 provisional rvas. The map
(`mashed_re_ddk.map`) defines the 5 externalized names from the VS2022 objects (`_RwCore_004c0c20`
vs_RwCore_004c0c20.obj, `_RwCore_004c0f00` vs_RwCore_004c0f00.obj, `_RwCore_0043c5b0` vs_RwCore_0042aad0.obj,
`_PortTop_Bound_0045cd30` vs_PortTop_0045cd30.obj, `_Orig_00498510` vs_PortTop_00498510.obj). In these
5 objects no linked function references the provisional one (no undefined entry left for it), so the DDK copy is dead.

## Both builds green, default exe unchanged

- `cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"`: exit 0 BEFORE and AFTER, link rc 0.
- Default exe: the code diff touches only `re/tools/ddkobj/ddkobj.py`, which `build.bat` runs only under
  `MASHED_DDKOBJ=1`, after the default link, writing under `build\ddkobj`. Check: the `mashed_re.exe` of the AFTER
  object-build pass against a plain default build (no `MASHED_DDKOBJ`): `.text` 0 bytes differ, `.data` 0, `.reloc` 0,
  `.rsrc` 0, `.rdata` 2 (the link timestamp in the debug directory; PE TimeDateStamp 0x6afa0138 vs 0x6afa0390).
  G-CTRL / B not run (top-down row, the merge train measures them).

## Knob boot (MASHED_ORIG_TOP=1, MASHED_DET_FRAMES=600, private root D:/MashedWT/DDKOBJ-PROV-MIXED-UNIT_root)

Script `verify/ddkobj_20261115/knob_arms.sh`, under test-slot then mashed_lock.

| arm | exe | result |
|---|---|---|
| vs_top | mashedmod/build/mashed_re.exe | rc 0 after 15.1 s, "WinMain 0x00492370 returned 0 after 600 frames" |
| ddk_top | mashedmod/build/ddkobj/mashed_re_ddk.exe (sha256 70d96599...86421) | rc 0 after 16.4 s, same line |

No menu walk: no menu code changed.

## Decision

None filed. Which copy runs for a provisional body is already decided by L-126 (6a) (VS2022), and a unit the tool
refuses keeps its VS2022 body (WORKER_RULES). The cost of that rule here: 0x004025f0 and 0x00498860 (100% matched)
run their VS2022 copies in the object build because their units hold a provisional static with a DDK-private
convention. If the oracle should run them as DDK bodies, that needs the provisional bodies of 0x00401690 / 0x00498810
matched or admitted (L-126), or a decision to run those two provisional bodies as DDK code.

## Raw data (D:/MashedWT/DDKOBJ-PROV-MIXED-UNIT_data)

| file | sha256 |
|---|---|
| before/BYTECHECK.tsv | 08bce2ddaf840554562ded2c92f27df0f6c4d338c12b9657e232d60af327a794 |
| before/ddkobj_build.json | 1e0c80dc85a36dbe90fffe11da961a9a06775ec8862a72ef3619e54ef28a2bf8 |
| before/ddkobj_summary.json | 298fc2217633bc16eb19247518197f58015c61e3649adbcea5f03cd7303dc05d |
| after/BYTECHECK.tsv | 6eee6468434b202ceb999c0d511b450e67ff0413d173b658f431aad46fe2d492 |
| after/ddkobj_build.json | e81013dcc0310bf7d5ac56c7f7f4834a133663287f20ca541dcacda9a0b76a10 |
| after/ddkobj_summary.json | f6eb5c5d544cd9cb574e03279607f8e6f3bdb7398e506c755fe0c54d4b7cbf37 |
| build_before.log | f9d0afeb49b9f0b256f0089481c94ab53a94645328f61a214ea3265e967e86d0 |
| build_after.log | e4192cac0ada0692af1a7bec21c9b2b165bcb5f8b520e1f519a020eb58e8aaa4 |
| build_default.log | 30ed5c17b79e8bfc0cb74ee1777f7946c526d15fb95a8d01c8f2755349c48080 |
| vs_top.log | 68e55c68a8c9ebef151fb15e894533255728e77550f5abc796e75ce93357cf27 |
| ddk_top.log | 430cfa44a003ca8787ecb08fec77da60d9378c96d26f6f4ee30bc02c4f376537 |
| selftest_final.log | fb2af99a1bb64e5383d08edb84393ba76daf292f315cf33429f3f5a9b2ec1ab8 |

Committed summaries: `ddkobj_summary_before.json`, `ddkobj_summary_after.json`, `bytecheck_delta.txt`.
