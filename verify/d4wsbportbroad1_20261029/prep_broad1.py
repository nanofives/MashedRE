#!/usr/bin/env python3
"""prep_broad1.py -- exact-regex rewrites of decomp_pc.py --port text before decomp2port.py.

    py -3.12 verify/d4wsbportbroad1_20261029/prep_broad1.py <in.json> <out.json>

decomp2port.py refuses 6 of the 17 D4-WSB-PORT-BROAD-1 rows: 5 INDIRECT_CALL (object vtable
calls, which its idiom table does not cover) and 1 PSEUDO_OP (CONCAT22). The disassembly of every
site (MASHED.exe.unpatched, verify/d4wsbportbroad1_20261029/disasm_lin.py) shows a plain cdecl
call: arguments pushed, `call [reg+off]`, caller-side `add esp, 4*n`. So each call becomes
`L2_vcall(<fn dword>, args...)`, a __cdecl call through the raw dword (helper defined in the TU).

  V1  `(**(code **)(*P + OFF))(`  ->  `L2_vcall(*(unsigned int *)(*P + OFF), `
      0x00559c40 x3 (0x00559d0c +0x18 / add esp,8; 0x00559dd4 +0x14 / add esp,4;
      0x00559e73 +0x10 / add esp,4), 0x00559ee0 x1 (0x00559f3c +0x14 / add esp,4),
      0x0055ad30 x1 (0x0055ad74 +0x10 / add esp,4), 0x0055ade0 x1 (0x0055ae0d +0x14 / add esp,4).
  V2  0x00558cb0 only, the local function pointer `pcVar2` (vtable +0x20, `mov ebp,[eax+0x20]`
      at 0x00558cc1) and the RwGlobals slot `*puVar1` (`lea ebx,[eax+0xe8]` at 0x00558cee):
        `code *pcVar2;`                        -> `unsigned int pcVar2;`
        `pcVar2 = *(code **)(*param_1 + 0x20);` -> `pcVar2 = *(unsigned int *)(*param_1 + 0x20);`
        `(*pcVar2)(uVar3,param_2)`             -> `L2_vcall(pcVar2,uVar3,param_2)`
        `(*(code *)*puVar1)(uVar6)`            -> `L2_vcall(*puVar1,uVar6,param_2)`
      The last one adds `param_2`: the disassembly pushes param_2 (0x00558cec) and the record
      (0x00558ced), `call ebp`, then `add esp, 4` (0x00558cf6) pops only the record, `push eax`
      (0x00558cf9), `call [ebx]` (0x00558cfa), `add esp, 8` (0x00558cfc). So the slot callee is
      entered with two stack args (result, param_2), not the one Ghidra shows.
  C1  0x0055ae70 only: `CONCAT22(extraout_var,uVar2)` -> `(uint)uVar2`, and the declaration
      `undefined2 extraout_var;` dropped. The high half is whatever EDX holds after the call to
      0x00565570 (`mov dx,[edi+0x20]` at 0x0055ae91, `push edx` at 0x0055aebf). The callee
      0x00565200 masks it away (`and ecx, 0xffff` at 0x00565208) before any use, so the
      zero-extended value gives the same result.

Counts are printed per row and must equal the numbers above. Any other `code` / `CONCAT` text
is left alone, so decomp2port.py still refuses it.
"""
import json
import re
import sys

V1 = re.compile(r"\(\*\*\(code \*\*\)\((\*\w+ \+ 0x[0-9a-f]+)\)\)\(")
V2 = [
    ("code *pcVar2;", "unsigned int pcVar2;"),
    ("pcVar2 = *(code **)(*param_1 + 0x20);", "pcVar2 = *(unsigned int *)(*param_1 + 0x20);"),
    ("(*pcVar2)(uVar3,param_2)", "L2_vcall(pcVar2,uVar3,param_2)"),
    ("(*(code *)*puVar1)(uVar6)", "L2_vcall(*puVar1,uVar6,param_2)"),
]
C1 = [
    ("CONCAT22(extraout_var,uVar2)", "(uint)uVar2"),
    ("  undefined2 extraout_var;\r\n", ""),
]
EXPECT = {"0x00559c40": (3, 0, 0), "0x00559ee0": (1, 0, 0), "0x0055ad30": (1, 0, 0),
          "0x0055ade0": (1, 0, 0), "0x00558cb0": (0, 4, 0), "0x0055ae70": (0, 0, 2)}


def main():
    src, dst = sys.argv[1], sys.argv[2]
    d = json.load(open(src, encoding="utf-8"))
    bad = 0
    for fn in d["functions"]:
        c = fn.get("decomp") or ""
        c, n1 = V1.subn(lambda m: f"L2_vcall(*(unsigned int *)({m.group(1)}), ", c)
        n2 = n3 = 0
        if fn["entry"] == "0x00558cb0":
            for a, b in V2:
                if a in c:
                    c = c.replace(a, b, 1); n2 += 1
        if fn["entry"] == "0x0055ae70":
            for a, b in C1:
                if a in c:
                    c = c.replace(a, b, 1); n3 += 1
        got = (n1, n2, n3)
        if got != EXPECT.get(fn["entry"], (0, 0, 0)):
            print(f"{fn['entry']}  COUNT MISMATCH {got} expected {EXPECT.get(fn['entry'], (0, 0, 0))}")
            bad += 1
        if any(got):
            print(f"{fn['entry']}  V1={n1}  V2={n2}  C1={n3}")
        fn["decomp"] = c
    json.dump(d, open(dst, "w", encoding="utf-8"), indent=1)
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
