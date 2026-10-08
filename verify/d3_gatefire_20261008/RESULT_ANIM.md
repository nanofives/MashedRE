# RESULT — the phase-3 animation is a CAMERA-PATH KEYFRAME FLY-IN

Date 2026-10-08. Follows `RESULT_EXITGATES.md` §5, chosen by **USER DECISION (Mariano,
2026-10-08)**. **Static, read-only** — one batched `decomp_pc.py` run against a read-only pool
clone. No build, nothing run. `original/` untouched. No C-level.

Raw: `anim_decomp.txt`.

## 0. Verdict first

> **Phase 3 plays a camera-path keyframe animation** — a pre-race camera fly-in. It interpolates a
> camera orientation from a keyframe array each frame, pushes it into a RenderWare camera object,
> and phase 3 ends when the path runs out.
>
> This is **not inferred from call shape**: Ghidra's own C2 header on `FUN_00404fa0` reads
> *"Camera-path keyframe interpolator. Outputs a 4×4 rotation matrix (12 floats) at `param_1` based
> on time…"*, and the body carries the debug strings **`"tween is less than 0.0f!!"`** and
> **`"tween is greater than 1.0f!!"`**.

## 1. The three bodies

**`FUN_00404fa0`** (1,063 B, C2) — the interpolator:

- walks a keyframe array at `*(int*)(param_2 + 0x10)`, **stride `0x24`** (36 B/key), to find the
  pair bracketing the cursor `param_3`;
- computes the tween `(t − key.t0) / (key.t1 − key.t0)`, and **range-checks it with the two debug
  strings above**;
- SLERPs a quaternion via `FUN_00546e70` plus a degree-6 polynomial in `_DAT_005cc8f4..908`
  (a sine/slerp approximation);
- interpolates the positional part through `FUN_00482ae0`;
- writes a **rotation matrix** into `param_1[0..10]`, zeroes `param_1[0xc..0xe]` (the translation
  row), and calls `FUN_004c51a0(param_1, …, 2)`;
- out-of-range cursors fall back to `FUN_00532a60` with the first or last key.

**`FUN_004c1c80`** (64 B, C1) — *"Viewport dims setter: stores at +0x68 (dim X), +0x6c (dim Y),
+0x70 (ratio X), +0x74 (ratio Y)"*. `FUN_00405460` passes
`{_DAT_005cc950 / DAT_00803344, 0.75}` — an aspect pair.

**`FUN_004c1480`** (146 B, C1) — applies the matrix (`FUN_004c52f0(param_1 + 0x10, …)`) and then
links the object at `*(int*)(param_1 + 0xa0)` into a list head at **`DAT_007d3ff8 + 0xbc`**, setting
flag bits `| 3` and `| 0xc`.

> **`DAT_007d3ff8` is `RwGlobals*`** (memory `rwglobals-is-dat-007d3ff8`). So this is a RenderWare
> object being given a transform and flagged dirty on a global list. Described mechanically on
> purpose — that memory also warns against naming `RwGlobals`-indexed slots beyond what arity
> proves.

## 2. So phase 3, completely

Each frame, while the camera path still has cursor left:

1. set the camera's viewport dims / aspect (`FUN_004c1c80`);
2. interpolate the camera orientation from the keyframe array at the cursor (`FUN_00404fa0`);
3. push it into the RW camera and mark it dirty (`FUN_004c1480`);
4. **re-initialise every alive car's record** (`FUN_0046baa0`, ~70 fields — `RESULT_EXITGATES.md`);
5. advance the cursor; decrement `DAT_005f29b8`.

When the cursor passes the clip length **and** `DAT_00897fe0 == 0`, state goes **3 → 4**, and the
capture's single substate-4 sample is that frame.

**So the 683 calls of `c4 = 0` are a camera fly-in over held cars.** That is what the port is
missing before its race starts.

## 3. What this settles for the port

The port has **no camera path, no keyframe array, and no RW camera driven from one** during its
race start — it is in sub-state 6 from frame 0 (`aib_game_sub_mode` returns a constant).

**It also closes the "is a timer stand-in faithful?" question from `RESULT_PHASE3.md` §3.** It is
not. The hold's *duration* is the length of a camera clip, and its *exit* is that clip ending.
A timer reproduces neither; it reproduces a number measured from one capture. Any stand-in must be
labelled a bridge, and `SCOPE_SUBSTATE.md`'s sizing (a) should be read as **duration-only
scaffolding for measurement**, never as a port of phase 3.

## 4. What is NOT claimed

- **Not which camera path** plays, nor where `DAT_00639d70` / `DAT_00639d78` are set up. Unread.
- Not what `DAT_00897fe0` is, nor who writes it — still unread, and it is half the exit condition.
- Not the keyframe record layout beyond the stride `0x24` and the offsets the interpolator touches
  (`+4` time, `+8` quaternion, `+0x18` position, `+0x2c`/`+0x3c` the next key's).
- Not that porting the fly-in is required for the AI lane. It explains the 683-call difference; it
  does not follow that the port needs a camera animation to fix branch 2.
- No C-level. Nothing executed.

## 5. Next

This thread is **identified and can stop here**. The `DEFERRED.md` row should record phase 3 as:
*camera-path keyframe fly-in over per-frame-re-initialised cars*, bodies `FUN_004102f0` (172 B),
`FUN_00405460` (223 B), `FUN_00404fa0` (1,063 B), `FUN_004c1c80` (64 B), `FUN_004c1480` (146 B),
`FUN_0046baa0` (569 B), `FUN_004430b0` (6 B); globals `DAT_005f29b8`, `DAT_00639d70`,
`DAT_00639d78`, `DAT_00897fe0` — none verified live standalone.

The open cheap read, if anyone resumes: **who sets `DAT_00639d70` / `DAT_00639d78`**, which is
where the camera clip comes from.
