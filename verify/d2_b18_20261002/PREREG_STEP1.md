# PRE-REGISTRATION — D2 attempt 16, STEP 1: confirm the `+0xb18` zero on the RUNNING original

Written and committed **before** the first run of this step. Nothing below may be amended
after a run. If a registered gate fails, the step STOPS and the failure is reported as-is.

Branch `race/first-frame-parity`, HEAD at writing `29bd7619`.
Binary anchor: `original/MASHED.exe.unpatched`, SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` (CLAUDE.md anchor).

---

## 1 The question this step answers

`U-9175` / `RESULT_STEP4.md` §3b: the drive-force accumulator's **Y component `+0xb18`** is
exactly `0.0` on **9336 of 9336** original frames across four captures, and non-zero on the
port (385 of 400 post-release frames; pre-fix arm `s1` median `0.009007`). The `.msd` is a
**render-tick snapshot**, so it cannot by itself tell:

- **H-struct**: the original's `+0xb18` is zero because its PRODUCER feeds a structural zero
  (the per-wheel drive-direction Y is `0.0`), so A6a accumulates `0 * drive = 0`; **or**
- **H-later**: A6a writes a non-zero `+0xb18` and a LATER step zeroes it before the snapshot.

This step distinguishes them **on the running original** before any code is touched.

## 2 The writers, by RVA (static; to be confirmed live)

Disassembly from `re/tools/disasm_va.py` against `MASHED.exe.unpatched`.

### 2a The accumulator `+0xb18` — written ONLY by A6a `FUN_00467650`

Per `RESULT_STEP4.md` §3b and `UNCERTAINTIES.md` U-9175, the only writers of `+0xb18` are
A6a's two drive arms and its hold-arm zero:

| site | RVA | effect |
|---|---|---|
| active-drive accumulate | `0x00467cc5` / `0x00467ccb` | `+0xb18 += axisY * drive` |
| boost accumulate | `0x00467d97` / `0x00467d9d` | `+0xb18 += axisY * ff` |
| hold-arm zero (`+0xbf8 == 2`) | `0x00467e12` | `+0xb18 = 0` |

`axisY` is the per-wheel forward-axis Y, read as `Rp(p,0x20)` (`Integrate2.cpp:263`).

### 2b The axis-Y input — written by A5 `FUN_0046ddb0`, phase 0

A5 runs immediately before A6a (`A4_Body`: `Call_A5; Call_A6a; Call_A6b`). The per-wheel
forward axis Y lives at wheel-block `+0xb8` (record `+0x224`/`+0x2e8`/`+0x3ac`/`+0x470` for
the four wheels). From the disasm of `0x0046ddb0`:

```
0x0046ddbd  lea  ebp,[edi+0x9d4]          ; forward-vector dest
0x0046ddc3  push 0x614708                 ; DAT_00614708 = (0,0,1)
0x0046ddc9  call 0x4c3df0                 ; forward (+0x9d4/d8/dc) = xform*(0,0,1)
...
0x0046de27  fcomp dword [0x5d757c]        ; steer angle (esi+0x3c) == 0.0 ?
0x0046de35  jnp  0x46de65                 ; steer==0 -> straight copy
  ; steer==0 branch:
  0x0046de6e  mov  ecx,[edi+0x9d8]        ; body forward Y
  0x0046de74  mov  [esi+0xb8],ecx         ; wheel axis Y = forward Y   <== axisY writer
  ; steer!=0 branch (0x46de37..0x46de63): RwMatrixFromAxisAngle(up,angle) @0x4c4d20
  ; then transform fwd by that matrix @0x4c3df0 into [esi+0xb4]
```

So the chain is `xform` -> body forward `+0x9d8` (written `0x0046ddc9`) -> wheel axis Y
`+0xb8`/record `+0x224..` (written `0x0046de74`, steer==0; or the rotation transform
`0x0046de5b`, steer!=0) -> `+0xb18` (A6a). On a flat arm the forward-Y is `0.0` iff the
car body matrix (`xform`) keeps its Z row's Y component at `0.0`.

### 2c The per-frame reset — A4 `FUN_00470670`

A4 zeroes `+0xb14/18/1c` every frame at `0x0047072c` (`PhysicsChainHooks.cpp:292`), BEFORE
A5/A6a. So at A6a entry `+0xb18` is expected `0.0`; at A6b entry it is A6a's own output.

## 3 The probe (committed in this same commit, unrun)

`re/frida/scenario_launch.py --axis-probe`. Two ENTRY hooks, the sanctioned bracket
(memory `frida-interceptor-is-entry-only`), reading the STATIC player record
`0x008815a0 + car*0xd04`:

- **site 0** = A6a entry `0x00467650`, ESI-filtered to the player record = **PRE**.
- **site 1** = A6b entry `0x00468980` = **POST** (A6a's immediate successor; A6b does not
  write `+0xb14/18/1c`, so POST == A6a's own output).

Each sample records: body forward `+0x9d4/d8/dc`, `+0xb14/18/1c`, speed `+0x9e4`, grounded
`+0x9e0`, boost-state `+0xbf8`, trackId `+0x1f0`, the four wheel axis-Ys
`+0x224/2e8/3ac/470`. Rate ~120/s, well under the ~1000/s destabilise floor.

**Known-answer self-check** (reported in `axisProbeStats.chk`): the static forward-axis
constant at `0x00614708` must read `(0.0, 0.0, 1.0)`, proving the probe reads the image at
the right VA. **Coverage counters** (reported before the rows): `a6a` (PRE frames), `a6b`
(POST frames), `skipped` (other cars), `drive` (PRE frames with `bf8==0 && speed>1.0` = the
active-drive regime), `fwdYnz`, `axYnz`, `b18nzPre`, `b18nzPost`.

## 4 The arms

- **ORIGINAL**: the §16.7 reference arm, identical to attempt 15's `orig_bp1`:
  `py -3.12 re/frida/scenario_launch.py --statediff-out verify/d2_b18_20261002/orig_ax.msd
  --statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38 --poke-ctrl-slots
  --axis-probe`. Launched muted (the harness sets `MASHED_MUTE`). `MASHED_TITLE=d2-a16-orig-ax`.
- **PORT**: `py -3.12 re/tools/statediff/a8_run_port.py verify/d2_b18_20261002/port_ax 90
  MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0
  MASHED_A6ADUMP=<abs path>/port_a6adump.log MASHED_TITLE=d2-a16-port-ax`. The port's own
  A6a dump already records `act.bf` (body forward) and `b14/18/1c`; it is the port side.
- Track the spawned PIDs; kill ONLY those. `MASHED_WIN_POS=primary-bl`.

## 5 Gates and the decision rule — pre-committed

| gate | required |
|---|---|
| **SC** self-check | `chk.fwdConst == [0.0, 0.0, 1.0]`; `chk.rec == 0x008815a0` |
| **COV** coverage | `a6a >= 400` AND `drive >= 50` (the active-drive regime is actually sampled); `err == null` |
| **PRE** | original `+0xb18` at site 0 is `0.0` on every PRE frame (`b18nzPre == 0`) — the A4 reset is confirmed |

**VERDICT, decided before the run:**

- **H-struct is CONFIRMED** iff, on the ORIGINAL, over the `drive` (active-drive) PRE frames:
  `fwdYnz == 0` AND `axYnz == 0` (body forward-Y and all four wheel axis-Ys are exactly
  `0.0`) **AND** `b18nzPost == 0` over the matched POST frames (A6a leaves `+0xb18 == 0`).
  Then the original's `+0xb18` zero is **structural**: the producer feeds `0.0`, A6a
  accumulates `0`, and no later step is involved. The writer of `+0xb18` is A6a
  `0x00467cc5/..`; the writer of the axis-Y is A5 `0x0046de74` (from forward-Y `+0x9d8`,
  written `0x0046ddc9`).

- **H-later is indicated** iff `b18nzPost > 0` on the original while the `.msd` snapshot of
  `+0xb18` is `0.0` — A6a produces a non-zero value that a later step clears.

- The PORT contrast is reported numerically from `port_a6adump.log`: `act.bf[1]` (forward-Y)
  and the post-A6a `b18`. H-struct predicts the port's forward-Y is non-zero where the
  original's is `0.0`.

**STEP 2 is entered only if H-struct is CONFIRMED and names a single confirmed cause** (the
port's forward-Y / axis-Y producer diverging from a structural zero). If the cause is a
magnitude spread with no single producer, STEP 2 is NOT entered and the finding is reported
as the first diverging term, per the kickoff's STEP 3 fallback.

No fix is authored in STEP 1. No value is changed. No knob is added.
