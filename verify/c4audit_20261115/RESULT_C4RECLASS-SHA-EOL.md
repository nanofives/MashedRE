# RESULT C4RECLASS-SHA-EOL: GREEN

Row kind: tool row on `re/tools/c4_reclass.py`. No build, no gates (not run: tool row). Base `17202f6c3`.

## Choice: option (a), LF-normalised hash, tolerant of the old convention

- `src sha256 <16 hex>` = first 16 hex of SHA-256 over the source bytes with CRLF -> LF (`src_sha()`), the same rule as
  `re/tools/gen_portstate_seed.py` `sha256_text`. Used by `gate_match` and `gate_extent` (both former
  `hashlib.sha256(path.read_bytes())` sites). Same value on a CRLF checkout (core.autocrlf=true), an LF checkout, a
  `git archive` and the notebook hub.
- `src_sha_legacy()` = the pre-row CRLF-raw hash, computed from the content (LF form -> CRLF), so it is also
  independent of the checkout. `token_state(token, path)` returns `lf` (current), `crlf` (old convention, same content,
  not stale) or None (content changed).
- `refresh`: `lf` = SKIP; `crlf` = REFRESH labelled `eol: legacy CRLF-raw hash of unchanged content` (CHANGELOG
  `sha eol-normalise`); None = REFRESH `source content changed` (unchanged behaviour). Every REFRESH still recompiles
  and needs byte-identical + relocheck OK. Summary line now counts `eol-only` and `content changed` apart.
- `SHA_TOKEN` now takes a 16-hex or a full 64-hex token (`(?![0-9a-f])` boundary); comparison is on the first 16 hex,
  a refresh writes the 16-hex form. Census on base hooks.csv: 3197 tokens of 16 hex, 326 of 64 hex, no other length.
- New read-only `refresh-tsv --out <file> --result <RESULT>`: lists every C4 `c4=match` row whose token is not `lf`.
- Docs: module docstring, `.claude/skills/re-classify/SKILL.md` (the "CRLF under autocrlf" sentence replaced).
  `re/CONFIDENCE.md` has no hash rule to change.

## One-time refresh TSV (dry run only, no --apply on the branch)

`re/promotions/C4RECLASS-SHA-EOL-refresh.tsv` (header `rva src result`), from
`py -3.12 re/tools/c4_reclass.py refresh-tsv --out re/promotions/C4RECLASS-SHA-EOL-refresh.tsv --result verify/c4audit_20261115/RESULT_C4RECLASS-SHA-EOL.md`:

```
refresh-tsv: c4=match rows with token: lf 96, crlf 3072, content 76, no-source 0, no-token 5; 3148 rows -> re/promotions/C4RECLASS-SHA-EOL-refresh.tsv
```

Dry run `py -3.12 re/tools/c4_reclass.py refresh --tsv re/promotions/C4RECLASS-SHA-EOL-refresh.tsv --jobs 4`:

```
refresh: 3148 would-refresh (3072 eol-only, 76 content changed), 0 refused, 0 skipped (already current), 3148 rows
```

All 3148 recompiled byte-identical with relocheck OK. 3072 are the old convention only. 76 carry a token that matches
neither form: those sources changed after their token was written and were already stale before this row. 96 rows are
already LF-current (left out of the TSV). 5 C4 `c4=match` rows have no token (left out, unchanged).
The merge train applies the TSV with `refresh --apply --strict` on its head.

Raw log (not committed): `D:/MashedWT/C4RECLASS-SHA-EOL_data/refresh_dryrun.log`
sha256 `10b692838c6e249909dc2c00bc0ff994cc2d98ac1e67320698d70c667f30e3c6`.

## Tests

`py -3.12 -m pytest re/tools/tests/test_c4_reclass.py -v`: **40 passed in 1.70s** (21 before, 19 new or parametrised
cases). New: `test_src_sha_is_eol_independent[lf|crlf]`, `test_batch_note_hash_same_on_lf_and_crlf_checkout`,
`test_refresh_legacy_crlf_token_is_eol_only[lf|crlf]`, `test_refresh_lf_token_is_current_on_either_checkout[16hex|64hex x lf|crlf]`,
`test_refresh_stale_64hex_token_becomes_16hex`, `test_refresh_tsv_lists_only_non_current_rows`. `_cursha` in the old
refresh tests now uses the LF-normalised hash.

## Transition note

Until the train applies the TSV, rows written by `batch` on other branches after this merges carry LF tokens and older
rows carry CRLF tokens. `token_state` tells them apart (`lf` / `crlf`), so neither is reported as a content change: a CRLF token is only
relabelled `eol-only` by refresh. A promotions TSV
applied by the train after this merge writes LF tokens.
