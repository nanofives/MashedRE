r"""contcfg_edit.py — read, dump and write Mashed controller-config files.

Layout is the one mapped in re/analysis/structs/contcfg_record.md, every offset
cited there to an instruction address. Record is 0x200 bytes:

    +0x000  dword   constant 6            (FUN_00498510 writes it; no reader found, U-9048)
    +0x004  0x104   device name string    (MAX_PATH-sized)
    +0x108  13 x d  binding array         (DWORDs, not bytes -- both readers scale by 4)
    +0x13c  dword   device type           0 inactive, 1 joypad, 2 keyboard
    +0x140  dword   joypad index
    +0x144  0xbc    unexamined tail       (no identified writer, U-9047; persisted anyway)

The on-disk file is a raw memcpy of the in-memory record at
DAT_007e95c0 + slot*0x200 -- no header, no version, no checksum
(FUN_004971b0 fread / FUN_00497230 fwrite, both 0x200 bytes).

For a KEYBOARD record (type 2) each binding value is a raw DIK scancode.
For a JOYPAD record (type 1) actions 0..8 are button bit indices and actions
9..12 are IGNORED -- the axis floats come from a fixed per-joypad table at
0x0077311c / 0x00773120, so which stick steers is NOT expressible here.

Usage:
  py -3.12 re\tools\contcfg_edit.py dump  original\contcfg0.bin
  py -3.12 re\tools\contcfg_edit.py defaults -o original\contcfg0.bin
  py -3.12 re\tools\contcfg_edit.py set   original\contcfg0.bin --action 4 --key SPACE
"""
import argparse
import struct
import sys
from pathlib import Path

RECORD_SIZE = 0x200
OFF_CONST = 0x000
OFF_NAME = 0x004
NAME_SIZE = 0x104
OFF_BINDINGS = 0x108
N_ACTIONS = 13
OFF_DEVTYPE = 0x13C
OFF_JOYIDX = 0x140

DEV_INACTIVE, DEV_JOYPAD, DEV_KEYBOARD = 0, 1, 2

# Action names. 0/1 from the U-0407/U-0413/U-9043 A/B capture; 2/3 from the
# powerup dispatcher (type methods +0x08 FIRE / +0x10 DEACT); 4/5 confirmed
# behaviourally 2026-08-29; 9..12 from FUN_00497310's axis cases.
# 8 = play your highest-priority pending driver voice line (U-9052 RESOLVED
# 2026-08-30: the queue's ID space is the per-character voice bank, 98 clips in
# groups [13,25,25,35] matching the template bases exactly).
# 6 = PAUSE (U-9049 RESOLVED 2026-08-30: in-race A/B with a negative control;
#     opens the pause menu "Transmission Interrupted". Its mechanism is
#     FUN_0043d2a0(0,2) = close every open panel with no menu-stack change).
# 7 = CHANGE STAT (U-9050 RESOLVED 2026-08-30: the Race Results screen, menu-stack
#     entry type 5, advertises exactly "Continue" and "Change Stat"; action 7
#     advances the stat page index DAT_0067ea08, wrap 14. NOTE the effect has not
#     yet been observed FIRING -- U-9059 -- so this label rests on image+code.)
# All 13 actions now have names.
ACTIONS = {
    0: "accelerate", 1: "brake", 2: "fire", 3: "deactivate",
    4: "select", 5: "back", 6: "pause", 7: "change-stat", 8: "voice-line",
    9: "steer-left (X-)", 10: "steer-right (X+)",
    11: "axis-Y-neg", 12: "axis-Y-pos",
}

# DIK scancodes. Only the ones needed to express the stock defaults plus a few
# obvious rebind targets -- this is not a complete DirectInput table.
DIK = {
    "ESCAPE": 0x01, "1": 0x02, "2": 0x03, "3": 0x04, "4": 0x05, "5": 0x06,
    "Q": 0x10, "W": 0x11, "E": 0x12, "R": 0x13, "T": 0x14, "Y": 0x15,
    "RETURN": 0x1C, "LCONTROL": 0x1D,
    "A": 0x1E, "S": 0x1F, "D": 0x20, "F": 0x21, "G": 0x22, "H": 0x23,
    "LSHIFT": 0x2A, "Z": 0x2C, "X": 0x2D, "C": 0x2E, "V": 0x2F, "B": 0x30,
    "SPACE": 0x39,
    "UP": 0xC8, "LEFT": 0xCB, "RIGHT": 0xCD, "DOWN": 0xD0,
}
DIK_REV = {v: k for k, v in DIK.items()}

# Stock defaults, written by FUN_00498510 (0x00498510). Keyboard entry writes
# all 13; joypad entry writes only actions 0..8.
DEFAULT_KEYBOARD = [0x1F, 0x2D, 0x1E, 0x20, 0x1C, 0x01, 0x01, 0x1E, 0x2E,
                    0xCB, 0xCD, 0xC8, 0xD0]
DEFAULT_JOYPAD = [0, 1, 2, 3, 0, 1, 4, 2, 5]


def build(devtype=DEV_KEYBOARD, bindings=None, name="", joyidx=0):
    rec = bytearray(RECORD_SIZE)
    struct.pack_into("<I", rec, OFF_CONST, 6)
    nb = name.encode("ascii", "replace")[:NAME_SIZE - 1]
    rec[OFF_NAME:OFF_NAME + len(nb)] = nb
    if bindings is None:
        bindings = (DEFAULT_KEYBOARD if devtype == DEV_KEYBOARD
                    else DEFAULT_JOYPAD + [0] * 4)
    vals = list(bindings) + [0] * (N_ACTIONS - len(bindings))
    for i in range(N_ACTIONS):
        struct.pack_into("<I", rec, OFF_BINDINGS + i * 4, vals[i] & 0xFFFFFFFF)
    struct.pack_into("<I", rec, OFF_DEVTYPE, devtype)
    struct.pack_into("<I", rec, OFF_JOYIDX, joyidx)
    return bytes(rec)


def parse(data):
    if len(data) != RECORD_SIZE:
        raise ValueError(f"expected {RECORD_SIZE} bytes, got {len(data)}")
    name = data[OFF_NAME:OFF_NAME + NAME_SIZE].split(b"\0", 1)[0]
    return {
        "const": struct.unpack_from("<I", data, OFF_CONST)[0],
        "name": name.decode("ascii", "replace"),
        "bindings": [struct.unpack_from("<I", data, OFF_BINDINGS + i * 4)[0]
                     for i in range(N_ACTIONS)],
        "devtype": struct.unpack_from("<I", data, OFF_DEVTYPE)[0],
        "joyidx": struct.unpack_from("<I", data, OFF_JOYIDX)[0],
        "tail_nonzero": any(data[0x144:RECORD_SIZE]),
    }


def show(rec):
    t = rec["devtype"]
    tname = {0: "inactive", 1: "joypad", 2: "keyboard"}.get(t, f"?({t})")
    print(f"  const +0x000 : {rec['const']}"
          + ("" if rec["const"] == 6 else "   <-- expected 6"))
    print(f"  name         : {rec['name']!r}")
    print(f"  device type  : {t} ({tname})")
    print(f"  joypad index : {rec['joyidx']}")
    print(f"  tail +0x144  : {'NONZERO' if rec['tail_nonzero'] else 'all zero'}")
    print("  bindings:")
    for i, v in enumerate(rec["bindings"]):
        if t == DEV_KEYBOARD:
            label = DIK_REV.get(v, f"DIK 0x{v:02x}")
        elif t == DEV_JOYPAD:
            label = "(ignored - axis from table)" if i >= 9 else f"button {v}"
        else:
            label = str(v)
        print(f"    {i:2d} {ACTIONS[i]:<18} = 0x{v:02x}  {label}")


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)

    d = sub.add_parser("dump", help="print a contcfg file")
    d.add_argument("path")

    g = sub.add_parser("defaults", help="write a record with the stock defaults")
    g.add_argument("-o", "--out", required=True)
    g.add_argument("--type", choices=["keyboard", "joypad"], default="keyboard")
    g.add_argument("--name", default="")

    s = sub.add_parser("set", help="rebind one action in an existing file")
    s.add_argument("path")
    s.add_argument("--action", type=int, required=True, choices=range(N_ACTIONS))
    s.add_argument("--key", help=f"DIK name, one of: {', '.join(sorted(DIK))}")
    s.add_argument("--value", type=lambda x: int(x, 0), help="raw value instead of --key")

    a = ap.parse_args()

    if a.cmd == "dump":
        rec = parse(Path(a.path).read_bytes())
        print(f"{a.path}:")
        show(rec)
        return 0

    if a.cmd == "defaults":
        t = DEV_KEYBOARD if a.type == "keyboard" else DEV_JOYPAD
        data = build(devtype=t, name=a.name)
        Path(a.out).write_bytes(data)
        print(f"wrote {a.out} ({len(data)} bytes, {a.type} defaults)")
        show(parse(data))
        return 0

    if a.cmd == "set":
        p = Path(a.path)
        data = bytearray(p.read_bytes())
        if a.key is not None:
            if a.key.upper() not in DIK:
                print(f"unknown key {a.key!r}; known: {', '.join(sorted(DIK))}",
                      file=sys.stderr)
                return 2
            val = DIK[a.key.upper()]
        elif a.value is not None:
            val = a.value
        else:
            print("need --key or --value", file=sys.stderr)
            return 2
        old = struct.unpack_from("<I", data, OFF_BINDINGS + a.action * 4)[0]
        struct.pack_into("<I", data, OFF_BINDINGS + a.action * 4, val)
        p.write_bytes(bytes(data))
        print(f"{p}: action {a.action} ({ACTIONS[a.action]}) "
              f"0x{old:02x} -> 0x{val:02x}"
              f"  [{DIK_REV.get(old, '?')} -> {DIK_REV.get(val, '?')}]")
        return 0


if __name__ == "__main__":
    sys.exit(main())
