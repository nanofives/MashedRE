# bregargs: warm-lane register args and a `jmp [IAT]` callee, 0x00427840 + 0x00495530 re-earned (2026-10-12)

Branch `work/bregargs` (base c00795d3). Triage `re/analysis/negctl/reearn_20261011/TRIAGE_2026-10-12.md` section B,
rows 0x00427840 and 0x00495530. Built with `MASHED_NO_DEPLOY=1`; the shared `original\mashed_re_dev.asi` was never
touched. No path2, no tracker edits.

## Verdict per row

| row | hook key | port vs the bytes | verdict |
|---|---|---|---|
| 0x00427840 FontText_UTF16WidenCopy | `font_text_utf16_widen_copy` | one register nit: `movzx ecx` where 0x00427860 has `movzx cx` (ECX bits 16..31 at return). Not consumed by any caller. Fixed, port now matches 0x00427840..0x00427875 instruction for instruction | every C2->C3 leg except path2 passed |
| 0x00495530 CreateDInputObject | `create_dinput_object` | matches 0x00495530..0x00495575, unchanged | every C2->C3 leg except path2 passed |

**Ready for path2: `font_text_utf16_widen_copy`, `create_dinput_object`.**

## The new capability

Both rows lacked an instrument, not a port. Two small extensions of existing machinery, no new arg_type:

| key | where | what |
|---|---|---|
| `reg_args` {reg: argIdx} | `scalars_to_scattered_globals` (diff_template.js), mirrored in verify_hook_install_template.js | register-only args, delivered on BOTH arms by `standins.js` `regThunk` (already used by `standin_object_call`, now exported). This is the triage's "eax_ecx_insert in the warm lane": the same register delivery, plus every feature of the scattered handler (scratch windows read back, seeds, `call_log`, `freeze_main`). Unlike `eax_arg`, a register arg is not also left on the stack. `reg_args` and `eax_arg` are exclusive |
| import spec `deref` {argIdx: 'a' \| 'w'} | `standins.js` import stand-in | log the string at that arg, not its address. 0x004987b0 formats into a stack buffer (0x004987d9) and hands it to OutputDebugStringA, so the address differs per arm and the text does not |
| per-test `import_ret` {api: v \| [..]} | `standin_object_call` tests -> `S.arm({impRet})` | that vector's import return, over the spec's install-time `ret`. Before, the return was fixed for the whole run |
| guard hint | `stub_at` / `call_log` install guards | when the target starts `FF 25` (a `jmp [IAT]` thunk), the refusal now names the route that works |

**Why the import route, not stub_at on the thunk.** The triage named two options: stub_at support for `jmp [IAT]`
thunks, or a pointer-valued observe. Frida's `Interceptor.replace` on 0x0049b300 follows `jmp [0x5cc014]` and plants
on `dinput8!DirectInput8Create`, process-wide. The import stand-in that already existed (plan item #3, `standins.js`)
swaps the IAT dword 0x005cc014 instead, behind a native gate that only diverts the A/B thread while armed, and every
other thread forwards to the real API. So the real DirectInput8Create never runs on either arm and nothing the game
sees changes. The precedent is `b11.py` (`window_msg_pump`, `window_show`). What was missing was a per-vector HRESULT
(`import_ret`) and readable debug-print text (`deref`).

How to use it:

1. Register-arg function with output buffers: `arg_type: scalars_to_scattered_globals`, `signature.args` one entry per
   register arg (`'pointer'` for a buffer), `reg_args: {eax: 0, ecx: 1}`, args `{scratch: off}`, `observe` the scratch,
   `scratch_fill` non-zero so missing / extra writes show, `fold_ret: True` for EAX.
2. Function whose callee is an import thunk: `arg_type: standin_object_call`, `object_alloc: 'frida'`,
   `heap_standin: None`, `import_standin: {'DLL.dll!Api': {args, ret, deref?}}` for every import the body reaches
   (also the ones inside game helpers it calls, like OutputDebugStringA under 0x004987b0), and per-test
   `import_ret`. A live global the body hands out goes in `observe_globals` with `seed: False`.

Fixture: `re/frida/negctl_fixtures/z_fix_bregargs.py` (replaces both entries whole). `re/frida/ARG_TYPES.md`
regenerated (line numbers only; it will churn again on merge, regenerate after).

## 0x00427840 FontText_UTF16WidenCopy

**Bytes** (0x00427840..0x00427875): `0x00427842 mov edi,eax` (src), `0x00427844 mov eax,[0x7d3ff8]`, `0x00427849 push edi`,
`0x0042784a mov esi,ecx` (dst), `0x0042784c call [eax+0xf4]`, `0x00427852 add esp,4`, `0x00427857 jle 0x42786e`
(signed), loop `0x00427860 movzx cx,[edi]; mov [esi],cx; add esi,2; inc edi; dec eax; jne`, `0x0042786f mov word [esi],0`,
`ret`. EAX on return: 0 after the loop, len when len <= 0.

**Port.** `HUD/HudBatch.cpp`, naked. One difference, `movzx ecx` vs `movzx cx`: ECX bits 16..31 on return are the length
getter's in the original and 0 in the port. All three callers (Ghidra boundary: 0x00427be0, 0x00427f00, 0x00428320)
call 0x00552d10 or reload ECX next (0x00427c04 -> 0x00427c09, 0x00427f32 -> 0x00427f37, 0x00428344 -> 0x00428357
`mov ecx,[0x67d838]`; 0x00552d10 reads neither EAX nor ECX before writing them), so it is not observable. Fixed for
byte faithfulness (commit 9a9e907b), and the built export disassembles to the same sequence.

**Fixture.** `reg_args {eax: 0, ecx: 1}`, src at scratch+0, dst at scratch+0x100, whole 0x400-byte scratch observed,
fill 0xA5, return folded. Vectors 0..4 use the LIVE RwGlobals (+0xf4 measures the NUL-terminated string): "Hello", "",
bytes 80 A9 FF E9 7F 01 (zero- vs sign-extension), "Z", 255 bytes. Vectors 5..10 point `*(0x007d3ff8)` at a fake at
scratch+0x300 whose +0xf4 is the `len` recorder (logs the src it was handed, returns the vector's length), under
`freeze_main`: len 3 of "ABCDEFGH", 0, -1, 0x80000000, 8 over "ABC" (copies the NUL and the fill), 1 over 0xFE.

**Prediction check.** Fingerprints of vectors 0..4 recomputed offline from the predicted dst bytes and return 0: 5/5
equal (`8281d07`, `437d161f`, `419e79a5`, `f67a1aed`, `29264415`). Vectors 5..10 log `len(s+0x0@a"ABCDEFGH")` etc. as
predicted. All 11 fingerprints differ from each other.

## 0x00495530 CreateDInputObject

**Bytes** (0x00495530..0x00495575): `push 0x5cfeb4; call 0x4987b0` ("Creating DInput object\n"),
`0x0049553d push 0; push 0x771e78; push 0x5d0a8c; push 0x800`, `0x0049554e push 0; call [0x5cc038]`
(GetModuleHandleA(0)), `0x00495556 push eax; call 0x49b300` = DirectInput8Create(hinst, 0x800, 0x5d0a8c, 0x771e78, 0),
thunk `0x0049b300 jmp [0x5cc014]`, `0x0049555e jge 0x495570` -> `mov eax,1; ret`, else
`push 0x5cfeac; call 0x4987b0` ("Failed\n"), `xor eax,eax; ret`.

**Port.** `Input/DirectInput.cpp`, unchanged. Calls 0x004987b0 and the thunk 0x0049b300 by address, GetModuleHandleA
through the .asi's own IAT (the stand-in patches both modules' slots: `exe:` and `asi:` counters in the run log).

**Fixture.** `standin_object_call`, `freeze_main`, imports GetModuleHandleA (ret sentinel 0x51510000, so the port must
pass its result through), DirectInput8Create (5 args recorded), OutputDebugStringA (`deref {0: 'a'}`). Seven HRESULTs:
0, 1, 0x7fffffff (return 1) and 0x80000000, 0x80004005, 0xffffffff, 0x8007000e (return 0 plus "Failed\n").
0x00771e78 observed with `seed: False` (it holds the live interface; neither arm writes it).

**Prediction check.** All 7 rows read exactly as predicted, e.g. idx 3:
`ret=0x0 ... log=[imp:OutputDebugStringA:a"Creating\x20DInput\x20object\x0a" imp:GetModuleHandleA:0x0
imp:DirectInput8Create:0x51510000:0x800:0x5d0a8c:0x771e78:0x0 imp:OutputDebugStringA:a"Failed\x0a"]`.

## Evidence (`re/analysis/negctl/session_20261012b/bregargs_evidence/`)

| leg | 0x00427840 `font_text_utf16_widen_copy` | 0x00495530 `create_dinput_object` |
|---|---|---|
| run_diff (final build) | GREEN 11/11 `fix_font_text_utf16_widen_copy.csv` (+ `calllog_*.csv`) | GREEN 7/7 `fix_create_dinput_object.csv` |
| --self-baseline | GREEN 11/11 `fix_*_selfbaseline.csv` | GREEN 7/7 `fix_*_selfbaseline.csv` |
| negctl (`negctl_20261012_204652.tsv`, `--any-conf --slots 1`) | real GREEN, baseline GREEN, null RED 11/11, ret RED 11/11, field_first RED 11/11, field_last RED 11/11, field_none CTRL-GREEN | real GREEN, baseline GREEN, null RED 7/7, ret RED 7/7, field_first NA, field_last NA, field_none CTRL-GREEN |
| witness, new channel | the 2026-05-27 defect restored (no `push edi` / `add esp,4`, re/analysis/menu_crash_scoping/REPORT.md): RED 11/11, live vectors AV on the port arm, fake vectors `len(@AV)` vs `len(s+0x0@a"...")` (`witness_red_font_text_utf16_widen_copy.csv`) | injected unsigned sign test (`static_cast<unsigned>(hr) < 0u`): RED 4/7, exactly the four negative HRESULTs, port `ret=0x1` and no "Failed" (`witness_red_create_dinput_object.csv`) |
| witness, old channel | the OLD entry (`arg_type none`, `crash_equal_ok`) on the same witness build: 10/10 "match", both arms `access violation accessing 0x0`, run_diff now says INCONCLUSIVE-DEGENERATE (`witness_oldchannel_font_text_utf16_widen_copy.csv`) | not re-run: the old entry calls the REAL DirectInput8Create on both arms (rule). Its blindness is on record: negctl_jobs_20261011.tsv `[create_dinput_object]` warm real DEGENERATE, re-earn b10 real=ERROR (stub_at guard refused) |
| restore | both TUs back, `touch`ed, rebuilt `HUD\HudBatch.cpp (source newer)`, `Input\DirectInput.cpp (source newer)`, then run_diff + self-baseline re-run (rows above). Row 1 re-run once more after the guard-hint template edit: same fingerprints | same |

Field classes NA on 0x00495530: the body stores nothing itself (its only memory effect is through DirectInput8Create,
which is stood in), so the kit finds no byte to flip. null and ret are RED, and the witness covers the branch.

**Callers / callees by Ghidra function boundary** (`re/tools/decomp_pc.py 0x00427840 0x00495530 --callers --callees
--no-decomp`, slot Mashed_pool0):
- 0x00427840: callees none (the length getter is the indirect `call [*(0x007d3ff8)+0xf4]`); callers 0x00427be0 (C2),
  0x00427f00 (C2), 0x00428320 (C3).
- 0x00495530: callees 0x004987b0 (C2), 0x0049b300 DirectInput8Create thunk (C2), GetModuleHandleA (import); caller
  0x004955b0 DInputInitPredicate (C3).

## Commits on `work/bregargs`

| commit | what | keep/drop |
|---|---|---|
| cff8be4a | run_diff: warm-lane `reg_args`, import stand-in `deref` + `import_ret`, regThunk export, jmp-[IAT] guard hint, ARG_TYPES.md | keep |
| 9a9e907b | HudBatch: 0x00427840 `movzx cx` | keep |
| (this commit) | fixture `z_fix_bregargs.py`, evidence, this note | keep |

No witness state is committed (the defects lived only in the working tree between two builds).

## Path2 handler keys (orchestrator runs these after merge)

- `font_text_utf16_widen_copy`: `scalars_to_scattered_globals` with `reg_args` (path2 mirror added in
  verify_hook_install_template.js, thunk through the installed JMP). path2_tests are vectors 0 and 2, live RwGlobals,
  no 0x007d3ff8 seed.
- `create_dinput_object`: `standin_object_call` (path2 already supported it). path2_tests HRESULT 0 and 0x80004005.

## Proposed re-classify transaction (not applied)

- `00495530,CreateDInputObject`: C2 -> C3. Append: "negctl re-earn 2026-10-12 (bregargs): C2->C3 re-earned. New
  instrument: standin_object_call with the IAT dword 0x005cc014 stood in (thread-gated, the real DirectInput8Create never
  runs) plus per-test import_ret and OutputDebugStringA text deref; GetModuleHandleA sentinel 0x51510000 proves the
  hinst pass-through. run_diff GREEN 7/7, self-baseline GREEN, negctl null/ret RED 7/7, field NA (no own store),
  witness RED 4/7 (unsigned sign test). Port unchanged. Evidence re/analysis/negctl/session_20261012b/bregargs_evidence/;
  result bregargs_RESULT.md." frida_diff pointer -> `.../bregargs_evidence/fix_create_dinput_object.csv`.
  C4 stays owed (path2 + canonical scenario).
- `00427840,FontText_UTF16WidenCopy`: C2 -> C3. Append: "negctl re-earn 2026-10-12 (bregargs): C2->C3 re-earned. New
  instrument: reg_args {eax:0, ecx:1} in the warm scattered lane (standins regThunk), dst read back from a 0xA5-filled
  scratch, live and recorder-backed length getter. Port nit fixed (movzx cx, 9a9e907b; unobservable by callers).
  run_diff GREEN 11/11, self-baseline GREEN, negctl null/ret/field_first/field_last RED 11/11, witness RED 11/11 (the
  2026-05-27 missing push src) where the old `none` entry read 10/10 match. Evidence re/analysis/negctl/session_20261012b/
  bregargs_evidence/; result bregargs_RESULT.md." frida_diff pointer -> `.../bregargs_evidence/fix_font_text_utf16_widen_copy.csv`.
  Its old C4 evidence (install-observe 2026-06-09) predates the demotion and is not re-used, C4 stays owed.
- No STUBS / UNCERTAINTIES / DEFERRED rows touched. No U-row proposed.

## Decisions needed

None. No forced input, no patched copy of MASHED.exe, no new dependency, nothing under `original\` changed.
Note for the merge: `ARG_TYPES.md` line numbers will conflict with other children's regenerations; regenerate after the
last merge rather than resolving by hand.
