"""RWC-FRAME-JITTER-STRICT: per-frame multiset compare of draw records by raw vertex bytes.

Usage: py -3.12 bitcmp.py <orig.json> <core.json> [...]
For each core file and frame pair (in key order), counts original draws whose vertex bytes for the chosen fields are
bit-identical to a core draw (multiset match). Field sets: x,y,col (offsets 0,4,16 of each 0x1c vertex),
x,y,z,rhw,col (0,4,8,12,16) and x,y,col,u,v (0,4,16,20,24).
"""
import json
import sys
from collections import Counter

FIELDS = {"x,y,col": (0, 4, 16), "x,y,z,rhw,col": (0, 4, 8, 12, 16), "x,y,col,u,v": (0, 4, 16, 20, 24)}


def key(rec, offs):
    b = bytes.fromhex(rec["v"])
    return tuple(b[i * 28 + o:i * 28 + o + 4] for i in range(len(b) // 28) for o in offs)


def main():
    a = json.load(open(sys.argv[1]))
    for path in sys.argv[2:]:
        b = json.load(open(path))
        for name, offs in FIELDS.items():
            got = []
            for (_, va), (_, vb) in zip(a.items(), b.items()):
                ca = Counter(key(x, offs) for x in va)
                cb = Counter(key(x, offs) for x in vb)
                got.append(sum((ca & cb).values()))
            print(f"{path}: {name}: bit-identical per frame {got} of {[len(v) for v in a.values()]}")


if __name__ == "__main__":
    main()
