"""txdscope: for every track piz, which texture names do its DFF / BSP streams reference, and in which dictionary do they live?

    py -3.12 verify\\txdscope_20261016\\scope_census.py [--out census.json]

Names are read from the rwID_TEXTURE chunks (id 6: STRUCT(4B filter) + STRING name) that sit inside the material lists of every
.DFF / .BSP entry, and from each .TXD's own texture chunks (same shape), case-insensitively. Per track:
  named      the dictionary COURSE.LUA names (Texture_Dictionary_Filename), the only one the original binds (U-9724)
  other      every other TXD of the piz
  only_other referenced names found in an `other` dictionary and NOT in `named` -> these resolved before the scoped lookup and do not now
  nowhere    referenced names found in no dictionary of the piz (untextured in both the base and the scoped build)
The scan is a byte pattern scan (same shape as the TXD reader), not a full RW parse: a name is counted once per entry that references it.
"""
import argparse
import json
import re
import struct
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\re\tools")))
import piz_extract as pz  # noqa: E402

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\TOASTART\TRACKS")
VERS = (0x1803FFFF, 0x1C02000A)


def tex_names(buf):
    out = []
    i = 0
    n = len(buf)
    while True:
        i = buf.find(b"\x06\x00\x00\x00", i)
        if i < 0 or i + 44 > n:
            break
        size, ver = struct.unpack_from("<II", buf, i + 4)
        if ver in VERS and buf[i + 12:i + 16] == b"\x01\x00\x00\x00" and buf[i + 28:i + 32] == b"\x02\x00\x00\x00":
            ssz = struct.unpack_from("<I", buf, i + 32)[0]
            if 0 < ssz <= 64:
                nm = buf[i + 40:i + 40 + ssz].split(b"\0")[0].decode("latin1")
                if nm:
                    out.append(nm)
        i += 1
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default="")
    a = ap.parse_args()
    res = {}
    for p in sorted(ROOT.glob("*.piz")):
        data, ver, ap2, entries, mode = pz.read_archive(p)
        ent = {n: data[o:o + l] for (n, o, l, i) in entries}
        lua = next((v for k, v in ent.items() if k.upper().startswith("COURSE") and k.upper().endswith(".LUA")), b"").decode("latin1")
        named = None
        for m in re.finditer(r'Texture_Dictionary_Filename\s*\(\s*"([^"]*)"', lua, re.I):
            named = m.group(1)
        if named and "." not in named:
            named += ".txd"
        txds = {k: {x.lower() for x in tex_names(v)} for k, v in ent.items() if k.upper().endswith(".TXD")}
        nkey = next((k for k in txds if named and k.lower() == named.lower()), None)
        in_named = txds.get(nkey, set())
        in_other = set().union(*[v for k, v in txds.items() if k != nkey]) if len(txds) > 1 else set()
        refs = defaultdict(set)
        for k, v in ent.items():
            if k.upper().endswith((".DFF", ".BSP")):
                for x in tex_names(v):
                    refs[x.lower()].add(k)
        only_other = sorted(x for x in refs if x not in in_named and x in in_other)
        nowhere = sorted(x for x in refs if x not in in_named and x not in in_other)
        res[p.name] = {
            "named": named, "named_entry": nkey, "txds": {k: len(v) for k, v in txds.items()},
            "referenced": len(refs), "resolved_in_named": sum(1 for x in refs if x in in_named),
            "only_other": {x: sorted(refs[x]) for x in only_other}, "nowhere": {x: sorted(refs[x]) for x in nowhere},
        }
        r = res[p.name]
        print(f"{p.name:14s} named={named!s:12s} txds={r['txds']} refs={r['referenced']} in_named={r['resolved_in_named']} "
              f"only_other={len(only_other)} nowhere={len(nowhere)}")
        for x in only_other:
            print(f"     only in a non-named TXD: {x}  <- {sorted(refs[x])}")
    if a.out:
        Path(a.out).write_text(json.dumps(res, indent=1))


if __name__ == "__main__":
    main()
