# RESULT MATCH-L69-SWAP (run 27, 2026-10-31)

Verdict: **GREEN.** ShadowAB wrapper retired, the matched source is the RVA's only body (match.py 100% and relocheck OK on this branch), build lint clean, default gates unchanged. Not C4 here (exe-callers=0; MATCH-S01-RC after the merge, L-68 ruling); no re-classify.

L-69 (a): retire the ShadowAB wrapper on 0x00407600 (`PlayerPositionPtr`) and make the S01 byte-matched source
`re/match/s01/00407600.c` its only port body. No re-classify in this row (MATCH-S01-RC handles it after the
merge). Per the L-68 ruling it is C4 only if mashed_re.exe calls it, and it does not today: record **match evidence:
identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling).**

## Swap

| rva | port symbol | targets | install | x87 | removed | new body | match.py | relocheck |
|---|---|---|---|---|---|---|---|---|
| 0x00407600 | PlayerPositionPtr | both | 1 (moved) | yes (MatchS01 is already in x87_tus.txt; no float here) | Render/SlotObjectAccessors.cpp: `PlayerPositionPtr_impl` + the `ShadowAB::Run` wrapper + its install | Match/MatchS01.cpp (`#include re/match/s01/00407600.c`) | 100.0% 8/8 ins | OK checked=1 unchecked=1 (`_DAT_00639d80`) |

- `re/match/s01/00407600.c`: its extern block (`DAT_00639d80[]`, `FUN_0047d150` prototype) now sits under
  `#ifndef MASHED_RE_PORT`, the pattern the other S01 sources use. Body unchanged; match.py still 100% (re-run
  on this branch, ddk, `/O2 /GS /Ob1`).
- MatchS01.cpp: `PlayerPositionPtr` export prototype (`char* __cdecl PlayerPositionPtr(int)`), `#define
  FUN_00407600 PlayerPositionPtr`, `DAT_00639d80` = `(Rec639d80*)0x00639d80u`, callee `FUN_0047d150` = absolute
  `0x0047d150` (what the replaced body called), `RH_ScopedInstall(PlayerPositionPtr, 0x00407600)`.
- Same computation as the retired body: record read at `0x00639d80 + i*0xec + 0x44` (= the old
  `0x00639dc4 + i*0xec`), call `0x0047d150`, `+ 0x30`. The return type is now `char*` (was `std::uint32_t`), same
  register and the same value. The export name is unchanged, so the hooks_registry entry for `PlayerPositionPtr`
  still resolves.
- `rva_body_scan`: one body (Match/MatchS01.cpp:106), one install (Match/MatchS01.cpp:121). Build lint:
  `[rva-lint] ... allowlisted=16 NEW=0`, `[asm-lint] OK`.

What retiring ShadowAB loses: the in-process original-vs-port A/B at this call site (armed only with
`MASHED_SHADOW_AB=1`, in the .asi). The match makes it redundant. The four other ShadowAB sites in
SlotObjectAccessors.cpp stay.

Exe callers: none. The original's callers are 0x00446520 and 0x00464a50 (E8 scan cited in the retired block).
mashedmod/src has no call of `PlayerPositionPtr` by name or of `0x00407600` by address.

## Gates

Runner `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, A4_ROOT
`D:/MashedWT/MATCH-L69-SWAP_root` (robocopy of `original/`), wrapped in `test-slot` +
`mashed_lock.py run --label MATCH-L69-SWAP`, exe copied out of the build dir first.

| run | build | exe sha256/16 | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs merge_train.json ref |
|---|---|---|---|---|---|
| 1 | commit b9b90de0a | `ca2698bcf7d072c3` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |

H_DEF self-exited in 481 s, each B arm in 40-41 s, exit 0. No gate hash moved, so no DECISION.

## -Full

The train's `-Full` legs (vsum / e_launch, D2 x3, 13 tracks) are not run here: they are the train's pass for a
source row (row note `SOURCE -Full`). The branch touches `mashedmod/src` outside `full_set_paths`, so the train
must be run with `-Full` explicitly.

## Raw data (D:/MashedWT/MATCH-L69-SWAP_data, not committed)

```
ca2698bcf7d072c3d3f06439fe7841737c2b0691e70d246cadda446695d1013f  exe_g1/mashed_re.exe (build of b9b90de0a)
ff87173cd8d082c06de86830dcf768be27b252e86daabd1bd8b38aea08ea924b  build1.log
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K{0,1,26}.step.csv
08f4e0882f8daa82455e87e735974e8f7fd21d31836f45754565afa9901f10bb  arms_g1/H_DEF.log
f36d8c330de1a8d6171ea7d3f15f064295f104ee5b31a8405095471fbd0fb936  arms_g1.log
```
Source: `re/match/s01/00407600.c` b390c57019fb32f2 (after the guard edit).

## Next

MATCH-S01-RC: record 0x00407600 as matched + swapped, exe-callers=0, not C4 until a caller lands.
