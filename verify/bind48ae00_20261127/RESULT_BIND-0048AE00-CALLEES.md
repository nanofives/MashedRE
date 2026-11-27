# RESULT BIND-0048AE00-CALLEES (run 57, 2026-11-27)

**Not met, stop moved from 0x00544a70 / 0x00544ad0 / 0x00544bf0 to 0x005449f0.** 0x0048ae00 stays unbound. The heap fault
(0xC0000374) is not caused by the three callees with no body: it is the bound matched body of **0x005449f0**
(`PortTop_Bound_005449f0`, re/match/rwl04/005449f0.c), which runs before them, on an exe librw object. One user decision is
needed before 0x0048ae00 can be bound (section 5).

Branch `orch/BIND-0048AE00-CALLEES`, rebased on the merged head 287c10369. The branch changes no compiled file: the binding
was measured on two diagnostic exes and then taken out (section 4). Raw data: D:/MashedWT/BIND-0048AE00-CALLEES_data
(SHA-256 in `raw_sha256.txt`).

## 1. The three callees (class, source / match state, what they write)

All three: hooks.csv subsystem `third-party-library[renderware]`, C2, no exe body (named stops in the bulk profile:
`Match/PortTop_BulkStops.cpp:733-740` for 0x00544a70 / 0x00544ad0, `Match/PortTop_RaceFrameStops.cpp:483` for 0x00544bf0).
equiv.py on this tree (`py -3.12 re/tools/match/equiv.py re/match/rwl04/00544a70.c re/match/rwl04/00544ad0.c re/match/rwl04/00544bf0.c`):

| rva | source, best | equiv.py verdict (first difference) | what the source writes (re/match source, mechanical) |
|---|---|---|---|
| 0x00544a70 | re/match/rwl04/00544a70.c, 58.1% (class b) | NOT-PROVEN, x87-region: ours +0x0 / orig 0x00544a70, x87 instruction 2 differs `fld m4[esp+0x1c] / fstp m4[ecx+0xc]` | with `p = *(DAT_007dc670 + param_1 + 8) + param_2*0x14`: dword p+0 = 0, word p+4 = param_3, word p+6 = param_4, float p+8 = param_5, float p+0xc = [0x005cc320] / param_5, dword p+0x10 = next record (p+0x14) or the array base on the last index |
| 0x00544ad0 | re/match/rwl04/00544ad0.c, 46.0% (class b) | NOT-PROVEN, branch-condition: ours +0x28 / orig 0x00544afc | reads param_1+0x18; on 0: error 0x105 through 0x004d7ff0 / 0x004d8480, returns 0. Else writes the record pointer at DAT_007dc66c + param_1 (+0, +8), param_1 +0x50 / +0x52 (words), +0x5c = 0, +0x54 float, +0x58 = [0x005cc320] / that float, ORs 3 into +0x4c four times and calls 0x004c0e50(param_1+4) when bit 4 was clear |
| 0x00544bf0 | re/match/rwl04/00544bf0.c, 74.3% (class b, TOOLCHAIN rw9044) | NOT-PROVEN, x87-motion: ours +0xb5 / orig 0x00544ca7, `mov m4[esi+0x4c],ecx` moved across x87 instruction 3 | reads param_1+0x18; same error path on 0. Else writes DAT_007dc66c + param_1 (+0, +4 = param_2, +8), param_1 +0x50 / +0x52 / +0x5c / +0x54 / +0x58, the same +0x4c / 0x004c0e50 dirty block |

None is admitted (no EQUIV). Each could be listed in PROVISIONAL.tsv as NOT-PROVEN, but that would not make the bind correct:
all three write through the RW plugin offsets DAT_007dc670 (geometry) and DAT_007dc66c (atomic) into RW-layout objects, and on
every knob boot the objects 0x0048ae00 hands them are exe views of librw objects (section 2), the same class of fault as
0x005449f0. So no PROVISIONAL.tsv row was added and the file is not touched. The bulk profile's moved.tsv lines for 0x00544a70 /
0x00544ad0 (written by BIND-00420420) stay as they are: both are still stops, nothing moved.

## 2. The heap fault: 0x005449f0 frees the geometry flags word

Measured (arm 1, `knob_vs_base` and `knob_vs_all`, VS2022 exe with 0x0048ae00 bound in a row-local profile and wired into
0x0040bbb0, the seven RW calls bound to the existing librw bindings, the form of 0x004548e0): both boots exit rc 0xC0000374
(9.7 s and 2.7 s). The log tail of both:

```
[RW_BIND] RpClumpStreamRead(0x09183F50) -> rw::Clump::streamRead 10A779B8 (1284 bytes, 1 atomics)
[RW_BIND] 0x004b3f90(clump 10A779B8) -> atomic 0F5A4B90 view 0F5A8458 (geometry 047C4190, 10 vertices)
[RW_BIND] RwGlobals +0x10c free(00000037) -> rw::Engine::memfuncs.rwfree
```

In re/match/s14/0048ae00.c the only call between 0x004b3f90 and the first of the three callees is `FUN_005449f0(mat, 1)` with
mat = *(atomic + 0x18). 0x005449f0's source: `piVar1 = DAT_007dc670 + param_1; if (piVar1[2] != 0) DAT_007d3ff8->fn10c(piVar1[2]);`
(RwGlobals +0x10c is the free slot, LibRw/RwApiBind.cpp:1814 logs it). The value freed, 0x37, is this geometry's flags word:
the diagnostic arm logs `0x004b5300(atomic 10C8A788): mask 0x40, set 1: geometry 047F12D8 flags 0x00000037 -> 0x00000077` for
the Splash.dff geometry (d3_vs_base.log line 241), and the flags sit at geometry +0x08 (static_assert in RwApiBind.cpp:919-920).
piVar1[2] = geometry + DAT_007dc670 + 8, so the free of 0x37 fits DAT_007dc670 = 0 on this path (the RW geometry plugin offset
is not registered on the librw route). [UNCERTAIN] the value of DAT_007dc670 at that call was not read; register line updated.

So the cause is the object model on the knob path: 0x0042a5d0 returns a librw `rw::Clump` on every knob boot, including
`MASHED_RWC_FRAME=1` (`RwCore_FrameRoute()` also needs `RwCore_Enabled()`, RwCore/RwCore_Knob.cpp:68-76; knob_vs_all logs the
same librw lines). The previous row's binding also bound 0x004b3f90 / 0x004e69a0 / 0x004e6e00 / 0x004e7e30 / 0x004c0b30 /
0x004c1040 / 0x004b52f0 straight to `RwCore_<rva>` (gates profile `rwcore_fallback`), which read RW layouts from the librw
clump. With those on the existing librw bindings the fault stays, at 0x005449f0.

## 3. Binding numbers

| number | value |
|---|---|
| units executing or reachable from a bound caller on the top-down path (this branch) | 0 new (0x0048ae00 not bound) |
| pre-bound keep-alive units in generated PortTop_BulkRefs.cpp | 0 new (file not touched) |
| bindable rows left unbound | 1: 0x0048ae00, reason: its callee 0x005449f0 (bound matched body, not owned by this row) frees the geometry flags word on the librw object and the boot exits 0xC0000374; its three other RW plugin callees have no body and the same layout problem (sections 1, 2) |
| measured execution count of 0x0048ae00's body | **0** on the head and on this branch (stand-in `PortTop_StandIn04G_0048ae00`). **1** per boot on the diagnostic arm d3 (section 4): 80 `0x004e69a0(view)` clone lines (8 x 10), 1 Splash.dff read, 1 `RpClumpDestroy` of that clump |

## 4. Diagnostic arm d3 (not committed)

0x005449f0 as a named stop in the 0x0048ae00 wrapper (`stop_force 005449f0`), the four stops in count mode
(`MASHED_OT_B48_STOPS=count`): `d3_vs_base` rc 0 after 3000 frames (33.5 s), `d3_vs_all` rc 0 after 3000 frames (27.1 s).
The four stops are each reached once in the log as first call (0x005449f0, 0x00544a70, 0x00544ad0, 0x00544bf0, callers 0x0048ae00).
This shows nothing else in 0x0048ae00's closure faults on the librw route. It is not a bind: 0x005449f0 is not owned here, the
stops exit by default (code = rva), and with count mode the morph data the four write is missing. The profile, rules, roots and
the generated wrapper are kept as prep in `prep/` (`rules.tsv` = the committed-form rules without the stop_force line,
`PortTop_0048ae00_d3.cpp.txt` = the d3 wrapper, `wiring.diff` = the 0x0040bbb0 rebound line and the two rsp lines).

## 5. Decision needed (for the lead to file)

0x0048ae00 on the knob path. Options: (a) bind it with 0x005449f0, 0x00544a70, 0x00544ad0, 0x00544bf0 as count-mode stops in its
wrapper (boot measured rc 0, body runs once, the four RW plugin writes missing; needs 0x005449f0's owner and a stop that returns
without exiting by default); (b) keep the stand-in until the RW-core route (`RwCore_Enabled()` + `MASHED_RWC_FRAME`) runs the
boot, where the RW-layout bodies of all five apply; (c) librw bindings for the five plugin calls, which L-124 forbids on the
top-down path without a decision. Recommendation: (b), with (a) only if a T1 measurement needs 0x0048ae00's body to execute
before the RW core runs. Evidence: this file.

## 6. Checks

| check | result |
|---|---|
| mashed_lock status before each build / boot / selftest | printed; free, one PRIORITY-HOLD waited out by mashed_lock run |
| VS2022 build, arm 1 tree (bddefb0f6 + wiring) | Build OK (5 min 22 s) |
| VS2022 build, d3 tree | Build OK |
| knob boots arm 1 (knob_vs_base, knob_vs_all) | both rc 0xC0000374, same log line; the earlier row's four boots gave the same, so no re-run (a crash, not a short exit) |
| knob boots d3 (d3_vs_base, d3_vs_all) | both rc 0, 3000 frames |
| binder selftest on the rebased tree (287c10369, branch adds only verify/ and the register line) | PASS, 0 failures, every profile. Run before the lead's BIND-SYNC-REMOVE-STALE ordering note; the branch writes no binder output (no write / sync / bindings / prune), so it does not depend on that tool row; re-run after "SYNC merged" if the lead wants the count on that head |
| object build, ddkobj.py check | not run: the branch compiles exactly the head's files; head numbers stand (IDENTICAL 2455/2562, DIFFERENT 79, not-in-map 28, linked 2437/2713, refusals 17, D:\MashedWT\mtrain_data\20261127_162413); 0 newly DIFFERENT units; dual-body count unchanged |
| match.py --bytes / relocheck.py on included sources | not run: no source newly included. 0x0048ae00.c was BYTE-IDENTICAL 357 B (hooks.csv c4=match); counted 0 match + 0 replay-C4 newly bound; no "listed unit -> VS2022 fallback" pairs |
| menu walk | not run: no menu code changed |
| frida-<hash> folders in %LOCALAPPDATA%\Temp | 0 before, 0 after (no Frida harness used) |
| C: free | 57 GB at start |

Unfinished scope: the bind itself (decision above); the [UNCERTAIN] DAT_007dc670 = 0 read at the 0x005449f0 call.
Tool: `tools/knob_boots.py` (copy of verify/bindgates_20261121/tools/knob_boots.py with this row's paths plus the two d3 runs).
