D4 burn-down 5 -> 4 (measured: `py -3.12 scripts/lint_rva_bodies.py` prints `D4 burn-down=4  ORIGTOP-ONLY=647 ORIGTOP-HELD=10 ORIGTOP-HELD-HAND=1`, 662 findings, allowlisted=662, NEW=0). Met.

# RESULT LINT-HELDPAIR-HANDCOPY (run 57, lint tool row, no build, no gates, no game arm)

Base: orch/integration c880ab12e. Branch orch/LINT-HELDPAIR-HANDCOPY.

## Change
- `scripts/lint_rva_bodies.py`: new class `ORIGTOP-HELD-HAND` (`is_held_hand`): DUP-IN-TARGET, two or more bodies in
  `// RVA-LINT: ORIGTOP-ONLY` marked TUs plus at least one unmarked body. Disjoint from ORIGTOP-ONLY (exactly one marked) and
  ORIGTOP-HELD (no unmarked). CROSS-TARGET and DUP-INSTALL never. Counted apart in the summary line, --verbose, --seed tags,
  and the NEW-finding hint. An untagged line of the shape stays D4 until tagged (adding the class moves no count by itself).
  `evaluate()` now returns a 6th list (old_hand); the selftest compares 6-tuples (older cases padded).
- `re/tools/dual_copy_allowlist.txt`: line 745 only (`DUP-IN-TARGET:0041f360:Match/PortTop_00420420.cpp,Match/PortTop_00420870.cpp,Vehicle/CarMaterialTables.cpp`)
  prefixed with `ORIGTOP-HELD-HAND ` and its comment sentence "No lint class covers ... D4 untagged" replaced by the new class
  citation. `git diff --stat`: 1 line changed in the allowlist, every other line byte-identical.

## Checks run
- `py -3.12 scripts/lint_rva_bodies.py --selftest`: OK (0 failing). 12 new cases: tagged hand class lands in hand; untagged stays D4;
  unlisted is NEW; wrong tags ORIGTOP-HELD / ORIGTOP-ONLY fail; 2 marked + 2 unmarked in class; 2 marked no hand copy fails;
  1 marked + 1 unmarked fails; plain dual fails; L-178 scaffold dual fails; cross-target fails; mixed counts stay apart.
- `py -3.12 scripts/lint_rva_bodies.py` on the branch tree: OK, NEW=0, D4=4 (before: 5 with the untagged 0x0041f360 line).
- `py -3.12 scripts/test_rva_body_scan.py`: OK 13/13.
- Not run: build, ddkobj, binder selftest, gates (lint row).

## Notes / unfinished
- Allowlist line 730 (`ORIGTOP-ONLY DUP-IN-TARGET:0041f360:Match/PortTop_00420870.cpp,Vehicle/CarMaterialTables.cpp`, BIND-GATES-ALLKNOBS) is
  the stale two-file key that line 745 says it supersedes. The scanner no longer reports it, so it matches no finding. It is also a
  0x0041f360 line but was outside this row's single-line scope: left unchanged. A prune is a separate trivial row.
- The worktree add took about 15 minutes (41k-file checkout on D:, concurrent with another lead's worktree add).
