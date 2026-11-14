# RESULT DDKOBJ-REGCONV-FALLBACK-7 (run 45, section D4, tool row)

Branch `orch/DDKOBJ-REGCONV-FALLBACK-7`, based on `orch/RWC-FRAME-CLUMP` 6ad200304 (where the guard lives).
Top-down tool row. 2026-11-13.

**Verdict: GREEN.** All seven refused units were really wrong as the object build linked them before the guard. Each
one's in-object call sites pass arguments in registers, and the external they were repointed to takes them on the
stack (section 1). So the guard is right for all seven and is not narrowed. What changed is a recovery: a generated
**forwarder** keeps the helper's private convention inside the object and pushes the arguments on to the one exe body.
It brings back **2 of the 7** units with byte-identical callers: 0x004111c0 and 0x00427ca0. The other 5 stay refused
for a named reason (section 3).

| check | result |
|---|---|
| `ddkobj.py check` | IDENTICAL **1220/1253** (was 1216/1249), DIFFERENT 18 (the same 18 RVAs as RWC-FRAME-CLUMP, all provisional or the predecessor's 11), not-in-map 15, units linked 1202/1483 (was 1200), refusals 9 (externalized-static-register-convention 5, wrapper-redefines-globals 4) |
| `ddkobj.py selftest` | OK, 0 failures (3 new forwarder cases) |
| VS2022 build / object build | both OK (`build_vs.log`, `build_ddk.log`) |
| knob boot vs_off / ddk_off | rc 0 both, `WinMain 0x00492370 returned 0 after 600 frames`. logcmp: 1146 / 1146 lines, 1 differs (the cfglog clock line) |
| knob boot vs_frameN / ddk_frameN (N=120) | 0x5257F001 both after present 120 on the RW core. logcmp: 195 / 195 lines, 1 differs (the clock line) |
| **self-test** | **PASS on both builds**: `[RWC-DEVICE] selftest PASS (0 fail)` and the RwTexDictionaryCreate binding `self-test PASS (0 of 11 checks failed)` in vs_frameN and ddk_frameN |
| menu walk | not run (no menu code changed) |
| G-CTRL / B / 12 captures | not run (top-down tool row, knob off for those hashes) |

## 1. Per-unit verdict: what the externalized unit computed

Tool: `caller_cmp.py` (this folder), output `caller_cmp.txt`. For each unit it compiles the source with the recorded
DDK compiler and flags twice: as written (helper `static`), and with the helper non-static, where cl must use the
standard convention. It compares every other function of the object, and prints the helper's live-in registers read
from the original binary (`MASHED.exe.unpatched`) and the call sites in the original.

The question it answers: after externalization the helper copy is dead, and the in-object call sites are repointed to
the exe body, which VS2022 compiled with the standard convention. A call site is correct only if it already passes its
arguments the standard way. That holds when the caller's bytes are identical in both compiles.

| unit | helper | original helper reads a caller register | original call site | callers in the non-static compile | verdict |
|---|---|---|---|---|---|
| 0x004111c0 | FUN_004103a0 | eax at 0x004103a4 (`mov ebx, eax`) | 0x004112b9 `mov eax, esi`, call 0x004112bb | 0x004111c0, 0x0040fc00, 0x004102f0 DIFFERENT | wrong: the exe body would get its argument in EAX |
| 0x00427ca0 | FUN_00427880 | eax at 0x00427882 (`push eax`, the name to 0x004cc230) | 0x00427d41 `lea eax,[esp+0x28]`, call 0x00427d45 | DIFFERENT | wrong (proven in RWC-FRAME-CLUMP: `0x004cc230(2, 1, "")`) |
| 0x00427e00 | FUN_00427680 | edi at 0x004276d9 (`push edi`), esi at 0x00427745 (`fstp [esi]`), never written in the body | 0x00427e94 `lea esi`, 0x00427e98 `lea edi`, call 0x00427e9c | DIFFERENT | wrong: text and out pointers in ESI/EDI |
| | FUN_004277a0 | eax at 0x004277a1 (`mov si,[eax]`), ebx at 0x004277af (`mov ecx, ebx`), never written | 0x00427e29 `lea ebx`, call 0x00427e2d (eax from 0x00427e24's return) | | wrong |
| 0x00427f00 | FUN_00427680 | as above | 0x00427f88 `lea esi`, 0x00427f8c `lea edi`, call 0x00427f90 | DIFFERENT | wrong |
| | FUN_00427840 | eax at 0x00427842 (`mov edi, eax`), ecx at 0x0042784a (`mov esi, ecx`) | 0x00427f27 `mov eax,[esp+0x420]`, 0x00427f2e `lea ecx,[esp+0x18]`, call 0x00427f32 | | wrong |
| 0x00427ff0 | FUN_00427680 / FUN_004277a0 | as above | 0x00428094 / 0x00428098 `lea esi` / `lea edi`, call 0x0042809c; 0x00428029 `lea ebx`, call 0x0042802d | DIFFERENT | wrong |
| 0x00428140 | FUN_00427680 / FUN_004277a0 | as above | 0x004281a9 / 0x004281ad, call 0x004281b1; 0x00428169 `lea ebx`, call 0x0042816d | DIFFERENT | wrong |
| 0x00458920 | FUN_00458570 | eax at 0x00458573 (`push eax`), esi at 0x004585c9 (`fst [esi]`), esi never written | 0x004589bf `mov eax, esi`, 0x004589c1 `lea esi,[esp+0x18]`, call 0x004589dd | DIFFERENT | wrong |

Call-site addresses are the unit RVA plus the offset in the plain object, and the windows in `caller_cmp.txt` show
the original's bytes at those addresses (the units are 100% matches, so the plain object's caller is the original's).
In every unit the caller loads one or more registers right before the call and the helper reads them without writing
them first, so a standard-convention external reading stack arguments gets the wrong values. That closes the
[UNCERTAIN] in RESULT_RWC-FRAME-CLUMP section 8 for all six: each one computed wrong results in the object build
before the guard, by the same mechanism as 0x00427ca0. The check is static. No run compared those six bodies, and no
run is needed to show the argument mismatch. None of the six was proven fine, so the guard is not narrowed.

## 2. Recovery: generated forwarders (ddkobj.py, `regconv_forwarder_recompile`)

When a unit ends with `fail:externalized-static-register-convention` (default `DDKOBJ_REGCONV=adapter` mode only),
ddkobj.py now recompiles the source once with each refused helper's C-view body replaced by
`{ return __ddkfwd_<name>(<args>); }` and adds a declaration of `__ddkfwd_<name>` with the convention the VS2022
wrapper object gives the exe name (`wrapper_decoration`, as the DDKOBJ-REGCONV adapters do). cl then emits both sides
of the helper's private convention itself. The forwarder object is accepted only when **every other function** is
byte-identical to the plain compile, relocation targets included (`_masked_funcs`). Then the forwarder's definition is
renamed `<name>__ddkfwd` (local) and its external takes the exe-name form, so it binds like any other undefined
symbol (wrapper `#define`, global binding, exe by name). The unit then goes through the two normal attempts again. Any
refusal keeps the RWC-FRAME-CLUMP refusal and adds `forwarder refused: <reason>` to its evidence.

The two forwarders in the linked objects (`forwarder_disasm.txt`):

| unit | forwarder | bound to |
|---|---|---|
| 0x00427ca0 | `push eax; call; add esp,4; ret` | `_MatchS06_00427ca0_Sel00427880` (the frame-route selector) |
| 0x004111c0 | `push eax; call; pop ecx; ret` | `_SubStateHeldCountdown5_C` |

Both take the argument in EAX, as the original helper does (section 1), and push it to the cdecl exe body. In
BYTECHECK.tsv (this folder) the unit's matched functions are IDENTICAL: 0x00427ca0, 0x004111c0, and the two
register-convention MATCH statics of the 0x004111c0 unit, 0x0040fc00 and 0x004102f0 (`__ddkreg_` bodies). They are
the +4 in the IDENTICAL count.

Runtime evidence for 0x00427ca0 (ddk_frameN): the DDK body runs on the frame route and
`rva 0x00427ca0 ran: font 0x0067d838=08F64B94`, a non-zero font, where the pre-guard object build stopped on font 0
(RWC-FRAME-CLUMP d12 signature). ddk_frameN equals vs_frameN line for line except the clock. 0x004111c0 is not reached
in these boots (no race). Its recovery rests on the byte-identical callers and the forwarder listing only.

Also added: `local_statics_unbindable` in the build report, the callees whose absence made the local-static attempt
fail (the reason the unit reached the externalizing attempt at all).

## 3. The five units that stay refused

`DDKOBJ_REGCONV_FORWARDERS.txt` lists each with its evidence. For 0x00427e00, 0x00427f00, 0x00427ff0, 0x00428140
(helpers FUN_00427680 + FUN_004277a0 / FUN_00427840) and 0x00458920 (FUN_00458570), the forwarder changes the matched
caller's bytes: cl gives the forwarder a different register assignment than the real body (FUN_00427680 takes ECX
instead of ESI/EDI, `fwd_probe.txt`). Two other shapes were probed and also changed the callers (`fwd_probe.py
--shape=nop`: an empty external call first, which cl turns into a standard-convention forwarder; `--shape=keep`: the
parameters kept live across the forwarding call, which moves them to ESI/EDI/EBX in another order; `fwd_probe_nop.txt`,
`fwd_probe_keep.txt`). They keep their VS2022 bodies in the object build.

The path not taken: the local-static attempt fails only on 3 unbound callees per unit (FUN_005554d0, FUN_0042b8c0,
FUN_0042b8b0 for the FUN_00427680 group; FUN_004c39b0, FUN_004c3ac0, FUN_0046d4a0 for 0x00458920). Binding those in
the unit's wrapper would let the DDK object keep its own helper copy, which is the original's code. That runs a second
copy of the helper next to the exe body, against ddkobj.py's "one exe body" rule for port-excluded helpers, so it is a
design choice for the lead, not done here.

## 4. Changes

- `re/tools/ddkobj/ddkobj.py`: `FWD_PREFIX`, `forwarder_source`, `regconv_forwarder_recompile`; the attempt loop is a
  local `attempts()` run once on the plain object and once on the forwarder object; refusal evidence names the
  forwarder reason; `local_statics_unbindable` in the report; 3 selftest cases (int forwarder with the port view kept,
  void stdcall forwarder, two static definitions refused).
- `verify/ddkregconv7_20261113/`: this RESULT, `caller_cmp.py` + `caller_cmp.txt`, `fwd_probe.py` + `fwd_probe.txt` +
  `fwd_probe_nop.txt` + `fwd_probe_keep.txt`, `forwarder_disasm.txt`, `DDKOBJ_REGCONV_FORWARDERS.txt`, `BYTECHECK.tsv`, `RAW_SHA256.txt`.

No source under `re/match/` or `mashedmod/` changed, so the default VS2022 exe is the same build input as
RWC-FRAME-CLUMP. No librw binding added (L-124). No tracker edit, no promotion. No `.ps1` / `.sh` edit needed.

## 5. Raw data

`D:/MashedWT/DDKOBJ-REGCONV-FALLBACK-7_data` (arm logs, build logs, `arms.out`, `ddkobj_summary.json`), sha256 in
`RAW_SHA256.txt`. Private root `D:/MashedWT/DDKOBJ-REGCONV-FALLBACK-7_root`. Arms run with
`verify/rwcframeclump_20261113/run_arms.sh` (RWC_DATA / RWC_ROOT pointed here) inside test-slot, then
`mashed_lock.py run --label DDKOBJ-REGCONV-FALLBACK-7`. The test slot waited 10 min for memory and started under the
cap.
