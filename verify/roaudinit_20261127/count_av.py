"""RO-AUDIOINIT-45D460-REACH-5 (count variant): entry counters on named exe symbols (map file) plus the AV catch.
Spawn one exe under Frida with a knob env, catch the first access violation
(address, registers, 16 stack words) and kill only that spawned PID. Run inside test-slot -> mashed_lock.py run.
usage: py -3.12 count_av.py <label> <exe> <map> <root> <timeout_s> [KEY=VALUE ...]   (symbols: COUNT_SYMS env, comma list)"""
import json, os, sys, time
import frida

D = os.path.dirname(os.path.abspath(__file__))
JS = r'''
var CNT = {};
function arm(list) { list.forEach(function (p) { CNT[p[0]] = 0; Interceptor.attach(ptr(p[1]), { onEnter: function () { CNT[p[0]]++; } }); }); }
Process.setExceptionHandler(function (d) {
  if (d.type === 'system') return false;
  var c = d.context, st = [];
  try { for (var i = 0; i < 24; i++) st.push(c.esp.add(i * 4).readU32().toString(16)); } catch (e) {}
  var mm = d.memory ? {op: d.memory.operation, addr: d.memory.address.toString()} : null;
  var m = Process.findModuleByAddress(d.address);
  send({type: d.type, addr: d.address.toString(), mod: m ? m.name : null, base: m ? m.base.toString() : null,
        mem: mm, eax: c.eax.toString(), ebx: c.ebx.toString(), ecx: c.ecx.toString(), edx: c.edx.toString(),
        esi: c.esi.toString(), edi: c.edi.toString(), ebp: c.ebp.toString(), esp: c.esp.toString(), stack: st, counts: CNT});
  return false;
});
rpc.exports = { arm: function (l) { arm(l); return Object.keys(CNT).length; }, counts: function () { return CNT; }, threads: function () {
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
    label, exe, mapf, root, tmo = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], float(sys.argv[5])
    import re
    want = [x for x in os.environ.get('COUNT_SYMS', '').split(',') if x]
    addr = {}
    for ln in open(mapf, errors='replace'):
        m = re.match(r'\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})', ln)
        if m and m.group(1).lstrip('_') in want:
            addr[m.group(1).lstrip('_')] = '0x' + m.group(2)
    print('symbols', addr, flush=True)
    env = {}
    for kv in sys.argv[6:]:
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
    print('armed', sc.exports_sync.arm([[k, v] for k, v in addr.items()]), flush=True)
    dev.resume(pid)
    t0 = time.time()
    while time.time() - t0 < tmo and not got:
        time.sleep(0.2)
    time.sleep(1.0)
    try:
        got.append({'type': 'final-counts', 'counts': sc.exports_sync.counts()})
    except Exception as e:
        pass
    if not [g for g in got if g.get('type') != 'final-counts']:
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
