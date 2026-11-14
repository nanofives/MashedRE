# RESULT MATCH-C4RECLASS-TAG-OP-2

Branch orch/MATCH-C4RECLASS-TAG-OP-2 = orch/MATCH-C4RECLASS-TAG-OP (e5d94e26f) + merge of orch/HB-TEST-TOOLCHAIN-DDK (34bc5abcb), no conflicts. Whole file re/tools/tests/test_c4_reclass.py (includes the 4 tag-op tests): 13 passed in 3.86s.

```
============================= test session starts =============================
platform win32 -- Python 3.12.10, pytest-9.1.1, pluggy-1.6.0 -- C:\Users\maria\AppData\Local\Programs\Python\Python312\python.exe
cachedir: .pytest_cache
rootdir: C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-C4RECLASS-TAG-OP-2
plugins: anyio-4.15.1
collecting ... collected 13 items

re/tools/tests/test_c4_reclass.py::test_dry_run_writes_nothing PASSED    [  7%]
re/tools/tests/test_c4_reclass.py::test_apply_touches_only_promoted_lines PASSED [ 15%]
re/tools/tests/test_c4_reclass.py::test_apply_needs_result PASSED        [ 23%]
re/tools/tests/test_c4_reclass.py::test_strict_writes_nothing_on_refusal PASSED [ 30%]
re/tools/tests/test_c4_reclass.py::test_match_refusals[ev0-not byte-identical] PASSED [ 38%]
re/tools/tests/test_c4_reclass.py::test_match_refusals[ev1-relocheck BAD] PASSED [ 46%]
re/tools/tests/test_c4_reclass.py::test_match_refusals[ev2-shorter than the original body] PASSED [ 53%]
re/tools/tests/test_c4_reclass.py::test_toolchain_must_be_ddk PASSED     [ 61%]
re/tools/tests/test_c4_reclass.py::test_second_apply_is_a_noop PASSED    [ 69%]
re/tools/tests/test_c4_reclass.py::test_tag_dry_run_writes_nothing PASSED [ 76%]
re/tools/tests/test_c4_reclass.py::test_tag_apply_is_notes_only_and_idempotent PASSED [ 84%]
re/tools/tests/test_c4_reclass.py::test_tag_refuses_missing_row_and_strict PASSED [ 92%]
re/tools/tests/test_c4_reclass.py::test_batch_lever_column_tags_promoted_row PASSED [100%]

============================= 13 passed in 3.86s ==============================
```
