#!/usr/bin/env python3
"""rw_track_data.py — parse + validate Mashed track data formats (WS-F).

All five formats are standard RenderWare chunk streams (chunk IDs cross-checked
against re/prior_art/renderware/.../rw/rwplcore.h):

  .SPL  rwID_SPLINE          (0x0C)  water / wave paths   (F1)
  .ANM  rwID_HANIMANIMATION  (0x1B)  helicopter/copter/cameraman flight paths (F2)
  .UVA  rwID_UVANIMDICT      (0x2B)  scrolling-UV dictionary (sea/sky) (F3)
  .MTS  rwID_MATRIX          (0x0D)  instance-placement matrices (crates/lights) (F5)
  LAPDATA.LUA  (text)         lap lines / split sectors / safe-start lines (F4)

NO-GUESSING: every offset/field below is derived from the actual asset bytes and,
where a text twin exists (.MTS.TXT, LAPDATA.LUA), cross-checked against it. Fields
whose *semantics* remain unconfirmed (the SPL 32-byte sub-header, the UVA affine
matrix convention, the keyframe prevFrame offset) are flagged in the format doc
re/analysis/formats/track_anim_data.md; the parse itself is byte-exact.

Usage:
  py -3.12 re/tools/rw_track_data.py dump  <file.spl|.anm|.uva|.mts|LAPDATA.LUA>
  py -3.12 re/tools/rw_track_data.py validate            # all 13 tracks, every format
  py -3.12 re/tools/rw_track_data.py validate <Track>    # one track piz
"""
from __future__ import annotations

import struct
import sys
from dataclasses import dataclass, field
from pathlib import Path

# reuse the piz reader
sys.path.insert(0, str(Path(__file__).resolve().parent))
from piz_extract import read_archive  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
TRACKS_DIR = ROOT / "original" / "TOASTART" / "TRACKS"
TRACKS = [
    "Arctic", "City", "Egypt", "Forest", "Highway", "Neustein",
    "Storm", "SuperG", "Warzone", "dump", "rouabout", "sands", "training",
]

# RenderWare core chunk IDs (rwplcore.h MAKECHUNKID(rwVENDORID_CORE, n) == n)
rwID_STRUCT = 0x01
rwID_SPLINE = 0x0C
rwID_MATRIX = 0x0D
rwID_HANIMANIMATION = 0x1B
rwID_UVANIMDICT = 0x2B


def _chunk(buf: bytes, off: int):
    """Return (type, size, version) of the RW chunk header at off."""
    return struct.unpack_from("<III", buf, off)


# ---------------------------------------------------------------------------
# F1 .SPL — rwID_SPLINE
# ---------------------------------------------------------------------------
@dataclass
class Spline:
    num_points: int
    flag: int
    header: tuple            # 8 leading dwords (bbox/reserved; zero in Arctic waves)
    points: list             # list of (x, y, z)
    version: int = 0

    @property
    def constant_y(self):
        if not self.points:
            return None
        ys = [p[1] for p in self.points]
        return ys[0] if max(ys) - min(ys) < 1e-3 else None


def parse_spl(buf: bytes) -> Spline:
    typ, size, ver = _chunk(buf, 0)
    if typ != rwID_SPLINE:
        raise ValueError(f"not a spline chunk: type=0x{typ:x}")
    o = 12
    header = struct.unpack_from("<8I", buf, o); o += 32
    num = struct.unpack_from("<I", buf, o)[0]; o += 4
    flag = struct.unpack_from("<I", buf, o)[0]; o += 4
    pts = [struct.unpack_from("<3f", buf, o + i * 12) for i in range(num)]
    expect = 12 + 32 + 8 + num * 12
    if expect != len(buf):
        raise ValueError(f"SPL size mismatch: expect {expect} got {len(buf)}")
    return Spline(num, flag, header, pts, ver)


# ---------------------------------------------------------------------------
# F2 .ANM — rwID_HANIMANIMATION  (also the UVA inner anim, scheme-dependent)
# ---------------------------------------------------------------------------
@dataclass
class KeyFrameHAnim:
    time: float
    q: tuple                 # x, y, z, w  (unit quaternion)
    t: tuple                 # x, y, z     (translation)
    prev_off: int            # byte offset to previous keyframe (runtime linkage)


@dataclass
class HAnim:
    version: int
    scheme: int              # 1 = HAnim std keyframe
    num_frames: int
    flags: int
    duration: float
    frames: list = field(default_factory=list)


def parse_anm(buf: bytes) -> HAnim:
    typ, size, ver = _chunk(buf, 0)
    if typ != rwID_HANIMANIMATION:
        raise ValueError(f"not an HAnim chunk: type=0x{typ:x}")
    version, scheme, num, flags = struct.unpack_from("<IIII", buf, 12)
    duration = struct.unpack_from("<f", buf, 28)[0]
    if scheme != 1:
        raise ValueError(f"ANM scheme {scheme} != 1 (not a std HAnim keyframe)")
    frames = []
    o = 32
    for _ in range(num):
        vals = struct.unpack_from("<8fI", buf, o)
        frames.append(KeyFrameHAnim(vals[0], vals[1:5], vals[5:8], vals[8]))
        o += 36
    expect = 12 + 20 + num * 36
    if expect != len(buf):
        raise ValueError(f"ANM size mismatch: expect {expect} got {len(buf)}")
    return HAnim(version, scheme, num, flags, duration, frames)


# ---------------------------------------------------------------------------
# F3 .UVA — rwID_UVANIMDICT  (dict of scheme-0x1C1 UV anims)
# ---------------------------------------------------------------------------
@dataclass
class KeyFrameUV:
    time: float
    uv: tuple                # 6 floats: 2x3 affine UV transform (a,b,c,d,tx,ty)
    prev_off: int


@dataclass
class UVAnim:
    name: str
    version: int
    scheme: int
    num_frames: int
    duration: float
    frames: list = field(default_factory=list)

    def scroll_rate(self):
        """Per-axis UV translation delta / duration (units per second).

        Returns (du_dt, dv_dt). The animated translation lives in uv[4],uv[5]
        (tx,ty); rate = (last - first) / duration.
        """
        if len(self.frames) < 2 or self.duration <= 0:
            return (0.0, 0.0)
        a, b = self.frames[0].uv, self.frames[-1].uv
        return ((b[4] - a[4]) / self.duration, (b[5] - a[5]) / self.duration)


@dataclass
class UVDict:
    anims: list = field(default_factory=list)


def parse_uva(buf: bytes) -> UVDict:
    typ, size, ver = _chunk(buf, 0)
    if typ != rwID_UVANIMDICT:
        raise ValueError(f"not a UV-anim dict chunk: type=0x{typ:x}")
    st, sts, stv = _chunk(buf, 12)
    if st != rwID_STRUCT:
        raise ValueError(f"UVA missing struct chunk: 0x{st:x}")
    n = struct.unpack_from("<I", buf, 24)[0]
    o = 28
    d = UVDict()
    for _ in range(n):
        at, asz, av = _chunk(buf, o)
        body = o + 12
        version, scheme, num, _flags = struct.unpack_from("<IIII", buf, body)
        duration = struct.unpack_from("<f", buf, body + 16)[0]
        # custom data: u32 @+20, name[32] @+24, pad/nodeToUV[32] @+56, keyframes @+88
        name = buf[body + 24: body + 24 + 32].split(b"\x00", 1)[0].decode("ascii", "replace")
        kf_off = body + 88
        frames = []
        for f in range(num):
            vals = struct.unpack_from("<7fI", buf, kf_off + f * 32)
            frames.append(KeyFrameUV(vals[0], vals[1:7], vals[7]))
        d.anims.append(UVAnim(name, version, scheme, num, duration, frames))
        o = body + asz
    return d


# ---------------------------------------------------------------------------
# F5 .MTS — count + N rwID_MATRIX chunks  (instance-placement matrices)
# ---------------------------------------------------------------------------
@dataclass
class MtxInstance:
    right: tuple
    up: tuple
    at: tuple
    pos: tuple
    type: int                # rwMatrix type flag (3 = rwMATRIXTYPEORTHONORMAL)

    def euler_y_deg(self):
        """Recover the Y-axis rotation (degrees) the exporter wrote, from the
        right/at basis (the .MTS.TXT rotations are Y-only for these props)."""
        import math
        return math.degrees(math.atan2(-self.at[0], self.at[2]))

    def scale(self):
        """Per-axis basis length. type==3 props are unit (orthonormal); type==0
        props (trees/foliage) carry a uniform scale here, matching the .MTS.TXT
        'Scale:' line."""
        import math
        n = lambda a: math.sqrt(sum(x * x for x in a))     # noqa: E731
        return (n(self.right), n(self.up), n(self.at))

    def is_wellformed(self):
        import math
        s = self.scale()
        finite = all(math.isfinite(v) for v in
                     (*self.right, *self.up, *self.at, *self.pos))
        return finite and all(v > 1e-4 for v in s) and self.type in (0, 1, 2, 3)


def parse_mts(buf: bytes):
    count = struct.unpack_from("<I", buf, 0)[0]
    o = 4
    out = []
    for _ in range(count):
        ot, osz, ov = _chunk(buf, o)          # rwID_MATRIX wrapper
        it, isz, iv = _chunk(buf, o + 12)     # rwID_STRUCT
        if ot != rwID_MATRIX or it != rwID_STRUCT:
            raise ValueError(f"MTS bad record @0x{o:x}: 0x{ot:x}/0x{it:x}")
        m = struct.unpack_from("<12f", buf, o + 24)
        flag = struct.unpack_from("<I", buf, o + 24 + 48)[0]
        out.append(MtxInstance(m[0:3], m[3:6], m[6:9], m[9:12], flag))
        o += 12 + 12 + 52
    if o != len(buf):
        raise ValueError(f"MTS trailing bytes: ended 0x{o:x} of 0x{len(buf):x}")
    return out


# ---------------------------------------------------------------------------
# F4 LAPDATA.LUA — text
# ---------------------------------------------------------------------------
@dataclass
class LapData:
    lap_variations: int = 1
    lap_lines: list = field(default_factory=list)        # gate indices (terminator -1 dropped)
    safe_start_lines: list = field(default_factory=list)  # (a, b) ranges
    split_sectors: list = field(default_factory=list)     # (split_id, gate)


def parse_lapdata(text: str) -> LapData:
    import re
    # strip Lua line comments first — training/LAPDATA.LUA documents the call
    # shapes in `-- Split_Sector(Sector_Number, Lap_Poly_Number)` comments that
    # must not be parsed as data.
    text = "\n".join(line.split("--", 1)[0] for line in text.splitlines())
    ld = LapData(lap_variations=1)

    def nums(call, s):
        m = re.findall(rf"{call}\(([^)]*)\)", s)
        return [[int(x.strip()) for x in g.split(",") if x.strip() not in ("",)] for g in m]

    lv = nums("Lap_Variations", text)
    if lv:
        ld.lap_variations = lv[0][0]
    for g in nums("Lap_Line", text):
        if g and g[0] >= 0:
            ld.lap_lines.append(g[0])
    for g in nums("Safe_Start_Lines", text):
        if len(g) == 2:
            ld.safe_start_lines.append((g[0], g[1]))
    for g in nums("Split_Sector", text):
        if len(g) == 2:
            ld.split_sectors.append((g[0], g[1]))
    return ld


# ---------------------------------------------------------------------------
# dump / validate drivers
# ---------------------------------------------------------------------------
def _dump_file(path: Path):
    name = path.name.upper()
    buf = path.read_bytes()
    if name.endswith(".SPL"):
        s = parse_spl(buf)
        print(f"SPL {path.name}: {s.num_points} pts flag={s.flag} "
              f"constant_y={s.constant_y} hdr_nonzero={any(s.header)}")
        for i, p in enumerate(s.points[:6]):
            print(f"  p{i}=({p[0]:.3f},{p[1]:.3f},{p[2]:.3f})")
    elif name.endswith(".ANM"):
        a = parse_anm(buf)
        print(f"ANM {path.name}: {a.num_frames} frames dur={a.duration:.4f}s ver=0x{a.version:x}")
        for f in a.frames[:4]:
            print(f"  t={f.time:7.3f} q=({f.q[0]:.3f},{f.q[1]:.3f},{f.q[2]:.3f},{f.q[3]:.3f}) "
                  f"t=({f.t[0]:.2f},{f.t[1]:.2f},{f.t[2]:.2f})")
    elif name.endswith(".UVA"):
        d = parse_uva(buf)
        print(f"UVA {path.name}: {len(d.anims)} anims")
        for a in d.anims:
            du, dv = a.scroll_rate()
            print(f"  '{a.name}' frames={a.num_frames} dur={a.duration:.2f}s "
                  f"scroll=(du/dt={du:.4f}, dv/dt={dv:.4f})")
    elif name.endswith(".MTS"):
        ms = parse_mts(buf)
        print(f"MTS {path.name}: {len(ms)} matrices")
        for i, m in enumerate(ms[:6]):
            print(f"  [{i}] pos=({m.pos[0]:.3f},{m.pos[1]:.3f},{m.pos[2]:.3f}) "
                  f"euler_y={m.euler_y_deg():.3f} type={m.type}")
    elif name.startswith("LAPDATA"):
        ld = parse_lapdata(path.read_text(errors="replace"))
        print(f"LAPDATA {path.name}: variations={ld.lap_variations} "
              f"lap_lines={ld.lap_lines} splits={ld.split_sectors} "
              f"safe_start={ld.safe_start_lines}")
    else:
        print(f"unknown format: {path.name}")


def _validate_archive(track: str) -> tuple[int, int, list]:
    piz = TRACKS_DIR / f"{track}.piz"
    if not piz.exists():
        return 0, 0, [f"{track}: missing piz"]
    data, _ver, _ap, entries, _mode = read_archive(piz)
    files = {n.upper(): data[o:o + l] for (n, o, l, _i) in entries}
    ok = bad = 0
    msgs = []

    def check(label, fn):
        nonlocal ok, bad
        try:
            fn()
            ok += 1
        except Exception as e:           # noqa: BLE001 - report any parse failure
            bad += 1
            msgs.append(f"  {track}/{label}: FAIL {e}")

    twin_stale = []
    for name, buf in files.items():
        if name.endswith(".SPL"):
            def f(buf=buf, name=name):
                # structural parse (exact byte consumption) is the validation;
                # constant-Y is a property of the numbered WAVE ripples only —
                # RAIL/master splines are general 3D paths.
                s = parse_spl(buf)
                assert s.num_points == len(s.points)
            check(name, f)
        elif name.endswith(".ANM"):
            def f(buf=buf):
                a = parse_anm(buf)
                assert a.frames[0].time == 0.0
                assert abs(a.frames[-1].time - a.duration) < 1e-2, "last time != duration"
                for k in a.frames:
                    qn = sum(x * x for x in k.q) ** 0.5
                    assert abs(qn - 1.0) < 2e-3, f"non-unit quat |q|={qn:.4f}"
            check(name, f)
        elif name.endswith(".UVA"):
            def f(buf=buf):
                d = parse_uva(buf)
                assert d.anims and all(a.name for a in d.anims), "empty UVA name"
            check(name, f)
        elif name.endswith(".MTS"):
            def f(buf=buf, name=name):
                # The structural parse (rwID_MATRIX/rwID_STRUCT headers at every
                # nested record + exact byte consumption) is the authoritative
                # validation. The .MTS.TXT twin is a bonus semantic cross-check
                # but is STALE for many tracks (re-exported binary, old txt) —
                # so only assert positions when the counts agree.
                ms = parse_mts(buf)
                # txt-independent semantic gate: every record must be a valid
                # rwMatrix (finite, non-degenerate basis, known type flag).
                # type 3 = orthonormal (crates/lights); type 0 = scaled (foliage).
                for i, mi in enumerate(ms):
                    assert mi.is_wellformed(), f"record {i} malformed (type={mi.type})"
                # bonus: positional cross-check vs the .MTS.TXT twin (set match;
                # twin is unsynced/stale on several tracks, so divergence is
                # informational only).
                twin = files.get(name + ".TXT")
                if twin:
                    import re
                    txt = twin.decode("ascii", "replace")
                    trans = re.findall(r"Translation:\s*\[([^\]]+)\]", txt)
                    if len(trans) != len(ms):
                        twin_stale.append(
                            f"  {track}/{name}: twin-stale (bin {len(ms)} != txt {len(trans)})")
                        return
                    tv = [tuple(float(x) for x in t.split(",")) for t in trans]
                    hit = sum(1 for mi in ms if any(
                        abs(mi.pos[0] - t[0]) < 1e-2 and abs(mi.pos[2] - t[2]) < 1e-2
                        for t in tv))
                    if hit < len(ms) * 0.85:
                        twin_stale.append(
                            f"  {track}/{name}: twin-stale ({hit}/{len(ms)} pos match, count coincides)")
            check(name, f)
        elif name.startswith("LAPDATA"):
            def f(buf=buf):
                ld = parse_lapdata(buf.decode("ascii", "replace"))
                assert ld.lap_lines, "no Lap_Line"
            check(name, f)
    return ok, bad, msgs + twin_stale


def main(argv=None):
    argv = argv or sys.argv[1:]
    if not argv:
        print(__doc__)
        return 2
    cmd = argv[0]
    if cmd == "dump":
        for p in argv[1:]:
            _dump_file(Path(p))
        return 0
    if cmd == "validate":
        tracks = argv[1:] or TRACKS
        tot_ok = tot_bad = 0
        all_msgs = []
        for t in tracks:
            ok, bad, msgs = _validate_archive(t)
            tot_ok += ok
            tot_bad += bad
            all_msgs += msgs
            print(f"{t:10} parsed_ok={ok:3} fail={bad}")
        for m in all_msgs:
            print(m)
        print(f"\nTOTAL parsed_ok={tot_ok} fail={tot_bad}")
        return 1 if tot_bad else 0
    print(f"unknown command: {cmd}")
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
