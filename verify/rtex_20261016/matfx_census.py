"""Census of MatFX material effects in every shipped DFF / BSP (all .piz under original/TOASTART).

    py -3.12 matfx_census.py > evidence/matfx_census.txt

Counts materials by (effect type, DUAL src/dst pair) per file, using rwmat_dump.py's decoder.
Read-only: piz entries are decoded in memory (re/tools/piz_extract.py's reader).
"""
import collections
import importlib.util
import io
import sys
from contextlib import redirect_stdout
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "re" / "tools"))
import piz_extract  # noqa: E402

spec = importlib.util.spec_from_file_location("rwmat_dump_mod", HERE / "rwmat_dump.py")
src = (HERE / "rwmat_dump.py").read_text()
src = src[:src.index("for path in sys.argv[1:]:")]          # library part only
mod = {}
exec(compile(src, "rwmat_dump.py", "exec"), mod)

total = collections.Counter()
per_file = []
for piz in sorted((ROOT / "original" / "TOASTART").rglob("*.piz")):
    try:
        data, _v, _a, ents, _m = piz_extract.read_archive(piz)
    except Exception as e:  # noqa: BLE001
        print(f"# {piz.name} unreadable: {e}")
        continue
    entries = [(n, data[o:o + ln]) for (n, o, ln, _i) in ents if o + ln <= len(data)]
    for name, blob in entries:
        if not name.upper().endswith((".DFF", ".BSP")):
            continue
        buf = io.StringIO()
        try:
            with redirect_stdout(buf):
                mod["walk"](blob, 0, len(blob), 0, "top")
        except Exception as e:  # noqa: BLE001
            print(f"# {piz.name}:{name} decode error {e}")
            continue
        c = collections.Counter()
        for line in buf.getvalue().splitlines():
            if "ext MATFX:" not in line:
                continue
            d = eval(line.split("ext MATFX:", 1)[1].strip())   # repr of the decoder's dict
            key = f"type{d['type']}"
            for s in d["slots"]:
                if s.get("eff") == 4:
                    key += f" dual{s['src']}/{s['dst']}"
                elif s.get("eff"):
                    key += f" eff{s['eff']}"
            c[key] += 1
        if c:
            per_file.append((f"{piz.relative_to(ROOT / 'original' / 'TOASTART')}:{name}", c))
            total.update(c)
for f, c in per_file:
    print(f, dict(c))
print("TOTAL", dict(total))
