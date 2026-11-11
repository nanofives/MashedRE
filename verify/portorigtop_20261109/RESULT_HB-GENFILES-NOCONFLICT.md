# RESULT HB-GENFILES-NOCONFLICT (2026-11-10)

Verdict: **GREEN**. The three generated port-state files now merge by record through a git merge driver, so two
ORIG_TOP rows in flight no longer conflict:

- `verify/porttop_20261108/BINDINGS.tsv`
- `verify/portstate_20261108/SEED_CANDIDATES.tsv`
- `mashedmod/src/mashed_re/Compat/PortStateSeed.inc`

A real disagreement still conflicts loudly.

## Decision: option (b), a record-level merge driver

`scripts/genfiles_merge.py`, registered in `.gitattributes` the same way as the `hooks.csv` `merge=hooksport`
precedent. Why (b) over (a), per-row fragment files:

1. `PortStateSeed.inc` is a committed build input, and the LAN release builds from a clean `git archive` with no
   generator step. With fragments, the assembled `.inc` would still be a single committed file both rows touch, so
   (a) would still need a driver or a new build-time generator.
2. (a) would have to migrate 16 chained `update_bindings_*.py` scripts and every reader of `BINDINGS.tsv`. (b)
   leaves the file format and every reader as they are.
3. (b) gives the merge equal to serial regeneration by construction. Each file is keyed (BINDINGS by `rva`,
   SEED_CANDIDATES and the `.inc` by address), and the `.inc` is re-rendered through
   `re/tools/gen_portstate_seed.py`'s own `render()`.

## What the driver does

| kind | key | rule |
|---|---|---|
| `genbindings` | `rva` (lowercase) | Merged field by field. If only one side changed a field, that side wins. If both sides added a suffix to the note, they combine as base + ours suffix + theirs suffix. Any other change to the same field on both sides is a conflict. New rows go after their nearest preceding row. Theirs' trailing new rows go after ours' rows. |
| `genseedtsv` | `int(addr)` | Rows are merged by address and output sorted by address. A header mismatch halts (exit 2). The same address with different row content is a conflict. |
| `genseedinc` | entry address | The `.inc` entries (bytes, name, section, readers) are parsed, merged by address and re-rendered with `gen.render()`. Overlapping ranges halt. The same address with different bytes is a conflict. |

A conflict writes `<<<<<<< ours` / `>>>>>>> theirs` markers and exits 1, so git stops the merge. A halt exits 2.

`gen_portstate_seed.py` changed in two ways so the `.inc` depends only on its entries:

- The header no longer embeds the inventory's sha256. That was the one line both sides always changed.
- `--check` now compares the name, section and readers of each entry against the inventory instead.
- `--check` on the regenerated file is OK: 263 entries, 3192 B, fnv `0xce5a7775`.

The only `.inc` change on this branch is that header line, so the compiled bytes are unchanged.

The driver command falls back to `git merge-file`, today's behaviour, when the checked-out tree has no
`scripts/genfiles_merge.py`. A branch that predates this row therefore merges exactly as before.

## Self-test: `scripts/test_genfiles_merge.py` (13 passed, no MASHED launch)

The real pair is base `5fc6a43e4` (integration), ours ATOMIC-VIEW-2 `0fd2195bb`, theirs HUDINIT-A `d4ee8a938`.

- Plain `git merge-file` on BINDINGS conflicts (the test asserts this).
- **BINDINGS** merges clean. The result is byte-equal to an independent row-wise serial oracle: base, then ours'
  changes, then theirs', with new rows appended.
- **SEED_CANDIDATES** merges clean and equals the address-sorted union of both sides.
- **PortStateSeed.inc** merges clean and is byte-equal to running `gen.resolve()` and `gen.render()` on the merged
  inventory against `original/MASHED.exe.unpatched`: 272 entries, fnv `0x5ecb7d52`, `--check` OK. This is
  "regenerate serially on the merged tree", done literally.
- **No-op.** theirs == base returns ours byte for byte for BINDINGS and SEED_CANDIDATES. For the `.inc` it is equal
  apart from the dropped inventory-sha header line of the pre-change base.
- **Disagreement.** In SEED_CANDIDATES and in the `.inc`, the same seed address with a different size or bytes gives
  conflicts=1 with markers. In BINDINGS, the same rva with a different `kind` conflicts. Separate rows at different
  addresses still merge clean.
- Note-suffix concatenation, new-row order (ours then theirs), the driver's exit and write path, and the no-chain
  helper's idempotence are each covered.

End-to-end `git merge --no-commit d4ee8a938` on a scratch checkout of `0fd2195bb`, with the attributes and the
driver config:

| | result | sha256[:16] |
|---|---|---|
| driver present | all three files merge clean, 0 markers | BINDINGS `aeb329f1aaf6b7f7`, SEED_CANDIDATES `318b29f39b540d95`, .inc `bec53abd30c2f86a` |
| driver file absent (fallback) | BINDINGS and the .inc conflict, as today | |

## Finding: the chained BINDINGS updaters are not a serial oracle

Each `update_bindings_<row>.py` runs its predecessor first, then writes its own rows. Running the two rows' chains
one after the other on the merged tree is lossy and depends on order:

- (ours, then theirs) differs from the driver result in 2 rows:
  - HUDINIT-A's chain predates ATOMIC-VIEW-2, so it reverts `0x004548e0` from matched back to stand-in.
  - A `0x004e7e30` note suffix is lost.
- (theirs, then ours) differs in `0x00427ca0` and `0x005c4d30`, and the row order changes.

The driver keeps both rows' deltas, which is the intended serial result. The chain is therefore **frozen** at
`update_bindings_clumptexread.py`. New rows use `verify/portorigtop_20261109/bindings_rows.py`: `apply(tag, FIXED,
APPEND)` edits BINDINGS.tsv in place with only that row's delta, is idempotent and runs no predecessor.

## Gates

- rva-lint OK: 35 known, NEW=0.
- Build OK (exit 0, 0 errors; the build input change is the `.inc` header comment only).
- G-CTRL `8284be46` / `67931834` / `31fe043e` and B `1d5bf4f9` x3, all **unchanged**.
  - Runner: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`
  - `A4_ROOT=D:/MashedWT/POWERUPINIT-C_rootA`, `A4_OUT=D:/MashedWT/HB-GENFILES-NOCONFLICT_data/arms`
  - Wrapped as test-slot, then mashed_lock.
- The drivers are registered in the shared clone config: `py -3.12 scripts/genfiles_merge.py --install`, run with
  no merge train process alive. `merge_train.ps1` needed no change, since git config plus `.gitattributes` are
  enough.

## Note for the worker brief

The note below is for the worker brief, on how a row adds seed entries and bindings:

> **Seed entries and bindings (from HB-GENFILES-NOCONFLICT, 2026-11-10).**
> - **Seed entries.** Add your rows to `verify/portstate_20261108/SEED_CANDIDATES.tsv`, then regenerate with
>   `py -3.12 re/tools/gen_portstate_seed.py`, and commit both files. Do not hand-edit `PortStateSeed.inc`.
> - **Bindings.** Do not chain a new `update_bindings_<row>.py` onto the previous one: that chain is frozen at
>   `update_bindings_clumptexread.py`. Write a script that calls
>   `bindings_rows.apply('<ROW>: ', FIXED, APPEND)` from `verify/portorigtop_20261109/bindings_rows.py`, with only
>   your own rows, and run it once.
> - **Merging.** `scripts/genfiles_merge.py` (`.gitattributes` `merge=genbindings` / `genseedtsv` / `genseedinc`,
>   registered once per clone with `py -3.12 scripts/genfiles_merge.py --install`) merges two rows' edits to these
>   three files by rva or address. Do not resolve a conflict on them by hand or by re-running another row's script.
>   A conflict there is a real disagreement: the same rva field, or the same seed address with different values.
>   Report it.
