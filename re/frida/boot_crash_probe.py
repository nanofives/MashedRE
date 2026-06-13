#!/usr/bin/env py -3.12
"""boot_crash_probe.py — spawn MASHED, arm an exception handler, report WHERE the
boot AV happens (module + offset + a module-classified stack trace).

Baseline mode (default): MASHED_RE_NO_AUTO_HOOK=1 so the .asi loads but installs
NO inline-JMP hooks -> the crash is pure MASHED + d3d9 shim + driver, isolating
the environmental/D3D9-init wedge from our reimpls.

Usage: py -3.12 re/frida/boot_crash_probe.py
"""
import os, sys, time, json, frida

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.path.join(ROOT, "original", "MASHED.exe")

AGENT = r'''
'use strict';
function modTable() {
  return Process.enumerateModules().map(function(m){
    return {name:m.name, base:m.base, end:m.base.add(m.size)};
  });
}
function classify(p, mods) {
  for (var i=0;i<mods.length;i++){ if (p.compare(mods[i].base)>=0 && p.compare(mods[i].end)<0)
    return mods[i].name + '+0x' + p.sub(mods[i].base).toString(16); }
  return p.toString();
}
const FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  const mods=modTable();
  const ctx=d.context;
  const out={type:d.type, addr:d.address.toString(), eip:classify(ptr(d.context.pc.toString()), mods),
             memory:d.memory?d.memory:null, regs:{}, stack:[]};
  ['eax','ebx','ecx','edx','esi','edi','ebp','esp','eip'].forEach(function(r){
    try{ out.regs[r]=ctx[r].toString(); }catch(e){}
  });
  try {
    const esp=ptr(ctx.esp.toString());
    for(let i=0;i<200;i++){
      const v=esp.add(i*4).readPointer();
      const c=classify(v,mods);
      if(c.indexOf('MASHED.exe')===0 || c.indexOf('mashed_re')===0 || c.indexOf('d3d9')===0)
        out.stack.push('[esp+0x'+(i*4).toString(16)+'] '+c);
      if(out.stack.length>=24) break;
    }
  } catch(e){}
  send({crash:out});
  return false; // let it die after we report
});
send({armed:true});
'''

def main():
    env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
    pid = frida.spawn([EXE], cwd=os.path.join(ROOT, 'original'), env=env)
    session = frida.attach(pid)
    got = {}
    def on_msg(m, data):
        if m.get('type')=='send':
            p=m['payload']
            if p.get('armed'): print('  exception handler armed')
            if p.get('crash'):
                got['crash']=p['crash']
                print('\n=== CRASH ===')
                print(json.dumps(p['crash'], indent=2))
    sc = session.create_script(AGENT)
    sc.on('message', on_msg)
    sc.load()
    frida.resume(pid)
    # wait up to 15s for the crash
    for _ in range(150):
        if got.get('crash'): break
        time.sleep(0.1)
    if not got.get('crash'):
        print("  no crash caught within 15s (process may have survived or died silently)")
    try: frida.kill(pid)
    except Exception: pass

if __name__ == '__main__':
    main()
