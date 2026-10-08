# RESULT — the clip comes from a camera-path module at `0x004053d0..0x00405540`. All three globals are image-ZERO.

Date 2026-10-08. Follows `RESULT_ANIM.md` §5, chosen by **USER DECISION (Mariano, 2026-10-08)**.
**Static, read-only** — one `decomp_pc.py --datarefs --no-decomp` run against a read-only pool
clone. No build, nothing run. `original/` untouched. No C-level.

Raw: `clipsrc_refs.txt`.

> **CORRECTED 2026-10-08 by [`RESULT_IMGCHECK.md`](RESULT_IMGCHECK.md).** The conclusion below
> stands, but the reason is sharper. All three globals sit in `.data`'s **uninitialised tail**
> (`.data` is VSize `0x32a704` vs RawSize `0x4d000`), so they have **no file bytes** and are zero
> at load **on the original too**. The original's fly-in exists because
> `FUN_004053d0`/`FUN_00405400` **write them at runtime**. The port therefore cannot transcribe
> its way to a hold — it needs those writers.

## 0. Verdict first

> **The camera clip is owned by a tight module at `0x004053d0..0x00405540`.** Both handles are
> written by the same two functions and read by the same three.
>
> **And all three globals are `.data` with image value `0`** — so in the standalone `FUN_00405460`'s
> guard fails immediately and `DAT_00897fe0` is already `0`. **If the port ran phase 3 as written,
> it would exit on the first frame**: a zero-length fly-in, no hold.

## 1. The reference sets, complete

| global | `.data` image value | writers | readers |
|---|---|---|---|
| `DAT_00639d70` | **`0`** | `FUN_004053d0` ×2, `FUN_00405400` | `FUN_00405430`, `FUN_00405460` ×2 |
| `DAT_00639d78` | **`0`** | `FUN_004053d0` ×2, `FUN_00405400` | `FUN_00405430`, `FUN_00405460` ×2, `FUN_00405540` |
| `DAT_00897fe0` | **`0`** | **`FUN_004430a0` only** (`0x004430a4`) | `FUN_004430b0` only |

`DAT_00897fe0` additionally has **4 address-taken references** — `PUSH 0x897fe0` in `FUN_00448220`
(×3) and `FUN_00448700` (×1) — so it is also passed by pointer somewhere.

## 2. Two loops this closes

**(a) Phase 3 sets its own exit flag.** `FUN_004102f0`'s one-shot init calls **`FUN_004430a0(1)`**
(`RESULT_PHASE3.md` §1), and `FUN_004430a0` is the **only** writer of `DAT_00897fe0`. So phase 3
raises the flag at countdown start and waits for it to return to `0` — meaning something else calls
`FUN_004430a0(0)`. The exit condition is a handshake, not a passive poll.

**(b) The same init touches the flag's address.** `FUN_004102f0` also calls **`FUN_00448700(0,0)`**,
and `FUN_00448700` is one of the functions that **pushes `&DAT_00897fe0`**. So the init both sets
the flag and hands its address to `FUN_00448700`.

## 3. The module, by its own reference cluster

`0x004053d0`, `0x00405400`, `0x00405430`, `0x00405460`, `0x00405540` — five adjacent functions
sharing exactly these two handles, and nothing outside that range touches them. Their roles, from
the reference directions alone (not from reading the bodies):

| | writes | reads | inferred role |
|---|---|---|---|
| `FUN_004053d0` | both handles, twice each | — | **setup / teardown** |
| `FUN_00405400` | both handles, once each | — | **setup** |
| `FUN_00405430` | — | both | query |
| `FUN_00405460` | — | both (2× each) | **advance** (`RESULT_ANIM.md`) |
| `FUN_00405540` | — | `d78` | query |

**[UNCERTAIN]** — those role labels are read off write-vs-read direction, not off the bodies. Only
`FUN_00405460` has been decompiled.

## 4. Why this matters more than the thread's length suggests

`RESULT_ANIM.md` §3 said a timer stand-in reproduces the fly-in's duration but not its mechanism.
This adds a sharper point: **the mechanism is not merely absent in the port, it is
self-cancelling.** With `DAT_00639d70 == 0` the playback guard fails, so `FUN_00405460` returns
`0` ("finished"), and with `DAT_00897fe0 == 0` the second gate is already satisfied. A faithful
port of `FUN_004102f0` alone would therefore produce **no hold at all** — it would transition 3→4
immediately and look like the port's current behaviour.

So porting phase 3 means porting the **camera-path module** that fills those handles, not just the
state function. That is a materially larger scope than `SCOPE_SUBSTATE.md` §3 costed, and it should
be recorded against sizing (b) and (c) before anyone takes the row.

## 5. What is NOT claimed

- **Not what `FUN_004053d0` / `FUN_00405400` actually do**, nor where they get the clip data.
  Unread — their role labels in §3 are direction-derived and marked `[UNCERTAIN]`.
- Not who calls `FUN_004430a0(0)` to clear the exit flag. That is the handshake's other half and
  is still unread.
- Not what `FUN_00448220` / `FUN_00448700` do with `&DAT_00897fe0`.
- **Not that `WRITES` is exhaustive.** `--datarefs` reports what Ghidra's analysis found; a missed
  cross-reference is indistinguishable from an absent one. The image values (`0`) are from the same
  source, not independently read out of `MASHED.exe.unpatched` the way `0x005f2770`'s was
  (`AiStandalone.cpp:1388-1390`). **If any of this becomes load-bearing, confirm it against the
  file image.**
- No C-level. Nothing executed.

## 6. Next

This thread is complete enough to stop. The `DEFERRED.md` row should carry §4's conclusion —
**phase 3 requires the `0x004053d0..0x00405540` camera-path module, not just the state
function** — and the two unread handshake ends (`FUN_004430a0(0)`'s caller, and
`FUN_004053d0`/`FUN_00405400`'s clip source) as the resume points.
