# Stubs

Every time a reimplementation calls a function that has not itself been reversed, the placeholder is recorded here. **Stubs block S-DoD.** A subsystem cannot be marked DONE while it has open stubs.

A "stub" is one of:
- A passthrough that calls the original at its RVA (`call_original_0x00xxxxxx()`).
- A no-op that returns a hardcoded value.
- A function with the body `assert(0 && "TODO");`.

Each stub gets one row. Resolve by reversing the target function (preferred) or by promoting the stub to `DEFERRED.md` with a wontfix rationale (rare).

## Active stubs

| ID | RVA called | Caller (RVA / name) | Subsystem | Type | Inserted | Notes |
|----|-----------|---------------------|-----------|------|----------|-------|
| S-0001 | 0x00499730 | 0x00493900 sub_00493900 | boot | passthrough | 2026-05-02 | Returns command-line string pointer; called as FUN_00499730(); depth-3 |
| S-0002 | 0x004a3ac9 | 0x00493900 sub_00493900 | boot | passthrough | 2026-05-02 | strtok-like tokenizer; takes (str, delimiter); depth-3 |
| S-0003 | 0x00499890 | 0x00499ba0 sub_00499ba0 | boot | passthrough | 2026-05-02 | Called after CreateWindowExA; no visible args; depth-3 |
| S-0004 | 0x004c5a60 | 0x004c5930 sub_004c5930 | boot | passthrough | 2026-05-02 | Linked-list traversal helper; takes (ptr-2, 0); depth-3 |
| S-0005 | 0x004d8060 | 0x004c5930 sub_004c5930 | boot | passthrough | 2026-05-02 | Called with (&DAT_00618150, param_1); depth-3 |

## Resolved stubs (audit trail — do not delete)

| ID | RVA | Caller | Resolved date | Resolution |
|----|-----|--------|---------------|------------|
|    |     |        |               |            |

## Conventions

- ID format: `S-NNNN`, monotonic, never reused.
- Every stub must have a corresponding `// STUB S-NNNN` comment in source.
- `re-classify` skill writes new rows; `hook-author` skill enforces that no new C3 row in `hooks.csv` lands while the function still has unresolved stubs.
