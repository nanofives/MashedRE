# Hardware write-watchpoint hunt for the crash #2 stack smasher. The crash is a
# ret-to-0 with a stable main-thread esp=0x1afe50, so the smashed return slot is
# ~0x1afe4c. Watch 0x1afe44/48/4c for WRITE; the instruction that stores 0 there
# is the smasher. Reports its pc + a fuzzy backtrace.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_watchpoint.txt'

AGENT = r'''
'use strict';
function hx(v){ return '0x'+(v>>>0).toString(16); }
var LO=0x401000, HI=0x995000;
var WADDR = ptr(0x1afe4c);          // single slot: the smashed return-address slot
var firstExcType = null;
var writes = [];                    // ring of recent writes to 0x1afe4c

function armThread(t){
  try { t.setHardwareWatchpoint(0, WADDR, 4, 'w'); }
  catch(e){ send({kind:'armerr', tid:t.id, err:e.message}); }
}
var armedTids = [];
Process.enumerateThreads().forEach(function(t){ armThread(t); armedTids.push(t.id); });
send({kind:'armed', tids: armedTids});

function btOf(esp){
  var bt=[]; try { for(var k=0;k<28 && bt.length<10;k++){ var x=esp.add(k*4).readU32()>>>0; if(x>=LO&&x<HI) bt.push('0x'+x.toString(16)); } } catch(e){}
  return bt;
}

var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  var c=d.context;
  if (firstExcType===null){ firstExcType=d.type; send({kind:'note', t:'first exception type = '+d.type}); }

  if (d.type==='breakpoint' || d.type==='single-step'){
    // every fire IS a write to 0x1afe4c. record pc + new value.
    var v; try{ v=WADDR.readU32()>>>0; }catch(e){ return true; }
    var rec = {pc:c.pc.toString(), val:hx(v), esp:c.esp.toString(),
               ecx:c.ecx.toString(), ebx:c.ebx.toString(), edi:c.edi.toString(),
               bt:btOf(c.esp)};
    writes.push(rec);
    if (writes.length>40) writes.shift();
    // flag writes that store something OTHER than a valid MASHED return addr
    if (!(v>=LO && v<HI))
      send({kind:'write', pc:c.pc.toString(), val:hx(v), esp:c.esp.toString(),
            ecx:c.ecx.toString(), ebx:c.ebx.toString(), bt:btOf(c.esp), n:writes.length});
    return true;
  }
  if (FATAL[d.type]){
    send({kind:'crash', type:d.type, eip:c.pc.toString(),
          mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
          ecx:c.ecx.toString(), ebx:c.ebx.toString(),
          tailWrites: writes.slice(-12)});
    return false;
  }
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
        if k=='armed': print("  watchpoints armed on tids:",p['tids'])
        elif k=='armerr': print("  ARM ERR tid=%s id=%s: %s"%(p['tid'],p['id'],p['err']))
        elif k=='note': print("  NOTE:",p['t'])
        elif k=='ready': print("  watching for the 0-write to the return slot")
        elif k=='write':
            print(f"  WRITE-to-0x1afe4c #{p['n']} pc={p['pc']} val={p['val']} esp={p['esp']} ecx={p['ecx']} ebx={p['ebx']} bt={p['bt']}")
            ev.append(p)
        elif k=='crash':
            print(f"  === CRASH {p['type']} eip={p['eip']} mem={p['mem']} ===")
            print("  last writes to 0x1afe4c (oldest->newest):")
            for w in p.get('tailWrites', []):
                print(f"    pc={w['pc']} val={w['val']} esp={w['esp']} ecx={w['ecx']} ebx={w['ebx']} edi={w['edi']} bt={w['bt']}")
            ev.append(p)
    sc=sess.create_script(AGENT); sc.on('message',on); sc.load()
    dl=time.time()+90
    while time.time()<dl:
        if any(e.get('kind')=='crash' for e in ev): time.sleep(0.5); break
        try:
            if not psutil.pid_exists(pid): print("  gone"); break
        except Exception: pass
        time.sleep(0.2)
    try: sess.detach()
    except Exception: pass
    OUT.write_text(json.dumps(ev,indent=2),encoding='utf-8'); print("  wrote",OUT)

if __name__=='__main__': main()
