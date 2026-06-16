#!/usr/bin/env py -3.12
"""
Mashed AI path-data (.AI) parser / validator  (WS-C2 step 2).

Cracks the `AI%d.AI` members inside `TOASTART/Common/AI.piz` — the opponent-AI
race-line splines + track tile grid that the FUN_00418860 controller indexes.

Format (verified this session against MASHED.exe and the real asset bytes):
  Each AI%d.AI is a single RW-style chunk, 0x11890 bytes total:
    - 12-byte header  : [type u32 = 0x13269902][size u32 = 0x11884][version u32 = 0x1c02000a]
      (written by FUN_00423540 via FUN_004cc580(h, 0x13269902, 0x11884, 0x37002, 10);
       read   by FUN_004235b0 via FUN_004cc5e0(h, 0x13269902, &size, &ver))
    - 0x11884-byte payload = the monolithic image loaded to DAT_007f1a9c:
        off 0x00000  tile grid     128x128 int16   (0x4000 B), empty = 0xffff
        off 0x08000  sub-cell grid 0x200 tiles x 8x8 char (0x8000 B), wall = 0 or 3
        off 0x10004  RACE  line array  (0x801aa0)
        off 0x10610  INSIDE line array (0x8020ac)
        off 0x10c1c  SLOW  line array  (0x8026b8)
        off 0x11228  CHEAT line array  (0x802cc4)
      Each line array = 3 splines x 0x204 B; each spline =
        [64 x (X f32, Z f32) = 0x200 B][count u32 @ +0x200].

  Index conventions cited from the controller:
    FUN_00418560  count field at base + 0x200; stride 0x204; bank valid iff count>=4
    FUN_00416060/00415d00  tile = grid16[((iz+0x1f0>>3)*0x80)+((ix+0x1f0>>3))],
                           subcell = grid8[((sub_iz)+(tile*8))*8 + sub_ix]

NO-GUESSING: every offset/constant above is cited to its RVA in
re/analysis/ai_controller.md / ai_path_following/SESSION_END.md.

Usage:
  py -3.12 re/tools/ai_data.py validate [AI.piz]        # parse + check all members
  py -3.12 re/tools/ai_data.py dump <AI%d.AI>           # one extracted member
"""
import sys, os, struct, subprocess, tempfile, shutil

CHUNK_TYPE   = 0x13269902
PAYLOAD_SIZE = 0x11884
CHUNK_TOTAL  = 0x11890          # 12-byte header + payload
RW_VERSION   = 0x1c02000a
HEADER_LEN   = 12

# payload-relative offsets of the four line-type arrays (base 0x007f1a9c)
LINE_ARRAYS = {
    "race":   0x10004,   # 0x00801aa0
    "inside": 0x10610,   # 0x008020ac
    "slow":   0x10c1c,   # 0x008026b8
    "cheat":  0x11228,   # 0x00802cc4
}
SPLINE_STRIDE = 0x204
SPLINES_PER_TYPE = 3
MAX_POINTS = 64
COUNT_OFF = 0x200

TILE_GRID_OFF = 0x0000      # 128x128 int16
TILE_GRID_DIM = 128
SUBCELL_OFF   = 0x8000      # 0x200 tiles * 8x8 char


class AiSpline:
    __slots__ = ("count", "points")
    def __init__(self, count, points):
        self.count = count
        self.points = points   # list of (x, z) floats, len == count (clamped to MAX_POINTS)


def parse_member(raw, name="<mem>"):
    """Parse one AI%d.AI byte string. Returns dict. Raises ValueError on format break."""
    if len(raw) != CHUNK_TOTAL:
        raise ValueError(f"{name}: size {len(raw)} != 0x11890 ({CHUNK_TOTAL})")
    typ, size, ver = struct.unpack_from("<III", raw, 0)
    if typ != CHUNK_TYPE:
        raise ValueError(f"{name}: type 0x{typ:08x} != 0x{CHUNK_TYPE:08x}")
    if size != PAYLOAD_SIZE:
        raise ValueError(f"{name}: declared size 0x{size:08x} != 0x{PAYLOAD_SIZE:08x}")
    payload = raw[HEADER_LEN:]
    if len(payload) != PAYLOAD_SIZE:
        raise ValueError(f"{name}: payload {len(payload)} != 0x{PAYLOAD_SIZE:08x}")

    out = {"name": name, "type": typ, "size": size, "version": ver, "lines": {}}
    for lname, loff in LINE_ARRAYS.items():
        splines = []
        for s in range(SPLINES_PER_TYPE):
            base = loff + s * SPLINE_STRIDE
            (count,) = struct.unpack_from("<i", payload, base + COUNT_OFF)
            if count < 0 or count > MAX_POINTS:
                raise ValueError(f"{name}/{lname}[{s}]: count {count} out of [0,{MAX_POINTS}]")
            pts = []
            for i in range(count):
                x, z = struct.unpack_from("<ff", payload, base + i * 8)
                pts.append((x, z))
            splines.append(AiSpline(count, pts))
        out["lines"][lname] = splines
    # race spline 0 count gates the whole AI tick (FUN_00418860 DAT_00801ca0 > 3)
    out["race0_count"] = out["lines"]["race"][0].count
    return out


def _extract_all(piz_path):
    """Extract every member of AI.piz to a temp dir via piz_extract.py. Returns (dir, names)."""
    tool = os.path.join(os.path.dirname(__file__), "piz_extract.py")
    tmp = tempfile.mkdtemp(prefix="ai_data_")
    subprocess.run([sys.executable, tool, "extract", piz_path, "-o", tmp],
                   check=True, capture_output=True, text=True)
    names = sorted(n for n in os.listdir(tmp) if n.upper().endswith(".AI"))
    return tmp, names


def cmd_validate(piz_path):
    tmp, names = _extract_all(piz_path)
    try:
        ok = 0
        print(f"{piz_path}: {len(names)} AI%d.AI members\n")
        print(f"  {'member':<10} {'ver':>10}  race/inside/slow/cheat spline counts")
        for n in names:
            with open(os.path.join(tmp, n), "rb") as f:
                raw = f.read()
            try:
                d = parse_member(raw, n)
            except ValueError as e:
                print(f"  RED  {e}")
                continue
            counts = " | ".join(
                ",".join(str(s.count) for s in d["lines"][k])
                for k in ("race", "inside", "slow", "cheat"))
            ver_flag = "" if d["version"] == RW_VERSION else f" !ver=0x{d['version']:08x}"
            print(f"  {n:<10} 0x{d['version']:08x}  {counts}{ver_flag}")
            ok += 1
        print(f"\n{'GREEN' if ok == len(names) else 'RED'}: {ok}/{len(names)} members parsed "
              f"with exact byte consumption (0x11890 each).")
        return 0 if ok == len(names) else 1
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def cmd_dump(path):
    with open(path, "rb") as f:
        raw = f.read()
    d = parse_member(raw, os.path.basename(path))
    print(f"{d['name']}: type=0x{d['type']:08x} size=0x{d['size']:08x} version=0x{d['version']:08x}")
    for lname in ("race", "inside", "slow", "cheat"):
        for s, sp in enumerate(d["lines"][lname]):
            head = ", ".join(f"({x:.1f},{z:.1f})" for x, z in sp.points[:3])
            print(f"  {lname:<6}[{s}] count={sp.count:<3} {head}{' ...' if sp.count > 3 else ''}")
    return 0


def main(argv):
    if len(argv) < 2:
        print(__doc__); return 2
    cmd = argv[1]
    if cmd == "validate":
        piz = argv[2] if len(argv) > 2 else os.path.join(
            os.path.dirname(__file__), "..", "..", "original", "TOASTART", "Common", "AI.piz")
        return cmd_validate(os.path.abspath(piz))
    if cmd == "dump" and len(argv) > 2:
        return cmd_dump(argv[2])
    print(__doc__); return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv))
