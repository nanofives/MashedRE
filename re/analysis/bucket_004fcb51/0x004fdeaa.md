---
rva: 0x004fdeaa
name_in_ghidra: FUN_004fdeaa
size_bytes: 101
confidence: C1
subsystem: shader-compiler
callees_depth1: []
callers_noted: [FUN_004fe2fe]
opened_in_slot: Mashed_pool8
session_date: 2026-05-18
---

## Mechanical description

String-escaping helper for `#`-stringizing macros (`__cdecl(src, src_len, dst_or_0)`). Two-pass-capable (dst==0 → just count; dst!=0 → write).

For each of `param_2` (src_len) characters of `param_1`:

- If `*src == '"'`: emit `\\` (0x5c) — also tracks balanced-quote state. The state machine has two bools `bVar1 (just-emitted-escape-for-quote)` and `bVar2 (inside-double-quote-region)`.
- If `bVar2 == true` AND `*src == '\\'`: emit `\\` (escape the backslash); set `bVar1` so the next char doesn't get backslash-escaped again.
- Always emit `*src`.
- Counter `iVar3` always increments by the number of bytes written.

End result: returns the count (well, doesn't return — but the count is in `iVar3` and would be the buffer-size needed). Caller uses dst==0 to size, then dst==buf to fill.

This is the canonical `# operator` in C/HLSL preprocessor: turn a token sequence into a string literal, escaping embedded `"` and `\\`.

## Constants

| Constant | Site | Note |
|---|---|---|
| 0x5c | byte literal '\\' | escape character |
| '"' | char literal | quote that triggers escape and state flip |
| '\\' | char literal | the char escaped inside quoted region |

## Uncertainties

- [UNCERTAIN] Whether `iVar3` is supposed to be returned — function returns void per decomp, but the count must escape somehow. Caller likely reads `iVar3` via a register convention Ghidra didn't surface.

## Stubs encountered

(none)
