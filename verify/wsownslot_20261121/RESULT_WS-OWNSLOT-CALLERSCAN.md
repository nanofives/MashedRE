# RESULT WS-OWNSLOT-CALLERSCAN (run 55, 2026-11-21)

Worker cmwr0nozv08mhp121147yw7ye (claude2, claude-opus-5-5 medium), lead cmwr0mi2n088jp121jgz9oroy (REPLAY).
Branch `orch/WS-OWNSLOT-CALLERSCAN` from `orch/integration` `39ee297f2`.

Row kind: replay row, applies L-180 (b) (Mariano 2026-11-21): a store into the body's own incoming parameter slots
counts as frame for the write-set leg when a static scan of every caller shows no caller reads that slot after the
call, cited per body. An unknown or indirect caller keeps the slot as caller stack.

Not run: build, ddkobj check, binder selftest, G-CTRL, B, any game arm. No new stepped run was needed: every passing
body already has an IDENTICAL stepped run from PROV-INSITU-REF1 / PROV-INSITU-REF1-WS2. No ddkobj unit changed, so no
DIFFERENT-unit classification applies. Nothing is bound.

## Verdict: GREEN

- 11 own-slot-only bodies with image stores (WS2_CLASSES.tsv subclass "caller-stack own-arg-slot (esp/ebp-relative)",
  image > 0 in WS_CUT.tsv) each have a verdict below.
- **5 promoted** under L-128 through `re/promotions/WS-OWNSLOT-CALLERSCAN.tsv`: `0x00420870`, `0x00470670`
  VehicleControlUpdate, `0x0046b540` VehicleSpawnInit, `0x00441b30` CamEntryBuild441b30, `0x0041a400`.
- **6 stay:** 4 fail the caller scan, 1 has no IDENTICAL stepped run, 1 has an open STUB row.
- C4 counts, reported apart: 5 replay-verified (`c4=frida-replay`), 0 byte-matched.

## Legs for the 5 promoted bodies

```
install-check: ok (prov_install.json jmp_ok true at the RVA in each cited stepped run: ws_g01ab 0x00420870, ws_g01bb 0x00470670, ws_g07 0x0046b540 and 0x00441b30, ws_g09 0x0041a400)
body-executed: yes (calls stepped / calls in the verdict run: 0x00420870 10 / 10876, 0x00470670 10 / 10263, 0x0046b540 10 / 20, 0x00441b30 7 / 7, 0x0041a400 2 / 2)
no STUB: no open STUBS.md row names any of the 5 in column 2 (0x0046b540 appears only in resolved S-2971)
write-set: covered (every observed store is frame, compared image, or the body's own incoming parameter slot counted as frame under L-180 (b) with the caller scan cited per body below; image > 0; each stepped run replays IDENTICAL-FOR-600 against ref1 / ref2; no stepped run of the body shows another escape; L-38 / L-40)
IDENTICAL-FOR-600: re/frida/prov_compare.py against ref1 / ref2 (verify/ro_20261116/REFERENCE_RO-REF-NAME.json), body live, no stepping, PROV-INSITU-REF1 verdict runs b2_035802_004_3 (0x00420870), b1_023950_003_6 (0x00470670), b2_035802_027_3 (0x0046b540), b1_023950_014_6 (0x00441b30), b2_035802_044_3 (0x0041a400); verify/provinsitu_ref1_20261121/VERDICTS.tsv
```

## The caller scan (`ownslot_callerscan.py`, output `CALLERSCAN.json`, log `callerscan_stdout.txt`)

Run on `original/MASHED.exe.unpatched` (SHA-256 `BDCAE093...3C0E`, the anchor).
`py -3.12 verify/wsownslot_20261121/ownslot_callerscan.py verify/wsownslot_20261121/CALLERSCAN.json`, rc 0.

Per body:
1. **Direct callers:** every `E8`/`E9 rel32` in .text that resolves to the body, confirmed by capstone at that address
   (5-byte call) and by membership in the containing body (linear sweep or CFG walk from its start,
   `re/analysis/function_inventory.csv`, each body bounded by the next inventoried start).
2. **Address taken:** every absolute-dword occurrence of the body's VA in the image. Any hit fails the body (indirect
   or unknown caller), except an unaligned hit outside .text whose two covering aligned dwords are both read as x87
   `[disp32]` operands in .text (a float-constant straddle, reported with the reading instructions).
3. **Read after the call:** from each call's return address the caller is walked over its control-flow graph with esp
   tracked relative to esp at the call (push, pop, add/sub esp, `mov esp, ebp`, leave, callee `ret imm16`), and ebp
   located by a depth walk from the caller's entry. A path is clean when every slot byte is below esp, overwritten, or
   at ret. Any memory read overlapping a live slot byte is READ. A `pop r32` over a slot byte is a read, accepted only
   when the popped register is written before any read on every path. A call made while slot bytes are still above esp
   (deferred cleanup) is accepted only when the callee is direct and its static arg reach (highest entry-relative byte
   it touches through esp/ebp, with no address of an argument taken) stays below the slot (PASSED_OVER). Everything
   else (indirect call or jump, esp copied, address of the slot taken, unknown callee cleanup, unknown ebp) is
   UNRESOLVED and fails the body.

Positive control: the same walker over every 7th direct call site in .text (first-argument slot) gave READ 18,
POPREAD_LIVE 11, POPREAD_DEAD 160, RELEASED 2364, UNRESOLVED 812. Spot check: `0x0040fde7 fld dword ptr [esp]` is a
real read. The walker can also over-report: `0x00418ee3 fstp dword ptr [esp]` (a write) came out READ because
capstone marks the operand read. Both errors fail a body, none passes one.

| rva | slot (entry esp) | direct sites | address refs | scan |
|---|---|---|---|---|
| 0x00420870 | +0x4 | 0x0040dad0 in FUN_0040da50: PASSED_OVER `0x0040dad6 call 0x40d8f0` (slot at callee entry+0x8, reach entry+0x8), RELEASED `0x0040dae8 add esp, 0x18` | 0 | PASS |
| 0x00407e20 | +0x10, +0x14 | 0x004087c5, 0x004087ec in FUN_00408610: RELEASED `0x004087d0 add esp, 0x18`, `0x004087f1 add esp, 0x18` | 0 | PASS |
| 0x00470670 | +0xc | 0x00471076 in FUN_00470c70: RELEASED `0x0047107b add esp, 0x10` | 0 | PASS |
| 0x0047bb10 | +0x4 | 0x0047c1c6 in FUN_0047c160: RELEASED `0x0047c1cb add esp, 0x10` | 0 | PASS |
| 0x0046b540 | +0x4 | 0x0040ed68 in FUN_0040e590: PASSED_OVER `0x0040ed6e call 0x46baa0` (entry+0x8, reach entry+0x8), RELEASED `0x0040ed78 add esp, 0x18`; 0x0040ff8c in FUN_0040feb0 (not in the inventory, starts after FUN_0040fc00's jump table 0x0040fe70..0x0040fead): PASSED_OVER `0x0040ff94 call 0x41eda0` (entry+0xc, reach entry+0xc) and `0x0040ff9c call 0x45ba00` (entry+0x14, reach entry+0xc), RELEASED `0x0040ffa6 add esp, 0x24` | 0 | PASS |
| 0x00441b30 | +0x8 | 0x004426cd in FUN_00442600: RELEASED `0x004426d4 add esp, 8` | 0 | PASS |
| 0x0041a400 | +0x4..+0x7 (bytes) | 0x0041a906 in FUN_0041a8d0: RELEASED `0x0041a919 add esp, 0x18` | 0 pointers; 1 straddle at 0x005cc9e5 .rdata, inside x87 constants 0x005cc9e4 = 0x41a40000 (`0x00406b71 fcomp dword ptr [0x5cc9e4]`) and 0x005cc9e8 = 0x41d80000 (`0x00406b59 fcomp`, `0x004654b2 fadd dword ptr [0x5cc9e8]`) | PASS |
| 0x00463640 | +0x4 | 0x00466d04 in FUN_00466b50: deferred cleanup into `0x00466d0a call 0x4645c0`, whose arg reach is unbounded: `0x004645d1 lea edx, [esp + 0x14]` takes the address of its argument 1 and passes it to `0x4645da call 0x46cbb0` | 0 | FAIL (UNRESOLVED) |
| 0x00420290 | +0x4 | 0x00420354 RELEASED; 0x0042055d in FUN_00420420: `0x00420577 call dword ptr [eax + 0xf4]` while the slot is above esp | 0 | FAIL (indirect call) |
| 0x00450b10 | +0x1c | 3 of 4 RELEASED; 0x004271e8 in FUN_004270f0: `0x004271f7 call dword ptr [edx + 0x20]` while the slot is above esp | 0 | FAIL (indirect call) |
| 0x005c2300 _strncat | +0xc | 107 of 108 RELEASED; 0x0058acd3: deferred cleanup into `0x0058acec call 0x4a2b60` (variadic, `0x004a2b6b lea eax, [ebp + 0x10]`) | `0x004d85c4 mov dword ptr [edx + 0xd8], 0x5c2300` (stored into `[DAT_007d3ff8] + 0xd8`) | FAIL (address taken) |

Group check: in each cited stepped run no caller above and no PASSED_OVER callee is a ported member (members from each
run's `prov_install.json`), so the original's code is what runs at every scanned site.

## Legs per body

Stepped runs: `--prov-wlog` stepping with the JMP live, compared against ref1 / ref2
(verify/ro_20261116/REFERENCE_RO-REF-NAME.json). Unstepped verdict runs: PROV-INSITU-REF1 `VERDICTS.tsv`.

| rva | name | stepped run (replay) | calls stepped | stores: frame / image / own slot | other escape in any stepped run | install check | body executed (verdict run calls) | no STUB | verdict |
|---|---|---|---|---|---|---|---|---|---|
| 0x00420870 | FUN_00420870 | ws_g01ab IDENTICAL-FOR-600 (WS2) | 10 | 40 / 40 / 4 | none (ws_g01_rep, ws_g01a decode the same 84 stores) | jmp_ok true, ws_g01ab | yes (10876, b2_035802_004_3) | none in STUBS.md | **promoted** |
| 0x00470670 | VehicleControlUpdate | ws_g01bb IDENTICAL-FOR-600 (WS2) | 10 | 0 / 110 / 10 | none (ws_g01_rep, ws_g01b same 120 stores) | jmp_ok true, ws_g01bb | yes (10263, b1_023950_003_6) | none | **promoted** |
| 0x0046b540 | VehicleSpawnInit | ws_g07 IDENTICAL-FOR-600 (REF1) | 10 | 50 / 680 / 10 (740 stores) | none | jmp_ok true, ws_g07 | yes (20, b2_035802_027_3) | S-2971 names it but is resolved (struck) | **promoted** |
| 0x00441b30 | CamEntryBuild441b30 | ws_g07 IDENTICAL-FOR-600 (REF1) | 7 | 56 / 231 / 7 (294 stores) | none | jmp_ok true, ws_g07 | yes (7, b1_023950_014_6) | none | **promoted** |
| 0x0041a400 | FUN_0041a400 | ws_g09 IDENTICAL-FOR-600 (REF1) | 2 | 0 / 6 / 8 bytes (14 stores) | none | jmp_ok true, ws_g09 | yes (2, b2_035802_044_3) | none | **promoted** |
| 0x0047bb10 | FUN_0047bb10 | ws_g02 IDENTICAL-FOR-600 (REF1) | 10 | 100 / 10 / 60 | none | jmp_ok true, ws_g02 | yes (4426) | **open S-0555** (column 2 `0x0047bb10`, caller 0x0047c160) | stays: no-STUB leg |
| 0x00407e20 | FUN_00407e20 | none IDENTICAL: ws_g01_rep, ws_g01b, ws_g01ba, ws_g01baa, ws_g01baab all DIVERGED@302 (WS2 "Splits") | 10 | 240 / 30 / 90 | n/a | jmp_ok true, ws_g01baab | yes (10663) | none | stays: stepping-sensitive (L-180 DECISION 2 kept (a)) |
| 0x00463640 | FUN_00463640 | ws_g01ab IDENTICAL-FOR-600 | 10 | 10 / 84 / 10 | | | | | stays: scan UNRESOLVED |
| 0x00420290 | FUN_00420290 | ws_g01ab IDENTICAL-FOR-600 | 10 | 0 / 10 / 10 | | | | | stays: scan, indirect call |
| 0x00450b10 | HudIm2DQuad | ws_g02 IDENTICAL-FOR-600 | 10 | | | | | | stays: scan, indirect call |
| 0x005c2300 | _strncat | ws_g06 IDENTICAL-FOR-600 | 10 | | | | | | stays: address taken (indirect callers) |

- Stores of the 6 REF1-run bodies are WS_CUT.tsv (stores, image) and `esc_detail_ref1groups.json` (only escape is the
  own slot). For 0x0046b540 / 0x00441b30 / 0x0047bb10 the frame count is stores - image - own slot.
- Write-set leg for the 5 promoted bodies: covered (every observed store is frame, own slot now counted as frame under
  L-180 (b) with the scan above, or compared image; image > 0; the stepped run replays IDENTICAL-FOR-600; no stepped run
  of the body shows another escape; L-38 / L-40).
- `0x0041a400` is a thin row: 2 calls in the race, both stepped. `0x00441b30`: 7 calls, all stepped. `0x0046b540`:
  10 of 20 stepped.
- Earlier hooks.csv rows: 0x00420870 C2, 0x00470670 C3 (demoted from C4 by C4-NOTAG-87-REEARN, match 98.0%),
  0x0046b540 C3 (same demotion, 59.2%), 0x00441b30 C3, 0x0041a400 C2. Their port columns are not touched (L-93).
- `0x0041a400` is a listed provisional body (`re/match/PROVISIONAL.tsv`, NOT-PROVEN, source
  `re/match/s03/h1_0041a4a0.c`). Provisional is not C4 by itself; the replay form is the route here (L-128).

## re-classify (dry run, no --apply on a branch)

`py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/WS-OWNSLOT-CALLERSCAN.tsv`: 5 pass, 0 refused, 0 skipped
[by route: frida-replay=5]. Would prepend C2->C4 0x00420870, C3->C4 0x00470670, C3->C4 0x0046b540, C3->C4 0x00441b30,
C2->C4 0x0041a400. The merge train applies it with --apply --strict.

## Tool limits ([UNCERTAIN], register line added)

The scan sees direct calls and absolute-dword address uses. It cannot see a call target computed arithmetically, nor
a pointer to a lower caller local indexed up into the slot. Test: one line in `re/orchestrator/UNCERTAIN_REGISTER.md`.

## Checks run

| check | result |
|---|---|
| anchor SHA-256 of original/MASHED.exe.unpatched | BDCAE093...3C0E, matches |
| caller scan, 11 bodies | 7 PASS, 4 FAIL (table above), rc 0 |
| positive control (every 7th .text call site) | READ detector fires (18 READ, 11 POPREAD_LIVE); over-reports only |
| install check (prov_install.json jmp_ok) | true for all 7 scan-passing bodies in their cited runs |
| STUBS.md column 2 | S-0555 open for 0x0047bb10; S-2971 resolved for 0x0046b540; none for the rest |
| group membership of callers / PASSED_OVER callees | none ported in the cited runs |
| c4_reclass batch dry run | 5 pass, 0 refused |
| mashed_lock status | not needed: no build, ddkobj, selftest or game arm ran |

## Not covered

- 0x00463640: the arg reach of 0x004645c0 through the pointer passed to 0x0046cbb0 (a second-level scan of 0x0046cbb0's
  use of that pointer would settle it).
- 0x00420290, 0x00450b10: the vtable calls at 0x00420577 / 0x004271f7 cannot be resolved statically.
- 0x0047bb10: open STUB S-0555 (a re-classify of that row first).
- 0x00407e20: no IDENTICAL stepped run.

## Data (SHA-256; no new raw per-frame data, all reads)

```
d63daa57a0ce213094cb7db53afce0e1d8e9f1a7706699a632091f13eca257b4  verify/wsownslot_20261121/CALLERSCAN.json
4336a1e55a98de8f8dd8a4c7eef594e2da94930cae692cbaa22a7266c138c891  D:/MashedWT/PROV-INSITU-REF1-WS2_data/esc_detail_ref1groups.json
3483232b36a4baa1a37fe7571b156a5f7a1c3a2697519f6a019bd251e4262509  D:/MashedWT/PROV-INSITU-REF1-WS2_data/wsdec_ws_g01ab.tsv
33c6085356f9599f4b2af11228bdc3153812f7036030e13525c5cb2f39e2a4af  D:/MashedWT/PROV-INSITU-REF1-WS2_data/wsdec_ws_g01bb.tsv
fe6f2ab4de99557ae1407e00da5d618c9efa5818a61d5d0bd575efd64ff73214  D:/MashedWT/PROV-INSITU-REF1-WS2_data/wsdec_ws_g01_rep.tsv
c08fc33599b1f3ead96b3b3a9cd9ebbd32989307c930b6cdd326860640fd1241  D:/MashedWT/PROV-INSITU-REF1-WS2_data/wsdec_ws_g01a.tsv
57e1a26d5e295f360a431ecd19aaadbb1f877cb1b88ce7f837da048f55bc06e6  D:/MashedWT/PROV-INSITU-REF1-WS2_data/wsdec_ws_g01b.tsv
8a94c4b7f6e9bb695a2fb2f4f8b2aa17b50e15ddceaf2ae72d89ea3759de7932  D:/MashedWT/PROV-INSITU-REF1-WS2_data/ws2_state.jsonl
3415af493a548bce695bf619ad6f9042e8159a65a0098cd69c7fafd68396acdb  D:/MashedWT/PROV-INSITU-REF1-WS2_data/ws_g01ab/prov_install.json
69c7109aa5cc443dd93ed9658cfb4f3f78f09d31c15b2b3f8664d1d0aeadbfcb  D:/MashedWT/PROV-INSITU-REF1-WS2_data/ws_g01bb/prov_install.json
42d5375126bc367b47dead6a3c9265057abfd18d1977e638b1fa34e46c61e110  D:/MashedWT/PROV-INSITU-REF1_data/ws_g07/prov_install.json
863f40d36cf06c1ac077d8d9075e4f94bcdc40d712257004ff60b45bd15629e9  D:/MashedWT/PROV-INSITU-REF1_data/ws_g09/prov_install.json
ce2c19ce4e6d2e667c8edb9129f49f6bb860aeecdbe922ed644009fa081d2dfc  D:/MashedWT/PROV-INSITU-REF1_data/ws_g02/prov_install.json
3fd0c8ea424a75ce0e676f2e47eb57aab608917014bab6029335e3e6ba63949f  D:/MashedWT/PROV-INSITU-REF1-WS2_data/ws_g01ab/wlog.bin
5c32f4ccd001560037d033b2a28fb3415555320b9a8d994ae0e59879b652e2ce  D:/MashedWT/PROV-INSITU-REF1-WS2_data/ws_g01bb/wlog.bin
02725093f8aa1e6e224c3a82dd9847332349db89fba28b47e15215bb14ae9e36  D:/MashedWT/PROV-INSITU-REF1_data/ws_g07/wlog.bin
2dc84a724d80170a3a387916d0608631aaf3112e103172123b093e5c367ea793  D:/MashedWT/PROV-INSITU-REF1_data/ws_g09/wlog.bin
```
