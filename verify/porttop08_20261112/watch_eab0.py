"""PORT-TOP-08: hardware write-watch on DAT_0067eab0 in the worktree mashed_re.exe (knob path).

usage: py -3.12 verify/porttop08_20261112/watch_eab0.py <label> <root> <timeout_s> [KEY=VALUE ...]
Launches mashed_re.exe (env + MASHED_ROOT=<root>), attaches right away, arms a 4-byte write watchpoint on 0x0067eab0 on every
thread, resumes, and logs every write: pc, new value, and the exe-range return addresses found on the stack.
Kills only the PID it spawned (on timeout). Output: <PORTTOP_OUT>/<label>.watch.tsv and <label>.log (mashed_re.log).
WATCH_POKE=0x007e96fc=2[,addr=val...] writes dwords once at attach (diagnostic).
Symbolize with verify/porttop08_20261112/symbolize.py (mashedmod/build/mashed_re.map).
"""
import os
import shutil
import sys
import time

import frida

W = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
EXE = os.environ.get('KNOB_EXE', os.path.join(W, 'mashedmod', 'build', 'mashed_re.exe'))
OUT = os.environ.get('PORTTOP_OUT', 'D:/MashedWT/PORT-TOP-08_data')
WADDR = int(os.environ.get('WATCH_ADDR', '0x0067eab0'), 16)

AGENT = r'''
'use strict';
var WADDR = ptr(%d);
var mod = Process.enumerateModules()[0];
var LO = mod.base, HI = mod.base.add(mod.size);
var POKES = %s;   // [[addr, dword], ...] written once at attach (WATCH_POKE, diagnostic only)
POKES.forEach(function(pv){ ptr(pv[0]).writeU32(pv[1]); send({k:'poke', a:pv[0], v:pv[1]}); });
var armed = {};
function arm(t){ if (armed[t.id]) return; try { t.setHardwareWatchpoint(0, WADDR, 4, 'w'); armed[t.id]=1; }
  catch(e){ send({k:'armerr', tid:t.id, e:e.message}); } }
Process.enumerateThreads().forEach(arm);
send({k:'armed', n:Object.keys(armed).length, base:LO.toString(), size:mod.size});
var last = WADDR.readU32();
var n = 0;
Process.setExceptionHandler(function(d){
  if (d.type !== 'breakpoint' && d.type !== 'single-step') return false;
  var c = d.context;
  var v = WADDR.readU32();
  var bt = [];
  try { for (var k=0; k<96 && bt.length<12; k++){ var x = c.esp.add(k*4).readPointer();
      if (x.compare(LO) >= 0 && x.compare(HI) < 0) bt.push(x.toString()); } } catch(e){}
  n++;
  if (n <= 4000) send({k:'w', pc:c.pc.toString(), old:last, val:v, tid:Process.getCurrentThreadId(), bt:bt});
  last = v;
  return true;
});
'''


def main():
    label, root, tmo = sys.argv[1], sys.argv[2], float(sys.argv[3])
    env = dict(os.environ)
    env['MASHED_ROOT'] = root
    for kv in sys.argv[4:]:
        k, v = kv.split('=', 1)
        env[k] = v
    log = os.path.join(root, 'mashed_re.log')
    if os.path.exists(log):
        os.remove(log)
    os.makedirs(OUT, exist_ok=True)
    tsv = open(os.path.join(OUT, label + '.watch.tsv'), 'w', encoding='utf-8')
    tsv.write('seq\tt\tpc\told\tval\ttid\tbt\n')
    seq = [0]
    t0 = time.time()

    def on_msg(m, _data):
        if m['type'] != 'send':
            print('agent:', m, flush=True)
            return
        p = m['payload']
        if p['k'] == 'w':
            seq[0] += 1
            tsv.write('%d\t%.3f\t%s\t%d\t%d\t%d\t%s\n' % (seq[0], time.time() - t0, p['pc'], p['old'], p['val'],
                                                         p['tid'], ','.join(p['bt'])))
            tsv.flush()
        else:
            print('agent:', p, flush=True)

    import subprocess
    proc = subprocess.Popen([EXE], cwd=root, env=env)
    pid = proc.pid
    print('spawned pid', pid, flush=True)
    sess = None
    for _ in range(40):   # attach as soon as the process takes it (spawn-suspended attach times out on this exe)
        try:
            sess = frida.attach(pid)
            break
        except Exception as e:
            time.sleep(0.1)
    if sess is None:
        proc.kill()
        sys.exit('attach failed')
    pokes = [[int(a, 16), int(v, 0)] for a, v in (x.split('=') for x in os.environ.get('WATCH_POKE', '').split(',') if x)]
    sc = sess.create_script(AGENT % (WADDR, pokes))
    sc.on('message', on_msg)
    sc.load()
    print('attached after %.2f s' % (time.time() - t0), flush=True)
    how = 'exited'
    import psutil
    try:
        pr = psutil.Process(pid)
        pr.wait(timeout=tmo)
    except psutil.TimeoutExpired:
        pr.kill()
        how = 'TIMEOUT (killed own pid %d)' % pid
    except psutil.NoSuchProcess:
        pass
    time.sleep(0.5)
    tsv.close()
    dst = os.path.join(OUT, label + '.log')
    if os.path.exists(log):
        shutil.copyfile(log, dst)
    print('%s: %s after %.1f s, %d writes, log -> %s' % (label, how, time.time() - t0, seq[0], dst), flush=True)


if __name__ == '__main__':
    main()
