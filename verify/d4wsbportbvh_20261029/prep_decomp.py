#!/usr/bin/env python3
"""prep_decomp.py -- two mechanical rewrites of decomp_pc.py --port text before decomp2port.py.

    py -3.12 verify/d4wsbportbvh_20261029/prep_decomp.py <in.json> <out.json>

decomp2port.py refuses 3 of the 25 D4-WSB-PORT-BVH rows on text patterns that are not what its
refusal names:

  R1  `*(code **)(E) = FUN_xxxxxxxx;`  (0x0055f800 x5, 0x00562600 x1)
      INDIRECT matches the bare word `code`. These lines are stores of a function address into a
      4-byte slot, not calls. Rewritten to `*(unsigned int *)(E) = 0xxxxxxxxxu;`: the original
      stores the original's absolute address (MASHED.exe has a fixed image base), so the port
      stores the same raw value, never the address of a port symbol.
  R2  `CONCAT22(CONCAT11(p,p),CONCAT11(p,p))`  (0x00564100 x1)
      PSEUDO_OP. Ghidra's byte replication of an undefined1 `p` into a dword. Rewritten to
      `((unsigned int)(unsigned char)(p) * 0x01010101u)`.

Each rewrite is an exact regex on the line; any other `code` or `CONCAT` text is left alone so
decomp2port.py still refuses it. Counts are printed per row and must equal the numbers above.
"""
import json
import re
import sys

R1 = re.compile(r"\*\(code \*\*\)(\([^;]*\)) = FUN_([0-9a-f]{8});")
R2 = re.compile(r"CONCAT22\(CONCAT11\((\w+),\1\),CONCAT11\(\1,\1\)\)")


def main():
    src, dst = sys.argv[1], sys.argv[2]
    d = json.load(open(src, encoding="utf-8"))
    for fn in d["functions"]:
        c = fn.get("decomp") or ""
        c, n1 = R1.subn(lambda m: f"*(unsigned int *){m.group(1)} = 0x{m.group(2)}u;", c)
        c, n2 = R2.subn(lambda m: f"((unsigned int)(unsigned char)({m.group(1)}) * 0x01010101u)", c)
        if n1 or n2:
            # the stored function addresses are no longer referenced as globals
            fn["globals"] = [g for g in fn.get("globals", []) if not (g["name"].startswith("FUN_") and g["name"] not in c)]
            print(f"{fn['entry']}  R1={n1}  R2={n2}")
        fn["decomp"] = c
    json.dump(d, open(dst, "w", encoding="utf-8"), indent=1)


if __name__ == "__main__":
    main()
