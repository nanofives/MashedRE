"""DDKOBJ-REGCONV-FALLBACK-7, recovery probe: can a refused unit keep its private-convention call sites and still reach
the one exe body? Replace each externalized helper's BODY (C view only) with a forwarder
`{ return __ddkfwd_<name>(<args>); }` to an external declared with the standard convention, keep the helper `static`,
compile with the recorded DDK compiler and flags, and compare every non-helper function against the plain compile
(relocations masked). Identical callers = cl kept the same private convention for the forwarder as for the real body,
so the forwarder could stand in for the helper copy in the object and `__ddkfwd_<name>` could bind to the exe body.

Prints per unit: each caller IDENTICAL / DIFFERENT, and the forwarder's live-in registers.

usage: py -3.12 verify/ddkregconv7_20261113/fwd_probe.py [src ...]
"""
from __future__ import annotations

import pathlib
import re
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import caller_cmp as C   # noqa: E402

D = C.D


def body_span(text: str, plain: str):
    """(start, end) of the `{...}` body of plain's static definition in text (first `static ... plain(` hit)."""
    m = re.search(r"(^|\n)[ \t]*static\s+[^;{(]*\b" + re.escape(plain) + r"\s*\(", text)
    if not m:
        return None
    i, depth = m.end(), 1
    while depth:
        depth += {"(": 1, ")": -1}.get(text[i], 0)
        i += 1
    j = text.index("{", i)
    k, depth = j + 1, 1
    while depth:
        depth += {"{": 1, "}": -1}.get(text[k], 0)
        k += 1
    return m, j, k


SHAPE = "plain"   # "plain": one forwarding call; "nop": an empty external call first, so the parameters live across a call


def forwarder_text(text: str, plain: str):
    d = D._definition(text, plain)
    if d is None:
        return None, "no definition"
    prefix, params = d
    names = D._param_names(params)
    if names is None:
        return None, "parameters not adaptable"
    sp = body_span(text, plain)
    if sp is None:
        return None, "no static definition"
    m, j, k = sp
    drop = D._cc_drop_tokens(text)
    ret = " ".join(t for t in prefix.split() if t not in drop).strip() or "int"
    fwd = f"__ddkfwd_{plain}"
    call = f"{fwd}({', '.join(names)})"
    lead = "__ddkfwd_nop(); " if SHAPE == "nop" else ""
    if SHAPE.startswith("keep"):
        # the parameters stay live across the forwarding call: an empty external after it reads the named ones
        keep = names if SHAPE == "keep" else [n for i, n in enumerate(names) if str(i) in SHAPE[5:].split(",")]
        kc = f"__ddkfwd_keep({', '.join(keep)});"
        body = "{ " + (f"{call}; {kc} " if ret == "void" else f"{ret} r = {call}; {kc} return r; ") + "}"
        decl = f"\n{ret} __cdecl {fwd}({params});\nvoid __cdecl __ddkfwd_keep();\n"
    else:
        body = "{ " + lead + (f"{call}; " if ret == "void" else f"return {call}; ") + "}"
        decl = f"\n{ret} __cdecl {fwd}({params});\n" + ("void __cdecl __ddkfwd_nop(void);\n" if SHAPE == "nop" else "")
    start = m.start() + (1 if m.group(1) == "\n" else 0)
    return text[:start] + decl + text[start:j] + body + text[k:], None


def main(argv):
    global SHAPE
    if argv and argv[0].startswith("--shape="):
        SHAPE, argv = argv[0].split("=", 1)[1], argv[1:]
    sel = argv or list(C.UNITS)
    for rel in sel:
        src = C.ROOT / rel
        info = D.parse_source(src)
        helpers = C.UNITS[rel]
        plain = C.compile_text(src, info["text"], info)
        t2, why = info["text"], None
        for h in helpers:
            t2, why = forwarder_text(t2, h)
            if t2 is None:
                break
        print(f"== {rel}  helpers {', '.join(helpers)}")
        if t2 is None:
            print(f"   forwarder not generated: {why}")
            continue
        fwd = C.compile_text(src, t2, info)
        fa, fb = C.funcs(plain), C.funcs(fwd)
        ok = True
        for name in sorted(set(fa) | set(fb)):
            if name in helpers or name.startswith("__ddkfwd_"):
                continue
            a, b = fa.get(name), fb.get(name)
            same = bool(a and b and a[0] == b[0])
            ok &= same
            print(f"   caller-side {name}: {'IDENTICAL' if same else 'DIFFERENT'}")
        for h in helpers:
            if h in fb:
                li = C.live_in(fb[h][0], 0)
                print(f"   forwarder {h}: {len(fb[h][0])} B, live-in "
                      + (", ".join(f"{r} ({t})" for r, _a, t in li) or "none"))
            else:
                print(f"   forwarder {h}: absent from the object (inlined)")
        print(f"   PROBE {'callers-unchanged' if ok else 'callers-changed'}")


if __name__ == "__main__":
    main(sys.argv[1:])
