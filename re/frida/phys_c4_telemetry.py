# WS-PHYS-C4-LANE — installed-hook canonical-race telemetry for the vehicle
# physics chain. Spawns the ORIGINAL MASHED.exe (Popen+attach so AppCompat /
# EMULATEHEAP applies and the dinput8 loader auto-loads mashed_re_dev.asi),
# drives a canonical Quick-Battle race (nav_agent.js recipe, same as
# scenario_attach_probe.py), and per-frame samples the player car's 0xd04 record
# fields (DAT_008815a0). Two modes:
#   hooked   : MASHED_HOOK_ONLY=<name>  -> ONLY that .asi hook installed live
#   baseline : MASHED_RE_NO_AUTO_HOOK=1 -> stock original, no hooks
# A faithful verbatim hook + a deterministic scripted race => bit-identical
# field trajectories. Divergence => RED (first-diff cited).
#
# Usage:
#   py -3.12 re/frida/phys_c4_telemetry.py hooked   A4_Entry,0x00470670
#   py -3.12 re/frida/phys_c4_telemetry.py baseline
#   py -3.12 re/frida/phys_c4_telemetry.py diff      # compare the two CSVs
import csv, os, shutil, subprocess, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"
EXE  = ORIG / "MASHED.exe"
LOG  = ROOT / "log"
NAV  = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")

REC0 = 0x008815a0   # DAT_008815a0 = player car 0 record base (vehicle.md)

# fields to sample (byte offset, label, type) — the chain's observable outputs
FIELDS = [
    (0x1a8, "driveTorque", "f"), (0x26c, "angTorque", "f"),
    (0x9b0, "velX", "f"), (0x9b4, "velY", "f"), (0x9b8, "velZ", "f"),
    (0x9d4, "fwdX", "f"), (0x9d8, "fwdY", "f"), (0x9dc, "fwdZ", "f"),
    (0x9bc, "angVelX", "f"), (0x9c0, "angVelY", "f"), (0x9c4, "angVelZ", "f"),
    (0x9e0, "groundedCnt", "x"), (0x9e4, "speed", "f"),
    (0xb0c, "slideMeasure", "f"), (0xb14, "boostX", "f"),
    (0xb24, "filtAccel", "i"), (0xb28, "filtBrake", "i"),
    (0x9f0, "motionState", "i"),
]

def telemetry_agent():
    fld = ",".join("[%d,'%s','%s']" % (o, n, t) for o, n, t in FIELDS)
    return NAV + r"""
;(function(){
  const REC = ptr(%d);
  const FLDS = [%s];
  let samples = [];
  function readF(off,t){
    const a = REC.add(off);
    try {
      if (t==='f') return a.readFloat();
      if (t==='x') return a.readU32();      // raw hex (grounded is float-as-int sentinel)
      return a.readS32();
    } catch(e){ return null; }
  }
  rpc.exports.a4jmp = function(){ try { return ptr(%d).readU8(); } catch(e){ return -1; } };
  rpc.exports.asibase = function(){ const m=Process.findModuleByName('mashed_re_dev.asi'); return m?m.base.toString():null; };
  rpc.exports.sample = function(tag){
    const row = { t: tag };
    for (const f of FLDS){ row[f[1]] = readF(f[0], f[2]); }
    samples.push(row);
    return row;
  };
  rpc.exports.dump = function(){ return samples; };
})();
""" % (REC0, fld, 0x00470670)

def drive_and_sample(mode, csv_path):
    canon = ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists():
        shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))
    env = dict(os.environ)
    if mode == "baseline":
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    else:                       # hooked: install ONLY the named hook(s)
        env["MASHED_RE_DEV"] = "1"
        env["MASHED_HOOK_ONLY"] = sys.argv[2] if len(sys.argv) > 2 else "A4_Entry,0x00470670"
        env.pop("MASHED_RE_NO_AUTO_HOOK", None)
    NPG, DET = 0x00000200, 0x00000008
    dev = frida.get_local_device()

    def spawn_attach():
        p = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            creationflags=NPG | DET)
        time.sleep(0.2)
        try: return p, dev.attach(p.pid)
        except Exception:
            try: p.kill()
            except Exception: pass
            return p, None

    proc = sess = None
    for attempt in range(5):
        proc, sess = spawn_attach()
        if sess is not None: break
        print(f"  spawn/attach attempt {attempt+1} failed — retry")
        time.sleep(1.0)
    if sess is None:
        print("attach failed after 5 retries"); return 4

    scr = sess.create_script(telemetry_agent())
    scr.on("message", lambda m, d: print("  agent:", m.get("description") or m.get("payload")) if m["type"]=="error" else None)
    scr.load()
    E = scr.exports_sync
    E.init()

    def wait(pred, timeout, what):
        end = time.time() + timeout
        while time.time() < end:
            if pred(): return True
            time.sleep(0.1)
        print(f"  timeout: {what} depth={E.depth()} phase={E.phase()}")
        return False
    def press(c, ms=180):
        E.press(c, ms); time.sleep(ms/1000.0 + 0.3)
    def confirm_to(target, tries=6):
        for _ in range(tries):
            if E.depth() >= target: return True
            press(4)
            if wait(lambda: E.depth() >= target, 2.0, f"d{target}"): return True
        return E.depth() >= target

    rc = 0
    try:
        print(f"[{mode}] booting to menu...")
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 30, "title"):
            return 2
        if mode != "baseline":
            jb = E.a4jmp()
            print(f"  A4 @0x00470670 first byte = 0x{jb:02x} "
                  f"({'JMP-LIVE' if jb==0xE9 else 'NOT-INSTALLED'}); asi={E.asibase()}")
        time.sleep(1.0)
        # Quick Battle, Arctic (race_refs recipe).
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3)
        E.setsel(1); time.sleep(0.3)
        confirm_to(4, 4); confirm_to(5, 4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3: break
            press(4); time.sleep(1.5)
        in_race = E.phase() != 3
        print(f"  in race? phase={E.phase()} ({'YES' if in_race else 'NO'})")
        if not in_race: return 3
        # Hold accelerate (control 4 acts as confirm; the demo/AI drives the car).
        # Sample the player record every ~250 ms. Window is env-tunable
        # (PHYS_C4_RACE_SECS).
        #
        # WS-PHYS-COVERAGE-SCENARIO (wave-13): the in-process self-test
        # (MASHED_PHYS_C4_SELFTEST) is the actual A/B; it fires for EVERY car the
        # per-frame dispatcher (FUN_00470c70) processes, NOT just the player. The AI
        # opponents (slots 1-3) brake into corners, steer, and fly off Arctic's terrain
        # on their own, so a LONGER race lets the brake / airborne / random-surface
        # branches arrive on the AI records and fill the dedicated per-branch quotas in
        # PhysicsChainHooks.cpp. Default raised 12 -> 45 s so the field reaches the first
        # corners + jumps. The Frida-sampled CSV here is only a liveness/screenshot
        # aid; the bit-identity verdict is the in-process selftest .log files.
        secs = float(os.environ.get("PHYS_C4_RACE_SECS", "45"))
        # Optional steer control to make the player car corner hard (ram walls/AI),
        # diversifying the per-car branch coverage. 4=accelerate (confirmed by
        # phys_c4_inrace_probe.py). PHYS_C4_STEER selects a steer control index.
        steer = int(os.environ.get("PHYS_C4_STEER", "-1"))
        n = 0
        end = time.time() + secs
        while time.time() < end:
            E.press(4, 300)         # keep accelerate held continuously
            time.sleep(0.05)
            if steer >= 0:
                E.press(steer, 300)
                time.sleep(0.05)
            E.sample(round(time.time()*4)/4.0)
            n += 1
            time.sleep(0.15)
        rows = E.dump()
        LOG.mkdir(exist_ok=True)
        with open(csv_path, "w", newline="", encoding="utf-8") as f:
            cols = ["t"] + [n for _, n, _ in FIELDS]
            w = csv.DictWriter(f, fieldnames=cols); w.writeheader()
            for r in rows: w.writerow(r)
        print(f"  {len(rows)} samples -> {csv_path}")
    finally:
        try: proc.kill()
        except Exception: pass
    return rc

def do_diff():
    a = LOG / "phys_c4_baseline.csv"
    b = LOG / "phys_c4_hooked.csv"
    if not (a.exists() and b.exists()):
        print("need both phys_c4_baseline.csv and phys_c4_hooked.csv"); return 1
    ra = list(csv.DictReader(open(a, encoding="utf-8")))
    rb = list(csv.DictReader(open(b, encoding="utf-8")))
    print(f"baseline={len(ra)} rows  hooked={len(rb)} rows")
    n = min(len(ra), len(rb))
    fields = [nm for _, nm, _ in FIELDS]
    mism = 0
    for i in range(n):
        for fnm in fields:
            va, vb = ra[i].get(fnm), rb[i].get(fnm)
            if va != vb:
                mism += 1
                if mism <= 20:
                    print(f"  DIFF row {i} {fnm}: base={va} hooked={vb}")
    print(f"\n{'GREEN: identical' if mism==0 else f'RED: {mism} field mismatches'} over {n} rows x {len(fields)} fields")
    return 0 if mism == 0 else 2

def main():
    if len(sys.argv) < 2:
        sys.exit("usage: phys_c4_telemetry.py {hooked|baseline|diff} [hook-names]")
    mode = sys.argv[1]
    if mode == "diff": return do_diff()
    csv_path = LOG / (f"phys_c4_{mode}.csv")
    return drive_and_sample(mode, csv_path)

if __name__ == "__main__":
    sys.exit(main())
