"""BIND-PROFILE-DATA: compare the rule tables an old porttop_bind.py held in code with the tables the new one loads from
re/tools/bind/state/. Run from the worktree root:
  py -3.12 verify/bindprofile_20261121/tools/cmp_tables.py <old porttop_bind.py> [<old2> ...]
Every PROFILES key the old file defines must load equal (sets / tuples normalised); the shared tables (PROV_BIND,
EXTRA_PRE, EXTRA_IN, LOCAL_SHARED, LOCAL_AS_BOUND, ALIAS) must be equal or a superset that adds only the rvas another
old file defines."""
import importlib.util, os, sys

sys.path.insert(0, 're/tools/bind')
import porttop_bind as new


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


def norm(v):
    if isinstance(v, (set, frozenset)):
        return sorted(norm(x) for x in v)
    if isinstance(v, (list, tuple)):
        return [norm(x) for x in v]
    if isinstance(v, dict):
        return {str(k): norm(x) for k, x in sorted(v.items(), key=lambda kv: str(kv[0]))}
    return v


bad = 0
olds = [load(p, 'old%d' % i) for i, p in enumerate(sys.argv[1:])]
shared_names = ('PROV_BIND', 'EXTRA_PRE', 'EXTRA_IN', 'LOCAL_SHARED', 'LOCAL_AS_BOUND', 'ALIAS')
for nm in shared_names:
    union = None
    for o in olds:
        v = getattr(o, nm)
        if isinstance(v, dict):
            union = dict(union or {}); union.update(v)
        else:
            union = set(union or set()) | set(v)
    ok = norm(union) == norm(getattr(new, nm))
    print('%-15s %s' % (nm, 'EQUAL (union of the old files)' if ok else 'DIFFERENT'))
    if not ok:
        bad += 1
        print('   old', norm(union)); print('   new', norm(getattr(new, nm)))
for o, path in zip(olds, sys.argv[1:]):
    for prof, P in o.PROFILES.items():
        N = new.PROFILES[prof]
        for k, v in P.items():
            nv = N.get(k)
            if k == 'own_mark' and v is None:
                pass
            same = norm(v) == norm(nv)
            if not same:
                bad += 1
                print('DIFF %s %s %s\n   old %r\n   new %r' % (os.path.basename(path), prof, k, norm(v), norm(nv)))
        print('%s %s: %d keys compared' % (os.path.basename(path), prof, len(P)))
print('RESULT', 'EQUAL' if not bad else 'DIFFERENT %d' % bad)
