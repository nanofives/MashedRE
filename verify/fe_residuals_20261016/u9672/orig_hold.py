"""U-9672 (3) key repeat on hold: ORIGINAL row timeline while DOWN is held on screen 6. Own PID only.
Usage: MASHED_ROOT=<gameroot> py -3.12 orig_hold.py <hold_ms> <out.json>
Route as row_movers (--route 0,0,k): title -> depth 2 -> advance_past_load_modal(3) -> confirm (Challenge Cup)
-> confirm (colour select) -> screen 6; one short DOWN (cursor -> -1), then statenav press(12, hold_ms) with a 20 ms
sampler of row [0x0067f17c], cursor of the top slot, depth. Needs a save with several unlocked mode-3 rows."""
import json, sys, time
from pathlib import Path
ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 're' / 'frida'))
import u9667_menu_table_dump as U
import statenav
from statenav import Nav
hold_ms = int(sys.argv[1]); out = Path(sys.argv[2])
SAMPLER = r'''
'use strict';
const IMG=0x00400000; let DELTA=0; const rows=[]; let t0=0;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0]; DELTA=m.base.toUInt32()-IMG; return DELTA; },
  start:function(){ t0=Date.now(); setInterval(function(){
      const d=abs(0x0067e9f8).readS32();
      rows.push([Date.now()-t0, abs(0x0067f17c).readS32(), abs(0x0067ed40+d*0x40).readS32(), d, abs(0x0067f184).readS32()]);
    }, 20); return 1; },
  mark:function(){ return Date.now()-t0; },
  rows:function(){ return rows; }
};
'''
dev, pid, sess = U.spawn()
rec = {"pid": pid, "hold_ms": hold_ms, "events": []}
try:
    S = U.load(sess, statenav.AGENT); S.exports_sync.init()
    ds = sess.create_script(U.DUMP_AGENT); ds.load(); D = ds.exports_sync; D.init()
    smp = U.load(sess, SAMPLER).exports_sync; smp.init()
    dev.resume(pid)
    nav = Nav(S, pid)
    U.wait_phase(D, 3, 30); time.sleep(1.0)
    nav.confirm_to_depth(2); rec["events"].append("advance " + str(nav.advance_past_load_modal(3))); time.sleep(0.8)
    smp.start()
    nav.press(4); time.sleep(1.2)          # Challenge Cup -> colour select
    nav.press(4); time.sleep(1.5)          # colour select -> screen 6
    d = D.dump(4); rec["at_screen6"] = {"depth": d["depth"], "mode": d["mode"], "row": d["row"], "sel": d["sel"]}
    nav.press(12, 180); time.sleep(0.6)    # DOWN: cursor -> -1
    rec["t_hold_start"] = smp.mark()
    S.exports_sync.press(12, hold_ms); time.sleep(hold_ms / 1000.0 + 0.8)
    rec["rows"] = smp.rows()
except Exception as e:
    rec["error"] = repr(e); print("error", e)
finally:
    try: dev.kill(pid)
    except Exception: pass
    out.parent.mkdir(parents=True, exist_ok=True); out.write_text(json.dumps(rec))
print(rec.get("at_screen6"), rec.get("error"), len(rec.get("rows", [])))
