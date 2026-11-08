# RESULT C4R-AC80-EXTENT (2026-11-07)

**Verdict: GREEN.** Ghidra's body of 0x0040ac80 (228 B, 5 ranges) is exactly the 80 B `// MATCH:` symbol plus the four
`// HIDDEN:` handler symbols of `re/match/s02/0040ac80.c`, with inter-function padding the only other bytes in the window,
all byte-identical against the compiled object. `re/tools/c4_reclass.py` now takes a per-RVA extent override
(`re/tools/match/extent_overrides.tsv`, one row). Full gate regression over every re/match source: 3,589 rows, 1 changed
(0x0040ac80 REFUSED -> PASS), 0 others. 0x0040ac80 promoted **C2 -> C4** by route match through `c4_reclass.py batch --apply`.
Text-only: no build of mashed_re, no game run, gates not run. tracker_lint NEW 0 before and after.

Branch `orch/C4R-AC80-EXTENT` from `orch/integration` (510fe2b98).

## 1. Ghidra extent (Mashed_pool0, read-only, analyzeHeadless + `FuncRangesPC.java`)

Ghidra MCP is not on the account2 allowlist, so the pool slot was read with `analyzeHeadless -readOnly -noanalysis`
and `FuncRangesPC.java` (this folder). Output: `ghidra_extent.txt`.

`FUN_0040ac80` body: 228 addresses in **5 ranges**:

| range | len | instructions (ghidra_extent.txt) |
|---|---|---|
| 0x0040abc0..0x0040abde | 31 | `MOV EAX,[0x008a9588]` .. `RET` at 0x0040abde |
| 0x0040abe0..0x0040abfe | 31 | `MOV EAX,[0x008a9594]` .. `CALL 0x0042c1a0` at 0x0040abef .. `RET` at 0x0040abfe |
| 0x0040ac00..0x0040ac3d | 62 | `MOV EAX,[0x008a9590]` .. two `RET` at 0x0040ac1e / 0x0040ac3d |
| 0x0040ac40..0x0040ac71 | 50 | `CMP dword ptr [0x008a958c],0x7` .. `RET` at 0x0040ac71 |
| 0x0040ac80..0x0040acb5 | 54 | `MOV EAX,[0x008a9584]`, `DEC EAX`, `CMP EAX,0x5`, `JA 0x0040acb5`, `JMP dword ptr [EAX*0x4 + 0x40acb8]`, `JMP 0x0040ac40` (0x0040ac92), `JMP 0x0040ac00` (0x0040ac97), `JMP 0x0040abe0` (0x0040ac9c), `JMP 0x0040abc0` (0x0040aca1), `CALL 0x0042c1a0` (0x0040aca6), `MOV [0x008a9584],0x0`, `RET` (0x0040acb5) |

31+31+62+50+54 = 228 = pc_funcs.csv `0x0040ac80,0x0040acb5,228`. No function entry exists at 0x0040abc0 / 0x0040abe0 /
0x0040ac00 / 0x0040ac40 (probe lines: `function_at=-`, `containing=FUN_0040ac80`). Bytes in 0x0040abc0..0x0040accf owned by
no function: 0x0040abdf, 0x0040abff, 0x0040ac3e..3f, 0x0040ac72..7f (all `90`), 0x0040acb6..b7 (`8b ff`), the jump table
0x0040acb8..0x0040accf (`a1ac4000 92ac4000 97ac4000 9cac4000 b5ac4000 a6ac4000`). Next entry: FUN_0040acd0 at 0x0040acd0.

## 2. Byte-for-byte against the compiled object (`extent_proof.py` -> `extent_proof.txt`)

Source compiled as match.py does (XP DDK, `// FLAGS: /O2 /GS /Ob1`), original = `original/MASHED.exe.unpatched`. Each
member's obj symbol (code + tables + its section's trailing pad) is laid at its RVA. Non-relocation bytes compared;
every relocation resolved from the obj and compared with the original operand (DIR32 dword / REL32 site+4+disp).

| member | obj symbol | covers | trailing pad | relocs | differing |
|---|---|---|---|---|---|
| `_FUN_0040abc0` (HIDDEN) | 32 B | 0x0040abc0..0x0040abdf | 1 B `90` | 2 (DAT_008a9588, DAT_008a9584) | 0 |
| `_FUN_0040abe0` (HIDDEN) | 32 B | 0x0040abe0..0x0040abff | 1 B `90` | 3 (DAT_008a9594, FUN_0042c1a0 REL32, DAT_008a9584) | 0 |
| `_FUN_0040ac00` (HIDDEN) | 64 B | 0x0040ac00..0x0040ac3f | 2 B `90 90` | 5 | 0 |
| `_FUN_0040ac40` (HIDDEN) | 64 B | 0x0040ac40..0x0040ac7f | 14 B `90` | 5 | 0 |
| `_FUN_0040ac80` (MATCH) | 80 B | 0x0040ac80..0x0040accf | 0 | 14: 2 DAT_008a9584, 4 REL32 tail jumps resolving to 0x0040ac40 / 0x0040ac00 / 0x0040abe0 / 0x0040abc0, 1 REL32 FUN_0042c1a0, 1 table base `$L604` -> 0x0040acb8, 6 table entries -> 0x0040aca1 / 0x0040ac92 / 0x0040ac97 / 0x0040ac9c / 0x0040acb5 / 0x0040aca6 | 0 |

Window 0x0040abc0..0x0040accf = 272 B = 32+32+64+64+80 member bytes + **0 B uncovered**. Ghidra body bytes outside every
member symbol: **0**. Member bytes outside the Ghidra body: 44 (the 18 pad bytes, `8b ff` and the 24 B jump table, all
in the table above). `VERDICT PROVEN: differing bytes 0, body bytes uncovered 0, non-pad gaps 0`.

So the extent is the 80 B dispatcher symbol plus four separately compiled statics. 228 B is not one compiled function.

## 3. Gate change (`re/tools/c4_reclass.py`, `re/tools/match/extent_overrides.tsv`)

- `extent_overrides.tsv`: `rva, source, members, evidence`. Row: `0040ac80  re/match/s02/0040ac80.c
  FUN_0040ac80@0040ac80:54,_FUN_0040abc0@0040abc0:31,_FUN_0040abe0@0040abe0:31,_FUN_0040ac00@0040ac00:62,_FUN_0040ac40@0040ac40:50`
  (member size = the Ghidra range it covers).
- `eval_source` adds, only for a source named in the table, a per-member pass on one more compile: `score_function` at the
  member size, `byte_compare` on the whole symbol, `relocheck.check` with exactly the members as annotations.
- `gate_match` branches to `gate_extent` only when the row's RVA is in the table. Refuses unless: the override's source is
  the batch source, the first member is the row, member sizes sum to pc_funcs `body_size`, each HIDDEN member has a
  matching `// HIDDEN: 0x<rva> <sym> <size>` marker, no member is naked / asm-only / STUB, and every member is insn 100%,
  0 differing bytes and relocheck OK. Every other row goes through the unchanged default path.
- Negative checks (`negative_gate.py` -> `negative_gate.txt`), 5/5 as expected: control PASS; sizes sum 227 REFUSED;
  one constant changed in 0x0040abc0 (`0xb` -> `0xc`) REFUSED "1 differing unmasked bytes of 31"; HIDDEN marker of
  0x0040abe0 removed REFUSED; override naming another source REFUSED.

## 4. Regression (`regress_gate.py`, `regress_before.tsv`, `regress_after.tsv`, `regress_diff.txt`)

`gate_match` on every `// MATCH:` row of every re/match source (3,236 sources, 3,589 rows, hooks.csv state ignored so
rows already at C4 are gated too), before and after the change.

| | rows | PASS |
|---|---|---|
| before | 3,589 | 2,788 |
| after | 3,589 | 2,789 |

`diff` = one line: `0x0040ac80 re/match/s02/0040ac80.c` REFUSED "whole symbol bytes equal but shorter than the original
body (12/22 insns, 54.5%)" -> PASS "extent override: 5 members = 228 B, each byte-identical, relocheck OK (22/0)".
**0 other rows changed** (verdict or detail text).

## 5. Promotion

`batch.tsv` (one row). Dry run `batch_dryrun.txt`: `[1/1] 0x0040ac80 match PASS`. Applied with
`c4_reclass.py batch --tsv verify/c4rac80extent_20261107/batch.tsv --apply --result verify/c4rac80extent_20261107/RESULT_C4R-AC80-EXTENT.md --date 2026-11-07`
(`batch_apply.txt`): hooks.csv line 799 only, **C2 -> C4**, notes appended with `c4=match` and the member list
(0x0040abc0, 0x0040abe0, 0x0040ac00, 0x0040ac40 named there); one CHANGELOG line. No hooks.csv rows added for the four
handlers (inlined statics, no pc_funcs row). Port column unchanged (`none`).

## 6. Lint

`tracker_lint.py --repo . --base orch/integration`: before `pre-existing=3 NEW=0` (`lint_before.txt`), after `pre-existing=3 NEW=0`
(`lint_after.txt`), hooks.csv 5974 rows both times.
