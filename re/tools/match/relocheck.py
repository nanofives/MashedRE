"""Relocation-address check for a matched source (MATCH-S02, 2026-10-31).

match.py masks every relocation site, so a 100% score does not show that each global, callee or float constant
is the right one: two statement orders can store to swapped arrays and still score 100% (found on 0x0040b180).
This compiles the source as match.py does and, for every relocation inside each `// MATCH:` function, resolves
the target our object names and compares it with the operand the original holds at that offset:

  global      `MGLOBAL(T, name, 0x<addr>)` / `MGLOBALA(T, name, N, 0x<addr>)` in the source -> addr + stored addend
  function    `FUN_<va8>` (any decoration) -> 0x<va8>
  constant    a symbol of our object's own data section (`__real@...`, string literals) -> the original operand
              must point at bytes equal to ours
  same-section labels (jump tables) are already checked by match.py's byte compare and are skipped here.
Everything else (imports, CRT helpers such as __ftol, file-static data) is listed as unchecked by name.

    py -3.12 re/tools/match/relocheck.py <src.c> [...]     -> one line per function: OK / BAD with detail
"""
import pathlib, re, struct, sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match

RE_G = re.compile(r"MGLOBALA?\(\s*[^,]+?,\s*(\w+)\s*,\s*(?:[^,;\n]+,\s*)?(0x[0-9a-fA-F]+)\s*\)")
REL_DIR32, REL_REL32 = 0x06, 0x14


def undecorate(n):
    n = n.lstrip("_@")
    return n.split("@")[0]


def check(src):
    text = src.read_text()
    gl = {name: int(a, 16) for name, a in RE_G.findall(text)}
    fl = match.file_flags(text) or match.DEFAULT_FLAGS
    secs, syms = match.coff(match.compile_obj(src, fl, match.file_lang(text), match.file_toolchain(text)))
    by_index = {s["index"]: s for s in syms}
    out = []
    for addr, sym in match.parse_annotations(text):
        cands = [s for s in syms if s["sec"] > 0 and s["type"] == 0x20 and (s["name"] == sym or undecorate(s["name"]) == sym)]
        if not cands:
            out.append((addr, "NOSYM", [], [])); continue
        s = cands[0]
        sec = secs[s["sec"] - 1]
        start = s["value"]
        later = sorted(x["value"] for x in syms if x["sec"] == s["sec"] and x["type"] == 0x20 and x["value"] > start)
        end = later[0] if later else len(sec["data"])
        data = sec["data"]
        orig = match.original_bytes(addr, end - start + 8)
        bad, unchecked, n = [], [], 0
        for off, symidx, typ in sec["relocs"]:
            if not (start <= off < end):
                continue
            t = by_index.get(symidx, {})
            if t.get("sec") == s["sec"]:
                continue                                   # same-section label: byte compare covers it
            site = off - start
            stored = struct.unpack_from("<i", data, off)[0]
            if typ == REL_DIR32:
                theirs = struct.unpack_from("<I", orig, site)[0]
            elif typ == REL_REL32:
                theirs = (addr + site + 4 + struct.unpack_from("<i", orig, site)[0]) & 0xFFFFFFFF
            else:
                unchecked.append(f"{t.get('name')}(type {typ:#x})"); continue
            name = t.get("name", "?")
            u = undecorate(name)
            exp = None
            if u in gl:
                exp = gl[u] + stored
            elif re.fullmatch(r"thunk_FUN_[0-9a-fA-F]{8}", u):
                # Ghidra names a jmp thunk after its target: the original operand must be a thunk (E9 rel32)
                # that lands on that target
                n += 1
                try:
                    tb = match.original_bytes(theirs, 5)
                    ok = tb[0] == 0xE9 and ((theirs + 5 + struct.unpack_from("<i", tb, 1)[0]) & 0xFFFFFFFF) == int(u[-8:], 16)
                except Exception:
                    ok = False
                if not ok:
                    bad.append(f"+{site:#x} {name}: orig {theirs:#010x} is not a jmp thunk to 0x{u[-8:]}")
                continue
            elif re.fullmatch(r"FUN_[0-9a-fA-F]{8}", u):
                exp = int(u[-8:], 16) + (stored if typ == REL_DIR32 else 0)
            elif t.get("sec", 0) > 0:                      # our own constant / string
                cs = secs[t["sec"] - 1]
                csec = cs["data"]
                k = t["value"] + (stored if typ == REL_DIR32 else 0)
                crel = [r for r in cs.get("relocs", []) if r[2] == REL_DIR32 and k <= r[0] < k + 64]
                if crel:
                    # a const that holds DIR32 pointers (MSVC SEH scope table $T: {prev, filter, handler} per
                    # entry, MATCH-S17 2026-11-07): compare it to the next symbol of its section, with each
                    # pointer resolved. A pointer into this function's section -> addr + (label - start);
                    # FUN_<va8> / MGLOBAL -> that address. Any other target is copied from the original and
                    # listed as unchecked.
                    nxt = sorted(x["value"] for x in syms if x["sec"] == t["sec"] and x["value"] > t["value"])
                    ours = bytearray(csec[k:min(nxt[0] if nxt else len(csec), k + 64)])
                    try:
                        theirs_b = match.original_bytes(theirs, len(ours))
                    except Exception:
                        theirs_b = b"<outside image>"
                    for roff, ridx, _ in crel:
                        j = roff - k
                        if j + 4 > len(ours):
                            continue
                        rt = by_index.get(ridx, {})
                        rst = struct.unpack_from("<i", csec, roff)[0]
                        ru = undecorate(rt.get("name", "?"))
                        if rt.get("sec") == s["sec"]:
                            v = addr + (rt["value"] - start) + rst
                        elif re.fullmatch(r"FUN_[0-9a-fA-F]{8}", ru):
                            v = int(ru[-8:], 16) + rst
                        elif ru in gl:
                            v = gl[ru] + rst
                        else:
                            unchecked.append(f"{name}->{rt.get('name')}")
                            v = struct.unpack_from("<I", theirs_b, j)[0] if len(theirs_b) >= j + 4 else 0
                        struct.pack_into("<I", ours, j, v & 0xFFFFFFFF)
                    ours = bytes(ours)
                else:
                    ours = csec[k:k + 8] if k + 8 <= len(csec) else csec[k:]
                    try:
                        theirs_b = match.original_bytes(theirs, len(ours)) if ours else b""
                    except Exception:
                        theirs_b = b"<outside image>"
                n += 1
                if ours != theirs_b:
                    bad.append(f"+{site:#x} const {name}: ours {ours.hex()} vs orig@{theirs:#010x} {theirs_b.hex()}")
                continue
            if exp is None:
                unchecked.append(name); continue
            n += 1
            if (exp & 0xFFFFFFFF) != theirs:
                bad.append(f"+{site:#x} {name}: ours {exp & 0xFFFFFFFF:#010x} vs orig {theirs:#010x}")
        out.append((addr, "BAD" if bad else "OK", bad, unchecked, n))
    return out


if __name__ == "__main__":
    for a in sys.argv[1:]:
        p = pathlib.Path(a)
        try:
            res = check(p)
        except SystemExit as e:
            print(f"{p.name}\tERROR\t{str(e).splitlines()[0][:120]}"); continue
        for r in res:
            if r[1] == "NOSYM":
                print(f"0x{r[0]:08x}\tNOSYM\t{p.name}"); continue
            addr, st, bad, unch, n = r
            print(f"0x{addr:08x}\t{st}\tchecked={n}\tunchecked={len(unch)}\t{p.name}\t{'; '.join(bad)}"
                  f"{'  unchecked: ' + ','.join(sorted(set(unch))) if unch else ''}")
