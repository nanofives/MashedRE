"""BIND-00420420: MEASURED entry count of exe functions on a knob-on arm.

Spawns mashed_re.exe under Frida with the arm's environment (the env `A4_PLAN=1 run_arms.py <arm>` prints), cwd = the
private root, and counts entries of each named symbol at its address from the build's .map (Rva+Base column, rebased to
the loaded module). Cold functions only (each runs a handful of times per race load), so Interceptor overhead is not a
factor (CLAUDE.md "Frida overhead on hot paths"). The process runs to its own exit (MASHED_DET_FRAMES) or is killed by
PID at the timeout. Prints one line per symbol: "<symbol> <count>", plus the exit kind.

    py -3.12 verify/bindgates_20261127/tools/count_body.py <exe> <map> <root> <timeout_s> <out.txt> <sym,sym,...> KEY=VAL ...
Wrap it as test-slot (outside) + mashed_lock.py run (inside).
"""
import os
import re
import sys
import threading
import time

import frida


def map_syms(path, names):
    base, out = None, {}
    for ln in open(path, encoding='utf-8', errors='replace'):
        m = re.search(r'Preferred load address is ([0-9a-fA-F]+)', ln)
        if m:
            base = int(m.group(1), 16)
        f = ln.split()
        if len(f) >= 3 and f[1] in names and re.fullmatch(r'[0-9a-fA-F]{8}', f[2]):
            out[f[1]] = int(f[2], 16)
    return base, out


def main():
    exe, mp, root, tmo, outp, syms = sys.argv[1:7]
    names = syms.split(',')
    base, va = map_syms(mp, set(names))
    env = {}
    for kv in sys.argv[7:]:
        k, v = kv.split('=', 1)
        env[k] = v
    env['MASHED_ROOT'] = root
    lines = ['exe %s' % exe, 'map %s preferred base 0x%08x' % (mp, base)]
    for n in names:
        lines.append('symbol %s %s' % (n, ('va 0x%08x' % va[n]) if n in va else 'NOT IN MAP'))
    for k in sorted(env):
        lines.append('env %s=%s' % (k, env[k]))
    dev = frida.get_local_device()
    pid = dev.spawn([exe], env=env, cwd=root)
    sess = dev.attach(pid)
    done = threading.Event()
    state = {'detach': None, 'counts': {}}
    sess.on('detached', lambda reason, crash: (state.__setitem__('detach', (reason, crash)), done.set()))
    offs = {n: va[n] - base for n in names if n in va}
    js = """
var m = Process.enumerateModules()[0];
var offs = %s;
var counts = {};
Object.keys(offs).forEach(function (n) {
  counts[n] = 0;
  Interceptor.attach(m.base.add(offs[n]), { onEnter: function () { counts[n]++; send({n: n, c: counts[n]}); } });
});
send({ready: m.name, base: m.base.toString()});
""" % repr(offs).replace("'", '"')
    sc = sess.create_script(js)

    def on_msg(msg, data):
        if msg.get('type') == 'send':
            p = msg['payload']
            if 'ready' in p:
                lines.append('module %s base %s' % (p['ready'], p['base']))
            else:
                state['counts'][p['n']] = p['c']
        else:
            lines.append('script message %s' % str(msg)[:300])
    sc.on('message', on_msg)
    sc.load()
    t0 = time.time()
    dev.resume(pid)
    if not done.wait(float(tmo)):
        lines.append('timeout after %.0f s: killing own pid %d' % (time.time() - t0, pid))
        dev.kill(pid)
        done.wait(30)
    r = state['detach']
    lines.append('ended after %.1f s: detach reason %s%s' % (time.time() - t0, r[0] if r else None,
                                                             (' crash ' + str(r[1].summary)) if r and r[1] else ''))
    for n in names:
        lines.append('COUNT %s %d' % (n, state['counts'].get(n, 0)))
    open(outp, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
    print('\n'.join(lines), flush=True)


if __name__ == '__main__':
    main()
