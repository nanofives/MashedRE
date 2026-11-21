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
# re/match/s10/0045cbe0.c carries a second copy of 0x0045c4e0 under #ifndef MATCH_PORT_SHARED_0045c4e0: the port binds
# the call to the one exe copy in the 0045cb20 TU (one exe body per rva).
EXTRA_PRE['0045cbe0'] = ['#define MATCH_PORT_SHARED_0045c4e0 1   // the exe copy of 0x0045c4e0 is the one in Match/PortTop_0045cb20.cpp']
EXTRA_PRE['00401570'] = ['// PORT-TOP-06: MASHED_RE_PORT drops the source extern block (same as Match/MatchS01.cpp:42 / :77-78); the table and',
                          '// the current-entry pointer are read at their original addresses.',
                          '#define MASHED_RE_PORT 1', '#define DAT_00636578 ((Entry636578*)0x00636578u)',
                          '#define DAT_00636ac0 (*(Entry636578**)0x00636ac0u)']
EXTRA_IN = {'00407600': ['char *FUN_0047d150(int h);   // declared here: the source declares it only outside MASHED_RE_PORT']}
EXTRA_IN['00401570'] = ['void FUN_00401da0(void);   // PORT-TOP-06: re/match/s01/00401570.c declares these two only outside MASHED_RE_PORT',
                        'void FUN_004e6680(void *e);   // the source passes Entry636578 * (typedef declared later in the source)']
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
            # 0x00458c70 stays the named counting stand-in (lead decision (b), 2026-11-16): its bound body faulted in
            # RwCore_004c15c0+0x85 (original 0x004c1645), [RwGlobals+0xbc] = 0 on the librw RwGlobals record.
            '00458c70': ('PortTop_StandInRL_00458c70', 'Match/PortTop_StandInsRL.cpp')},
        stop_force=set(), knob={},
        # a callee whose exe body takes another argument list than the caller's source declares binds to the forwarder
        callee_override={('0042a5d0', '004b3bf0'): ('MatchS06_0042a5d0_Call004b3bf0',
                         'Match/MatchS06_0042a5d0.cpp: the source passes 1 argument, Orig_004b3bf0 takes 3; '
                         'declared deviation there, 2nd / 3rd passed as 0')},
        not_local={('0047bf70', '0047be80')},   # re/match/s13/0047bf70.c: 0x0047be80 HELD (86.4%), its copy is #ifndef MATCH_PORT
        own_mark=RL_MARK, prune_stale=True, rwcore_fallback=True, reuse_any=True, force_old_binds=False,
        stop_prefix='PortTop_RLStop_', stop_file='Match/PortTop_RaceLoadStops.cpp',
        local_shared_note='   // the one exe copy', local_bound_note='   // this source carries its only exe copy',
        bindings_roots=None,   # roots of the plan, caller 0x0040d270 (see cmd_bindings)
        bindings_root_caller='0x0040d270',
        bindings_tag='PORT-OT-RACE',
        allow_text='%(step)s (L-111 (a), L-126), %(date)s: the matched / replay-C4 body runs only on MASHED_ORIG_TOP + '
                   'MASHED_OT_RACELOAD (race-load closure of 0x0040d270, %(tool)s); deleted at the ORIG_TOP flip. '
                   'Evidence: %(evidence)s.',
        allow_default=dict(step='PORT-OT-RACE', date='2026-11-16', evidence='verify/ro_20261116/RESULT_PORT-OT-RACE.md',
                           tool='verify/ro_20261116/rltools/rl_bind_gen.py')),
    # PORT-TOP-06 (2026-11-21): the race-frame closure of 0x004111c0 (MatchT01G source re/match/t01g/004111c0.c, whole
    # symbol BYTE-IDENTICAL), called by the frame machine 0x00492d30 (Match/PortTop_00492d30.cpp) only under the sub-knob
    # MASHED_OT_RACEFRAME (Match/PortTop_RaceFrameKnob.cpp; default off = the PORT-TOP-05 counting stand-in).
    'raceframe': dict(
        force=set(), stop_force=set(), knob={}, callee_override={},
        # re/match/t01g/004111c0.c: 0x004103a0 (HELD, :301) and the five extent members 0x0040dd80 / 0x0040ddf0 /
        # 0x0040feb0 / 0x00410130 / 0x00410ab0 (HIDDEN, :347) are #ifndef MATCH_PORT: external on the port.
        not_local={('004111c0', c) for c in ('004103a0', '0040dd80', '0040ddf0', '0040feb0', '00410130', '00410ab0')},
        # rvas another row owns or another unmerged branch binds (state/raceframe/stop_reasons.tsv): planned binds of
        # them become named stops with the reason; an existing exe body of them is still reused.
        stop_reasons='stop_reasons.tsv',
        # a bind whose own function is `static` in its source has no linkable exe body: named stop with the line
        static_stop=True,
        # several binds from one source: one wrapper (the rva the file is named after, else the lowest non-static one)
        # carries them all, the others reuse it (one exe body per rva, rva-lint DUP-IN-TARGET)
        group_same_source=True,
        held_external=True, thunk_names=True, bindings_keep_missing=True,
        stop_why={'0041b540': 'link: re/match/s04/0041b540.c calls FUN_0041af50_body (:48), which has no exe body (LNK2019)'},
        # members of sources another top-down TU already compiles under an exe name (samesrc): one exe body per rva
        alias={'0045bba0': ('Orig_0045bba0', 'Match/PortTop_0045bae0.cpp'),
               '0045bf30': ('Orig_0045bf30', 'Match/PortTop_0045bae0.cpp')},
        own_mark='// PORT-TOP-06 (2026-11-21): exe-only port body', own_tag='PORT-TOP-06',
        prune_stale=True, rwcore_fallback=True, reuse_any=True, force_old_binds=False,
        stop_prefix='PortTop_RFStop_', stop_file='Match/PortTop_RaceFrameStops.cpp',
        local_shared_note='   // the one exe copy', local_bound_note='   // this source carries its only exe copy',
        hdr=['%(mark)s of rva 0x%(rva)s = %(src)s',
             '// (%(ev)s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME',
             '// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to',
             '// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).'],
        stops_subst=[
            ('// PORT-OT-RACE (2026-11-16): named stops of the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py).',
             '// PORT-TOP-06 (2026-11-21): named stops of the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe).'),
            ('MASHED_OT_RL_STOPS', 'MASHED_OT_RF_STOPS'), ('PortTop_RL_StopHits', 'PortTop_RF_StopHits'),
            ('PortTop_RLStop_', 'PortTop_RFStop_'), ('race-load stop (PORT-OT-RACE)', 'race-frame stop (PORT-TOP-06)'),
            ('so the load', 'so the frame')],
        bindings_roots=None, bindings_root_caller='0x00492d30', bindings_tag='PORT-TOP-06',
        allow_text='%(step)s (L-111 (a), L-126), %(date)s: the matched body runs only on MASHED_ORIG_TOP + '
                   'MASHED_OT_RACEFRAME (race-frame closure of 0x004111c0, %(tool)s); deleted at the ORIG_TOP flip. '
                   'Evidence: %(evidence)s.',
        allow_default=dict(step='PORT-TOP-06', date='2026-11-21', evidence='verify/porttop06_20261121/RESULT_PORT-TOP-06.md',
                           tool='re/tools/bind/porttop_bind.py --profile raceframe')),
    # PORT-TOP-RENDERFRAME-492E90 (2026-11-21): the frame-render closure of 0x00492e90 (MATCH-S15 source
    # re/match/s15/00492e90.c, 100%), called by the run loop 0x00492290 through PortTop_StandIn_00492e90
    # (Match/PortTop_00492370.cpp) only under the sub-knob MASHED_OT_RENDERFRAME (Match/PortTop_RenderFrameKnob.cpp;
    # default off = the hand RenderFrame / the RWC-FRAME-492E90 route). Rvas other run-53 rows own or unmerged
    # branches bind (RACE-BIND-C4-BULK cp1, PORT-TOP-06's 0x004111c0 closure) are named stops with the reason
    # (state/renderframe/stop_reasons.tsv, verify/porttoprenderframe_20261121/tools/make_stop_reasons.py).
    'renderframe': dict(
        force=set(), stop_force=set(), knob={}, callee_override={}, not_local=set(),
        stop_reasons='stop_reasons.tsv', static_stop=True, group_same_source=True,
        held_external=True, thunk_names=True, bindings_keep_missing=True,
        stop_why={'0041b540': 'link: re/match/s04/0041b540.c calls FUN_0041af50_body (:48), which has no exe body (LNK2019)'},
        alias={'0045bba0': ('Orig_0045bba0', 'Match/PortTop_0045bae0.cpp'),
               '0045bf30': ('Orig_0045bf30', 'Match/PortTop_0045bae0.cpp'),
               # re/match/s01/00401340.c needs MASHED_RE_PORT and its prototypes (Match/MatchS01.cpp:42-99): the exe's one
               # body of it there is reused, not a second copy (PortTop_00401340.cpp failed LNK2019 on its 4 externs)
               '00401340': ('CupSpinSpeedAndColor', 'Match/MatchS01.cpp')},
        own_mark='// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body', own_tag='PORT-TOP-RENDERFRAME-492E90',
        # the root keeps its one exe body, the RW core TU (RWC-FRAME-492E90); its callees go through knob selectors
        root_external={'00492e90': dict(tu='RwCore/RwCore_00492e90.cpp', sym='RwCore_00492e90', sel_file='Match/PortTop_RenderFrameSel.cpp',
                                        sel_prefix='PortTop_RRSel_', knob='MASHED_OT_RENDERFRAME', gate='RwCore_FrameRoute, MASHED_RWC_FRAME')},
        prune_stale=True, rwcore_fallback=True, reuse_any=True, force_old_binds=False,
        stop_prefix='PortTop_RRStop_', stop_file='Match/PortTop_RenderFrameStops.cpp',
        local_shared_note='   // the one exe copy', local_bound_note='   // this source carries its only exe copy',
        hdr=['%(mark)s of rva 0x%(rva)s = %(src)s',
             '// (%(ev)s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME',
             '// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to',
             '// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).'],
        stops_subst=[
            ('// PORT-OT-RACE (2026-11-16): named stops of the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py).',
             '// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): named stops of the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe).'),
            ('MASHED_OT_RL_STOPS', 'MASHED_OT_RR_STOPS'), ('PortTop_RL_StopHits', 'PortTop_RR_StopHits'),
            ('PortTop_RLStop_', 'PortTop_RRStop_'), ('race-load stop (PORT-OT-RACE)', 'frame-render stop (PORT-TOP-RENDERFRAME-492E90)'),
            ('so the load', 'so the frame render')],
        bindings_roots=None, bindings_root_caller='0x00492290', bindings_tag='PORT-TOP-RENDERFRAME-492E90',
        allow_text='%(step)s (L-111 (a), L-126), %(date)s: the matched body runs only on MASHED_ORIG_TOP + '
                   'MASHED_OT_RENDERFRAME (frame-render closure of 0x00492e90, %(tool)s); deleted at the ORIG_TOP flip. '
                   'Evidence: %(evidence)s.',
        allow_default=dict(step='PORT-TOP-RENDERFRAME-492E90', date='2026-11-21',
                           evidence='verify/porttoprenderframe_20261121/RESULT_PORT-TOP-RENDERFRAME-492E90.md',
                           tool='re/tools/bind/porttop_bind.py --profile renderframe')),
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
# Planner
# --------------------------------------------------------------------------------------------------------------------
class Binder:
    def __init__(self, profile, roots):
        self.name = profile
        self.P = PROFILES[profile]
        self.roots = [r.lower().replace('0x', '') for r in roots if r]
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
        self.reasons = {}
        if self.P.get('stop_reasons'):
            rp = os.path.join(self.sd, self.P['stop_reasons'])
            for l in (open(rp, encoding='utf-8') if os.path.exists(rp) else []):
                if l.strip() and not l.startswith('#'):
                    k, _, why = l.rstrip('\r\n').partition('\t')
                    self.reasons.setdefault(k.strip().lower().replace('0x', ''), why.strip())
        self.group_bound = set()
        for k, why in self.P.get('stop_why', {}).items():
            self.reasons[k] = why

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
        if rva in PROV_BIND and rva in self.prov and os.path.exists(self.prov[rva][2]):
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
            if rva in self.unclean:
                plan[rva] = ('stop', 'cpp-unclean: ' + self.unclean[rva]); continue
            if rva in self.alias:
                plan[rva] = ('reuse',) + self.alias[rva]; continue
            ru = None if rva in self.force else self.reuse(rva)
            if not ru and P['rwcore_fallback'] and rva not in self.force and os.path.exists(SRC + '/RwCore/RwCore_%s.cpp' % rva):
                ru = ('RwCore_%s' % rva, 'RwCore/RwCore_%s.cpp' % rva)   # the RW core's one exe body per rva (L-124)
            if ru:
                plan[rva] = ('reuse', ru[0], ru[1]); continue
            s, kind = self.msrc(rva)
            if s and rva in self.reasons and rva not in self.force:
                plan[rva] = ('stop', self.reasons[rva]); continue
            if s and P.get('static_stop'):
                m_ = self.static_line(s, rva)
                if m_:
                    plan[rva] = ('stop', 'static in %s:%d (file-local there): no linkable exe body' % (s, m_)); continue
            if s:
                cs, local, _ = self.callees(s, rva)
                # a callee defined only under #ifndef MATCH_PORT (a HELD copy the DDK needs) is external on the port
                for c in [c for c in local if (rva, c) in P['not_local']]:
                    local = [x for x in local if x != c]; cs = sorted(set(cs) | {c})
                if P.get('held_external'):   # the same, found from the source's #ifndef MATCH_PORT branches
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
        if P.get('group_same_source'):
            groups = {}
            for k, v in plan.items():
                if v[0] == 'bind':
                    groups.setdefault(v[1], []).append(k)
            for s, ks in sorted(groups.items()):
                if len(ks) < 2:
                    continue
                own = [k for k in ks if re.search(r'[/_]%s\.c$' % k, s)] or sorted(ks)
                o = own[0]
                for m_ in sorted(ks):
                    if m_ != o and m_ not in self.force:
                        plan[m_] = ('reuse', 'PortTop_Bound_%s' % m_, 'Match/PortTop_%s.cpp' % o)
                        self.group_bound.add((o, m_))
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
            if rva in self.P.get('root_external', {}):
                return self.P['root_external'][rva]['sym']
            return 'PortTop_Bound_%s' % rva
        return self.P['stop_prefix'] + rva

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
            tl[i] = '#define FUN_%s %s%s   // was %s; %s: MASHED_OT_RENDERFRAME=1 calls %s' % (c, pre, c, old, tag, new)
            sels.append((c, ret, params or 'void', args, old, new))
        tt = '\n'.join(tl)
        if write_if_changed(tu, tt):
            changed.append(tu)
        S = ['%s: selectors of the direct callees of 0x%s (re/tools/bind/porttop_bind.py --profile %s).' % (self.P['own_mark'], rva, self.name),
             '// %s is the one exe body of 0x%s (%s). Each callee below whose walked binding' % (cfg['tu'], rva, src),
             '// differs from that TU\'s own symbol goes through a selector: %s=1 calls the walked body (the frame-render' % cfg['knob'],
             '// closure of 0x%s, Match/PortTop_<rva>.cpp, or its named stop in %s); unset or 0 calls the TU\'s' % (rva, self.P['stop_file']),
             '// own symbol, so the knob-off path is unchanged. The TU runs only on the RW core frame route (%s).' % cfg['gate'],
             '// RVA-LINT: ORIGTOP-ONLY',
             '#ifdef MASHED_STANDALONE',
             '#include <windows.h>',
             'extern "C" void RwCore_Log(const char* fmt, ...);   // C linkage (RwCore/RwCore_Knob.cpp:28-53)',
             'static int s_knob = -1;',
             'static bool PortTop_RRSelOn(void) {',
             '    if (s_knob < 0) {',
             '        char v[8] = {};',
             '        s_knob = (GetEnvironmentVariableA("%s", v, sizeof(v)) > 0 && v[0] && v[0] != \'0\') ? 1 : 0;' % cfg['knob'],
             '        RwCore_Log("[%s] %s %%s: the direct callees of 0x%s %%s", s_knob ? "on" : "off",' % (tag, cfg['knob'], rva),
             '                   s_knob ? "call the walked frame-render closure" : "keep the RW core TU\'s own symbols");',
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
                S.append('void %s%s(%s) { if (PortTop_RRSelOn()) { %s(%s); return; } %s(%s); }   // 0x%s' % (
                    pre, c, params, new, args, old, args, c))
            else:
                S.append('%s %s%s(%s) { if (PortTop_RRSelOn()) return %s(%s); return %s(%s); }   // 0x%s' % (
                    ret, pre, c, params, new, args, old, args, c))
        S += ['}', '#endif // MASHED_STANDALONE']
        sf = '%s/%s' % (SRC, cfg['sel_file'])
        if write_if_changed(sf, NL.join(S) + NL):
            changed.append(sf)
        print('selectors', len(sels), 'of', len(cs), 'callees of 0x%s' % rva)
        return [sf]

    def listing(self):
        plan = self.plan
        out = ['%s %s' % (Counter(p[0] for p in plan.values()), Counter(p[2] for p in plan.values() if p[0] == 'bind'))]
        for rva in sorted(plan):
            p = plan[rva]
            r = self.rows.get(rva) or ['', '', '', '-']
            if self.name != 'audio':
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
        if self.P.get('hdr'):
            return [l % dict(mark=self.P['own_mark'], rva=rva, src=src, ev=ev) for l in self.P['hdr']]
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
        fn = '%s/Match/PortTop_%s.cpp' % (SRC, rva)
        L = self.header(rva, src, kind, fn)
        L += ['// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#define MATCH_PORT 1', '#define FUN_%s PortTop_Bound_%s' % (rva, rva)]
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
            L.append('#define FUN_%s %s%s' % (c, self.sym(c), note))
        for c in local:
            if (rva, c) in LOCAL_SHARED:
                L.append('#define FUN_%s %s%s' % (c, LOCAL_SHARED[(rva, c)], P['local_shared_note']))
            elif (rva, c) in self.group_bound:
                L.append('#define FUN_%s PortTop_Bound_%s   // bound rva of the same source: this TU carries its only exe copy' % (c, c))
            elif (rva, c) in LOCAL_AS_BOUND:
                n = P['local_bound_note'] % c if '%s' in P['local_bound_note'] else P['local_bound_note']
                L.append('#define FUN_%s PortTop_Bound_%s%s' % (c, c, n))
            else:
                L.append('#define FUN_%s PortTop_Local_%s_%s   // file-local in this source' % (c, rva, c))
        if P.get('thunk_names'):
            # Ghidra `thunk_FUN_<x>` names a thunk that jumps to x; the port calls x's exe body (as EXTRA_PRE 005aafe0 does)
            st = rd(src)
            for c in cs:
                if re.search(r'\bthunk_FUN_%s\b' % c, st) and not re.search(r'^[A-Za-z_][^;=(\n]*\bthunk_FUN_%s\s*\([^;{]*\)\s*\{' % c, st, re.M):
                    L.append('#define thunk_FUN_%s %s   // Ghidra thunk name of the jump to 0x%s: the port calls its exe body' % (c, self.sym(c), c))
        if 'FID_conflict__wprintf' in rd(src):
            L.append('#define FID_conflict__wprintf PortTop_Printf04G   // printf -> log (Match/PortTop_0040bbb0.cpp)')
        L += EXTRA_PRE.get(rva, [])
        L.append('extern "C" {')
        L += EXTRA_IN.get(rva, [])
        L += ['#include "../../../../%s"' % src, '}', '#endif // MASHED_STANDALONE']
        return fn, NL.join(L) + NL

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
        text = NL.join(S) + NL
        for a_, b_ in self.P.get('stops_subst', []):
            text = text.replace(a_, b_)
        return stops, text

    def write(self):
        changed, out = [], []
        if self.P['prune_stale']:
            # remove this profile's wrappers whose rva is no longer bound (and their exe_sources.rsp lines)
            rr = open(RSP, 'rb').read(); nl = '\r\n' if b'\r\n' in rr else '\n'
            rl = rr.decode().split(nl); n0 = len(rl)
            for f in glob.glob(SRC + '/Match/PortTop_*.cpp'):
                m = re.search(r'PortTop_([0-9a-f]{8})\.cpp$', f)
                if not m or not rd(f).startswith(self.P['own_mark']):
                    continue
                if self.plan.get(m.group(1), ('x',))[0] != 'bind' or m.group(1) in self.P.get('root_external', {}):
                    os.remove(f)
                    rl = [l for l in rl if l.strip() != '"Match\\PortTop_%s.cpp"' % m.group(1)]
                    print('REMOVED', f)
            if len(rl) != n0:
                open(RSP, 'wb').write(nl.join(rl).encode())
        for rva, p in sorted(self.plan.items()):
            if p[0] != 'bind':
                continue
            if rva in self.P.get('root_external', {}):   # the root's one exe body is an existing TU: selectors instead
                for fn in self.write_selectors(rva, self.P['root_external'][rva], changed):
                    out.append(fn)
                continue
            fn, text = self.wrapper_text(rva)
            if self.P['own_mark'] and os.path.exists(fn) and self.P.get('own_tag', 'PORT-OT-RACE') not in rd(fn):
                print('REFUSE existing wrapper of another closure', fn); sys.exit(2)
            if write_if_changed(fn, text):
                changed.append(fn)
            out.append(fn)
        stops, st = self.stops_text()
        sf = '%s/%s' % (SRC, self.P['stop_file'])
        if write_if_changed(sf, st):
            changed.append(sf)
        json.dump({k: list(v) for k, v in self.plan.items()}, open(self.plan_path, 'w'), indent=0)
        open(os.path.join(self.sd, 'newfiles.txt'), 'w').write(NL.join(out + [sf]) + NL)
        print('wrote', len(out), 'bind files +', len(stops), 'stops;', len(changed), 'files changed')
        for f in changed:
            print('CHANGED', f)
        return changed


# --------------------------------------------------------------------------------------------------------------------
# Extern-guard step (was rltools/guard_externs.py)
# --------------------------------------------------------------------------------------------------------------------
EXTERN_PAT = re.compile(r'^(\s*)extern\s+((?:const\s+)?(?:struct\s+)?[A-Za-z_]\w*(?:\s*\*+)?)\s*(_?(?:DAT|LAB)_([0-9a-fA-F]{8}))'
                        r'\s*(\[\s*([0-9xXa-fA-F]*)\s*\])?\s*;\s*$')


def guard_externs(path, apply):
    """A matched source's plain `extern <T> DAT_/LAB_<addr>;` data declaration does not link in the exe. Put it under
    #ifndef MATCH_PORT and bind the port (MATCH_PORT, C++) to the original address. The DDK never defines MATCH_PORT,
    so its object is unchanged: run re/tools/match/match.py <src> --bytes after the edit (done by `externs --apply`)."""
    raw = open(path, 'rb').read()
    nl = '\r\n' if b'\r\n' in raw else '\n'
    lines = raw.decode('latin-1').split(nl)   # byte-preserving round trip (sources mix UTF-8 and cp1252)
    out, n, prev, conds = [], 0, '', []
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
    hit = [(s, guard_externs(s, False)) for s in srcs]
    hit = [(s, n) for s, n in hit if n]
    for s, n in hit:
        print('EXTERNS', s, n)
    print('sources with unguarded extern data:', len(hit))
    if not a.apply:
        return 0
    bad = 0
    for s, n in hit:
        guard_externs(s, True)
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
            if f[2] == 'missing' and kind == 'missing' and P.get('bindings_keep_missing'):
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


def cmd_index(a):
    idx = build_index(PROFILES[a.profile]['own_mark'] if a.profile else None)
    if a.out:
        json.dump(idx, open(a.out, 'w'), indent=0)
    print(len(idx), 'rvas indexed from', len(rsp_tus()), 'files')
    return 0


def cmd_plan(a, write):
    b = Binder(a.profile, a.roots.split(','))
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
SELFTEST = {'raceload': '00426e10,004264d0',   # PORT-OT-RACE checkpoint 3 (verify/ro_20261116/RESULT_PORT-OT-RACE.md 7.1)
            'raceframe': '004111c0',            # PORT-TOP-06 (verify/porttop06_20261121/RESULT_PORT-TOP-06.md)
            'renderframe': '00492e90'}          # PORT-TOP-RENDERFRAME-492E90 (verify/porttoprenderframe_20261121/RESULT_PORT-TOP-RENDERFRAME-492E90.md)


def audio_roots():
    p = json.load(open(os.path.join(state_dir('audio'), 'plan.json')))
    called = {c for v in p.values() if v[0] == 'bind' for c in v[3] + v[4]}
    return ','.join(sorted(k for k in p if k not in called))


def cmd_selftest(a):
    bad = 0
    for prof in ('raceload', 'audio', 'raceframe', 'renderframe'):
        if not os.path.exists(os.path.join(state_dir(prof), 'plan.json')):
            print('[selftest] %s: no plan.json, skipped' % prof)
            continue
        roots = SELFTEST.get(prof) or audio_roots()
        pp = os.path.join(state_dir(prof), 'plan.json')
        before = json.load(open(pp))
        b = Binder(prof, roots.split(','))
        plan = {k: list(v) for k, v in b.build_plan().items()}
        same_plan = plan == before
        diffs = []
        for rva, p in sorted(b.plan.items()):
            if p[0] != 'bind':
                continue
            if rva in b.P.get('root_external', {}):
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
    sp.add_parser('selftest')
    a = ap.parse_args()
    if not os.path.exists('hooks.csv') or not os.path.exists(RSP):
        sys.exit('run from the worktree root')
    return {'plan': lambda: cmd_plan(a, False), 'write': lambda: cmd_plan(a, True), 'externs': lambda: cmd_externs(a),
            'sync': lambda: cmd_sync(a), 'bindings': lambda: cmd_bindings(a), 'unclean': lambda: cmd_unclean(a),
            'samesrc': lambda: cmd_samesrc(a), 'prune': lambda: cmd_prune(a), 'index': lambda: cmd_index(a),
            'selftest': lambda: cmd_selftest(a)}[a.cmd]()


if __name__ == '__main__':
    sys.exit(main())
