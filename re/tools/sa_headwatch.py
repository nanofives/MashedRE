"""U-9185 item (b) discriminator: which port-side heading candidate carries the residual.

Pre-registration: verify/d3_heading_20261004/PREREG_HEADING.md. READ-ONLY on the game.

Spawns the STANDALONE mashed_re.exe the way re/tools/sa_capture.py does and polls with
ReadProcessMemory -- no injection, no Frida, no game-code change, no build.

Per sample and per vehicle slot it reads BOTH headings the port carries:
  bridge  g_aib.fwd[v]                 = (cos(a.yaw), sin(a.yaw))  TrackRenderer.cpp:3729
  record  g_records[v] + 0x9d4/+0x9dc  = the record's forward row   VehiclePhysicsRun.cpp:1002-1003
and reports the angle between them. off::kForward == 0x9d4 (VehicleStruct.h:105) is the
same field the ORIGINAL's FUN_0046d510 returns, so this is the bridge-vs-record question
with the original held out of it.

ADDRESSES ARE RESOLVED FROM THE LINKER MAP, never hardcoded: sa_boostwatch.py's constant
G_RECORDS = 0x00c09b88 is already stale against today's map (0x00c09b90), and a stale base
would print a quiet wrong answer.

H-JITTER: the sampler is a poll, so bridge and record are read microseconds apart rather
than atomically. Every sample therefore re-reads g_aib.pos[v] AFTER the record read and
reports |pos_before - pos_after|. If that bound is not at least 10x smaller than the angle
difference being claimed, the run is VOID (memory next-sample-pairing-needs-a-frame-marker).

Usage:
  py -3.12 re/tools/sa_headwatch.py <out_prefix> <seconds> [ENV=VAL ...]
            [--hz 30] [--map mashedmod/build/mashed_re.map]

Writes <out_prefix>.head.csv (every sample) and <out_prefix>.head.json (the gate summary).
The process is spawned and killed BY PID; no blanket kill by name.
"""
import ctypes, json, math, os, re, struct, subprocess, sys, time
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
EXE = ROOT / "mashedmod" / "build" / "mashed_re.exe"
MAP = ROOT / "mashedmod" / "build" / "mashed_re.map"

REC_STRIDE = 0xd04        # VehiclePhysicsRun.cpp:109
OFF_FWD_X  = 0x9d4        # off::kForward      VehicleStruct.h:105
OFF_FWD_Z  = 0x9dc        # kForward + 8
OFF_SPEED  = 0x9e4        # the (e) statistic's field, used as an H-BASE channel
AIB_FWD    = 64           # AiBridgeState: pos[4][2] +0, vel[4][2] +32, fwd[4][2] +64
AIB_POS    = 0
NSLOT      = 4

PROCESS_VM_READ = 0x0010
PROCESS_QUERY_INFORMATION = 0x0400
k32 = ctypes.WinDLL("kernel32", use_last_error=True)
k32.OpenProcess.restype = wintypes.HANDLE
k32.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p,
                                  ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]


def read(h, addr, n):
    buf = (ctypes.c_char * n)()
    got = ctypes.c_size_t(0)
    if not k32.ReadProcessMemory(h, ctypes.c_void_p(addr), buf, n, ctypes.byref(got)):
        return None
    return bytes(buf) if got.value == n else None


def f32(h, a):
    b = read(h, a, 4)
    return None if b is None else struct.unpack("<f", b)[0]


def map_lookup(path, *needles):
    """Rva+Base of the first map symbol whose name contains EVERY needle."""
    pat = re.compile(r"^\s[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s", re.I)
    for ln in open(path, encoding="utf-8", errors="replace"):
        m = pat.match(ln)
        if m and all(n in m.group(1) for n in needles):
            return int(m.group(2), 16)
    return None


def main():
    argv = sys.argv[1:]
    hz, mapfile = 30.0, str(MAP)
    if "--hz" in argv:
        i = argv.index("--hz"); hz = float(argv[i + 1]); del argv[i:i + 2]
    if "--map" in argv:
        i = argv.index("--map"); mapfile = argv[i + 1]; del argv[i:i + 2]
    prefix, secs = argv[0], float(argv[1])

    # Vehicle's g_records, not Frontend's MenuRecord array of the same name.
    g_rec = map_lookup(mapfile, "g_records@?A", "@Vehicle@mashed_re@@3PAEA")
    g_aib = map_lookup(mapfile, "g_aib@?A", "@D3d9Render@mashed_re@")
    if not g_rec or not g_aib:
        sys.exit("map lookup failed: g_records=%s g_aib=%s" % (g_rec, g_aib))
    print("[addr] g_records=0x%08x  g_aib=0x%08x  (from %s)" % (g_rec, g_aib, mapfile))

    env = dict(os.environ)
    env.setdefault("MASHED_WIN_POS", "primary-bl")
    for kv in argv[2:]:
        k, _, v = kv.partition("=")
        env[k] = v

    proc = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print("[pid] %d  (spawned and killed BY PID)" % proc.pid)
    h = k32.OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, False, proc.pid)
    if not h:
        proc.kill(); sys.exit("OpenProcess failed: %d" % ctypes.get_last_error())

    rows = []
    t0 = time.time()
    try:
        while time.time() - t0 < secs:
            r = {"t": round(time.time() - t0, 3)}
            ok = True
            for v in range(NSLOT):
                ab = g_aib + AIB_FWD + v * 8
                ap = g_aib + AIB_POS + v * 8
                rb = g_rec + v * REC_STRIDE
                px0, pz0 = f32(h, ap), f32(h, ap + 4)          # pos BEFORE
                bx, bz = f32(h, ab), f32(h, ab + 4)            # bridge fwd
                rx, rz = f32(h, rb + OFF_FWD_X), f32(h, rb + OFF_FWD_Z)   # record fwd
                sp = f32(h, rb + OFF_SPEED)
                px1, pz1 = f32(h, ap), f32(h, ap + 4)          # pos AFTER  (H-JITTER)
                if None in (px0, pz0, bx, bz, rx, rz, sp, px1, pz1):
                    ok = False; break
                r["b_x%d" % v], r["b_z%d" % v] = bx, bz
                r["r_x%d" % v], r["r_z%d" % v] = rx, rz
                r["spd%d" % v] = sp
                r["px%d" % v], r["pz%d" % v] = px0, pz0
                # H-JITTER bound for this sample/slot
                r["jit%d" % v] = math.hypot(px1 - px0, pz1 - pz0)
                # the discriminator: angle between the two headings, degrees
                if (bx or bz) and (rx or rz):
                    da = math.degrees(math.atan2(bz, bx) - math.atan2(rz, rx))
                    while da > 180.0:  da -= 360.0
                    while da < -180.0: da += 360.0
                    r["dang%d" % v] = abs(da)
                else:
                    r["dang%d" % v] = None
            if ok:
                rows.append(r)
            time.sleep(1.0 / hz)
    finally:
        proc.kill(); print("[pid] %d killed" % proc.pid)

    if not rows:
        sys.exit("no samples -- the process never became readable")

    cols = list(rows[0].keys())
    outc = Path(prefix + ".head.csv"); outc.parent.mkdir(parents=True, exist_ok=True)
    with open(outc, "w", newline="") as f:
        f.write(",".join(cols) + "\n")
        for r in rows:
            f.write(",".join("" if r.get(c) is None else repr(r[c]) for c in cols) + "\n")

    summary = {"samples": len(rows), "g_records": "0x%08x" % g_rec, "g_aib": "0x%08x" % g_aib}
    for v in range(NSLOT):
        d = [r["dang%d" % v] for r in rows if r.get("dang%d" % v) is not None]
        j = [r["jit%d" % v] for r in rows if r.get("jit%d" % v) is not None]
        moved = len({(round(r["px%d" % v], 3), round(r["pz%d" % v], 3)) for r in rows}) > 1
        nz = any(r["spd%d" % v] not in (0.0, None) for r in rows)
        summary["slot%d" % v] = {
            "n": len(d),
            "dang_max_deg": max(d) if d else None,
            "dang_median_deg": sorted(d)[len(d) // 2] if d else None,
            "dang_mean_deg": (sum(d) / len(d)) if d else None,
            "jitter_max_units": max(j) if j else None,
            "H_BASE_moved": moved,
            "H_BASE_speed_nonzero": nz,
            "speed_range": [min(r["spd%d" % v] for r in rows),
                            max(r["spd%d" % v] for r in rows)],
            "pos_range_x": [min(r["px%d" % v] for r in rows),
                            max(r["px%d" % v] for r in rows)],
        }
    Path(prefix + ".head.json").write_text(json.dumps(summary, indent=1))
    print(json.dumps(summary, indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
