# RESULT EQUIV-FU-48E820 (2026-11-12): the s15 source of 0x0048e820 now byte-matches. C4 by match is queued, and the index bridge is gone

L2 worker `cmwdk1hb96bxcsc1b3lht78wj` (claude3, Opus 5.5) for lead `cmwddbai85n02sc1bwkkpmtb7`. Branch
`orch/EQUIV-FU-48E820` from `orch/integration` `9ae0f0ec1`. The equiv.py used is the integration copy at `eee2fc626`.
That is the EQUIV-NORETURN-RECHECK commit, already merged in `3ef5b2da2`.

## Verdict

**GREEN on all legs: source, trackers, both builds, ddkobj and both knob boots.** The finding is fixed in the source, so no wrapper needs recording.

| Check | Before (`orch/integration`, src sha256 `6e7679c6ffdf8c8b`) | After (src sha256 `ed6028bfda6e5298`) |
|---|---|---|
| `match.py --bytes --only 0x0048e820` | 85.5% (regs 100, ops 100), ESI/EDI swapped in 18 instructions | **100.0%, 124/124 ins, 664 B, 0 differing unmasked bytes: BYTE-IDENTICAL** |
| `relocheck.py` | OK 48/48 | **OK checked=48 unchecked=0** |
| `equiv.py --only 0x0048e820` | `NOT-PROVEN call-regargs: region ours +0x244 / orig 0x0048ea64 call 0x48cee0: (('edi', 0x3f800000),) \| (('edi', 0x10),)` | **`EQUIV [ENC,VALUE-FLOW]`** |
| `c4_reclass.py batch --tsv re/promotions/EQUIV-FU-48E820.tsv` (dry run) | n/a | `[1/1] 0x0048e820 match PASS byte-identical 664 B, relocheck OK (48/0)` (C2 -> C4) |

## The fix

- 0x0048cee0 reads the emitter index in EDI: Ghidra shows it as `unaff_EDI`. `re/match/s14/0048cee0.c` already models
  this as `static void FUN_0048cee0(int i)`, because cl gives a static callee a register convention.
- The s15 source used to declare `void FUN_0048cee0(void)` and call it with no argument. It now defines the same
  static callee, with the s14 body text, and calls `FUN_0048cee0(i)`.
- With that change, cl keeps `i` in EDI and the hoisted 1.0f in ESI, as the original does. This removes the ESI/EDI
  swap that 423 permuter variants could not move (L-118 byte table).
- The static copy is itself BYTE-IDENTICAL to 0x0048cee0 (140 B, 30/30 ins). I checked this in a scratch copy with a
  temporary `// MATCH: 0x0048cee0` marker. It is not this file's matched symbol.
- Under `MATCH_PORT` (VS2022), the static copy and its declarations sit inside `#ifndef MATCH_PORT`. The port gets
  `void FUN_0048cee0(int i);` instead. The DDK compile does not define `MATCH_PORT`, so the matched bytes come from
  the static form.

## Port side (`mashedmod/src/mashed_re/Match/PortTop_0048e820.cpp`)

- `FUN_0048cee0` now binds straight to `PortTop_PS3_Cee0_0048cee0(int i)` (Match/PortTop_0048cee0.cpp). The source
  passes `i` explicitly.
- The PARTICLE-3-SWAP index bridge is removed. That bridge was `s_emitter` set from the 0x004b6480 pointer, plus
  `PortTop_PS3_Call0048cee0(void)`.
- The 0x004b6480 bridge stays only as a type adapter, from `(void *, unsigned int)` to
  `Orig_004b6480(unsigned char *, int)`.
- The one-time log line no longer prints "last emitter". `git grep` finds no consumer of that line.
- For the object build, the wrapper adds three defines: `FUN_0040bb30 Search40bb30`, `FUN_004770c0 Orig_004770c0`
  and `FUN_00476cb0 Batch476cb0`. These are the bindings Match/PortTop_0048cee0.cpp uses. They exist because the DDK
  object of s15 carries the static 0x0048cee0 copy, and with it those three names as undefined symbols. ddkobj renames
  them from the wrapper's `#define` lines. Under VS2022 the defines expand nothing.
- `git grep` finds no other user of `PortTop_PS3_Call0048cee0` or `s_emitter` outside verify/ and re/analysis/.

**Not run, and why:**

- `mashed_lock.py status` showed the merge train's PRIORITY-HOLD (`merge-train:20261112_093808`) for this whole
  session. WORKER_RULES forbids `build.bat` under that hold.
- Not run as a result:
  - the VS2022 build;
  - the object build with `ddkobj.py check`;
  - the knob boot self-test. Its expected line is "0x0048cee0 built 17 of 17 records".
- What did run: the VS2022 `cl /Zs /TP /std:c++17 /DMASHED_STANDALONE` syntax pass over PortTop_0048e820.cpp and
  PortTop_0048cee0.cpp. Both are clean.
- The brief scoped this row as no-build. The port file had to change because it `#include`s the s15 source. Without
  the change, it would no longer compile against that source.
- **The lead or the merge train should run both builds, `ddkobj.py check` and the knob boot once the hold clears.**
- G-CTRL and B were not run. This is a top-down-only file (`RVA-LINT: ORIGTOP-ONLY`) and the default path does not
  execute it.

## Build legs (lead request, run 2026-11-12 11:02, after the hold cleared)

| Check | Result |
|---|---|
| Default build + object build (`MASHED_NO_DEPLOY=1`, `MASHED_DDKOBJ=1`, `mashedmod\build.bat`) | `=== Build OK ===`. `mashed_re.exe` sha256 `7edc34427d33b3b651b121b149fe69e43a443c1e5355e118d6df58e176f3b0fc`. `mashed_re_ddk.exe` sha256 `ef0d5556fb9fec8c86b9a5571cfebcc8d0d9e2c93133392b245b93abb1bcaa07`. Log: `D:/MashedWT/EQUIV-FU-48E820_data/build.log`, sha256 `a9922628be90d8acc3270c3e9497bc62923be65c4f25dc89bdae80637c4b18f2` |
| rva-lint | First build: 2 NEW findings on 0x004b6480. A comment starting `// 0x004b6480 bridge:` above the type adapter bound the adapter as a second body. The comment was reworded. Then OK: 96 known, 0 new |
| `ddkobj.py check`, 0x0048e820 | BYTECHECK.tsv `0x0048e820 _Orig_0048e820 0x0001cca0 672 19 46 0 IDENTICAL` (s15 unit status `ok`) |
| ddkobj, local 0x0048cee0 copy | The s15 unit keeps the static copy as `local_helpers: _FUN_0048cee0`. So the DDK 0x0048e820 calls its own DDK copy with the EDI convention. The copy's callees bound to `_Search40bb30`, `_Orig_004770c0` and `_Batch476cb0` through the wrapper defines. `ddkobj.py check` covers only MATCH definitions, so `verify/equivcheck_20261112/check_local_cee0.py` applies the same masking to that symbol: `0x0048cee0 _FUN_0048cee0 exe_va=0x0001cc10 size=144 relocs_masked=12 abs_data_refs_exact=2 diff_bytes=0 IDENTICAL` |
| ddkobj totals | IDENTICAL 716/796, DIFFERENT 77, not-in-map 3, units linked 786/834, refusals 47. Neither RVA is in the DIFFERENT list. `re/match/s14/0048cee0.c` stays refused, `fail:static-register-convention` as before |
| BINDINGS | `verify/equivcheck_20261112/update_bindings_equivfu48e820.py` (bindings_rows.apply): 0x0048e820 `near-match` -> `matched`, level 4 (near-match 3 -> 2 rows overall) |
| Knob boots (2026-11-12, after the reboot, test-slot outside, mashed_lock inside, same exe hashes as above, no rebuild) | **PASS.** `knob_on` (default exe): rc=0 after 57.3 s, `knob_on.log:292` `0x0048cee0 built 17 of 17 records`, `:1111` 0x0048cf70 on 17 records, `:1337` WinMain returned 0 after 3000 frames (log sha256 `ab95b2daaceb22ce787ca3808fc879ce17529db252b36e043aa1ffb8a36f0435`). `knob_on_ddk` (DDK exe): rc=0 after 56.3 s, the same three lines at 292 / 1111 / 1337 (log sha256 `53dbe82662eefe7f5f3ccde28dc7c71f20862325db677692c68acf23b3e98fc3`). Logs in `D:/MashedWT/EQUIV-FU-48E820_data/`. The first DDK attempt failed in the harness before launch (relative `KNOB_EXE` with `Popen(cwd=root)`, FileNotFoundError); `run_boots.sh` now passes an absolute path |

## HANDOFF, closed after the reboot (2026-11-12 ~11:55, LIVING-PC reboot coordinator: token leak, automatic reboot pending)

- **Done and committed:** the source fix, the port wrapper, both builds, ddkobj check, the local-copy check and the
  BINDINGS row.
- **Boots:**
  - The boot run was stopped before any game started. It had a test slot and was queued on `mashed_lock` behind
    RO-MENU-REPLAY.
  - I stopped it and killed my own PID tree (2940 / 4064) so the slot is free for the reboot.
  - No `mashed_re.exe` of mine was ever spawned.
- **Next, after the reboot:**
  1. Rebuild, because `mashedmod/build` is in this worktree and survives the reboot, but rebuild if in doubt. Use
     `cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"`, and only when
     `mashed_lock.py status` shows no PRIORITY-HOLD or SOLO.
  2. Run `test-slot bash verify/equivcheck_20261112/run_boots.sh > D:/MashedWT/EQUIV-FU-48E820_data/boots.out 2>&1`
     in the background, with a long timeout. It runs two knob boots, `knob_on` (default exe) and `knob_on_ddk`
     (DDK exe), each with `MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000` on the private root
     `D:/MashedWT/EQUIV-FU-48E820_root`. Logs go to `D:/MashedWT/EQUIV-FU-48E820_data/knob_on*.log`.
  3. Self-test pass: in both logs, rc=0 and the line `[ORIG_TOP] rva 0x0048e820: matched body ran (EQUIV-FU-48E820);
     0x0048cee0 built 17 of 17 records`, plus the 0x0048cf70 line with 17. The reference is PARTICLE-3-SWAP
     `knob_on.log:346`, 17 of 17.
  4. Fill the Knob boots row above, commit, run `roadmap.ps1 set EQUIV-FU-48E820 review -Evidence ...`, and send the
     REPORT.

## Trackers and admission records

- **0x0048e820:**
  - `re/promotions/EQUIV-FU-48E820.tsv` (route match, evidence `re/match/s15/0048e820.c`) is committed for the
    merge train's `--apply --strict` (HB-TRACKER-AT-MERGE). hooks.csv is not edited here.
  - The L-118 per-function admission (ESI<->EDI near-match) is superseded by the byte match. The row was never in
    `ADMITTED.tsv`, because L-118 predates it, so there is nothing to withdraw.
- **0x00555360:** `re/match/ADMITTED.tsv:23` already records it as `EQUIV-IF-CALLERS-IGNORE(EAX)`, with caller scan
  `callers=1, scan says ignore: 1/1: 0x00552b79:ignores` (EQUIV-ADMIT-1). It is not plain EQUIV, so no edit is needed.

## Files

- `re/match/s15/0048e820.c`: the source fix.
- `mashedmod/src/mashed_re/Match/PortTop_0048e820.cpp`: bridge removed, object-build bindings added.
- `re/promotions/EQUIV-FU-48E820.tsv`: the C4 by match promotion for the merge train.
- `verify/equivcheck_20261112/RESULT_EQUIV-FU-48E820.md`: this file.
