# In-process menu input feeder v2 — writes the COOKED frontend edge flags directly.
#
# SAFE: writes ONLY MASHED's own global byte flags via Frida. Never calls keybd_event /
# SendInput / PostMessage / SetForegroundWindow — nothing touches the OS input queue or
# any other window. Cannot affect the host PC.
#
# Why v2: the v1 DI-buffer feed (input_feed.py) wrote the GetDeviceState 256-byte buffer
# (the correct raw stage), but the frontend's control resolver FUN_00497310 gates keyboard
# on the active player's device-type==2 AND the injected DIK matching the player's mapped
# scancode. v2 bypasses that by writing the cooked per-player edge flags the frontend tick
# reads directly. See re/analysis/frontend_input_nav_trace/TRACE.md for the full chain.
#
# Chain (verified, MASHED.exe @ image base 0x00400000):
#   FUN_004967e0 (per-frame input mgr) -> FUN_00496530(player) cooks edge flags
#     player P edge flags: 0x007f1044 + P*0x4c  (+0=left +1=right +2=up +3=down)
#   FUN_0043dfd0 (frontend tick) reads those flags -> moves cursor
#   selection index = *(int*)(0x0067ed40 + DAT_0067e9f8*0x40)   (DAT_0067e9f8 @ 0x0067e9f8)
#
# Hook point: FUN_004967e0 onLeave -> after cooking, set/clear player-0 edge flag (tap),
# and sample the selection index to prove the menu navigated.
#
# Usage: py -3.12 re/frida/input_feed_cooked.py [--seq down,down,down] [--settle MS]
#        [--seconds N] [--player P] [--count 0x..,..]
import os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

# control index within a player's 0x4c flag block (byte offset from block base)
CTRL = {"left":0, "right":1, "up":2, "down":3}

AGENT = r'''
'use strict';
const IMGBASE = 0x00400000;
let DELTA = 0;
const RVA_INPUTMGR = 0x004967e0;   // FUN_004967e0 (per-frame input manager)
const RVA_FLAGS    = 0x007f1044;   // player-0 edge-flag block base
const RVA_DEPTH    = 0x0067e9f8;   // DAT_0067e9f8 menu-stack depth
const RVA_SEL      = 0x0067ed40;   // selection table base (col*0x40)

let SEQ = [], SETTLE = 13000, PRESS_FRAMES = 2, GAP_FRAMES = 14, PLAYER = 0;
let startT = 0, frame = 0, taps = 0;
const CNT = {};
let selTimeline = [];   // [{f, t, depth, ctrl, v}]
let lastSig = null;

function abs(rva){ return ptr(rva + DELTA); }

rpc.exports = {
  setplan: function(seq, settle, press, gap, player){
    SEQ = seq; SETTLE = settle; PRESS_FRAMES = press; GAP_FRAMES = gap; PLAYER = player;
    startT = Date.now(); return 1; },
  taps: function(){ return taps; },
  timeline: function(){ return selTimeline; },
  countthese: function(rvas){ rvas.forEach(function(r){ const a = parseInt(r,16); CNT[r]=0;
    try{ Interceptor.attach(abs(a),{onEnter(){CNT[r]++;}}); }catch(e){ CNT[r]=-1; } });
    return Object.keys(CNT).length; },
  counts: function(){ return CNT; }
};

// broaden: sample several candidate selection globals so we can see which (if any) moves
const PROBES = [0x0067ed40,0x0067ed44,0x0067ed74,0x0067ed78,0x0067ed7c,0x0067ed80,0x0067ece0,0x008990e4];
function readSel(){
  try {
    const depth = abs(RVA_DEPTH).readS32();
    const v = {};
    PROBES.forEach(function(p){ try { v['0x'+p.toString(16)] = abs(p + depth*0x40).readS32(); } catch(e){ v['0x'+p.toString(16)]=-999; } });
    // signature = depth + all probe values joined, to detect ANY change
    const sig = depth + ':' + PROBES.map(function(p){ return v['0x'+p.toString(16)]; }).join(',');
    return {depth:depth, sel:v['0x67ed40'], sig:sig, v:v};
  } catch(e){ return {depth:-999, sel:-999, sig:'err'}; }
}

function curCtrl(){
  if (!SEQ.length) return -1;
  const t = Date.now() - startT - SETTLE;
  if (t < 0) return -1;
  // schedule by FRAME count instead of wall time for stable edges
  const period = PRESS_FRAMES + GAP_FRAMES;
  const sinceSettle = frame - settleFrame;
  if (sinceSettle < 0) return -1;
  const idx = Math.floor(sinceSettle / period);
  if (idx >= SEQ.length) return -1;
  const phase = sinceSettle - idx*period;
  return (phase < PRESS_FRAMES) ? SEQ[idx] : -1;
}

let settleFrame = -1;
function onMgrLeave(){
  frame++;
  const t = Date.now() - startT;
  if (settleFrame < 0 && t >= SETTLE) settleFrame = frame;
  // DECISIVE TEST: blast the chosen control across ALL representations & players.
  //   edge flag:      0x7f1044 + p*0x4c + ctrl
  //   working buffer: 0x7f1038 + p*0x4c + ctrl   (raw mapped value the cook also writes)
  // ctrl 0=L 1=R 2=U 3=D. Covers whichever byte the directional consumer reads.
  const ctrl = curCtrl();
  if (ctrl >= 0){
    for (let p = 0; p < 4; p++){
      try { abs(0x007f1044 + p*0x4c + ctrl).writeU8(0xff); } catch(e){}
      try { abs(0x007f1038 + p*0x4c + ctrl).writeU8(0x7f); } catch(e){}
    }
    taps++;
  }
  // sample selection probes, log on ANY change
  const s = readSel();
  if (s.sig !== lastSig){
    selTimeline.push({f:frame, t:t, depth:s.depth, ctrl:ctrl, v:s.v});
    lastSig = s.sig;
  }
}

function hook(){
  try { Interceptor.attach(abs(RVA_INPUTMGR), { onLeave(){ onMgrLeave(); } });
        send({kind:'info', msg:'hooked input mgr @'+abs(RVA_INPUTMGR)}); return true; }
  catch(e){ send({kind:'err', msg:'hook '+e}); return false; }
}
// self-resolve module base (main module is mapped even when spawned suspended) and arm
(function(){
  try {
    const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
    DELTA = m.base.toUInt32() - IMGBASE;
    send({kind:'info', msg:'MASHED.exe base='+m.base+' DELTA=0x'+DELTA.toString(16)});
    hook();
  } catch(e){ send({kind:'err', msg:'base '+e}); }
})();
'''

def main():
    seq_names = (sys.argv[sys.argv.index("--seq")+1] if "--seq" in sys.argv else "down,down,down,down").split(",")
    settle = int(sys.argv[sys.argv.index("--settle")+1]) if "--settle" in sys.argv else 13000
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 32
    player = int(sys.argv[sys.argv.index("--player")+1]) if "--player" in sys.argv else 0
    count_rvas = sys.argv[sys.argv.index("--count")+1].split(",") if "--count" in sys.argv else []
    seq = [CTRL[s.strip().lower()] for s in seq_names if s.strip()]

    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"   # stock, no dev .asi
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    state = {"base": None}
    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") == "info": print("  [feed]", p["msg"])
        elif p.get("kind") == "err": print("  [err]", p["msg"])
        elif p.get("kind") == "need_base": state["base"] = True
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()  # arms hook at load

    if count_rvas:
        scr.exports_sync.countthese(count_rvas)
    scr.exports_sync.setplan(seq, settle, 2, 14, player)
    dev.resume(pid)
    print(f"  resumed; feeding seq={seq_names} player={player} after {settle}ms (COOKED flags, in-process)")

    t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("  exited"); break
        time.sleep(0.5)

    taps = 0; counts = {}; tl = []
    try: taps = scr.exports_sync.taps()
    except Exception: pass
    try: tl = scr.exports_sync.timeline()
    except Exception: pass
    if count_rvas:
        try: counts = scr.exports_sync.counts()
        except Exception: pass
    print(f"\n  flag-writes (taps) applied: {taps}")
    print(f"  selection-probe timeline (on ANY change; ctrl 0=L 1=R 2=U 3=D, -1=none):")
    for e in tl[:80]:
        print(f"    f={e['f']:>5} t={e.get('t',0):>6}ms depth={e['depth']} ctrl={e.get('ctrl')} v={e.get('v')}")
    if count_rvas:
        print(f"  nav-handler counts: {counts}")
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
