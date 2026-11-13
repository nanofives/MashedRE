# PREREG DDKOBJ-REGCONV (2026-11-12)

Written before the runtime arms. Ledger row DDKOBJ-REGCONV (top-down tool row). Evidence it builds on:
`verify/ddkobj_20261111/RESULT_DDKOBJ-SPIKE.md` (sections 1 item 5, 5.1, 10 item 3, 11 item 1).

## Change under test

`re/tools/ddkobj/ddkobj.py` only (the object build, `MASHED_DDKOBJ=1`). The default build does not run this tool, so
the default exe is unchanged.

A unit whose static MATCH function gets a private register convention from cl 13.00 was refused
(`fail:static-register-convention`, 38 units). It is now recompiled by the DDK once with one generated adapter per such
function appended to the source text:

    <ret> <cc> __ddkadapt_<name>(<params>) { return <name>(<args>); }

`<cc>` is the convention of the exe name's symbol in the default VS2022 wrapper object (`_x` cdecl, `_x@N` stdcall,
`@x@N` fastcall). The DDK emits the call into the private convention itself. The adapter takes the exe name (the
wrapper surgery repoints VS2022 callers to it). The static body is renamed `__ddkreg_<rva>` (external), keeps its
in-object callers, and is byte-checked at its RVA. When the wrapper object has no C-named symbol of the exe name, no
adapter is generated and the body is linked the same way.

Refusal rules (the unit keeps its VS2022 body, reason in the build JSON): the adapter text cannot be generated
(unnamed or variadic parameter, no definition found), the adapter compile fails, or any function of the plain DDK
compile changes bytes or relocation targets in the adapted compile (compiler-local `$L` label names compared by
position). `DDKOBJ_REGCONV=refuse` restores the DDKOBJ-SPIKE refusal. `DDKOBJ_REGCONV=promote` (diagnostic only)
reproduces the first, unguarded spike build for the crash bisect.

Offline result already in hand (before this PREREG, DDK single-file compiles against the merge train's current default
objects): 38/38 units adapt with every plain-compile function unchanged; `ddkobj.py selftest` 25/25 PASS.

## Arms and pass bars

1. **Byte check.** Object build with every unit, `ddkobj.py check`. Pass: IDENTICAL for every substituted function
   whose source is 100% in match.py, including each `__ddkreg_<rva>` body. The 8 known non-IDENTICAL rows of the spike
   (7 below 100%, 0x00414060 operand order) are reported, not counted as failures. Refusals are listed with reasons.
2. **Tool self-test.** `ddkobj.py selftest`: 0 failures.
3. **Knob boots.** Variant exe, private root `D:/MashedWT/DDKOBJ-REGCONV_root`, `MASHED_DET_FRAMES=3000`, knob OFF
   and `MASHED_ORIG_TOP=1`. Pass: both run to their own exit (ON: `returned 0 after 3000 frames`). The ON log is diffed
   against the VS2022 exe's ON log (pointers and timings masked); every differing line is explained or listed open.
4. **Menu walk.** The adapters put 33 or more new DDK bodies on menu paths (text, menu dim, nav lookup), so the
   12-capture walk runs on the variant, `capflake_runs.py` OFF@dflt ON@dflt OFF@dflt ON@dflt. Pass: 4/4 class E.
5. **Which unit crashed the first boot.** Object build of the 38 units only with `DDKOBJ_REGCONV=promote`, knob OFF
   boot. If it fails with an exception code, `bisect_units.py` halves the set until one unit is left, then repeats
   until the set minus the culprits boots. Each culprit is reported with its static, its exe name and the VS2022
   caller (static scan of the default objects) that called it with stack arguments. If the 38-unit build does not fail,
   the same is run with all units.

No G-CTRL, no B, no 12 captures x 4 on the default exe (top-down row, WORKER_RULES): the default exe is not touched.
Every launch runs inside `mashed_lock.py run`, under the test slot. No build or boot while `mashed_lock.py status`
shows PRIORITY-HOLD or SOLO.
