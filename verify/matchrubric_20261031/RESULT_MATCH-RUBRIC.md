# MATCH-RUBRIC (run 27, 2026-10-31)

Text-only. Branch `orch/MATCH-RUBRIC` off `orch/MATCH-MERGE`. Files changed: `re/CONFIDENCE.md`,
`.claude/skills/re-classify/SKILL.md`. No tracker, hooks.csv, `original\` write, no build, no game run.

## Delivered
1. `re/CONFIDENCE.md`: new section "C4 routes by byte match" after C4. Routes `match` and `libid` worded as in the
   roadmap-loop MATCH section (DDK cl 9176 / c2 9178, flags, exe uses the source as the body, one body per RVA, both
   targets, `x87_tus.txt`, gates unchanged, matched source alone is not C4, naked inline asm is not evidence, inline
   `__asm` inside compiled C is).
2. Write-set leg: stated as a frida-route leg. Byte identity plus the same body compiled into the exe discharges it for
   the function's own stores. Residuals carried by the gates: callees, global/heap state behind masked relocations,
   inputs the port computes differently. Rows with unexercised callees or globals say so in `notes`.
3. `re-classify` accepts evidence types `match` and `libid`. Required fields: tool command, compiler build, flags,
   matched source path + sha, port body file. Refusal checks are listed in the skill.
4. CRT rows: `libid` does not apply when the port gets the function from the platform CRT (the "same vendored
   source" condition is false by design). That follows from the text. How those rows count toward P-DoD is not
   derivable, so a decision was filed.

## Decision filed
CRT rows (342 in hooks.csv, 203 byte-identical to DDK libcmt, port links the platform CRT). Options:
A) waive at library level (platform CRT, out of P-DoD); B) link the DDK libcmt member into the exe for the identical
ones so `libid` applies (changes the port link, gate risk); C) keep on the frida route per function.
Recommendation: A. Evidence: `verify/libidentity_20261030/RESULT_LIB-IDENTITY.md`.

## Not verified
Wording was checked against the roadmap-loop MATCH section, the pilot and libid RESULTs, and CONFIDENCE.md text only.
No tool was run.
