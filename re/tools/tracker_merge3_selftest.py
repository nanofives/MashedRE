#!/usr/bin/env python3
"""Self-test for tracker_merge3.py: 3-way fixtures for hooks.csv and the allowlist. Prints [N/M] name PASS|FAIL."""
import os, subprocess, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__))
TOOL = os.path.join(HERE, "tracker_merge3.py")
H = "rva,name,conf\n# comment,,\n"
def rows(*r): return H + "".join(f"{a},{b},{c}\n" for a, b, c in r)
def run(kind, ours, base, theirs, eol="\n"):
    with tempfile.TemporaryDirectory() as d:
        ps = []
        for n, t in (("o", ours), ("b", base), ("t", theirs)):
            p = os.path.join(d, n); open(p, "w", newline="", encoding="utf-8").write(t.replace("\n", eol)); ps.append(p)
        r = subprocess.run([sys.executable, TOOL, kind, *ps], capture_output=True, text=True)
        return r.returncode, r.stdout.split(), open(ps[0], newline="", encoding="utf-8").read()
A, B, C, D = "0040a000", "0040b000", "0040c000", "0040d000"
base = rows((A, "a", "C2"), (B, "b", "C2"), (C, "c", "C2"))
cases = []
def case(name, ok): cases.append((name, bool(ok)))
rc, cf, out = run("hooks", rows((A, "a", "C4"), (B, "b", "C2"), (C, "c", "C2")), base, rows((A, "a", "C2"), (B, "b", "C4"), (C, "c", "C2")))
case("different rows on both sides merge", rc == 0 and out == rows((A, "a", "C4"), (B, "b", "C4"), (C, "c", "C2")))
rc, cf, out = run("hooks", rows((A, "a", "C4"), (B, "b", "C2"), (C, "c", "C2")), base, rows((A, "a", "C3"), (B, "b", "C2"), (C, "c", "C2")))
case("same row edited differently = conflict, ours untouched", rc == 1 and cf == [A] and out == rows((A, "a", "C4"), (B, "b", "C2"), (C, "c", "C2")))
rc, cf, out = run("hooks", rows((A, "a", "C4"), (B, "b", "C2"), (C, "c", "C2")), base, rows((A, "a", "C4"), (B, "b", "C2"), (C, "c", "C2")))
case("same row edited identically = keep one", rc == 0 and out == rows((A, "a", "C4"), (B, "b", "C2"), (C, "c", "C2")))
rc, cf, out = run("hooks", base, base, rows((A, "a", "C2"), (B, "b", "C2"), (C, "c", "C2"), (D, "d", "C1")))
case("row added by theirs lands at the end", rc == 0 and out == rows((A, "a", "C2"), (B, "b", "C2"), (C, "c", "C2"), (D, "d", "C1")))
rc, cf, out = run("hooks", rows((A, "a", "C2"), (B, "b", "C2"), (C, "c", "C2"), (D, "d", "C1")), base, rows((A, "a", "C2"), (B, "b", "C2"), ("0040b800", "x", "C1"), (C, "c", "C2")))
case("rows added by both sides, theirs placed after its predecessor", rc == 0 and out == rows((A, "a", "C2"), (B, "b", "C2"), ("0040b800", "x", "C1"), (C, "c", "C2"), (D, "d", "C1")))
rc, cf, out = run("hooks", rows((A, "a", "C2"), (C, "c", "C2")), base, rows((A, "a", "C2"), (B, "b", "C2"), (C, "c", "C4")))
case("ours deleted B, theirs edited C = both stand", rc == 0 and out == rows((A, "a", "C2"), (C, "c", "C4")))
rc, cf, out = run("hooks", rows((A, "a", "C2"), (B, "b2", "C2"), (C, "c", "C2")), base, rows((A, "a", "C2"), (C, "c", "C2")))
case("ours edited B, theirs deleted B = conflict", rc == 1 and cf == [B])
rc, cf, out = run("hooks", rows((A, "a", "C4"), (B, "b", "C2"), (C, "c", "C2")), base, rows((A, "a", "C2"), (B, "b", "C4"), (C, "c", "C2")), eol="\r\n")
case("CRLF preserved", rc == 0 and "\r\n" in out and "\n" not in out.replace("\r\n", ""))
rc, cf, out = run("hooks", H.replace("conf", "CONF") + "".join(f"{a},{b},{c}\n" for a, b, c in [(A, "a", "C2")]), rows((A, "a", "C2")), H.replace("conf", "cf") + "".join(f"{a},{b},{c}\n" for a, b, c in [(A, "a", "C2")]))
case("header changed on both sides = <unkeyed> conflict", rc == 1 and cf == ["<unkeyed>"])
AH = "# list\n"
ao = AH + "X:0040a000:f.cpp  # why\nX:0040b000:g.cpp\n"
ab = AH + "X:0040a000:f.cpp  # why\nX:0040b000:g.cpp\nX:0040c000:h.cpp\n"
at = AH + "X:0040a000:f.cpp  # why\nX:0040b000:g.cpp\nX:0040c000:h.cpp\nX:0040d000:i.cpp\n"
rc, cf, out = run("allow", ao, ab, at)
case("allowlist: ours removed C, theirs added D = both stand", rc == 0 and out == AH + "X:0040a000:f.cpp  # why\nX:0040b000:g.cpp\nX:0040d000:i.cpp\n")
rc, cf, out = run("allow", AH + "X:0040a000:f.cpp  # why2\nX:0040b000:g.cpp\nX:0040c000:h.cpp\n", ab, AH + "X:0040a000:f.cpp  # why3\nX:0040b000:g.cpp\nX:0040c000:h.cpp\n")
case("allowlist: one entry's comment edited on both sides = conflict", rc == 1 and cf == ["X:0040a000:f.cpp"])
bad = [n for n, ok in cases if not ok]
for i, (n, ok) in enumerate(cases, 1): print(f"[{i}/{len(cases)}] {n} {'PASS' if ok else 'FAIL'}")
sys.exit(1 if bad else 0)
