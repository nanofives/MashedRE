# RESULT EQUIV-FU-48E820 (2026-11-12): the s15 source of 0x0048e820 now byte-matches. C4 by match is queued, and the index bridge is gone

L2 worker `cmwdk1hb96bxcsc1b3lht78wj` (claude3, Opus 5.5) for lead `cmwddbai85n02sc1bwkkpmtb7`. Branch
`orch/EQUIV-FU-48E820` from `orch/integration` `9ae0f0ec1`. The equiv.py used is the integration copy at `eee2fc626`.
That is the EQUIV-NORETURN-RECHECK commit, already merged in `3ef5b2da2`.

## Verdict

**GREEN on the source and tracker legs.** The finding is fixed in the source, so no wrapper needs recording.

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
