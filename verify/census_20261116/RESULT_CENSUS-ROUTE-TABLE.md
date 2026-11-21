# RESULT CENSUS-ROUTE-TABLE (run 53, text-only)

Committed `re/census/` (README.md, t2_route_table.tsv, t2_route_table.py, census_join2.py, xtab.py, logs, t2_nonc4.tsv).
Input commit b24bba46c. Row kind: text, no build, no gates.

Counts (from xtab.py and t2_route_table.py, both runs): 618 functions, game code 213, library 203, RenderWare core 92,
RW-Physics 62, no hooks.csv row 48. Same as BATCH_PLAN_runs53-56.md. Share of T2 calls: 68.9 / 1.7 / 20.0 / 3.1 / 6.3 %.

Check run: re-ran census_join2.py (t2_nonc4.tsv cmp-identical to log/orch/audit_20261121/t2_nonc4.tsv), xtab.py, and
t2_route_table.py; the two tables hold the same 618 rvas with equal calls and best_pct on every row.
Limit: generated with --root on the main checkout because verify/provinsitu_20261111/PROV_VERDICTS.tsv is not on orch/integration.
