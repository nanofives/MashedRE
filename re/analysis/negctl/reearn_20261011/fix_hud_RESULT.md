# Re-earn cluster hud: RESULT (2026-10-12, branch reearn/hud)

Six rows, all FIXED. Fix commit `ea7a93af`. Every port was re-read from
`original/MASHED.exe.unpatched` with `re/tools/disasm_fn.py`, not just the defect named in the decision row.
Builds used `MASHED_NO_DEPLOY=1`. No path2, no tracker edits, no re-classify.

Shared evidence:
- negctl run: `runs/fix_hud/negctl_20261012_045126.tsv`, copied to `evidence/fix_negctl_hud.tsv`
  (handler `scalars_to_scattered_globals`, classes null, ret, field_first, field_last, field_none, slots 1).
- Fixtures: `re/frida/negctl_fixtures/z_fix_hud.py` overrides all six keys (loads last).
- Harness: `re/frida/diff_template.js` `stub_at` support for `scalars_to_scattered_globals`. ea7a93af first
  carried a hand copy of the single `diff_template.js` hunk of reearn/replay `e58dfc6c`. After the parent's
  note, race/first-frame-parity (with reearn/replay, 606a8bbd) was merged into this branch (6571b4a6). The
  merge was clean, the block appears once, and `diff_template.js` is byte-identical to 606a8bbd, so the
  support now comes from the merged commit. Rebuilt on the merged tree (`MASHED_NO_DEPLOY=1`), run_diff is
  GREEN again on all six (5/5, 13/13, 6/6, 7/7, 7/7, 10/10). No merged fixture overrides the six hud keys.
- Witness: one build with each function's principal old defect put back alone (the six functions are
  independent), run_diff RED on all six. Then the sources were restored, rebuilt, and run_diff re-run GREEN.
  The `fix_diff_*.csv` files come from that re-run, not from the witness build.
- Neighbours in the touched files (`HUD/HudBatch.cpp`, `Frontend/BatchAA_s1.cpp`, `Util/UtilMid.cpp`), re-run
  on the fixed build: `font_text_string_table_lookup` 10/10, `mode_gated_player_check` 9/9,
  `pending_op_queue_flush` 5/5, `get_dat0066d6e4` 10/10, `hud_rect_emitter5` 5/5 GREEN.
  `font_ctx_set_rotation` reads INCONCLUSIVE-DEGENERATE: its registry vector returns the constant 1, and its
  body is unchanged in this branch.
- Caller confidences come from a scan of every E8/E9 call site in `.text`. The containing function is the
  nearest preceding `hooks.csv` row.

---

## 0x00403160 Sub00403160_SubMode0BViewport (`sub_00403160_submode0b_viewport`)

**Verdict: FIXED**

Defects:
1. One dereference too many. The bytes are `mov eax,[0x007d3ff8]` @0x004031b6 then `call [eax+0x20]`
   @0x004031bf, so the callee is the dword at `*0x007d3ff8 + 0x20`. The port read `*(*(*0x007d3ff8)+0x20)`.
   The global is re-read before each of the four calls (@0x004031c2, @0x004031ff, @0x0040320c), and the port
   now does the same.
2. Return value. EAX at return is the second `FUN_004c1a00` result (@0x0040322d). The epilogue
   (@0x00403232 cookie xor, `jmp 0x004a2be9`) does not touch EAX. The port returned void and now returns int.

The rest was re-read and matches: viewport {0.8f, 0.8f} @0x00403173, the +0x68/+0x6c save, call order, and
`FUN_00428760(flag, 50, 30, 240, 120, 0)` @0x004031e0..0x004031f7.

Harness: b01's camera fixture faulted on BOTH arms at 0x0 inside `FUN_00427e00`. That is the text draw reached
from `FUN_00402fb0` @0x0040301b/0x0040303b, and its font globals are not live at the menu. The new fixture:
- points `DAT_007d3ff8` at a scratch block whose +0x20 is the recorder (2 args, cdecl). None of
  `FUN_004671a0`, `FUN_004c19f0`, `FUN_004c1a00` or `FUN_004c1c80` reads `DAT_007d3ff8`.
- stubs `0x00427e00` and `0x00428760` with the same recorder.
- adds a flag vector and a begin-fails vector (camera +0x18 = 0x0040dca5, `xor eax,eax; ret`).
- folds the return (`ret` int).

Evidence:
- `evidence/fix_diff_sub_00403160_submode0b_viewport.csv`: GREEN 5/5.
- `evidence/fix_diff_sub_00403160_submode0b_viewport_selfbaseline.csv`: GREEN 5/5.
- negctl: real GREEN 0/5, baseline GREEN 0/5, null RED 5/5, ret RED 5/5, field_first RED 5/5,
  field_last RED 5/5, field_none CTRL-GREEN 0/5.
- Witness (`evidence/fix_witness_doublederef_sub_00403160_submode0b_viewport.csv`): the double dereference
  alone reads RED 4/5. The one GREEN is the begin-fails vector, which never reaches the render-state calls.

C3 gate:
- Callees: 0x004671a0 C2, 0x004c19f0 C3, 0x004c1c80 C3, 0x004c1a00 C3, 0x00402fb0 C2, 0x00428760 C3.
- Caller: 0x0040dfc0 C3 (call site 0x0040e0c4).

Plate corrected: `re/analysis/hud_ingame_promote_c2/0x00403160.md`. Its "vtable slot" wording is the
claim the port followed.

[UNCERTAIN] none.

---

## 0x0041db80 Sub0041db80_HudThresholdDispatch (`sub_0041db80_hud_threshold_dispatch`)

**Verdict: FIXED**

The thunk is `mov eax,0x0063d55c` + `jmp 0x0041dab0`. The body is at 0x0041dab0.

Defects:
1. Inverted second block. `jnp 0x0041db78` @0x0041dae1 returns when c < [0x005cd238] (0.34), so the block
   runs when !(c < 0.34). The port ran it only when c < 0.34.
2. Counter cached once. Every compare reloads `[esi+0x2c]` (`fld` @0x0041dab3/d3/e7, @0x0041db03/26/4e/5e),
   and the dispatched functions can write it. The port now re-reads it at each compare. Object slots are
   re-read at each call.
3. The plate's decomp (not the old port) called obj+0x04 and obj+0x08 for c >= 1.86. The bytes call none of
   them there (@0x0041db34 `jp 0x0041db4e`).

Compare semantics: `test ah,5; jp` takes the jump unless C0 alone is set. That gives `!(c < thr)`, including
NaN. The tail @0x0041db59 is `test ah,1; jne`, which exits on C0, so it exits for NaN too. The port uses
`c < thr` and `!(c >= thr)`, which agree in both cases.

Evidence:
- run_diff GREEN 13/13 (`evidence/fix_diff_sub_0041db80_hud_threshold_dispatch.csv`). b01's 8 vectors plus
  0.82, 1.3, 1.86 exact, -1.0 and NaN.
- Self-baseline GREEN 13/13.
- negctl: real GREEN 0/13, baseline GREEN 0/13, null RED 10/13, field_first RED 10/13, field_last RED 10/13,
  field_none CTRL-GREEN, ret SKIP (void). The 3 GREEN vectors under null are ones where the bytes dispatch
  nothing.
- Witness (`evidence/fix_witness_invertedblock2_sub_0041db80_hud_threshold_dispatch.csv`): the inverted
  0.34 test alone reads RED 10/13.

C3 gate:
- No direct callees (dispatch through the function pointer at obj+0x48 only).
- Caller: 0x0040dfc0 C3 (0x0040e0ee).

Plate corrected: `re/analysis/hud_ingame_promote_c2/0x0041db80.md`. It now has a mechanical description from
the bytes, with the old decomp kept as superseded.

[UNCERTAIN] none.

---

## 0x0041ded0 HudModeGuardDispatch (`hud_mode_guard_dispatch`)

**Verdict: FIXED**

Defect: the ESI register argument. `mov esi,0x0063d5a0` @0x0041dedf comes before `call 0x0041de80`
@0x0041dee4, and `FUN_0041de80` reads its record through ESI (0x0041de80..0x0041dec0). The port called it as
plain cdecl and never set ESI. The port now issues `push esi; push param_1; mov esi,0x0063d5a0;
call 0x0041de80; add esp,4; pop esi` in inline asm. The gate (dword `[0x0063d5e8]` @0x0041ded0) matches.

The file `Frontend/BatchAA_s1.cpp` is also in `exe_sources.rsp`. The old port already called 0x0041de80 by
raw address, and nothing in the exe calls `HudModeGuardDispatch`.

Evidence:
- run_diff GREEN 6/6 on b07's fixture (gate closed, gate open p=0, sel=3, p=1 idx 0/5/9).
- Self-baseline GREEN 6/6.
- negctl: real GREEN 0/6, baseline GREEN 0/6, null RED 5/6, field_first RED 5/6, field_last RED 5/6,
  field_none CTRL-GREEN, ret SKIP (void).
- Witness (`evidence/fix_witness_noesi_hud_mode_guard_dispatch.csv`): the plain cdecl call alone reads
  RED 5/6. The one GREEN is the gate-closed vector.

C3 gate:
- Callee: 0x0041de80 C2.
- Caller: 0x0040dfc0 C3 (call sites 0x0040e02f, 0x0040e03a, 0x0040e0a7, 0x0040e0b3).

Plate corrected: `re/analysis/hud_ingame_promote_c2/0x0041ded0.md` (ESI section added).

[UNCERTAIN] none.

---

## 0x004295a0 HudDualLabelRender (`hud_dual_label_render`)

**Verdict: FIXED**

Defects:
1. `FUN_0040dc80` takes no arguments. Its body is `fld [0x0063b910]; ret` (0x0040dc80). The port passed it
   six args and then four.
2. `FUN_00427e00` takes six arguments, twice:
   - @0x004295ed: (0xff, y + [0x005cc31c] (3.0f), 63.0f, 0xff000000, 1.0f, 2)
   - @0x00429614: (0xff, y, 60.0f, 0xff0000ff, 1.0f, 2)

   The frame is built around the `FUN_0040dc80` call (0x004295a3..0x004295d4 and 0x004295f9..0x00429601).
   The port passed two args, so args 3-6 were stack garbage.
3. Return value. EAX at `ret` (0x0042961c) is the second `FUN_00427e00` EAX. The port now returns it.

Harness: `FUN_00427e00` faults at the menu (font globals 0x0067d838/0x0067d83c). The fixture stubs it with a
6-arg cdecl recorder, seeds `DAT_0063b910` (0, 100, -3, 0.1, 3e38, 1e-45, NaN) and folds the return.

Evidence:
- run_diff GREEN 7/7. Self-baseline GREEN 7/7.
- negctl: real GREEN 0/7, baseline GREEN 0/7, null RED 7/7, ret RED 7/7, field_none CTRL-GREEN.
  field_first and field_last read NA: the only observable is the recorder's scratch log, which those mutants
  do not flip.
- Witness (`evidence/fix_witness_twoargs_hud_dual_label_render.csv`): the 2-arg first call alone reads
  RED 7/7.

C3 gate:
- Callees: 0x0040dc80 C3, 0x00427e00 C2.
- Caller: 0x00429e10 C2 (call sites 0x0042a077, 0x0042a08e, 0x0042a16a).

Plate corrected: `re/analysis/util_c0_promote/0x004295a0.md`. The old [UNCERTAIN] on `FUN_0040dc80`'s
return is replaced by the cited body. The meaning of 0x0063b910 is not established (naming only, no U-row
needed).

---

## 0x00429aa0 GameStateSlotsFill (`game_state_slots_fill`)

**Verdict: FIXED**

Defects:
1. Path A never loads ST0. The bytes `fld [idx*0xc+0x00614718]` @0x00429ab1 and `fld [idx*0xc+0x0061471c]`
   @0x00429aca come before each `call 0x004a2c48`. The port called 0x004a2c48 as a no-arg cdecl, so it stored
   the conversion of a stale ST0. `FUN_004a2c48` pops ST0 on every path (0x004a2c57, 0x004a2c6b,
   0x004a2c71/0x004a2c8f, 0x004a2cb3/0x004a2cb7).
2. Both `[0x0067d9a0]` stores are x87 float copies (`fld`+`fstp` @0x00429ae3 and @0x00429b19). The port did
   dword moves, which differ on a signalling NaN.
3. Return value. EAX at both `ret`s is the third idx (0x00429af0, 0x00429b26).

The port uses inline-asm helpers `X87FtolLoad` / `X87FloatCopy` in `Util/UtilMid.cpp`. They issue the same
x87 sequence and call the original `0x004a2c48` under the live control word.

Evidence:
- run_diff GREEN 7/7: b02's 5 vectors plus -2.7, 3.0e9 (above 2^31) and a signalling-NaN third float on both
  paths, with fold_ret.
- Self-baseline GREEN 7/7.
- negctl: real GREEN 0/7, baseline GREEN 0/7, null RED 7/7, ret RED 7/7, field_first RED 7/7,
  field_last RED 7/7, field_none CTRL-GREEN.
- Witness (`evidence/fix_witness_unloadedst0_game_state_slots_fill.csv`): the unloaded ST0 alone, on the
  first path-A store, reads RED 4/7. Those are exactly the 4 path-A vectors.

C3 gate:
- Callees: 0x00430820 C3, 0x00430790 C3, 0x004a2c48 C2.
- Callers: 0x0043dfd0 C2 (0x0043e927, 0x0043f99a) and 0x0043d7c0 C2 (0x0043dda5).

Plate corrected: `re/analysis/timer_d2_cont1/0x00429aa0.md`. Its "calls FUN_004a2c48() (frame counter)" is
the claim the port followed.

[UNCERTAIN] none.

---

## 0x0043c000 TimerSlotTickDispatcher (`timer_slot_tick_dispatcher`)

**Verdict: FIXED**

The port was rewritten from 0x0043c000..0x0043c5aa. Defects:
1. Every counter store is a BYTE (`mov [..],al` / `mov [..],bl`). The port wrote dwords on 13 of 19 slots.
2. Unloaded ST0 again:
   - state 1 is `fld [0x005cc320]; fsub [0x0067e840]; fmul [0x005cd04c]; call 0x004a2c48` (@0x0043c137),
     which is (1.0 - x) * 255.0.
   - state 2 is `fld [0x0067e840]; fmul [0x005cd04c]` (@0x0043c15f), which is x * 255.0.

   The port called 0x004a2c48 with nothing loaded.
3. Reset mode:
   - `fcomp [0x005d757c]` (0.0f); `test ah,0x44; jp` (@0x0043c007..0x0043c01a) resets 19 STATE dwords
     (`if (s != 1) s = 0`), including 0x0067e800.
   - The 18-slot pass still runs afterwards (@0x0043c12c `jmp 0x0043c133`).
   - The port cleared counters instead of states and skipped the pass.
4. Slot 0x0067e7d8's else-branch stores byte 0 to 0x0067e7d8 itself (@0x0043c1c6), not to its counter.
5. Slot order and addresses come from 0x0043c133..0x0043c5a3. 0x0067e800 is reset but never ticked. The port
   walked a stride-8 table that ticked 0x0067e800.
6. Return value. EAX at ret is the last slot's conversion, or its state dword on the else path (@0x0043c55c).
   The port now returns it.

Evidence:
- run_diff GREEN 10/10: b03's 5 vectors plus state 1 on the 14 slots without an action, x = 0.3 / 0.5 / -0.0
  / NaN, and fold_ret.
- Self-baseline GREEN 10/10.
- negctl: real GREEN 0/10, baseline GREEN 0/10, null RED 10/10, ret RED 10/10, field_first RED 10/10,
  field_last RED 10/10, field_none CTRL-GREEN.
- Witness (`evidence/fix_witness_dwordcounter_timer_slot_tick_dispatcher.csv`): a dword counter store on
  state 1 alone reads RED 5/10. Those are exactly the 5 vectors with a state-1 slot.

C3 gate:
- Callees: 0x004a2c48 C2, 0x004332a0 C2, 0x0042f7b0 C3, 0x0042c960 C2, 0x0042fa00 C2.
- Caller: 0x0043dfd0 C2 (0x0043e05d).

Plate corrected: the 0x0043c000 section of `re/analysis/game_mode_cont1/notes.md`. Its slot table,
"frame counter" and reset description are the claims the port followed.

Coverage gap: the four action slots (0x0067e7a8, 0x0067e7c8, 0x0067e7d8, 0x0067e7e0) are never driven to
state 1, because their actions (0x004332a0, 0x0042c960, 0x0042f7b0, 0x0042fa00) read live input. So the
"store, then call the action" order on those four (e.g. @0x0043c14e then @0x0043c153) is cited from the
bytes but not exercised by a vector. It is not a [UNCERTAIN] about the bytes. If the parent wants it closed,
the remedy is a stub_at recorder on the four actions.

---

## Verdicts

| RVA | hook key | verdict |
|---|---|---|
| 0x00403160 | sub_00403160_submode0b_viewport | FIXED |
| 0x0041db80 | sub_0041db80_hud_threshold_dispatch | FIXED |
| 0x0041ded0 | hud_mode_guard_dispatch | FIXED |
| 0x004295a0 | hud_dual_label_render | FIXED |
| 0x00429aa0 | game_state_slots_fill | FIXED |
| 0x0043c000 | timer_slot_tick_dispatcher | FIXED |
