# PORT-OT-RWG-SLOTS: HANDOFF (nothing built yet)

Branch `orch/PORT-OT-RWG-SLOTS` from `orch/PORT-OT-SEED-2` `a33c7f238`. The previous worker
(cmw9srv5g098tle1c2ba4475l) handed off at the row boundary: its context got heavy after GAMEPLAY, FRONTEND,
ORIGTOP-SIZE, the lint class, OT-MEASURE and OT-SEED-2. No code changed on this branch yet.

## Brief (lead cmw7xu1960f37p41557tefpnd)

1. Read the original's values first, with an original-side probe under mashed_lock:
   - field +0x18 of the RW device table (RwGlobals)
   - call slots +0x20 and +0x30
2. Fill those slots in the exe's fake RW device table: the slots bound to librw or to matched bodies.
3. Register module offset 0x007d3ffc and the other three module-offset globals.
4. Prefer the ORIG_TOP path only. If the change touches the default path, the gates must prove it unchanged:
   - G-CTRL `8284be46/67931834/31fe043e`
   - B `1d5bf4f9` x3
   - 12 nav captures, OFF/ON/OFF/ON
5. Report which of the 42 `needs binding` rows are released.

## What is measured already (verify/portorigtop_20261109/RESULT_PORT-OT-MEASURE.md, OT_MEASURE.tsv)

**Under MASHED_ORIG_TOP=1:**
- `*(0x007d3ff8)` is 0 for ~1.5 to 6 s.
- After that it points at the exe's fake RW device on the heap: `0x00ff8290`, or `0x00ff9290` after SEED-2.
- It is installed by the B15 bridge:
  - `D3d9Render/RwIm2DBridge.cpp:281-299` (`kRwDeviceSlot` = 0x007d3ff8, written at :299)
  - `exe_main.cpp:598`, `:9806`

**Slots the table fills:** +0xcc, +0xd4, +0x108, +0x10c, +0x114.

**What the 42 rows need, with row counts** (offsets from the original binary via capstone; rows listed per slot in
`OT_MEASURE.tsv` `rwg_detail`):

| piece | rows | named in the matched sources as |
|---|---|---|
| call +0x20 | 19 | `setState`, `fpRenderStateSet` (e.g. 0x00401f10, 0x00472c60) |
| data +0x18 | 14 | `zBufferNear`, `nearZ` (0x00472b10..0x004736c0) |
| call +0x30 | 13 | draw call in 0x00472c60 / 0x00473220 |
| module 0x007d3ffc | 7 | the RW math LUT module (FastSqrt 0x004c3b30, Vec2Length 0x004c3bf0, ...) |
| call +0x11c | 5 | |
| call +0xe8 | 4 | |
| modules 0x007d4028, 0x007d4628 | 2 each | |
| call +0x34, +0xf4, +0x118 | 2 each | |
| one row each | | +0x0, +0x24, +0x60, +0xbc, +0xc4, +0xd0, +0xdc, +0xf8, +0xfc, module 0x007d4054 |

- 17 rows are blocked only by +0x18 / +0x20 / +0x30.
- All four module-offset globals read 0 in the exe (no RW module registered).

## Suggested steps

1. **Original-side probe.** Run the original `MASHED.exe` under mashed_lock, from a private root copy of `original\`.
   Read `*(0x007d3ff8)` with ReadProcessMemory once the menu is up, then:
   - every slot above
   - the four module-offset globals 0x007d3ffc / 0x007d4028 / 0x007d4054 / 0x007d4628
   - the module data at RwGlobals + offset

   `ot_measure.py probe` already does exactly these reads for the exe. Point a copy at MASHED.exe: launch it the way
   `re/frida/*` do (patched install, MASHED_RE_NO_AUTO_HOOK=1), kill only the own PID. In the original, each call slot
   holds a MASHED.exe code address: map it to its RVA / hooks.csv name. That is the function the exe must bind
   (librw equivalent, matched body, or exe port).
2. **Decide the scope.** Read RwIm2DBridge.cpp to see whether the fake device is installed on the default path too
   (it looks so: B15 / B17).
   - If it is, fill the new slots only when `MASHED_ORIG_TOP` is set (exe_main.cpp:10488-10492 reads it once),
     unless the gates prove a default-path change neutral.
   - Module offsets: write the original's offset values to 0x007d3ffc etc. and supply module data at
     RwGlobals + offset. The fake table must be large enough, or RwGlobals moved to a block that is.
3. **Re-run the measurement on the new build:**
   ```
   ot_measure.py probe --data <dir with ot_rows.json from D:/MashedWT/PORT-OT-MEASURE_data> --exe <new exe> --root ...
   ot_measure.py verdict --data <dir>
   ```
   Count the RwGlobals rows that turn OK.
4. **Gate** with `re/tools/port_batch_bisect.py --exe-off D:/MashedWT/PORT-OT-SEED-2_data/mashed_re.exe --exe-on <new exe>`
   (build mode), inside test-slot + mashed_lock.

## Kickoff for the next session

```
You are an L2 port worker for Mashed (lead cmw7xu1960f37p41557tefpnd). Read
.worktrees/orch-integration/re/orchestrator/WORKER_RULES.md (binding), then this handoff:
verify/portorigtop_20261109/RESULT_PORT-OT-RWG-SLOTS.md on branch orch/PORT-OT-RWG-SLOTS (worktree
.worktrees/PORT-BATCH-GAMEPLAY, or a new worktree from that branch). Claim PORT-OT-RWG-SLOTS with your session id
(-Parent cmw7xu1960f37p41557tefpnd, -Force if it is in handoff). Do the brief: original-side probe of RwGlobals
+0x18 / +0x20 / +0x30 and the module offsets, fill them in the exe's fake RW device (ORIG_TOP path preferred),
register the module offsets, re-run ot_measure.py probe + verdict, gate OFF/ON/OFF/ON if the default path is
touched, write RESULT_PORT-OT-RWG-SLOTS.md with the released rows, set review, REPORT.
```
