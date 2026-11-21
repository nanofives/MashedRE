# RESULT LINT-ORIGTOP-HELDPAIR (L-179, tool rule inside L-111)

Change: `scripts/lint_rva_bodies.py` only (no exe-compiled code, no build or gates).

New class `ORIGTOP-HELD`: a DUP-IN-TARGET finding with 2+ bodies of one rva, every body in a TU carrying
`// RVA-LINT: ORIGTOP-ONLY`, not the L-178 scaffold dual. Allowlist tag `ORIGTOP-HELD <key>   # citation`.
Counted apart from D4 and ORIGTOP-ONLY; printed as `ORIGTOP-HELD=<n>`; deleted at the ORIG_TOP flip.
Rules (selftest cases in `--selftest`): held pair/triple tagged = held class; tag on a pair with an unmarked body,
a plain dual copy, CROSS-TARGET, or the L-178 scaffold dual = error; an unlisted held pair = NEW; a held pair on an
untagged line stays D4 until tagged (so landing the class moves no count); one marked + one unmarked body keeps
the ORIGTOP-ONLY class (D4 if untagged-and-not-class is impossible: untagged fails as before).

Before (head 65ef9f74a): selftest OK; `[rva-lint] D4 burn-down=6  ORIGTOP-ONLY=459`, 465 findings, NEW=0, OK.
After: selftest OK (0 failing, 20 cases + 4 marker shapes); `D4 burn-down=6  ORIGTOP-ONLY=459 ORIGTOP-HELD=0`,
465 findings, NEW=0, OK. D4 count identical.

Observation (for PORT-TOP-06): `DUP-IN-TARGET:004d5480` (RwCore_004d5480/004d5bc0/004d7480) and other all-marked
groups on the head are untagged D4 lines today. They can be retagged `ORIGTOP-HELD` by the row that owns them,
no lint change needed. [UNCERTAIN] which of the 6 D4 lines are all-marked: test is
`py -3.12 scripts/lint_rva_bodies.py --seed | grep ^ORIGTOP-HELD`.
