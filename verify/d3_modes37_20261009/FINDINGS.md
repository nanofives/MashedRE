# FINDINGS: what modes 3/7 need before a port can be non-inert (2026-10-09)

Measurement only. No game code changed. Probe: `scenario_launch.py --wobj-probe` (new, two entry
hooks: `0x00414c30` + registry dump, `0x00414a70` inputs). Captures `o_wobj1`, `o_wobj2`.

## 0. Verdict first

> - **Mode 3 does not come from `FUN_00414c30` in the scored window. It comes from `FUN_00414a70`.**
>   In every regime-A original window (6 captures, `o_breg_r1/3/4/5/7/8`, identical), `FUN_00414c30`
>   returns 1 on **0** calls. It returns 2 (mode 7) on 49 calls of car 1 only.
> - **Car 2, the car failing (b), has no mode-7 calls.** Its carriers are `FUN_00414a70 == 1`
>   (mode 3, 35 calls) and `== 2` (brake exit at `0x00416405`, 10 calls). So `FUN_00414a70` is the
>   port to do first, and mode 7 second.
> - **A faithful `FUN_00414a70` would be inert today.** Three of its inputs have no writer in
>   `mashed_re.exe`: per-car path progress `0x008a96e8+v*0x30c` (`FUN_00408a50`), the path node count
>   `DAT_0066d6d8` (`FUN_00426bb0`, live value 30), and record `+0xbc` (`FUN_0046d6d0`).
> - **Mode 7's substrate mostly exists.** The world-object entries it acts on are the active pickups
>   (type 7, radius 0.5, callback `0x00458920`, registered by `FUN_00459000`), and the port owns the
>   pickup pool (`Gameplay/PickupPoolSpawn.cpp`). It also needs refdist (default-ON bridge), the
>   held-powerup word (port has it), `DAT_0063ba8c` not 10/11.

## 1. Original window counts per car (regime A, identical on 6 runs)

| car | `14c30==2` mode 7 | `14a70==1` mode 3 | `14a70==2` brake | branch 2 |
|---|---|---|---|---|
| 1 | 49 | 16 | 0 | 31 |
| 2 | 0 | 35 | 10 | 4 |
| 3 | 0 | 12 | 26 | 29 |

Regime C (`o_breg_r2`): car 1 mode 7 49; car 2 18 / 6; car 3 12 / 27; branch 2 0 (flag 2).

## 2. The registry (`0x006dccb8`, count `0x006e70d8`, stride 0x8c)

Per frame: 4 car entries typed `0xff00..0xff03` (skipped by `(int)type >= 0xff00`), up to 5 type-7
pickups (`d7 = 0x00458920`), and type-0x0A entries (radius 0.223/0.463; some `d7 = 0x0047cd50`). No
type 0x0D or 0x15 on Training. Type 0x0A never produces a return in the window, so omitting it is
measured-harmless on this scenario only.

## 3. `FUN_00414a70` inputs, original live values (`o_wobj2.msd.wobj_ins.csv`)

| input | address | original | port |
|---|---|---|---|
| path progress | `0x008a96e8+v*0x30c` (`FUN_00408a50`) | node position on a 30-node loop, wraps at the line (grid 30.00/29.73/29.45/29.18), non-monotone | **no writer** (writer `FUN_00408610`, D-11072) |
| node count | `DAT_0066d6d8` (`FUN_00426bb0`) | 30 | **no writer** (no direct xref, set through a computed address) |
| speed | rec `+0xbc` = `0x00881f84+v*0xd04` (`FUN_0046d6d0`) | 0..3595 | **no writer**. Original: `0x004710c3 call 0x004c3ac0(rec+0x9a0)` / `0x004710c8 fstp [esi+0x9d4]`, esi = `0x008815b0+k*0xd04`, i.e. `|velocity|` |
| slot state | `*(0x005f2770)+0x34+v*4` (`FUN_0040e370/e470`) | 1 / 2 / 2 / 2 | seeded (`TrackRenderer.cpp:414`) |
| `DAT_0063ba8c` | `FUN_0040e350` | 3 / 6 / 7 | knob-dependent |
| held powerup | `DAT_0088fc88+v*0xb4` | 0 on 56% | host accessor exists |

## 4. Decision needed (architecture)

Path progress is the blocker for `FUN_00414a70`. Two routes:
- **Bridge** (precedent: `MASHED_RACEPCT_BRIDGE`): write `0x008a96e8` from the port's arc progress
  scaled to the node count, seed `DAT_0066d6d8`, write `+0xbc` faithfully. Not a C-level change.
  Fidelity checkable against the original's car-1 (position, progress) pairs in `o_wobj2`.
- **Faithful**: port `FUN_00408610` (1066 B) + `FUN_00407e20` + the `FUN_00538c80` world query (D-11072).

## 5. Raw

`decomp_chain.txt`, `decomp_leaves.txt`, `decomp_producers.txt`, `o_wobj{1,2}.msd.{aistep,wobj_calls,wobj_ents}.csv`,
`o_wobj2.msd.wobj_ins.csv`, logs.
