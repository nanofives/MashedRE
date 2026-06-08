# Find the REAL frontend input consumer — OBSERVATION ONLY (no writes, no OS input).
#
# Uses Frida MemoryAccessMonitor (guard-page) over the data page holding the cooked input
# flag blocks, filters to the byte ranges we care about, and logs the accessing PC + op
# (read/write) + target address. Run during the live interactive menu (settle inside the
# ~5..24.5s window). Deduped by (pc, op, slot) -> reveals every code site that touches the
# input flags while the menu is up = the actual consumers/producers. No globals are written.
#
# Ranges of interest (player p in 0..3, stride 0x4c):
#   0x7f1038 + p*0x4c .. +0x4c : cooked block (working bytes, digital flags, analog floats)
#   0x7f14f8 + p*0x4c .. +0x4c : previous-frame copy (edge-detect partner)
#
# Usage: py -3.12 re/frida/input_consumer_watch.py [--settle MS] [--seconds N]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMGBASE = 0x00400000;
let DELTA = 0;
let SETTLE = 6000;
const COOK_BASE = 0x007f1038, COOK_SPAN = 4*0x4c;   // players 0..3 cooked blocks
const PREV_BASE = 0x007f14f8, PREV_SPAN = 4*0x4c;   // players 0..3 prev-frame copies
let t0 = Date.now(), armed = false;
const hits = {};   // key "pc|op|slot" -> {pc, op, off, n}
let total = 0;

function abs(rva){ return ptr(rva + DELTA); }
rpc.exports = { hits: function(){ return hits; }, total: function(){ return total; } };

function inRange(addrU32){
  const cb = COOK_BASE + DELTA, pb = PREV_BASE + DELTA;
  if (addrU32 >= cb && addrU32 < cb + COOK_SPAN) return {blk:'cook', off: addrU32 - cb};
  if (addrU32 >= pb && addrU32 < pb + PREV_SPAN) return {blk:'prev', off: addrU32 - pb};
  return null;
}

function modName(pc){
  try { const m = Process.findModuleByAddress(pc); return m ? m.name : '?'; } catch(e){ return '?'; }
}

function arm(){
  if (armed) return; armed = true;
  // page(s) covering both regions
  const pageCook = ptr((COOK_BASE + DELTA) & ~0xfff);
  const pagePrev = ptr((PREV_BASE + DELTA) & ~0xfff);
  const ranges = [{base: pageCook, size: 0x1000}];
  if (pagePrev.compare(pageCook) !== 0) ranges.push({base: pagePrev, size: 0x1000});
  try {
    MemoryAccessMonitor.enable(ranges, { onAccess: function(d){
      total++;
      const au = d.address.toUInt32();
      const r = inRange(au);
      if (!r) return;                          // ignore other data on the same page
      const pc = d.from;
      const pcRva = pc.toUInt32() - DELTA;
      const op = d.operation;                  // 'read' | 'write' | 'execute'
      const slot = r.blk + '+0x' + r.off.toString(16);
      const key = '0x' + pcRva.toString(16) + '|' + op + '|' + slot;
      if (!hits[key]) hits[key] = {pcRva:'0x'+pcRva.toString(16), mod:modName(pc), op:op, slot:slot, n:0};
      hits[key].n++;
    }});
    send({kind:'info', msg:'MemoryAccessMonitor armed on '+ranges.length+' page(s)'});
  } catch(e){ send({kind:'err', msg:'arm '+e}); }
}

(function(){
  try { const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
        DELTA = m.base.toUInt32() - IMGBASE; send({kind:'info', msg:'base='+m.base}); }
  catch(e){ send({kind:'err', msg:'base '+e}); return; }
  rpc.exports.setsettle = function(ms){ SETTLE = ms; return ms; };
  // arm after settle so we catch the live-menu consumers, not boot
  const iv = setInterval(function(){ if (Date.now()-t0 >= SETTLE){ clearInterval(iv); arm(); } }, 200);
})();
'''

def main():
    settle = int(sys.argv[sys.argv.index("--settle")+1]) if "--settle" in sys.argv else 6000
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 18
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") == "info": print("  [watch]", p["msg"])
        elif p.get("kind") == "err": print("  [err]", p["msg"])
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    try: scr.exports_sync.setsettle(settle)
    except Exception: pass
    dev.resume(pid)
    print(f"  resumed; arming access-monitor after {settle}ms; watching {seconds}s (no input)")
    t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  exited"); break
        time.sleep(0.5)
    hits = {}; total = 0
    try: hits = scr.exports_sync.hits()
    except Exception: pass
    try: total = scr.exports_sync.total()
    except Exception: pass
    rows = sorted(hits.values(), key=lambda h: (h["op"], h["slot"], -h["n"]))
    print(f"\n  === input-flag accessors (total page hits {total}) ===")
    print(f"  {'PC(rva)':>12} {'mod':>12} {'op':>6}  slot               n")
    for h in rows:
        print(f"  {h['pcRva']:>12} {h['mod']:>12} {h['op']:>6}  {h['slot']:<16} {h['n']}")
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
