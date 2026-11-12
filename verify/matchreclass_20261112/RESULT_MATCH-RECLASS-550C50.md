# RESULT MATCH-RECLASS-550C50
Match-only row. Two RtFS handler functions score 100% with no hooks.csv row (found by RWC-MATCH-S03). Sources are on orch/integration (re/match/s24/).

| rva | source | toolchain | match.py --bytes | relocheck | asm/naked/STUB |
|---|---|---|---|---|---|
| 0x00550c50 | re/match/s24/00550c50.c | rw9044, /O2 /Ob1 /G5 | 298 B, 0 differing unmasked bytes, BYTE-IDENTICAL, 100/100 ins | OK checked=3 unchecked=3 (__imp__CreateFileA@28, __imp__GetFileSize@8, __imp__GetLastError@0) | none |
| 0x00550da0 | re/match/s24/00550da0.c | ddk (cl 13.00.9176), /O2 /Ob1 /G5 | 369 B, 0 differing unmasked bytes, BYTE-IDENTICAL, 139/139 ins | OK checked=5 unchecked=5 (__imp__CreateEventA@16, __imp__GetLastError@0, __imp__ReadFile@20) | none |

Commands: `py -3.12 re/tools/match/match.py <src> --bytes`, `py -3.12 re/tools/match/relocheck.py <src>`.
Rows added with `re/tools/c1_add.py` (subsystem third-party-library[renderware], same as the neighbouring RtFS handlers 0x00550520..0x00550a20). Promotion: re/promotions/MATCH-RECLASS-550C50.tsv (route match), applied by the merge train.
