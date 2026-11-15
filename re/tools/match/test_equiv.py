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
    # EQUIV-X87-INTEGER-MOTION (L-147 (a)): integer motion across x87, admitted only when independent
    "x87_move_indep": ("EQUIV", None),
    "x87_move_rr": ("EQUIV", None),
    "neg_x87_fnstsw": ("NOT-PROVEN", "x87-motion"),
    "neg_x87_sahf": ("NOT-PROVEN", "x87-motion"),
    "neg_x87_alias": ("NOT-PROVEN", "x87-motion"),
    "neg_x87_eflags": ("NOT-PROVEN", "x87-motion"),
    "neg_x87_call": ("NOT-PROVEN", "x87-region"),
    "x87_move_frame": ("EQUIV", "FRAME"),
    "neg_x87_slot": ("NOT-PROVEN", "x87-motion"),
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
    # TOOL-EQUIV-EBPDATA-REPSTOS: rep stos with a variable count
    "stosv": ("EQUIV", None),
    "neg_stosv_val": ("NOT-PROVEN", None),
    "neg_stosv_stack": ("NOT-PROVEN", "unsupported"),
}

# A6 rules a case must (True) or must not (False) report
RULES = {
    "com_ident": ("A6-pinned-pop", True),
    "com_stdcall": ("A6-pinned-pop", True),
    "com_cdecl": ("A6-pinned-pop", True),
    "com_loop": ("A6-pinned-pop", True),
    "com_ambiguous": ("A6-pop-solution", True),
    "x87_move_indep": ("x87-motion", True),
    "x87_move_rr": ("x87-motion", True),
    "x87_same": ("x87-motion", False),
    "x87_move_frame": ("x87-motion", True),
    "stosv": ("rep-stos-var", True),
}

# rva -> (source, expected verdict, note)
REFERENCES = {
    0x00475a00: ("re/match/pilot/a3_00475a00.c", "EQUIV", "L-115 admitted: SIB base/index order"),
    0x0045a3a0: ("re/match/s09/0045a3a0.c", "EQUIV-IF-CALLERS-IGNORE", "L-115 admitted: EAX differs at return"),
    0x00555360: ("re/tools/match/equiv_tests/refs/00555360_held.c", "EQUIV-IF-CALLERS-IGNORE", "L-117 admitted: return blocks + store order; frozen copy of the 94.4% DDK source, the live source is 100% under rw9044 since MATCH-RT2D-RESID run 40"),
    0x0048e820: ("re/tools/match/equiv_tests/refs/0048e820_held.c", "NOT-PROVEN", "L-118 admitted with the PortTop EDI wrapper; frozen copy of the pre-EQUIV-FU-48E820 source (the live one is byte-identical since 58cc241be); the "
                 "unedited source passes the wrong EDI to 0x0048cee0"),
    0x005c4ad0: ("re/tools/match/equiv_tests/refs/005c4ad0_held.c", "EQUIV", "held: imul operand order + a local whose address escapes (A1'); frozen copy, the live source is 100% since MATCH-RT2D-FONT"),
    0x00556e90: ("re/tools/match/equiv_tests/refs/00556e90_held.c", "NOT-PROVEN", "held: moves scheduled across x87 (excluded); frozen copy of the 97.5% source"),
    0x005441f0: ("re/tools/match/equiv_tests/refs/005441f0_held.c", "NOT-PROVEN", "EQUIV-5441F0-TIMEOUT: ran 50 min with no verdict "
                 "(show() exponential on a call text); now NOT-PROVEN unsupported (rep movsd with a variable count) in seconds; "
                 "frozen copy of the 99.1% rwtb source"),
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


def test_indirect_callers():
    """EQUIV-CALLER-0045AC2A: 0x0045ac20 has no direct caller; its ret hands EAX to the table call at 0x0045b9e2."""
    assert equiv.callers(0x0045ac20) == []
    assert equiv.indirect_sites(0x0045ac20) == [(0x0045b9e2, 2)]
    assert equiv.caller_reads(0x0045ac2a, "eax") == "ignores"
    saved = dict(equiv.INDIRECT_CALLERS[0x0045ac20]["routes"][0])
    try:                                                # a wrong slot is refused, not trusted
        equiv.INDIRECT_CALLERS[0x0045ac20]["routes"][0]["slot"] = 0x005f9ab8
        try:
            equiv.indirect_sites(0x0045ac20)
            raise AssertionError("bad slot accepted")
        except ValueError:
            pass
    finally:
        equiv.INDIRECT_CALLERS[0x0045ac20]["routes"][0].update(saved)


def test_route_audit():
    """EQUIV-INDIRECT-SCAN-COMPLETENESS: the byte-anchored audit of 0x0045ac20's routes holds on the image, any change
    to the registered reference lists is refused, and a disp8 cannot encode +0xa8."""
    rep = equiv.route_audit(0x0045ac20)
    assert rep == {"entry_dwords": [0x005f9abc], "table_refs": 28, "holder_refs": 15, "methods": 95}, rep
    a = equiv.INDIRECT_CALLERS[0x0045ac20]["audit"]
    cs = next(equiv._MD.disasm(bytes.fromhex("8b40a8"), 0x1000))           # mov eax, [eax-0x58]
    assert cs.operands[1].mem.disp == -0x58
    for key, drop in (("table_refs", 0x0045b9de), ("holder_refs", 0x0045bffc)):
        saved = a[key]
        try:
            a[key] = (tuple(x for x in saved if x != drop) if isinstance(saved, tuple)
                      else {k: v for k, v in saved.items() if k != drop})
            equiv._AUDIT.clear()
            try:
                equiv.route_audit(0x0045ac20)
                raise AssertionError(f"{key} without 0x{drop:08x} accepted")
            except ValueError:
                pass
        finally:
            a[key] = saved
            equiv._AUDIT.clear()
    saved = a["store_fn_branches"]
    try:                                                # a second entry into the store function is refused
        a["store_fn_branches"] = ()
        try:
            equiv.route_audit(0x0045ac20)
            raise AssertionError("store fn branch list accepted")
        except ValueError:
            pass
    finally:
        a["store_fn_branches"] = saved
        equiv._AUDIT.clear()
    assert equiv.route_audit(0x0045ac20) == rep


def test_summary_precision():
    """EQUIV-NORETURN-RECHECK: the no-return reason, r11 / r12, the constant idioms, frame alignment, A7."""
    s = equiv.summary(0x00408b00)                       # 6 rets; it calls CRT acos 0x004a3384 (r11, not no-return)
    assert not s["noreturn"], s
    a = equiv.summary(0x004a3384)
    assert not a["noreturn"] and a.get("why") == "r11", a
    al = equiv.summary(0x00498a00)                      # push ebp; mov ebp,esp; and esp,-8; ...; push esi; push edi
    assert "edi" not in al["livein"] and "ebx" in al["livein"], al
    md = equiv._MD
    for code, want in ((b"\x83\xc9\xff", True), (b"\x83\xe1\x00", True), (b"\x83\xc9\x01", False),
                       (b"\x0c\xff", True), (b"\x83\xe1\xfe", False)):
        cs = next(md.disasm(code, 0x1000))
        assert equiv.const_idiom(cs) is want, (code, cs.op_str)
    esp0 = equiv.S("esp0")
    stk = equiv.add(esp0, equiv.C(-0x64 & 0xffffffff))
    assert equiv.disjoint(equiv.C(0x00773408), 4, stk, 4)            # A7: image constant vs frame slot
    assert not equiv.disjoint(equiv.C(0x10000000), 4, stk, 4)        # outside the image: no claim
    assert not equiv.disjoint(equiv.S("eax_in"), 4, stk, 4)           # unknown pointer: no claim
    # TOOL-EQUIV-EBPDATA-REPSTOS: 0x005ae800 loads its first argument into EBP (`mov ebp, [esp+0x10]` at 0x005ae803)
    # and reads through it (`mov edi, [ebp+0x10]` at 0x005ae80b): a data EBP, not a frame. Two stack arguments.
    s = equiv.summary(0x005ae800)
    assert s["N"] == 2 and not s["n_why"], s
    md = equiv._MD
    for code, ebp, want in (("8b6c2410", None, True),           # mov ebp, [esp+0x10]
                            ("33ed", None, True),               # xor ebp, ebp
                            ("8bec", None, False),              # mov ebp, esp (frame, handled apart)
                            ("8be8", None, False),              # mov ebp, eax (may be a stack address)
                            ("8d6c2408", None, False),          # lea ebp, [esp+8]
                            ("83c504", equiv.EBP_DATA, True),   # add ebp, 4 on a data EBP
                            ("03ec", equiv.EBP_DATA, False)):   # add ebp, esp
        cs = next(md.disasm(bytes.fromhex(code), 0x1000))
        assert equiv._ebp_data_write(cs, ebp) is want, (code, cs.op_str)


def _summary_of(code, va):
    """summary() of raw bytes placed at `va` (outside the image), with the image reader and the size lookup patched."""
    img = equiv.image()
    rd, gs = img.bytes, equiv.match.ghidra_size
    def fake_bytes(v, n):
        if va <= v < va + len(code):
            return (code[v - va:] + b"\xcc" * 16)[:n]
        return rd(v, n)
    img.bytes = fake_bytes
    equiv.match.ghidra_size = lambda v: len(code) if v == va else gs(v)
    try:
        return equiv.summary(va)
    finally:
        img.bytes = rd
        equiv.match.ghidra_size = gs


def test_push_slot():
    """TOOL-EQUIV-PUSHECX-SLOT: a pushed slot whose address is taken and that is fully overwritten before any read does
    not read the pushed register (0x005ad910: push ecx at 0x005ad910, lea ecx,[esp+8] at 0x005ad927, mov [esp+0x14],0
    at 0x005ad932, call 0x005ad770 at 0x005ad93a). A read through the address first, or a partial overwrite, still
    counts the register read."""
    assert "ecx" not in equiv.summary(0x005ad910)["livein"], equiv.summary(0x005ad910)
    base = 0x7ff00000
    cases = (
        # push ecx; lea eax,[esp]; mov dword ptr [esp],0; mov eax,[eax]; pop ecx; ret
        ("51" "8d0424" "c7042400000000" "8b00" "59" "c3", False),
        # push ecx; lea eax,[esp]; mov eax,[eax] (read through the address first); pop ecx; ret
        ("51" "8d0424" "8b00" "59" "c3", True),
        # push ecx; lea eax,[esp]; mov byte ptr [esp],0 (partial); pop ecx; ret
        ("51" "8d0424" "c6042400" "59" "c3", True),
        # push ecx; lea eax,[esp]; jmp +0 (a branch before the overwrite); mov dword ptr [esp],0; pop ecx; ret
        ("51" "8d0424" "eb00" "c7042400000000" "59" "c3", True),
    )
    for i, (hx, live) in enumerate(cases):
        s = _summary_of(bytes.fromhex(hx), base + 0x100 * i)
        assert ("ecx" in s["livein"]) is live, (hx, s)


def check_show_budget():
    """EQUIV-5441F0-TIMEOUT: a term whose arguments hold terms inside a tuple used to render from depth 0 again at
    every tuple (repr -> T.__repr__), exponential on a DAG. 64 levels of a doubled tuple must render under the budget."""
    equiv.reset_terms()
    t = equiv.S("x")
    for _ in range(64):
        t = equiv.mk("phi", 32, (t, t))
    import time
    t0 = time.time()
    txt = equiv.show(t)
    dt = time.time() - t0
    small = equiv.show(equiv.add(equiv.S("eax_in"), equiv.C(4)))
    ok = dt < 30 and "<#" in txt and small == "(eax_in+0x4)"
    return ok, {"verdict": "EQUIV" if ok else "NOT-PROVEN", "kind": "show-budget",
                "detail": f"{dt:.2f} s, {len(txt)} chars, small={small}"}


def check_step_budget():
    """EQUIV-5441F0-TIMEOUT: past --max-steps the proof ends NOT-PROVEN budget with the place it stopped."""
    old = equiv.STEP_BUDGET
    equiv.STEP_BUDGET = 3
    try:
        r = run_pair("reg_call")
    finally:
        equiv.STEP_BUDGET = old
    ok = r["verdict"] == "NOT-PROVEN" and r.get("kind") == "budget" and "reached at" in r.get("detail", "")
    return ok, r


BUDGET_CHECKS = {"show_budget": check_show_budget, "step_budget": check_step_budget}


def test_budgets():
    bad = [n for n, f in BUDGET_CHECKS.items() if not f()[0]]
    assert not bad, bad


def main():
    items = [("case", n) for n in CASES] + [("ref", a) for a in REFERENCES] + [("budget", n) for n in BUDGET_CHECKS]
    fails = 0
    for i, (kind, x) in enumerate(items, 1):
        ok, r = (check_case(x) if kind == "case" else check_reference(x) if kind == "ref" else BUDGET_CHECKS[x]())
        fails += not ok
        label = f"0x{x:08x}" if kind == "ref" else x
        got = r["verdict"] + (f" [{','.join(r.get('classes', []))}]" if r["verdict"] != "NOT-PROVEN" else
                              f" {r.get('kind')}")
        print(f"[{i}/{len(items)}] {label} {'PASS' if ok else 'FAIL'}  got {got}", flush=True)
        if not ok:
            print(f"      detail: {str(r.get('detail', r.get('regs', '')))[:300]}")
    print(f"{len(items) - fails}/{len(items)} passed")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
