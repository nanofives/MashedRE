# RESULT DDKOBJ-SPIKE (2026-11-11)

Brief: `log/orch/briefs/DDKOBJ-SPIKE.md`. PREREG: `verify/ddkobj_20261111/PREREG_DDKOBJ-SPIKE.md` (commit f3c285196,
written after the two exploratory boots of a first build, section 5.1). Branch `orch/DDKOBJ-SPIKE`.

## Verdict

**Yes, the object-link route works.** The variant `mashed_re_ddk.exe` links the XP DDK objects of **540 of the 583**
matched sources the exe includes. Every linked function's bytes were checked in the image: **541 of 552 are the
original's bytes**, data addresses included. 8 differ: 7 come from sources below 100%, and 1 is a "100%" source
whose `fld`/`fmul` operands are swapped, which match.py's masking misses. 3 are not linked (no caller).
The variant boots knob OFF and ON (3000 frames, exit 0). Its knob-ON log equals the VS2022 build's line for line
(1133 lines, pointers masked), and its 12-capture menu walk is class E in 4/4 runs, identical to VS2022. The one
function measurable offline (0x00443300) equals the original on 20000/20000 inputs at PC_24. The VS2022 compile of
the same source differed on 14039.

The route has two hard limits, both found and fenced mechanically:

1. **Register conventions.** cl 13.00 gives a `static` function a private register convention. 38 matched
   sources are written that way (`MATCH_STATIC`), and VS2022 code calls them with stack arguments. The first variant
   crashed at boot because of this. The tool now compiles each such function non-static and refuses it if its bytes change.
2. **Globals the port re-binds to its own cells** (`#define MGLOBAL ... MATCH_CELL_*`, 5 sources). A DDK object can
   only address the original location. The first menu walk crashed on 0x004282a0 (bisected, one unit). The tool now
   refuses these.

The default build is unchanged: same `link.rsp`, same 874 objects, and the exe differs from the pre-change one only
in the link timestamp.

## 1. What was built

`set MASHED_DDKOBJ=1` before `mashedmod\build.bat` builds `mashedmod/build/ddkobj/mashed_re_ddk.exe` after the default
exe link (`build.bat` block). Unset, the block does not run. Tool `re/tools/ddkobj/ddkobj.py` (`build`, `check`),
COFF editor `re/tools/ddkobj/coffedit.py`. A unit is one `re/match` source an exe wrapper (`exe_sources.rsp`) includes.

1. **Compile** with the DDK exactly as `match.py:91-104` does (cl 13.00.9176, `/c /Gy` + `// FLAGS:`, clean env, C,
   no `MATCH_PORT`). 583/583 compile.
2. **Symbol rewrite** of the DDK object (COFF edit):
   - a. the MATCH function the port compiles takes the exe name its wrapper gives it (`#define FUN_x Orig_x`),
     decoration kept (`_x`, `_x@N`, `@x@N`). Other external definitions get a `__ddk<rva>` suffix.
   - b. **Data**: `MGLOBAL`/`MGLOBALA` names, and other undefined data names with a Ghidra address suffix, become
     `__ddkabs_<addr>`, absolute symbols in a generated COFF object (850). The exe maps the original range through
     `g_b17_low_arena_pad` (`exe_main.cpp:272-294`), the same place the VS2022 wrapper's `(*(T*)addr)` reads.
     **Port copies**: when the source's `MATCH_PORT` view defines a global with its own initializer (one source in all
     of `re/match`: `s_Tri_dff_005cf4f8 = "Tri.dff"`, `re/match/s15/0048ff20.c:34-39`), the arena is zero there.
     The DDK object then reads a copy of the original bytes (`ddk_copies.obj`). Without it, the TRIPOOL body opened
     `""` and built 0 of 30 slots (section 5.2).
   - c. **Callees** bind the way the unit's own wrapper binds them, read from its `#define`s under the exe's
     preprocessor view (`#ifdef MASHED_STANDALONE` evaluated). Forms: identifier, `::Ns::Name` (mangled name from the
     default objects), forwarding macros `#define FUN_x(a,b) T((a),(U)(b))`, original-address calls
     `((T)0x00xxxxxx)` / `static ... const p = reinterpret_cast<T>(0x...)`. A name the wrapper does not bind takes the
     binding every exe wrapper agrees on, else its own name.
   - d. **Arena calls** (12) go through a `push imm32 / ret` thunk (`ddk_thunks.obj`). REL32 to an absolute symbol is
     LNK2016. The thunk touches no register.
   - e. **Port-excluded helpers** (`#ifndef MATCH_PORT` copies): an external copy becomes a reference to the one exe
     body. A static copy stays local when its callees bind, else it is externalized.
   - f. References reachable only from dropped code go to `__ddk_dropped` (0).
   - g. **SafeSEH**: cl 13.00 predates `@feat.00`. The first variant linked with **no** safe-handler table (dumpbin
     `/loadconfig`: 0 entries, default exe 0x2DE), because one unmarked object makes the linker drop it. A DDK object
     with no `__except_handler*` reference and no `.sxdata` is now marked `@feat.00 = 1` (all 540), and the table is 0x2DE again.
3. **Wrapper object**: the wrapper TU is recompiled for the variant with the default flags plus `/Ob0` (VS2022 had
   inlined 0x00443300's matched body into its dispatcher, which left the DDK body unreachable). Its definition of the
   exe name (external, or `static` when the wrapper makes `MATCH_STATIC` static) is renamed `<name>__vs`, and every
   relocation that pointed at it is repointed to a new undefined symbol of the exe name. Callers in other TUs and inside
   the wrapper both reach the DDK body (552/552 renamed, none absent). 25 wrapper statics a DDK object binds to are
   promoted to externals under unique names.
4. **Link**: default `link.rsp` with the edited wrapper objects swapped in, plus the DDK objects and the three
   generated objects, on the default link line plus `/NODEFAULTLIB:libc.lib`.
5. **Refusals** (unit keeps its VS2022 body): `fail:static-register-convention` 38 (recompiled non-static, bytes
   change), `fail:wrapper-redefines-globals` 5 (`004769f0`, `004282a0`, `004963e0`, `00496400`, `00498910`).
   Table: `UNITS_all.tsv`.

## 2. Brief items, answered

| # | item | answer |
|---|---|---|
| 1 | compile | match.py's invocation, unchanged. 583/583. |
| 2 | data | 850 absolute symbols, compared exactly in the byte check (stricter than relocheck's masking). 1 port copy. |
| 3 | callees | from the wrappers' `#define`s, mechanically (2c): the wrapper is what the default build links. BINDINGS.tsv drives the wrapperless bulk route (section 4). Register args survive: renames are aliases, the thunk is `push/ret`. |
| 4 | callers | wrapper surgery (item 3): the VS2022 copy goes dead, its name resolves to the DDK object. |
| 5 | `/GS` | 22 units reference `___security_cookie` / `@__security_check_cookie@4`. Both resolve against the VS2022 CRT and run on both paths (section 5). |
| 6 | CRT | the substituted set uses `__ftol` (8), `__alldiv`, `__allmul`, `_strchr` (5), `_strrchr` (2) and 9 kernel32 imports, all from the VS2022 CRT/SDK. No substituted unit calls CRT math. Probe: section 7. |
| 7 | linker | `/DEFAULTLIB:LIBC` in every DDK object (`/NODEFAULTLIB:libc.lib`). `__real@` COMDATs merge (select-any). Duplicate helpers suffixed. No debug info. SafeSEH: item 2g. |

## 3. Byte check (deliverable B)

`ddkobj.py check` after the link: each substituted function's bytes in the image against `MASHED.exe.unpatched` at
its RVA. REL32/DIR32 fields to code or literals are masked. DIR32 fields to original data addresses are **compared
exactly**. `BYTECHECK_all.tsv` (552 functions), `BYTECHECK_small4.tsv` (first 4-unit build: 4/4 IDENTICAL).

| result | functions | detail |
|---|---|---|
| IDENTICAL | 541 | the original's bytes, data addresses included |
| DIFFERENT, source below 100% in match.py | 7 | 0x00493900 (66.9%), 0x00493710 (94.2%), 0x0042f530 (96.9%), 0x00475a00 (93.1%), 0x0048e820 (85.5%), 0x004929d0 (95.7%), 0x0045a3a0 (81.0%) |
| DIFFERENT, source 100% in match.py | 1 | **0x00414060** (`re/match/s03/h3_00414060.c:31`): the DDK object emits `fld [0x5cc32c]; fmul [0x89a37c]`, the original `fld [0x89a37c]; fmul [0x5cc32c]` (+0x0d / +0x13). match.py masks both operands as relocations and scores 100%. Same value (one commutative product), different bytes. |
| not linked | 3 | 0x004053d0, 0x00405420, 0x00405430: no caller, removed by `/OPT:REF` |

Of the 246 BINDINGS `matched` rows: 229 substituted (226 IDENTICAL, 3 below 100%), 16 refused by the
register-convention guard, 1 not included by any exe wrapper.

## 4. Bulk sizing (deliverable E)

`re/tools/ddkobj/size_bulk.py`. Per undefined symbol: in-set, in-variant (a unit of the 540-unit variant), data,
bind:<kind> (BINDINGS row whose exe symbol the default link defines), bind-missing, wrapper-name (a callee all
wrappers bind alike), exe-name, crt, crt-math, crt-absent, unbound (FUN_x with no row, no binding), unknown.

| set | rows | linkable | blocked | blocked by |
|---|---|---|---|---|
| BINDINGS `matched` (wrapperless view) | 246 | 181 | 65 | in-set chain 59, unbound 7, bind-missing 6, crt-absent 1 |
| closed-set runnable (`origtop_size.py`, 1065 today) | 1065 | 987 | 78 | unbound 63, bind-missing 10, in-set 7, unknown 5, crt-absent 1 |

The 1065 runnable rows came from `origtop_size.py` with one added dump flag
(`re/tools/ddkobj/_origtop_size_dump.py`). `SIZE.tsv` says 1064 (BINDINGS changed since).
Of the 987 linkable runnable rows, 381 already have an exe wrapper. **Link test**: the variant plus the 606 others as
wrapperless DDK objects (`DDK_<rva>`, one object per source file) links with **0 errors**. One import lib was added,
`winmm.lib` (0x005ab040 `timeGetTime`). Nothing calls the wrapperless objects yet: link closure, not runtime.
Run on the 545-unit variant (before the `MATCH_CELL` refusal). The 5 refused units do not change the closure (their
callers bind by name either way).

Top unbound callees (runnable set): `FUN_004b6fc0` (21 callers), `FUN_004b7090` (18), `FUN_004c1480` (8),
`FUN_004c51a0` (8), `FUN_00490e50` (6). Bind-missing: `FUN_0047b9b0` (port, 3), `FUN_004a45fb` (crt, 3). Unknown:
`_FUN_stand_in_a` (2), `_DAT_stand_in_mat`, `_UseUv`, `_thunk_FUN_00419760`. crt-absent `__free` (1), crt-math
`_floor` (1). Tables: `SIZE_BULK_matched246.tsv`, `SIZE_BULK_runnable1065.tsv` + `_summary.json`.

## 5. Runtime (deliverable C)

Private roots `D:/MashedWT/DDKOBJ-SPIKE_root`, `_rootK` (junction `original` -> `D:/MashedWT/PORT-TOP-04J_rootK/original`),
every launch in `mashed_lock.py run` inside `test-slot`. Raw logs in `D:/MashedWT/DDKOBJ-SPIKE_data`, sha256 in
`RAW_SHA256.txt`.

### 5.1 History (each failure and its cause)

| build | run | result | cause, fix |
|---|---|---|---|
| 583 units | knob OFF / ON (exploratory, before the PREREG) | `0xC0000005` at 1.5 / 5.1 s, after `R2-2 gamesave load` | register-convention statics (item 5). The unit was not isolated: the bisect stopped for the merge train's PRIORITY-HOLD, and the guard removed all 38 candidates. Next build boots. [UNCERTAIN: which of the 38 crashed] |
| 545 units, no port copy | knob ON | 3000 frames, rc 0, log diff: TRIPOOL `Tri.dff` opened as `""`, 0 of 30 slots | port copy (item 2b) |
| 545 units | 12-capture walk, knob OFF and ON | `0xC0000005` after capture 3 (3/12 equal E), 4/4 runs | bisect (`BISECT_nav.tsv`, 11 probes): **0x004282a0** alone. Its wrapper re-binds `DAT_0067d838`/`DAT_0067d830` to exe cells (`MatchS06_004282a0.cpp`, `exe_main.cpp:3575`). The DDK object read the zero arena as a font pointer. Fix: refusal rule (item 5). Full set minus that unit: OK. |

### 5.2 Final build (540 units, `mashed_re_ddk.exe` sha256 `c59eecba...`)

| arm | VS2022 `mashed_re.exe` | variant |
|---|---|---|
| boot knob OFF, `MASHED_DET_FRAMES=3000` | (default path, not re-run) | rc 0, 52.1 s |
| boot knob ON (`MASHED_ORIG_TOP=1`) | rc 0, 52.9 s, `returned 0 after 3000 frames` | rc 0, 52.8 s, `returned 0 after 3000 frames` |
| `[ORIG_TOP]`/`[RW_BIND]` log lines, pointers and timings masked | 1133 | 1133, **0 lines differ** |
| 12 captures, OFF/ON/OFF/ON (`capflake_runs.py`, pin dflt) | 4/4 class E, eqE 12 | **4/4 class E, eqE 12** |

`CAPS_runs.tsv` holds every capture row (the `ddk` arm rows are the crashing 545-unit build, `ddk2` the final).
The walk reaches the knob path's frontend, so the screens the menu walk covers render the same. This is the same
check that the VS2022 port passes. It does not prove the substituted units compute the original's values on the
menu: the menu walk's captures do not depend on the float paths section 8 measures.

One run pair (VS2022 `vs2_on`, variant `all3_on`/`all4_on`) slowed to about 4 frames/s after frame 480, the VS2022 one
in the same lock window. It is not a variant effect. [UNCERTAIN: cause not investigated]

## 6. Default build unchanged

Rebuilt the worktree with the `build.bat` block present and `MASHED_DDKOBJ` unset:

| check | before (`ee3562e27` tree) | after |
|---|---|---|
| `build/obj/exe/link.rsp` sha256 | `3494f40e...` | `3494f40e...` |
| 874 exe objects, sha256 each | `DEFAULT_OBJS_before.sha256` | identical |
| `mashed_re.exe` sections | `.text/.data/.rsrc/.reloc/.fptable` | byte-identical. `.rdata` differs in 2 bytes at 0x0029d8e8, the link timestamp copy (0x6af420c1 vs 0x6af44194) |

Default gates (`run_arms.py H_DEF B_DEF_K*`) not run. The default exe is the same machine code and data, so the gates
cannot move. They are required before an enqueue (WORKER_RULES).

Process note: this rebuild started while `mashed_lock.py status` showed a merge-train PRIORITY-HOLD. My check printed
it and the command did not stop. The build compiled nothing (all 874 + 1208 objects up to date) and only relinked.
That breaks the worker rule, and it is recorded here.

## 7. CRT math probe (L-120 input)

`re/tools/ddkobj/mathprobe/`: one program built three ways (DDK cl+link+libc.lib, VS2022 `/arch:IA32`, VS2022 SSE2),
4096 inputs (float angles in [-8pi, 8pi], pairs for atan2), at x87 PC_24 and PC_53. `MATHPROBE.md`.

| function | PC | DDK vs VS2022 x87 (double / float) | DDK vs VS2022 SSE2 (double / float) |
|---|---|---|---|
| sin, intrinsic `__CIsin` and function `_sin` | 24, 53 | 9 / 0 | 9 / 0 |
| cos, intrinsic and function | 24, 53 | 3 / 0 | 3 / 0 |
| atan2 | 24, 53 | 0 / 0 | 0 / 0 |
| sqrt | 24 | 0 / 0 | **4096** / 0 |
| sqrt | 53 | 0 / 0 | 0 / 0 |

sin/cos differ in 9 and 3 of 4096 double results, none after rounding to float. `fsin`/`fcos` ignore precision
control. An SSE2 sqrt ignores PC_24 where the DDK's `fsqrt` honours it. This probe does not reproduce a float-level
sin/cos difference [UNCERTAIN: L-119's inputs and path were not re-run here].

**Direction (b) works**: `ddkcrt_rename.py` extracts `sin.obj cos.obj 87disp.obj common.obj genexcep.obj 87except.obj
fpctrl.obj` from the DDK `libc.lib` and renames the 44 symbols they define to `_ddkcrt*`, references included. The
6 left (`___fastflag __fltused __handle_exc __matherr __raise_exc __set_errno`, error path) resolve in the VS2022 CRT.
A VS2022 exe that keeps its own CRT and calls `_ddkcrt_sin`/`_ddkcrt__CIsin` (ST0 convention, inline asm) gives
**0/4096 differences from the pure DDK build in every column at PC_24 and PC_53** (`MATHPROBE_renamed.md`, `ren.bin`
sha256 = `ddk.bin`). In the variant, a DDK object's `__CIsin` reference would be renamed to `_ddkcrt__CIsin`, so
only DDK callers see the DDK bodies. The full map closure pulls in the whole DDK CRT startup, so the member list has
to be cut at the error path as done here.

## 8. Float proof (deliverable D)

`re/tools/ddkobj/fuzz_exe_00443300.py`: Unicorn, inputs of `aispline_ddk/fuzz_00443300.py` (seed 7, 20000. The
stream is reproduced: the original at PC_53 gives 14944, as in `AISPLINE_FUZZ.tsv`). Bodies are taken from the
**linked exes**, with the original image written over the exe arena.

| body | PC_24 | PC_53 |
|---|---|---|
| variant `_AiSplineEval_x87_match` (the DDK object) | **0 / 20000** | 14944 / 20000 = the original's own PC_53 count |
| default `_AiSplineEval_x87_ddk` (hand-written DDK order, L-104) | 0 / 20000 | 0 / 20000 |
| VS2022 `_match` (`AISPLINE_FUZZ.tsv`, X `_match`, same inputs) | 14039 / 20000 | 14862 / 20000 |

The DDK object is the original function, including its control-word sensitivity. The default exe's VS2022 `_match`
body exists only inlined into `_AiSplineEval_x87` (not in the map), so the VS2022 row is cited from the earlier harness
run on the same inputs.

## 9. Files

Committed in `verify/ddkobj_20261111/`: this RESULT, the PREREG, `BYTECHECK_all.tsv`, `BYTECHECK_small4.tsv`,
`UNITS_all.tsv`, `WRAPPERS_all.tsv`, `SIZE_BULK_matched246.tsv` (+ summary), `SIZE_BULK_runnable1065.tsv`
(+ summary), `CAPS_runs.tsv`, `BISECT_nav.tsv`, `MATHPROBE.md`, `MATHPROBE_renamed.md`, `DEFAULT_OBJS_before.sha256`,
`RAW_SHA256.txt`. Tools in `re/tools/ddkobj/` (`ddkobj.py`, `coffedit.py`, `size_bulk.py`, `bisect_units.py`,
`catch_crash.py`, `fuzz_exe_00443300.py`, `_origtop_size_dump.py`, `mathprobe/`). Raw logs and captures:
`D:/MashedWT/DDKOBJ-SPIKE_data` (logs listed with sha256 in `RAW_SHA256.txt`, captures under `caps/`). Probe binaries
and outputs: the session scratchpad (`<scratch>/mathprobe`, sha256 in `RAW_SHA256.txt`).

## 10. Recommended design for the mass route

1. **The wrappers stay the binding table.** Every binding a DDK object needs is already a wrapper `#define`. The
   generator reads them, so a newly matched body needs nothing beyond today's wrapper. Restrict wrapper defines to
   the forms the tool parses (item 2c), and make the generator fail on any other form instead of guessing.
2. **VS2022 stays the fallback.** The default build is the source tree compiled by VS2022 and needs no DDK. The
   variant is generated from it at link time. A tree without `tools/vc7ddk` (the LAN `git archive` build) does not
   set `MASHED_DDKOBJ`, so nothing changes for it. If the variant becomes the replay or shipping target, its builder
   needs the DDK, and every refused unit falls back to its VS2022 body automatically.
3. **Refusals are automatic and per unit** (item 5). The 38 register-convention units can be recovered later with a
   generated VS2022 `__declspec(naked)` adapter per function that moves stack arguments into the registers the DDK body
   expects. That needs the convention read from the DDK body, so it is per-function work. The 5 `MATCH_CELL` units need their
   cells moved to the original addresses (or the exe hand code changed to read the arena), a port decision.
4. **The byte check is the gate.** Run `ddkobj.py check` after every variant link: IDENTICAL for each 100% source,
   anything else reported. It is stricter than match.py (0x00414060), so the C4 match route should gain an
   exact-data-operand option.
5. **Port column.** With the object route, the body that runs is the `re/match` file itself. A substituted unit with
   an IDENTICAL byte check row is port `body` (`called` when reached) on the variant target, evidenced by
   `BYTECHECK.tsv` rather than by scanning wrapper TUs. The scanner would read the variant's build JSON + byte check.
6. **Dual-copy lint.** In the variant, the wrapper's VS2022 copy is renamed `__vs` and dropped by `/OPT:REF`
   (`vs_copy_in_map` = False for every row), so a substituted RVA has one body, the original's. The lint keeps
   guarding the source tree (the fallback target). One new rule for the variant: a substituted RVA must not also
   be bound to a hand body the variant links. The ORIGTOP-ONLY class already covers today's hand-vs-matched pairs.
7. **CRT math.** Link the renamed DDK libc math (`_ddkcrt*`, section 7) into the variant and rename DDK-object
   references to it. No substituted unit needs it today.
8. **Control word.** A DDK object equals the original at the original's CW and differs at PC_53 the way the original
   does. The game thread must keep the original's CW (PC_24). That is the D3D device path's job, not the objects'.
9. **Fail loudly.** Two classes now fail the variant build or are reported: an object that registers SEH handlers
   is left unmarked, which drops the safe-handler table (today: reported, should fail), and a wrapper `#define`
   form the tool does not parse.

## 11. Open questions

1. Which of the 38 register-convention units crashed the first boot (item 5.1). The guard does not depend on it.
2. 0x00414060's C4-by-match should be re-checked with exact data operands (no tracker edit in this row).
3. The 7 sources below 100% link as DDK objects whose bytes are not the original's. A release statement "the
   original's bytes" must use the byte check, not the unit list.
4. Frame pacing stall at frame 480 on both exes in one window (section 5.2).
5. `__ftol` (8 units) runs VS2022's. Both truncate, values not probed [UNCERTAIN].
6. The 606 wrapperless runnable rows link but are not called, so they did not run.
7. The full race replay against the original (the goal behind this row) was not attempted. The menu walk does
   not exercise the race float paths.

## Decision input for Mariano

The question: can `mashed_re.exe` run the original's own machine code for the functions we have matched, instead of
VS2022's rebuild of the same source? Yes, for most of what the top-down path runs today.

- 540 of the 583 matched sources the exe includes link as the original's objects. The game boots, runs 3000 frames on
  the top-down path, logs exactly what the normal build logs, and the 12-screen menu walk renders identically.
  541 functions in that exe were checked byte by byte against MASHED.exe and are the same.
- For the one function measured offline (the AI spline), VS2022's rebuild differs from the original on 70% of inputs.
  The object version differs on none. Today L-104 fixed that one function by hand. This route does it for every
  substituted function at once.
- 43 sources cannot be swapped automatically. In 38, the old compiler used a private calling convention. In 5, our
  code redirects the function's globals elsewhere. They stay on the normal build. The tool detects both, and both
  crashed the game before the tool refused them.
- Nothing changes for anyone who does not opt in. The normal exe is the same machine code.

Choice: (a) make the object variant the build the race replay is measured on, with the normal VS2022 build kept as
the default and fallback, or (b) keep it as a measurement tool only. Recommendation (a): the replay can only be exact
when the code is the original's, and this variant is that today for 540 sources, with a byte check that says which.
