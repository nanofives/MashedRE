"""In-process A/B of 0x004282a0: the original's own body vs the shared body (Frontend/MenuMeasure.cpp) in a .asi.

Derived from verify/scr19_parity_20261012/arrow_fix/measure_probe.py (same boot, same screen-19 push, same
game-thread call site: one onEnter of the frame shell 0x0042e3a0, detached after one use). Differences:
  * spawns MASHED.exe from the PRIVATE root D:\\MashedWT\\D4-DC-82a0_root\\original (WORKER_RULES), with
    MASHED_RE_NO_AUTO_HOOK=1, so 0x004282a0 is the stock code;
  * Module.load()s the .asi given on argv[2] and resolves its export MenuMenusBA (the one shared body, its .asi
    cells pointing at the original's FUN_00427780 / FUN_004277a0 / FUN_005554d0 and [0x0067d838] / [0x0067d830]);
  * calls BOTH for every (id, scale) on the game thread, then again from the Frida JS thread (a thread the game
    did not create, so not the game's x87 precision control), and records each return as a double plus its bits.
Writes nothing into game memory. No key input. Kills only the PID it spawned.
"""
import json, os, struct, subprocess, sys, time
from pathlib import Path
import frida

ORIG = Path(r"D:\MashedWT\D4-DC-82a0_root\original")
EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
function abs(r){return ptr(r+DELTA);}
let nav=null, orig=null, port=null, result=null;
function run(ids, scales){
  const out=[];
  ids.forEach(function(id){ scales.forEach(function(s){
    out.push({id:id, scale:s, orig:orig(id, s), port:port(id, s)});
  }); });
  return out;
}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(0x0043d2a0),'void',['int','int']);
    orig=new NativeFunction(abs(0x004282a0),'double',['uint32','float']);
    return DELTA;
  },
  load:function(path){
    const mod=Module.load(path);
    const e=mod.findExportByName('MenuMenusBA');
    port=new NativeFunction(e,'double',['uint32','float']);
    return mod.name+' base='+mod.base+' MenuMenusBA='+e+' (+0x'+e.sub(mod.base).toString(16)+')';
  },
  phase:function(){ return abs(0x0067eca4).readS32(); },
  push:function(scr){ nav(scr,0); abs(0x0067ecb0).writeS32(scr); return abs(0x0067e9f8).readS32(); },
  arm:function(ids, scales){
    const h=Interceptor.attach(abs(0x0042e3a0), { onEnter(){
      h.detach();
      result={d830_hex:abs(0x0067d830).readU32().toString(16), calls:run(ids, scales)};
    }});
    return 1;
  },
  result:function(){ return result===null ? '' : JSON.stringify(result); },
  offthread:function(ids, scales){ return JSON.stringify(run(ids, scales)); }
};
'''

def bits(x):
    return struct.pack('<d', x).hex() if x is not None else None

def main():
    out = Path(sys.argv[1]); asi = Path(sys.argv[2]).resolve()
    ids = [0x59, 0x1b2, 0x1b1, 0x5a, 0x25f, 0x36, 40, 1, 5, 20]
    scales = [0.7, 0.72, 0.6, 0.8, 0.9, 1.0, 8.0]
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    # subprocess + attach, as re/frida/scenario_launch.py does for private roots
    proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env); pid = proc.pid
    print("spawned PID", pid, flush=True)
    try:
        sess = None
        for _ in range(200):
            try: sess = dev.attach(pid); break
            except Exception: time.sleep(0.1)
        sess.on("detached", lambda reason, crash: print("detached:", reason, crash, "exit", proc.poll(), flush=True))
        scr = sess.create_script(AGENT); scr.on("message", lambda m, d: print(m)); scr.load()
        E = scr.exports_sync; E.init()
        end = time.time() + 25
        while time.time() < end and E.phase() != 3: time.sleep(0.2)
        time.sleep(1.5)
        print("load", E.load(str(asi)), flush=True)
        print("push 19 -> depth", E.push(19), flush=True)
        time.sleep(6)
        E.arm(ids, scales)
        end = time.time() + 10; r = ''
        while time.time() < end and not r: time.sleep(0.2); r = E.result()
        game = json.loads(r) if r else None
        off = json.loads(E.offthread(ids, scales))
        rows = []
        for where, calls in (("game", game["calls"] if game else []), ("offthread", off)):
            for c in calls:
                c["where"] = where; c["orig_bits"] = bits(c["orig"]); c["port_bits"] = bits(c["port"])
                c["equal"] = c["orig_bits"] == c["port_bits"]; rows.append(c)
        data = {"pid": pid, "asi": str(asi), "d830_hex": game and game["d830_hex"], "rows": rows,
                "game_calls": sum(1 for x in rows if x["where"] == "game"),
                "equal": sum(1 for x in rows if x["equal"]), "total": len(rows)}
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(data, indent=1))
        for x in rows:
            print(f'{x["where"]:9s} id=0x{x["id"]:x} s={x["scale"]:<5} orig={x["orig"]!r:24} port={x["port"]!r:24} {"EQ" if x["equal"] else "DIFF"}')
        print(f'equal {data["equal"]}/{data["total"]} (game {data["game_calls"]}) d830={data["d830_hex"]}', flush=True)
    finally:
        if proc.poll() is None: proc.kill(); print("killed PID", pid)
        else: print("PID", pid, "exited", proc.returncode)

if __name__ == "__main__":
    main()
