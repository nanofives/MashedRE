"""rsky: print the draw sequence of an original and a standalone record side by side, for chosen textures.

    py -3.12 verify\\rsky_20261016\\seqdump.py <orig draws.json> <stand draws.json> <tex,tex,...> [--from-tex water05]

Per draw: index, texture, triangle count, first three bb values (the world AABB inf). Used for the Forest object-BSP
order adjudication (PREREG_rsky.md).
"""
import argparse
import json


def load(f):
    return [e for e in json.load(open(f))["events"] if e.get("e") == "d"]


def seq(ev, names, label):
    print(f"--- {label}: {len(ev)} draws")
    for i, e in enumerate(ev):
        t = e.get("tex")
        if t in names:
            bb = e.get("bb")
            print(f"  {i:4d} {t:<16} tri={e.get('tri'):<4} bb={[round(x, 1) for x in bb[:3]] if bb else None}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("orig"); ap.add_argument("stand"); ap.add_argument("tex")
    a = ap.parse_args()
    names = set(a.tex.split(","))
    seq(load(a.orig), names, "ORIG")
    seq(load(a.stand), names, "STAND")


if __name__ == "__main__":
    main()
