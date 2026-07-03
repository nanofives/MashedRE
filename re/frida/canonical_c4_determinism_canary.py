# canonical_c4_determinism_canary.py — Phase 2 feasibility probe.
#
# QUESTION: is a fresh MASHED race bit-deterministic across two runs with identical setup/input?
#   YES -> Path A: a two-run in-race stalkerdiff works generically (mass-able, zero per-fn work).
#   NO  -> Path B: must use the physics-lane in-process A/B (same live input -> reimpl copy), per-fn.
#
# Method: spawn stock ORIGINAL MASHED (no installed hooks) and WARP straight into the SAME canonical
# race twice via scenario_launch's proven setup (deterministic global-poke: track/mode/rule/car +
# FUN_0040e480 slot activation, then PHASE:=2 -> race). No menu-walk, no injected input -> both runs
# get byte-identical setup, so any divergence is the game's own nondeterminism (which is exactly the
# question). The A4 0x00470670 physics counter is armed the instant the race is live, and counting
# is gated on the session-phase global (0x00771968 == 3 = race running), so call #1 is the first A4
# tick with the race live, identically in both runs — arm/attach-timing variance is absorbed. All-
# zero snapshots are INVALID (record read failed) and excluded; DETERMINISTIC needs >=1 valid common
# checkpoint. A4 is hot (>1000/s) -> grab the checkpoints quickly, detach at the last one.
#
# Usage: py -3.12 re/frida/canonical_c4_determinism_canary.py
import os, subprocess, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
sys.path.insert(0, str(Path(__file__).resolve().parent))
from scenario_launch import AGENT as WARP   # reuse the proven warp-into-race plumbing (no 2nd copy)

A4 = 0x00470670
REC_SLICE_OFF = 0xb00         # physics state region (pos/vel/orient) per project_wsa_a1
REC_SLICE_N   = 64            # dwords
CHECKPOINTS   = [120, 300, 600, 900]    # A4-call indices (A4 ticks ~60/s in a 1-car race)

def agent():
    # WARP defines top-level `ga(addr)` and `const PHASE` (0x00771968); the appended IIFE reuses
    # both. WARP also defines `rpc.exports = {...}`; we add our capture exports onto that object.
    return WARP + r"""
;(function(){
  const A4c = %d, OFF = %d, NW = %d, CKPTS = [%s];
  let n = 0, capping = false, started = false, lh = null;
  const snaps = {};   // ckpt -> [dwords]
  rpc.exports.startcap = function(){
    n = 0; started = false; capping = true;
    const last = CKPTS[CKPTS.length-1];
    lh = Interceptor.attach(ga(A4c), { onEnter(a){
      if (!capping) return;
      if (!started){
        // Race-start anchor: don't count until the session-phase global
        // (PHASE 0x00771968 == 3 = race running) says the race is live, so
        // both runs number A4 calls from the same in-game event, not from
        // the arm/attach moment.
        try { if (ga(PHASE).readU8() !== 3) return; } catch(e){ return; }
        started = true;
      }
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
  rpc.exports.started = function(){ return started ? 1 : 0; };
  rpc.exports.ncap = function(){ return n; };
  rpc.exports.got = function(){ return Object.keys(snaps).length; };
  rpc.exports.snaps = function(){ return snaps; };
})();
""" % (A4, REC_SLICE_OFF, REC_SLICE_N, ",".join(str(c) for c in CHECKPOINTS))

# Identical canonical setup for both runs (deterministic; matches scenario_launch defaults).
CFG = {"track": 0, "mode": 10, "cars": 1, "car": 0, "rule": 0,
       "team": 0, "difficulty": -1, "powerups": -1}

def run_once(tag):
    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"        # stock original, no installed hooks
    dev = frida.get_local_device()
    proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env)
    sess = None
    for _ in range(200):
        try: sess = dev.attach(proc.pid); break
        except Exception: time.sleep(0.1)
    if sess is None:
        print(f"  [{tag}] attach failed")
        try: proc.kill()
        except Exception: pass
        return None
    scr = sess.create_script(agent())
    scr.on("message", lambda m, d: print("  agent:", m.get("description")) if m["type"] == "error" else None)
    scr.load(); E = scr.exports_sync

    def wait(pred, t, what):
        end = time.time()+t
        while time.time() < end:
            try:
                if pred(): return True
            except Exception: pass
            time.sleep(0.1)
        try: ph = E.phase()
        except Exception: ph = "?"
        print(f"  [{tag}] timeout {what} (phase={ph})"); return False

    snaps = None
    try:
        if not wait(lambda: E.phase() == 1, 40, "menu"): return None
        print(f"  [{tag}] {E.setup(CFG)}")
        E.launch()
        if not wait(lambda: E.phase() == 3, 40, "race running"): return None
        E.startcap()
        print(f"  [{tag}] in race; capturing physics fingerprints at A4 checkpoints...")
        end = time.time() + 22
        while time.time() < end and E.got() < len(CHECKPOINTS):
            time.sleep(0.02)
        snaps = E.snaps()
        gate = "opened" if E.started() else "NEVER OPENED"
        print(f"  [{tag}] A4 calls={E.ncap()} (race-start gate {gate})  checkpoints captured={E.got()}/{len(CHECKPOINTS)}")
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
    total_diff = 0; common = 0; valid = 0
    for c in CHECKPOINTS:
        a = r1.get(str(c)) or r1.get(c); b = r2.get(str(c)) or r2.get(c)
        if not a or not b:
            print(f"  ckpt {c}: MISSING (run1={'ok' if a else 'none'} run2={'ok' if b else 'none'})"); continue
        common += 1
        nz1 = sum(1 for v in a if v); nz2 = sum(1 for v in b if v)
        if nz1 == 0 or nz2 == 0:
            print(f"  ckpt {c}: !! INVALID: all-zero snapshot (nonzero run1={nz1} run2={nz2}) -- record read")
            print(f"      failed (EAX not the record?); EXCLUDED from verdict")
            continue
        valid += 1
        diffs = [(i, a[i], b[i]) for i in range(min(len(a), len(b))) if a[i] != b[i]]
        total_diff += len(diffs)
        if not diffs:
            print(f"  ckpt {c}: IDENTICAL ({len(a)} dwords, {nz1} nonzero)")
        else:
            print(f"  ckpt {c}: {len(diffs)}/{len(a)} dwords DIFFER (nonzero run1={nz1} run2={nz2})")
            for i, x, y in diffs[:6]:
                print(f"      [+0x{REC_SLICE_OFF+i*4:x}] run1=0x{x:08x} run2=0x{y:08x}")
    print()
    if common == 0:
        print("  INCONCLUSIVE: no overlapping checkpoints captured")
    elif valid == 0:
        print("  INCONCLUSIVE: all overlapping checkpoints were all-zero (record reads failed) -- no valid data")
    elif total_diff == 0:
        print(f"  >>> DETERMINISTIC: race evolves bit-identically across runs ({valid} valid checkpoints) -> PATH A viable")
        print("      (two-run in-race stalkerdiff works generically; add input-forcing for input paths)")
    else:
        print(f"  >>> NON-DETERMINISTIC: {total_diff} dword diffs across {valid} valid checkpoints -> PATH B")
        print("      (need in-process A/B on identical live input, physics-lane style, per-function)")
    return 0

if __name__ == "__main__":
    sys.exit(main())
