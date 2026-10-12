# U-9675: FUN_0041bc50 re-ported from the bytes (2026-10-12)

Branch `work/u9675`. Commits:

| commit | content | keep/drop |
|---|---|---|
| `8e3c6b97` | re-port of 0x0041bc50 in `HUD/HudBatch.cpp`, install enabled, fixture `z_fix_u9675.py`, plate corrected, evidence | keep |
| `97e1560a` | port moved to `HUD/HudDispatch.cpp`; HudDispatchSlot2 (0x0041c0c0) calls the port instead of the original 0x0041bc50 | orchestrator's call after path2 on 0x0041bc50 |
| (this note) | RESULT note only | keep |

No tracker edits, no path2, no C-level claim here.

## Verdict

**0x0041bc50 FUN_0041bc50: PORTED-GREEN, every C2->C3 leg except path2 passed.** Ready for path2: `hud_rec29_dispatch`.

## The bytes

capstone on `original/MASHED.exe.unpatched` (`u9675_evidence/u9675_capstone_0041bc50.txt`) and the Ghidra pool decomp
(`decomp_pc.py 0x0041bc50 --callers --callees`, slot Mashed_pool0, released; `u9675_evidence/u9675_decomp_0041bc50.txt`)
agree pair for pair.

```
0x0041bc50 push esi
0x0041bc51 mov  esi,eax            ; record in EAX
0x0041bc53 mov  eax,[esi+0x1c]     ; pair 1 flag
0x0041bc56 test eax,eax            ; dword test
0x0041bc58 je   0x0041bc67
0x0041bc5a mov  eax,[esi+0xbc]     ; o = [rec+X+0xa0]
0x0041bc60 push eax
0x0041bc61 call [eax+0x48]         ; one dereference: a function-pointer field, not a vtable
0x0041bc64 add  esp,4              ; caller cleans
...                                ; 27 more pairs of the same shape
0x0041be95 mov  eax,[esi+0x38]     ; pair 29 flag
0x0041be9c mov  esi,[esi+0xd8]     ; pair 29 loads o into ESI
0x0041bea2 push esi
0x0041bea3 call [esi+0x48]
0x0041bea6 add  esp,4
0x0041bea9 pop  esi
0x0041beaa ret                     ; EAX = the 0x38 flag (0) if pair 29 skipped, else that call's EAX
```

603 bytes, 29 pairs. Flag X in byte order, object at X+0xa0 every time:

| # | X | X+0xa0 | flag load | call | # | X | X+0xa0 | flag load | call |
|---|---|---|---|---|---|---|---|---|---|
| 1 | 0x1c | 0xbc | 0x0041bc53 | 0x0041bc61 | 16 | 0x78 | 0x118 | 0x0041bd91 | 0x0041bd9f |
| 2 | 0x18 | 0xb8 | 0x0041bc67 | 0x0041bc75 | 17 | 0x7c | 0x11c | 0x0041bda5 | 0x0041bdb3 |
| 3 | 0x10 | 0xb0 | 0x0041bc7b | 0x0041bc89 | 18 | 0x3c | 0xdc | 0x0041bdb9 | 0x0041bdc7 |
| 4 | 0x14 | 0xb4 | 0x0041bc8f | 0x0041bc9d | 19 | 0x40 | 0xe0 | 0x0041bdcd | 0x0041bddb |
| 5 | 0x20 | 0xc0 | 0x0041bca3 | 0x0041bcb1 | 20 | 0x44 | 0xe4 | 0x0041bde1 | 0x0041bdef |
| 6 | 0x80 | 0x120 | 0x0041bcb7 | 0x0041bcc8 | 21 | 0x48 | 0xe8 | 0x0041bdf5 | 0x0041be03 |
| 7 | 0x84 | 0x124 | 0x0041bcce | 0x0041bcdf | 22 | 0x4c | 0xec | 0x0041be09 | 0x0041be17 |
| 8 | 0x88 | 0x128 | 0x0041bce5 | 0x0041bcf6 | 23 | 0x50 | 0xf0 | 0x0041be1d | 0x0041be2b |
| 9 | 0x8c | 0x12c | 0x0041bcfc | 0x0041bd0d | 24 | 0x24 | 0xc4 | 0x0041be31 | 0x0041be3f |
| 10 | 0x90 | 0x130 | 0x0041bd13 | 0x0041bd24 | 25 | 0x28 | 0xc8 | 0x0041be45 | 0x0041be53 |
| 11 | 0x94 | 0x134 | 0x0041bd2a | 0x0041bd3b | 26 | 0x2c | 0xcc | 0x0041be59 | 0x0041be67 |
| 12 | 0x68 | 0x108 | 0x0041bd41 | 0x0041bd4f | 27 | 0x30 | 0xd0 | 0x0041be6d | 0x0041be7b |
| 13 | 0x6c | 0x10c | 0x0041bd55 | 0x0041bd63 | 28 | 0x34 | 0xd4 | 0x0041be81 | 0x0041be8f |
| 14 | 0x70 | 0x110 | 0x0041bd69 | 0x0041bd77 | 29 | 0x38 | 0xd8 | 0x0041be95 | 0x0041bea3 |
| 15 | 0x74 | 0x114 | 0x0041bd7d | 0x0041bd8b | | | | | |

0x00..0x0c and 0x54..0x64 are never read. The U-9675 row's three defects are confirmed against the old port: (a) vtable
deref `mov eax,[edi]; call [eax+0x48]`, (b) ascending order from 0x10 instead of 0x1c first, (c) 28 pairs with 0x94
missing. A fourth defect: (d) it pushed the argument and never popped it (no `add esp,4`), so its `pop edi; pop esi;
ret` would unwind a misaligned stack after the first taken pair.

## Prediction (written before the build)

For every vector the log is the objects `[rec+X+0xa0]`, X in the byte order above, whose flag dword is non-zero, and
EAX is 0 in every vector: the skip path returns the zero flag and the taken path returns the recorder's 0. The
witness (old body) should go RED on all 11: AV at 0x48 on the first taken pair ([o]=0 read as a vtable), and on the
no-flag vector EAX = the record pointer instead of 0.

Check: both arms share one scratch, so GREEN means the original logged exactly what the port's table produces. The
log carries absolute scratch addresses, so its hash cannot be computed offline except for vector 1 (no flag set,
empty log). FNV-1a over 0x104 zero bytes plus EAX 0 = `0xd4592565`, and the original's vector-1 fingerprint is
`0xd4592565` on every boot (path1, self-baseline, both witness boots). That also shows the original read none of the
nine seeded unread offsets.

## The port

After `97e1560a` it is in `mashedmod/src/mashed_re/HUD/HudDispatch.cpp`. At `8e3c6b97` it was in `HUD/HudBatch.cpp`.
Naked export `FUN_0041bc50`: `push eax; call HudRec29DispatchBody; add esp,4; ret`. The cdecl body walks a 29-entry
`s_bc50FlagOffsets` table (each entry commented with its flag-load and call RVA): `r = [rec+X]`, and if non-zero,
`r = (*(HudMemberFn*)(o+0x48))(o)` with `o = [rec+X+0xa0]`. It returns r, which is the last pair's EAX, as in the
original.

Compiled code (capstone of the built .asi): body `push esi; push edi; mov edi,[esp+0xc]`, loop `movzx ecx,byte
[table+esi]; mov eax,[ecx+edi]; test eax,eax; je; mov eax,[ecx+edi+0xa0]; push eax; mov eax,[eax+0x48]; call eax;
add esp,4`, `pop edi; pop esi; ret`. ESI/EDI are preserved, the stack is balanced, and EAX = the last pair's value.
ECX/EDX at return are not part of the contract: the only caller, 0x0041c0c0, reads neither after 0x0041c0c8
(0x0041c0cd add esi,0x16c / 0x0041c0d3 cmp / 0x0041c0db pop esi; ret). No float path. EAX is the only register arg.

Install `RH_ScopedInstall(FUN_0041bc50, 0x0041bc50)` enabled. The old one was MASS-DISABLED on 2026-05-24.

## Fixture (`re/frida/negctl_fixtures/z_fix_u9675.py`, new key `hud_rec29_dispatch`)

`scalars_to_scattered_globals`, `freeze_main`, `fold_ret`, ret uint32, `eax_arg: 1` with args `[decoy, record]`. Both
arms go through `mov eax,[esp+8]; jmp`, so EAX = the record and the stack slot [esp+4] = a **decoy record** (all 29 flags
on, disjoint recording objects). A port that reads its record from the stack instead of EAX records a different
log instead of faulting.

* `recorder` (1 arg, mscdecl, returns 0) seeded at [o+0x48] of fake objects at scratch+0x600+k*0x50. [o+0] stays 0,
  so a vtable-reading port faults.
* The 9 unread offsets (0x00..0x0c, 0x54..0x64) are set non-zero in every vector, with recording objects at X+0xa0.
  A port that reads any of them records an extra entry.
* Vectors: all 29 on; none on; alternating 0x15555555 / 0x0aaaaaaa; pair 29 only (the ESI-loaded pair); pair 11
  (0x94, the one the old port lacked) + pair 1; flags 0x80000000 (sign bit only); flags 0x100 (low byte 0: a byte
  test would skip); every pair aliased to one object; two vectors on the live 0x0041c0c0 records 0x0063cab8 /
  0x0063cc24 (seeded and restored). 11 vectors, 10 distinct fingerprints (vectors 0 and 7 coincide by design: both
  have every flag non-zero, and 7 exists to catch a byte-test port).

`hooks_registry.py` still carries a "0x0041bc50 REFUSED" comment and no entry. Not edited here (parallel children never
edit the registry). The fixture adds the key. The orchestrator may move the entry into the registry.

## Evidence (`re/analysis/negctl/session_20261012b/u9675_evidence/`)

run_diff uses the worktree's `mashedmod/build/mashed_re_dev.asi` (`MASHED_NO_DEPLOY=1`, nothing deployed). The exe is
`original\MASHED.exe` through the worktree junction. Log: `.asi loaded ... FUN_0041bc50@...`.

| leg | result | file |
|---|---|---|
| run_diff path1 | GREEN 0/11 | `u9675_hud_rec29_dispatch.csv` |
| --self-baseline | GREEN 0/11 | `u9675_hud_rec29_dispatch_selfbaseline.csv` |
| negctl real / baseline | GREEN / GREEN | `u9675_negctl.tsv` |
| negctl null | RED 10/11 | same |
| negctl ret | RED 11/11 | same |
| negctl field_first / field_last | NA (flips=0) | same |
| negctl field_none | CTRL-GREEN | same |
| witness: old body restored alone | RED 11/11 on boot 1 and boot 2. Reimpl `access violation accessing 0x48` on the 10 vectors with a flag set, original arm clean. Vector 1: EAX mismatch (original `0xd4592565`) | `u9675_witness_oldport_boot{1,2}.csv` |
| callers / callees (Ghidra boundary) | caller `FUN_0041c0c0` only, no direct callees | `u9675_decomp_0041bc50.txt` |

null misses one vector by construction: vector 1 sets no flag, so the original makes no call and returns 0, which
is exactly what a do-nothing port returns. Every vector with a flag set is RED. field_first / field_last read NA
because the body makes no stores of its own (the only writes are the recorder's), the same as U-9671 and the four
rows in `fix_hud_dispatch_RESULT.md`. null and ret are the discriminating mutants, and the witness is the third.

Witness procedure: the old naked body (vtable deref, ascending order, 28 pairs, no pops) was put back alone in place
of the shim, under the same export, and built ("(source newer)"). run_diff ran twice (two boots, RED 11/11 each).
Then the fix was restored from a copy and `touch`ed, rebuilt with "(source newer)", and run_diff + self-baseline were
re-run, so the committed CSVs above are GREEN (`u9675_path1_stdout.txt`, `u9675_selfbaseline_stdout.txt`).

Callers / callees by Ghidra function boundary: caller `FUN_0041c0c0` HudDispatchSlot2 **C3** (hooks.csv row
0x0041c0c0). Callees: none direct (29 indirect `call [o+0x48]`). Nothing blocks C3 under the caller/callee gate.

## Second commit (`97e1560a`): HudDispatchSlot2 calls the port

`HUD/HudBatch.cpp` is in `asi_sources.rsp` only. `HUD/HudDispatch.cpp` is in both rsp files. So the port moves to
HudDispatch.cpp (after FUN_0041c2d0, reusing its `HudMemberFn` typedef), and HudBatch.cpp keeps a pointer comment. In
HudDispatchSlot2, `call FUN_0041bc50` with EAX = record replaces `mov ecx,0x0041bc50; call ecx`. Compiled code
confirmed: `mov eax,[ebp-4]; call 0x10013410` = the port export. Both targets link. On the rebuilt .asi:

| key | run_diff | self-baseline | distinct fingerprints | file |
|---|---|---|---|---|
| hud_rec29_dispatch | GREEN 0/11 | GREEN 0/11 | 10/11 | `u9675_c2_hud_rec29_dispatch*.csv` |
| hud_dispatch_slot2 | GREEN 0/9 | GREEN 0/9 | 8/9 (the all-on and fval=0x100 vectors coincide by design) | `u9675_c2_hud_dispatch_slot2*.csv` |

In run_diff the original arm runs the original bytes of 0x0041c0c0 -> 0x0041bc50 (hooks bypassed) and the reimpl arm
runs HudDispatchSlot2 -> the port. So the slot2 GREEN is a second, independent cross-check of the port.
`z_fix_hud_dispatch.py`'s docstring still says 0x0041bc50 runs "for real (original bytes, C2, not installed)". That
stays true inside run_diff, but it is stale once 0x0041bc50 is installed. The orchestrator may reword it.

## Proposed tracker changes (for the orchestrator, after path2)

* `hooks.csv` 0x0041bc50 FUN_0041bc50: C2 -> **C3** if path2 (`hud_rec29_dispatch`) PASSES. Status `mapped` ->
  `impl`. exe_file -> `mashedmod/src/mashed_re/HUD/HudDispatch.cpp` if `97e1560a` is kept, else
  `mashedmod/src/mashed_re/HUD/HudBatch.cpp`. frida_diff ->
  `re/analysis/negctl/session_20261012b/u9675_evidence/u9675_hud_rec29_dispatch.csv`. Note: replace "29-slot HUD
  render dispatcher EAX-thiscall; guard at EAX+X render-obj at EAX+X+0xA0 vtable[0x48]" with "EAX-in record; 29
  pairs in byte order 0x1c,0x18,0x10,0x14,0x20,0x80..0x94,0x68..0x7c,0x3c..0x50,0x24..0x38: if [rec+X] != 0, push o;
  call [o+0x48] with o = [rec+X+0xa0] (function-pointer field, one deref), caller-clean add esp,4; ESI preserved; EAX
  = last pair's EAX. 2026-10-12 re-ported from bytes (U-9675): run_diff GREEN 11/11, self-baseline GREEN, negctl
  null RED 10/11 (no-flag vector identical by construction), ret RED 11/11, field_none CTRL-GREEN, witness old port
  RED 11/11 x2 boots; caller 0x0041c0c0 C3, no direct callees.
  re/analysis/negctl/session_20261012b/u9675_RESULT.md".
* `UNCERTAINTIES.md` U-9675: **RESOLVED 2026-10-12** (re-ported from the bytes, `8e3c6b97`; plate
  `re/analysis/hud_ingame_d2/0x0041bc50.md` corrected; path2 by the orchestrator).
* hooks.csv 0x0041c0c0 note: if `97e1560a` is kept, append "calls the FUN_0041bc50 port (U-9675); run_diff GREEN 9/9
  + self-baseline on the rewired build (u9675_c2_hud_dispatch_slot2.csv)".

No [UNCERTAIN] items for 0x0041bc50.
