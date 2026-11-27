"""RO-AUDIOINIT-45D460-REACH-5: spawn one exe under Frida with a knob env, catch the first access violation
(address, registers, 16 stack words) and kill only that spawned PID. Run inside test-slot -> mashed_lock.py run.
usage: py -3.12 catch_av.py <label> <exe> <root> <timeout_s> [KEY=VALUE ...]"""
import json, os, sys, time
import frida

D = os.path.dirname(os.path.abspath(__file__))
JS = r'''
Process.setExceptionHandler(function (d) {
  if (d.type === 'system') return false;
  var c = d.context, st = [];
  try { for (var i = 0; i < 24; i++) st.push(c.esp.add(i * 4).readU32().toString(16)); } catch (e) {}
  var mm = d.memory ? {op: d.memory.operation, addr: d.memory.address.toString()} : null;
  var m = Process.findModuleByAddress(d.address);
  send({type: d.type, addr: d.address.toString(), mod: m ? m.name : null, base: m ? m.base.toString() : null,
        mem: mm, eax: c.eax.toString(), ebx: c.ebx.toString(), ecx: c.ecx.toString(), edx: c.edx.toString(),
        esi: c.esi.toString(), edi: c.edi.toString(), ebp: c.ebp.toString(), esp: c.esp.toString(), stack: st});
  return false;
});
rpc.exports = { threads: function () {
  var out = [];
  Process.enumerateThreads().forEach(function (t) {
    var bt = [];
    try { bt = Thread.backtrace(t.context, Backtracer.FUZZY).slice(0, 24).map(function (a) { return a.toString(); }); } catch (e) {}
    out.push({id: t.id, state: t.state, pc: t.context.pc.toString(), esp: t.context.sp.toString(), bt: bt});
  });
  return out;
}};
'''


def main():
    label, exe, root, tmo = sys.argv[1], sys.argv[2], sys.argv[3], float(sys.argv[4])
    env = {}
    for kv in sys.argv[5:]:
        k, v = kv.split('=', 1)
        env[k] = v
    env['MASHED_ROOT'] = root
    full = dict(os.environ)
    full.update(env)
    dev = frida.get_local_device()
    pid = dev.spawn([exe], cwd=root, env=full)
    print('spawned pid', pid, flush=True)
    s = dev.attach(pid)
    got = []
    sc = s.create_script(JS)
    sc.on('message', lambda m, data: got.append(m.get('payload', m)))
    sc.load()
    dev.resume(pid)
    t0 = time.time()
    while time.time() - t0 < tmo and not got:
        time.sleep(0.2)
    time.sleep(1.0)
    if not got:
        try:
            got.append({'type': 'timeout-threads', 'threads': sc.exports_sync.threads()})
        except Exception as e:
            got.append({'type': 'timeout-threads-error', 'err': str(e)})
    out = os.path.join(D, 'av_%s.json' % label)
    json.dump({'label': label, 'exe': exe, 'env': env, 'events': got, 'secs': round(time.time() - t0, 1)},
              open(out, 'w'), indent=1)
    print(json.dumps(got, indent=1)[:3000])
    try:
        dev.kill(pid)   # only the PID this script spawned
    except Exception as e:
        print('kill:', e)
    print('wrote', out)


if __name__ == '__main__':
    main()
