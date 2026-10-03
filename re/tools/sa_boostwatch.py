"""Knob-took witness for MASHED_NO_START_BOOST (D3, 2026-10-03). READ-ONLY on the game.

Spawns the STANDALONE mashed_re.exe exactly the way re/tools/sa_capture.py does
(same cwd, same env pass-through) and then polls its memory with
ReadProcessMemory -- no injection, no code change, no Frida. It records, per
vehicle slot 0..3 and per sample:

  sb      g_startBoosted[slot]                       (bool, VehiclePhysicsRun.cpp:148)
  bf8     record +0xbf8  (boost state; seeded = 1 at VehiclePhysicsRun.cpp:706)
  bf4     record +0xbf4  (boost timer; seeded = 1300 at VehiclePhysicsRun.cpp:707)
  v9e4    record +0x9e4  (speed, the (e) statistic's field)
  self    record +0x000  (the self-ref slot index)

ADDRESSES. The standalone links /BASE:0x10000 /DYNAMICBASE:NO (mashedmod/build.bat:218),
so ASLR is OFF and the linker map's "Rva+Base" column is the runtime VA:

  g_records       0x00c09b88   mashedmod/build/mashed_re.map line 16264
  g_startBoosted  0x00c16ff8   mashedmod/build/mashed_re.map line 16269
  record stride   0xd04        VehiclePhysicsRun.cpp:109

Both are file-static arrays of THIS exe; they are NOT the original's DAT_008815a0.
Pass --map to re-read them out of a map file instead of trusting the constants.

SELF-CHECK (a base that is wrong must not produce a quiet answer): the original
writes the slot index into the record's first dword, `(&DAT_008815a0)[p*0x341] = p`
(0x0046bab8, Util/PromoLoop_round80.cpp:31), and the port keeps that. A sample is
only counted when record[v]+0x000 == v for every slot 0..3. The run reports how
many samples passed the self-check; zero passing samples makes the witness VOID
and the tool says so rather than printing zeros.

Usage:
  py -3.12 re/tools/sa_boostwatch.py <out_prefix> <seconds> [ENV=VAL ...]
            [--hz 50] [--map mashedmod/build/mashed_re.map]

Writes <out_prefix>.json (summary) and <out_prefix>.csv (every self-checked sample).
The process is spawned and killed BY PID; no blanket kill by name.
"""
import ctypes, json, os, re, subprocess, sys, time
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
EXE = ROOT / "mashedmod" / "build" / "mashed_re.exe"

G_RECORDS = 0x00c09b88
G_STARTBOOSTED = 0x00c16ff8
REC_STRIDE = 0xd04
NSLOT = 4

PROCESS_VM_READ = 0x0010
PROCESS_QUERY_INFORMATION = 0x0400

k32 = ctypes.WinDLL("kernel32", use_last_error=True)
k32.OpenProcess.restype = wintypes.HANDLE
k32.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p,
                                  ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]


def read(h, addr, n):
    buf = (ctypes.c_char * n)()
    got = ctypes.c_size_t(0)
    ok = k32.ReadProcessMemory(h, ctypes.c_void_p(addr), buf, n, ctypes.byref(got))
    if not ok or got.value != n:
        return None
    return bytes(buf)


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
    hz = 50.0
    mapfile = None
    if "--hz" in argv:
        i = argv.index("--hz"); hz = float(argv[i + 1]); del argv[i:i + 2]
    if "--map" in argv:
        i = argv.index("--map"); mapfile = argv[i + 1]; del argv[i:i + 2]
    prefix = argv[0]
    secs = float(argv[1])

    g_rec, g_sb = G_RECORDS, G_STARTBOOSTED
    if mapfile:
        # Vehicle's g_records, not Frontend's MenuRecord array of the same name.
        a = map_lookup(mapfile, "g_records@?A", "@Vehicle@mashed_re@@3PAEA")
        b = map_lookup(mapfile, "g_startBoosted")
        if a: g_rec = a
        if b: g_sb = b
    print("[addr] g_records=0x%08x g_startBoosted=0x%08x" % (g_rec, g_sb))

    env = dict(os.environ)
    env.setdefault("MASHED_WIN_POS", "primary-bl")
    for kv in argv[2:]:
        k, _, v = kv.partition("=")
        env[k] = v

    proc = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print("[pid] %d" % proc.pid)
    h = k32.OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, False, proc.pid)
    if not h:
        proc.kill()
        sys.exit("OpenProcess failed: %d" % ctypes.get_last_error())

    span = REC_STRIDE * NSLOT
    rows = []
    n_try = n_ok = 0
    t0 = time.time()
    try:
        while time.time() - t0 < secs:
            if proc.poll() is not None:
                print("exe exited early (rc=%s) at t=%.2f" % (proc.returncode, time.time() - t0))
                break
            n_try += 1
            blob = read(h, g_rec, span)
            sbb = read(h, g_sb, NSLOT)
            if blob is not None and sbb is not None:
                ints = [int.from_bytes(blob[v * REC_STRIDE:v * REC_STRIDE + 4], "little", signed=True)
                        for v in range(NSLOT)]
                if all(ints[v] == v for v in range(NSLOT)):
                    n_ok += 1
                    t = round(time.time() - t0, 3)
                    for v in range(NSLOT):
                        o = v * REC_STRIDE
                        bf8 = int.from_bytes(blob[o + 0xbf8:o + 0xbfc], "little", signed=True)
                        bf4 = int.from_bytes(blob[o + 0xbf4:o + 0xbf8], "little", signed=True)
                        sp = ctypes.c_float.from_buffer_copy(blob[o + 0x9e4:o + 0x9e8]).value
                        rows.append((t, v, sbb[v], bf8, bf4, sp))
            time.sleep(1.0 / hz)
    finally:
        try: proc.kill()
        except Exception: pass

    out = {"pid": proc.pid, "seconds": secs, "hz": hz,
           "g_records": "0x%08x" % g_rec, "g_startBoosted": "0x%08x" % g_sb,
           "samples_attempted": n_try, "samples_selfchecked": n_ok,
           "selfcheck": "VOID (0 samples passed record[v]+0x000 == v)" if n_ok == 0 else "OK",
           "env": {k: env[k] for k in sorted(env) if k.startswith("MASHED_")},
           "slots": {}}
    for v in range(NSLOT):
        r = [x for x in rows if x[1] == v]
        nz = [x for x in r if x[3] != 0]
        sbon = [x for x in r if x[2] != 0]
        out["slots"][str(v)] = {
            "n": len(r),
            "sb_values": sorted({int(x[2]) for x in r}),
            "sb_first_t": sbon[0][0] if sbon else None,
            "bf8_values": sorted({x[3] for x in r}),
            "bf8_nonzero_samples": len(nz),
            "bf8_first_t": nz[0][0] if nz else None,
            "bf4_max": max((x[4] for x in r), default=None),
            "bf4_at_bf8_first": nz[0][4] if nz else None,
            "bf4_values_while_bf8": sorted({x[4] for x in nz}) if nz else [],
            "v9e4_max": round(max((x[5] for x in r), default=0.0), 3),
        }
    Path(prefix + ".json").write_text(json.dumps(out, indent=1), encoding="utf-8")
    with open(prefix + ".csv", "w", newline="", encoding="utf-8") as f:
        f.write("t,v,sb,bf8,bf4,v9e4\n")
        for t, v, sb, bf8, bf4, sp in rows:
            f.write("%.3f,%d,%d,%d,%d,%.6f\n" % (t, v, sb, bf8, bf4, sp))
    print(json.dumps(out["slots"], indent=1))
    print("selfcheck: %s  (%d/%d samples)" % (out["selfcheck"], n_ok, n_try))
    return 0


if __name__ == "__main__":
    sys.exit(main())
