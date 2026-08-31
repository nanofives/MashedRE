# gamesave_edit.py — minimal, evidence-backed unlock editor for Mashed gamesave.bin.
#
# Scope: flips the championship-span UNLOCK columns only. It does NOT touch the
# opaque profile block, times, trophies, or any column whose meaning is not
# established. Built for the T-ARCTIC capture (re/HARNESS_BACKLOG.md / U-9059):
# to reach a non-TRAINING track on the ORIGINAL we must unlock a cup row.
#
# Layout (Save/GameSaveFormat.h, re/analysis/structs/gamesave_layout.md):
#   file size   0x24FA0, magic 0xDEADBEEF at +0
#   span        0x24A40, 13 rows x 0x30 bytes, 12 dword columns each
#   col 1 (+0x04) = challenge-cup launch gate (FrontendModeIndex(mode3)=1)
#   col 3 (+0x0c) = cup membership marker (=2 on the 4 Bronze-cup rows 0..3)
#   col 4 (+0x10) = track "known" flag (=2 on every row in the shipped save)
#   col 11 (+0x2c)= quick-battle launch gate (FrontendModeIndex(mode10)=11)
# LINKAGE (measured 2026-08-30): the shipped save's span col1/col11 signature —
# only row 0 set — is IDENTICAL to the live launch gate DAT_007f0a40 the
# race/nav-champ probe read, so editing span col1 propagates to the gate on load.
#
# SAFETY: refuses to write original/gamesave.bin (the diffing reference). Verifies
# size + magic on read, and reports exactly which bytes changed.
import argparse, struct, sys, shutil
from pathlib import Path

MAGIC = 0xDEADBEEF
SIZE  = 0x24FA0
SPAN  = 0x24A40
STRIDE= 0x30
ROWS  = 13
COL   = {"c1": 0x04, "c3": 0x0c, "c4": 0x10, "c11": 0x2c}


def load(p: Path) -> bytearray:
    b = bytearray(p.read_bytes())
    if len(b) != SIZE:
        sys.exit(f"bad size {len(b):#x} (want {SIZE:#x})")
    if struct.unpack_from("<I", b, 0)[0] != MAGIC:
        sys.exit("magic gate failed (not a written save)")
    return b


def show(b: bytearray):
    print("row | " + " ".join(f"c{c:<2}" for c in range(12)))
    for r in range(ROWS):
        base = SPAN + r * STRIDE
        cols = [struct.unpack_from("<I", b, base + c * 4)[0] for c in range(12)]
        print(f"{r:3} | " + " ".join(f"{v:<3}" for v in cols))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src", type=Path)
    ap.add_argument("-o", "--out", type=Path, help="output (required to write)")
    ap.add_argument("--rows", default="", help="comma rows to edit, e.g. 0,1,2,3")
    ap.add_argument("--set", default="", help="comma col=val, e.g. c1=1,c11=1")
    ap.add_argument("--show", action="store_true", help="print the span and exit")
    a = ap.parse_args()

    b = load(a.src)
    if a.show:
        show(b); return

    if a.out is None:
        sys.exit("refusing to run without --out (never edit in place)")
    outp = a.out.resolve()
    # Hard guard: never write the reference save.
    if outp.name == "gamesave.bin" and outp.parent.name == "original":
        sys.exit("REFUSED: --out resolves to original/gamesave.bin (the reference)")

    rows = [int(x) for x in a.rows.split(",") if x != ""]
    sets = {}
    for kv in a.set.split(","):
        if kv == "": continue
        k, v = kv.split("="); sets[k] = int(v)
    for k in sets:
        if k not in COL: sys.exit(f"unknown column {k}; known {list(COL)}")
    if not rows or not sets:
        sys.exit("need --rows and --set")

    changed = []
    for r in rows:
        if not (0 <= r < ROWS): sys.exit(f"row {r} out of range")
        base = SPAN + r * STRIDE
        for k, v in sets.items():
            off = base + COL[k]
            old = struct.unpack_from("<I", b, off)[0]
            if old != v:
                struct.pack_into("<I", b, off, v)
                changed.append((r, k, off, old, v))

    a.out.write_bytes(b)
    # round-trip: re-read and confirm only intended bytes differ from src
    src = bytearray(a.src.read_bytes())
    diff = [i for i in range(SIZE) if src[i] != b[i]]
    exp = set()
    for r, k, off, _, _ in changed:
        exp.update(range(off, off + 4))
    unexpected = [i for i in diff if i not in exp]
    print(f"wrote {a.out}  ({len(changed)} column(s) changed)")
    for r, k, off, old, v in changed:
        print(f"  row {r} {k} @{off:#x}: {old} -> {v}")
    if unexpected:
        sys.exit(f"ROUND-TRIP FAIL: {len(unexpected)} unexpected byte diffs")
    print(f"round-trip OK: exactly {len(diff)} bytes differ, all intended")


if __name__ == "__main__":
    main()
