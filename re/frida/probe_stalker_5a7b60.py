# Stalker block-tracer: follow execution from FUN_005a7b60 entry and record the
# last basic blocks executed. When the AV fires (jmp/ret to 0x44), the most
# recent recorded block contains the faulting transfer instruction.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_stalker_5a7b60.txt'

AGENT = r'''
'use strict';
var MX_LO=ptr(0x401000), MX_HI=ptr(0x995000);
var ring = [];           // last block-start addresses (original VAs)
var RINGMAX = 40;
var following = false;

function inMashed(p){ return p.compare(MX_LO) >= 0 && p.compare(MX_HI) < 0; }

Interceptor.attach(ptr(0x5a7b60), { onEnter: function(){
  if (following) return;          // only follow the first entry
  following = true;
  var tid = this.threadId;
  Stalker.follow(tid, {
    transform: function(iterator){
      var insn = iterator.next();
      if (insn !== null){
        var start = insn.address;
        // record only blocks inside MASHED.exe to keep the ring meaningful
        if (inMashed(start)){
          iterator.putCallout(function(){
            ring.push(start.toString());
            if (ring.length > RINGMAX) ring.shift();
          });
        }
        iterator.keep();
        while ((insn = iterator.next()) !== null) iterator.keep();
      }
    }
  });
  send({kind:'following', tid: tid});
}});
send({kind:'hooked'});

var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', type:d.type, address:d.address.toString(), eip:c.pc.toString(),
        eax:c.eax.toString(), edi:c.edi.toString(), ecx:c.ecx.toString(),
        esi:c.esi.toString(), esp:c.esp.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        ring: ring.slice(-24)});
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
        time.sleep(0.05)
    if not pid: sys.exit("timeout")
    sess=frida.get_local_device().attach(pid); print("  attached")
    ev=[]
    def on(m,_):
        if m['type']=='error': print("  AGENT ERR:",m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='hooked': print("  FUN_005a7b60 hooked")
        elif k=='following': print("  Stalker following tid=%s"%p['tid'])
        elif k=='ready': print("  watching")
        elif k=='crash':
            print(f"  === CRASH eip={p['eip']} mem={p['mem']} eax={p['eax']} ecx={p['ecx']} esi={p['esi']} ===")
            print("  last blocks (oldest->newest):")
            for a in p['ring']: print("    ", a)
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
