#!/usr/bin/env py -3.12
"""run_negctl.py — prove each Frida A/B handler (arg_type) can read RED.

Port of SimGolf's negative-control discipline (shim/src/re/*_negctl.cpp), made generic:
instead of a hand-written wrong body per function, the dev .asi carries ONE mutation
forwarder (mashedmod/src/mashed_re/Harness/NegCtl.cpp) that wraps the REAL port and
breaks it in exactly one way. For every registry entry backing a C3/C4 row this runs:

  real       the normal path1 diff (must be GREEN, else the row's own evidence is stale)
  baseline   original vs original (run_diff --self-baseline; must be GREEN, else the
             verifier is broken — memory verifier-needs-passing-baseline)
  null       port never called, returns 0 (0.0 in ST0)
  ret        EAX ^= 1  (ret_int)  or  ST0 = ST0*2 + 0.75  (ret_float); skipped for void
  field_first / field_last
             XOR 0x5A into the first / last byte the port changed (arg windows, then
             MASHED.exe .data minus the game's own per-frame writes; the game's threads are
             frozen for the snapshot .. diff span)
  field_none the CONTROL for the two above: same freeze + snapshot + diff, no flip. RED here
             means the vector is sensitive to the freeze latency (the game rewrote a seeded
             global meanwhile) and that hook's field verdicts are reported NA
  heap_extra one extra game-CRT _malloc(0x20)  (booted lane only)
  import_extra one extra kernel32!GetTickCount through the .asi IAT

Verdict per mutant: RED (handler detects it), BLIND (stays GREEN or degenerate),
NA (field class and the port changed no byte the kit can see, so nothing was flipped),
SKIP (ret on a void signature; heap_extra in the early-window lane), ERROR.

Lanes:
  warm   arg_types handled by diff_template.js: a pool of booted MASHED instances
         (MASHED_RE_NO_AUTO_HOOK=1, MASHED_NEGCTL=1), many jobs per process, a dead
         instance is respawned. Hooks with scenario='race' are skipped (listed).
  early  arg_types that only early_window_leaf_diff.py handles: one suspended spawn per
         job via that script (--negctl / --self-baseline), MASHED_EW_ASI = this build.

Negative-control ports are NEVER listed in batch files and these CSVs are never
promotion evidence: they land in <out>/csv/, not log/diff_<hook>.csv.

Usage:
  py -3.12 re/frida/run_negctl.py --inventory                  # rank handlers, no runs
  py -3.12 re/frida/run_negctl.py --top 15 [--per-handler K] [--slots 4] [--out DIR]
  py -3.12 re/frida/run_negctl.py --handlers int_scalar,none [--hooks a,b]
Kills only the PIDs it spawns.
"""
import argparse, csv, json, os, random, re, subprocess, sys, threading, time
from collections import Counter, defaultdict
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
import run_diff                                            # noqa: E402
from hooks_registry import HOOKS                           # noqa: E402

AGENT_JS = run_diff.AGENT_JS
ASI_PATH = run_diff.ASI_PATH
MASHED_EXE = run_diff.MASHED_EXE
EW_SCRIPT = ROOT / 're' / 'frida' / 'early_window_leaf_diff.py'
CLASSES = ('null', 'ret', 'field_none', 'field_first', 'field_last', 'heap_extra', 'import_extra')
HOOK_TIMEOUT_S = 60.0
STATS_GRACE_S = 0.7

# Known false-GREEN history per handler (risk weight 1..3), from memory notes and trackers.
RISK = {
    'none': (3, 'pushes no args, return-only observable (ARG_TYPES hazard note); void targets match trivially'),
    'void': (3, 'return-only observable; void targets match trivially'),
    'read_global': (2, 'pushes no args; return-only observable'),
    'entity_field_set': (3, 'DEC-6 per-side sentinel history'),
    'early_window_scattered': (3, 'ignores args; degenerate GREEN on fn(slot) (memory early-window-scattered-handler-ignores-args)'),
    'void_write_observe': (2, 'sentinel pre-write, no restore; crash_equal_ok counts identical crashes as pass'),
    'void_setter_observe': (2, 'one target_global only'),
    'range_init': (2, 'return value not observed'),
    'near_leaf_seed_multi_obs': (2, 'return value not observed'),
    'global_field_read': (2, 'return-only observable; side writes undetected'),
    'indexed_table_set': (2, 'one slot observed, index fixed'),
    'int_scalar': (1, 'return-only observable'),
    'int_pair': (1, 'return-only observable'),
    'float_scalar': (1, 'return-only observable'),
}


def load_rows():
    conf = {}
    with open(ROOT / 'hooks.csv', encoding='utf-8') as fh:
        for r in csv.DictReader(fh):
            try:
                conf[int(r['rva'], 16)] = (r['confidence'].strip(), r['name'], r['frida_diff'], r['notes'])
            except (ValueError, TypeError):
                pass
    return conf


def early_only_argtypes():
    src = EW_SCRIPT.read_text(encoding='utf-8')
    m = re.search(r'PURE_LEAF_ARGTYPES\s*=\s*\{(.*?)\n\}', src, re.S)
    ew = set(re.findall(r"^\s*'([a-z0-9_]+)'", m.group(1), re.M)) if m else set()
    dt = AGENT_JS.read_text(encoding='utf-8')
    return {a for a in ew if f"'{a}'" not in dt}


def inventory():
    conf = load_rows()
    by_at = defaultdict(list)
    for k, h in HOOKS.items():
        c = conf.get(h['rva'])
        if c and c[0] in ('C3', 'C4'):
            by_at[h.get('arg_type')].append(k)
    ranked = sorted(by_at.items(), key=lambda kv: -(len(kv[1]) * RISK.get(kv[0], (1, ''))[0]))
    return ranked, conf


def census_exposure():
    """rva -> list of static heap/import calls within depth 2 (re/coverage/standin_census_d2.tsv)."""
    out = {}
    p = ROOT / 're' / 'coverage' / 'standin_census_d2.tsv'
    if not p.exists():
        return out
    with p.open(encoding='utf-8') as fh:
        for r in csv.DictReader(fh, delimiter='\t'):
            cols = ('crt_alloc', 'crt_free', 'rw_alloc', 'rw_free', 'imp_heap', 'imp_file', 'imp_other')
            hit = [c for c in cols if (r.get(c) or '').strip()]
            if hit:
                out[int(r['rva'], 16)] = hit
    return out


# ---------------------------------------------------------------- verdicts

def classify(mode, results, err, stats, hook):
    """Return (verdict, mismatches, total)."""
    if err or not results:
        return f'ERROR', 0, 0
    total = len(results)
    mism = sum(1 for r in results if not r.get('match'))
    if mism:
        if mode == 'field_none':
            return 'CTRL-RED', mism, total
        if mode in ('field_first', 'field_last') and re.search(r'flips=0\b', stats or ''):
            return 'NA', mism, total        # nothing flipped: a RED here is the freeze latency, not detection
        return 'RED', mism, total
    all_trivial = all(run_diff.is_trivial(r.get('original')) and run_diff.is_trivial(r.get('reimpl'))
                      for r in results)
    degenerate = all_trivial and not (hook.get('arg_type') in run_diff.SEEDED_ARG_TYPES or hook.get('degenerate_ok'))
    if mode in ('real', 'baseline'):
        return ('DEGENERATE' if degenerate else 'GREEN'), 0, total
    if mode in ('field_first', 'field_last'):
        m = re.search(r'flips=(\d+)', stats or '')
        if m and int(m.group(1)) == 0:
            return 'NA', 0, total
    if mode == 'field_none':
        return 'CTRL-GREEN', 0, total
    return ('BLIND-DEGENERATE' if degenerate else 'BLIND'), 0, total


def mode_to_negctl(mode, hook):
    if mode == 'ret':
        ret = hook['signature']['ret']
        if ret == 'void':
            return None
        return 'ret_float' if ret in ('float', 'double') else 'ret_int'
    return mode


# ---------------------------------------------------------------- warm lane

class Slot:
    def __init__(self, idx):
        self.idx, self.proc, self.session = idx, None, None
        self.spawn()

    def spawn(self):
        env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1', 'MASHED_NEGCTL': '1'}
        self.proc = subprocess.Popen([str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
                                     stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
                                     stderr=subprocess.DEVNULL, creationflags=0x00000200 | 0x00000008)
        time.sleep(0.4)
        try:
            self.session = frida.get_local_device().attach(self.proc.pid)
        except Exception as e:
            print(f'  [slot {self.idx}] attach failed: {e}')
            self.session = None
        print(f'  [slot {self.idx}] pid={self.proc.pid}')

    def alive(self):
        return self.proc.poll() is None and self.session is not None and not getattr(self.session, 'is_detached', False)

    def kill(self):
        try:
            if self.session is not None:
                self.session.detach()
        except Exception:
            pass
        try:
            self.proc.kill(); self.proc.wait(timeout=3)
        except Exception:
            pass

    def respawn(self):
        self.kill()
        time.sleep(1.0)
        self.spawn()

    def restore(self):
        """NegCtl_Restore: undo a pending field flip before the next job."""
        src = ("const m = Module.load('%s'); const f = m.findExportByName('NegCtl_Restore');"
               "send(f ? new NativeFunction(f, 'int', [], 'mscdecl')() : -1);") % str(ASI_PATH).replace('\\', '\\\\')
        got = []
        try:
            sc = self.session.create_script(src)
            sc.on('message', lambda m, d: got.append(m.get('payload')))
            sc.load(); time.sleep(0.2); sc.unload()
        except Exception:
            pass
        return got[0] if got else None

    def run(self, name, mode):
        hook = HOOKS[name]
        config = run_diff.build_config(hook)
        if mode == 'baseline':
            config['self_baseline'] = True
        elif mode != 'real':
            config['negctl'] = mode_to_negctl(mode, hook)
        results, err, stats, done = [], {'msg': None}, {'s': None}, threading.Event()

        def on_message(message, data):
            if message.get('type') == 'error':
                err['msg'] = (message.get('description') or 'script error')[:200]; done.set(); return
            p = message.get('payload', {}) or {}
            k = p.get('type')
            if k == 'results':
                results.extend(p['data']); done.set()
            elif k == 'error':
                err['msg'] = str(p.get('msg'))[:200]; done.set()
            elif k == 'negctl':
                stats['s'] = p.get('stats')

        try:
            sc = self.session.create_script(run_diff.agent_source(AGENT_JS, config))
            sc.on('message', on_message)
            sc.load()
        except Exception as e:
            return [], f'EXC {e}', None
        done.wait(timeout=HOOK_TIMEOUT_S)
        if results and 'negctl' in config:
            t = time.time() + STATS_GRACE_S
            while time.time() < t:
                time.sleep(0.05)
        try:
            sc.unload()
        except Exception:
            pass
        if not results and err['msg'] is None:
            err['msg'] = 'TIMEOUT'
        return results, err['msg'], stats['s']


def warm_worker(slot, jobs, sink, lock):
    dead_hooks = set()
    for name, mode in jobs:
        hook = HOOKS[name]
        if name in dead_hooks:
            with lock:
                sink.append(row(name, 'warm', mode, 'SKIP', 0, 0, 'real run errored', None))
            continue
        if mode == 'ret' and mode_to_negctl(mode, hook) is None:
            with lock:
                sink.append(row(name, 'warm', mode, 'SKIP', 0, 0, 'void signature', None))
            continue
        if not slot.alive():
            slot.respawn()
        results, err, stats = slot.run(name, mode)
        if err and err.startswith('EXC') and ('session is gone' in err or 'connection is closed' in err):
            slot.respawn()          # died before/at script creation (a previous job's late crash): retry once
            results, err, stats = slot.run(name, mode)
        if mode in ('field_none', 'field_first', 'field_last') and slot.alive():
            slot.restore()
        if err and not slot.alive():
            err = f'{err} (process died)'
            slot.respawn()
        elif err == 'TIMEOUT' or (err or '').startswith('EXC'):
            slot.respawn()          # a hung call / wedged agent poisons the process for the next job
        if mode == 'real' and err:
            dead_hooks.add(name)
        verdict, mism, total = classify(mode, results, err, stats, hook)
        with lock:
            sink.append(row(name, 'warm', mode, verdict, mism, total, err, stats))
        print(f'  [slot {slot.idx}] {name:42s} {mode:12s} {verdict} {mism}/{total} {stats or ""} {err or ""}')


# ---------------------------------------------------------------- early lane

def early_job(name, mode, out_dir):
    hook = HOOKS[name]
    if mode == 'heap_extra':
        return row(name, 'early', mode, 'SKIP', 0, 0, 'heap_extra not run in a suspended spawn (CRT uninitialised)', None)
    args = [sys.executable, str(EW_SCRIPT), name]
    if mode == 'baseline':
        args.append('--self-baseline')
    elif mode != 'real':
        n = mode_to_negctl(mode, hook)
        if n is None:
            return row(name, 'early', mode, 'SKIP', 0, 0, 'void signature', None)
        args += ['--negctl', n]
    env = {**os.environ, 'MASHED_EW_ASI': str(ASI_PATH)}
    try:
        p = subprocess.run(args, env=env, capture_output=True, text=True, timeout=120, cwd=str(ROOT))
        out = p.stdout + p.stderr
    except subprocess.TimeoutExpired:
        return row(name, 'early', mode, 'ERROR', 0, 0, 'TIMEOUT', None)
    m = re.search(r'cases=(\d+) mismatches=(\d+)', out)
    st = re.search(r'NEGCTL_STATS: (.*)', out)
    stats = st.group(1).strip() if st else None
    if not m:
        tail = ' | '.join(out.strip().splitlines()[-2:])[:200]
        return row(name, 'early', mode, 'ERROR', 0, 0, tail, stats)
    total, mism = int(m.group(1)), int(m.group(2))
    results = []
    suffix = '_selfbaseline' if mode == 'baseline' else ('' if mode == 'real' else f'_negctl_{mode_to_negctl(mode, hook)}')
    csvp = ROOT / 'log' / f'diff_{name}{suffix}.csv'
    if csvp.exists():
        with csvp.open(encoding='utf-8', errors='replace') as fh:
            for r in csv.DictReader(fh):
                if None in r:       # the early-window CSV writes inputs unquoted; a comma in one breaks columns
                    results = []; break
                results.append({'original': r.get('original'), 'reimpl': r.get('reimpl'),
                                'match': r.get('match') == 'True'})
    if len(results) != total:
        results = [{'original': 1, 'reimpl': 1, 'match': True}] * (total - mism) + \
                  [{'original': 1, 'reimpl': 0, 'match': False}] * mism
    verdict, mism2, total2 = classify(mode, results, None, stats, hook)
    return row(name, 'early', mode, verdict, mism2, total2, None, stats)


# ---------------------------------------------------------------- driver

LIVE_TSV = None
LIVE_COLS = ['hook', 'rva', 'arg_type', 'lane', 'mode', 'verdict', 'mismatches', 'total', 'stats', 'err']
_live_lock = threading.Lock()


def live_append(r):
    """Append each finished job to the live TSV so a killed run loses nothing (--resume reads it)."""
    if LIVE_TSV is None:
        return
    with _live_lock:
        new = not LIVE_TSV.exists()
        with LIVE_TSV.open('a', encoding='utf-8', newline='') as fh:
            w = csv.DictWriter(fh, fieldnames=LIVE_COLS, delimiter='\t')
            if new:
                w.writeheader()
            w.writerow(r)


def row(name, lane, mode, verdict, mism, total, err, stats):
    h = HOOKS[name]
    r = {'hook': name, 'rva': f"0x{h['rva']:08x}", 'arg_type': h.get('arg_type'), 'lane': lane,
         'mode': mode, 'verdict': verdict, 'mismatches': mism, 'total': total,
         'stats': stats or '', 'err': (err or '').replace('\t', ' ').replace('\n', ' ')}
    live_append(r)
    return r


def load_done(paths):
    """(hook, mode) pairs already finished with a usable verdict; ERROR rows (and the SKIPs they caused) re-run."""
    done = set()
    for p in paths:
        text = Path(p).read_text(encoding='utf-8', errors='replace')
        if p.endswith('.tsv'):
            for r in csv.DictReader(text.splitlines(), delimiter='\t'):
                if r['verdict'] != 'ERROR' and r['err'] != 'real run errored':
                    done.add((r['hook'], r['mode']))
        else:
            for ln in text.splitlines():
                m = re.match(r'\s+\[(?:slot \d+|early)\] (\S+)\s+(\S+)\s+(\S+)', ln)
                if m and m.group(1) in HOOKS and m.group(3) != 'ERROR' and 'real run errored' not in ln:
                    done.add((m.group(1), m.group(2)))
    return done


def main(argv):
    ap = argparse.ArgumentParser(description='negative-control (mutation) runner for Frida A/B handlers')
    ap.add_argument('--inventory', action='store_true', help='print the ranked handler inventory and exit')
    ap.add_argument('--top', type=int, default=0, help='run the top-N handlers by rows x risk')
    ap.add_argument('--handlers', default='', help='comma list of arg_types')
    ap.add_argument('--hooks', default='', help='comma list of registry keys (restricts the selection)')
    ap.add_argument('--per-handler', type=int, default=0, help='sample at most K hooks per handler (0 = all)')
    ap.add_argument('--classes', default=','.join(CLASSES))
    ap.add_argument('--slots', type=int, default=4)
    ap.add_argument('--early-workers', type=int, default=3)
    ap.add_argument('--seed', type=int, default=20261011)
    ap.add_argument('--out', default=str(ROOT / 'log' / 'negctl'))
    ap.add_argument('--resume', action='append', default=[],
                    help='TSV(s) or run logs whose finished (hook, mode) jobs are skipped; repeatable')
    a = ap.parse_args(argv[1:])

    ranked, conf = inventory()
    early_only = early_only_argtypes()
    if a.inventory:
        print(f"{'rank':>4} {'arg_type':34s} {'C3+C4':>6} {'C4':>4} {'risk':>4} {'lane':6s} note")
        for i, (at, ks) in enumerate(ranked, 1):
            c4 = sum(1 for k in ks if conf[HOOKS[k]['rva']][0] == 'C4')
            rk, note = RISK.get(at, (1, ''))
            print(f"{i:>4} {str(at):34s} {len(ks):>6} {c4:>4} {rk:>4} {'early' if at in early_only else 'warm':6s} {note}")
        return 0

    sel = [at for at, _ in ranked[:a.top]] if a.top else []
    sel += [x for x in a.handlers.split(',') if x and x not in sel]
    want_hooks = set(x for x in a.hooks.split(',') if x)
    classes = [c for c in a.classes.split(',') if c]
    rnd = random.Random(a.seed)
    by_at = dict(ranked)
    warm_jobs, early_jobs, skipped_race = [], [], []
    for at in sel:
        ks = sorted(by_at.get(at, []))
        if want_hooks:
            ks = [k for k in ks if k in want_hooks]
        if a.per_handler and len(ks) > a.per_handler:
            ks = sorted(rnd.sample(ks, a.per_handler))
        for k in ks:
            if HOOKS[k].get('scenario') == 'race':
                skipped_race.append(k); continue
            jobs = [(k, m) for m in ['real', 'baseline'] + classes]
            (early_jobs if at in early_only else warm_jobs).extend(jobs)

    out = Path(a.out); out.mkdir(parents=True, exist_ok=True)
    global LIVE_TSV
    LIVE_TSV = out / f"negctl_live_{time.strftime('%Y%m%d_%H%M%S')}.tsv"
    done = load_done(a.resume)
    if done:
        before = len(warm_jobs) + len(early_jobs)
        warm_jobs = [j for j in warm_jobs if j not in done]
        early_jobs = [j for j in early_jobs if j not in done]
        print(f'--resume: skipping {before - len(warm_jobs) - len(early_jobs)} finished jobs')
    print(f'handlers: {sel}')
    print(f'warm jobs {len(warm_jobs)}, early jobs {len(early_jobs)}, race-scenario hooks skipped {len(skipped_race)}')
    if not ASI_PATH.exists():
        sys.exit(f'{ASI_PATH} missing: build the dev .asi first')

    sink, lock = [], threading.Lock()
    t0 = time.time()
    if warm_jobs:
        # keep one hook's jobs on one slot, in order (real, baseline, mutants)
        per_hook = defaultdict(list)
        for j in warm_jobs:
            per_hook[j[0]].append(j)
        n = max(1, min(a.slots, len(per_hook)))
        shards = [[] for _ in range(n)]
        for i, k in enumerate(per_hook):
            shards[i % n].extend(per_hook[k])
        slots = []
        for i in range(n):
            if i:
                time.sleep(3.0)
            slots.append(Slot(i))
        time.sleep(6.0)
        try:
            with ThreadPoolExecutor(max_workers=n) as ex:
                for f in [ex.submit(warm_worker, slots[i], shards[i], sink, lock) for i in range(n)]:
                    f.result()
        finally:
            for s in slots:
                s.kill()
    if early_jobs:
        def ew(j):
            r = early_job(j[0], j[1], out)
            with lock:
                sink.append(r)
            print(f"  [early] {r['hook']:42s} {r['mode']:12s} {r['verdict']} {r['mismatches']}/{r['total']} {r['stats']} {r['err']}")
        per_hook = defaultdict(list)
        for j in early_jobs:
            per_hook[j[0]].append(j)
        with ThreadPoolExecutor(max_workers=a.early_workers) as ex:
            list(ex.map(lambda js: [ew(j) for j in js], per_hook.values()))

    stamp = time.strftime('%Y%m%d_%H%M%S')
    tsv = out / f'negctl_{stamp}.tsv'
    cols = ['hook', 'rva', 'arg_type', 'lane', 'mode', 'verdict', 'mismatches', 'total', 'stats', 'err']
    with tsv.open('w', encoding='utf-8', newline='') as fh:
        w = csv.DictWriter(fh, fieldnames=cols, delimiter='\t')
        w.writeheader()
        for r in sorted(sink, key=lambda r: (r['arg_type'], r['hook'], r['mode'])):
            w.writerow(r)
    if skipped_race:
        (out / f'negctl_{stamp}_race_skipped.txt').write_text('\n'.join(skipped_race) + '\n', encoding='utf-8')
    print(f'\n{len(sink)} jobs in {time.time() - t0:.0f}s -> {tsv}')
    summarize(sink)
    return 0


def summarize(sink):
    per = defaultdict(lambda: defaultdict(Counter))
    for r in sink:
        per[r['arg_type']][r['mode']][r['verdict']] += 1
    for at in sorted(per):
        print(f'\n{at}')
        for m in ['real', 'baseline'] + list(CLASSES):
            if m in per[at]:
                print(f"  {m:13s} " + '  '.join(f'{v}={n}' for v, n in sorted(per[at][m].items())))


if __name__ == '__main__':
    sys.exit(main(sys.argv))
