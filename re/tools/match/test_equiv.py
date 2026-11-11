"""Tests for re/tools/match/equiv.py (EQUIV-CHECK).

    py -3.12 re/tools/match/test_equiv.py            # prints one [N/M] line per case
    py -3.12 -m pytest re/tools/match/test_equiv.py  # same cases under pytest

Synthetic pairs: re/tools/match/equiv_tests/cases.asm, assembled with MSVC ml.exe; <name>_a plays ours,
<name>_b plays the original. Reference rows: the four admitted near-match bodies (L-115 / L-117 / L-118) and the
two held ones, through the real DDK compile of their re/match source.
"""
import os
import pathlib
import subprocess
import sys
import tempfile

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import importlib, os  # noqa: E402
equiv = importlib.import_module(os.environ.get("EQUIV_MODULE", "equiv"))  # noqa: E402

ML = pathlib.Path(r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.44.35207\bin\Hostx86\x86\ml.exe")
ROOT = HERE.parents[2]
FAKE = 0x00400000          # address the synthetic pairs pretend to replace (no callers, never called)

# name -> (verdict, required class or NOT-PROVEN kind, or IF regs)
CASES = {
    "enc_movrr": ("EQUIV", None),
    "enc_imm": ("EQUIV", None),
    "enc_jmp": ("EQUIV", None),
    "enc_nop": ("EQUIV", None),
    "neg_enc_imm": ("NOT-PROVEN", "ret-memory"),
    "sib": ("EQUIV", "SIB"),
    "neg_sib_scale": ("NOT-PROVEN", "ret-memory"),
    "reg_swap": ("EQUIV", "REG"),
    "reg_call": ("EQUIV", "REG"),
    "neg_reg_ret": ("EQUIV-IF-CALLERS-IGNORE", ("eax", "ecx")),
    "neg_reg_regarg": ("NOT-PROVEN", "call-regargs"),
    "neg_reg_nosave": ("NOT-PROVEN", "ret-callee-saved"),
    "neg_reg_one": ("NOT-PROVEN", "ret-memory"),
    "order_st": ("EQUIV", "ORDER"),
    "order_alu": ("EQUIV", "ORDER"),
    "order_ldst": ("EQUIV", None),
    "neg_order_alias": ("NOT-PROVEN", "ret-memory"),
    "neg_order_overlap": ("NOT-PROVEN", "ret-memory"),
    "neg_order_ldst": ("NOT-PROVEN", "ret-memory"),
    "neg_order_flags": ("NOT-PROVEN", "branch-condition"),
    "layout_pol": ("EQUIV", "LAYOUT"),
    "layout_dup": ("EQUIV", "LAYOUT"),
    "neg_layout": ("NOT-PROVEN", "ret-memory"),
    "comm_add": ("EQUIV", "COMM"),
    "comm_imul": ("EQUIV", "COMM"),
    "neg_comm_sub": ("NOT-PROVEN", "ret-memory"),
    "frame_reload": ("EQUIV", "FRAME"),
    "neg_frame_escape": ("NOT-PROVEN", None),
    "x87_same": ("EQUIV", None),
    "neg_x87_comm": ("NOT-PROVEN", "x87-region"),
    "neg_x87_sched": ("NOT-PROVEN", "x87-region"),
    "neg_call_arg": ("NOT-PROVEN", "call-args"),
    "neg_call_order": ("NOT-PROVEN", "call-args"),
    "neg_call_storeorder": ("NOT-PROVEN", "call-memory"),
    "loop_reg": ("EQUIV", "REG"),
    "neg_loop_bound": ("NOT-PROVEN", None),
    "loop_ptr": ("EQUIV", None),
    "neg_loop_stride": ("NOT-PROVEN", None),
    # A6, calls through a pointer (EQUIV-CALLEE-POPS)
    "com_ident": ("EQUIV", None),
    "com_stdcall": ("EQUIV", None),
    "com_cdecl": ("EQUIV", None),
    "com_loop": ("EQUIV", None),
    "mxcsr": ("EQUIV-IF-CALLERS-IGNORE", ("edx",)),
    "neg_com_argcount": ("NOT-PROVEN", "call-args"),
    "neg_com_order": ("NOT-PROVEN", "call-target"),
    "neg_com_conv": ("NOT-PROVEN", "ret-esp"),
    "com_ambiguous": ("EQUIV", None),
    "neg_com_solution": ("NOT-PROVEN", None),
    "neg_mxcsr": ("NOT-PROVEN", None),
}

# A6 rules a case must (True) or must not (False) report
RULES = {
    "com_ident": ("A6-pinned-pop", True),
    "com_stdcall": ("A6-pinned-pop", True),
    "com_cdecl": ("A6-pinned-pop", True),
    "com_loop": ("A6-pinned-pop", True),
    "com_ambiguous": ("A6-pop-solution", True),
}

# rva -> (source, expected verdict, note)
REFERENCES = {
    0x00475a00: ("re/match/pilot/a3_00475a00.c", "EQUIV", "L-115 admitted: SIB base/index order"),
    0x0045a3a0: ("re/match/s09/0045a3a0.c", "EQUIV-IF-CALLERS-IGNORE", "L-115 admitted: EAX differs at return"),
    0x00555360: ("re/match/s24/00555360.c", "EQUIV-IF-CALLERS-IGNORE", "L-117 admitted: return blocks + store order"),
    0x0048e820: ("re/match/s15/0048e820.c", "NOT-PROVEN", "L-118 admitted with the PortTop EDI wrapper; the "
                 "unedited source passes the wrong EDI to 0x0048cee0"),
    0x005c4ad0: ("re/tools/match/equiv_tests/refs/005c4ad0_held.c", "EQUIV", "held: imul operand order + a local whose address escapes (A1'); frozen copy, the live source is 100% since MATCH-RT2D-FONT"),
    0x00556e90: ("re/tools/match/equiv_tests/refs/00556e90_held.c", "NOT-PROVEN", "held: moves scheduled across x87 (excluded); frozen copy of the 97.5% source"),
}

_OBJ = None


def cases_obj():
    global _OBJ
    if _OBJ is None:
        tmp = pathlib.Path(tempfile.mkdtemp(prefix="eqt_"))
        out = tmp / "cases.obj"
        r = subprocess.run([str(ML), "/nologo", "/c", "/coff", f"/Fo{out}", str(HERE / "equiv_tests" / "cases.asm")],
                           capture_output=True, text=True, cwd=str(tmp))
        if r.returncode != 0:
            raise RuntimeError(r.stdout + r.stderr)
        _OBJ = equiv.coff_full(out)
    return _OBJ


def run_pair(name, secs=None):
    secs0, syms = cases_obj()
    secs = secs or secs0
    equiv.reset_terms()
    res = equiv.Resolver("", secs, syms, FAKE)
    A = equiv.ObjSide("ours", secs, syms, name + "_a", FAKE, res)
    B = equiv.ObjSide("orig", secs, syms, name + "_b", FAKE, res)
    try:
        return equiv.check_sides(A, B, res, FAKE)
    except equiv.NotProven as e:
        return {"verdict": "NOT-PROVEN", "kind": e.kind, "detail": e.detail}


def check_case(name):
    want, extra = CASES[name]
    r = run_pair(name)
    ok = r["verdict"] == want
    if ok and want == "EQUIV" and extra:
        ok = extra in r.get("classes", [])
    if ok and want == "NOT-PROVEN" and extra:
        ok = r.get("kind") == extra
    if ok and want == "EQUIV-IF-CALLERS-IGNORE" and extra:
        ok = tuple(sorted(r.get("regs", []))) == tuple(sorted(extra))
    if ok and name in RULES and r["verdict"] != "NOT-PROVEN":
        rule, must = RULES[name]
        ok = (rule in r.get("rules", [])) == must
    return ok, r


def check_reference(rva):
    src, want, _ = REFERENCES[rva]
    rs = equiv.check_source(ROOT / src, {rva})
    r = rs[0]
    return r["verdict"] == want, r


def test_cases():
    bad = [n for n in CASES if not check_case(n)[0]]
    assert not bad, bad


def test_references():
    bad = [hex(a) for a in REFERENCES if not check_reference(a)[0]]
    assert not bad, bad


def main():
    items = [("case", n) for n in CASES] + [("ref", a) for a in REFERENCES]
    fails = 0
    for i, (kind, x) in enumerate(items, 1):
        ok, r = check_case(x) if kind == "case" else check_reference(x)
        fails += not ok
        label = x if kind == "case" else f"0x{x:08x}"
        got = r["verdict"] + (f" [{','.join(r.get('classes', []))}]" if r["verdict"] != "NOT-PROVEN" else
                              f" {r.get('kind')}")
        print(f"[{i}/{len(items)}] {label} {'PASS' if ok else 'FAIL'}  got {got}", flush=True)
        if not ok:
            print(f"      detail: {str(r.get('detail', r.get('regs', '')))[:300]}")
    print(f"{len(items) - fails}/{len(items)} passed")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
