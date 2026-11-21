# RESULT FIX-00420420-CRASH (run 55, 2026-11-21)

**Verdict: GREEN.** The cause is **the body**, not the U-9677 collision. `re/match/s05/00420420.c` declared one parameter
and passed `param_1` to `FUN_0041f330`. The original passes **param_2** there. With both changes below the source is a
**100.0% byte match** (relocheck OK), and the fixed body runs the ref1 replay **IDENTICAL-FOR-N 600 of 600** in 2 of 2
solo runs.

## 1. Solo re-run of the crashing arm (step 1 of the brief)

- Recipe: `verify/provinsitu_ref1_20261121/run_arm.py` argv exactly. Private root `D:\MashedWT\PROV-INSITU-REF1_root`,
  the same as the sweep. Driver: `verify/fix_00420420_crash_20261121/arm.py`.
- Before the runs: `mashed_lock.py status` = `free 'mashed_machine'`. No `MASHED.exe` or `mashed_re.exe` process was
  live (filtered by command line, at the start and again inside the solo window).
- Isolation: the whole set ran in one `mashed_lock.py run --solo --ttl 3600 --label FIX-00420420-CRASH` window, with
  `test-slot` outside it. Each boot took `mashed_machine` inside the window.

| run | body | result |
|---|---|---|
| c0 | none (control) | rc 0, 600 ticks, IDENTICAL-FOR-N 600 of 600 vs ref1/ref2 |
| b1 | bodies309.json 0x00420420 (old source, 79.8%) | **CRASH**, read 0x8 at eip 0x004e66db, clock call n=3593. The warp was at N=3523, so this is load call 70 of 108, before race tick 301. 0 ticks, rc 6. |

That makes 4 of 4 crashes in total (3 in the sweep, 1 here). This one had no other Mashed process and no other piz load
live, on a private root. So it is **not** a cross-process piz collision, although the eip equals the U-9677 signature.

**First tick:** the crash comes in the warp-tick-300 race load, at clock call 70 after the warp. That is before any race
tick was hashed. `fault-counts` shows the body ran once (`["0x00420420",1,0]`).

## 2. Cause (static, cited)

- **Original 0x00420420.**
  - `0x00420433` loads `EBX = [esp+0x270]` = param_1.
  - `0x0042045c` loads `ESI = [esp+0x284]` = **param_2**. At that point esp = frame 0x268 + 3 saved registers + 2
    pushed args, so `+0x27c` = the return address and `+0x284` = arg 2.
  - `0x00420466` calls `FUN_0041f330` with ESI still holding param_2.
  - `0x00420472` then runs `xor esi,esi`.
- **0x0041f330 takes its id in ESI.** At `0x0041f341` it runs `cmp dword ptr [ecx], esi` over the table at
  `0x005f3828`, stride 0x84.
- **The only caller passes two args.** At `0x0040d1fb`: `push esi; push ebx; call 0x420420; add esp, 8`. The caller
  source `re/match/topm01/0040d110.c:106` already calls `FUN_00420420(i, iVar2)`.
- **Old body.** At +0x14 it loads `ESI = [esp+0x274]` = param_1 and calls `0x41f330` with that, so the id lookup ran on
  the wrong value. In the crash frame (`D:/MashedWT/PROV-INSITU-REF1_data/rep2_061516_001_1/crash.jsonl` and the new
  `b1/crash.jsonl`):
  - `FUN_0042a5d0` got `0x005f37b8` = entry `0x005f37a8` + 0x10 = "Shuriken0". The stack buffer reads "Shuriken0.dff".
  - It returned 0.
  - Then `FUN_004b3fc0(NULL, ...)` (body +0xfc, return address body+0x101) reached `0x004e66d0`, where
    `mov eax,[eax+8]` faults at `0x004e66db`.
- **Disassembly compare** (`verify/fix_00420420_crash_20261121/disasm_cmp.py`, run against `MASHED.exe.unpatched`).
  Up to the crash call, the old body equals the original except for this argument and the EBX/ESI role swap.
- [UNCERTAIN] Why `FUN_0042a530` returned 0 for "Shuriken0" (whether that name is absent from the loaded vehicle piz)
  rests on reading `0x0042a5d0..0x0042a636`. Its test is filed in `re/orchestrator/UNCERTAIN_REGISTER.md`. The fix does
  not depend on it.
- The header's h2 note ("reloads param_1 into ESI for the FUN_0041f330 call") was a misreading of `0x0042045c`. The new
  header records the correction.

## 3. Match raised (L-168: raise the match first)

- Changes to `re/match/s05/00420420.c`:
  1. `void FUN_00420420(int param_1, int param_2)` and `ent = FUN_0041f330(param_2);`
  2. `DAT_0063d830[param_1] = 1;` moved before `DAT_0063e498++;`. This only changes store order: the two stores go to
     different addresses, 0x0063d830[0..3] and 0x0063e498. After change 1 this order was the last residual, at 99.7%.
- `py -3.12 re/tools/match/match.py re/match/s05/00420420.c --bytes`:
  - `0x00420420 _FUN_00420420: 100.0% (regs 100.0%, ops 100.0%) 317/317 ins`
  - `1099 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)`
  - Toolchain: ddk, `/O2 /GS /Ob1`. The head copy scored 79.8%.
  - `0x0041fe10` in the same file stays at 100.0% and byte-identical.
- `py -3.12 re/tools/match/relocheck.py re/match/s05/00420420.c`:
  - `0x00420420 OK checked=62 unchecked=2`. The two unchecked symbols are `@__security_check_cookie@4` and
    `___security_cookie`, the /GS cookie.
  - `0x0041f330`, `0x0041f360` and `0x0041fe10` are OK.
- The body is not inline asm.
- Promotion: `re/promotions/FIX-00420420-CRASH.tsv`, route `match`, as a TSV only. I did not run `c4_reclass --apply`.
- `re/analysis/match_ledger.tsv:517` still reads 79.8 / REFUSED. The ledger regenerator updates it, not this row.

## 4. Fixed body in the replay (confirmation)

- `py -3.12 re/tools/prov/provlink.py --rvas 0x00420420 --out D:/MashedWT/FIX-00420420-CRASH_data/bodies_fix.json`
  gave status OK, size 1104, unresolved 0. Its code differs from the bodies309 blob from offset 0x13 on. The `pct` field
  it prints (79.8) comes from the stale ledger, not from this compile.
- Two arms (f1, f2) ran in a second solo window with the same recipe and `--crash-catch 20`.

| run | install | executed | compare vs ref1/ref2 | compare vs c0 |
|---|---|---|---|---|
| f1 | 1 site, 0 failed | 1 | IDENTICAL-FOR-N 600 of 600 (menu 300, race 300) | 600 of 600, 0 allowed blocks |
| f2 | 1 site, 0 failed | 1 | IDENTICAL-FOR-N 600 of 600 (menu 300, race 300) | 600 of 600, 0 allowed blocks |

- The compare tool was `py -3.12 re/frida/prov_compare.py <run> <ref1> <ref2>`, with the refs at
  `D:/MashedWT/RO-REF-NAME_data/ref1` and `ref2`, and c0 from this row.
- The raw `ticks.bin` files differ byte for byte between c0, f1 and f2, while prov_compare reports them identical. The
  differences are confined to the masked words (the boot class / MASK rows that `RESULT_PROV-INSITU-REF1.md` lists).

## Checks run

| check | result |
|---|---|
| `mashed_lock.py status` before each arm set | free, both times |
| live Mashed process check (command line) | none, both windows |
| solo arm c0 | rc 0, IDENTICAL 600/600 |
| solo arm b1 (old body) | CRASH 0x004e66db read 0x8, n=3593 |
| match.py --bytes (fixed) | 100.0%, byte-identical |
| relocheck.py (fixed) | OK |
| provlink.py (fixed) | OK, 0 unresolved |
| solo arms f1, f2 (fixed body) | rc 0, executed 1, IDENTICAL-FOR-N 600/600 vs ref1/ref2 and vs c0 |

**Not run:**
- Replay gates (this is a replay / match row).
- ddkobj object build. `0x00420420` is not in the object build or the port compile: `s05/00420420` is not referenced
  from mashedmod or the ddkobj inputs. So no ddkobj unit changed, and there is no newly DIFFERENT unit to classify
  against `log\orch\run55_headcheck\`.
- G-CTRL / B (not part of this row kind).

## Raw data (D:/MashedWT/FIX-00420420-CRASH_data), SHA-256

```
5672e52e17d2cca3d46c223f9d8b622fd2b03800e370420435cfb21b0710db9a  bodies_fix.json
fb328adccf9572d02c6426b569deb6b0172c7c77b58d038217e0fc86273ee90e  c0/ticks.bin
1e0adfcd113f80f4af7bc3fe66bee08f02f25d83024806354c6a062a38241f9c  f1/ticks.bin
cea155d66c7691c3e4d06696a91faea6757b51ee3c3a043b470f6e1008fa2462  f2/ticks.bin
7523a5ceea58acba4c8ca7196957ad86de4f086760d9d44bb71833d717d12662  b1/crash.jsonl
41640fbb18a39f5b7d7b4310756a8477b7ba202de95e4361d5c3ead879a62ebb  PROV-INSITU-REF1_data/bodies309.json (old body)
```

Driver logs: `D:/MashedWT/fix00420420_driver.log` (c0, b1) and `D:/MashedWT/fix00420420_driver_fix.log` (f1, f2).

## Unfinished scope

- **frida-replay C4 tag.** The replay leg is IDENTICAL with the JMP live and the body executed. The Write-set leg
  (L-38 / L-40 / L-54) was not run, so I make no `c4=frida-replay` claim. C4 rests on the match route.
- **Applying the promotion.** `re/promotions/FIX-00420420-CRASH.tsv` goes to the re-classify / c4_reclass step on
  integration.
- **The sweep's VERDICTS row.** `verify/provinsitu_ref1_20261121/VERDICTS.tsv:242` (CRASH) still describes the old
  body. A re-sweep with the new source would update it.
- **The UNCERTAIN register line** on why `FUN_0042a530` returned 0 for "Shuriken0".
