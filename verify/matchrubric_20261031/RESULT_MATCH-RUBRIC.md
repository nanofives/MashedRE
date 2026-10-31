# MATCH-RUBRIC (run 27, 2026-10-31)

Text-only. Branch `orch/MATCH-RUBRIC` off `orch/MATCH-MERGE`, then `orch/integration` (0116b9fec) merged in for the
follow-up. Files changed: `re/CONFIDENCE.md`, `.claude/skills/re-classify/SKILL.md`. No tracker, hooks.csv or
`original\` write, no build, no game run.

## Delivered
1. `re/CONFIDENCE.md`: new section "C4 routes by byte match" after C4. Routes `match` and `libid` worded as in the
   roadmap-loop MATCH section (DDK cl 9176 / c2 9178, flags, exe uses the source as the body, one body per RVA, both
   targets, `x87_tus.txt`, gates unchanged, matched source alone is not C4, naked inline asm is not evidence, inline
   `__asm` inside compiled C is).
2. `match` also requires `re/tools/match/relocheck.py` OK next to match.py 100%. Reason: match.py masks every
   relocation, and MATCH-S02 found 0x0040b250 and 0x0040b810 at 100% while storing to swapped arrays
   (`verify/matchs02_20261031/RESULT_MATCH-S02.md`). The tool lives on `orch/MATCH-S02` @ 250b4c74d and reaches
   integration with that branch. The rubric names it by path.
3. Callerless bodies: a `match` row may be C4 with no exe caller. The rubric states that the unchanged gates then show
   only that the swap changed nothing the default build runs. They do not exercise the body, its callees or its
   inputs, so C4 rests on identity + relocheck + one body per RVA, and `notes` records `exe-callers=0`. The
   frida-route "body must execute" rule does not apply to this route.
4. Write-set leg: stated as a frida-route leg. Byte identity plus the same body in the exe discharges it for the
   function's own stores, and relocheck checks the address behind each named global, callee and constant. Residuals
   the gates carry: callee behaviour, relocations relocheck lists as unchecked (imports, CRT helpers, file-static
   data), the layout and contents of port state behind a correct address, and inputs the port computes differently.
5. `re-classify` accepts evidence types `match` and `libid`. Required fields: tool command, relocheck result
   (`match` only), compiler build, flags, matched source path + sha, port body file. Refusal checks are listed.
6. CRT rows, L-67 answered A (Mariano 2026-10-31, `log/orch/DECISIONS.md`): waived at library level, out of the P-DoD
   count, not C4, exe link unchanged. The rubric and the skill carry the settled rule:
   - `status` = `waived-platform-crt`, a single token because the column holds one-word values (`mapped`, `impl`,
     ...) and a comma would need CSV quoting. The confidence cell is not changed (all 342 CRT rows are C2 today).
   - `notes` starts `identified, waived: platform CRT (L-67)` plus the libid provenance (libmatch command, member,
     `@comp.id`) for the byte-identical rows. An ambiguous row lists every candidate member. The other rows start
     `waived: platform CRT (L-67)`.
   - How MATCH-LIB applies it: through re-classify's "CRT rows (L-67)" operation, over the CRT rows and the 197 CRT
     functions filed under other subsystems. Each of the 197 is refiled to the CRT bucket its address range already
     uses (`third-party-library[msvc-crt-main]`, `-tail` or `[msvc-crt]`) with the same status and notes, plus one
     changelog line per row.

## For the lead
- The P-DoD count in `ROADMAP.md` should drop the CRT rows. Not edited here (WORKER_RULES: ROADMAP.md only when it is
  the step's deliverable). Whoever owns the P-DoD text (D5-PDOD) applies it.
- Status token `waived-platform-crt` is new. Any script that enumerates `status` values (for example a P-DoD counter)
  needs to know it. Not checked here.

## Not verified
The wording was checked against the roadmap-loop MATCH section, the MATCH-PILOT, LIB-IDENTITY and MATCH-S02 RESULTs,
the L-67 answer and the CONFIDENCE.md text only. No tool was run.
