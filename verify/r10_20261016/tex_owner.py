"""r10: which COURSE.LUA asset owns each RED texture? (static, from the track piz bytes only)

    py -3.12 tex_owner.py [--prefix diff0_]

For every (track, RED texture) of the scored frames: the DFF clumps (Clump_Filename idx, flagged by
Clump_Exclude_From_World or not, paired by Clump_Mts_Pair or not) and the BSP files (World / Object / Collision / AI)
of the track piz whose bytes contain the texture name as a NUL-delimited ASCII string (RenderWare stores material
texture names as String chunks). A string match means "the asset references a texture of that name", nothing more.
"""
import argparse, glob, json, re, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent.parent / "re" / "tools"))
import piz_extract as pe

BASE = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\TOASTART\TRACKS")
PIZ = {"Egypt": "Egypt", "Neustein": "Neustein", "Highway": "Highway", "Sands": "sands", "SuperG": "SuperG",
       "Roundabout": "rouabout", "Storm": "Storm", "Forest": "Forest", "Dump": "dump", "Warzone": "Warzone"}


def load(track):
    data, ver, app, ents, mode = pe.read_archive(BASE / f"{PIZ[track]}.piz")
    return {n.lower(): data[o:o + l] for (n, o, l, i) in ents}


def lua_info(files):
    txt = files["course.lua"].decode("latin1").replace("\r", "")
    clumps = {int(m.group(1)): m.group(2) for m in re.finditer(r'Clump_Filename\(\s*(\d+)\s*,\s*"([^"]+)"', txt)}
    excl = {int(m.group(1)) for m in re.finditer(r'^\s*Clump_Exclude_From_World\(\s*(\d+)\s*\)', txt, re.M)}
    pairs = {int(m.group(1)): int(m.group(2)) for m in re.finditer(r'^\s*Clump_Mts_Pair\(\s*(\d+)\s*,\s*(\d+)\s*\)', txt, re.M)}
    bsps = re.findall(r'^\s*(World|Object|Collision|AI)_Bsp_Filename\(\s*(?:\d+\s*,\s*)?"([^"]+)"', txt, re.M)
    rwp = [(int(m.group(1)), m.group(2), m.group(3)) for m in re.finditer(r'^\s*RWP_Object\(\s*(\d+)\s*,\s*"([^"]*)"\s*,\s*"([^"]+)"', txt, re.M)]
    return clumps, excl, pairs, bsps, rwp


def has(blob, name):
    return re.search(rb"(?<![\x20-\x7e])" + re.escape(name.encode("latin1")) + rb"\x00", blob, re.I) is not None


def main():
    ap = argparse.ArgumentParser(); ap.add_argument("--prefix", default="diff0_"); a = ap.parse_args()
    red = {}
    for f in sorted(glob.glob(str(HERE / "[0-9][0-9]_*" / f"{a.prefix}[0-9]*.json"))):
        trk = Path(f).parent.name.split("_", 1)[1]
        for r in json.loads(Path(f).read_text())["rows"]:
            if "cite" not in r and r["tex"] not in ("-", "*") and not r["tex"].startswith("#"):
                red.setdefault(trk, set()).add(r["tex"])
    out = {}
    for trk, texs in sorted(red.items()):
        files = load(trk)
        clumps, excl, pairs, bsps, rwp = lua_info(files)
        print(f"=== {trk}: clumps {len(clumps)}, excluded {len(excl)}, Mts pairs {pairs}, bsp {bsps}, RWP objects {len(rwp)}")
        for t in sorted(texs):
            own = []
            for idx, nm in sorted(clumps.items()):
                b = files.get(nm.lower())
                if b and has(b, t):
                    own.append(f"clump{idx}:{nm}{'[EXCL]' if idx in excl else ''}{'[PAIR]' if idx in pairs else ''}")
            for kind, nm in bsps:
                b = files.get(nm.lower())
                if b and has(b, t):
                    own.append(f"{kind}Bsp:{nm}")
            for idx, lab, nm in rwp:
                b = files.get(nm.lower())
                if b and has(b, t):
                    own.append(f"RWP{idx}:{nm}")
            out[f"{trk}/{t}"] = own
            print(f"  {t:<18} {' | '.join(own) if own else '(no asset of the piz names it)'}")
    (HERE / f"tex_owner_{a.prefix.strip('_')}.json").write_text(json.dumps(out, indent=1))


if __name__ == "__main__":
    main()
