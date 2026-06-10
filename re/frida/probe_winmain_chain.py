# Call-stack tracer for the baseline eip=0 crash in the RW WinMain/AppInit chain.
# Hooks entry+exit of the WinMain chain and AppInitialiseOnBootup's major callees;
# the deepest entered-but-not-returned function localizes the fault.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_winmain_chain.txt'

FUNCS = {
    'entry_004a4bb7':         0x4a4bb7,
    'WinMainChain_00492370':  0x492370,
    'SubsystemInit_00492270': 0x492270,
    'AppInitBootup_00402750': 0x402750,
    'MainLoop_00492290':      0x492290,
    'AppDestroy_00402a40':    0x402a40,
    'FUN_004caea0':           0x4caea0,
    'FUN_004d8560':           0x4d8560,
    'FUN_004b6540':           0x4b6540,
    'FUN_004b6560':           0x4b6560,
    'FUN_00496e40':           0x496e40,
    'FUN_004caea0_dup':       0x4caea0,
}

AGENT = r'''
'use strict';
var FUNCS = %s;
var stack = [];
Object.keys(FUNCS).forEach(function(name){
  try {
    Interceptor.attach(ptr(FUNCS[name]), {
      onEnter: function(){ stack.push({n:name, ra:this.returnAddress.toString()}); },
      onLeave: function(){ for (var i=stack.length-1;i>=0;i--){ if(stack[i].n===name){ stack.splice(i,1); break; } } }
    });
  } catch(e){ send({kind:'hookerr', name:name, err:e.message}); }
});
send({kind:'hooked', n:Object.keys(FUNCS).length});
var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', eip:c.pc.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        eax:c.eax.toString(), ecx:c.ecx.toString(), ebx:c.ebx.toString(), esi:c.esi.toString(),
        stack: stack.slice()});
  return false;
});
send({kind:'ready'});
''' % json.dumps(FUNCS)

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
        if k=='hooked': print("  hooked %d funcs"%p['n'])
        elif k=='hookerr': print("  HOOK ERR %s: %s"%(p['name'],p['err']))
        elif k=='ready': print("  watching")
        elif k=='crash':
            print(f"  === CRASH eip={p['eip']} mem={p['mem']} eax={p['eax']} ecx={p['ecx']} ebx={p['ebx']} ===")
            print("  shadow call stack (outer->inner):")
            if not p['stack']: print("    <empty>")
            for f in p['stack']: print(f"    {f['n']}  (from {f['ra']})")
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
