"""PortTop binder: plan and generate exe-only port bodies (Match/PortTop_<rva>.cpp) for a call closure on the top-down
route (MASHED_ORIG_TOP, L-96 / L-111), plus the follow-up steps every binding checkpoint runs. BIND-TOOL, 2026-11-21.

One tool for what three per-row generators did before:
  verify/portorigtop_20261113/tools/audiotick_bind_gen.py   -> profile `audio`    (audio-tick / audio-init closure)
  verify/ro_20261116/rltools/rl_bind_gen.py                 -> profile `raceload` (race-load closure of 0x0040d270)
  verify/portorigtop_20261113/tools/bindings_update.py      -> subcommand `bindings`
and their helpers (exe_body_index.py, sync_rsp_allowlist.py / rl_sync.py, guard_externs.py, unclean_from_log.py,
same_source_exe.py, prune_allowlist.py). The rule tables of both generators are carried over unchanged (PROFILES below),
so a re-run on the same tree writes the same files. State (plan.json, cpp_unclean.txt, newfiles.txt) lives in
re/tools/bind/state/<profile>/ (copied from the old verify/ folders at BIND-TOOL).

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
  py -3.12 re/tools/bind/porttop_bind.py selftest                               # rebind both profiles, expect no change
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
# Shared rule tables (from audiotick_bind_gen.py; the raceload profile inherits every one of them, as rl_bind_gen.py
# did by exec-ing that generator's prefix).
# --------------------------------------------------------------------------------------------------------------------
# L-126 provisional bodies bind on the knob path only for the rvas named here (kept narrow so the closure grows one
# reached stop at a time); c4=frida-replay rows (L-128) bind from their one re/match source.
PROV_BIND = {'005a6ea0', '005a6f30', '004627f0', '005aa0c0'}   # 004627f0 + its reached callee 005aa0c0: PORT-TOP-AUDIO-HWINIT-4627F0
PROV_BIND |= {'005a5e70'}   # PORT-TOP-RWAUDIO-TABLE-632BA8: table slot 0x00632ba8 (tail-merge layout, PROVISIONAL.tsv)
PROV_BIND |= {'005ae650'}   # PORT-TOP-AUDIOPOOL-5AE650: AudioPoolConstruct (prologue placement, PROVISIONAL.tsv)
PROV_BIND |= {'005adc10'}   # PORT-TOP-AUDIOSTOP-5ADC10: 0x005add60's callee, register-only residual 92.2% (PROVISIONAL.tsv)
PROV_BIND |= {'0045d460'}   # PORT-TOP-AUDIO-BODY-45D460: 0x004669b0's permdict loader, register-only residual 94.2% (PROVISIONAL.tsv)
PROV_BIND |= {'005a61b0', '005b35a0', '005ae800'}   # PORT-TOP-AUDIOSTOP-5A61B0: 0x005ad770's stops (PROVISIONAL.tsv)
PROV_BIND |= {'005a9ab0'}   # PORT-TOP-AUDIOINIT-SPLIT-5A9970: 0x005a9970's stop (67.9%, EQUIV-IF-CALLERS-IGNORE(ECX), callers read)
PROV_BIND |= {'005aa560'}   # PORT-TOP-AUDIOINIT-SPLIT-5A9970: table entry 7 0x005aa480's stop (79.8%, NOT-PROVEN call-args at 0x005aa593)
PROV_BIND |= {'005aa330'}   # PORT-TOP-AUDIO-5AA3D0-STOPS: 0x005a9c50's other stop (54.7%, NOT-PROVEN call-args at 0x005aa350)
PROV_BIND |= {'005ad420'}   # PORT-TOP-AUDIO-5AA3D0-STOPS: shared callee of the 0x00632c2c entries (74.5%, NOT-PROVEN at 0x005ad4e5)
PROV_BIND |= {'005a8ea0'}   # PORT-TOP-AUDIO-5AA3D0-STOPS: table 0x00632c28 entry (77.6%, EQUIV-IF-CALLERS-IGNORE(EAX), caller 0x005ad080 reads)
PROV_BIND |= {'005aa200'}   # PORT-TOP-AUDIO-5AA3D0-STOPS: census order 5, 0x005a62b0's callee (66.1%, NOT-PROVEN at 0x005aa271)
PROV_BIND |= {'005a9f10'}   # PORT-TOP-AUDIO-5AEF30-KNOB: census 69, existing PROVISIONAL.tsv row (81.0%, NOT-PROVEN, PROV-FIX-9)
PROV_BIND |= {'005bad30'}   # PORT-TOP-AUDIO-5AEF30-KNOB: census 58, source from the listing 94.4%, NOT-PROVEN
PROV_BIND |= {'005b9ea0', '005b85e0', '005c7a00', '005c6c20', '005a7fe0', '005c80a0', '005b7440', '005b6b80', '005bce20'}   # PORT-TOP-AUDIO-5A8F30-STOPS: table 0x00635194 entries walked by 0x005bbb20 (L-171 class)

EXTRA_PRE = {
    '004645c0': ['// The source declares its globals extern under #ifndef MATCH_PORT; the port reads them at their original addresses.',
                 '#define DAT_00604a10 (*(int *)0x00604a10u)', '#define DAT_0068f640 ((Slot *)0x0068f640u)'],
    '0045d250': ['// The source declares its globals extern under #ifndef MATCH_PORT; the port reads them at their original addresses.',
                 '#define DAT_007f0fd0 (*(int *)0x007f0fd0u)', '#define DAT_0088fbc0 (*(int *)0x0088fbc0u)'],
    '00407600': ['// MASHED_RE_PORT drops the source extern block (same as Match/MatchS01.cpp); the table is read at its original address.',
                 '#define MASHED_RE_PORT 1', '#define DAT_00639d80 ((Rec639d80 *)0x00639d80u)'],
}
EXTRA_PRE['005ab040'] = ['#pragma comment(lib, "winmm.lib")   // timeGetTime (the original imports it from WINMM.dll)']
EXTRA_PRE['005aafe0'] = ['#define thunk_FUN_005ab010 PortTop_Bound_005aafe0   // re/match/s30/005aafe0.c names its body after the jump target (Ghidra thunk name)']
EXTRA_PRE['005ac590'] = ['// LAB_005ac5c0 is a code address the body stores, not called here: the port stores the original address (no exe body;',
                         '// a later call through it is reported by MASHED_CODE_TRAP, image design (b)).',
                         '#define LAB_005ac5c0 (*(undefined1 *)0x005ac5c0u)']
EXTRA_PRE['005ae330'] = ['#define thunk_FUN_004522d0 RwCore_004522d0   // Ghidra thunk_ name of the jump to 0x004522d0 (re/match/s26/005ae330.c)']
EXTRA_PRE['005ab180'] = ['#define thunk_FUN_005ab620 PortTop_Bound_005ab180   // PORT-TOP-AUDIO-5A8F30-STOPS: re/match/s25/005ab180.c names its body after the jump target (Ghidra thunk name)']
EXTRA_PRE['005b7880'] = ['#define thunk_FUN_005b1500 PortTop_Bound_005b1500   // PORT-TOP-AUDIO-5A8F30-STOPS: Ghidra thunk_ name of 0x005b15d0 (jmp 0x005b1500, called at 0x005b789a); the port calls the target body']
# BIND-CUNCLEAN-AS-C: C units whose sources call a callee by an RW API name or a site alias (relocheck leaves these
# names unchecked); the target is read from the original listing (capstone on original/MASHED.exe.unpatched).
EXTRA_PRE['004e3e70'] = ['#define RwFrameAddChild RwCore_004e4450   // the one call, 0x004e3e88: call 0x004e4450']
EXTRA_PRE['005386b0'] = ['#define RpWorldForAllWorldSectors RwCore_004e5c70   // 0x00538762: call 0x004e5c70 (as RwCore/RwCore_00538bd0.cpp:12)']
EXTRA_PRE['005444a0'] = ['#define FUN_004cc790_site2 RwCore_004cc790   // the case-2 read: the 9 direct calls of 0x005444a0 target 0x004cc790 (7) and 0x004cbd30 (2)']
# re/match/s10/0045cbe0.c carries a second copy of 0x0045c4e0 under #ifndef MATCH_PORT_SHARED_0045c4e0: the port binds
# the call to the one exe copy in the 0045cb20 TU (one exe body per rva).
EXTRA_PRE['0045cbe0'] = ['#define MATCH_PORT_SHARED_0045c4e0 1   // the exe copy of 0x0045c4e0 is the one in Match/PortTop_0045cb20.cpp']
EXTRA_IN = {'00407600': ['char *FUN_0047d150(int h);   // declared here: the source declares it only outside MASHED_RE_PORT']}
LOCAL_SHARED = {('0045cbe0', '0045c4e0'): 'PortTop_Local_0045cb20_0045c4e0'}
LOCAL_AS_BOUND = {('0045c6b0', '0045c640')}   # re/match/s10/0045c6b0.c carries the only exe copy of 0x0045c640
ALIAS = {'0045c640': ('PortTop_Bound_0045c640', 'Match/PortTop_0045c6b0.cpp'),
         '004671a0': ('RwCore_004671a0', 'RwCore/RwCore_004671a0.cpp'),   # one ORIGTOP-only exe copy per rva (RWC-FRAME-492E90)
         '004a2cbd': ('PortTop_Crt_004a2cbd', 'Match/PortTop_00493710.cpp'),   # printf (CRT, L-67), PORT-TOP-AUDIO-BODY-45D460
         '005c1920': ('PortTop_Imp_005c1920', 'Match/PortTop_Imp_005c1920.cpp')}   # DSOUND ordinal 11 import thunk

# --------------------------------------------------------------------------------------------------------------------
# Profiles. A profile is one closure's rule set: its extra aliases, forced stops, header text and stop file.
# --------------------------------------------------------------------------------------------------------------------
RL_MARK = '// PORT-OT-RACE (2026-11-16): exe-only port body'
PROFILES = {
    'audio': dict(
        force={'00466b50', '0045d3f0', '0045d430'},
        alias={},
        stop_force={'005bf610',   # its callee 0x005bf670 is __stdcall with no body: the cdecl stop thunk does not link (LNK2019)
                    '005aef30'},  # PORT-TOP-AUDIO-5A8F30-STOPS: calls __beginthread (0x005c2e79), census order 42
        knob={'005aef30': ('PortTop_Knob_005aef30', 'Match/PortTop_Knob_005aef30.cpp')},   # L-174 (c)
        callee_override={}, not_local=set(), own_mark=None, prune_stale=False, rwcore_fallback=False,
        reuse_any=False, force_old_binds=True, stop_prefix='PortTop_AStop_',
        stop_file='Match/PortTop_AudioStops.cpp',
        local_shared_note='   // the one exe copy (Match/PortTop_0045cb20.cpp); this source compiles its copy out',
        local_bound_note='   // the exe body of 0x%s: this source carries its only exe copy',
        bindings_roots=[('00466b50', 2, '0x004926c0'), ('0045d3f0', 2, '0x004926c0'), ('0045d430', 2, '0x004926c0')],
        bindings_tag='PORT-TOP-AUDIOTICK-STOPS',
        allow_text='%(step)s (L-111 (a), L-126), %(date)s: the matched / replay-C4 / provisional body runs only on MASHED_ORIG_TOP + '
                   'MASHED_OT_AUDIOTICK / MASHED_OT_AUDIOINIT (audio closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430 / 0x004627f0); '
                   'deleted at the ORIG_TOP flip. Evidence: %(evidence)s.',
        allow_default=dict(step='PORT-TOP-AUDIO-RWA', date='2026-11-13', evidence=None)),
    'raceload': dict(
        force=set(),   # the roots are forced (always planned, never reused)
        alias={
            # The exe's file layer on the top-down route (no RtFSManager): the piz mount stack and the RW stream open /
            # close bindings over it (PORT-TOP-04F).
            '00495280': ('PortTop_PizFs_Mount_00495280', 'Match/PortTop_00402750_Pieces.cpp'),
            '004952f0': ('PortTop_PizFs_Unmount_004952f0', 'Match/PortTop_00402750_Pieces.cpp'),
            '004cc230': ('PortTop_RwStreamOpen_004cc230', 'Match/PortTop_RwStream04F.cpp'),
            '004cc160': ('PortTop_RwStreamClose_004cc160', 'Match/PortTop_RwStream04F.cpp'),
            '00550b00': ('RwCoreBind_FsFileExists_00550b00', 'RwCore/RwCore_Bind_00550b00.cpp'),
            # 0x0042fe30: one exe body already compiled from re/match/s07/0042fe30.c (MATCH-S07 TU); a second copy would be
            # a dual copy of the rva in the exe.
            '0042fe30': ('RaceEndFlagIfEndMode', 'Match/MatchS07_0042fe30.cpp'),
            # 0x00458c70: checkpoint 3 (lead decision (b), 2026-11-16) kept the named counting stand-in, its bound body
            # faulted in RwCore_004c15c0+0x85 (original 0x004c1645), [RwGlobals+0xbc] = 0 on the librw RwGlobals record.
            # RACE-RWGLOBALS-FRAMELIST (run 53): the RW core frame list now exists on the record under MASHED_OT_RACELOAD
            # (RwApiBind.cpp RaceLoadFrameOpen, 0x004c07b0). Callers bind PortTop_RL_00458c70, which runs the hand-written
            # bound TU Match/PortTop_00458c70.cpp when [RwGlobals+0xbc] != 0 (rl_bind_gen.py at integration 34c823375).
            '00458c70': ('PortTop_RL_00458c70', 'Match/PortTop_StandInsRL.cpp')},
        stop_force=set(), knob={},
        # PORT-OT-RACE-BIND-PROV4 (run 53, carried from rl_bind_gen.py at integration 34c823375): the four COURSE.LUA-path
        # stops of checkpoint 2 bind their L-126 provisional sources (PROVISIONAL.tsv rows of RACE-COURSELUA-BODIES, all
        # NOT-PROVEN), knob path only, never C4. Profile-local: the audio plan is not changed.
        prov_bind={'0047a020', '00479330', '00478660', '00462950'},
        # PORT-OT-RACE-BIND-PROV4: rvas kept as named stops although a matched source exists, with the reason.
        stop_reason={'004790e0': 'register-arg: world pointer in EAX (re/match/s12/004790e0.c header; caller 0x00479330 loads '
                                 '[ebx+0x105d4] into EAX at 0x00479e2e), declared void(void) by re/match/s12/00479330.c under '
                                 'MATCH_PORT and static in its own source; no exe body takes EAX'},
        # PORT-OT-RACE-BIND-PROV4: a source that defines its function under a name other than FUN_<rva>
        name_def={'004e5c30': 'RpWorldForAllAtomics'},   # re/match/rwl02/004e5c30.c line 34
        # PORT-OT-RACE-BIND-PROV4: non-FUN_ callee names in a source, bound to the exe body of that rva
        extra_def={'0042a740': ['#define _strchr MashedCrt_strchr_004a2fd0   // the original CRT strchr 0x004a2fd0 '
                                '(Match/PortTop_CrtString.cpp, BIND-CRT-STRING)']},
        # a callee whose exe body takes another argument list than the caller's source declares binds to the forwarder
        callee_override={('0042a5d0', '004b3bf0'): ('MatchS06_0042a5d0_Call004b3bf0',
                         'Match/MatchS06_0042a5d0.cpp: the source passes 1 argument, Orig_004b3bf0 takes 3; '
                         'declared deviation there, 2nd / 3rd passed as 0')},
        not_local={('0047bf70', '0047be80'),   # re/match/s13/0047bf70.c: 0x0047be80 HELD (86.4%), its copy is #ifndef MATCH_PORT
                   # PORT-OT-RACE-BIND-PROV4: re/match/s12/00478660.c holds a HELD static copy of 0x004785e0 (C2) under
                   # #ifndef MATCH_PORT (source lines 55-77); on the port it is an external callee
                   ('00478660', '004785e0')},
        own_mark=RL_MARK, prune_stale=True, rwcore_fallback=True, reuse_any=True, force_old_binds=False,
        stop_prefix='PortTop_RLStop_', stop_file='Match/PortTop_RaceLoadStops.cpp',
        refs_file='Match/PortTop_BulkRefs.cpp',   # `refs` step (BIND-CUNCLEAN-AS-C): keeps caller-less bound bodies linked
        # wrappers another generator wrote whose PortTop_Keep_<rva> anchors this refs file also carries:
        # RACE-BIND-C4-BULK checkpoint 1 (verify/bindbulk_20261121, 100 anchors; its refs_gen.py is history only)
        foreign_keep_marks=('// RACE-BIND-C4-BULK (2026-11-21): exe-only port body',),
        local_shared_note='   // the one exe copy', local_bound_note='   // this source carries its only exe copy',
        bindings_roots=None,   # roots of the plan, caller 0x0040d270 (see cmd_bindings)
        bindings_root_caller='0x0040d270',
        bindings_tag='PORT-OT-RACE',
        allow_text='%(step)s (L-111 (a), L-126), %(date)s: the matched / replay-C4 body runs only on MASHED_ORIG_TOP + '
                   'MASHED_OT_RACELOAD (race-load closure of 0x0040d270, %(tool)s); deleted at the ORIG_TOP flip. '
                   'Evidence: %(evidence)s.',
        allow_default=dict(step='PORT-OT-RACE', date='2026-11-16', evidence='verify/ro_20261116/RESULT_PORT-OT-RACE.md',
                           tool='verify/ro_20261116/rltools/rl_bind_gen.py')),
}


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
        incs = set(m.lower() for m in re.findall(r'#include\s+"[^"]*re/match/[^/"]+/([0-9a-fA-F]{8})\.c"', t))
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
            self.force = set(self.roots) | set(self.P['force'])
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
        self.cemit = set(self.unclean) - set(self.hold)

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
        if (rva in PROV_BIND or rva in self.P.get('prov_bind', ())) and rva in self.prov and os.path.exists(self.prov[rva][2]):
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
            if rva in P.get('stop_reason', {}):
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
            if s and self.roots_only and rva not in self.roots and (self.old_plan.get(rva) or ['x'])[0] != 'bind':
                plan[rva] = ('stop', 'not bound here (--roots-only): matched source %s' % s); continue
            if s:
                cs, local, _ = self.callees(s, rva)
                # a callee defined only under #ifndef MATCH_PORT (a HELD copy the DDK needs) is external on the port
                for c in [c for c in local if (rva, c) in P['not_local']]:
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
        self.plan = plan
        return plan

    def sym(self, rva):
        p = self.plan[rva]
        if p[0] == 'reuse':
            return p[1]
        if p[0] == 'bind':
            return 'PortTop_Bound_%s' % rva
        return self.P['stop_prefix'] + rva

    def listing(self):
        plan = self.plan
        out = ['%s %s' % (Counter(p[0] for p in plan.values()), Counter(p[2] for p in plan.values() if p[0] == 'bind'))]
        for rva in sorted(plan):
            p = plan[rva]
            r = self.rows.get(rva) or ['', '', '', '-']
            if self.name == 'raceload':
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
        if self.name == 'raceload':
            return ['%s of rva 0x%s = %s' % (RL_MARK, rva, src),
                    '// (%s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD' % ev,
                    '// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies',
                    '// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).']
        # audio: an existing wrapper keeps the header its row wrote; new ones say which root closure reached them
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
        if keep_hdr and rva in ('00466b50', '0045d3f0', '0045d430'):
            L += [l for l in keep_hdr if not l.startswith('// RVA-LINT')]
            for x in ('// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): callees rebound to matched / admitted exe bodies; the rest stay named stops',
                      '// (PortTop_AStop_<rva>, Match/PortTop_AudioStops.cpp).'):
                if x not in L:
                    L.append(x)
        elif keep_hdr and not table_new:
            L += [l for l in keep_hdr if not l.startswith('// RVA-LINT')]
        elif table_new:
            L.append('// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x%s = %s' % (rva, src))
            L.append('// (%s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT' % ev)
            L.append('// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the')
            L.append('// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;')
            L.append('// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).')
        elif rva not in self.old_plan:   # unreachable while table_new covers every new bind; kept from the old generator
            L.append('// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x%s = %s' % (rva, src))
            L.append('// (%s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT' % ev)
            L.append('// through the closure of 0x004627f0 (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;')
            L.append('// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).')
        else:   # an earlier bind whose wrapper file is missing
            L.append('// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x%s = %s' % (rva, src))
            L.append('// (%s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK' % ev)
            L.append('// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;')
            L.append('// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).')
        return L

    def wrapper_text(self, rva):
        P = self.P
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
        if rva in P.get('name_def', {}):
            L.append('#define %s PortTop_Bound_%s   // the source names 0x%s %s' % (P['name_def'][rva], rva, rva, P['name_def'][rva]))
        L += P.get('extra_def', {}).get(rva, [])
        for c in cs:
            if (rva, c) in P['callee_override']:
                L.append('#define FUN_%s %s   // %s' % ((c,) + P['callee_override'][(rva, c)]))
                continue
            pc = self.plan[c]
            note = {'reuse': '   // exe body (%s)' % (pc[2] if pc[0] == 'reuse' else ''), 'bind': '', 'stop': '   // named stop'}[pc[0]]
            if c in P['stop_force']:
                note = '   // named stop: re/match/s10/0045cbe0.c carries a second copy of 0x0045c4e0 (one exe body per rva)'
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
            if (rva, c) in LOCAL_SHARED:
                L.append('#define FUN_%s %s%s' % (c, LOCAL_SHARED[(rva, c)], P['local_shared_note']))
            elif (rva, c) in LOCAL_AS_BOUND:
                n = P['local_bound_note'] % c if '%s' in P['local_bound_note'] else P['local_bound_note']
                L.append('#define FUN_%s PortTop_Bound_%s%s' % (c, c, n))
            else:
                L.append('#define FUN_%s PortTop_Local_%s_%s   // file-local in this source' % (c, rva, c))
        if 'FID_conflict__wprintf' in rd(src):
            L.append('#define FID_conflict__wprintf PortTop_Printf04G   // printf -> log (Match/PortTop_0040bbb0.cpp)')
        L += EXTRA_PRE.get(rva, [])
        keep = []
        if rva in self.refs() and rva in self._refs_own:
            # `refs` step: no bound caller references this body yet, so /OPT:REF would drop it (ddkobj.py check: not-in-map).
            # The data anchor is named by Match/PortTop_BulkRefs.cpp's /include (decoration-free, works for a static body).
            a = 'void *PortTop_Keep_%s = (void *)PortTop_Bound_%s;' % (rva, rva)
            keep = [(a if is_c else 'extern "C" { %s }' % a) + '   // anchor: %s /include (no bound caller yet)'
                    % self.P['refs_file']]
        if is_c:
            L += EXTRA_IN.get(rva, [])
            L += ['#include "../../../../%s"' % src] + keep + ['#endif // MASHED_STANDALONE']
            return fn, NL.join(L) + NL
        L.append('extern "C" {')
        L += EXTRA_IN.get(rva, [])
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
        if self.name == 'audio':
            S = ['// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): named stops of the audio-tick closure (0x00466b50 / 0x0045d3f0 / 0x0045d430).',
                 '// Each is a callee with no matched (c4=match) or L-126 admitted body yet. A stop logs and exits with code = rva',
                 '// (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so a reached stop names the next body to bind.',
                 '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#include <intrin.h>', 'extern "C" {',
                 'void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp']
            for s in stops:
                S.append('void PortTop_AStop_%s(void) { PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); }' % (s, s))
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
                S.append('int PortTop_RLStop_%s(void) { RlStopLog(0x%su, "%s", 1u, _ReturnAddress(), 1); PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); return 0; }' % (s_, s_, cl, s_))
            else:
                S.append('int PortTop_RLStop_%s(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();' % s_)
                S.append('    if (n == 1 || ex) RlStopLog(0x%su, "%s", n, _ReturnAddress(), ex);' % (s_, cl))
                S.append('    if (ex) PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); return 0; }' % s_)
        S += ['}', '#endif // MASHED_STANDALONE']
        return stops, NL.join(S) + NL

    def write(self):
        changed, out = [], []
        if self.P['prune_stale']:
            # remove this profile's wrappers whose rva is no longer bound (and their exe_sources.rsp lines)
            rr = open(RSP, 'rb').read(); nl = '\r\n' if b'\r\n' in rr else '\n'
            rl = rr.decode().split(nl); n0 = len(rl)
            for f in glob.glob(SRC + '/Match/PortTop_*.cpp') + glob.glob(SRC + '/Match/PortTop_*.c'):
                m = re.search(r'PortTop_([0-9a-f]{8})\.(cpp|c)$', f)
                if not m or not rd(f).startswith(self.P['own_mark']):
                    continue
                # BIND-CUNCLEAN-AS-C: a wrapper of the other language (.cpp <-> .c) is stale too
                if (self.plan.get(m.group(1), ('x',))[0] != 'bind'
                        or os.path.normcase(f) != os.path.normcase(self.wrapper_path(m.group(1)))):
                    os.remove(f)
                    rl = [l for l in rl if l.strip() != '"Match\\PortTop_%s.%s"' % (m.group(1), m.group(2))]
                    print('REMOVED', f)
            if len(rl) != n0:
                open(RSP, 'wb').write(nl.join(rl).encode())
        for rva, p in sorted(self.plan.items()):
            if p[0] != 'bind':
                continue
            fn, text = self.wrapper_text(rva)
            if self.P['own_mark'] and os.path.exists(fn) and 'PORT-OT-RACE' not in rd(fn):
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
        roots = [(r, lv, rc) for r in sorted(plan) if r not in called]
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
    tag = a.step or P['bindings_tag']
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
        elif pr[0] == 'reuse':
            kind, sym = 'matched', pr[1]
            note = '%s: existing exe body %s' % (tag, pr[2])
        else:
            kind, sym = 'missing', P['stop_prefix'] + r
            note = '%s: named stop (log + exit code = rva, %s). No matched or admitted body' % (tag, P['stop_file'])
        key = '0x' + r
        if key in idx:
            f = lines[idx[key]].split('\t')
            if f[2] == 'missing':   # only a missing row is updated; a row another row wrote keeps its text
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
    b = Binder(a.profile, a.roots.split(','), roots_only=a.roots_only)
    b.build_plan()
    lst = b.listing()
    if a.listing:
        open(a.listing, 'w', encoding='utf-8').write(NL.join(lst) + NL)
    print(NL.join(lst if a.verbose or not write else lst[:1]))
    if write:
        b.write()
    return 0


# --------------------------------------------------------------------------------------------------------------------
# selftest: rebind both profiles on the current tree and require no change in the generated files, the plan and
# BINDINGS.tsv. Wrappers / stop file / BINDINGS.tsv are compared modulo CRLF (core.autocrlf working copies).
# --------------------------------------------------------------------------------------------------------------------
SELFTEST = {'raceload': '00426e10,004264d0'}   # PORT-OT-RACE checkpoint 3 (verify/ro_20261116/RESULT_PORT-OT-RACE.md 7.1)
# BIND-CUNCLEAN-AS-C (2026-11-21): the 34 cpp-unclean rvas of RACE-BIND-C4-BULK checkpoint 1, bound as C units with
# --roots-only (verify/bindcunclean_20261121/RESULT_BIND-CUNCLEAN-AS-C.md). A later raceload `write` must pass them as roots
# too (roots are cumulative), or prune removes their wrappers.
CUNCLEAN_ROOTS = ('0041a8d0,00472330,004cac10,004d3450,004d34d0,004e3bd0,004e3e70,004e3ea0,004eb320,004f4540,005142d0,'
                  '005142f0,0052e4e0,0052e500,00533840,00534ad0,00534af0,005386b0,00538c30,0053d3e0,0053ec40,0053ec60,'
                  '0053f9b0,0053fa90,005444a0,00544e00,00544f10,005492d0,00549310,0054a000,0054a0f0,005518f0,00557d90,'
                  '0057c2e0')
SELFTEST['raceload'] += ',' + CUNCLEAN_ROOTS
SELFTEST_ROOTS_ONLY = {'raceload'}

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


def audio_roots():
    p = json.load(open(os.path.join(state_dir('audio'), 'plan.json')))
    called = {c for v in p.values() if v[0] == 'bind' for c in v[3] + v[4]}
    return ','.join(sorted(k for k in p if k not in called))


def cmd_selftest(a):
    bad = 0
    for prof in ('raceload', 'audio'):
        roots = SELFTEST.get(prof) or audio_roots()
        pp = os.path.join(state_dir(prof), 'plan.json')
        before = json.load(open(pp))
        b = Binder(prof, roots.split(','), roots_only=prof in SELFTEST_ROOTS_ONLY)
        plan = {k: list(v) for k, v in b.build_plan().items()}
        same_plan = plan == before
        diffs = []
        for rva, p in sorted(b.plan.items()):
            if p[0] != 'bind':
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
    # BINDINGS.tsv: the audio plan's rows rebuilt in memory must equal the committed file
    raw = open(BINDINGS, 'rb').read()
    bak = raw
    ns = argparse.Namespace(profile='audio', step=None, dry=False)
    try:
        import io, contextlib
        with contextlib.redirect_stdout(io.StringIO()) as o:
            cmd_bindings(ns)
        same = open(BINDINGS, 'rb').read().replace(b'\r\n', b'\n') == bak.replace(b'\r\n', b'\n')
    finally:
        open(BINDINGS, 'wb').write(bak)
    print('[selftest] BINDINGS.tsv from the audio plan: %s (%s)' % ('IDENTICAL' if same else 'DIFFERENT', o.getvalue().strip()))
    bad += not same
    print('[selftest] %s' % ('PASS' if not bad else 'FAIL %d' % bad))
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sp = ap.add_subparsers(dest='cmd', required=True)
    for n in ('plan', 'write'):
        s = sp.add_parser(n)
        s.add_argument('--profile', required=True, choices=sorted(PROFILES))
        s.add_argument('--roots', required=True, help='comma-separated rvas (cumulative: pass every root of the closure)')
        s.add_argument('--listing', help='also write the listing to this file')
        s.add_argument('-v', '--verbose', action='store_true')
        s.add_argument('--roots-only', action='store_true',
                       help='newly bind only the roots: other matched callees not bound by the previous plan are named stops')
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
    sp.add_parser('selftest')
    a = ap.parse_args()
    if not os.path.exists('hooks.csv') or not os.path.exists(RSP):
        sys.exit('run from the worktree root')
    return {'plan': lambda: cmd_plan(a, False), 'write': lambda: cmd_plan(a, True), 'externs': lambda: cmd_externs(a),
            'sync': lambda: cmd_sync(a), 'bindings': lambda: cmd_bindings(a), 'unclean': lambda: cmd_unclean(a),
            'samesrc': lambda: cmd_samesrc(a), 'prune': lambda: cmd_prune(a), 'index': lambda: cmd_index(a),
            'selftest': lambda: cmd_selftest(a), 'refs': lambda: cmd_refs(a)}[a.cmd]()


if __name__ == '__main__':
    sys.exit(main())
