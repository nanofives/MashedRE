import csv, json, os, re, sys, glob

wt = os.getcwd()
promo = json.load(open(sys.argv[1]))
out = sys.argv[2]
rows = list(csv.DictReader(open('hooks.csv', encoding='utf-8', newline='')))
sel = [r for r in rows if r['confidence'].strip() == 'C4' and 'c4=' not in (r['notes'] or '')]
assert len(sel) == 183

DI = 'diff with the hook installed'
IS = 'install manifest plus survival'
DB = 'diff with the hook bypassed'
FM = 'file missing'
OT = 'other'

def exists(p):
    p = re.sub(r'^c4n:', '', p).split(' (')[0]
    return os.path.exists(os.path.join(wt, p)) or os.path.exists(p) or os.path.exists(os.path.join('C:/Users/maria/Desktop/Proyectos/Mashed', p))

def tsvrow(path, rva):
    best = None
    for l in csv.DictReader(open(path, encoding='utf-8', newline=''), delimiter='\t'):
        if l['rva'].lower() == '0x' + rva:
            best = l
    return best

def jrow(path, rva):
    d = json.load(open(path, encoding='utf-8'))
    for k, v in d.items():
        if isinstance(v, dict) and v.get('rva', k).lower() == '0x' + rva or k.lower() == '0x' + rva:
            return v
    return None


def tsv_rows(path, rva):
    out_ = []
    try:
        for l in csv.DictReader(open(path, encoding='utf-8', newline=''), delimiter='	'):
            k = l.get('rva', '').lower()
            if k == '0x' + rva or k.startswith('0x' + rva + '_'):
                out_.append(l)
    except Exception:
        pass
    return out_

def race_ok(l):
    try:
        return l.get('verdict') == 'C4-CLEAN' and l.get('installed') == 'True' and l['off_calls'] == l['on_calls'] and int(l['on_calls']) > 0
    except Exception:
        return False

def wsb_eval(rva, cited):
    rr = tsv_rows(cited, rva) if os.path.exists(cited) else []
    if any(race_ok(l) for l in rr):
        l = [l for l in rr if race_ok(l)][-1]
        return DI, 'cited racediff tsv: C4-CLEAN installed=True off_calls=on_calls=%s' % l['on_calls']
    for f in sorted(glob.glob('verify/wsb*/*racediff*summary*.tsv')):
        for l in tsv_rows(f, rva):
            if race_ok(l):
                return DI, 'cited tsv row is %s; racediff C4-CLEAN installed=True %s calls in %s' % (rr[-1]['verdict'] if rr else 'absent', l['on_calls'], f)
    for f in sorted(glob.glob('verify/wsb*/RESULT_D4-WSB*.md')):
        for line in open(f, encoding='utf-8', errors='replace'):
            if ('0x' + rva) in line and 'C4-CLEAN' in line and 'not observed' not in line and 'HOLD' not in line:
                return DI, 'cited tsv row is %s; %s states C4-CLEAN racediff with hook installed (raw samples off-git, hashes listed)' % (rr[-1]['verdict'] if rr else 'absent', f)
    for f in sorted(glob.glob('verify/wsb*/shadow*summary*.tsv')):
        for l in tsv_rows(f, rva):
            if l.get('verdict') == 'CLEAN' and 'installed=e9' in l.get('proof', '') and l.get('ndiff') == '0':
                return DI, 'cited racediff row %s; in-process write-set shadow A/B CLEAN n=%s ndiff=0 hook installed (%s)' % (rr[-1]['verdict'] if rr else 'absent', l['n'], f)
    return OT, 'no C4-CLEAN installed racediff row, RESULT line or CLEAN shadow found; cited row: %s' % ((rr[-1]['verdict'], rr[-1].get('why')) if rr else None)

racejsons = ['log/c4_racediff_result.json', 're/analysis/phys_c4_evidence/c4_racediff_result_2026-08-15.json']
verify_json = json.load(open('log/c4_verify_result.json'))
save_obs = {l['rva'].lower(): l for l in csv.DictReader(open('log/observe_c4_save_hooks.csv', encoding='utf-8', newline=''))}

def csv_all_match(path):
    try:
        ls = open(path, encoding='utf-8').read().splitlines()[1:]
        ls = [l for l in ls if l.strip()]
        return len(ls), sum(1 for l in ls if ',True' in l and ',False' not in l)
    except Exception:
        return None

NEGCTL = {'5fd8f82e6', '5d382e9cf', '3eb062777', '016ac17e5', '3dca66d38', 'a56f9f551', '5ce0a1c15', '22d9ac879'}
INSTALL_ONLY = {'3f4a33f3e', '1350c55a8', '86f41ab50', '21baaa8fd', '763ac8793', '23c3832e8', 'd9a7e6046', '7c2af680f',
                '1aadeb963', 'fa7815edc', 'c2232357e', 'db06f5319', 'a15f82c02'}
VERIFYJSON = {'c77569bf5', 'f095e483f', ' 19cb4b56f'.strip()}
WSB = {'020d70c14', 'f28461621', 'da9d0b582', '130929d9d', '42ff9dd2c', '51aaefe34', '24795a2f8', 'dcc0f2bba', '80096c7cb', '7661c57e5', 'a053b5611'}

res = []
for r in sel:
    rva = r['rva']; fd = r['frida_diff']
    ch, cdate = promo[rva].split()
    parts = [re.sub(r'^c4n:', '', p).split(' (')[0] for p in fd.split(';') if p]
    ex = [exists(p) for p in parts]
    cls = OT; why = ''
    if ch in WSB:
        cls, why = wsb_eval(rva, parts[0] if parts else '')
    elif ch == '474a99d3b':
        cls = DI if exists(parts[0]) else FM
        why = 'strict canonical race, hook installed alone (manifest installed=1), in-process full-output A/B 769/769+1793/1793 or 129/129+129/129; per RESULT'
    elif ch == 'f8fc7622e':
        hit = None
        for j in racejsons:
            v = jrow(j, rva)
            if v: hit = (j, v); break
        if hit and hit[1].get('verdict') == 'C4-CLEAN' and hit[1].get('installed') and hit[1].get('off') == hit[1].get('on'):
            cls = DI; why = 'log/c4_racediff_result.json (commit CHANGELOG cite): C4-CLEAN installed=True off==on n=%s' % hit[1].get('off_n')
        elif rva == '0042b900':
            cls = FM; why = 'cited log/c4_stalkerdiff_result.json is gitignored and absent; commit CHANGELOG cites it'
        elif rva == '0042b8c0':
            aj = json.load(open('log/c4_autodiff_result.json'))
            has = any(v.get('rva', '').lower() == '0x0042b8c0' for v in aj.values())
            cls = OT; why = 'cited log/c4_autodiff_result.json exists but has %s entry for 0x0042b8c0 (it holds 0x0042e3a0 HOLD only)' % ('an' if has else 'no')
        else:
            n = csv_all_match(parts[0]) if parts and parts[0].endswith('.csv') else None
            cls = DB
            why = 'cited csv is path1 force-call original-vs-reimpl diff (%s); commit CHANGELOG cites log/c4_racediff_result.json which has no entry for this rva' % ('%d/%d match' % (n[1], n[0]) if n else 'not parsed')
    elif ch in VERIFYJSON:
        v = verify_json.get('0x' + rva)
        if v and v['calls'] > 0 and v['installed'] and v['survived'] and v['C4']:
            cls = IS; why = 'c4_verify_result.json: calls=%d installed survived; no original-vs-modded diff in it (OFF count, ON install+boot survival)' % v['calls']
        else:
            cls = IS; why = 'commit message: inline-JMP live 0xE9, exercised in boot-to-menu OFF count, booted and survived; cited c4_verify_result.json now has no entry for this rva (overwritten by a later batch); no diff in the commit'
    elif ch == '19cb4b56f':
        v = verify_json.get('0x' + rva)
        if v and v['calls'] > 0 and v['installed'] and v['survived'] and v['C4']:
            cls = IS; why = 'c4_verify_result.json: calls=%d installed survived; no original-vs-modded diff in it' % v['calls']
        else:
            cls, why = OT, 'verify json entry %s' % (v,)
    elif ch == '7c2af680f':
        v = verify_json.get('0x' + rva)
        if v and v['calls'] > 0 and v['installed'] and v['survived']:
            cls = IS; why = 'commit: E9+rel32 verified, 10s idle, no Interceptor; cited c4_verify_result.json calls=%d installed survived' % v['calls']
        elif v:
            cls = IS; why = 'commit: E9+rel32 verified, 10s idle alive, batch B1..B4 observe; cited c4_verify_result.json entry calls=%d (0 calls: body not shown executed)' % v['calls']
        else:
            cls = IS; why = 'commit: E9+rel32 verified, 10s idle alive (observe_c4_sweep B1..B4); cited path is a synthetic csv diff, not used for the class'
    elif ch == '1aadeb963':
        o = save_obs.get('0x' + rva)
        if o and o['install_ok'] == 'True' and o['alive_throughout'] == 'True':
            cls = IS; why = 'log/observe_c4_save_hooks.csv: install_ok alive_throughout boot_main_menu_idle_15s; cited reearn csv is a synthetic diff (bypassed)'
        else:
            cls, why = OT, 'rva not in observe_c4_save_hooks.csv with install_ok+alive: %s' % (o,)
    elif ch in INSTALL_ONLY:
        cls = IS
        why = {'3f4a33f3e': 'observe_hooks_at_menu: E9+rel32 verified at install and end of 10s window, ~37,800 natural calls, no crash; commit says synthetic A/B had hook bypassed',
               '1350c55a8': 'main_menu_idle_10s: E9+rel32 OK, bytes stable 10s; interceptor harness-limited',
               '86f41ab50': 'E9+rel32, 1920 calls cold_boot_to_menu, alive 10s',
               '21baaa8fd': 'path2 E9+rel32, alive 20s main_menu_idle; interceptor count 0/3 harness-limited',
               '763ac8793': 'path2 E9+rel32, 10s observe alive; RPC interceptor harness-limited',
               '23c3832e8': 'E9+rel32, alive 20s main_menu_navigate_all; interceptor count harness-limited or AV on bare inputs',
               'd9a7e6046': 'path2 E9+rel32, 60 calls cold_boot_to_menu, game reaches menu alive',
               'fa7815edc': 'c4-lift: canonical observation, fired during boot-to-menu or per-frame survival; row notes add R1-B subset-install 0xE9 live + 25 s survival',
               'c2232357e': 'canonical_install_observe 19-hook subset: all JMP-LIVE 0xE9, boot-to-menu, 25 s survival; cited csv is a synthetic diff (bypassed)',
               'db06f5319': 'c4_manifest_frontend_twins_20260610.txt lists only the 3 twins installed=1; 30 s boot-to-menu firing every frame; cited diff json is the C3 nav-driven twin-vs-original draw-sequence diff',
               'a15f82c02': 'subset-install canonical observe x3 (50/30/40s), JMP-LIVE 0xE9, manifest 1/535, organic nav fires; 264/264 diff is a synthetic direct-call sweep'}[ch]
        missing = [p for p, e in zip(parts, ex) if not e]
        if missing:
            why += '; cited path(s) missing: ' + ','.join(missing)
    elif ch in ('9bcd1dd55', 'b49625913'):
        cls = IS
        why = 'canonical race with hook installed: body executed via .asi export counter (not a diff); bit-identity half is the synthetic path1 A/B with hook bypassed'
    elif ch in ('305f3682b', '882b07209', '5a7251f83', '4e318b65f'):
        jf = {'305f3682b': 're/analysis/frontend_input_nav_trace/c4_leafdiff_gts.json', '882b07209': 're/analysis/frontend_input_nav_trace/c4_leafdiff_gts.json',
              '5a7251f83': 're/analysis/frontend_input_nav_trace/c4_sidediff_gts.json', '4e318b65f': 're/analysis/frontend_input_nav_trace/c4_readinessA_gts.json'}[ch]
        v = jrow(jf, rva) if os.path.exists(jf) else None
        if v is None and ch == '4e318b65f':
            d = json.load(open(jf)); v = d if isinstance(d, dict) else None
            ok = 'C4-CLEAN' in json.dumps(d) and '"installed": true' in json.dumps(d).lower()
        else:
            ok = bool(v and v.get('verdict') == 'C4-CLEAN' and v.get('installed'))
        if ok:
            cls = DI; why = '%s: OFF original vs ON modded, inline-JMP live, verdict C4-CLEAN' % jf
            if jf not in parts: why += '; cited path is a later negctl csv, not this file'
        else:
            cls, why = OT, 'no C4-CLEAN installed entry for rva in %s' % jf
    elif ch == '19766af56':
        p0 = parts[0]
        if exists(p0):
            ls = list(csv.DictReader(open(p0, encoding='utf-8', newline='')))
            live_green = [l for l in ls if l.get('result') == 'GREEN' and (l.get('hook_live') in ('True',) or l.get('first_bytes', '').startswith('e9') or l.get('export'))]
            cls = DI if live_green else OT
            why = 'compare csv: %d GREEN hook-live arms vs stock ref, control arms RED' % len(live_green)
        else:
            cls, why = FM, ''
    elif ch in NEGCTL:
        p0 = parts[0]
        cls = DI if exists(p0) else FM
        why = 'negctl canonical scenario, 0xE9 live before attach, per-call check vs original-bytes model and original runs, old-defect control RED'
        if ch in ('3eb062777', '3dca66d38', '5ce0a1c15', 'a56f9f551'):
            why += '; includes a user-accepted forced input'
    elif ch in ('702047b0e', '0817199b3'):
        p0 = parts[0]
        txt = open(p0, encoding='utf-8', errors='replace').read() if os.path.exists(p0) else ''
        nm = r['name']
        lines = [l for l in txt.splitlines() if ('fn=' + nm + ' ') in l]
        bad = [l for l in lines if 'ndiff=0' not in l]
        if lines and not bad:
            cls = DI; why = 'in-process bit-identity self-test, inline-JMP installed, canonical QuickRace: %d lines fn=%s all ndiff=0 OK in %s; original run via HookSystem::Uninstall/Install' % (len(lines), nm, p0)
        else:
            cls = OT; why = 'cited log has %d lines for fn=%s (%d with ndiff!=0)' % (len(lines), nm, len(bad))
    elif ch in ('98bbd2c9c', 'c0f35c8f2'):
        cls = DI
        why = 'in-process A/B on the original (MASHED_LEAF_SELFTEST, hook live, canonical race) per RESULT_%s; cited path is off-git (D:/ or csv) and not read' % ('dcmix' if ch == '98bbd2c9c' else 'dcleaf')
        if ch == 'c0f35c8f2': why += '; coverage gap U-9787 for 0x00442ce0 tier branches'
        if ch == '98bbd2c9c': why += '; coverage gap U-9788 (dying-but-alive input not shown)'
    elif ch in ('fadf5eb63', '22e9bc96d', '2d9cd2b2f'):
        cls = FM
        why = "cited value 'green' is not a path; commit RESULT (verify/dcphys*_2026102*/RESULT_*.md) states in-process A/B on the original, hook live, canonical recipe, on the default body after a default flip"
    if cls == OT and not why: why = 'unclassified'
    res.append((rva, fd, ch, cls, why))

import collections
c = collections.Counter(x[3] for x in res)
print(len(res), c)
with open(out, 'w', encoding='utf-8', newline='') as f:
    f.write('rva\tcited_evidence_path\tpromotion_commit\tclass\treason\n')
    for x in res:
        f.write('\t'.join(['0x' + x[0], x[1], x[2], x[3], x[4].replace('\t', ' ').replace('\n', ' ')]) + '\n')
for x in res:
    if x[3] in (OT, FM, DB):
        print(x[0], x[2], x[3], '|', x[4][:160])
