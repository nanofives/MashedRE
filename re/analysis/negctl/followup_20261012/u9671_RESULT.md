# U-9671: FUN_0041c2d0 re-ported from the bytes (2026-10-12)

Branch `work/u9671`. Commits:

| commit | content | keep/drop |
|---|---|---|
| `8dd0ce22` | re-port of 0x0041c2d0, install enabled, fixture `z_fix_u9671.py`, plates corrected, evidence | keep |
| `5bdf69d8` | HudDispatchMode10 / HudDispatchMode5 call the port instead of the original 0x0041c2d0 | orchestrator's call after path2 on 0x0041c2d0 |
| (this note) | RESULT note only | keep |

No tracker edits, no path2, no C-level claim here.

## Verdict

**0x0041c2d0 FUN_0041c2d0: PORTED-GREEN, every C2->C3 leg except path2 passed.** ready for path2: `hud_dispatch_draw4`.

## The bytes (re-checked 2026-10-12)

capstone on `original/MASHED.exe.unpatched`, and the Ghidra pool decomp (`decomp_pc.py 0x0041c2d0`, slot Mashed_pool1,
released) agree: `(**(code **)(in_EAX[3] + 0x48))(in_EAX[3])` and the same for [1], [0], [2].

```
0x0041c2d0 push esi              0x0041c2e1 mov eax,[esi]
0x0041c2d1 mov esi,eax           0x0041c2e3 push eax
0x0041c2d3 mov eax,[esi+0xc]     0x0041c2e4 call [eax+0x48]
0x0041c2d6 push eax              0x0041c2e7 mov esi,[esi+8]
0x0041c2d7 call [eax+0x48]       0x0041c2ea push esi
0x0041c2da mov eax,[esi+4]       0x0041c2eb call [esi+0x48]
0x0041c2dd push eax              0x0041c2ee add esp,0x10
0x0041c2de call [eax+0x48]       0x0041c2f1 pop esi
                                 0x0041c2f2 ret
```

35 bytes. Block in EAX. For m = block[3], block[1], block[0], block[2]: `push m; call [m+0x48]` (one dereference: a
function-pointer field, not a vtable). Caller-cleaned (`add esp,0x10`). ESI saved/restored. EAX at `ret` = the last
member call's EAX. No guards. The U-9671 row's description of the bytes and of the three defects is confirmed.

## The port (`mashedmod/src/mashed_re/HUD/HudDispatch.cpp`)

Naked export `FUN_0041c2d0`: `push eax; call HudDispatchDraw4Body; add esp,4; ret`. The cdecl body calls
`*(HudMemberFn*)(m+0x48)` on block[3], [1], [0], [2] and returns the last call's result. Compiled code (capstone of the
built .asi): body `push esi; mov esi,[esp+8]`, three `push m; call [m+0x48]`, `add esp,0xc; pop esi`, then a tail
`mov [esp+4],m; jmp [m+0x48]` into the fourth call, which returns into the shim's `add esp,4; ret`. ESI preserved,
stack balanced, EAX = last member call's EAX. ECX/EDX at return are not part of the contract (no caller reads them:
both callers tail-jump, and 0x0040dfc0 does not read them after 0x0040e028 / 0x0040e043 / 0x0040e09e / 0x0040e0bd per
`re/analysis/negctl/reearn_20261011/fix_hud_dispatch_RESULT.md`). No float path, no register args besides EAX.

Install `RH_ScopedInstall(FUN_0041c2d0, 0x0041c2d0)` re-enabled (was MASS-DISABLED 2026-05-24).

## Fixture (`re/frida/negctl_fixtures/z_fix_u9671.py`, `__replace__` of `hud_dispatch_draw4`)

The old entry was `arg_type: none` + `crash_equal_ok`, EAX never set: both arms faulted at the first dereference, so
it measured nothing. New: `scalars_to_scattered_globals`, `eax_arg: 0` (both arms through `mov eax,[esp+4]; jmp`),
`freeze_main`, `fold_ret`, ret uint32.

* `recorder` (1 arg, mscdecl, returns 0) seeded at [m+0x48] of fake members at scratch+0x200+k*0x50. The log
  (scratch+0) is the dispatch order and member set. [m+0] stays 0, so a vtable-reading port faults.
* Every vector seeds a decoy block with disjoint, also-recording members, so a port that ignores EAX records a
  different log instead of faulting. Blocks at scratch+0x100 / +0x140 / +0x180, plus two vectors on the live mode
  blocks 0x0063c610 (0x0041a3e9) and 0x0063cda4 (0x0041c309), seeded and restored.
* Orders and aliasing: (0,1,2,3), (4,5,6,7), (3,2,1,0), (1,3,0,2) on a moved block, aliased (2,2,9,9), (5,5,5,5),
  (6,0,6,0), the two live blocks, and a scratch block with a live decoy. 10 vectors, 10 distinct fingerprints.

`hooks_registry.py`'s own comment above `hud_dispatch_draw4` still says "vtable[0x48]". Not edited here (the fixture
file is the per-batch override, so parallel children never edit the registry); the orchestrator may fix the comment.

## Evidence (`re/analysis/negctl/followup_20261012/evidence/`)

How run_diff picks the .asi in a worktree: `run_diff.py` uses `ROOT/mashedmod/build/mashed_re_dev.asi` with ROOT =
its own repo root, so in the worktree it loads the worktree build (`MASHED_NO_DEPLOY=1`, nothing deployed). The exe is
`original\MASHED.exe` through the worktree junction. Log line: `.asi loaded ... FUN_0041c2d0@0x6dc733d0`.

| leg | result | file |
|---|---|---|
| run_diff path1 | GREEN 0/10 | `u9671_diff_hud_dispatch_draw4.csv` |
| --self-baseline | GREEN 0/10 | `u9671_diff_hud_dispatch_draw4_selfbaseline.csv` |
| negctl real / baseline | GREEN / GREEN | `u9671_negctl.tsv` |
| negctl null | RED 10/10 | same |
| negctl ret | RED 10/10 | same |
| negctl field_first / field_last | NA (flips=0) | same |
| negctl field_none | CTRL-GREEN | same |
| witness: old body restored alone | RED 10/10 on boot 1 and boot 2, reimpl `access violation accessing 0x48` on every vector, original arm clean | `u9671_witness_oldport_boot{1,2}.csv` |

field_first / field_last read NA because the body makes no stores of its own (the only writes are the harness
recorder's), the same as the four sibling rows in `fix_hud_dispatch_RESULT.md`. null and ret are the discriminating
mutants.

Witness procedure: the old naked body (vtable deref, no pops) put back in place of the shim alone, built ("(source
newer)" for both targets), run_diff twice (two boots, RED 10/10 each). Then the fix restored from a copy, `touch`ed,
rebuilt with "(source newer)" for both targets, and run_diff + self-baseline re-run, so the committed CSVs are GREEN.
The AV address 0x48 is `[vtbl+0x48]` with vtbl = [m+0] = 0: the old port's defect (a) fires first.

Callers / callees by Ghidra function boundary (`decomp_pc.py 0x0041c2d0 --callers --callees`):
callers FUN_0041a3e0 HudDispatchMode10 **C3**, FUN_0041c300 HudDispatchMode5 **C3**; callees none direct (four
indirect `call [m+0x48]`). Nothing blocks C3 under the caller/callee gate.

## Second commit (`5bdf69d8`): Mode10 / Mode5 call the port

`call FUN_0041c2d0` with EAX = 0x0063c610 / 0x0063cda4 replaces `mov ecx,0x0041c2d0; call ecx`. Compiled code
confirmed (`call 0x100133d0` = the port export). On the rebuilt .asi:

| key | run_diff | self-baseline | distinct fingerprints | file |
|---|---|---|---|---|
| hud_dispatch_mode10 | GREEN 0/8 | GREEN 0/8 | 7/8 (two flag-0 vectors coincide by design) | `u9671_c2_diff_hud_dispatch_mode10*.csv` |
| hud_dispatch_mode5 | GREEN 0/8 | GREEN 0/8 | 7/8 | `u9671_c2_diff_hud_dispatch_mode5*.csv` |

## Proposed tracker changes (for the orchestrator, after path2)

* `hooks.csv` 0x0041c2d0 FUN_0041c2d0: C2 -> **C3** if path2 PASSES. Status `mapped` -> `impl`. frida_diff ->
  `re/analysis/negctl/followup_20261012/evidence/u9671_diff_hud_dispatch_draw4.csv`. Note: replace the "4-vtable
  EAX-thiscall dispatcher; dispatches EAX[3/1/0/2]+vtable[0x48]" text with "EAX-in block; push m; call [m+0x48] for
  m = block[3],[1],[0],[2] (function-pointer field, one deref); caller-clean add esp,0x10; ESI preserved; EAX = last
  call's EAX. 2026-10-12 re-ported from bytes (U-9671): run_diff GREEN 10/10, self-baseline GREEN, negctl null/ret
  RED 10/10, field_none CTRL-GREEN, witness old port RED 10/10 x2 boots; callers 0x0041a3e0/0x0041c300 C3.
  re/analysis/negctl/followup_20261012/u9671_RESULT.md".
* `UNCERTAINTIES.md` U-9671: **RESOLVED 2026-10-12** (re-ported from the bytes, 8dd0ce22; plates corrected; path2 by
  the orchestrator).
* hooks.csv 0x0041a3e0 / 0x0041c300 notes: if `5bdf69d8` is kept, append "calls the FUN_0041c2d0 port (U-9671)".

## Finding outside this row (not fixed; for the orchestrator)

**0x0041bc50 port `Sub0041bc50_HudRender29Dispatcher` (`HUD/HudBatch.cpp:326`, install MASS-DISABLED) carries the same
vtable misreading.** Its pairs do `mov eax,[edi]; push edi; call [eax+0x48]` (vtable deref). The bytes call the field
directly: `0x0041bc5a mov eax,[esi+0xbc]` / `0x0041bc60 push eax` / `0x0041bc61 call [eax+0x48]` /
`0x0041bc64 add esp,4`. It also starts at flag offset 0x10 where the bytes start at 0x1c (0x0041bc53), and implements
28 pairs where the sibling RESULT counts 29 (0x0041bc53..0x0041be95). Proposed new U-row: "0x0041bc50 port in
HudBatch.cpp does not match the bytes (vtable deref, flag order, 28 vs 29 pairs); re-port from the bytes before any
install. The fixture `hud_dispatch_slot2` in z_fix_hud_dispatch.py already drives 0x0041bc50 with field recorders."

No [UNCERTAIN] items for 0x0041c2d0.
