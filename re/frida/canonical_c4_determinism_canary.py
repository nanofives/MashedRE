# canonical_c4_determinism_canary.py — Phase 2 feasibility probe.
#
# QUESTION: is a fresh MASHED race bit-deterministic across two runs with identical setup/input?
#   YES -> Path A: a two-run in-race stalkerdiff works generically (mass-able, zero per-fn work).
#   NO  -> Path B: must use the physics-lane in-process A/B (same live input -> reimpl copy), per-fn.
#
# Method: spawn ORIGINAL MASHED (no hooks), drive the SAME canonical race twice (in-process input
# via nav_agent), and fingerprint the vehicle physics record at FRAME-SYNCED checkpoints (counting
# A4 0x00470670 physics calls from race start, so menu-timing variance is absorbed). Compare the
# two runs' fingerprints. A4 is hot (>1000/s) -> attach, grab the few checkpoints in <1s, detach.
#
# Usage: py -3.12 re/frida/canonical_c4_determinism_canary.py
import os, shutil, subprocess, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
NAV = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")

A4 = 0x00470670
REC_SLICE_OFF = 0xb00         # physics state region (pos/vel/orient) per project_wsa_a1
REC_SLICE_N   = 64            # dwords
CHECKPOINTS   = [120, 300, 600, 1200]   # A4-call indices (multiples of ~6 cars)

def agent():
    return NAV + r"""
;(function(){
  const A4 = %d, OFF = %d, NW = %d, CKPTS = [%s];
  let DELTA2 = 0, n = 0, capping = false, lh = null;
  const snaps = {};   // ckpt -> [dwords]
  function ab(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0]; return m.base; }
  rpc.exports.startcap = function(){
    n = 0; capping = true; const B = ab(); DELTA2 = B.toUInt32() - 0x00400000;
    const last = CKPTS[CKPTS.length-1];
    lh = Interceptor.attach(B.add(A4 - 0x00400000), { onEnter(a){
      if (!capping) return;
      n++;
      if (CKPTS.indexOf(n) >= 0){
        const rec = this.context.eax; const arr = [];
        for (let i=0;i<NW;i++){ try{ arr.push(rec.add(OFF+i*4).readU32()); }catch(e){ arr.push(0); } }
        snaps[n] = arr;
      }
      if (n >= last){ capping=false; try{ lh.detach(); }catch(e){} }
    }});
    return 1;
  };
  rpc.exports.ncap = function(){ return n; };
  rpc.exports.got = function(){ return Object.keys(snaps).length; };
  rpc.exports.snaps = function(){ return snaps; };
})();
""" % (A4, REC_SLICE_OFF, REC_SLICE_N, ",".join(str(c) for c in CHECKPOINTS))

def run_once(tag):
    canon = ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists(): shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))
    env = dict(os.environ); env["MASHED_RE_DEV"] = "1"
    env["MASHED_HOOK_ONLY"] = "ZZ_NO_HOOK_ZZ"        # .asi loads, installs nothing -> original code
    NPG, DET = 0x00000200, 0x00000008
    dev = frida.get_local_device()
    proc = sess = None
    for attempt in range(5):
        proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            creationflags=NPG | DET)
        time.sleep(0.2)
        try: sess = dev.attach(proc.pid); break
        except Exception:
            try: proc.kill()
            except Exception: pass
            time.sleep(1.0)
    if sess is None: print(f"  [{tag}] attach failed"); return None
    scr = sess.create_script(agent())
    scr.on("message", lambda m, d: print("  agent:", m.get("description")) if m["type"] == "error" else None)
    scr.load(); E = scr.exports_sync; E.init()

    def wait(pred, t, what):
        end = time.time()+t
        while time.time() < end:
            if pred(): return True
            time.sleep(0.1)
        print(f"  [{tag}] timeout {what} depth={E.depth()} phase={E.phase()}"); return False
    def press(c, ms=180): E.press(c, ms); time.sleep(ms/1000.0+0.3)
    def confirm_to(tg, tries=6):
        for _ in range(tries):
            if E.depth() >= tg: return True
            press(4)
            if wait(lambda: E.depth() >= tg, 2.0, f"d{tg}"): return True
        return E.depth() >= tg

    snaps = None
    try:
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 30, "title"): return None
        time.sleep(1.0)
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3); E.setsel(1); time.sleep(0.3)
        confirm_to(4, 4); confirm_to(5, 4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3: break
            press(4); time.sleep(1.5)
        if E.phase() == 3: print(f"  [{tag}] not in race"); return None
        print(f"  [{tag}] in race; capturing physics fingerprints at A4 checkpoints...")
        E.startcap()
        end = time.time() + 12
        while time.time() < end and E.got() < len(CHECKPOINTS):
            time.sleep(0.02)
        snaps = E.snaps()
        print(f"  [{tag}] A4 calls={E.ncap()}  checkpoints captured={E.got()}/{len(CHECKPOINTS)}")
    finally:
        try: proc.kill()
        except Exception: pass
    return snaps

def main():
    print("=== RUN 1 ===");  r1 = run_once("run1")
    if not r1: print("run1 produced no data"); return 2
    time.sleep(1.0)
    print("=== RUN 2 ==="); r2 = run_once("run2")
    if not r2: print("run2 produced no data"); return 2

    print("\n=== DETERMINISM VERDICT (record[0x%03x..] %d dwords per checkpoint) ===" % (REC_SLICE_OFF, REC_SLICE_N))
    total_diff = 0; common = 0
    for c in CHECKPOINTS:
        a = r1.get(str(c)) or r1.get(c); b = r2.get(str(c)) or r2.get(c)
        if not a or not b:
            print(f"  ckpt {c}: MISSING (run1={'ok' if a else 'none'} run2={'ok' if b else 'none'})"); continue
        common += 1
        diffs = [(i, a[i], b[i]) for i in range(min(len(a), len(b))) if a[i] != b[i]]
        total_diff += len(diffs)
        if not diffs:
            print(f"  ckpt {c}: IDENTICAL ({len(a)} dwords)")
        else:
            print(f"  ckpt {c}: {len(diffs)}/{len(a)} dwords DIFFER")
            for i, x, y in diffs[:6]:
                print(f"      [+0x{REC_SLICE_OFF+i*4:x}] run1=0x{x:08x} run2=0x{y:08x}")
    print()
    if common == 0:
        print("  INCONCLUSIVE: no overlapping checkpoints captured")
    elif total_diff == 0:
        print("  >>> DETERMINISTIC: race evolves bit-identically across runs -> PATH A viable")
        print("      (two-run in-race stalkerdiff works generically; add input-forcing for input paths)")
    else:
        print(f"  >>> NON-DETERMINISTIC: {total_diff} dword diffs across {common} checkpoints -> PATH B")
        print("      (need in-process A/B on identical live input, physics-lane style, per-function)")
    return 0

if __name__ == "__main__":
    sys.exit(main())
