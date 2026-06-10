# Call-stack tracer for the hooks-ON 0x44 crash. Hooks entry+exit of FUN_005a7b60
# and its early callees; maintains a shadow call stack. On the AV, dumps the
# current stack — the deepest entered-but-not-returned function contains the fault.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_callstack.txt'

# name -> VA  (FUN_005a7b60 and the functions it calls before 0x5a7c17, plus a few deeper)
FUNCS = {
    'FUN_005a7b60': 0x5a7b60,
    'FUN_005ab380': 0x5ab380,
    'FUN_005aea00': 0x5aea00,
    'FUN_004cbd30': 0x4cbd30,
    'FUN_004522d0': 0x4522d0,
    'FUN_004cc050': 0x4cc050,
    'FUN_005a7b50': 0x5a7b50,
    'FUN_005a7b40': 0x5a7b40,
    'FUN_00550950': 0x550950,   # called inside 0x4cbd30 (stream read primitive)
    'FUN_00550af0': 0x550af0,   # called inside 0x4cbd30
}

AGENT = r'''
'use strict';
var FUNCS = %s;
var stack = [];   // [{name, retaddr}]
Object.keys(FUNCS).forEach(function(name){
  var va = ptr(FUNCS[name]);
  Interceptor.attach(va, {
    onEnter: function(){ stack.push({n:name, ra:this.returnAddress.toString()}); },
    onLeave: function(){ for (var i=stack.length-1;i>=0;i--){ if(stack[i].n===name){ stack.splice(i,1); break; } } }
  });
});
send({kind:'hooked', n:Object.keys(FUNCS).length});
var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', eip:c.pc.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        eax:c.eax.toString(), ecx:c.ecx.toString(), edi:c.edi.toString(),
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
        elif k=='ready': print("  watching")
        elif k=='crash':
            print(f"  === CRASH eip={p['eip']} mem={p['mem']} eax={p['eax']} ecx={p['ecx']} ===")
            print("  shadow call stack at crash (outer->inner):")
            if not p['stack']: print("    <empty — crash not under any hooked function>")
            for f in p['stack']: print(f"    {f['n']}  (called from {f['ra']})")
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
