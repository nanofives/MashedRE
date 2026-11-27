"""PortTop binder: plan and generate exe-only port bodies (Match/PortTop_<rva>.cpp) for a call closure on the top-down
route (MASHED_ORIG_TOP, L-96 / L-111), plus the follow-up steps every binding checkpoint runs. BIND-TOOL, 2026-11-21.

One tool for what three per-row generators did before:
  verify/portorigtop_20261113/tools/audiotick_bind_gen.py   -> profile `audio`    (audio-tick / audio-init closure)
  verify/ro_20261116/rltools/rl_bind_gen.py                 -> profile `raceload` (race-load closure of 0x0040d270)
  verify/portorigtop_20261113/tools/bindings_update.py      -> subcommand `bindings`
and their helpers (exe_body_index.py, sync_rsp_allowlist.py / rl_sync.py, guard_externs.py, unclean_from_log.py,
same_source_exe.py, prune_allowlist.py). The rule tables of both generators are carried over unchanged, so a re-run on
the same tree writes the same files. State (plan.json, cpp_unclean.txt, newfiles.txt) lives in
re/tools/bind/state/<profile>/ (copied from the old verify/ folders at BIND-TOOL).

BIND-PROFILE-DATA (2026-11-21): profiles and rule tables are data, not code. A profile is state/<profile>/profile.json
(settings) + rules.tsv (per-rva rules) + roots.txt (roots); state/_shared/rules.tsv holds the rules every profile reads.
A binding row adds lines there and never edits this file (KICKOFF_run54 rule 1). Profiles: raceload, audio, raceframe
(PORT-TOP-06), renderframe (PORT-TOP-RENDERFRAME-492E90), bulk (BIND-BULK-PROFILE: RACE-BIND-C4-BULK checkpoint 1, was
verify/bindbulk_20261121/tools/bulk_bind_gen.py; after `write --profile bulk` run `refs --profile raceload`, which owns
Match/PortTop_BulkRefs.cpp, and `sync --profile bulk`).

BIND-LIB-ROUTE (2026-11-21): binding kind `lib` next to `crt`. A vendored library (Lua 4.0 first, then libpng, zlib,
qhull) binds by name: `lib_root <lib> <root> <globs>` declares the library tree and its source globs, `lib <rva> <lib>
<symbol>` maps an rva to the library's own symbol (rules.tsv, shared or per profile). The planner gives that rva
('reuse', <symbol>, 'lib:<lib>:<file>:<line>'), so caller wrappers #define FUN_<rva> <symbol>; the symbol must have
exactly one non-static definition in the globs, else the tool refuses. The library objects are linked by the library's
own build step (mashedmod/deps/<lib>), not by this tool. Unit test: re/tools/bind/test_porttop_bind_lib.py.

Run from the worktree root. Usage (full walk-through: .claude/skills/roadmap-loop/SKILL.md, "Binding checkpoints"):
  py -3.12 re/tools/bind/porttop_bind.py plan    --profile raceload --roots 00426e10,004264d0      # dry run, listing
  py -3.12 re/tools/bind/porttop_bind.py write   --profile raceload --roots 00426e10,004264d0      # wrappers + stops + plan
  py -3.12 re/tools/bind/porttop_bind.py externs --profile raceload [--apply]   # extern-guard step (match.py --bytes after)
  py -3.12 re/tools/bind/porttop_bind.py sync    --profile raceload --step <ROW> --evidence <RESULT path>
  py -3.12 re/tools/bind/porttop_bind.py unclean --profile raceload <vs2022 build.log>   # cpp-unclean list from a build
  py -3.12 re/tools/bind/porttop_bind.py bindings --profile audio [--dry]       # BINDINGS.tsv rows from plan.json
  py -3.12 re/tools/bind/porttop_bind.py samesrc --profile raceload            # planned binds another TU already compiles
  py -3.12 re/tools/bind/porttop_bind.py prune   --profile raceload [--apply]   # allowlist lines of rvas no longer bound
  py -3.12 re/tools/bind/porttop_bind.py index   [--out <json>]                 # rva -> exe body index (debug)
  py -3.12 re/tools/bind/porttop_bind.py selftest                               # rebind every profile, expect no change
(--roots may be left out of plan / write: the profile's roots.txt is used.)
"""
import argparse, csv, glob, json, os, re, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
SRC = 'mashedmod/src/mashed_re'
RSP = 'mashedmod/exe_sources.rsp'
ALLOW = 're/tools/dual_copy_allowlist.txt'
BINDINGS = 'verify/porttop_20261108/BINDINGS.tsv'
NL = '\n'


def rd(p):
    return open(p, encoding='utf-8', errors='replace').read()


def write_if_changed(fn, text):
    """Write LF text unless the file already holds it (CRLF working copies compare equal: core.autocrlf, so a re-run
    does not rewrite every wrapper with only CR changes). Returns True when written."""
    if os.path.exists(fn) and open(fn, 'rb').read().replace(b'\r\n', b'\n') == text.encode('utf-8'):
        return False
    open(fn, 'w', encoding='utf-8', newline='\n').write(text)
    return True


# --------------------------------------------------------------------------------------------------------------------
# Profiles and rule tables are data (BIND-PROFILE-DATA, 2026-11-21): a binding row adds lines to
#   re/tools/bind/state/_shared/rules.tsv        rules every profile reads (prov_bind, extra_pre, extra_in, alias, ...)
#   re/tools/bind/state/<profile>/profile.json   the profile's settings (marks, stop file, header and stop text, flags)
#   re/tools/bind/state/<profile>/rules.tsv      its per-rva rules (force, stop_force, knob, alias, not_local, ...)
#   re/tools/bind/state/<profile>/roots.txt      its roots (plan / write default, selftest)
# and never edits this file (KICKOFF_run54 rule 1). The rule kinds are listed in state/_shared/rules.tsv. A profile is a
# folder under state/ with a profile.json; keys a profile.json leaves out take the defaults in PROFILE_DEFAULTS.
# --------------------------------------------------------------------------------------------------------------------
STATE = os.path.join(HERE, 'state')
SHARED_RULES = os.path.join(STATE, '_shared', 'rules.tsv')

# rule kind -> number of fields after the kind (the last field takes the rest of the line, tabs included)
RULE_KINDS = {'prov_bind': 1, 'force': 1, 'stop_force': 1, 'extra_pre': 2, 'extra_in': 2, 'extra_def': 2,
              'alias': 3, 'knob': 3, 'stop_reason': 2, 'stop_why': 2, 'name_def': 2, 'not_local': 2,
              'local_shared': 3, 'local_as_bound': 2, 'callee_override': 4,
              # BIND-LIB-ROUTE (2026-11-21): vendored library route (Lua 4.0 first, then libpng / zlib / qhull)
              #   lib_root <lib> <root> <globs>   library <lib>: its tree under the worktree root and the source globs
              #                                   (space-separated, relative to <root>) that define its symbols
              #   lib      <rva> <lib> <symbol>   callers of rva bind the vendored library's own symbol by name
              'lib_root': 3, 'lib': 3}
RVA_PAIR_KINDS = {'not_local', 'local_shared', 'local_as_bound', 'callee_override'}   # <caller> <callee> first
NO_RVA_KINDS = {'lib_root'}   # first field is a name, not an rva: kept as written

PROFILE_DEFAULTS = dict(
    own_mark=None, own_tag=None, prune_stale=False, rwcore_fallback=False, reuse_any=False, force_old_binds=False,
    c_emit=True, listing='full', stops={'style': 'count'}, stops_subst=[], hdr=None, header=None, stop_force_note=None,
    refs_file=None, foreign_keep_marks=[], stop_reasons=None, static_stop=False, group_same_source=False,
    held_external=False, thunk_names=False, bindings_keep_missing=False, root_external={},
    local_shared_note='', local_bound_note='', bindings_roots=None, bindings_root_caller=None, bindings_tag=None,
    selftest_order=99, selftest_roots_only=False, selftest_bindings=False,
    # BIND-BULK-PROFILE (2026-11-21), the keys the RACE-BIND-C4-BULK cp1 generator (bulk_bind_gen.py) needed:
    #   force_roots false        a root that already has an exe body is reused, never bound twice
    #   roots_only_reason <fmt>  a matched callee that is not a root is a named stop with this reason (%s = its source)
    #   bound_wins true          a PortTop body another profile bound (exe index, own wrappers left out) is reused
    #                            before any stop rule (cpp-unclean, stop_reason, roots-only); stop_force still wins
    #   samesrc_reuse true       a planned bind whose source another exe .cpp TU already #includes reuses that TU
    #   group_primary lowest     group_same_source: the lowest rva carries the shared source (default: the file's rva)
    #   drop_local_copies true   a planned bind that is a file-local copy in another bind's source becomes a stop
    #   stop_sym_foreign true    a stop's callers name another profile's existing stop function when one exists
    #   wrapper_style bulk       wrapper text of the bulk generator (always a PortTop_Keep_<rva> anchor, group members)
    #   selftest_history {listing, moved}   selftest: the listing against a frozen listing, moved lines named in moved
    force_roots=True, roots_only_reason=None, bound_wins=False, samesrc_reuse=False, group_primary='own',
    drop_local_copies=False, stop_sym_foreign=False, wrapper_style='default', selftest_history=None)


def read_rules(path):
    """rules.tsv -> {kind: [fields tuple, ...]} in file order. '#' at column 0 is a comment line."""
    out = {}
    if not os.path.exists(path):
        return out
    for n, l in enumerate(open(path, encoding='utf-8'), 1):
        l = l.rstrip('\r\n')
        if not l.strip() or l.startswith('#'):
            continue
        kind, _, rest = l.partition('\t')
        if kind not in RULE_KINDS:
            sys.exit('%s:%d: unknown rule kind %r' % (path, n, kind))
        f = rest.split('\t', RULE_KINDS[kind] - 1)
        if len(f) != RULE_KINDS[kind]:
            sys.exit('%s:%d: %s takes %d fields' % (path, n, kind, RULE_KINDS[kind]))
        nrva = 2 if kind in RVA_PAIR_KINDS else 0 if kind in NO_RVA_KINDS else 1   # leading rva fields, lower case
        out.setdefault(kind, []).append(tuple(x.strip().lower() if i < nrva else x for i, x in enumerate(f)))
    return out


def rule_tables(rules):
    """the planner's tables from one rules dict (state/_shared/rules.tsv or a profile's rules.tsv)."""
    r = lambda k: rules.get(k, [])
    t = dict(prov_bind={x[0] for x in r('prov_bind')}, force={x[0] for x in r('force')},
             stop_force={x[0] for x in r('stop_force')},
             alias={x[0]: (x[1], x[2]) for x in r('alias')}, knob={x[0]: (x[1], x[2]) for x in r('knob')},
             stop_reason={x[0]: x[1] for x in r('stop_reason')}, stop_why={x[0]: x[1] for x in r('stop_why')},
             name_def={x[0]: x[1] for x in r('name_def')},
             not_local={(x[0], x[1]) for x in r('not_local')},
             local_shared={(x[0], x[1]): x[2] for x in r('local_shared')},
             local_as_bound={(x[0], x[1]) for x in r('local_as_bound')},
             callee_override={(x[0], x[1]): (x[2], x[3]) for x in r('callee_override')},
             lib_root={x[0].strip(): (x[1].strip(), x[2].split()) for x in r('lib_root')},
             lib={x[0].replace('0x', ''): (x[1].strip(), x[2].strip()) for x in r('lib')})
    for k in ('extra_pre', 'extra_in', 'extra_def'):
        t[k] = {}
        for x in r(k):
            t[k].setdefault(x[0], []).append(x[1])
    return t


def load_profiles():
    profs = {}
    for d in sorted(glob.glob(os.path.join(STATE, '*', 'profile.json'))):
        name = os.path.basename(os.path.dirname(d))
        P = dict(PROFILE_DEFAULTS)
        P.update({k: v for k, v in json.load(open(d, encoding='utf-8')).items() if not k.startswith('_')})
        P.update(rule_tables(read_rules(os.path.join(os.path.dirname(d), 'rules.tsv'))))
        if P['bindings_roots'] is not None:
            P['bindings_roots'] = [tuple(x) for x in P['bindings_roots']]
        P['stops_subst'] = [tuple(x) for x in P['stops_subst']]
        profs[name] = P
    return profs


SHARED = rule_tables(read_rules(SHARED_RULES))
PROV_BIND = SHARED['prov_bind']
EXTRA_PRE = SHARED['extra_pre']
EXTRA_IN = SHARED['extra_in']
LOCAL_SHARED = SHARED['local_shared']
LOCAL_AS_BOUND = SHARED['local_as_bound']
ALIAS = SHARED['alias']
PROFILES = load_profiles()


def profile_roots(profile):
    """state/<profile>/roots.txt (one rva per line, # comments); else the plan keys no bound plan entry calls."""
    p = os.path.join(STATE, profile, 'roots.txt')
    if os.path.exists(p):
        return ','.join(l.split('#')[0].strip().lower() for l in open(p, encoding='utf-8') if l.split('#')[0].strip())
    pl = json.load(open(os.path.join(state_dir(profile), 'plan.json')))
    called = {c for v in pl.values() if v[0] == 'bind' for c in v[3] + v[4]}
    return ','.join(sorted(k for k in pl if k not in called))


def state_dir(profile):
    d = os.path.join(HERE, 'state', profile)
    os.makedirs(d, exist_ok=True)
    return d


# --------------------------------------------------------------------------------------------------------------------
# Trackers and the exe body index
# --------------------------------------------------------------------------------------------------------------------
def load_trackers():
    rows = {}
    with open('hooks.csv', encoding='utf-8', errors='replace') as f:
        for r in csv.reader(f):
            if r and re.fullmatch(r'[0-9a-fA-F]{8}', r[0]):
                rows[r[0].lower()] = r
    adm = {}
    with open('re/match/ADMITTED.tsv', encoding='utf-8') as f:
        for r in csv.reader((l for l in f if not l.startswith('#')), delimiter='\t'):
            if r and r[0].startswith('0x'):
                adm[r[0][2:].lower()] = r
    prov = {}
    with open('re/match/PROVISIONAL.tsv', encoding='utf-8') as f:
        for r in csv.reader((l for l in f if not l.startswith('#')), delimiter='\t'):
            if r and r[0].startswith('0x') and len(r) > 10 and r[10] == 'provisional':
                prov[r[0][2:].lower()] = r
    # a match promotion committed on a branch (re/promotions/<ID>.tsv, HB-TRACKER-AT-MERGE) binds like a c4=match row
    pend = {}
    for p in glob.glob('re/promotions/*.tsv') + glob.glob('re/promotions/applied/*.tsv'):
        with open(p, encoding='utf-8') as f:
            for r in csv.DictReader((l for l in f if not l.startswith('#')), delimiter='\t'):
                if r.get('route') == 'match' and r.get('rva', '').startswith('0x') and os.path.exists(r.get('evidence', '')):
                    pend[r['rva'][2:].lower()] = r['evidence']
    return rows, adm, prov, pend


def rsp_tus():
    out = []
    for line in open(RSP, encoding='utf-8'):
        line = line.strip().strip('"')
        if line.endswith('.cpp') or line.endswith('.c'):
            out.append(line.replace('\\', '/'))
    return out


def build_index(exclude_mark=None):
    """rva -> [(symbol, TU, how)] for every TU of exe_sources.rsp (was exe_body_index.py). A TU that #includes
    re/match/<d>/<rva>.c and #defines FUN_<rva> <sym> is 'match-include'; RH_ScopedInstall(sym, 0x<rva>) is 'rh'.
    exclude_mark: TUs whose text starts with it (this profile's own generated wrappers) are left out, so a re-run plans
    them again instead of reusing them (rl_bind_gen.py's index predated its own wrappers; a fresh index without this
    turns every earlier bind into a reuse and the writer then deletes it)."""
    idx = {}
    for f in rsp_tus():
        p = os.path.join(SRC, f)
        if not os.path.exists(p):
            continue
        t = rd(p)
        if exclude_mark and t.startswith(exclude_mark):
            continue
        incs = set(m.lower() for m in re.findall(r'#include\s+"[^"]*re/match/[^"]*?/(?:[A-Za-z0-9]+_)?([0-9a-fA-F]{8})\.c"', t))
        # (PORT-TOP-06: the pattern also takes nested folders, re/match/s01/raw/<rva>.c, and prefixed names, h2_<rva>.c)
        for m in re.finditer(r'#define\s+FUN_([0-9a-fA-F]{8})\s+(\w+)', t):
            r = m.group(1).lower()
            if r in incs:
                idx.setdefault(r, []).append((m.group(2), f, 'match-include'))
        for m in re.finditer(r'RH_ScopedInstall\(\s*([\w:]+)\s*,\s*0x([0-9a-fA-F]{8})', t):
            idx.setdefault(m.group(2).lower(), []).append((m.group(1), f, 'rh'))
    return idx


def crt_bodies():
    crt = {}
    for p in glob.glob(SRC + '/Match/PortTop_Crt*.cpp'):
        for m in re.finditer(r'\b(MashedCrt_\w+?_([0-9a-f]{8}))\s*\(', rd(p)):
            crt.setdefault(m.group(2), (m.group(1), 'Match/' + os.path.basename(p)))
    return crt


# --------------------------------------------------------------------------------------------------------------------
# lib binding kind (BIND-LIB-ROUTE, 2026-11-21). The `crt` route reuses hand-named MashedCrt_<name>_<rva> bodies found by
# file name; a vendored library (Lua 4.0 at mashedmod/deps/lua-4.0, later libpng / zlib / qhull) keeps its own symbol
# names, so the rva -> symbol map is data: `lib_root` names the library tree, `lib` maps an rva to one of its symbols.
# The planner gives such an rva ('reuse', <symbol>, 'lib:<lib>:<file>'), so every caller wrapper #defines
# FUN_<rva> <symbol> (C linkage: the wrapper includes the matched source inside extern "C", and a C library defines
# <symbol> with C linkage). The library objects themselves are linked by the library's own build step, not by this tool.
# --------------------------------------------------------------------------------------------------------------------
def lib_symbol_defs(root, globs, sym):
    """every non-static function definition of sym in the library sources: [(file relative to root, line)]."""
    # the return type may sit on the line before (`int` / `fix_mul (int a,` / `{`): then that line is the prefix
    pat = re.compile(r'^((?:[A-Za-z_][^;{}#\n]*?)?)\b%s\s*\([^;{}]*\)\s*\{' % re.escape(sym), re.M)
    out = []
    seen = set()
    for g in globs:
        for p in sorted(glob.glob(os.path.join(root, g))):
            k = os.path.normcase(os.path.abspath(p))
            if k in seen:
                continue
            seen.add(k)
            t = rd(p)
            for m in pat.finditer(t):
                pre = m.group(1)
                if not pre.strip():
                    prev = t[:m.start()].rstrip('\r\n').rsplit('\n', 1)[-1]
                    if not re.fullmatch(r'\s*[A-Za-z_][\w \t\*]*', prev):
                        continue   # no return type: a call or a macro use at column 0, not a definition
                    pre = prev
                if re.match(r'\s*static\b', pre):
                    continue
                out.append((os.path.relpath(p, root).replace(os.sep, '/'), t[:m.start()].count('\n') + 1))
    return out


def resolve_libs(lib_root, lib):
    """lib rules -> ({rva: (symbol, 'lib:<lib>:<file>:<line>')}, [error lines]). An rva resolves only when its library
    is declared, the library root exists, and the symbol has exactly one non-static definition in the source globs."""
    out, errs = {}, []
    for rva, (name, sym) in sorted(lib.items()):
        if not re.fullmatch(r'[0-9a-f]{8}', rva):
            errs.append('lib %s: not an rva' % rva); continue
        if name not in lib_root:
            errs.append('lib %s: library %r has no lib_root line' % (rva, name)); continue
        root, globs = lib_root[name]
        if not os.path.isdir(root):
            errs.append('lib %s: library %s root %s does not exist' % (rva, name, root)); continue
        if not re.fullmatch(r'[A-Za-z_]\w*', sym):
            errs.append('lib %s: %r is not a C symbol' % (rva, sym)); continue
        defs = lib_symbol_defs(root, globs, sym)
        if len(defs) != 1:
            errs.append('lib %s: %s defined %d times (non-static) in %s %s%s' % (
                rva, sym, len(defs), root, ' '.join(globs), (': ' + ', '.join('%s:%d' % d for d in defs)) if defs else ''))
            continue
        out[rva] = (sym, 'lib:%s:%s:%d' % (name, defs[0][0], defs[0][1]))
    return out, errs


# --------------------------------------------------------------------------------------------------------------------
# Planner
# --------------------------------------------------------------------------------------------------------------------
class Binder:
    def __init__(self, profile, roots, roots_only=False):
        self.name = profile
        self.P = PROFILES[profile]
        self.roots = [r.lower().replace('0x', '') for r in roots if r]
        # --roots-only (BIND-CUNCLEAN-AS-C): a matched callee that is neither a root nor bound by the previous plan is a
        # named stop instead of a new bind, so a row binds only the rvas it owns (one owner per rva, run 53 merge bar)
        self.roots_only = roots_only
        self.rows, self.adm, self.prov, self.pend = load_trackers()
        self.idx = build_index(self.P['own_mark'])
        self.crt = crt_bodies()
        # BIND-LIB-ROUTE: lib_root / lib rules of state/_shared/rules.tsv, then the profile's own (a profile line wins)
        self.lib_root = dict(SHARED['lib_root']); self.lib_root.update(self.P['lib_root'])
        lib = dict(SHARED['lib']); lib.update(self.P['lib'])
        self.lib, errs = resolve_libs(self.lib_root, lib)
        if errs:
            sys.exit('lib rules (profile %s):\n  %s' % (profile, '\n  '.join(errs)))
        self.sd = state_dir(profile)
        self.plan_path = os.path.join(self.sd, 'plan.json')
        self.old_plan = json.load(open(self.plan_path)) if os.path.exists(self.plan_path) else {}
        self.alias = dict(ALIAS)
        self.alias.update(self.P['alias'])
        if self.P['force_old_binds']:
            # audio: re-runs regenerate every wrapper written before, so a callee that moved from stop to bind is
            # rebound in all its callers instead of the old wrapper being reused
            self.force = set(self.P['force']) | {k for k, v in self.old_plan.items() if v and v[0] == 'bind'}
            self.force -= set(self.alias)
        else:
            self.force = (set(self.roots) if self.P['force_roots'] else set()) | set(self.P['force'])
        self.refused = {}   # samesrc_reuse / drop_local_copies: root -> why it ends up unbound (disposition.json)
        self.unclean = {}
        up = os.path.join(self.sd, 'cpp_unclean.txt')
        if os.path.exists(up):
            for l in open(up, encoding='utf-8'):
                if l.strip() and not l.startswith('#'):
                    k, _, why = l.rstrip('\r\n').partition('\t')
                    self.unclean[k.strip().lower()] = why.strip()
        # BIND-CUNCLEAN-AS-C: a cpp-unclean source binds as a C unit (Match/PortTop_<rva>.c, compiled as C by its
        # extension), unless state/<profile>/c_hold.txt holds it back (owned by another row: it stays a named stop)
        self.hold = {}
        hp = os.path.join(self.sd, 'c_hold.txt')
        if os.path.exists(hp):
            for l in open(hp, encoding='utf-8'):
                if l.strip() and not l.startswith('#'):
                    k, _, why = l.rstrip('\r\n').partition('\t')
                    self.hold[k.strip().lower()] = why.strip()
        # profile.json c_emit false (raceframe / renderframe, written before BIND-CUNCLEAN-AS-C): cpp-unclean rvas stay stops
        self.cemit = (set(self.unclean) - set(self.hold)) if self.P['c_emit'] else set()
        # stop_reasons file (rvas other rows own) + stop_why rules: a planned bind of them becomes a named stop
        self.reasons = {}
        if self.P['stop_reasons']:
            rp = os.path.join(self.sd, self.P['stop_reasons'])
            for l in (open(rp, encoding='utf-8') if os.path.exists(rp) else []):
                if l.strip() and not l.startswith('#'):
                    k, _, why = l.rstrip('\r\n').partition('\t')
                    self.reasons.setdefault(k.strip().lower().replace('0x', ''), why.strip())
        self.group_bound = set()
        for k, why in self.P['stop_why'].items():
            self.reasons[k] = why
        self.extra_pre = {k: EXTRA_PRE.get(k, []) + self.P['extra_pre'].get(k, []) for k in set(EXTRA_PRE) | set(self.P['extra_pre'])}
        self.extra_in = {k: EXTRA_IN.get(k, []) + self.P['extra_in'].get(k, []) for k in set(EXTRA_IN) | set(self.P['extra_in'])}
        self.local_shared = dict(LOCAL_SHARED); self.local_shared.update(self.P['local_shared'])
        self.local_as_bound = set(LOCAL_AS_BOUND) | self.P['local_as_bound']

    # matched / replay / provisional / admitted source of an rva, with its kind
    def msrc(self, rva):
        r = self.rows.get(rva)
        if rva in self.pend and not (r and r[3] == 'C4' and 'c4=match' in r[8]):
            return self.pend[rva], 'match'
        if not r:
            return None, None
        if r[3] == 'C4' and 'c4=frida-replay' in r[8] and 'c4=match' not in r[8]:
            hits = glob.glob('re/match/*/%s.c' % rva)
            if len(hits) == 1:
                return hits[0].replace(os.sep, '/'), 'replay'
        if (rva in PROV_BIND or rva in self.P['prov_bind']) and rva in self.prov and os.path.exists(self.prov[rva][2]):
            return self.prov[rva][2], 'provisional'
        if 'c4=match' in r[8] and ('BYTE-IDENTICAL' in r[8] or 'match evidence: identical' in r[8]
                                   or ('100.0%' in r[8] and 'relocheck OK' in r[8])):
            ps = [m.group(1) for m in re.finditer(r'(re/match/[^\s;,()]+?\.c)', r[8]) if os.path.exists(m.group(1))]
            ps.sort(key=lambda q: not q.endswith('/%s.c' % rva))   # prefer the note's re/match/<d>/<rva>.c
            if ps:
                return ps[0], 'match'
        if rva in self.adm and os.path.exists(self.adm[rva][2]):
            return self.adm[rva][2], 'admitted'
        return None, None

    @staticmethod
    def callees(path, rva):
        t = rd(path)
        cs = sorted(set(c.lower() for c in re.findall(r'FUN_([0-9a-fA-F]{8})', t)) - {rva})
        local = set()
        for c in cs:
            if re.search(r'^[A-Za-z_][^\n;=(]*\bFUN_%s\s*\([^;{]*\)\s*\{' % c, t, re.I | re.M):
                local.add(c)
        return [c for c in cs if c not in local], sorted(local), t

    def reuse(self, rva):
        """existing exe body usable as-is: a CRT body, a PortTop / RwCore match-include body, or the body of a leaf
        source (a match-include, or an rh C function of a C4 row)."""
        if rva in self.crt:
            return self.crt[rva]
        for sym, f, how in self.idx.get(rva, []):
            if f.startswith('Match/PortTop_') and how == 'match-include':
                return sym, f
        for sym, f, how in self.idx.get(rva, []):
            if f.startswith('RwCore/') and how == 'match-include':
                return sym, f
        src, kind = self.msrc(rva)
        if src:
            cs, _, _ = self.callees(src, rva)
            if not cs:
                for sym, f, how in self.idx.get(rva, []):
                    if how == 'match-include':
                        return sym, f
                r = self.rows.get(rva)
                if r and r[3] == 'C4':
                    for sym, f, how in self.idx.get(rva, []):
                        if how == 'rh' and '::' not in sym:
                            return sym, f
        return None

    def build_plan(self):
        P, plan, q = self.P, {}, list(self.roots)
        while q:
            rva = q.pop(0)
            if rva in plan:
                continue
            if rva in P['stop_force']:
                plan[rva] = ('stop',); continue
            if rva in self.lib:
                # BIND-LIB-ROUTE: the vendored library's own symbol is the rva's one exe body; it wins over every rule
                # below (a library rva never gets a PortTop wrapper, a matched source bind or a named stop)
                plan[rva] = ('reuse',) + self.lib[rva]; continue
            if P['bound_wins'] and rva not in self.force and rva not in self.alias:
                # BIND-BULK-PROFILE: a body another profile bound wins over every stop rule below (the audio profile's
                # PortTop_Bound_005aba20 under the bulk 0x005c91f0 wrapper; raceload's C units of cpp-unclean rvas)
                pt = [(s_, f_) for s_, f_, h_ in self.idx.get(rva, []) if f_.startswith('Match/PortTop_') and h_ == 'match-include']
                if pt:
                    plan[rva] = ('reuse',) + pt[0]; continue
            if rva in P['stop_reason']:
                plan[rva] = ('stop', P['stop_reason'][rva]); continue
            if rva in self.unclean and rva not in self.cemit:
                plan[rva] = ('stop', 'cpp-unclean: ' + self.unclean[rva]); continue
            if rva in self.alias:
                plan[rva] = ('reuse',) + self.alias[rva]; continue
            ru = None if rva in self.force else self.reuse(rva)
            if not ru and P['rwcore_fallback'] and rva not in self.force and os.path.exists(SRC + '/RwCore/RwCore_%s.cpp' % rva):
                ru = ('RwCore_%s' % rva, 'RwCore/RwCore_%s.cpp' % rva)   # the RW core's one exe body per rva (L-124)
            if ru:
                plan[rva] = ('reuse', ru[0], ru[1]); continue
            s, kind = self.msrc(rva)
            if s and P['roots_only_reason'] and rva not in self.roots:
                plan[rva] = ('stop', P['roots_only_reason'] % s); continue
            if s and self.roots_only and rva not in self.roots and (self.old_plan.get(rva) or ['x'])[0] != 'bind':
                plan[rva] = ('stop', 'not bound here (--roots-only): matched source %s' % s); continue
            if s and rva in self.reasons and rva not in self.force:
                plan[rva] = ('stop', self.reasons[rva]); continue
            if s and P['static_stop']:
                m_ = self.static_line(s, rva)
                if m_:
                    plan[rva] = ('stop', 'static in %s:%d (file-local there): no linkable exe body' % (s, m_)); continue
            if s:
                cs, local, _ = self.callees(s, rva)
                # a callee defined only under #ifndef MATCH_PORT (a HELD copy the DDK needs) is external on the port
                for c in [c for c in local if (rva, c) in P['not_local']]:
                    local = [x for x in local if x != c]; cs = sorted(set(cs) | {c})
                if P['held_external']:   # the same, found from the source's #ifndef MATCH_PORT branches
                    for c in self.held_defs(s, local):
                        local = [x for x in local if x != c]; cs = sorted(set(cs) | {c})
                plan[rva] = ('bind', s, kind, cs, local)
                q.extend(cs)
                continue
            if P['reuse_any'] and any(h == 'match-include' for _, _, h in self.idx.get(rva, [])):
                for sym, f, how in self.idx.get(rva, []):
                    if how == 'match-include':
                        plan[rva] = ('reuse', sym, f); break
                continue
            plan[rva] = ('stop',)
        if P['samesrc_reuse']:
            # BIND-BULK-PROFILE (bulk_bind_gen.py pass 1): the matched source is already compiled by an exe .cpp TU outside
            # this profile's own wrappers: that TU carries the rva, a second copy would duplicate it (rva-lint)
            inc = {}
            for tu in rsp_tus():
                p_ = os.path.join(SRC, tu)
                if not tu.endswith('.cpp') or not os.path.exists(p_):
                    continue
                t_ = rd(p_)
                if P['own_mark'] and t_.startswith(P['own_mark']):
                    continue
                for m_ in re.finditer(r'#include\s+"(?:\.\./)+(re/match/[^"]+)"', t_):
                    inc.setdefault(m_.group(1), []).append((tu, t_))
            for rva, p in list(plan.items()):
                if p[0] != 'bind':
                    continue
                for tu, t_ in inc.get(p[1], []):
                    m_ = re.search(r'#define\s+FUN_%s\s+(\w+)' % rva, t_)
                    plan[rva] = ('reuse', m_.group(1) if m_ else 'FUN_%s' % rva, tu)
                    if rva in self.roots:
                        self.refused[rva] = 'already carried by %s (same source %s)' % (tu, p[1])
                    break
        if P['group_same_source']:
            groups = {}
            for k, v in sorted(plan.items()) if P['group_primary'] == 'lowest' else plan.items():
                if v[0] == 'bind':
                    groups.setdefault(v[1], []).append(k)
            for s, ks in sorted(groups.items()):
                if len(ks) < 2:
                    continue
                own = sorted(ks) if P['group_primary'] == 'lowest' else \
                    ([k for k in ks if re.search(r'[/_]%s\.c$' % k, s)] or sorted(ks))
                o = own[0]
                for m_ in sorted(ks):
                    if m_ != o and m_ not in self.force:
                        plan[m_] = ('reuse', 'PortTop_Bound_%s' % m_, 'Match/PortTop_%s.cpp' % o)
                        self.group_bound.add((o, m_))
        if P['drop_local_copies']:
            # BIND-BULK-PROFILE (bulk_bind_gen.py pass 3): one body per RVA. A planned bind that the source of another
            # planned bind carries as a file-local copy becomes a named stop (iterates the plan as it was before the pass)
            for rva, p in list(plan.items()):
                if p[0] != 'bind':
                    continue
                for c in p[4]:
                    if (rva, c) in self.local_shared or (rva, c) in self.local_as_bound or (rva, c) in self.group_bound:
                        continue
                    if plan.get(c, ('x',))[0] == 'bind':
                        plan[c] = ('stop', 'file-local copy of this rva in the source of 0x%s (one body per RVA)' % rva)
                        if c in self.roots:
                            self.refused[c] = ('file-local static copy inside the source of 0x%s; a second body of the rva '
                                               'would fail rva-lint' % rva)
        self.plan = plan
        return plan

    @staticmethod
    def held_defs(src, local):
        """locals whose every definition sits in the `#ifndef MATCH_PORT` branch (before its #else / #endif): the port
        does not compile them, so the port's call goes to an external body (PORT-TOP-06)."""
        lines = rd(src).splitlines()
        held, stk = [False] * len(lines), []
        for i, l in enumerate(lines):
            s = l.strip()
            if re.match(r'#\s*if', s):
                stk.append(bool(re.match(r'#\s*ifndef\s+MATCH_PORT\b', s)))
            elif re.match(r'#\s*el', s) and stk:
                stk[-1] = False
            elif re.match(r'#\s*endif', s) and stk:
                stk.pop()
            held[i] = any(stk)
        out = []
        for c in local:
            defs = [i for i, l in enumerate(lines)
                    if re.match(r'^[A-Za-z_][^;=(]*\bFUN_%s\s*\([^;{]*\)\s*\{?\s*$' % c, l, re.I)
                    and ('{' in l or (i + 1 < len(lines) and lines[i + 1].strip().startswith('{')))]
            if defs and all(held[i] for i in defs):
                out.append(c)
        return out

    @staticmethod
    def static_line(src, rva):
        t = rd(src)
        m = re.search(r'^[ \t]*static\b[^\n;]*\bFUN_%s\s*\([^;{]*\)\s*\{' % rva, t, re.M | re.I)
        return t[:m.start()].count('\n') + 1 if m else 0

    def sym(self, rva):
        p = self.plan[rva]
        if p[0] == 'reuse':
            return p[1]
        if p[0] == 'bind':
            if rva in self.P['root_external']:
                return self.P['root_external'][rva]['sym']
            return 'PortTop_Bound_%s' % rva
        if self.P['stop_sym_foreign']:
            return self.foreign_stops().get(rva) or self.P['stop_prefix'] + rva
        return self.P['stop_prefix'] + rva

    def foreign_stops(self):
        """stop_sym_foreign (bulk_bind_gen.py existing_stop): rva -> the first `PortTop_*Stop*_<rva>(` function another
        Match/PortTop_*.cpp file defines or calls (glob order, first file wins), this profile's own stop file left out."""
        if getattr(self, '_fstops', None) is None:
            own = os.path.normcase(os.path.join(SRC, self.P['stop_file']))
            self._fstops = {}
            for f in glob.glob(SRC + '/Match/PortTop_*.cpp'):
                if os.path.normcase(f) == own:
                    continue
                for m in re.finditer(r'\b(PortTop_\w*Stop\w*_([0-9a-f]{8}))\s*\(', rd(f)):
                    self._fstops.setdefault(m.group(2), m.group(1))
        return self._fstops

    def listing(self):
        plan = self.plan
        out = ['%s %s' % (Counter(p[0] for p in plan.values()), Counter(p[2] for p in plan.values() if p[0] == 'bind'))]
        for rva in sorted(plan):
            p = plan[rva]
            r = self.rows.get(rva) or ['', '', '', '-']
            if self.P['listing'] == 'full':
                extra = p[1] if len(p) > 1 else ''
                out.append('%s %s %s %s %s' % (rva, p[0], extra, r[3], (r[1] if len(r) > 1 else '')))
            else:
                out.append('%s %s %s %s' % (rva, p[0], p[1] if p[0] != 'stop' else '', r[3]))
        return out

    # ----------------------------------------------------------------------------------------------------------------
    # Writer
    # ----------------------------------------------------------------------------------------------------------------
    def evidence_text(self, rva, kind):
        if kind == 'match':
            return 'match.py BYTE-IDENTICAL, hooks.csv C4 c4=match'
        if kind == 'admitted':
            return 'L-126 admitted (re/match/ADMITTED.tsv, %s)' % self.adm[rva][6]
        if kind == 'replay':
            return 'hooks.csv C4 c4=frida-replay (L-128)'
        if kind == 'provisional':
            return 'L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4'
        return kind

    def header(self, rva, src, kind, fn):
        ev = self.evidence_text(rva, kind)
        fill = dict(mark=self.P['own_mark'], rva=rva, src=src, ev=ev)
        if self.P['hdr']:   # profile.json hdr: one template for every wrapper
            return [l % fill for l in self.P['hdr']]
        # profile.json header, style keep (audio): an existing wrapper keeps the header its row wrote; new ones say which
        # root closure reached them
        H = self.P['header']
        keep_hdr = None
        if os.path.exists(fn):
            keep_hdr = []
            for l in rd(fn).splitlines():
                if not l.startswith('//'):
                    break
                keep_hdr.append(l)
        old_binds = {k for k, v in self.old_plan.items() if v and v[0] == 'bind'}
        table_new = rva not in old_binds
        L = []
        if keep_hdr and rva in H['keep_roots']:
            L += [l for l in keep_hdr if not l.startswith('// RVA-LINT')]
            for x in H['keep_roots_add']:
                if x not in L:
                    L.append(x)
        elif keep_hdr and not table_new:
            L += [l for l in keep_hdr if not l.startswith('// RVA-LINT')]
        elif table_new:   # a new bind
            L += [l % fill for l in H['new']]
        else:   # an earlier bind whose wrapper file is missing
            # (the old generator's third branch, `rva not in old_plan`, could not be reached: table_new covers it)
            L += [l % fill for l in H['missing']]
        return L

    def wrapper_text_bulk(self, rva):
        """wrapper_style bulk (bulk_bind_gen.py, RACE-BIND-C4-BULK checkpoint 1): the profile's hdr lines, callees by name,
        group members of the same source defined here, the source's one thunk_FUN_ body name, and a PortTop_Keep_<rva>
        anchor for the rva and each group member (the raceload refs file /includes them while no bound caller exists)."""
        P = self.P
        _, src, kind, cs, local = self.plan[rva]
        fn = self.wrapper_path(rva)
        fill = dict(mark=P['own_mark'], rva=rva, src=src, ev=self.evidence_text(rva, kind))
        L = [l % fill for l in P['hdr']]
        L += ['// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#define MATCH_PORT 1', '#define FUN_%s PortTop_Bound_%s' % (rva, rva)]
        for c in cs:
            if (rva, c) in P['callee_override']:
                L.append('#define FUN_%s %s   // %s' % ((c,) + P['callee_override'][(rva, c)]))
                continue
            pc = self.plan[c]
            note = {'reuse': '   // exe body (%s)' % (pc[2] if pc[0] == 'reuse' else ''), 'bind': '', 'stop': '   // named stop'}[pc[0]]
            L.append('#define FUN_%s %s%s' % (c, self.sym(c), note))
        for c in local:
            if (rva, c) in self.local_shared:
                L.append('#define FUN_%s %s%s' % (c, self.local_shared[(rva, c)], P['local_shared_note']))
            elif (rva, c) in self.local_as_bound:
                L.append('#define FUN_%s PortTop_Bound_%s%s' % (c, c, P['local_bound_note']))
            else:
                L.append('#define FUN_%s PortTop_Local_%s_%s   // file-local in this source' % (c, rva, c))
        st = rd(src)
        fundef = r'^[A-Za-z_][^\n;=(]*\bFUN_%s\s*\([^;{]*\)\s*\{'
        done = {rva} | set(cs) | set(local)
        if not re.search(fundef % rva, st, re.I | re.M):
            th = re.findall(r'^[A-Za-z_][^\n;=(]*\b(thunk_FUN_[0-9a-fA-F]{8})\s*\([^;{]*\)\s*\{', st, re.I | re.M)
            if len(th) == 1:
                L.append('#define %s PortTop_Bound_%s   // the source names this body %s (a jump thunk)' % (th[0], rva, th[0]))
        members = sorted(m_ for o_, m_ in self.group_bound if o_ == rva)
        for c in members:
            L.append('#define FUN_%s PortTop_Bound_%s   // member of this source, its only exe copy' % (c, c))
            done.add(c)
        for c in sorted(set(x.lower() for x in re.findall(r'FUN_([0-9a-fA-F]{8})', st)) - done):
            if re.search(fundef % c, st, re.I | re.M):
                L.append('#define FUN_%s PortTop_Local_%s_%s   // defined in this source, not a bound rva' % (c, rva, c))
        if 'FID_conflict__wprintf' in st:
            L.append('#define FID_conflict__wprintf PortTop_Printf04G   // printf -> log (Match/PortTop_0040bbb0.cpp)')
        L += self.extra_pre.get(rva, [])
        L.append('extern "C" {')
        L += self.extra_in.get(rva, [])
        L += ['#include "../../../../%s"' % src, '}']
        # a body the matched source defines `static` has no external symbol and no caller in its own TU, so the compiler
        # drops it: take its address once (the object build then promotes it, ddkobj.py "externalized static")
        for c in [rva] + members:
            L.append('extern "C" { void *PortTop_Keep_%s = (void *)PortTop_Bound_%s; }   // anchor: BulkRefs /include%s' % (
                c, c, ' (the source defines it static)' if re.search(r'^\s*static\b[^\n;{(]*\bFUN_%s\s*\(' % c, st, re.I | re.M) else ''))
        L.append('#endif // MASHED_STANDALONE')
        return fn, NL.join(L) + NL

    def keep_list(self):
        """wrapper_style bulk: every bound rva and group member carries a PortTop_Keep_<rva> anchor."""
        return sorted({r for r, p in self.plan.items() if p[0] == 'bind' and r not in self.P['root_external']}
                      | {m_ for _, m_ in self.group_bound})

    def wrapper_text(self, rva):
        P = self.P
        if P['wrapper_style'] == 'bulk':
            return self.wrapper_text_bulk(rva)
        _, src, kind, cs, local = self.plan[rva]
        is_c = rva in self.cemit
        fn = self.wrapper_path(rva)
        L = self.header(rva, src, kind, fn)
        if is_c:
            # BIND-CUNCLEAN-AS-C: the source does not compile as C++ under MATCH_PORT (cpp_unclean.txt), so the unit is C.
            # cl compiles a .c by its extension (no /TP in build_objs.ps1 or ddkobj.py). Same exe symbol: a C unit's
            # PortTop_Bound_<rva> has C linkage like the extern "C" include of a .cpp wrapper. Every callee below binds
            # to an exe body defined with C linkage (PortTop_Bound_* / stops / Orig_* / RwCore_* are extern "C").
            L.append('// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails')
            L.append('// with %s (state/%s/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.'
                     % (self.unclean[rva].split()[0], self.name))
        L += ['// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#define MATCH_PORT 1', '#define FUN_%s PortTop_Bound_%s' % (rva, rva)]
        if rva in P['name_def']:
            L.append('#define %s PortTop_Bound_%s   // the source names 0x%s %s' % (P['name_def'][rva], rva, rva, P['name_def'][rva]))
        L += P['extra_def'].get(rva, [])
        for c in cs:
            if (rva, c) in P['callee_override']:
                L.append('#define FUN_%s %s   // %s' % ((c,) + P['callee_override'][(rva, c)]))
                continue
            pc = self.plan[c]
            note = {'reuse': '   // exe body (%s)' % (pc[2] if pc[0] == 'reuse' else ''), 'bind': '', 'stop': '   // named stop'}[pc[0]]
            if c in P['stop_force'] and P['stop_force_note'] is not None:
                note = P['stop_force_note']
            if c in P['knob'] and pc[0] == 'stop':
                L.append('#define FUN_%s %s   // knob stand-in, default off = the named stop (%s)' % (c, P['knob'][c][0], P['knob'][c][1]))
                continue
            ex = call_excess(rd(src), c) if is_c else None
            if ex:
                # C rejects a call with more arguments than the source's own prototype (C2197, VS2022); the DDK object
                # pushes them (caller cleans, cdecl) and the callee does not read them. The function-like form drops the
                # extra ones (ddkobj.py fnlike_target binds it, as Match/PortTop_004039c0.cpp's FUN_0042a5d0).
                n, mx = ex
                ps = ['a%d' % k for k in range(n)]
                L.append('#define FUN_%s(%s) %s(%s)   // %sthe source passes %d argument(s), its prototype declares %d'
                         % (c, ', '.join(ps + ['...']), self.sym(c), ', '.join(ps),
                            (note.strip()[3:] + '; ') if note else '', mx, n))
                continue
            L.append('#define FUN_%s %s%s' % (c, self.sym(c), note))
        for c in local:
            if (rva, c) in self.local_shared:
                L.append('#define FUN_%s %s%s' % (c, self.local_shared[(rva, c)], P['local_shared_note']))
            elif (rva, c) in self.group_bound:
                L.append('#define FUN_%s PortTop_Bound_%s   // bound rva of the same source: this TU carries its only exe copy' % (c, c))
            elif (rva, c) in self.local_as_bound:
                n = P['local_bound_note'] % c if '%s' in P['local_bound_note'] else P['local_bound_note']
                L.append('#define FUN_%s PortTop_Bound_%s%s' % (c, c, n))
            else:
                L.append('#define FUN_%s PortTop_Local_%s_%s   // file-local in this source' % (c, rva, c))
        if P['thunk_names']:
            # Ghidra `thunk_FUN_<x>` names a thunk that jumps to x; the port calls x's exe body (as extra_pre 005aafe0 does)
            st = rd(src)
            for c in cs:
                if re.search(r'\bthunk_FUN_%s\b' % c, st) and not re.search(r'^[A-Za-z_][^;=(\n]*\bthunk_FUN_%s\s*\([^;{]*\)\s*\{' % c, st, re.M):
                    L.append('#define thunk_FUN_%s %s   // Ghidra thunk name of the jump to 0x%s: the port calls its exe body' % (c, self.sym(c), c))
        if 'FID_conflict__wprintf' in rd(src):
            L.append('#define FID_conflict__wprintf PortTop_Printf04G   // printf -> log (Match/PortTop_0040bbb0.cpp)')
        L += self.extra_pre.get(rva, [])
        keep = []
        if rva in self.refs() and rva in self._refs_own:
            # `refs` step: no bound caller references this body yet, so /OPT:REF would drop it (ddkobj.py check: not-in-map).
            # The data anchor is named by Match/PortTop_BulkRefs.cpp's /include (decoration-free, works for a static body).
            a = 'void *PortTop_Keep_%s = (void *)PortTop_Bound_%s;' % (rva, rva)
            keep = [(a if is_c else 'extern "C" { %s }' % a) + '   // anchor: %s /include (no bound caller yet)'
                    % self.P['refs_file']]
        if is_c:
            L += self.extra_in.get(rva, [])
            L += ['#include "../../../../%s"' % src] + keep + ['#endif // MASHED_STANDALONE']
            return fn, NL.join(L) + NL
        L.append('extern "C" {')
        L += self.extra_in.get(rva, [])
        L += ['#include "../../../../%s"' % src, '}'] + keep + ['#endif // MASHED_STANDALONE']
        return fn, NL.join(L) + NL

    def refs(self):
        """`refs` step (BIND-CUNCLEAN-AS-C): the bound rvas no bound caller references. A bound caller is a planned bind
        that lists the rva as a callee, or an exe TU outside this profile's own wrappers and its refs file that names
        PortTop_Bound_<rva>. An rva drops out of the set (and its anchor out of its wrapper) once such a caller exists."""
        if getattr(self, '_refs', None) is not None:
            return self._refs
        self._refs_own = set()
        if not self.P.get('refs_file'):
            self._refs = []
            return self._refs
        binds = {k for k, v in self.plan.items() if v[0] == 'bind'}
        called = {c for k in binds for c in self.plan[k][3]}
        cand = sorted(binds - called)
        rf = os.path.normcase(os.path.join(SRC, self.P['refs_file']))
        hits = set()
        pat = re.compile(r'\bPortTop_Bound_([0-9a-f]{8})\b')
        fmarks = tuple(self.P.get('foreign_keep_marks', ()))
        foreign = set()
        for f in rsp_tus():
            p = os.path.join(SRC, f)
            if os.path.normcase(p) == rf or not os.path.exists(p):
                continue
            t = rd(p)
            if self.P['own_mark'] and t.startswith(self.P['own_mark']):
                continue
            if fmarks and t.startswith(fmarks):
                # a foreign keep-alive group (RACE-BIND-C4-BULK checkpoint 1, its generator committed history only): its
                # PortTop_Keep_<rva> anchors are listed like this profile's own caller-less binds. Calls inside the group
                # do not count as a bound caller, because the group itself is kept only by this file.
                foreign |= set(re.findall(r'\bPortTop_Keep_([0-9a-f]{8})\s*=', t))
                continue
            hits |= set(pat.findall(t))
        own = [r for r in cand if r not in hits]
        self._refs_own = set(own)
        # a foreign anchor leaves the list once a bound caller exists (a planned bind calls it, or another exe TU names it)
        self._refs = sorted(set(own) | (foreign - called - hits - binds))
        return self._refs

    def refs_text(self):
        L = ['// Generated by re/tools/bind/porttop_bind.py refs (profile %s). Do not edit by hand: re-run `write` or `refs`.' % self.name,
             '// Keeps every bound body that no bound caller references yet linked in the exe (without a reference /OPT:REF',
             '// drops it and ddkobj.py check reports the unit not-in-map). Each /include names the PortTop_Keep_<rva> data anchor',
             '// its wrapper defines. An rva leaves this file once a bound caller exists (BIND-CUNCLEAN-AS-C, 2026-11-21).',
             '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE']
        L += ['#pragma comment(linker, "/include:_PortTop_Keep_%s")' % r for r in self.refs()]
        L.append('#endif // MASHED_STANDALONE')
        return NL.join(L) + NL

    def wrapper_path(self, rva):
        return '%s/Match/PortTop_%s.%s' % (SRC, rva, 'c' if rva in self.cemit else 'cpp')

    def stops_text(self):
        stops = sorted(r for r, p in self.plan.items() if p[0] == 'stop')
        if self.P['stops']['style'] == 'exit':   # profile.json stops.style exit (audio): every stop logs and exits
            S = list(self.P['stops']['head']) + [
                 '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#include <intrin.h>', 'extern "C" {',
                 'void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp']
            for s in stops:
                S.append('void %s%s(void) { PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); }' % (self.P['stop_prefix'], s, s))
            S += ['}', '#endif // MASHED_STANDALONE']
            return stops, NL.join(S) + NL
        plan = self.plan

        def ret_types(stop):   # return type of each stop as its callers' sources declare it
            ts = set()
            for rva, p in plan.items():
                if p[0] != 'bind' or stop not in p[3]:
                    continue
                for m in re.finditer(r'^\s*(?:extern\s+)?([A-Za-z_][\w\s\*]*?)\s*\bFUN_%s\s*\(' % stop, rd(p[1]), re.M | re.I):
                    ts.add(' '.join(m.group(1).split()))
            return ts

        def callers(stop):
            return sorted(r for r, p in plan.items() if p[0] == 'bind' and stop in p[3])
        S = ['// PORT-OT-RACE (2026-11-16): named stops of the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py).',
             '// Each is a callee with no matched (c4=match) or L-126 admitted body yet. None is a body of its rva.',
             '// Default: a reached stop logs and exits with code = rva (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so it',
             '// names the next body to bind. MASHED_OT_RL_STOPS=count: a stop logs its first call and returns 0 (eax), so the load',
             '// runs on past it with that callee\'s effects missing; stops declared float / double by a caller always exit.',
             '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#include <intrin.h>', '#include <cstdio>', '#include <cstdlib>',
             'namespace {', 'int RlStopCount() {', '    static int s = -1;',
             '    if (s < 0) { const char* const v = std::getenv("MASHED_OT_RL_STOPS"); s = (v && v[0] == \'c\') ? 1 : 0; }',
             '    return s;', '}', 'unsigned g_rlStopHits = 0;',
             'void RlStopLog(unsigned rva, const char* callers, unsigned n, void* ret, int exits) {',
             '    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {',
             '        std::fprintf(f, "[ORIG_TOP] rva 0x%08X: race-load stop (PORT-OT-RACE), call %u, callers %s, return %p, %s\\n", rva, n, callers, ret,',
             '                     exits ? "exits" : "returns 0 (MASHED_OT_RL_STOPS=count)");', '        std::fclose(f);', '    }', '}', '} // namespace',
             'extern "C" {',
             'void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp',
             'unsigned PortTop_RL_StopHits(void) { return g_rlStopHits; }']
        for s_ in stops:
            rt = ret_types(s_)
            fl = any(('float' in x or 'double' in x) for x in rt)
            cl = ' '.join('0x' + c for c in callers(s_))
            S.append('// stop rva %s: declared %s by %s%s' % (s_, ' / '.join(sorted(rt)) or '(none found)', cl,
                     ('; ' + plan[s_][1]) if len(plan[s_]) > 1 else '; no matched or admitted source'))
            if fl:
                S.append('int %s%s(void) { RlStopLog(0x%su, "%s", 1u, _ReturnAddress(), 1); PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); return 0; }' % (self.P['stop_prefix'], s_, s_, cl, s_))
            else:
                S.append('int %s%s(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();' % (self.P['stop_prefix'], s_))
                S.append('    if (n == 1 || ex) RlStopLog(0x%su, "%s", n, _ReturnAddress(), ex);' % (s_, cl))
                S.append('    if (ex) PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); return 0; }' % s_)
        S += ['}', '#endif // MASHED_STANDALONE']
        text = NL.join(S) + NL
        for a_, b_ in self.P['stops_subst']:   # profile.json stops_subst: the count text of another closure
            text = text.replace(a_, b_)
        return stops, text

    def write_selectors(self, rva, cfg, changed):
        """root_external (PORT-TOP-RENDERFRAME-492E90): the root already has one ORIGTOP-only exe body (cfg['tu'], the
        RW core TU of the same matched source), and a second wrapper would be a dual copy of the rva (rva-lint
        DUP-IN-TARGET, untaggable). So the TU stays the only body and each of its direct callees whose walked symbol
        differs from the TU's symbol is routed through a selector (cfg['sel_file']): the walked symbol when the sub-knob
        cfg['knob'] is set, the TU's own symbol otherwise (knob off = unchanged). Returns the files to list in newfiles."""
        p = self.plan[rva]
        src, cs = p[1], sorted(set(p[3]) | set(p[4]))
        tu = '%s/%s' % (SRC, cfg['tu'])
        raw = open(tu, 'rb').read()
        tl = raw.decode('utf-8').replace('\r\n', '\n').split('\n')
        st = rd(src)
        pre, tag = cfg['sel_prefix'], self.P['own_tag']
        on = pre.rstrip('_') + 'On'   # PortTop_RRSel_ -> PortTop_RRSelOn
        clo = cfg.get('closure', 'frame-render')
        sels = []
        for i, l in enumerate(tl):
            m = re.match(r'#define FUN_([0-9a-f]{8}) (\w+)(.*)$', l)
            if not m or m.group(1) not in cs:
                continue
            c, cur, rest = m.group(1), m.group(2), m.group(3)
            old = cur
            if cur == pre + c:
                mo = re.search(r'// was (\w+)', rest)
                old = mo.group(1) if mo else cur
            new = self.sym(c)
            # a callee the RW core TU already binds to an RW core body keeps it (L-124: RW on the top-down path is the
            # matched RW core); only RwCoreMissing_ stops get a selector
            if new == old or (old.startswith('RwCore_') and not old.startswith('RwCoreMissing_')):
                tl[i] = '#define FUN_%s %s' % (c, old)
                continue
            mp = re.search(r'^[ \t]*([A-Za-z_][\w \t\*]*?)[ \t\*]*\bFUN_%s\s*\(([^)]*)\)\s*;' % c, st, re.M)
            if not mp:
                print('SELECTOR no prototype for FUN_%s in %s: kept %s' % (c, src, old)); continue
            ret = re.search(r'^[ \t]*(.*?)\bFUN_%s\s*\(' % c, mp.group(0)).group(1).strip()
            params = mp.group(2).strip()
            args = '' if params in ('', 'void') else ', '.join(re.findall(r'(\w+)\s*(?:,|$)', params))
            tl[i] = '#define FUN_%s %s%s   // was %s; %s: %s=1 calls %s' % (c, pre, c, old, tag, cfg['knob'], new)
            sels.append((c, ret, params or 'void', args, old, new))
        tt = '\n'.join(tl)
        if write_if_changed(tu, tt):
            changed.append(tu)
        S = ['%s: selectors of the direct callees of 0x%s (re/tools/bind/porttop_bind.py --profile %s).' % (self.P['own_mark'], rva, self.name),
             '// %s is the one exe body of 0x%s (%s). Each callee below whose walked binding' % (cfg['tu'], rva, src),
             '// differs from that TU\'s own symbol goes through a selector: %s=1 calls the walked body (the %s' % (cfg['knob'], clo),
             '// closure of 0x%s, Match/PortTop_<rva>.cpp, or its named stop in %s); unset or 0 calls the TU\'s' % (rva, self.P['stop_file']),
             '// own symbol, so the knob-off path is unchanged. The TU runs only on the RW core frame route (%s).' % cfg['gate'],
             '// RVA-LINT: ORIGTOP-ONLY',
             '#ifdef MASHED_STANDALONE',
             '#include <windows.h>',
             'extern "C" void RwCore_Log(const char* fmt, ...);   // C linkage (RwCore/RwCore_Knob.cpp:28-53)',
             'static int s_knob = -1;',
             'static bool %s(void) {' % on,
             '    if (s_knob < 0) {',
             '        char v[8] = {};',
             '        s_knob = (GetEnvironmentVariableA("%s", v, sizeof(v)) > 0 && v[0] && v[0] != \'0\') ? 1 : 0;' % cfg['knob'],
             '        RwCore_Log("[%s] %s %%s: the direct callees of 0x%s %%s", s_knob ? "on" : "off",' % (tag, cfg['knob'], rva),
             '                   s_knob ? "call the walked %s closure" : "keep the RW core TU\'s own symbols");' % clo,
             '    }',
             '    return s_knob == 1;',
             '}',
             'extern "C" {']
        seen = set()
        for c, ret, params, args, old, new in sels:
            for s_ in (old, new):
                if s_ not in seen:
                    seen.add(s_); S.append('%s %s(%s);' % (ret, s_, params))
        for c, ret, params, args, old, new in sels:
            if ret == 'void':
                S.append('void %s%s(%s) { if (%s()) { %s(%s); return; } %s(%s); }   // 0x%s' % (
                    pre, c, params, on, new, args, old, args, c))
            else:
                S.append('%s %s%s(%s) { if (%s()) return %s(%s); return %s(%s); }   // 0x%s' % (
                    ret, pre, c, params, on, new, args, old, args, c))
        S += ['}', '#endif // MASHED_STANDALONE']
        sf = '%s/%s' % (SRC, cfg['sel_file'])
        if write_if_changed(sf, NL.join(S) + NL):
            changed.append(sf)
        print('selectors', len(sels), 'of', len(cs), 'callees of 0x%s' % rva)
        return [sf]


    def write(self):
        changed, out, P_ = [], [], self.P
        if self.P['prune_stale']:
            # remove this profile's wrappers whose rva is no longer bound (and their exe_sources.rsp lines)
            rr = open(RSP, 'rb').read(); nl = '\r\n' if b'\r\n' in rr else '\n'
            rl = rr.decode().split(nl); n0 = len(rl)
            for f in glob.glob(SRC + '/Match/PortTop_*.cpp') + glob.glob(SRC + '/Match/PortTop_*.c'):
                m = re.search(r'PortTop_([0-9a-f]{8})\.(cpp|c)$', f)
                if not m or not rd(f).startswith(self.P['own_mark']):
                    continue
                # BIND-CUNCLEAN-AS-C: a wrapper of the other language (.cpp <-> .c) is stale too
                if (self.plan.get(m.group(1), ('x',))[0] != 'bind' or m.group(1) in self.P['root_external']
                        or os.path.normcase(f) != os.path.normcase(self.wrapper_path(m.group(1)))):
                    os.remove(f)
                    rl = [l for l in rl if l.strip() != '"Match\\PortTop_%s.%s"' % (m.group(1), m.group(2))]
                    print('REMOVED', f)
            if len(rl) != n0:
                open(RSP, 'wb').write(nl.join(rl).encode())
        for rva, p in sorted(self.plan.items()):
            if p[0] != 'bind':
                continue
            if rva in self.P['root_external']:   # the root's one exe body is an existing TU: selectors instead
                for fn in self.write_selectors(rva, self.P['root_external'][rva], changed):
                    out.append(fn)
                continue
            fn, text = self.wrapper_text(rva)
            if self.P['own_mark'] and os.path.exists(fn) and self.P['own_tag'] not in rd(fn):
                print('REFUSE existing wrapper of another closure', fn); sys.exit(2)
            if write_if_changed(fn, text):
                changed.append(fn)
            out.append(fn)
        stops, st = self.stops_text()
        sf = '%s/%s' % (SRC, self.P['stop_file'])
        if write_if_changed(sf, st):
            changed.append(sf)
        extra = [sf]
        if self.P.get('refs_file'):
            rf = '%s/%s' % (SRC, self.P['refs_file'])
            if write_if_changed(rf, self.refs_text()):
                changed.append(rf)
            extra.append(rf)
            print('refs', len(self.refs()), 'bound bodies with no bound caller ->', rf)
        json.dump({k: list(v) for k, v in self.plan.items()}, open(self.plan_path, 'w'), indent=0)
        if P_['wrapper_style'] == 'bulk':   # bulk_bind_gen.py's disposition_cp.json: unbound roots, groups, anchors
            grp = {}
            for o_, m_ in sorted(self.group_bound):
                grp.setdefault(o_, []).append(m_)
            json.dump({'refused': self.refused, 'group': grp, 'keep': self.keep_list(), 'unclean': self.unclean},
                      open(os.path.join(self.sd, 'disposition.json'), 'w'), indent=0)
        open(os.path.join(self.sd, 'newfiles.txt'), 'w').write(NL.join(out + extra) + NL)
        print('wrote', len(out), 'bind files +', len(stops), 'stops;', len(changed), 'files changed')
        for f in changed:
            print('CHANGED', f)
        return changed


# --------------------------------------------------------------------------------------------------------------------
# Extern-guard step (was rltools/guard_externs.py)
# --------------------------------------------------------------------------------------------------------------------
EXTERN_PAT = re.compile(r'^(\s*)extern\s+((?:const\s+)?(?:struct\s+)?[A-Za-z_]\w*(?:\s*\*+)?)\s*(_?(?:DAT|LAB|PTR(?:_\w+?)?)_([0-9a-fA-F]{8}))'   # PTR_<x>_<addr>: BIND-CUNCLEAN-AS-C (0x005518f0)
                        r'\s*(\[\s*([0-9xXa-fA-F]*)\s*\])?\s*;\s*$')


def _call_args(text, start):
    """top-level argument count of the call whose '(' is at text[start] (None when unbalanced)."""
    depth, n, seen = 0, 0, False
    for k in range(start, len(text)):
        ch = text[k]
        if ch == '(':
            depth += 1
            if depth == 1:
                continue
        elif ch == ')':
            depth -= 1
            if depth == 0:
                return n + 1 if seen else 0
        elif ch == ',' and depth == 1:
            n += 1
            continue
        if depth >= 1 and not ch.isspace():
            seen = True
    return None


def call_excess(text, c):
    """(prototype params, max call args) when a call of FUN_<c> in the source passes more arguments than the source's
    own prototype of it declares; None otherwise (no prototype, a variadic one, or no excess)."""
    name = 'FUN_' + c

    def is_proto(l):
        return bool(PROTO_PAT.match(l)) and (l.split() or [''])[0] not in ('return', 'else', 'do', 'case', 'goto')
    proto = None
    for l in text.splitlines():
        if is_proto(l) and re.search(r'\b%s\s*\(' % name, l, re.I):
            inner = l[l.lower().index(name.lower()) + len(name):]
            inner = inner[inner.index('(') + 1:inner.rindex(')')].strip()
            if '...' in inner:
                return None
            proto = 0 if inner in ('', 'void') else inner.count(',') + 1
    if proto is None:
        return None
    mx = -1
    for m in re.finditer(r'\b%s\s*\(' % name, text, re.I):
        ls = text.rfind('\n', 0, m.start()) + 1
        le = text.find('\n', m.start())
        if is_proto(text[ls:le if le >= 0 else len(text)]):
            continue
        k = _call_args(text, m.end() - 1)
        if k is not None:
            mx = max(mx, k)
    return (proto, mx) if mx > proto else None


PROTO_PAT = re.compile(r'^\s*(?:extern\s+)?[A-Za-z_][\w\s\*]*?\bFUN_[0-9a-fA-F]{8}\s*\([^;{}]*\)\s*;\s*(?://.*)?$')


def unwrap_hidden(lines):
    """BIND-CUNCLEAN-AS-C, C units only: a `#ifndef MATCH_PORT` ... `#endif` block holding only extern data declarations,
    FUN_ prototypes, blanks and // comments hides them from the port (re/match/s03/h1_0041a4a0.c lines 41-62: the C++
    exe never compiled that source, and a C unit needs the prototypes, or a float return / float argument goes through
    an implicit int declaration). The block is unwrapped: the DDK sees the same declarations in the same order (it never
    defines MATCH_PORT), and the data lines are then port-guarded by guard_externs. Returns (lines, blocks unwrapped)."""
    out, i, n = [], 0, 0
    while i < len(lines):
        if lines[i].strip() == '#ifndef MATCH_PORT':
            j = i + 1
            while j < len(lines) and not lines[j].strip().startswith('#'):
                j += 1
            body = lines[i + 1:j]
            ok = j < len(lines) and lines[j].strip().startswith('#endif') and body and all(
                not l.strip() or l.strip().startswith('//') or EXTERN_PAT.match(l) or PROTO_PAT.match(l) for l in body)
            if ok and any(EXTERN_PAT.match(l) for l in body):
                out += body
                i = j + 1
                n += 1
                continue
        out.append(lines[i])
        i += 1
    return out, n


def guard_externs(path, apply, cunit=False):
    """A matched source's plain `extern <T> DAT_/LAB_<addr>;` data declaration does not link in the exe. Put it under
    #ifndef MATCH_PORT and bind the port (MATCH_PORT, C++) to the original address. The DDK never defines MATCH_PORT,
    so its object is unchanged: run re/tools/match/match.py <src> --bytes after the edit (done by `externs --apply`)."""
    raw = open(path, 'rb').read()
    nl = '\r\n' if b'\r\n' in raw else '\n'
    lines = raw.decode('latin-1').split(nl)   # byte-preserving round trip (sources mix UTF-8 and cp1252)
    nun = 0
    if cunit:
        lines, nun = unwrap_hidden(lines)
    out, n, prev, conds = [], nun, '', []
    for l in lines:
        s = l.strip()
        if s.startswith('#if'):
            conds.append(s)
        elif s.startswith('#endif') and conds:
            conds.pop()
        m = EXTERN_PAT.match(l)
        # already port-guarded: inside any #if / #ifndef naming MATCH_PORT or MASHED_RE_PORT (the wrapper defines both)
        guarded = any('MATCH_PORT' in c or 'MASHED_RE_PORT' in c for c in conds)
        if not m or guarded or 'MATCH_PORT' in l or prev.startswith('#else'):
            out.append(l); prev = l; continue
        ind, T, name, addr, arr, N = m.groups()
        T = ' '.join(T.split())
        if arr is not None:
            d = '#define %s (*(%s (*)[%s])0x%su)' % (name, T, N or '1', addr.lower())
        else:
            d = '#define %s (*(%s *)0x%su)' % (name, T, addr.lower())
        out += ['#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */', d, '#else', l, '#endif']
        n += 1; prev = l
    if n and apply:
        open(path, 'wb').write(nl.join(out).encode('latin-1'))
    return n


def cmd_externs(a):
    b = Binder(a.profile, [])
    plan = json.load(open(b.plan_path))
    srcs = sorted({v[1] for v in plan.values() if v and v[0] == 'bind'})
    if a.src:
        srcs = a.src
    # sources of C-unit binds also get their hidden `#ifndef MATCH_PORT` declaration blocks unwrapped (unwrap_hidden)
    csrc = {v[1] for k, v in plan.items() if v and v[0] == 'bind' and k in b.cemit}
    hit = [(s, guard_externs(s, False, s in csrc)) for s in srcs]
    hit = [(s, n) for s, n in hit if n]
    for s, n in hit:
        print('EXTERNS', s, n)
    print('sources with unguarded extern data:', len(hit))
    if not a.apply:
        return 0
    bad = 0
    for s, n in hit:
        guard_externs(s, True, s in csrc)
        r = subprocess.run([sys.executable, 're/tools/match/match.py', s, '--bytes'], capture_output=True, text=True)
        ok = 'BYTE-IDENTICAL' in r.stdout
        print('GUARDED', s, n, 'match.py --bytes', 'BYTE-IDENTICAL' if ok else 'NOT IDENTICAL')
        if not ok:
            bad += 1
            print(r.stdout[-1500:])
    print('guarded', len(hit), 'sources;', bad, 'not byte-identical after the edit (revert those with git checkout)')
    return 1 if bad else 0


# --------------------------------------------------------------------------------------------------------------------
# sync: exe_sources.rsp lines for the new TUs + ORIGTOP-ONLY allowlist lines (was sync_rsp_allowlist.py / rl_sync.py)
# --------------------------------------------------------------------------------------------------------------------
def cmd_sync(a):
    P = PROFILES[a.profile]
    sd = state_dir(a.profile)
    raw = open(RSP, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
    t = raw.decode()
    add = []
    nf = os.path.join(sd, 'newfiles.txt')
    for f in (open(nf).read().split() if os.path.exists(nf) else []):   # written by `write`
        e = '"Match\\%s"' % os.path.basename(f)
        if e not in t:
            add.append(e)
    if add and not a.dry:
        if not t.endswith(nl): t += nl
        t += nl.join(add) + nl
        open(RSP, 'wb').write(t.encode())
    print('rsp +', len(add))
    seed = subprocess.run([sys.executable, 'scripts/lint_rva_bodies.py', '--seed'], capture_output=True, text=True).stdout
    raw = open(ALLOW, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
    t = raw.decode()
    have = set(l.split('#')[0].strip() for l in t.splitlines())
    fill = dict(P['allow_default'])
    if a.step: fill['step'] = a.step
    if a.date: fill['date'] = a.date
    if a.evidence: fill['evidence'] = a.evidence
    if 'tool' in fill and a.step:
        fill['tool'] = 're/tools/bind/porttop_bind.py --profile %s' % a.profile
    if not fill.get('evidence'):
        fill['evidence'] = 'verify/portorigtop_20261113/RESULT_%s.md' % fill['step']
    new, bad = [], []
    for l in seed.splitlines():
        k = l.split('#')[0].strip()
        if not k or l.startswith('#') or k in have:
            continue
        if not k.startswith('ORIGTOP-ONLY '):
            bad.append(l); continue
        new.append(k + '   # ' + P['allow_text'] % fill)
    if new and not a.dry:
        if not t.endswith(nl): t += nl
        t += nl.join(new) + nl
        open(ALLOW, 'wb').write(t.encode())
    print('allowlist +', len(new))
    for b in bad:
        print('UNTAGGABLE', b[:250])
    return 0


# --------------------------------------------------------------------------------------------------------------------
# bindings: BINDINGS.tsv rows from the profile's plan.json (was bindings_update.py)
# --------------------------------------------------------------------------------------------------------------------
def cmd_bindings(a):
    P = PROFILES[a.profile]
    plan = json.load(open(os.path.join(state_dir(a.profile), 'plan.json')))
    raw = open(BINDINGS, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
    lines = raw.decode().split(nl)
    if lines and lines[-1] == '': lines.pop()
    idx = {l.split('\t')[0].lower(): i for i, l in enumerate(lines) if l.startswith('0x')}
    if P['bindings_roots'] is not None:
        roots = P['bindings_roots']
    else:
        called = {c for v in plan.values() if v[0] == 'bind' for c in v[3] + v[4]}
        rc = P['bindings_root_caller']
        lv = int(lines[idx[rc]].split('\t')[5]) + 1 if rc in idx else 1
        roots = [(r, lv, rc or '') for r in sorted(plan) if r not in called]
    level, caller = {}, {}
    q = list(roots)
    while q:   # level / caller by BFS from the roots (default level 3 for the rest, as bindings_update.py did)
        r, lv, c = q.pop(0)
        if r in level: continue
        level[r], caller[r] = lv, c
        pr = plan.get(r)
        if pr and pr[0] == 'bind':
            for cc in pr[3]:
                q.append((cc, lv + 1, '0x' + r))
    ADM = {l.split('\t')[0][2:].lower() for l in open('re/match/ADMITTED.tsv', encoding='utf-8') if l.startswith('0x')}
    skip = {r for r, _, _ in P['bindings_roots']} if P['bindings_roots'] else set()
    tag = a.step or P['bindings_tag'] or P['own_tag']
    if not tag:
        sys.exit('profile %s: no bindings_tag, own_tag or --step for the BINDINGS.tsv notes' % a.profile)
    upd = add = 0
    for r, pr in sorted(plan.items()):
        if r in skip:
            continue
        if pr[0] == 'bind':
            # only 'match' is a byte match; an admitted body stays kind matched (L-126); replay / provisional is near-match
            sym = 'PortTop_Bound_' + r
            if pr[2] == 'match':
                kind, why = 'matched', 'c4=match'
            elif pr[2] == 'admitted' or r in ADM:
                kind, why = 'matched', 'L-126 admitted, re/match/ADMITTED.tsv, not a byte match'
            elif pr[2] == 'replay':
                kind, why = 'near-match', 'hooks.csv C4 by c4=frida-replay, NOT a byte match; L-126 provisional, re/match/PROVISIONAL.tsv'
            else:
                kind, why = 'near-match', 'L-126 provisional, re/match/PROVISIONAL.tsv, NOT matched, knob path only'
            note = '%s: %s (%s), Match/PortTop_%s.cpp' % (tag, pr[1], why, r)
        elif pr[0] == 'reuse' and pr[2].startswith('lib:'):
            # BIND-LIB-ROUTE: a vendored library body (L-97 / L-177: outside the port count, like the crt rows)
            kind, sym = 'library', pr[1]
            note = '%s: vendored library symbol %s (%s)' % (tag, pr[1], pr[2])
        elif pr[0] == 'reuse':
            kind, sym = 'matched', pr[1]
            note = '%s: existing exe body %s' % (tag, pr[2])
        else:
            kind, sym = 'missing', P['stop_prefix'] + r
            note = '%s: named stop (log + exit code = rva, %s). No matched or admitted body' % (tag, P['stop_file'])
        key = '0x' + r
        if key in idx:
            f = lines[idx[key]].split('\t')
            if f[2] == 'missing' and kind == 'missing' and P['bindings_keep_missing']:
                pass   # PORT-TOP-06: a stop of this closure does not overwrite another closure's missing row
            elif f[2] == 'missing':   # only a missing row is updated; a row another row wrote keeps its text
                if kind == 'missing':
                    f[3], f[4] = sym, note
                else:
                    f[2], f[3], f[4] = kind, sym, note
                new = '\t'.join(f)
                if new != lines[idx[key]]:
                    lines[idx[key]] = new; upd += 1
        else:
            lines.append('\t'.join([key, 'FUN_' + r, kind, sym, note, str(level.get(r, 3)), caller.get(r, '')])); add += 1
    if not a.dry:
        text = nl.join(lines) + nl
        if raw != text.encode():
            open(BINDINGS, 'wb').write(text.encode())
    print('updated', upd, 'added', add, '(dry run)' if a.dry else '')
    return 0


# --------------------------------------------------------------------------------------------------------------------
# unclean / samesrc / prune / index
# --------------------------------------------------------------------------------------------------------------------
def cmd_unclean(a):
    """append every generated PortTop_<rva>.cpp TU a VS2022 build log shows a compile error for (first error kept)."""
    log = open(a.log, encoding='mbcs', errors='replace').read().splitlines()
    pat = re.compile(r'error (C\d+): (.*?) \((?:compilando archivo de origen|compiling source file) Match\\PortTop_([0-9a-f]{8})\.cpp\)')
    first = {}
    for l in log:
        m = pat.search(l)
        if m and m.group(3) not in first:
            first[m.group(3)] = m.group(1) + ' ' + m.group(2)[:110]
    p = os.path.join(state_dir(a.profile), 'cpp_unclean.txt')
    have = set()
    if os.path.exists(p):
        have = {l.split('\t')[0].strip() for l in open(p, encoding='utf-8') if l.strip() and not l.startswith('#')}
    new = [k for k in sorted(first) if k not in have]
    with open(p, 'a', encoding='utf-8', newline='\n') as f:
        if not os.path.getsize(p):
            f.write('# matched sources that fail to compile as C++ (MATCH_PORT, VS2022) in the exe build; first error.\n')
        for k in new:
            f.write('%s\t%s\n' % (k, first[k]))
    print('unclean +', len(new), 'total', len(have) + len(new), '(re-run write: they become named stops)')
    return 0


def cmd_samesrc(a):
    """planned binds whose matched source a non-generated TU already #includes under FUN_<rva> (the index misses those
    when the TU keeps the name with no rename #define): binding it gives the rva a second exe copy."""
    P = PROFILES[a.profile]
    plan = json.load(open(os.path.join(state_dir(a.profile), 'plan.json')))
    inc = {}
    for tu in rsp_tus():
        p = os.path.join(SRC, tu)
        if not os.path.exists(p):
            continue
        t = rd(p)
        if P['own_mark'] and t.startswith(P['own_mark']):
            continue
        if not P['own_mark'] and re.search(r'Match/PortTop_[0-9a-f]{8}\.cpp$', tu):
            continue
        for m in re.finditer(r'#include\s+"(?:\.\./)+(re/match/[^"]+)"', t):
            inc.setdefault(m.group(1), []).append((tu, t))
    n = 0
    for rva, v in sorted(plan.items()):
        if v[0] != 'bind':
            continue
        for tu, t in inc.get(v[1], []):
            m = re.search(r'#define\s+FUN_%s\s+(\w+)' % rva, t)
            print(rva, v[1], tu, m.group(1) if m else 'FUN_%s' % rva); n += 1
    print('total', n)
    return 0


def cmd_prune(a):
    """allowlist lines this profile's sync wrote whose rva is neither a bound plan key nor a file-local function of a
    bound source (rltools/prune_allowlist.py dropped the file-local ones, PORT-OT-RACE checkpoint 3)."""
    P = PROFILES[a.profile]
    plan = json.load(open(os.path.join(state_dir(a.profile), 'plan.json')))
    keep = {k for k, v in plan.items() if v and v[0] == 'bind'} | {c for v in plan.values() if v and v[0] == 'bind' for c in v[4]}
    tag = '%s (L-111 (a), L-126)' % (a.step or P['allow_default']['step'])
    raw = open(ALLOW, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
    out, drop = [], []
    for l in raw.decode().split(nl):
        if tag in l:
            m = re.search(r':([0-9a-fA-F]{8})', l.split('#')[0])
            if m and m.group(1).lower() not in keep:
                drop.append(l); continue
        out.append(l)
    for l in drop:
        print('DROP', l[:160])
    if drop and a.apply:
        open(ALLOW, 'wb').write(nl.join(out).encode())
    print('allowlist -', len(drop), '' if a.apply else '(dry run; --apply writes)')
    return 0


def cmd_refs(a):
    """`refs` step: from state/<profile>/plan.json (no re-plan), recompute the bound rvas with no bound caller, rewrite
    their wrappers' PortTop_Keep_<rva> anchors (added or dropped) and the generated refs file. --dry lists only."""
    b = Binder(a.profile, [])
    if not b.P.get('refs_file'):
        print('profile', a.profile, 'has no refs_file'); return 1
    b.plan = {k: tuple(v) for k, v in json.load(open(b.plan_path)).items()}
    refs = b.refs()
    print('refs', len(refs), 'bound bodies with no bound caller:', ' '.join('0x' + r for r in refs))
    if a.dry:
        return 0
    n = 0
    for rva, p in sorted(b.plan.items()):
        if p[0] == 'bind':
            fn, text = b.wrapper_text(rva)
            if write_if_changed(fn, text):
                n += 1; print('CHANGED', fn)
    rf = '%s/%s' % (SRC, b.P['refs_file'])
    if write_if_changed(rf, b.refs_text()):
        n += 1; print('CHANGED', rf)
    print(n, 'files changed')
    return 0


def cmd_index(a):
    idx = build_index(PROFILES[a.profile]['own_mark'] if a.profile else None)
    if a.out:
        json.dump(idx, open(a.out, 'w'), indent=0)
    print(len(idx), 'rvas indexed from', len(rsp_tus()), 'files')
    return 0


def cmd_plan(a, write):
    b = Binder(a.profile, (a.roots or profile_roots(a.profile)).split(','), roots_only=a.roots_only)
    if a.index:   # debug / history check: plan against a frozen exe index (`index --out` json) instead of the live one
        if write:
            sys.exit('--index is for plan only (a write needs the live exe index)')
        b.idx = {k: [tuple(x) for x in v] for k, v in json.load(open(a.index)).items()}
    b.build_plan()
    lst = b.listing()
    if a.listing:
        open(a.listing, 'w', encoding='utf-8').write(NL.join(lst) + NL)
    print(NL.join(lst if a.verbose or not write else lst[:1]))
    if write:
        b.write()
    return 0


# --------------------------------------------------------------------------------------------------------------------
# selftest: rebind every profile (state/<profile>/profile.json) on the current tree and require no change in the generated files, the plan and
# BINDINGS.tsv. Wrappers / stop file / BINDINGS.tsv are compared modulo CRLF (core.autocrlf working copies).
# --------------------------------------------------------------------------------------------------------------------
# Roots come from state/<profile>/roots.txt (raceload: PORT-OT-RACE checkpoint 3 plus the 34 BIND-CUNCLEAN-AS-C C units,
# rebound with --roots-only, profile.json selftest_roots_only), else the plan keys no bound plan entry calls (audio).

VCVARS = r'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat'


def cl_compile(files, outdir):
    """compile files with the exe's VS2022 flags (build.bat line 250: /EHa /W3 /O2 /DMASHED_STANDALONE), one cl call,
    each file in the language of its extension (cl: .c = C, .cpp = C++). Returns {file: error lines}."""
    import tempfile
    rsp = os.path.join(outdir, 'cemit.rsp')
    open(rsp, 'w').write(NL.join('"%s"' % os.path.abspath(f) for f in files) + NL)
    bat = os.path.join(outdir, 'cemit.bat')
    open(bat, 'w').write('@echo off\r\n%scl /nologo /EHa /W3 /O2 /DMASHED_STANDALONE /c "/Fo%s\\\\" "@%s"\r\n' % (
        '' if os.environ.get('VCINSTALLDIR') else 'call "%s" >nul\r\n' % VCVARS, outdir, rsp))
    r = subprocess.run(['cmd', '/c', bat], capture_output=True)
    out = r.stdout.decode('mbcs', errors='replace') + r.stderr.decode('mbcs', errors='replace')
    errs = {f: [] for f in files}
    for l in out.splitlines():
        if ': error ' in l or ': fatal error ' in l:
            for f in files:
                if (os.path.basename(f).lower() + '(') in l.lower():
                    errs[f].append(l.strip())
    return errs, out


def selftest_cemit(b):
    """C emit: (1) every bound cpp-unclean rva has a .c wrapper and no .cpp twin, the C text carries no extern "C";
    (2) fixture: one g2sh-preamble shape (typedef unsigned char bool / unsigned short wchar_t, unprototyped code() call,
    undefined4 to pointer) fails as C++ and compiles as C under the same flags; (3) every held rva (c_hold.txt) emitted as
    a C unit into a scratch dir compiles (the C emit works for them; they are not bound). Returns the failure count."""
    import tempfile, shutil
    bad = 0
    nb = 0
    for rva in sorted(b.cemit):
        p = b.plan.get(rva)
        if not p or p[0] != 'bind':
            continue
        nb += 1
        fn, text = b.wrapper_text(rva)
        twin = fn[:-2] + '.cpp'
        if not fn.endswith('.c') or 'extern "C"' in text or os.path.exists(twin):
            print('   cemit wrapper', fn, 'twin' if os.path.exists(twin) else '', 'extern-C' if 'extern "C"' in text else '')
            bad += 1
    print('[selftest] C emit: %d cpp-unclean rvas bound as .c units, %d wrapper-shape failures' % (nb, bad))
    if not os.path.exists(VCVARS):
        print('[selftest] C emit compile: SKIPPED (no %s)' % VCVARS)
        return bad
    d = tempfile.mkdtemp(prefix='cemit_')
    try:
        fx = ('typedef unsigned int undefined4; typedef int code();\ntypedef unsigned char bool;\n'
              'typedef unsigned short wchar_t;\nint FUN_00400100(int *p);\n'
              'int FUN_00400110(undefined4 a, code *f) { return FUN_00400100(a) + f(1, 2); }\n')
        for ext in ('c', 'cpp'):
            open(os.path.join(d, 'cemit_fixture.' + ext), 'w').write(fx)
        errs, _ = cl_compile([os.path.join(d, 'cemit_fixture.c'), os.path.join(d, 'cemit_fixture.cpp')], d)
        okc = not errs[os.path.join(d, 'cemit_fixture.c')]
        okx = bool(errs[os.path.join(d, 'cemit_fixture.cpp')])
        print('[selftest] C emit fixture: as C %s, as C++ %s' % ('compiles' if okc else 'FAILS', 'fails (expected)' if okx else 'COMPILES (unexpected)'))
        bad += (not okc) + (not okx)
        # held rvas: emit as if bound (callees as the plan has them), absolute include path, compile only
        held = [r for r in sorted(b.hold) if r in b.unclean]
        files = []
        for rva in held:
            src, kind = b.msrc(rva)
            if not src:
                print('   held', rva, 'has no matched source'); bad += 1; continue
            cs, local, _ = b.callees(src, rva)
            saved = b.plan.get(rva)
            b.plan[rva] = ('bind', src, kind, cs, local)
            missing = [c for c in cs if c not in b.plan]
            for c in missing:
                b.plan[c] = ('stop',)
            b.cemit.add(rva)
            try:
                _, text = b.wrapper_text(rva)
            finally:
                b.cemit.discard(rva)
                b.plan[rva] = saved
                for c in missing:
                    del b.plan[c]
            text = text.replace('#include "../../../../%s"' % src, '#include "%s"' % os.path.abspath(src).replace('\\', '/'))
            f = os.path.join(d, 'PortTop_%s.c' % rva)
            open(f, 'w', encoding='utf-8', newline='\n').write(text)
            files.append(f)
        if files:
            errs, out = cl_compile(files, d)
            nfail = sum(1 for f in files if errs[f])
            for f in files:
                for e in errs[f][:2]:
                    print('   held', os.path.basename(f), e[:200])
            print('[selftest] C emit of the %d held rvas (c_hold.txt, not bound): %d compile, %d fail' % (
                len(files), len(files) - nfail, nfail))
            bad += nfail
    finally:
        shutil.rmtree(d, ignore_errors=True)
    return bad


def history_moves(b):
    """listing lines of the current plan against the frozen listing named by profile.json selftest_history.listing:
    [(rva or 'totals', old line, new line)] for every line that differs (keyed by the rva, the first word)."""
    h = b.P['selftest_history']
    old = [l.rstrip('\r\n') for l in open(h['listing'], encoding='utf-8')
           if l.strip() and not l.startswith('wrote ')]   # the old writer's last print line is not a plan line
    new = b.listing()
    ko = {'totals': old[0]}; ko.update({l.split(' ', 1)[0]: l for l in old[1:]})
    kn = {'totals': new[0]}; kn.update({l.split(' ', 1)[0]: l for l in new[1:]})
    return [(k, ko.get(k, '-'), kn.get(k, '-')) for k in sorted(set(ko) | set(kn)) if ko.get(k) != kn.get(k)]


def selftest_history(b):
    """BIND-BULK-PROFILE: every line by which the plan's listing moved from the frozen listing (bulk: RACE-BIND-C4-BULK
    checkpoint 1's last_plan.txt) is named, with its cause, in state/<profile>/<moved>; no unnamed or stale row."""
    h = b.P['selftest_history']
    mv = history_moves(b)
    named = {}
    mp = os.path.join(b.sd, h['moved'])
    for l in (open(mp, encoding='utf-8') if os.path.exists(mp) else []):
        f = l.rstrip('\r\n').split('\t')
        if len(f) >= 3 and not l.startswith('#') and f[0] != 'rva':
            named[f[0]] = (f[1], f[2])
    cur = {k: (o, n) for k, o, n in mv}
    unnamed = sorted(k for k in cur if named.get(k) != cur[k])
    stale = sorted(k for k in named if k not in cur)
    for k in unnamed:
        print('   moved, not named:', k, '|', cur[k][0], '->', cur[k][1])
    for k in stale:
        print('   named, not moved:', k)
    print('[selftest] %s listing vs %s: %d lines moved, %s' % (
        b.name, h['listing'], len(mv), 'all named in state/%s/%s' % (b.name, h['moved']) if not unnamed and not stale
        else 'FAIL %d unnamed, %d stale' % (len(unnamed), len(stale))))
    return len(unnamed) + len(stale)


def moved_cause(old, new):
    """BIND-MOVED-TSV-AUTO: the cause text of one moved line, from the two listing lines: the kind change and, when the
    plan now binds or reuses a PortTop body, the RVA it is bound as ("RVA now bound")."""
    ok = old.split(' ')[1] if old != '-' and len(old.split(' ')) > 1 else '-'
    nk = new.split(' ')[1] if new != '-' and len(new.split(' ')) > 1 else '-'
    m = re.search(r'PortTop_Bound_([0-9a-f]{8})', new)
    t = '%s -> %s' % (ok, nk)
    if m and nk in ('bind', 'reuse'):
        t += ', now bound as PortTop_Bound_%s (RVA 0x%s)' % (m.group(1), m.group(1))
    return t


def cmd_moved(a):
    """BIND-MOVED-TSV-AUTO (RULE 3): append to state/<profile>/<moved> a line for every listing line that moved from the
    frozen listing and is not named yet (only --rva ones when given), with its cause (--cause text first, then the
    automatic kind change and RVA now bound), and a totals line carrying the current totals. Existing lines stay as
    written (the reader keeps the last row of a key)."""
    P = PROFILES[a.profile]
    h = P['selftest_history']
    if not h:
        sys.exit('profile %s has no selftest_history' % a.profile)
    b = Binder(a.profile, profile_roots(a.profile).split(','), roots_only=P['selftest_roots_only'])
    b.build_plan()
    mp = os.path.join(b.sd, h['moved'])
    text = open(mp, encoding='utf-8', newline='').read() if os.path.exists(mp) else ''
    nl = '\r\n' if '\r\n' in text else '\n'
    named = {}
    for l in text.splitlines():
        f = l.split('\t')
        if len(f) >= 3 and not l.startswith('#') and f[0] != 'rva':
            named[f[0]] = (f[1], f[2])
    want = {r.lower() for r in a.rva} if a.rva else None
    out, tot = [], None
    for k, o, n in history_moves(b):
        if k == 'totals':
            tot = (o, n)
        elif named.get(k) != (o, n) and (want is None or k in want):
            c = moved_cause(o, n)
            out.append('\t'.join([k, o, n, (a.cause + ': ' if a.cause else '') + c]))
    if want:
        miss = want - {l.split('\t')[0] for l in out} - set(named)
        for r in sorted(miss):
            print('   --rva %s: not moved from the frozen listing, nothing written' % r)
    if tot and out:
        out.append('\t'.join(['totals', tot[0], tot[1], 'plan totals after the moves (%s; supersedes the totals rows above)'
                              % (a.cause or 'porttop_bind.py moved')]))
    for l in out:
        print(l)
    print('moved: %d line(s) %s' % (len(out), 'would be added (dry run)' if a.dry else 'added to ' + mp))
    if out and not a.dry:
        if text and not text.endswith('\n'):
            text += nl
        open(mp, 'w', encoding='utf-8', newline='').write(text + nl.join(out) + nl)
    return 0


def selftest_libs():
    """BIND-LIB-ROUTE: every lib rule (shared and per profile) resolves to one non-static definition in its library."""
    bad, n = 0, 0
    for prof in sorted(PROFILES):
        roots_ = dict(SHARED['lib_root']); roots_.update(PROFILES[prof]['lib_root'])
        lib = dict(SHARED['lib']); lib.update(PROFILES[prof]['lib'])
        ok, errs = resolve_libs(roots_, lib)
        n += len(ok)
        for e in errs:
            print('   %s: %s' % (prof, e))
        bad += len(errs)
    print('[selftest] lib kind: %d lib roots, %d profile x rva lib bindings resolve, %d fail (unit test: '
          're/tools/bind/test_porttop_bind_lib.py)' % (len(SHARED['lib_root']) + sum(len(P['lib_root']) for P in PROFILES.values()), n, bad))
    return bad


def cmd_selftest(a):
    # HB-LOCK-ENFORCE: wait line only, no change to what the selftest checks
    sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'orchestrator'))
    import mashed_lock
    mashed_lock.wait_clear(label='porttop_bind selftest')
    bad = selftest_libs()
    for prof in sorted(PROFILES, key=lambda p: (PROFILES[p]['selftest_order'], p)):
        if not os.path.exists(os.path.join(state_dir(prof), 'plan.json')):
            print('[selftest] %s: no plan.json, skipped' % prof)
            continue
        roots = profile_roots(prof)
        pp = os.path.join(state_dir(prof), 'plan.json')
        before = json.load(open(pp))
        b = Binder(prof, roots.split(','), roots_only=PROFILES[prof]['selftest_roots_only'])
        plan = {k: list(v) for k, v in b.build_plan().items()}
        same_plan = plan == before
        diffs = []
        for rva, p in sorted(b.plan.items()):
            if p[0] != 'bind':
                continue
            if rva in b.P['root_external']:
                # root kept in its existing TU: regenerate the selectors and the TU's define lines, compare, restore
                cfg = b.P['root_external'][rva]
                keep = {f: open(f, 'rb').read() for f in ('%s/%s' % (SRC, cfg['tu']), '%s/%s' % (SRC, cfg['sel_file']))
                        if os.path.exists(f)}
                ch = []
                import io, contextlib
                with contextlib.redirect_stdout(io.StringIO()):
                    b.write_selectors(rva, cfg, ch)
                for f, raw_ in keep.items():
                    open(f, 'wb').write(raw_)
                diffs.extend(ch)
                continue
            fn, text = b.wrapper_text(rva)
            if not os.path.exists(fn) or open(fn, 'rb').read().replace(b'\r\n', b'\n') != text.encode():
                diffs.append(fn)
        _, st = b.stops_text()
        sf = '%s/%s' % (SRC, b.P['stop_file'])
        if open(sf, 'rb').read().replace(b'\r\n', b'\n') != st.encode():
            diffs.append(sf)
        c = Counter(v[0] for v in plan.values())
        print('[selftest] %s roots %d: plan %s (bind %d reuse %d stop %d), wrappers+stops %s (%d checked)' % (
            prof, len(roots.split(',')), 'IDENTICAL' if same_plan else 'DIFFERENT', c['bind'], c['reuse'], c['stop'],
            'IDENTICAL' if not diffs else 'DIFFERENT %d' % len(diffs), c['bind'] + 1))
        if not same_plan:
            for k in sorted(set(plan) | set(before)):
                if plan.get(k) != before.get(k):
                    print('   plan', k, before.get(k, '-')[:3] if before.get(k) else '-', '->', plan.get(k, '-')[:3] if plan.get(k) else '-')
        for d in diffs:
            print('   file', d)
        bad += (not same_plan) + len(diffs)
        if b.P['selftest_history']:
            bad += selftest_history(b)
        if b.P.get('refs_file'):
            # refs: the committed file equals the generated text; and the drop-out rule: give one listed rva a bound
            # caller (in memory) and it must leave the set, its wrapper losing the anchor
            rf = '%s/%s' % (SRC, b.P['refs_file'])
            same = os.path.exists(rf) and open(rf, 'rb').read().replace(b'\r\n', b'\n') == b.refs_text().encode()
            refs = list(b.refs())
            drop = 'n/a (empty set)'
            if refs:
                # one own rva (its wrapper loses the anchor) and one foreign-group rva (it leaves the list), when present
                own = sorted(b._refs_own)
                fgn = sorted(set(refs) - set(own))
                parts = []
                for r0, is_own in ([(own[0], True)] if own else []) + ([(fgn[0], False)] if fgn else []):
                    k = next(k for k, v in sorted(b.plan.items()) if v[0] == 'bind' and k != r0)
                    saved = b.plan[k]
                    b.plan[k] = (saved[0], saved[1], saved[2], list(saved[3]) + [r0], saved[4]); b._refs = None
                    gone = r0 not in b.refs() and (not is_own or 'PortTop_Keep_%s' % r0 not in b.wrapper_text(r0)[1])
                    b.plan[k] = saved; b._refs = None
                    parts.append(('0x%s (%s) drops out with a bound caller 0x%s' % (r0, 'own' if is_own else 'foreign', k))
                                 if gone else 'FAIL: 0x%s stays listed' % r0)
                    bad += not gone
                b.refs()
                drop = '; '.join(parts) + ' (own %d, foreign %d)' % (len(own), len(fgn))
            print('[selftest] %s refs: %d caller-less bound bodies, %s %s, %s' % (
                prof, len(refs), b.P['refs_file'], 'IDENTICAL' if same else 'DIFFERENT', drop))
            bad += not same
        if b.unclean:
            bad += selftest_cemit(b)
    # BINDINGS.tsv: the rows of each profile with profile.json selftest_bindings (audio), rebuilt in memory, must equal
    # the committed file
    for prof in sorted(PROFILES, key=lambda p: (PROFILES[p]['selftest_order'], p)):
        if not PROFILES[prof]['selftest_bindings']:
            continue
        raw = open(BINDINGS, 'rb').read()
        bak = raw
        ns = argparse.Namespace(profile=prof, step=None, dry=False)
        try:
            import io, contextlib
            with contextlib.redirect_stdout(io.StringIO()) as o:
                cmd_bindings(ns)
            same = open(BINDINGS, 'rb').read().replace(b'\r\n', b'\n') == bak.replace(b'\r\n', b'\n')
        finally:
            open(BINDINGS, 'wb').write(bak)
        print('[selftest] BINDINGS.tsv from the %s plan: %s (%s)' % (prof, 'IDENTICAL' if same else 'DIFFERENT', o.getvalue().strip()))
        bad += not same
    print('[selftest] %s' % ('PASS' if not bad else 'FAIL %d' % bad))
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sp = ap.add_subparsers(dest='cmd', required=True)
    for n in ('plan', 'write'):
        s = sp.add_parser(n)
        s.add_argument('--profile', required=True, choices=sorted(PROFILES))
        s.add_argument('--roots', help='comma-separated rvas (cumulative: pass every root of the closure); '
                                       'default: state/<profile>/roots.txt')
        s.add_argument('--listing', help='also write the listing to this file')
        s.add_argument('-v', '--verbose', action='store_true')
        s.add_argument('--roots-only', action='store_true',
                       help='newly bind only the roots: other matched callees not bound by the previous plan are named stops')
        s.add_argument('--index', help='plan only: read the exe body index from this json (`index --out`) instead of the tree')
    s = sp.add_parser('externs'); s.add_argument('--profile', required=True, choices=sorted(PROFILES))
    s.add_argument('--apply', action='store_true'); s.add_argument('src', nargs='*')
    s = sp.add_parser('sync'); s.add_argument('--profile', required=True, choices=sorted(PROFILES))
    s.add_argument('--step'); s.add_argument('--date'); s.add_argument('--evidence'); s.add_argument('--dry', action='store_true')
    s = sp.add_parser('bindings'); s.add_argument('--profile', required=True, choices=sorted(PROFILES))
    s.add_argument('--step'); s.add_argument('--dry', action='store_true')
    s = sp.add_parser('unclean'); s.add_argument('--profile', required=True, choices=sorted(PROFILES)); s.add_argument('log')
    s = sp.add_parser('samesrc'); s.add_argument('--profile', required=True, choices=sorted(PROFILES))
    s = sp.add_parser('prune'); s.add_argument('--profile', required=True, choices=sorted(PROFILES))
    s.add_argument('--step'); s.add_argument('--apply', action='store_true')
    s = sp.add_parser('index'); s.add_argument('--profile', choices=sorted(PROFILES)); s.add_argument('--out')
    s = sp.add_parser('refs'); s.add_argument('--profile', required=True, choices=sorted(PROFILES))
    s.add_argument('--dry', action='store_true')
    s = sp.add_parser('moved'); s.add_argument('--profile', required=True, choices=sorted(PROFILES))
    s.add_argument('--rva', nargs='*'); s.add_argument('--cause'); s.add_argument('--dry', action='store_true')
    sp.add_parser('selftest')
    a = ap.parse_args()
    if not os.path.exists('hooks.csv') or not os.path.exists(RSP):
        sys.exit('run from the worktree root')
    return {'plan': lambda: cmd_plan(a, False), 'write': lambda: cmd_plan(a, True), 'externs': lambda: cmd_externs(a),
            'sync': lambda: cmd_sync(a), 'bindings': lambda: cmd_bindings(a), 'unclean': lambda: cmd_unclean(a),
            'samesrc': lambda: cmd_samesrc(a), 'prune': lambda: cmd_prune(a), 'index': lambda: cmd_index(a),
            'selftest': lambda: cmd_selftest(a), 'moved': lambda: cmd_moved(a), 'moved': lambda: cmd_moved(a), 'refs': lambda: cmd_refs(a)}[a.cmd]()


if __name__ == '__main__':
    sys.exit(main())
