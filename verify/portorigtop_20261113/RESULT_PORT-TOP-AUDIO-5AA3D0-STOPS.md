# RESULT PORT-TOP-AUDIO-5AA3D0-STOPS: the init-arm stops from 0x005AA3D0 on

## Verdict: in progress (checkpoint 1 done; both builds now stop at 0x005A6340)

Base: orch/integration @ 810042e0e. Branch orch/PORT-TOP-AUDIO-5AA3D0-STOPS. Top-down row: both builds, ddkobj check,
knob boot (init arms), self-tests. Gates: not run (top-down row). No menu walk: no menu code changed.
equiv.py = the merged copy (tool commit 1ed7ceb1a, no diff on this branch).

## Stops passed (one line each)

| # | stop | how it was passed | checkpoint |
|---|---|---|---|
| 1 | 0x005aa3d0 | **admitted** (L-126): source reshaped 86.0% -> 93.0%, `EQUIV-IF-CALLERS-IGNORE(ECX)`, callers 2/2 ignore; bound | 1 |
| 2 | 0x005aa330 | **provisional** (54.7%, NOT-PROVEN call-args at 0x005aa350, masked-high-bits tool limit); bound with 0x005aa3d0 (0x005a9c50's other named stop) | 1 |
| 3 | 0x005b36b0 | **byte match**: source rewritten from the listing, BYTE-IDENTICAL 57 B, relocheck OK; promotion row; bound (0x005aa3d0's callee) | 1 |

## 1. Checkpoint 1: 0x005aa3d0 (admitted), 0x005aa330 (provisional), 0x005b36b0 (match)

### 1.1 0x005aa3d0
- Old source (86.0%) loaded field +0x10 / +0x14 into a local and formed the loop pointer after the ushort count test;
  the DDK sank the base load below the test, and equiv.py reported `NOT-PROVEN branch-condition` at 0x005aa400: the
  compared expression was `and32(ins32(X, ld16(count)), 0xffff)` on both sides with X = the register whose low half
  takes the count (ours param_2's register, orig the +0x4c pointer). The and clears X, so this was a form gap, not a
  value difference.
- New source: `puVar4 = (undefined4 *)(*(int *)(iVar5 + 0x10) + 0x10);` before the count load (same for +0x14 in the
  else arm, 0x005aa430). match.py 93.0% (regs 96.5%, ops 96.5%, 57/57 ins). Residual: the original keeps the base in
  ecx and forms the pointer with `lea esi,[ecx+0x10]`, ours loads into esi and adds 0x10. Four other spellings scored
  61.4% to 93.0%.
- equiv.py: `EQUIV-IF-CALLERS-IGNORE(ECX) [REG,VALUE-FLOW] callers 2, scan says ignore: 2/2: 0x005a9c5e:ignores
  0x005a9c66:ignores` (both call sites are in 0x005a9c50). Every caller ignores, so it is admitted with no decision row
  (L-126). equiv_dyn.py cross-check with ecx ignored: default `INCONCLUSIVE same=0 inc=32 diff=0`, seeded
  `SAME same=32 inc=0 diff=0` (32 conclusive runs in one mode, the EQUIV-ADMIT-1 coverage rule). Without the ignore
  list the seeded run reported `DIFFERS` on ecx only, the register the verdict names. ADMITTED.tsv row (source commit
  bc5fd77ae, tool commit 1ed7ceb1a). Not C4.

### 1.2 0x005aa330
- Source unchanged (`re/match/s25/005aa330.c`, 54.7%, 53/53 ins). equiv.py: `NOT-PROVEN call-args: region ours +0x1d /
  orig 0x005aa350 call 0x5ae030 arg 0`: both sides compute `[[p+0x4c]+0x10] + and32(ins32(X, ld16([[p+0x4c]+0xc])),
  0xffff)*24 - 0x18` and differ only in X (ours eax_in, orig `[p+0x4c]`). Same masked-high-bits form as 1.1, a tool limit.
  Five reshapes (base local, swapped product, tail locals, end pointer first, char pointer) did not move the score or
  the verdict. Listed provisional (PROVISIONAL.tsv) and bound. Not C4.

### 1.3 0x005b36b0
- The old source (53.1%) tested `param_1 != piVar1 + 1` (node + 4 bytes == list head), a different condition from the
  original, which compares the next node with the list end `param_1 + 4 bytes` (`lea edi,[eax+4]` 0x005b36c7,
  `cmp ecx,edi` 0x005b36d5). equiv.py had reported it as `NOT-PROVEN branch-condition` at 0x005b36d3.
- New source from the listing (guard `param_2 <= *param_1 && *param_1 != 0`, result `node - 0x20`, do-while walk that
  returns 0 at the list end): `match.py --bytes` 100.0% 29/29 ins, 57 B BYTE-IDENTICAL; relocheck OK checked=0;
  equiv.py `EQUIV [BYTE-IDENTICAL]`. Not in PROVISIONAL.tsv (no drop). Promotion row
  `re/promotions/PORT-TOP-AUDIO-5AA3D0-STOPS.tsv`; dry run `[1/1] 0x005b36b0 match PASS byte-identical 57 B, relocheck
  OK (0/0)`, no `--apply` on the branch. hooks.csv is C3 (hand body `Audio/AudioList_ag.cpp`, frozen hand path,
  untouched).

### 1.4 Binding
- `audiotick_bind_gen.py`: `PROV_BIND |= {'005aa330'}`; 0x005aa3d0 binds through ADMITTED.tsv, 0x005b36b0 through the
  promotion TSV. Generator fix: for a hooks.csv c4=match row it now prefers the note's `re/match/<d>/<rva>.c`
  (0x005aa510's note cites `re/match/s30/005aa480.c` first, and the plan had bound 0x005aa510 to that file).
- Roots unchanged (`roots.txt` sha256 73566306..., PORT-TOP-AUDIO-BODY-45D460's). `exe_body_index.py` re-run (1996 rvas;
  the committed index stays the base's).
- Plan: 276 bind (237 match, 17 admitted, 14 provisional, 8 replay), 64 reuse, 33 stops. Against the base plan:
  0x005aa3d0 / 0x005aa330 / 0x005b36b0 stop -> bind; 0x005aa510 / 0x005ad910 / 0x005ad9d0 show C4 now (applied by the
  merge train), same sources.
- Wrappers changed: `PortTop_005a9be0.cpp`, `PortTop_005a9c50.cpp`, `PortTop_005adb70.cpp`, `PortTop_AudioStops.cpp`
  (three stops removed). New TUs `PortTop_005aa3d0.cpp`, `PortTop_005aa330.cpp`, `PortTop_005b36b0.cpp`. newfiles.txt
  +3, rsp +3, allowlist +1 (ORIGTOP-ONLY DUP-IN-TARGET 0x005b36b0 with `Audio/AudioList_ag.cpp`). BINDINGS.tsv updated
  36. The generator rewrote the other wrappers with LF endings only (no content change); not committed.

### 1.5 Builds, check, arms
- VS2022: Build OK, rva-lint 327 known, NEW 0. Object build: Build OK.
- `ddkobj.py check`: IDENTICAL 1573/1614, DIFFERENT 25, not-in-map 16, units linked 1564/1782, refusals 11, provisional
  kept VS2022 207 units + 5 fns. DIFFERENT = the 24 of RESULT_PORT-TOP-AUDIOINIT-SPLIT-5A9970 checkpoint 4 (same list) +
  **0x005aa3d0** (`_PortTop_Bound_005aa3d0`, 28 bytes differ, first +0x25), newly linked here: the admitted 93.0% body.
  0x005b36b0 IDENTICAL. 0x005aa330 is provisional (kept VS2022). Fresh base check: section 1.6.
- Arms:

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005A6340** after 9.2 s | 0x0023AB85 = `_PortTop_Bound_005a62b0+0x35` (`mashed_re.map`, symbol at 0x0023ab50) | 05f2412afa83d4a0 |
  | ddk_init_audio | object | **0x005A6340** after 9.9 s | 0x00048743 = `_PortTop_Bound_005a62b0+0x33` (`mashed_re_ddk.map`, symbol at 0x00048710) | 405d50c29553232f |

  - The STOP line in both logs: `the matched 0x00000000 called original 0x005A6340 (DAT_006904e8 = 0, DAT_006904ec = 0)`.
    0x005a6340 is a plan stop (C2, no admitted / provisional / match source).
  - **Self-tests PASS in both arms:** portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367 failed (188 slots, 11 bound),
    code-trap self-test pages 459/459 and read / write / execute ok.

### 1.6 Fresh base check
(filled in below once run)

## Next (not done yet)
- 0x005a6340 (both builds), then the census window (`verify/census_20261116/audio_window_65.tsv` @ bbbe54319).
- 0x005ad420 (74.5%, guard spelling landed in bc5fd77ae, NOT-PROVEN call-regargs ecx at 0x005ad4e5): provisional when
  the arms reach the 0x00632c2c entries.

## Arms command
`ARMS_STEP=PORT-TOP-AUDIO-5AA3D0-STOPS ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIO-5AA3D0-STOPS_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIO-5AA3D0-STOPS -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
Private root: robocopy of `D:/MashedWT/PORT-TOP-AUDIOINIT-SPLIT-5A9970_root`, its old `mashed_re.log` removed.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIO-5AA3D0-STOPS_data/`, sha256)
Checkpoint 1:
- `cp1/vs_init_audio.log` 05f2412afa83d4a06ecf3bc0a029b303e489bff2e646c0ceda3181e7f778f6ba
- `cp1/ddk_init_audio.log` 405d50c29553232147c55cb09e35b014e4190943428ed648f2550bfac59e0413
- `arms1.out` 49a211d32f1c8466d6cef924ee968d3b65059392b93691ca17037224fd8d13d6
- `build1_vs.log` 12870b249d21fb937365b90de4be2bc3906154ce29f8cda87dd8394e4c3db95d
- `build1_ddk.log` 7062ea45e7a7148bb4914978bb40f9f37197a774ad29786fc048b00421e110c1
- `ddkcheck1.log` 6028784f6c6eaa8f8b4aca5410829faef723b2fc838bc80c109d29f6fd817e61
- `bytecheck1.tsv` 8833963692ed00bbc21a5a9ff1cd87fdd79b1087760cc88a4a3811df7130b94d
- `plan1_dry.txt` d059b0739916df4c5c4fd1e124bcf2546ea54390e4267e3ea340f61abf99eb6f
- `plan1_write.txt` 4518f0722b57a76affcfcda46d2734aa8088c15fe9bbf1cb832a185e9eee7973
- `equiv_a3d0.txt` df65d593c35331cdddbda54e5b29b62f9b9ebd30071345708c2b3fb57cb5565a
- `equiv_a330.txt` 02e8bc07eb7928da86676bbbde3f669faca5c5320c28d7fa7593f5e731056522
- `a3d0_xcheck_default.txt` 1c62b99adae9b49346ce99a544be39b0c485a16371a1c5965db7a13322544f73
- `a3d0_xcheck_seeded.txt` efd6a4cfef64cb55b0c4f25efc51fe114b6f550d1e130b298481cd25fdc42ddb
- `005b36b0_before.c` 902dc90b198a1eccb42d19b0640da66e2018eccf219b7001761ca302de38c380
- `b36b0_match_bytes.txt` 61f256eee4a4bceee41451d67f09558a740f968112806803b61c39a99aec2b94
- `b36b0_relocheck.txt` 73eff4d6c451cf509953e491a045f4188c6ad9c276962c754dce518da696cac4
- `c4_dry1.txt` 8e991f19b3a6d3a7c5bc82d3d265bcb2d1ebbc5a30fbae4bd60bd70b2d674fb1
- `exeidx1.json` 6f7403c4e9476ef48f7d1f2c871e843254d444ea8c41cc14e871d576df6e4513
- `roots.txt` 73566306c2eb5600447befbb32c4667ff8465d6d18e268986f50ac1fe77142bb
- `sync1.txt` eef80ba63a0ed94a64d6d5a1d791a8b15ef3f3f868d398e4b9a4dc16bbfc9bde
