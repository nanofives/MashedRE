"""BIND-PROFILE-DATA: dump everything porttop_bind.py generates for one profile, without writing any file, so the binder
before and after the profile-data move can be compared byte for byte.

Run from a tree root (worktree or extracted branch tree):
  py -3.12 <this> <tree-root> <profile> <roots|plan> <roots_only 0|1> <out file>
roots = comma list, or `plan` = the plan keys no bound plan entry calls (the audio selftest rule).

Dumped: plan (as built), listing, every bind wrapper text, stops text, refs text, root_external selectors (writes
captured in memory), BINDINGS.tsv after `bindings` (restored), `sync --dry`, `samesrc`, `prune` (dry) stdout.
"""
import argparse, contextlib, hashlib, io, json, os, sys

root, prof, roots, ronly, out = sys.argv[1:6]
os.chdir(root)
sys.path.insert(0, os.path.join(root, 're', 'tools', 'bind'))
import porttop_bind as pb

cap = {}
real_wic = pb.write_if_changed


def fake_wic(fn, text):
    cap[fn.replace('\\', '/')] = text
    return True


pb.write_if_changed = fake_wic
if roots == 'plan':
    p = json.load(open(os.path.join(pb.state_dir(prof), 'plan.json')))
    called = {c for v in p.values() if v[0] == 'bind' for c in v[3] + v[4]}
    roots = ','.join(sorted(k for k in p if k not in called))
kw = {'roots_only': True} if ronly == '1' else {}
b = pb.Binder(prof, roots.split(','), **kw)
plan = b.build_plan()
O = []
O.append('== plan')
O.append(json.dumps({k: list(v) for k, v in sorted(plan.items())}, sort_keys=True, indent=0))
O.append('== listing')
O += b.listing()
for rva, p in sorted(plan.items()):
    if p[0] != 'bind':
        continue
    rx = b.P.get('root_external', {}) if isinstance(b.P, dict) else {}
    if rva in rx:
        cap.clear()
        saved = {}
        with contextlib.redirect_stdout(io.StringIO()):
            b.write_selectors(rva, rx[rva], [])
        for f, t in sorted(cap.items()):
            O.append('== selector-write %s' % f)
            O.append(t)
        continue
    fn, text = b.wrapper_text(rva)
    O.append('== wrapper %s' % fn)
    O.append(text)
st = b.stops_text()
O.append('== stops %s' % b.P['stop_file'])
O.append(st[1])
if hasattr(b, 'refs_text') and b.P.get('refs_file'):
    O.append('== refs')
    O.append(b.refs_text())
pb.write_if_changed = real_wic


def run(fn, ns):
    with contextlib.redirect_stdout(io.StringIO()) as o:
        try:
            fn(ns)
        except SystemExit as e:
            print('SystemExit', e)
        except Exception as e:
            print('EXC', type(e).__name__, e)
    return o.getvalue()


raw = open(pb.BINDINGS, 'rb').read()
try:
    O.append('== bindings stdout')
    O.append(run(pb.cmd_bindings, argparse.Namespace(profile=prof, step=None, dry=False)))
    O.append('== bindings file sha256 %s' % hashlib.sha256(open(pb.BINDINGS, 'rb').read()).hexdigest())
finally:
    open(pb.BINDINGS, 'wb').write(raw)
O.append('== sync --dry')
O.append(run(pb.cmd_sync, argparse.Namespace(profile=prof, step='X', date='2026-11-21', evidence='E', dry=True)))
P = b.P
O.append('== allow_text')
O.append(P['allow_text'] % dict(dict(P['allow_default']), step='X', date='D', evidence='E', tool='T'))
O.append('== bindings_roots %s root_caller %r tag %r prefix %r' % (json.dumps(P['bindings_roots']), P.get('bindings_root_caller'),
                                                                   P['bindings_tag'], P['stop_prefix']))
O.append('== samesrc')
O.append(run(pb.cmd_samesrc, argparse.Namespace(profile=prof)))
O.append('== prune')
O.append(run(pb.cmd_prune, argparse.Namespace(profile=prof, step=None, apply=False)))
open(out, 'w', encoding='utf-8', newline='\n').write('\n'.join(O) + '\n')
print(prof, 'plan', len(plan), 'dump', out, hashlib.sha256(open(out, 'rb').read()).hexdigest())
