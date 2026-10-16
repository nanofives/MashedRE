"""Original-side spark-burst counter. Attach to an ORIGINAL pid started by scenario_launch.py (own pid, passed in).
Counts calls of FUN_004864f0 (burst spawn, hooks.csv 004864f0) and FUN_00486370 (per-particle spawn into the spark ring
buffer) with ENTRY hooks that only increment (no memory reads), sampled every 5 s. Usage: orig_fx_count.py <pid> <secs> <out.json>"""
import json, sys, time, frida
pid, secs, out = int(sys.argv[1]), float(sys.argv[2]), sys.argv[3]
SRC = r'''
const IMG=0x400000; const m=Process.findModuleByName('MASHED.exe'); const d=m.base.toUInt32()-IMG;
const C={burst:0,spawn:0,wrap4:0};
Interceptor.attach(ptr(0x004864f0+d),{onEnter(){C.burst++;}});
Interceptor.attach(ptr(0x00486370+d),{onEnter(){C.spawn++;}});
Interceptor.attach(ptr(0x00486610+d),{onEnter(){C.wrap4++;}});
rpc.exports={c:function(){return JSON.stringify(C);}, ph:function(){return ptr(0x00771968+d).readU8();}};
'''
dev = frida.get_local_device(); s = dev.attach(pid); sc = s.create_script(SRC); sc.load(); E = sc.exports_sync
rows = []; t0 = time.time()
while time.time() - t0 < secs:
    time.sleep(5); rows.append({'t': round(time.time() - t0), 'phase': E.ph(), **json.loads(E.c())}); print(rows[-1], flush=True)
open(out, 'w').write(json.dumps(rows, indent=1))
