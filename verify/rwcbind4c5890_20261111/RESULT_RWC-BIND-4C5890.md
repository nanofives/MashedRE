# RESULT RWC-BIND-4C5890: RwTexDictionaryCreate 0x004c5890 on the matched RW core (L-129 (b))

Verdict: **GREEN.** On the top-down path, 0x004c5890 now has one binding entry,
`RwCoreBind_TexDictionaryCreate_004c5890` (`mashedmod/src/mashed_re/RwCore/RwCore_Bind_004c5890.cpp`). With
`MASHED_RWC_CORE=1` the entry calls the byte-matched body `RwCore_004c5890` (`re/match/s19/004c5890.c`, 100%, IDENTICAL
in the object build). With the knob off it returns 0 and adds no librw binding (L-124, L-129 (a)). On both knob boots the
RW core's own caller 0x004c5f60 reaches the matched body through the entry. This happens inside RwEngineStart and before
the module 0x40a fault, and the self-test passes 11 of 11 checks.

Branch `orch/RWC-BIND-4C5890`, stacked on `orch/RWC-CORE-RUN` cf738ac9b (not merged yet). Gates: G-CTRL / B not run,
because this is a top-down row (WORKER_RULES, L-127). No tracker edit, no BINDINGS.tsv row (section 5).

## 1. What changed

| file | change |
|---|---|
| `RwCore/RwCore_Bind_004c5890.cpp` (new, exe only, `RVA-LINT: ORIGTOP-ONLY`) | `void* RwCoreBind_TexDictionaryCreate_004c5890(void)` (C linkage). Knob on: it reads RwGlobals [0x007d3ff8], the module offset [0x007d4054], the texdict freelist [+off+0xc] and the list head [+off+0] before the call. It then calls `RwCore_004c5890` and checks the record against the source text: `rwObjectInitialize(d, 6, 0)` (type 6, subType 0, flags 0, privateFlags 0, parent 0), `rwLinkListInitialize(&d->texturesInDict)` (d+8 next = prev = d+8), and `rwLinkListAddLLLink(&TGL(texDictList), &d->lInInstance)` (d+0x10 next = old head, d+0x14 prev = list, list next = d+0x10, old head prev = d+0x10). It logs one `[RWC_CORE]` line per call. Knob off: one log line, returns 0 (the matched body's own result when the freelist allocation fails). The file is not a body: the comment above the function names no RVA, so lint_rva_bodies does not count a second body |
| `verify/rwccorerun_20261111/gen_rwcore_tus.py` | new `CALL_VIA` map, `{'004c5890': RwCoreBind_TexDictionaryCreate_004c5890}`. In every TU other than the RVA's own, the name binds to the entry. The RVA's own TU (`RwCore_004c5890.cpp`) still compiles the body as `RwCore_004c5890`. The entry's file joins the fixed block of `exe_sources.rsp` |
| `RwCore/RwCore_004c5f60.cpp` | the one line the generator changes: `#define FUN_004c5890 RwCoreBind_TexDictionaryCreate_004c5890`. Regenerating with the patched generator gives a diff in this file only (ignoring line endings) |
| `mashedmod/exe_sources.rsp` | one line, `"RwCore\RwCore_Bind_004c5890.cpp"`, after `RwCore_Hand.cpp` |

Game callers: 0x00555360 (Rt2d open) is the only one named in L-129. On this base it is not compiled (it is a stand-in in
HUDINIT-A, and its body is in NEARMATCH-555360 4503d7205). When that row restacks, it replaces
`#define FUN_004c5890 MashedRw_TexDictionaryCreate_004c5890` (`Match/PortTop_00555360.cpp:36`, whose target L-129 (a)
removed) with `#define FUN_004c5890 RwCoreBind_TexDictionaryCreate_004c5890`. The entry's knob-off branch is not exercised
on this base, because no knob-off caller exists yet.

Generator note: a full regeneration also rewrites the order of the 235 generated lines in `exe_sources.rsp` (145 lines
moved between runs) and writes LF line endings. That changes link order, and code layout moves sim trajectories (memory
`sim-trajectory-is-code-layout-sensitive`). So this row commits only the committed order plus the one new line, and the
regenerated TUs other than 0x004c5f60 are not committed (they differ only in line endings). The order instability is a
generator defect for a later RWC row. It is not fixed here.

## 2. Builds and checks

| check | result |
|---|---|
| default (`MASHED_NO_DEPLOY=1`, VS2022) | Build OK |
| object build (`MASHED_DDKOBJ=1`) | Build OK. units 818, DDK objects 775, the same 20 `fail:` refusals as the parent |
| `ddkobj.py check` | IDENTICAL 689, DIFFERENT 97, not-in-map 3. `BYTECHECK.tsv` equals `verify/rwccorerun_20261111/BYTECHECK.tsv` line for line (sorted diff empty, exe_va included), so the parent rows are unchanged. 0x004c5890 `_RwCore_004c5890` is IDENTICAL (160 B, 1 reloc masked). 0x004c5f60 is DIFFERENT at +0x53 as before (provisional, 11 bytes) |
| `scripts/lint_rva_bodies.py` | OK, 0 new (89 known, D4 burn-down 4, ORIGTOP-ONLY 85). The first build failed the lint on a draft that had an `// 0x004c5890` comment above the entry (DUP-IN-TARGET + CROSS-TARGET, 2 new). The comment was reworded and the lint went back to the baseline |

## 3. Knob boots

These are the four RWC-CORE-RUN arms, unchanged (`run_arms.sh`). The private root is `D:/MashedWT/RWC-BIND-4C5890_root`,
with a junction from `original` to `D:/MashedWT/PORT-TOP-04J_rootK/original`. They ran as one launch inside
`mashed_lock.py run` inside `test-slot`. The slot waited 600 s for memory, then started under the cap. The common env was
`MASHED_ORIG_TOP=1 MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600`.

| arm | exe | result |
|---|---|---|
| vs_off | VS2022 | rc 0, 13.6 s, 600 frames |
| vs_core | VS2022 + `MASHED_RWC_CORE=1` | rc 0xC0000005 after 0.6 s (module 0x40a, as the parent) |
| ddk_off | object build | rc 0, 10.4 s, 600 frames |
| ddk_core | object build + `MASHED_RWC_CORE=1` | rc 0xC0000005 after 0.5 s (module 0x40a, as the parent) |

Knob off, compared with `verify/rwcdevice_20261111/logcmp.py`:
- vs_off against the parent's vs_off (`D:/MashedWT/RWC-CORE-RUN_data/vs_off.log`): 1133 / 1133 lines, 0 differ.
- ddk_off against the parent's ddk_off: 1133 / 1133, 0 differ.
- vs_off against ddk_off: 1133 / 1133, 0 differ.

Knob on (`KNOB_LINES.txt`): the same sequence as the parent, plus one new line between RwEngineOpen and the fault:

```
vs_core  [RWC_CORE] RwTexDictionaryCreate 0x004c5890 call 1 on the RW core (RwCore_004c5890) = 016EA584; RwGlobals 01647690,
         module offset 0x3F4, texdict freelist 007D4030, list head 01647A84 -> 016EA594; self-test PASS (0 of 11 checks failed)
ddk_core [RWC_CORE] RwTexDictionaryCreate 0x004c5890 call 1 on the RW core (RwCore_004c5890) = 01664C6C; RwGlobals 015F02C0,
         module offset 0x3F4, texdict freelist 007D4030, list head 015F06B4 -> 01664C7C; self-test PASS (0 of 11 checks failed)
```

- The call is 0x004c5f60's `DAT_007d4080 = FUN_004c5890()` (texture module 0x408 constructor) during RwEngineStart's plugin
  registry init 0x004d8000. The texdict freelist it allocates from is 0x007D4030, the address section 4 of the parent
  RESULT names. The module offset is 0x3F4.
- The new list head equals the record + 0x10 (`lInInstance`): 016EA584 + 0x10 = 016EA594, 01664C6C + 0x10 = 01664C7C.
- The fault probe then reads current texdict [+0x10] = [0x007d4080] = the same record (016EA584 / 01664C6C). So the
  dictionary the matched body made is the module's default dictionary at the fault.
- Fault frames, resolved with `verify/rwccorerun_20261111/mapsym.py`. VS2022: `RwCore_004d4bf0+0x2A` <-
  `RwCore_004d4380+0x28E` <- `RwCore_004e18f0+0x39` <- `RwCore_004cd850+0x5C` <- `RwCore_004d8000+0x1D` <-
  `RwCore_004c2fb0+0x32` <- `RwCore_EngineStart+0x6`. Object build: the same chain, with `RwCore_004d4f90+0xA8` between
  0x004d4bf0 and 0x004d4380. This is the parent's fault (RWC-MATCH-MOD40A), unchanged by this row.

[UNCERTAIN] Whether the dictionary survives the rest of RwEngineStart, and the knob-on call from a game caller (0x00555360),
cannot be shown on this base: the boot ends at the 0x40a fault before any game code runs on the core. That needs a run
past module 0x40a (RWC-MATCH-MOD40A) and the NEARMATCH-555360 restack onto this entry. This row does not stretch for it.

## 4. Not done here, by scope

- No PREREG. This row adds no knob and changes nothing off the knob. Its arms are the parent's PREREG arms
  (`verify/rwccorerun_20261111/PREREG_RWC-CORE-RUN.md`), repeated as written.
- No tracker edit. 0x004c5890 is already C4 by match. This row is port-axis only.

## 5. For the restacks (NEARMATCH-555360, HUDINIT-B)

- In `Match/PortTop_00555360.cpp`: `#define FUN_004c5890 RwCoreBind_TexDictionaryCreate_004c5890`, and declare it
  `extern "C" void* RwCoreBind_TexDictionaryCreate_004c5890(void);` (C linkage, cdecl, no arguments).
- BINDINGS.tsv: the 0x004c5890 row (NEARMATCH-555360's line 532) should name `RwCoreBind_TexDictionaryCreate_004c5890`
  with note "RwCore/RwCore_Bind_004c5890.cpp: knob on = matched body RwCore_004c5890 (re/match/s19/004c5890.c); knob
  off = 0". This row adds no BINDINGS row because no game caller is compiled on this base.
- With the knob off, 0x00555360 gets 0 for its dictionary (DAT_00912a20). Before L-129 it got a librw dictionary. Whether
  0x00555360 and its readers handle 0 is for that row to check.

## 6. Raw data

`D:/MashedWT/RWC-BIND-4C5890_data`, with sha256 in `RAW_SHA256.txt`: the 4 arm logs, the arm driver transcript, both build
logs and both exes.
