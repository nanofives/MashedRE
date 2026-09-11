# Next session — kickoff prompt

## ⇒ CURRENT STATE (2026-09-11, merge-drain + uncertainty-loop session) — READ THIS FIRST

Branch `race/first-frame-parity`, tree clean. **Zero worktrees, zero Ghidra pool locks,
zero stray processes.** Trackers: hooks.csv 5,930 rows (C4 184, C3 1,011, C2 3,883,
C1 821 — unchanged, no function moved C-level this session) · DEFERRED 678 active.

**Uncertainties, with the command that derives each number:**

| measure | count | command |
|---|---:|---|
| open rows in the Active section | 3,029 | rows matching `^\| *U-[0-9]+` between `## Active uncertainties` and `## Resolved`, minus `~~`-prefixed |
| of those, **actually gating** | **150** | same set, `Blocks` cell (index 7) exactly `C2->C3` or `C3` |
| gating at session start | 235 | — |
| gating at session end | 150 (was 235) | — |
| rows with a non-canonical column count | 131 | pre-existing baseline, unchanged by this session's 33 edits |

## What landed

### 1. Merge + cleanup — the "merge worktrees" item was already done
**All 11 worktree branches were already ancestors of HEAD**, so there was nothing to
merge; the work was safe removal. Two things had to happen first:

- **A cited artifact lived only inside a worktree.**
  `verify/camera_racecam/race1/01_inrace_track.png` is cited twice by
  `re/analysis/race_camera/render_camera_child_c_20260830.md`, which is already merged,
  but the file was untracked (`verify/**/*.png` and `*.bmp` are both gitignored) and
  existed nowhere else. Rescued into the main checkout and force-added, along with
  `cam_ctrlpose/`, `camera_racecam/`, `run_26588/` and the three pose/position text dumps.
- **`.worktrees/d1-invert` was a partially-removed worktree corpse.** Git had already
  deregistered it (no `.git` file) but **its `original/` junction to the real game
  install was still live** — the exact configuration behind the two WORKTREE-SYMLINK-WIPE
  incidents. Deleted the junction non-recursively first (`Directory.Delete(path, false)`
  removes a reparse point without touching the target), verified `original/` intact, then
  removed 334 MB after confirming **all 10,938 files were duplicated in main** (checked at
  file level, not by directory name). Also removed two stray zero-byte files.

Ghidra pool: 10 stale lock files cleared, 5 of them nested project locks dated 26 June.

**`area/frontend` is still unmerged, deliberately** — it is a WIP checkpoint whose
`PanelSortInit` hook (`0x00420d00`) would go live in the dev ASI unverified.

### 2. Ghidra headless was unreachable from PowerShell — fixed
`bash` on this machine resolves to **WSL** (`C:\WINDOWS\system32\bash.exe`), not Git Bash.
WSL cannot open a Windows path as an argv entry: the backslash form arrives with the
backslashes eaten, and the forward-slash form it rejects outright. Both `decomp_pc.py` and
`ghidra_pool.ps1` shelled out to `bash scripts/ghidra_pool.sh`, so **every pool acquire
failed** and headless decompilation only ever worked from a Git Bash session. Both now
resolve Git Bash explicitly. Memory: `bash-on-path-is-wsl-not-git-bash`.

Pre-existing and left alone: `ghidra_pool.sh status` returns **exit 1 on success**.

### 3. Uncertainty loop — 235 → 150 gating, in six passes

**Pass 1 — 22 false gates.** The file's own D0.3 rule ("target is C3/C4 in hooks.csv with
the row still open ⇒ it demonstrably did not gate") ran **once**, on 2026-08-15, and was
never re-run, so every function promoted since left its rows still claiming `C2->C3`.
Re-applied mechanically; each cell records the value it replaced. **No uncertainty is
resolved by this** and every repaired row says so. Memory:
`blocks-column-goes-stale-rerun-d03`.

**Passes 2–3 — 8 rows resolved, each against a literal citation.**

| row | outcome |
|---|---|
| U-4416 | `RpClumpForAllAtomics(int,code*,undefined4)` @ `0x004e66d0`: circular-list walk, `(*param_2)(puVar2 + -0x10, param_3)` per node, early-out when the callback returns 0 |
| U-4417 | the `0x40/4/4/4` arg is a **per-element size** — `FUN_00474db0` computes `param_1 * param_2` |
| U-4508 | **port hazard**: the "skipped" pool slot pointer is passed in **EAX** |
| U-5107 | MSVC signed `% 2`, i.e. count truncated to even; traced on 7, 8, −3 |
| U-5161 | `tex+0x14` and `tex+0x50` are **one field** (`int*` index 0x14 = byte 0x50) |
| U-5645 | `ctx+0x138` holds a **function pointer** (`puVar1[0x4e] = FUN_00554940`) |
| U-5427 | **hypothesis refuted** — Ghidra mis-recovered the frame, `unaff_retaddr` is plain stack arg1 |
| U-5584 | **port hazard**: `0x0041d410` takes its object pointer in **EAX**, no stack args |

**One unread constant closed three rows.** U-4420, U-4750 and U-4800 all wanted
`DAT_005d757c` and none had read it: it is `0.0f`. Built `re/tools/memread.py` for this —
it prints section, file offset, raw bytes, dword and float from the anchored
`MASHED.exe.unpatched`, and it **refuses to invent a value**: asked for `0x00773208` it
reports the address is past `.data`'s raw size and that a static read proves nothing,
which is why U-4717 was left alone rather than "answered" with a zero.

**Pass 4 — the 41 parked claims adjudicated: 37 resolved, 4 refused.** Details under
"A. DONE" below. Notable refutations, which are worth more than the confirmations:
`U-4429`'s `param_1` is a pure OUT parameter so the row's premise is false; `U-5158` is
none of forAll/deInit/readData; `U-5614` has no reference count in 35 bytes; `U-5621`'s
`0x301a1` vs `0x401a1` are masked with `0xff0000` and handed to one allocator as flags,
not separate arenas. `U-4602` settled a three-way outright: a **sphere against six
planes** with a tri-state return, not a point test and not an AABB.

### ⚠ The two hazards are a matched pair — read both before trusting either
`U-4508` and `U-5584` are real register arguments. `U-5427` **looked identical and was
not**: `sub esp,8` plus four pushes moved ESP to entry−0x18, so `[esp+0x1c]` was ordinary
stack arg1. **`in_`/`unaff_` is a suspicion, not a verdict — compute the frame offset
before concluding a register argument, or you will invent one.**

## ⇒ THE STRUCTURAL FINDING: the gating bucket is XREF-shaped, not decomp-shaped

Full triage of all 213 rows that were gating at session start, one line each with
evidence, in `re/analysis/plans/gating_triage_20260911.md`:

| bucket | count |
|---|---:|
| NEEDS-XREF | 128 |
| PARTIAL | 47 |
| NEEDS-EXTERNAL | 29 |
| ANSWERED | 5 |
| NEEDS-RUNTIME | 2 |
| STALE | 2 |

The dominant blocker is **not** "nobody decompiled this" — it is "nobody chased the
**writers** of a global or the **callers** of a function". `decomp_pc.py --xrefs
--callers` already produces exactly that, in batch, and one headless run covers ~160
addresses in a few minutes.

## PICK ONE

### A. DONE — the 41 parked claims are adjudicated (37 resolved, 4 refused)
See CHANGELOG 2026-09-11 "pass 4". My reason for parking them was **wrong and I measured
it**: chunk 3's 27/48 hit rate came from holding 46% identity-shaped rows against 8-29%
elsewhere, and those are exactly what a callee body answers. Composition, not leniency.

Two review gates, both worth reusing. **Mechanical:** check that every `FUN_`/`DAT_`/hex
token a claim quotes actually occurs in the decomp corpus — 41/41 passed, so nothing was
fabricated. **Judgment:** does the evidence answer the question *asked* — 4 failed there,
which is why the first gate is necessary and not sufficient.

**The one that inverted is now a memory-safety item.** U-5162's claim "proved" safety by
quoting a 0x80 clamp that **is not in the function the row is about**. `0x004cf5a0` hands
two 128-byte stack buffers to `FUN_004d8810` as destinations, and the amount written is
`local_9c` straight from the stream chunk header, never compared against 128. Write-up and
port guidance (do **not** silently add a bounds check — it is a behavioural divergence):
`re/analysis/rw_native_raster_name_buffer_20260911.md`.

### B. DONE — the data-xref lane is built and drained
`decomp_pc.py --datarefs` now answers "who writes this global": references split into
writes / reads / other, each with the containing function and the referencing
instruction, plus a per-address initialised check. Validated against two globals whose
answers were already known independently.

Drained: **7 resolved, 15 narrowed.** Ten globals turned out to have **zero writes
anywhere**, so their file value is their runtime value — that alone closed four rows
that had each asked "who writes this". `DAT_007d3ff8` is a pointer written twice with
the literal `0x7d3ec8`, which identifies the object behind every `+N` dispatch in the
tree and moves six vtable rows to NEEDS-EXTERNAL.

### B2. DONE — the 47 PARTIAL rows: 7 resolved, 2 narrowed, lane mined out
Needed only 14 new bodies, then 9 more for the rows that named a precise next callee.
`re/analysis/plans/partial_readjudication_20260911.md`. **32 rows have no further body
to fetch** — they need a runtime read or an RW/D3D9 reference, not another round.

### B3. Two port notes came out of it, worth acting on
- **U-4501:** `local_8`/`local_4` are NOT dead. `FUN_004c5010` reads three consecutive
  floats through the pointer, so a transcription that drops them loses two of three
  components.
- **U-4581:** the discarded random call is **not** side-effect free — `FUN_00534870`
  bumps a shared cursor. Optimising the call away desynchronises every later draw.
`decomp_pc.py --xrefs` takes a function entry, not a data address, so this needs either a
DecompPC.java addition for data xrefs or a `reference_to` equivalent. That one tool change
unlocks the largest remaining sub-bucket.

### C. NEEDS-EXTERNAL — 29 rows want a RenderWare/D3D9 table
Pixel-format IDs (`0x51`, `0x3d`), RW enum names, D3DFORMAT values. Needs
`re/prior_art/` or librw, not Ghidra. **Note: external web is unusable on the worker
account — it fabricated results when asked. Run any web lookup on account3.**

### D. Writing a resolution? Do not put a pipe in the cell.
My own U-5107 resolution quoted a bitwise OR and the literal `|` — escaped as `\|` or not —
split the row and pushed `Blocks` from index 7 to index 9. Rebuilt with the operator
spelled `OR` and a note saying so. The `resolve*.py` scripts in the scratchpad now refuse
a cell containing a pipe.

## Carried over, still needing YOUR call
- **`main` is 270+ commits behind** — `race/first-frame-parity` has never been merged to
  `main`. Branch decision, not taken unilaterally.
- **D-11069** — 4 duplicate-RVA rows need `hooks.csv` to express "different implementation
  per build target", which the single `file` column cannot. That is a **schema change**
  affecting every parser.
- **U-9087** — 4 C4 rows may need demotion; their install proof is a byte the original
  already has.
- **G / rubric L37** — 387 C3 rows are shadow-generatable but `re/CONFIDENCE.md` names a
  Frida CSV as the C4 evidence. Amend, or keep C4 Frida-only.

## Standing rules that bit earlier sessions
- Shadow lane: **single-boot verdicts are unreliable** (transient consecutive-failure
  windows, cause still unknown). Require two independent boots.
- Run races with `--cars 4 --hold 60`; a 1-car race never fires the contact solver.
- Never `git worktree remove --force` — use `py -3.12 scripts/diag.py wt-remove`.
- Kill only PIDs you spawned; never blanket-kill MASHED by name.

## Ready-to-paste kickoff

> Resume the Mashed RE lane on `race/first-frame-parity`. Read `re/NEXT_SESSION.md`, then
> pick A–D (B and C are tooling/reference lanes; A is the biggest pile of nearly-done
> work but every row needs verifying against the quoted decomp before it is landed —
> 1 in 5 worker verdicts did not survive review last time). Ghidra headless now works
> from PowerShell: `py -3.12 re\tools\decomp_pc.py --file rvas.txt --callees --xrefs
> --json -o out.json` batches ~160 addresses in one run. `re/tools/memread.py` reads a
> constant out of the anchored binary and refuses to guess at BSS addresses.



