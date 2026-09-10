"""Positive-and-negative test for decomp2port.py's refusal guards.

Usage from repo root:

    py -3.12 re/tools/tests/test_decomp2port_refusals.py
        # exits 0 if every guard fires on its known-bad body AND stays quiet on
        # every known-good body, 1 otherwise.

WHY THIS FILE EXISTS
--------------------
On 2026-09-10 the HIDDEN_REG_ARG refusal was found to have been DEAD CODE since it
was written: 13 stray control bytes in decomp2port.py had replaced `\\b` with 0x08
and a `\\1` backreference with 0x01, so its entry condition demanded a literal SOH
byte and could never match. The guard had a comment, a named failure mode and a
cited RVA, and it still shipped two crashing ports
(re/analysis/decomp2port_refusal_was_disabled_20260910.md).

A refusal that never fires is indistinguishable from a refusal that always passes,
and NEGATIVE controls alone cannot tell them apart -- every "accept" case would have
passed against the broken regex too. So each guard needs a POSITIVE case: a body it
is required to reject.

The regexes are read out of decomp2port.py at run time rather than copied, so this
test cannot silently drift from the tool it checks.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
TOOL = ROOT / "re" / "tools" / "decomp2port.py"
SRC = TOOL.read_text(encoding="utf-8")

# --- guard 0: no stray control bytes at all -------------------------------------------
# This is what disabled the refusals. Check it directly rather than inferring it from
# behaviour: 0x01 SOH, 0x07 BEL, 0x08 BS, 0x0b VT, 0x0c FF have no business in a .py.
CTRL = {1: r"\a?", 7: r"\a", 8: r"\b", 11: r"\v", 12: r"\f"}


def control_byte_check():
    raw = TOOL.read_bytes()
    bad = [(i, c) for i, c in enumerate(raw) if c in CTRL]
    if bad:
        lines = sorted({raw[:i].count(10) + 1 for i, _ in bad})
        return f"{len(bad)} stray control byte(s) on line(s) {lines}"
    return None


# --- pull the live regexes out of the tool --------------------------------------------
def live_pattern(name):
    m = re.search(name + r" = re\.compile\(\s*(.*?)\)\n(?=[A-Za-z#\n])", SRC, re.S)
    if not m:
        sys.exit(f"FATAL: could not locate `{name}` in {TOOL.name}")
    return re.compile(eval("(" + m.group(1) + ")"))


IRREVERSIBLE = live_pattern("IRREVERSIBLE")

# HIDDEN_REG_ARG is inline in port_one(), so its two patterns are lifted by text search
# and asserted to be the tool's current ones.
_lv = re.search(r're\.findall\(\s*\n?\s*r"(\\b\(\(\?:p\[a-z\]\*Var.*?)",\s*body\)', SRC)
if not _lv:
    sys.exit("FATAL: could not locate the HIDDEN_REG_ARG induction-variable pattern")
# The captured text is the CONTENT of an r"..." literal, so it is already the pattern.
# Do NOT round-trip it through unicode_escape: that turns `\b` back into 0x08 and `\1`
# into 0x01 -- precisely the corruption this test exists to catch. (Got that wrong on the
# first draft of this file, which is a fair demonstration of how easy it is.)
LV = re.compile(_lv.group(1))
ZEROARG = re.compile(r"\bFUN_[0-9a-fA-F]{8}\s*\(\s*\)")


def hidden_reg_arg(body):
    for m0 in ZEROARG.finditer(body):
        for lv in set(LV.findall(body)):
            nod = re.sub(r"^[ \t]*[A-Za-z_][\w \*]*\b%s;[ \t]*$" % lv, "", body, flags=re.M)
            reads = len(re.findall(r"\b%s\b" % lv, nod))
            writes = len(re.findall(r"\b%s\s*=" % lv, body))
            conds = (len(re.findall(r"\b%s\b[^;]*[<>]" % lv, body))
                     + len(re.findall(r"[<>][^;]*\b%s\b" % lv, body)))
            if reads - writes - conds <= 1:
                return f"HIDDEN_REG_ARG:{lv} -> {m0.group(0).split('(')[0]}"
    return None


def refusals(body):
    out = []
    m = IRREVERSIBLE.search(body)
    if m:
        out.append("IRREVERSIBLE:" + m.group(1))
    h = hidden_reg_arg(body)
    if h:
        out.append(h)
    return out


# --- cases -----------------------------------------------------------------------------
# Every body here is the real Ghidra-shaped decompilation of the cited RVA, or a minimal
# control built to isolate one guard.
MUST_REFUSE = {
    # 0x005aeed0 -- auto-reset event poll. The A/B runs original-then-port on the SAME
    # handle, so the first call consumes the signal. Reproducible false ret-mismatch.
    "0x005aeed0 WaitForSingleObject": """
{
  DWORD DVar1;
  DVar1 = WaitForSingleObject((HANDLE)*param_1,0);
  return DVar1 != 0x102;
}
""",
    # 0x005b8080 -- the A/B would call it twice, i.e. double-close a handle.
    "0x005b8080 CloseHandle": """
{
  CloseHandle(*(HANDLE *)(param_1 + 4));
  return;
}
""",
    # 0x00495fe0 -- FUN_00495870 takes its device pointer in ESI (original:
    # mov esi,0x771e88 / call / add esi,0x448). Ghidra models a no-arg call plus an
    # unused counter; the port crashed inside the fix_joypad cave with ESI=0.
    "0x00495fe0 hidden ESI argument": """
{
  int iVar1;
  iVar1 = 0;
  if (0 < DAT_00772fac) {
    do {
      FUN_00495870();
      iVar1 = iVar1 + 1;
    } while (iVar1 < DAT_00772fac);
  }
  return;
}
""",
    # 0x004219c0 -- same class, 0x208 stride. The refusal's own comment was written
    # from this RVA and then never fired.
    "0x004219c0 hidden pointer argument": """
{
  undefined *puVar1;
  DAT_006403b0 = *DAT_007d3ff8;
  puVar1 = &DAT_0063fb90;
  do {
    FUN_00421720();
    puVar1 = puVar1 + 0x208;
  } while ((int)puVar1 < 0x6403b0);
  return;
}
""",
}

MUST_ACCEPT = {
    # 0x005a6e10 -- promoted C3 on 2026-09-10 (CLEAN 24/24 on two independent boots).
    # If a guard rejects this, the guard is too broad.
    "0x005a6e10 (known-good, C3)": """
{
  int iVar1;
  iVar1 = FUN_005a6d90(param_1, param_2, param_3, &param_3);
  if (param_4 != (uint *)0x0) {
    *param_4 = (uint)(iVar1 != 0);
  }
  return param_3;
}
""",
    # the induction variable IS passed to the call -> no hidden register argument.
    # This is the control that proves HIDDEN_REG_ARG is not just "any counted loop".
    "counted loop, induction var passed": """
{
  int iVar1;
  iVar1 = 0;
  do {
    FUN_00401000(iVar1);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 8);
  return;
}
""",
    "pure arithmetic, no calls": """
{
  float fVar1;
  fVar1 = param_1[0] * param_1[1] + param_1[2];
  *param_2 = fVar1;
  return;
}
""",
    # a zero-arg call with no advanced local at all -> nothing to suspect.
    "zero-arg call, no loop local": """
{
  int iVar1;
  iVar1 = DAT_006403b0;
  FUN_00421720();
  return iVar1;
}
""",
}


def main():
    fails = 0

    print("guard 0: source integrity")
    cb = control_byte_check()
    if cb:
        print(f"  [FAIL] {cb}")
        print("         a stray control byte inside an r\"...\" literal silently disables"
              " the guard it belongs to")
        fails += 1
    else:
        print("  [ok  ] no stray control bytes in decomp2port.py")

    print("\npositive cases (a guard MUST fire):")
    for name, body in MUST_REFUSE.items():
        got = refusals(body)
        if got:
            print(f"  [ok  ] {name:<36} -> {', '.join(got)}")
        else:
            print(f"  [FAIL] {name:<36} -> ACCEPTED (guard is not firing)")
            fails += 1

    print("\nnegative cases (no guard may fire):")
    for name, body in MUST_ACCEPT.items():
        got = refusals(body)
        if got:
            print(f"  [FAIL] {name:<36} -> {', '.join(got)} (guard too broad)")
            fails += 1
        else:
            print(f"  [ok  ] {name:<36} -> accepted")

    total = 1 + len(MUST_REFUSE) + len(MUST_ACCEPT)
    print(f"\n{total - fails}/{total} passed")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
