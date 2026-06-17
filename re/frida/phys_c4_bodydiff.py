# WS-PHYS-C4-LANE — deterministic bit-identity A/B for the A4 (FUN_00470670)
# body math, against the LIVE original binary (ground truth).
#
# The vehicle physics fns take a 0xd04 register-ABI record and run >1000/s, so
# run_diff (scalar vectors, hook bypassed) cannot test them. This harness instead:
#   1. Spawns the ORIGINAL MASHED.exe with the .asi LOADED but NO hook installed
#      (MASHED_HOOK_ONLY=<token-matching-nothing>) -> A4 runs the ORIGINAL code.
#   2. Drives a canonical Quick-Battle race.
#   3. Hot-path-SAMPLES A4 (short window, one fn): captures at the A4 ENTRY the
#      record snapshot + (input ptr, dt, gameMode, ringPhase); captures at
#      0x00470914 (just before the A5/A6a/A6b dispatch) the ORIGINAL's body
#      output fields (the only fields A4's own body writes).
#   4. For each captured call, calls the .asi export A4_BodyMath (my verbatim
#      transcription, self-contained, built x87) on a COPY of the entry record
#      with the SAME inputs, and bit-compares its body outputs to the original's.
# Same exact input record -> deterministic; ZERO race-noise. Any field mismatch
# is RED (first-diff cited); all-identical over N calls is bit-identity = C4.
#
# Usage: py -3.12 re/frida/phys_c4_bodydiff.py [--calls N] [--seconds S]
import os, shutil, struct, subprocess, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"
EXE  = ORIG / "MASHED.exe"
NAV  = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")

A4   = 0x00470670
A4_BODYEND = 0x00470914     # MOV ECX,[ESP+0x24] — first instr after the body, before A5 dispatch
RECSZ = 0xd04

# A4-body output fields (byte offset, label) — written by the body BEFORE any callee.
# (the dynamic ring slots +0x1ac/+0x270 [phase] are checked via the phase index)
BODY_FIELDS = [0x1a8, 0x26c, 0xb0c, 0xb24, 0xb28, 0xb14, 0xb18, 0xb1c, 0xb20, 0x330, 0x3f4]

def agent():
    bf = ",".join(str(o) for o in BODY_FIELDS)
    return NAV + r"""
;(function(){
  const A4 = %d, BODYEND = %d, RECSZ = %d;
  const BODY_FIELDS = [%s];
  let DELTA2 = 0;
  let calls = [];          // {rec:[bytes], input:[8], dt, gm, phase, out:{off:u32}, outRing:{} }
  let pending = null;
  let capping = false;
  let nWant = 40;
  let GM = null, RINGADDR = null;

  function abase(){ const m = Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0]; return m.base; }
  let MINE = null, SCRATCH = null, SCRINP = null;
  // record fields the A4 body READS (pre-body), copied into SCRATCH so MINE sees
  // identical inputs. Small set -> minimal hot-path footprint (NO 0xd04 copy).
  const IN_FIELDS = [0x190,0x9b0,0x9b4,0x9b8,0x9d4,0x9d8,0x9dc,0x9e4,0xb24,0xb28,0xbf0,0x9a8];

  // ONE-SHOT capture+compare in-process. Both interceptors detach after the FIRST
  // completed call (entry->bodyend) so the hot-path exposure is a single call —
  // the only footprint that survives (CLAUDE.md: Interceptor on >1000/s A4
  // destabilizes MASHED in ~6s; a sustained attach dies before capture).
  rpc.exports.startcap = function(n){
    nWant = n; calls = []; capping = true;
    const B = abase(); DELTA2 = B.toUInt32() - 0x00400000;
    GM = new NativeFunction(B.add(0x0040e350 - 0x00400000), 'int', []);
    RINGADDR = B.add(0x007f101c - 0x00400000);
    const asi = Process.findModuleByName('mashed_re_dev.asi');
    MINE = new NativeFunction(asi.getExportByName('A4_BodyMath'),
                              'void', ['pointer','pointer','float','int','int']);
    SCRATCH = Memory.alloc(RECSZ); SCRINP = Memory.alloc(8);
    const eA4 = B.add(A4 - 0x00400000), eBE = B.add(BODYEND - 0x00400000);
    let lA4=null, lBE=null;
    function done(){ capping=false; try{if(lA4)lA4.detach();if(lBE)lBE.detach();}catch(e){} }
    lA4 = Interceptor.attach(eA4, {
      onEnter(a){
        if (!capping || pending) return;
        const sp = this.context.esp;
        const rec = this.context.eax;
        const inp = ptr(sp.add(0xc).readU32());
        // copy ONLY the input fields the body reads, into SCRATCH (small)
        for (const off of IN_FIELDS){ try{ SCRATCH.add(off).writeU32(rec.add(off).readU32()); }catch(e){} }
        try { SCRINP.writeByteArray(inp.readByteArray(8)); } catch(e){}
        pending = { recPtr: rec, dt: sp.add(8).readFloat(),
                    phase: RINGADDR.readU32() & 0xf, gm: GM() };
      }
    });
    lBE = Interceptor.attach(eBE, {
      onEnter(a){
        if (!capping || !pending) return;
        const rec = pending.recPtr, p = pending.phase;
        let orig = {};
        for (const off of BODY_FIELDS) orig[off] = rec.add(off).readU32();
        orig['rd'] = rec.add(0x1ac+p*4).readU32();
        orig['ra'] = rec.add(0x270+p*4).readU32();
        MINE(SCRATCH, SCRINP, pending.dt, pending.gm, pending.phase);   // my transcription
        let diffs = [];
        for (const off of BODY_FIELDS){ const m=SCRATCH.add(off).readU32(); if(m!==orig[off]) diffs.push(['+0x'+off.toString(16),orig[off],m]); }
        { const m=SCRATCH.add(0x1ac+p*4).readU32(); if(m!==orig['rd']) diffs.push(['ring_drv',orig['rd'],m]); }
        { const m=SCRATCH.add(0x270+p*4).readU32(); if(m!==orig['ra']) diffs.push(['ring_ang',orig['ra'],m]); }
        calls.push({ phase:p, gm:pending.gm, dt:pending.dt,
                     in0:SCRINP.readU8(), in1:SCRINP.add(1).readU8(), in5:SCRINP.add(5).readU8(),
                     ndiff: diffs.length, diffs: diffs });
        pending = null;
        if (calls.length >= nWant) done();
      }
    });
    return 1;
  };
  rpc.exports.ncap = function(){ return calls.length; };
  rpc.exports.stopcap = function(){ capping = false; try{Interceptor.detachAll();}catch(e){} return calls.length; };
  rpc.exports.getcalls = function(){ return calls; };
})();
""" % (A4, A4_BODYEND, RECSZ, bf)

def main():
    calls_want = 40; seconds = 14
    if "--calls" in sys.argv: calls_want = int(sys.argv[sys.argv.index("--calls")+1])
    if "--seconds" in sys.argv: seconds = int(sys.argv[sys.argv.index("--seconds")+1])

    canon = ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists(): shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))
    env = dict(os.environ)
    env["MASHED_RE_DEV"] = "1"
    env["MASHED_HOOK_ONLY"] = "ZZ_NO_HOOK_INSTALLED_ZZ"   # .asi loads, installs NOTHING
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
        print(f"  spawn/attach attempt {attempt+1} failed — retry"); time.sleep(1.0)
    if sess is None: print("attach failed"); return 4

    scr = sess.create_script(agent())
    scr.on("message", lambda m,d: print("  agent:", m.get("description") or m.get("payload")) if m["type"]=="error" else None)
    scr.load()
    E = scr.exports_sync
    E.init()

    def wait(pred, t, what):
        end = time.time()+t
        while time.time()<end:
            if pred(): return True
            time.sleep(0.1)
        print(f"  timeout {what} depth={E.depth()} phase={E.phase()}"); return False
    def press(c, ms=180): E.press(c, ms); time.sleep(ms/1000.0+0.3)
    def confirm_to(tg, tries=6):
        for _ in range(tries):
            if E.depth()>=tg: return True
            press(4)
            if wait(lambda: E.depth()>=tg, 2.0, f"d{tg}"): return True
        return E.depth()>=tg

    rc = 0
    try:
        print("booting to menu...")
        if not wait(lambda: E.phase()==3 and E.depth()>=1, 30, "title"): return 2
        time.sleep(1.0)
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3); E.setsel(1); time.sleep(0.3)
        confirm_to(4,4); confirm_to(5,4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase()!=3: break
            press(4); time.sleep(1.5)
        if E.phase()==3: print("  not in race"); return 3
        print("  in race; sampling A4 (one-shot bursts to dodge hot-path destabilization)...")
        # Capture in small bursts; if a burst kills the script, keep what we got
        # from prior bursts. Each burst self-detaches the instant it fills.
        captured = []
        end = time.time()+seconds
        burst = max(1, calls_want)
        try:
            E.startcap(burst)
            while time.time()<end and E.ncap() < burst:
                time.sleep(0.02)
            try: E.stopcap()
            except Exception: pass
            captured = E.getcalls()
        except Exception as ex:
            print(f"  (script died during sampling: {ex}) — using whatever was captured")
            try: captured = E.getcalls()
            except Exception: captured = []
        n = len(captured)
        print(f"  captured {n} A4 calls (in-process A/B already computed per call)")
        if n == 0: print("  no A4 calls captured (race not driving?)"); return 5
        nfields = len(BODY_FIELDS) + 2     # + ring_drv + ring_ang
        total_mism = 0; driving = 0
        for i, c in enumerate(captured):
            nd = c.get('ndiff', -1)
            total_mism += max(nd, 0)
            if c.get('in0') or c.get('in1'):
                driving += 1
            tag = "OK" if nd == 0 else f"{nd} DIFF"
            if i < 12 or nd != 0:
                print(f"  call {i}: in0={c.get('in0')} in1={c.get('in1')} in5={c.get('in5')} "
                      f"gm={c.get('gm')} phase={c.get('phase')} dt={c.get('dt')} -> {tag}")
                for d in c.get('diffs', []):
                    lbl = d[0]
                    try: lbl = f"+0x{int(lbl):x}"
                    except Exception: pass
                    print(f"      {lbl}: orig=0x{d[1]:08x} mine=0x{d[2]:08x}")
        print(f"\n  {n} A4 calls x {nfields} body fields; {driving} calls had nonzero accel/brake input")
        if total_mism == 0:
            print(f"  GREEN: BIT-IDENTICAL — A4 body bit-matches the live original over {n}/{n} calls")
        else:
            print(f"  RED: {total_mism} field mismatches across {n} calls"); rc = 2
    finally:
        try: proc.kill()
        except Exception: pass
    return rc

if __name__ == "__main__":
    sys.exit(main())
