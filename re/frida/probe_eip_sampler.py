# Non-perturbing localizer for the timing-sensitive eip=0 crash: periodically
# sample the MASHED main thread's eip + a fuzzy return-address backtrace. The last
# MASHED-range eip before the AV is the function that performed the bad ret-to-0.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_eip_sampler.txt'

AGENT = r'''
'use strict';
var LO=0x401000, HI=0x995000;
function inM(p){ var v=p.toUInt32?p.toUInt32():(p>>>0); return v>=LO && v<HI; }
var ring=[];            // [{pc, bt:[...]}]
var RINGMAX=60;
var mainTid=null;

// pick the thread currently executing MASHED code as "main"
function pickMain(){
  var ts=Process.enumerateThreads();
  for (var i=0;i<ts.length;i++){
    try { if (inM(ts[i].context.pc)) return ts[i].id; } catch(e){}
  }
  return ts.length? ts[0].id : null;
}

var iv=setInterval(function(){
  var ts=Process.enumerateThreads();
  for (var i=0;i<ts.length;i++){
    var t=ts[i];
    var pc, esp;
    try { pc=t.context.pc; esp=t.context.esp; } catch(e){ continue; }
    var pcv=pc.toUInt32()>>>0;
    if (pcv>=LO && pcv<HI){
      var bt=[];
      try {
        for (var k=0;k<24 && bt.length<8;k++){
          var v=esp.add(k*4).readU32()>>>0;
          if (v>=LO && v<HI) bt.push('0x'+v.toString(16));
        }
      } catch(e){}
      ring.push({tid:t.id, pc:'0x'+pcv.toString(16), bt:bt});
      if (ring.length>RINGMAX) ring.shift();
    }
  }
}, 2);

var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  clearInterval(iv);
  var c=d.context;
  send({kind:'crash', type:d.type, eip:c.pc.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        ecx:c.ecx.toString(), ebx:c.ebx.toString(),
        tail: ring.slice(-30)});
  return false;
});
send({kind:'ready'});
'''

def find_pid():
    for p in psutil.process_iter(['name','pid']):
        try:
            if p.info['name'] and p.info['name'].lower()=='mashed.exe': return p.info['pid']
        except Exception: pass
    return None

def main():
    print("waiting for MASHED.exe — launch now")
    dl=time.time()+120; pid=None
    while time.time()<dl:
        pid=find_pid()
        if pid: print("  pid=%d"%pid); break
        time.sleep(0.02)
    if not pid: sys.exit("timeout")
    sess=frida.get_local_device().attach(pid); print("  attached")
    ev=[]
    def on(m,_):
        if m['type']=='error': print("  AGENT ERR:",m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='ready': print("  sampling main-thread eip @2ms")
        elif k=='crash':
            print(f"  === CRASH {p['type']} eip={p['eip']} mem={p['mem']} ecx={p['ecx']} ebx={p['ebx']} ===")
            print("  last MASHED-thread samples before crash (oldest->newest):")
            for s in p['tail']:
                print(f"    tid={s['tid']} pc={s['pc']}  bt={s['bt']}")
            ev.append(p)
    sc=sess.create_script(AGENT); sc.on('message',on); sc.load()
    dl=time.time()+90
    while time.time()<dl:
        if any(e.get('kind')=='crash' for e in ev): time.sleep(0.3); break
        try:
            if not psutil.pid_exists(pid): print("  gone"); break
        except Exception: pass
        time.sleep(0.2)
    try: sess.detach()
    except Exception: pass
    OUT.write_text(json.dumps(ev,indent=2),encoding='utf-8'); print("  wrote",OUT)

if __name__=='__main__': main()
