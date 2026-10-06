# PRE-REGISTRATION — KA-ACCEPT v2: does the ORIGINAL load the standalone's save?

**Committed UNRUN. 2026-10-05.** Replaces `PREREG_SAVE.md`'s KA-ACCEPT, which was **VOID** (its
`0x00803358` was a transient serialization buffer read at an arbitrary moment).
Anchor verified: `MASHED.exe.unpatched` `BDCAE093…3C0E`, `launch.exe` `0150…8DA2`.

## 1. The corrected instrument — no indirection needed

`FUN_00404e80` `Save::DeserializeFromBuffer` copies the save buffer **back into the live
championship table at `DAT_007F0A40`**, a **fixed image address**. `gamesave_edit.py:16-18` records
the same linkage, measured 2026-08-30: *"the shipped save's span col1/col11 signature … is IDENTICAL
to the live launch gate DAT_007f0a40 … editing span col1 propagates to the gate on load."*

So the question "did the original load our file" is answered by peeking four fixed addresses. No
`*0x008a94a8` chase, no heap pointer.

Span cell address = `0x007F0A40 + row*0x30 + col*4`, matching the file's span at `0x24A40` with the
same geometry (`gamesave_edit.py:10`).

| peek | row/col | ORIGINAL file | STANDALONE file |
|---|---|---|---|
| `007f0a44` | r0 c1 challenge-cup gate | **1** | **0** |
| `007f0a50` | r0 c4 track known | 2 | 2 |
| `007f0ab0` | r2 c4 track known | **2** | **0** |
| `007f0a6c` | r0 c11 quick-battle gate | **1** | **0** |

Three of four discriminate. `007f0a50` is deliberately a **non-discriminating control** — it must
read 2 on both arms; if it differs, something other than the save is driving the table.

Expected values are taken from the committed files via `gamesave_spandiff.py`, whose decode passed
KA-LAYOUT on the reference file.

## 2. Arms

Two **separate install copies** under a scratch path, launched with `MASHED_ROOT`. **Nothing under
the repo's `original/` is written** — it is read only to make the copies, and its SHA-256
(`BD18788182B2343E…`) is re-verified afterwards.

- **arm C** — the shipped `gamesave.bin`.
- **arm S** — the standalone's `mashed_re_gamesave.bin`, copied in as `gamesave.bin`.

`scenario_launch.py --peek` only. **No Interceptor, no hook, no write.**

## 3. Gates

**KA-LOAD (the control, and it can void the leg).** On **arm C**, the four live values must equal the
ORIGINAL FILE's span values — `1, 2, 2, 1` — on **>= 4 of 4** samples. If they do not, the save is
not being loaded in this harness, or the address is wrong, and the leg is **VOID**: no statement is
made about arm S. An assert that fails on the control is a false RED.

**G-ACCEPT.** On **arm S**, scored only if KA-LOAD passes:

- **ACCEPTED** — the three discriminating peeks read the STANDALONE file's values (`0, 0, 0`) **and**
  the control reads `2`. The original parsed our file and populated its live table from it.
- **REJECTED/IGNORED** — the three read the ORIGINAL's values (`1, 2, 1`), i.e. the table was not
  repopulated from our file.
- **NEITHER** — any other combination, e.g. all zeros including the control, which would mean the
  table was cleared rather than loaded. Reported as **INCONCLUSIVE** with the raw values.

**Registered limitation, so a PASS is not over-read:** ACCEPTED means the original **parses and
loads** our save into its live progression table. It does **not** show the game plays correctly on
it, and it says nothing about the 148,540-byte profile block (all-zero in both files, so this leg
cannot discriminate there at all).

**Registered prediction:** ACCEPTED. The file has the right size, the right magic, and a
structurally valid span whose `col4` values are all in the documented `{0,2}` domain. If it comes
back REJECTED, something gates the load that none of this session's offline work saw.

## 4. Not in scope

Behaviour beyond the load; the profile block; the 0x40 never-written residual; any C-level move.
