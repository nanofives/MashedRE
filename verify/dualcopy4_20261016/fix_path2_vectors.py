"""Rewrite `path2_tests` of the registry entries whose path2 call-through could not be driven in window A (2026-10-16):
`bad argument count` BEFORE entry, the same class verify/dupinstall2_20261016/fix_path2_vectors.py fixed for 7 other keys. path1 vectors
are untouched. The new shape is the `p2bufs/p2args` branch of re/frida/verify_hook_install_template.js (path2 proves install + entry +
return only; the values are run_diff's business).

    py -3.12 fix_path2_vectors.py            # dry run
    py -3.12 fix_path2_vectors.py --apply
"""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
REG = ROOT / "re" / "frida" / "hooks_registry.py"
B = "{'p2bufs': [%s], 'p2args': [%s]}"
NEW = {
    # void(int slot, int flag): was [[0,1],[0,0],[1,1]] -> one list argument ("bad argument count")
    "timer_bit_field_set": "[" + ", ".join(B % ("", a) for a in ("0, 1", "0, 0", "1, 1")) + "]",
    # void(int slot, int* dst): was [0,1,2] -> one argument. dst = a zero-filled 0x40-byte buffer (24 bytes are written)
    "timer_slot_data_copy": "[" + ", ".join(B % ("0x40", "%d, {'buf': 0}" % s) for s in (0, 1, 2)) + "]",
}

raw = REG.read_bytes()
nl = b"\r\n" if b"\r\n" in raw else b"\n"
ls = raw.split(nl)
for key, vec in NEW.items():
    pat = re.compile(rb"^    '" + key.encode() + rb"':\s*\{")
    i = [k for k, l in enumerate(ls) if pat.match(l)]
    assert len(i) == 1, (key, i)
    i = i[0]
    j = next(k for k in range(i + 1, len(ls)) if ls[k].rstrip() in (b"    },", b"    }"))
    blk = nl.join(ls[i:j + 1])
    m = re.search(rb"'path2_tests':\s*\[", blk)
    assert m, key
    s = m.end() - 1
    depth = 0
    for e in range(s, len(blk)):
        c = blk[e:e + 1]
        depth += (c == b"[") - (c == b"]")
        if depth == 0:
            break
    old = blk[m.start():e + 1]
    new = b"'path2_tests':    " + vec.encode()
    print(key, "\n  old:", old.decode()[:120].replace("\r\n", " "), "\n  new:", new.decode()[:160])
    blk = blk[:m.start()] + new + blk[e + 1:]
    ls[i:j + 1] = blk.split(nl)
if "--apply" in sys.argv:
    REG.write_bytes(nl.join(ls))
    print("written")
