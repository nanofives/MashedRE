# Read-only probe: per-frame [0x0067e7f8] mode / [0x0067e7fc] alpha around a push of screen 19.
# Uses menu_draw_burst's own agent (init/phase/push) + a second script that only READS.
import sys, time, json, os
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\re\frida")
import frida
import menu_draw_burst as M
PROBE = r'''
const IMG=0x00400000; let D=0; let log=[]; let n=0; let h=null;
function a(r){return ptr(r+D);}
rpc.exports={
 init:function(){ const m=Process.findModuleByName('MASHED.exe'); D=m.base.toUInt32()-IMG; return D; },
 arm:function(){ n=0; log=[]; h=Interceptor.attach(a(0x0042e3a0),{onEnter(){ n++;
   if(log.length<400) log.push([n, a(0x0067e7f8).readS32(), a(0x0067e7fc).readS32(), a(0x0067ecb0).readS32(), a(0x0067e9f8).readS32(), a(0x0067e840).readFloat()]); }}); return 1; },
 stop:function(){ if(h){h.detach(); h=null;} return JSON.stringify(log); }
};'''
env = dict(os.environ); env.setdefault("MASHED_RE_NO_AUTO_HOOK", "1")
dev = frida.get_local_device()
pid = dev.spawn(str(M.EXE), cwd=str(M.ORIG), env=env)
print("spawned pid", pid, flush=True)
s = dev.attach(pid)
A = s.create_script(M.AGENT); A.load(); A.exports_sync.init()
P = s.create_script(PROBE); P.load(); P.exports_sync.init()
dev.resume(pid)
E = A.exports_sync
end = time.time()+25
while time.time() < end and E.phase() != 3: time.sleep(0.2)
time.sleep(1.5)
P.exports_sync.arm(); time.sleep(0.5)
print("push 19 -> depth", E.push(19), flush=True)
time.sleep(3.0)
log = json.loads(P.exports_sync.stop())
out = sys.argv[1]
open(out, "w").write(json.dumps(log))
prev=None
for r in log:
    k=tuple(r[1:])
    if k!=prev: print(r)
    prev=k
print("frames", len(log))
dev.kill(pid)
