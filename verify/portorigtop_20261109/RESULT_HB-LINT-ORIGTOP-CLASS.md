# RESULT HB-LINT-ORIGTOP-CLASS (L-111 (a)): the dual-copy lint gets an ORIGTOP-ONLY class

Branch `orch/HB-LINT-ORIGTOP-CLASS` from orch/integration `3fbf77214`. Files: `scripts/lint_rva_bodies.py`,
`re/tools/dual_copy_allowlist.txt` (header only). No mashedmod change, no build input other than the lint and the
allowlist header.

## What the class is

On the MASHED_ORIG_TOP route, a byte-matched body runs only when the exe enters the matched WinMain 0x00492370. Until
the ORIG_TOP flip it sits beside the hand body the default path still runs. The lint now accepts that pair as its own
class, counted apart from the D4 burn-down.

**How the lint knows a body is ORIG_TOP-only: a structural marker in the TU, not a list.** The matched body's TU
carries the line

```
// RVA-LINT: ORIGTOP-ONLY
```

The marker must be in that TU's own text; an included file does not count. Rules (`evaluate()` in the lint):
- **The TU is exe-only.** A marked TU that `asi_sources.rsp` compiles fails the build, because ORIG_TOP is an exe
  path.
- **What counts as the class.** Exactly one body of the finding is in a marked TU, and at least one is in an unmarked
  TU. That covers DUP-IN-TARGET in the exe and CROSS-TARGET against an asi-only hand body.
  - Two marked bodies for one RVA are a real duplicate, not this class.
  - DUP-INSTALL is never this class.
- **The allowlist line carries the tag:** `ORIGTOP-ONLY <CHECK>:<rva>:<files>   # citation`.
  - A tag on a finding that is not of the class fails the build.
  - A class finding on an untagged line also fails.
  - An unlisted class pair is NEW, like any other finding. Its ERROR line names the class and the tag to use.
  - `--seed` writes class lines already tagged.
- **Output:** the lint prints both counts on one line:
  `[rva-lint] D4 burn-down=<n>  ORIGTOP-ONLY=<m> (class deleted at the ORIG_TOP flip; <k> marked TU(s))`.
  `--verbose` lists class pairs as `INFO (ORIGTOP-ONLY)`, apart from the D4 `WARN (allowlisted)` lines.

The marker was chosen over a separate ORIG_TOP source list because the exe build compiles one `exe_sources.rsp`.
A second list would need a build.bat change and would still have to stay in step with that file. The marker
travels with the TU, the lint reads it from the file, and nothing is kept by hand.

The allowlist header documents the class and states that **the whole class is deleted at the ORIG_TOP flip**: the
hand bodies go, every `ORIGTOP-ONLY` line goes, and the markers are removed.

## Self-test

`py -3.12 scripts/lint_rva_bodies.py --selftest`: 12/12 PASS.

8 class cases on synthetic pairs. Each gives new / d4 / origtop / errors:

| case | result |
|---|---|
| tagged ORIG_TOP pair | (0,0,1,0) |
| tagged ORIG_TOP cross-target pair | (0,0,1,0) |
| untagged ORIG_TOP pair | (0,0,0,1) |
| unlisted ORIG_TOP pair | (1,0,0,0) |
| two marked bodies with the tag | (0,0,0,1) |
| tag on a plain dual copy | (0,0,0,1) |
| plain dual copy untagged | (0,1,0,0) |
| marker on an asi-compiled TU | (0,0,1,1) |

4 marker-regex cases:
- matches `// RVA-LINT: ORIGTOP-ONLY` and `//RVA-LINT:ORIGTOP-ONLY (L-111)`
- rejects `ORIGTOP-ONLYX` and a `/* */` comment

**End to end through the real scanner** (log `D:/MashedWT/HB-LINT-ORIGTOP-CLASS_data/e2e.log`). Setup:
- a temporary exe-only TU `Match/OrigTop_SelfTest.cpp` carrying the marker and an anchored body of 0x00407a00
- beside the real hand body `Gameplay/RangeTable_ah1.cpp:245 ReadAiCheckpointField`

Steps:
1. Not allowlisted: `NEW=1`, `ERROR DUP-IN-TARGET 0x00407a00 ... [ORIGTOP-ONLY pair: allowlist it with the
   'ORIGTOP-ONLY ' tag]`, rc 1.
2. The `--seed` line (`ORIGTOP-ONLY DUP-IN-TARGET:00407a00:Gameplay/RangeTable_ah1.cpp,Match/OrigTop_SelfTest.cpp`)
   appended: `D4 burn-down=2  ORIGTOP-ONLY=1`, OK, rc 0.
3. The same line without the tag: `ERROR ... is an ORIGTOP-ONLY pair on an untagged line`, rc 1.

The TU, the rsp line and the allowlist line were removed afterwards. `git status` shows only the two intended files.

## The current tree is unchanged

```
[rva-lint] 2 finding(s): CROSS-TARGET=1, DUP-IN-TARGET=1  |  allowlisted=2 NEW=0
[rva-lint] D4 burn-down=2  ORIGTOP-ONLY=0 (class deleted at the ORIG_TOP flip; 0 marked TU(s))
[rva-lint] OK -- no NEW duplicate bodies (2 known, tracked in re\tools\dual_copy_allowlist.txt)
```

These are the same 2 findings and 2 allowlisted lines as before the change, with 0 marked TUs.
`scripts/lan_release/archive_lint.py --ref HEAD` is OK.

Other readers of the allowlist are compatible:
- `re/tools/port_closed_sets.py` splits on `:` and still gets the RVA from a tagged line.
- `scripts/lan_release/archive_lint.py` filters `WARN (allowlisted)` lines and passes the rest through.
- `merge_train.ps1` runs the lint unchanged.

## For the step-1 subsystem commits

A matched ORIG_TOP body beside a hand body needs:
1. its wrapper TU exe-only, with `// RVA-LINT: ORIGTOP-ONLY` near the top
2. the line from `py -3.12 scripts/lint_rva_bodies.py --seed | grep ORIGTOP-ONLY`, appended to the allowlist with a
   citation

Nothing else changes. The SIZE count of such pairs was 92, listed per subsystem in RESULT_PORT-ORIGTOP-SIZE.md.
