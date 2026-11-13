"""Self-test for re/tools/match/relocheck.py, the exact data-operand compare (MATCH-EXACT-DATA, 2026-11-11).

    py -3.12 re/tools/match/test_relocheck.py            # one [N/M] line per case
    py -3.12 -m pytest re/tools/match/test_relocheck.py  # same cases under pytest

Fixtures in relocheck_tests/ are frozen copies, compiled with the recorded XP DDK compiler:
  swap_00414060.c     0x00414060 as it stood at 778b8b91d: byte-identical at 186 B (match.py 100%), but the
                      fld / fmul operands DAT_0089a37c and DAT_005cc32c are swapped. Must be BAD with named-data
                      on, and was OK before the change (named-data off), which is the hole this closes.
  control_00413fa0.c  0x00413fa0, a byte-identical source with right operands. Must be OK both ways.
"""
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import relocheck  # noqa: E402

FIX = HERE / "relocheck_tests"

# (name, fixture, named_data, expected status, substrings that must appear in the BAD detail)
CASES = [
    ("swap_strict_bad", "swap_00414060.c", True, "BAD",
     ["+0xf _DAT_005cc32c: ours 0x005cc32c vs orig 0x0089a37c", "+0x15 _DAT_0089a37c: ours 0x0089a37c vs orig 0x005cc32c"]),
    ("swap_legacy_ok", "swap_00414060.c", False, "OK", []),
    ("control_strict_ok", "control_00413fa0.c", True, "OK", []),
    ("control_legacy_ok", "control_00413fa0.c", False, "OK", []),
    # MATCH-RELOCHECK-5444A0: FUN_<va8>_site<N> is checked against the original operand at its own site
    ("site2_ok", "site2_005444a0.c", True, "OK", []),
    ("site2_wrong_target_bad", "site2_wrong_005444a0.c", True, "BAD", ["_FUN_004cc791_site2: ours 0x004cc791 vs orig 0x004cc790"]),
]


def run_case(fixture, named_data):
    res = relocheck.check(FIX / fixture, named_data=named_data)
    assert len(res) == 1, res
    return res[0]


def _one(case):
    name, fixture, nd, want, subs = case
    addr, st, bad, unch, n = run_case(fixture, nd)
    assert st == want, f"{name}: {st} != {want}: {bad}"
    joined = "; ".join(bad)
    for s in subs:
        assert s in joined, f"{name}: missing '{s}' in '{joined}'"
    if nd and fixture.startswith("swap"):
        # every named data operand is now checked: nothing DAT_-shaped is left unchecked
        assert not [u for u in unch if "DAT_" in u], unch


def test_cases():
    for c in CASES:
        _one(c)


if __name__ == "__main__":
    fails = 0
    for i, c in enumerate(CASES, 1):
        try:
            _one(c)
            print(f"[{i}/{len(CASES)}] {c[0]} PASS", flush=True)
        except AssertionError as e:
            fails += 1
            print(f"[{i}/{len(CASES)}] {c[0]} FAIL {e}", flush=True)
    sys.exit(1 if fails else 0)
