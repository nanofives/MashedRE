# WS-PHYS-COVERAGE-SCENARIO probe — learn the in-RACE control indices (accelerate /
# steer-left / steer-right) for player 0 so the coverage harness can drive the player
# car into hard cornering / collisions and lift wheels (grounded != 4.0 -> the A5/A6a
# airborne suspension branch; grounded == 0.0 -> the A6b aero body).
#
# Mechanism: same FUN_00497310 return-override the menu nav uses (no Interceptor on a
# physics hot path). After the race starts, sweep control index 0..15 one at a time,
# hold each for ~1.5 s while watching the player record's speed (+0x9e4) and grounded
# (+0x9e0); report which index makes the car accelerate / which lifts a wheel.
#
# Spawns its OWN MASHED.exe (Popen) and kills ONLY that PID in finally (process-hygiene).
import os, shutil, subprocess, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"
EXE  = ORIG / "MASHED.exe"
NAV  = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")
REC0 = 0x008815a0

def agent():
    return NAV + r"""
;(function(){
  const REC = ptr(%d);
  rpc.exports.speed = function(){ try { return REC.add(0x9e4).readFloat(); } catch(e){ return -1; } };
  rpc.exports.grounded = function(){ try { return REC.add(0x9e0).readU32(); } catch(e){ return 0; } };
})();
""" % (REC0,)

def main():
    canon = ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists():
        shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))
    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"     # stock original; we only need to drive it
    NPG, DET = 0x00000200, 0x00000008
    dev = frida.get_local_device()

    proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=NPG | DET)
    print(f"SPAWNED_PID={proc.pid}")
    time.sleep(0.3)
    sess = None
    for _ in range(5):
        try: sess = dev.attach(proc.pid); break
        except Exception: time.sleep(1.0)
    if sess is None:
        try: proc.kill()
        except Exception: pass
        print("attach failed"); return 4

    try:
        scr = sess.create_script(agent()); scr.load()
        E = scr.exports_sync; E.init()
        def wait(pred, to):
            end = time.time()+to
            while time.time()<end:
                if pred(): return True
                time.sleep(0.1)
            return False
        def press(c, ms=180):
            E.press(c, ms); time.sleep(ms/1000.0+0.25)
        def confirm_to(t, tries=6):
            for _ in range(tries):
                if E.depth()>=t: return True
                press(4)
                if wait(lambda: E.depth()>=t, 2.0): return True
            return E.depth()>=t
        print("booting...")
        if not wait(lambda: E.phase()==3 and E.depth()>=1, 30): print("no title"); return 2
        time.sleep(1.0)
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3); E.setsel(1); time.sleep(0.3)
        confirm_to(4,4); confirm_to(5,4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase()!=3: break
            press(4); time.sleep(1.5)
        if E.phase()==3: print("not in race"); return 3
        print(f"in race (phase={E.phase()}); sweeping accel+steer combos for wheel-lift...")
        time.sleep(2.0)
        # ctrl 4 = accelerate (confirmed). Find a steer control: combine accel with each
        # candidate, drive full-throttle while turning hard (rams walls / AI -> wheel lift),
        # and watch grounded(+0x9e0). Any value != 4.0 = a lifted wheel (airborne branch).
        cand = [0,1,2,3,5,6,9,10,11,12]
        results = []
        for steer in cand:
            mxsp=0.0; lifted=set()
            end=time.time()+5.0
            while time.time()<end:
                # interleave accel + steer presses to keep both "held"
                E.press(4, 250); time.sleep(0.05)
                E.press(steer, 250); time.sleep(0.05)
                sp=E.speed(); g=E.grounded()
                if sp>mxsp: mxsp=sp
                if g!=0x40800000: lifted.add(g)
            results.append((steer, round(mxsp,1), sorted(hex(x) for x in lifted)))
            print(f"  accel+steer={steer:2d}  maxSpeed={mxsp:8.1f}  groundedNon4={sorted(hex(x) for x in lifted)}")
            time.sleep(0.5)
        print("\n=== combos that lifted a wheel (grounded != 0x40800000) ===")
        for s,m,l in results:
            if l: print(f"  steer={s} maxSpeed={m} liftedGroundedVals={l}")
    finally:
        try: proc.kill()
        except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
