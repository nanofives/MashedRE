# RESULT L156-REFUSAL-REASONS (run 46, D4, text row)

Branch `orch/L156-REFUSAL-REASONS`, based on `orch/integration` d1e5ac586. 2026-11-14. No build, no gate, nothing bound,
no tracker edit. L-156 was answered (b): the five units 0x00427e00, 0x00427f00, 0x00427ff0, 0x00428140 and 0x00458920
stay refused by ddkobj, with no local copies and no binding.

**Verdict: GREEN.** Reasons and evidence are recorded per unit.

## 1. Where ddkobj keeps refusals

ddkobj has no stored refusal list. The status is computed at run time: `re/tools/ddkobj/ddkobj.py` sets
`fail:externalized-static-register-convention` when a unit's helper copy has a private register convention
(RWC-FRAME-CLUMP, `process_unit`). The nearest data file, `re/match/PROVISIONAL.tsv`, covers unmatched bodies
(L-126) and does not list these five units, whose sources are 100% DDK matches.

## 2. Files edited

* `re/match/REFUSED_UNITS.tsv` (new, data only). One row per unit: rva, source, ddkobj_status, refusal_reason,
  unicorn_vector_compare, equiv_vs2022, uncertain, evidence. `ddkobj.py` does not read it.
* `re/tools/ddkobj/ddkobj.py`: two comment lines at the `fail:externalized-static-register-convention` assignment,
  pointing at the TSV. No code line changed.
* This RESULT.

## 3. Evidence recorded (source: RESULT_L156-REGCONV-EVIDENCE.md, RESULT_L156-ECX-556CA0.md, same folder)

* All five: equiv.py on the DDK compile is `EQUIV [BYTE-IDENTICAL]` (`equiv_ddk.txt`). On the VS2022 compile every
  unit is NOT-PROVEN (`equiv_vc19_single.txt`). NOT-PROVEN means equiv.py cannot pair the bodies across the
  register-convention helper call, not that a value differs.
* Unicorn unit vector compare, 300 vectors per unit and control word (`unit_cmp.py`): 0x00427e00 and 0x00458920 same
  300/300 at CW 0x037f and 0x007f. 0x00427f00, 0x00427ff0, 0x00428140 differ 300/300 only in ECX at the call to
  0x00556ca0 (calls 7, 11, 8). With that ECX excluded they are same 300/300 at both CWs. Negative control
  (`negctl_mut_00427e00.txt`): a one-constant mutant reports differs 10 of 10.
* L156-ECX-556CA0: 0x00556ca0 does not read ECX. It jumps through `[DAT_0067d838 + 0x138]`, whose targets
  0x005547f0 and 0x00554940 each write ECX fully before any read.

## 4. [UNCERTAIN] items carried onto each row

* ECX target list is static: the writer set of `+0x138` comes from a linear displacement sweep. Register stores and
  block copies were not traced. Missing evidence: a write watch on `[DAT_0067d838]+0x138` in a run
  (`--watch-write`). Applies to 0x00427f00, 0x00427ff0, 0x00428140.
* Built without /GS in the vc19 compare while the exe builds the TUs with the default /GS. Whether the exe's /GS code
  changes a value is not shown. All five.
* Callees outside unit + helpers are stubbed identically on both sides and not tested against the original
  (FUN_00427780, FUN_00552840, forwarders 0x00552d10, 0x00552750, 0x00552d70, 0x00556ca0, 0x00556e90). All five.
* PK_458570 (the exe adapter's naked helper for 0x00458920) is not byte-checked against 0x00458570.
* Random-vector coverage only: paths the vectors did not reach are not covered. All five.

## 5. Behaviour guard for ddkobj

`py -3.12 re/tools/ddkobj/ddkobj.py check` and `selftest` both need a build output (`check` stops with
FileNotFoundError on `mashedmod/build/ddkobj/ddkobj_build.json`, `selftest` on `mashedmod/build/obj/exe/link.rsp`),
before and after the edit alike, and this row runs no build. So "same output as before" is shown by the diff instead:
the only change to `ddkobj.py` is two added `#` comment lines, and `py -3.12 -m py_compile` passes. Run `check` on
the next object build to confirm.

## 6. Cited raw files

None. Everything cited is committed under verify/l156_20261114.
