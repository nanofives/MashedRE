r"""screen_opcodes.py — decode the frontend screen-record opcode streams.

`PTR_DAT_005f7638` holds 34 pointers (screen ids 0x00..0x21; 0x1b is NULL) to
tagged `ff<op>0000` opcode streams. `FUN_0043dfd0` walks the CURRENT screen's
stream every tick and turns the bare opcodes into per-action enable flags -- so
which actions a screen accepts is DATA, not code.

Mapping read from the listing at 0x0043e3f2..0x0043e48f (all slots zeroed first at
0x0043e3ff..0x0043e417):

    ff11 -> [ESP+0x20]      ff0b -> [ESP+0x24]      ff0c -> [ESP+0x28]
    ff0d -> [ESP+0x2c]      ff0e -> [ESP+0x30]
    ff12 -> [ESP+0x1c]   == contcfg ACTION 6 enable  (gate 0x0043fef9)
    ff33 -> [ESP+0x34]   == contcfg ACTION 7 enable  (gate 0x004409c8)
    ff10 / ff23 -> EDX

The scan starts after an `ff09` marker (required at 0x0043e3e5, else the whole
action-dispatch block is skipped by the JNZ to 0x004409f2) and stops at `ff0a`.

Operand-carrying opcodes seen so far: ff00 = message id (the screen title),
ff02/ff03/ff04/ff08 = numeric fields. Everything else is [UNCERTAIN].

Usage:
    py -3.12 re\tools\screen_opcodes.py            # table for every screen
    py -3.12 re\tools\screen_opcodes.py --op ff33  # which screens carry an opcode
"""
import argparse
import struct
import sys
from pathlib import Path

EXE = Path(__file__).resolve().parents[2] / "original" / "MASHED.exe"
TABLE_VA = 0x005F7638
N_SCREENS = 0x22          # 0x00..0x21 inclusive

# Opcodes that carry a following dword operand.
WITH_OPERAND = {0x00, 0x02, 0x03, 0x04, 0x08}

# Bare opcodes that FUN_0043dfd0 turns into enable flags.
ENABLE = {
    0x11: "[ESP+0x20]",
    0x0B: "[ESP+0x24]",
    0x0C: "[ESP+0x28]",
    0x0D: "[ESP+0x2c]",
    0x0E: "[ESP+0x30]",
    0x12: "[ESP+0x1c]  ACTION 6",
    0x33: "[ESP+0x34]  ACTION 7",
    0x10: "EDX",
    0x23: "EDX",
}

SCAN_START = 0x09
SCAN_END = 0x0A


def load():
    d = EXE.read_bytes()
    pe = struct.unpack_from("<I", d, 0x3C)[0]
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    optsz = struct.unpack_from("<H", d, pe + 20)[0]
    base = struct.unpack_from("<I", d, pe + 24 + 28)[0]
    secs = []
    for i in range(nsec):
        o = pe + 24 + optsz + i * 40
        vsz, va, rsz, ro = struct.unpack_from("<IIII", d, o + 8)
        secs.append((va, vsz, ro, rsz))

    def v2f(va):
        r = va - base
        for sva, vsz, ro, rsz in secs:
            if sva <= r < sva + max(vsz, rsz):
                off = ro + (r - sva)
                return off if off < len(d) else None
        return None

    return d, v2f


def stream(d, v2f, ptr, limit=64):
    """Yield (opcode, operand_or_None) until ff0a or a non-tag word."""
    f = v2f(ptr)
    if f is None:
        return
    i = 0
    while i < limit:
        w = struct.unpack_from("<I", d, f + i * 4)[0]
        if (w & 0xFF00FFFF) != 0xFF000000:
            return                     # not a tag: end of record
        op = (w >> 16) & 0xFF
        if op in WITH_OPERAND:
            operand = struct.unpack_from("<I", d, f + (i + 1) * 4)[0]
            yield op, operand
            i += 2
        else:
            yield op, None
            i += 1
        if op == SCAN_END:
            return


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--op", help="only list screens containing this opcode, e.g. ff33 or 33")
    a = ap.parse_args()

    d, v2f = load()
    want = None
    if a.op:
        t = a.op.lower().replace("0x", "")
        if t.startswith("ff"):
            t = t[2:]
        want = int(t, 16)

    hits = []
    for sid in range(N_SCREENS):
        f = v2f(TABLE_VA + sid * 4)
        ptr = struct.unpack_from("<I", d, f)[0]
        if ptr == 0:
            print("  %2d 0x%02x  (NULL table entry)" % (sid, sid))
            continue
        ops = list(stream(d, v2f, ptr))
        codes = [op for op, _ in ops]
        # the enable run is what sits between ff09 and ff0a
        enables = []
        if SCAN_START in codes:
            k = codes.index(SCAN_START)
            for op in codes[k + 1:]:
                if op == SCAN_END:
                    break
                if op in ENABLE:
                    enables.append(op)
        msg = next((o for c, o in ops if c == 0x00), None)
        if want is not None:
            if want in enables:
                hits.append(sid)
            continue
        lab = ", ".join("ff%02x=%s" % (o, ENABLE[o]) for o in enables) or "(none)"
        print("  %2d 0x%02x  msg=0x%-4x  enables: %s"
              % (sid, sid, msg if msg is not None else -1, lab))

    if want is not None:
        print("screens whose enable run contains ff%02x: %s" % (want, hits))
    return 0


if __name__ == "__main__":
    sys.exit(main())
