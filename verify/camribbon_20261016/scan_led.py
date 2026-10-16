"""camribbon: offline scan of all 13 tracks' LED data (Common/LED.piz LE<Course_Id>.LED, layout per
Race/RaceCamera.cpp LoadLed: 12-byte header then (height, elev, azim) float triples, -1.0 = absent).

For every track: Course_Id (COURSE.LUA), the LED file, the count of triples, and how many of the first
N triples have elev == -1.0 (the no-LED fallback of FUN_00441820 fires on those).  N = AI node count
when given on the command line as track=N pairs, else 'all triples with a non-default entry'.

    py -3.12 scan_led.py [--nodes training=30,arctic=NN ...]
"""
import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\re\tools")
import piz_extract as P  # noqa: E402

ORIG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\TOASTART")


def entries(path):
    data, _v, _a, ents, _m = P.read_archive(path)
    return data, {n.rstrip("\0"): (o, l) for (n, o, l, _i) in ents}


def main():
    nodes = {}
    for a in sys.argv[1:]:
        if a.startswith("--nodes"):
            continue
        for kv in a.split(","):
            if "=" in kv:
                k, v = kv.split("=")
                nodes[k.lower()] = int(v)
    led_data, led_ents = entries(ORIG / "Common" / "LED.piz")
    print(f"LED.piz entries: {sorted(k for k in led_ents if k.upper().endswith('.LED'))}")
    for tp in sorted((ORIG / "TRACKS").glob("*.piz")):
        d, ents = entries(tp)
        lua = next((k for k in ents if k.upper() == "COURSE.LUA"), None)
        cid = None
        if lua:
            o, l = ents[lua]
            txt = d[o:o + l].decode("latin1")
            m = re.search(r"Course_Id\s*\(\s*(\d+)\s*\)", txt)
            cid = int(m.group(1)) if m else None
        name = tp.stem
        want = f"LE{cid}.LED" if cid is not None else None
        hit = next((k for k in led_ents if k.upper() == (want or "").upper()), None)
        if not hit:
            print(f"{name:10s} course_id={cid} led=MISSING")
            continue
        o, l = led_ents[hit]
        b = led_data[o:o + l]
        ntrip = (l - 12) // 12
        tr = [struct.unpack_from("<3f", b, 12 + k * 12) for k in range(ntrip)]
        used = [k for k, t in enumerate(tr) if t != (-1.0, -1.0, -1.0)]
        last = max(used) if used else -1
        n = nodes.get(name.lower())
        lim = n if n is not None else (last + 1)
        # file triple order = (elev, azim, height) = RaceCamera::LedEntry; the fallback tests elev
        noelev = [k for k in range(lim) if tr[k][0] == -1.0]
        print(f"{name:10s} course_id={cid} led={hit} triples={ntrip} last_nondefault={last} "
              f"nodes={n} no_elev={len(noelev)} first={noelev[:12]}")


if __name__ == "__main__":
    main()
