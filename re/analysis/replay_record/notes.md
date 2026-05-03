# Replay Record / Playback — First Pass
Session: replay_record-20260503 | Slot: Mashed_pool10 | Date: 2026-05-03

## Summary

Mashed implements a **position/rotation ghost replay** for Time Trial mode (game mode 2).
It does NOT record steering/accel/brake inputs; it records world position and quaternion
orientation per frame into a singly-linked list of 0x24-byte nodes.

On each lap finish the current lap's replay is compared against the stored best; if
faster it becomes the new best. The best lap is serialized to `c:\toast\Replay%d.rep`
(where %d = player ID) at race end.

## Architecture

```
TimeTrial::Update (0x004111c0, state 6)
  └─ TimeTrial::RecordPlayback (0x00411170)        [per-frame dispatcher]
       ├─ Replay::RecordFrame (0x00411600)          [per-frame WRITE — REPLAY_FN]
       │    ├─ FUN_0046d4a0(&vehicle_state, 0)     [S-1563 — get player-0 state]
       │    └─ Replay::WriteFrame (0x004829d0)      [append node to linked list]
       │         └─ FUN_00546b10(node, vehicle)     [S-1564 — copy quaternion]
       └─ Ghost::PlaybackTick (0x00411ae0)          [per-frame READ — ghost car]
            └─ Replay::ReadFrame (0x00482c10)       [interpolated read from list]
                 ├─ FUN_00546e70(a, b, &slerp)      [S-1572 — node interp]
                 └─ FUN_00482ae0(t, ...)            [S-1573 — quat blend]
```

## REPLAY_FN: Replay::RecordFrame (0x00411600)

Called every game tick while in state 6 of the time trial state machine.

```c
void RecordFrame(uint time_ticks)   // @ 0x00411600
{
    if (FUN_0042f6a0() != 2) return;           // mode check @ 0x00411610
    if (DAT_0063bb14 == DAT_0063bb10)          // sanity: cur != best @ 0x00411630
        wprintf("Time trial recording error...");

    FUN_0046d4a0(&vehicle_state, 0);           // get player-0 vehicle @ 0x00411640 (S-1563)
    WriteFrame(DAT_0063bb14, vehicle_state,    // append frame @ 0x00411648
               time_ticks, 10);

    DAT_0063bb20 = 0;                          // @ 0x00411660

    // checkpoint 0 detection
    if (FUN_00407a00(0,0) && *(DAT_0063bb14+0x17c)==0)
        *(DAT_0063bb14+0x17c) = time_ticks;   // record checkpoint-0 time @ 0x0041168c

    // checkpoint 1 detection
    if (FUN_00407a00(0,1) && *(DAT_0063bb14+0x180)==0)
        *(DAT_0063bb14+0x180) = time_ticks;   // record checkpoint-1 time @ 0x004116c0
}
```

**Not** an input recorder — position+rotation ghost only.

## Key Globals

| Address | Role |
|---------|------|
| `DAT_008a94a8` | Global binary I/O buffer (0x24a3c = 150,588 bytes) for .rep file read/write |
| `DAT_0063bb04` | Replay object slot 0 ptr |
| `DAT_0063bb08` | Replay object slot 1 ptr |
| `DAT_0063bb14` | Current-lap replay ptr (write target) |
| `DAT_0063bb10` | Best-lap replay ptr (compare + ghost display) |
| `DAT_0063bb0c` | Ghost (loaded-from-disk) replay ptr |
| `DAT_0063bb18` | Slot toggle index (0/1 → indexes into DAT_0063bb04/08) |
| `DAT_0063bb1c` | Current playback time offset |
| `DAT_0063bb20` | Lap-finished flag (set by LapFinish; cleared by RecordFrame) |
| `DAT_0063bb2c` | Has-recorded-a-lap flag |
| `DAT_007f0ff4` | Main game tick timer |
| `DAT_007f1008` | Lap-start tick value (delta used in LapFinish) @ 0x00411893 |
| `DAT_005f29d0` | Track config table: alternating (player_id, buffer_size) pairs, terminated by -1 |
| `DAT_005f29c8` | Save-once guard flag (prevents double save) |
| `DAT_0063ba8c` | Race state machine state |

## Replay Object Struct (0x19c = 412 bytes, magic 0x10b = 267)

All fields observed from FUN_00482930, FUN_004829d0, FUN_00482c10, FUN_00483d10.

```
+0x00  uint32  magic = 0x10b
+0x04  uint32  computed buffer size
+0x08  uint32  player/vehicle ID
+0x0c  float   lap duration (seconds)
+0x10  uint32  time scale (= 10)
+0x14  void*   checkpoint-array ptr (FUN_004a2c48()*0x24 bytes)
+0x18  void*   frame list head (oldest frame node)
+0x1c  void*   frame write cursor (newest frame node)
...
+0x16c uint32  checkpoint count - 2 (= FUN_004a2c48() - 2)  [offset 0x5b*4]
+0x168 uint32  frame count
+0x170 uint32  frame write counter
+0x174 uint32  end-of-lap time (ticks)
+0x178 uint32  first-frame time
+0x17c uint32  checkpoint-0 time (ticks)  [UNCERTAIN if 2+ checkpoints use this array]
+0x180 uint32  checkpoint-1 time (ticks)
+0x194 uint32  has-started flag (0 until first frame appended)
```

[UNCERTAIN U-1568] Fields +0x20..+0x167 not directly observed in this session.

## Frame Node Struct (0x24 = 36 bytes)

Observed from FUN_004829d0 (write) and FUN_00482c10 (read):

```
+0x00  float[4]  quaternion xyzw  (written by FUN_00546b10 from vehicle struct)
+0x10  float     pos.x  (from vehicle_state + 0x30)
+0x14  float     pos.y  (from vehicle_state + 0x34)
+0x18  float     pos.z  (from vehicle_state + 0x38)
+0x1c  void*     next node ptr (advances via FUN_004829d0; last node wraps to head)
+0x20  uint32    time (ticks)
```

Node list is singly-linked and circular at end (last node's +0x1c → head node).
Pre-seeded: each WriteFrame copies pos+quat into the newly-allocated next node before
advancing, ensuring the next slot always holds the last recorded pose.

## File Format

- Path: `c:\toast\Replay<player_id>.rep`  (`DAT_007d3ff8+0xc4` sprintf @ 0x00411e46)
- Also `c:\toast\All267.rep` (player ID = 0x10b) — aggregate slot

Binary layout (from FUN_00483d10 and FUN_00483ca0):
1. 0x19c bytes — replay object header (verbatim struct dump)
2. `(framecount+2) * 0x24` bytes — frame node array (contiguous, linked list rebuilt on load)

## Call Chain / State Machine

`FUN_004111c0` (@ 0x004111c0) is the Time Trial state machine (`DAT_0063ba8c` dispatch):

| State | Function | Action |
|-------|----------|--------|
| 1 | init | race setup, init vehicles |
| 2 | main loop | physics, vehicle update |
| 6 | recording | `TimeTrial::RecordPlayback` ← REPLAY_FN called here |
| 9 | post-race | `FUN_0040ddb0` |
| 10 | save | `Replay::Save` → `c:\toast\Replay%d.rep` |

`FUN_00411870` (`Replay::LapFinish`) is called via `FUN_0040e560` from `FUN_00429310`.
It records the final frame with `WriteFrame(param_4=0xffffffff)`, compares
`*(DAT_0063bb10+0x174)` vs `*(DAT_0063bb14+0x174)` (@ 0x004119c0), and if the new
time is better promotes: `DAT_0063bb10 = DAT_0063bb14`, toggles `DAT_0063bb18`,
picks next free slot for `DAT_0063bb14`.

## Greenfield port notes

- The replay subsystem is entirely Time Trial — it can be omitted from initial greenfield
  build until Time Trial mode is implemented.
- Storage system uses `c:\toast\` hardcoded path; will need virtualisation for port.
- Ghost-car rendering driven by `Ghost::PlaybackTick` → `FUN_0041a9b0` (S-1569) and
  `FUN_0041ad00` (S-1570) — unknown vehicle transform API; defer until vehicle rendering
  is established.
- Replay file I/O uses a custom stream API (FUN_004cc230/004cbd30/004cc160, S-1566..1568);
  not standard fopen/fread.
