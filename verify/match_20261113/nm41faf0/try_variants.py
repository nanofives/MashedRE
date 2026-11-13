"""Try source variants of 0x0041faf0 under match.py (MATCH-NM-41FAF0).

usage: py -3.12 try_variants.py <variants.py>
<variants.py> defines VARIANTS = [(name, [(old, new), ...]), ...]; each pair is a literal text replacement applied
to re/match/s05/0041faf0.c. Prints one line per variant: [N/M] name score.
"""
import pathlib
import re
import runpy
import subprocess
import sys
import tempfile

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
SRC = ROOT / "re" / "match" / "s05" / "0041faf0.c"
MATCH = ROOT / "re" / "tools" / "match" / "match.py"


def score(text, show=False):
    with tempfile.TemporaryDirectory() as td:
        p = pathlib.Path(td) / "v.c"
        p.write_text(text, encoding="utf-8")
        cmd = [sys.executable, str(MATCH), str(p), "--only", "0x0041faf0"] + (["--show"] if show else [])
        out = subprocess.run(cmd, capture_output=True, text=True).stdout
    return out


def main():
    base = SRC.read_text(encoding="utf-8")
    variants = runpy.run_path(sys.argv[1])["VARIANTS"]
    show = "--show" in sys.argv
    for i, (name, reps) in enumerate(variants, 1):
        t = base
        ok = True
        for old, new in reps:
            if old not in t:
                ok = False
                break
            t = t.replace(old, new)
        if not ok:
            print(f"[{i}/{len(variants)}] {name} SKIP (pattern missing)", flush=True)
            continue
        out = score(t, show)
        m = re.search(r"_FUN_0041faf0: ([\d.]+)%", out)
        print(f"[{i}/{len(variants)}] {name} {m.group(1) if m else 'ERR ' + out[-300:]}", flush=True)
        if show:
            print(out)


if __name__ == "__main__":
    main()
