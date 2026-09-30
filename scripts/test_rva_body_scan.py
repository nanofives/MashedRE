"""Self-test for `scripts/rva_body_scan.py`'s anchoring.

Why this exists: on 2026-09-29 `0x0046b1c0` shipped TWO bodies in
`mashed_re.exe` and `scripts/lint_rva_bodies.py` reported `NEW=0`
(`re/analysis/D2_REOPEN_2026-09-29.md` §15.4, commit 7908af78). The scanner
anchored the naked copy (`Vehicle/VehicleSlotAabbExpand.cpp:34`) and MISSED the
other one, because `Vehicle/VehicleInit.cpp` annotated it in a HEADER block far
above the definition instead of directly above it. One miss is enough: the
DUP-IN-TARGET check needs both.

`rva_body_scan.scan_tu` now runs a second anchoring pass for exactly that shape.
This file is the falsifiable proof, run as:

    py -3.12 scripts/test_rva_body_scan.py

It builds a synthetic TU tree, so it cannot rot with the real sources, and it
includes the negative controls that keep the second pass from inventing bodies.
"""
from __future__ import annotations

import pathlib
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rva_body_scan as S            # noqa: E402

FAILED: list[str] = []


def check(name: str, got, want) -> None:
    if got == want:
        print(f"  PASS  {name}")
    else:
        print(f"  FAIL  {name}\n          got  {got}\n          want {want}")
        FAILED.append(name)


# --- the shape the guard used to miss -------------------------------------
# An RVA header comment, then a NON-signature line (the exact thing that stopped
# the first pass in VehicleInit.cpp: a `static const float ...[6] = {`), then the
# definition much further down.
UNANCHORED = """\
// Mashed RE - some subsystem.
//
// 0x0046b1c0 - VehicleBuildContactHull(slot, box)
//   builds the 14 contact-hull points from a 6-float box.
//   [0x0040ed62] call 0x0046b1c0
static const float kContactHullBox[6] = {
    0.2188f, 0.3086f, 0.4538f, -0.2188f, 0.0374f, -0.5233f,
};

void VehicleBuildContactHull(int slot, const float* b) {
    (void)slot; (void)b;
}
"""

# The classic shape the first pass has always caught.
ANCHORED = """\
// 0x0046b1c0  VehicleSlotAabbExpand
int VehicleSlotAabbExpand(unsigned slot, float* in6) {
    (void)slot; (void)in6;
    return 0;
}
"""

# NEGATIVE CONTROL 1: an RVA mentioned in call prose, with the TU's function
# defined under a DIFFERENT name and out of the first pass's reach (the next
# non-comment line is not a signature). The second pass must anchor nothing:
# the token after the RVA is `call`, which this TU does not define.
#
# STATED LIMIT, not fixed here: if the definition DID follow the comment block
# directly, the FIRST pass would bind it -- that looseness predates this file
# and is the documented "closest comment wins" rule in rva_body_scan.py. It is
# not what U-9156 set out to change.
NEG_CALLSITE = """\
// 0x00470afe  call 0x0046ef70   THIS FUNCTION
// The chain is 0x00470ae8 -> 0x00470aef -> 0x00470afe.
static int kPad1[2] = { 0, 0 };

void SomethingElse(int* self) { (void)self; }
"""

# NEGATIVE CONTROL 2: one name annotated with TWO different RVAs. Ambiguous, so
# the second pass must refuse rather than pick.
NEG_AMBIGUOUS = """\
// 0x00468d80 - AmbiguousName
// 0x004694e0 - AmbiguousName
static int kPad[2] = { 0, 0 };

void AmbiguousName(void) { }
"""

# NEGATIVE CONTROL 3: a data-range RVA (>= TEXT_END = 0x005d0000) must stay
# excluded. 0x005d8b41 is the RenderWare "Core built at" string in the anchored
# image, i.e. .rdata, not code.
NEG_DATA = """\
// 0x005d8b41 - DataOnlyThing
static int kPad3[2] = { 0, 0 };

void DataOnlyThing(void) { }
"""


def main() -> int:
    with tempfile.TemporaryDirectory() as td:
        root = pathlib.Path(td)
        (root / "Vehicle").mkdir()
        (root / "Vehicle" / "Unanchored.cpp").write_text(UNANCHORED, encoding="utf-8")
        (root / "Vehicle" / "Anchored.cpp").write_text(ANCHORED, encoding="utf-8")
        (root / "Vehicle" / "NegCallsite.cpp").write_text(NEG_CALLSITE, encoding="utf-8")
        (root / "Vehicle" / "NegAmbiguous.cpp").write_text(NEG_AMBIGUOUS, encoding="utf-8")
        (root / "Vehicle" / "NegData.cpp").write_text(NEG_DATA, encoding="utf-8")

        old_src = S.SRC
        S.SRC = root
        try:
            un = S.scan_tu("Vehicle/Unanchored.cpp")[0]
            an = S.scan_tu("Vehicle/Anchored.cpp")[0]
            nc = S.scan_tu("Vehicle/NegCallsite.cpp")[0]
            na = S.scan_tu("Vehicle/NegAmbiguous.cpp")[0]
            nd = S.scan_tu("Vehicle/NegData.cpp")[0]
        finally:
            S.SRC = old_src

    print("rva_body_scan anchoring self-test")
    check("the MISSED shape is anchored (header comment far above the definition)",
          {hex(k): v[0] for k, v in un.items()},
          {"0x46b1c0": "VehicleBuildContactHull"})
    check("the classic shape still anchors",
          {hex(k): v[0] for k, v in an.items()},
          {"0x46b1c0": "VehicleSlotAabbExpand"})
    check("BOTH bodies are seen, so the duplicate is detectable",
          sorted(set(un) & set(an)), [0x0046b1c0])
    check("NEG 1: an RVA in call prose anchors nothing", nc, {})
    check("NEG 2: one name, two RVAs -> refuse", na, {})
    check("NEG 3: a data-range RVA stays excluded", nd, {})

    print()
    if FAILED:
        print(f"FAILED: {len(FAILED)} check(s): {', '.join(FAILED)}")
        return 1
    print("OK -- 6/6")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
