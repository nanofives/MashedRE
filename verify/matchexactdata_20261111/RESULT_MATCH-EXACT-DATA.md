# RESULT MATCH-EXACT-DATA (2026-11-11)

Verdict: **GREEN**. relocheck now compares every data operand named after its address. One ledger run under the new
check refused 2 C4-by-match rows. Both sources were fixed to the original's operand order and pass again. Nothing was
demoted. hooks.csv is untouched.

## The hole

`match.py` masks relocations, and `relocheck.py` resolved only `MGLOBAL` names, `FUN_<rva>` callees and its own
constants. A source that declares `extern float DAT_005cc32c;` (the g2s / Ghidra naming used by most game-code sources)
had every data operand listed as **unchecked by name**. So two loads of different globals could be swapped and the row
still scored 100% with relocheck OK. 0x00414060 was C4 by match with `checked=5 unchecked=12`, all twelve `_DAT_` names.

## The change (relocheck.py, not match.py)

relocheck is the operand checker already, and match.py stays a byte score. The change sits in relocheck:

- New branch: a symbol whose undecorated name fully matches
  `(DAT|UNK|BYTE|WORD|DWORD|QWORD|FLOAT|DOUBLE|PTR_\w+|s_\w*|u_\w*)_<8 hex>` is expected at `0x<8 hex>` + the stored
  addend (DIR32) and compared with the original's operand, the same way as `FUN_` and `MGLOBAL`.
- `MGLOBAL` names still win (checked first). Everything else is unchanged.
- `check(src, named_data=True)`. CLI `--no-named-data` gives the old behaviour (used by the self-test only).

0x00414060 under the new check, source at 778b8b91d:
```
0x00414060  BAD  checked=20 unchecked=0  +0xf _DAT_005cc32c: ours 0x005cc32c vs orig 0x0089a37c; +0x15 _DAT_0089a37c: ours 0x0089a37c vs orig 0x005cc32c
```

## Self-test

`py -3.12 re/tools/match/test_relocheck.py` (also under pytest). Fixtures in `re/tools/match/relocheck_tests/`:
`swap_00414060.c` is the frozen 778b8b91d source, `control_00413fa0.c` a byte-identical control.
```
[1/4] swap_strict_bad PASS      swapped source -> BAD, both swapped sites named
[2/4] swap_legacy_ok PASS       same source, --no-named-data -> OK (the hole, reproduced)
[3/4] control_strict_ok PASS
[4/4] control_legacy_ok PASS
```
`py -3.12 -m pytest re/tools/tests/test_c4_reclass.py`: exit 0.

## Ledger run (once)

`py -3.12 verify/matchexactdata_20261111/build_ledger.py --jobs 8` (copy of the batch-5 builder; relocheck reached
through `c4_reclass.eval_source`). 4,111 sources, 4,133 rvas. Log `D:/MashedWT/matchexactdata_ledger.log`
(sha256 `32332d6742a19a573a6e4787986aa37e07524013ddabb89ee3fb8d84c9b9ad62`). `compare.py` against the ledger at
778b8b91d:

| before -> after | rows |
|---|---|
| PASS -> PASS | 2,978 (2,979 after the fixes below) |
| PASS -> REFUSED | **1** (0x004030d0; 0x00414060 was already fixed in the tree before the run) |
| REFUSED -> PASS | 41 |
| REFUSED -> REFUSED | 1,065 |
| new -> PASS / REFUSED | 29 / 19 |

The 41 REFUSED->PASS and 48 new rows are sources merged after the last ledger build (the ledger had not been
rebuilt since batch 5), not an effect of this change. 10 sources error on compile or Ghidra size, all already
REFUSED or covered by another passing source before.

## Rows the stricter compare refused

Both were C4 by route match (L-93). Both were byte-identical with the two operands of one x87 op swapped:

| rva | hooks.csv | original | old source | fix | now |
|---|---|---|---|---|---|
| 0x00414060 | C4, port called | operand `0x0089a37c` at +0xf, `0x005cc32c` at +0x15 (ours had them the other way) | `DAT_0089a37c * DAT_005cc32c` | `DAT_005cc32c * DAT_0089a37c` | 186 B identical, relocheck OK 20/0 |
| 0x004030d0 | C4 | `fld [0x00636ae8]` +0x9, `fadd [0x007f1004]` +0x10, `fstp [0x00636ae8]` | `A = A + B` (cl loads B first; `A += B`, `(float)(A+B)`, `B + A` all the same) | `step = DAT_007f1004; DAT_00636ae8 = DAT_00636ae8 + step;` | 137 B identical, relocheck OK 16/0 |

Since both pass with the right operands, the RE they certify was right in value. The text had the operands in the wrong
order. No demotion is needed. `c4_reclass.py batch` dry run on the two (`recheck_batch.tsv`, `recheck_dryrun.log`):
both SKIP "already C4": the batch tool does not re-gate C4 rows. The re-gate is the ledger's `gate_match` (independent of
hooks.csv), whose rows read PASS for both. Their hooks.csv notes still cite the old source sha
(0x00414060 `467f5f3c30c62d5f`). New shas: 0x00414060 `035003945058d28f`, 0x004030d0 `d4d1d0d68787b3c5`. Notes are
not rewritten here (no re-classify operation updates a C4 row's evidence).

Port: `Match/MatchS03_00414060.cpp` compiles `h3_00414060.c`. The change swaps the operands of one float multiply,
which is commutative in IEEE arithmetic, so the VS2022 body computes the same value. The DDK object is byte-identical
to the original either way. No build or gates run (match-only row).

## Files

- `re/tools/match/relocheck.py`: named-data branch, `named_data` flag, `--no-named-data`.
- `re/tools/match/test_relocheck.py`, `re/tools/match/relocheck_tests/{swap_00414060,control_00413fa0}.c`.
- `re/match/s03/h3_00414060.c`, `re/match/s01/004030d0.c`: operand order + a note.
- `re/CONFIDENCE.md` C4 match item 2: names the new check.
- `re/analysis/match_ledger.tsv`: rebuilt; the two fixed rows re-gated in place afterwards.
- this folder: `build_ledger.py`, `compare.py`, `pairs.tsv`, `batch.tsv` (3,049 match rows passing now),
  `refused.tsv` (empty after the fixes), `recheck_*`.

## Follow-ups

- `c4_reclass.py batch` skips C4 rows, so it cannot re-gate them after a rule change. A `--recheck` mode that gates C4
  rows and lists the ones that would drop would make the next tightening a one-command check.
- Of the 3,049 passing ledger rows, 3,048 are C4 already. The one left has no hooks.csv row (`hooks_conf` = `(no row)`).
