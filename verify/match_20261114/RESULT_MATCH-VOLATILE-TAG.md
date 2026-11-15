# RESULT MATCH-VOLATILE-TAG

L-154 (a): tag `lever=volatile` on 7 existing `c4=match` rows. Tracker batch, text only. No build, no gates.

## Method
The row's note asked for `c4_reclass.py batch` with a 5th `lever` column. That column only applies when the batch
promotes a row, and these 7 are already C4. The matching operation is `c4_reclass.py tag` (notes-only tag for an
existing row). The merge train applies a `re/promotions/<ID>-tag.tsv` with `tag --apply --strict` (HB-TRAIN-TAG-TSV,
`merge_train.ps1:325-362`), so the file is `re/promotions/MATCH-VOLATILE-TAG-tag.tsv`, header `rva`, `tag`, `result`.

## Rows (hooks.csv at orch/integration 198109d50)
| rva | confidence | c4=match in notes | lever=volatile before |
|---|---|---|---|
| 0x005baf60 | C4 | yes | no |
| 0x004111c0 | C4 | yes | no |
| 0x0054d4d0 | C4 | yes | no |
| 0x0054dbd0 | C4 | yes | no |
| 0x005b1500 | C4 | yes | no |
| 0x00538310 | C4 | yes | no |
| 0x004dcbb0 | C4 | yes | no |

0x004dcbb0 landed C4-by-match untagged from MATCH-RWT-S5 (the batch ignored the lever column).

## Dry run
`py -3.12 re/tools/c4_reclass.py tag --tsv re/promotions/MATCH-VOLATILE-TAG-tag.tsv --strict` (no --apply):

```
[1/7] 0x005baf60 tag TAG would append lever=volatile
[2/7] 0x004111c0 tag TAG would append lever=volatile
[3/7] 0x0054d4d0 tag TAG would append lever=volatile
[4/7] 0x0054dbd0 tag TAG would append lever=volatile
[5/7] 0x005b1500 tag TAG would append lever=volatile
[6/7] 0x00538310 tag TAG would append lever=volatile
[7/7] 0x004dcbb0 tag TAG would append lever=volatile
tag: 7 would-tag, 0 refused, 0 skipped (already tagged), 7 rows
dry run: nothing written
```

## Files
- `re/promotions/MATCH-VOLATILE-TAG-tag.tsv` sha256 `50bbfc5c31f2ea0214fa52bb873a05d5eeaf07d973360c43c1351146e56b315b`
- hooks.csv and CHANGELOG.md untouched on this branch. The merge train writes them.

## Branch note
`orch/MATCH-VOLATILE-TAG` already existed from run 45 with no commits of its own. It was at 2c7f97e41 and
fast-forwarded to orch/integration 198109d50 before the work.
