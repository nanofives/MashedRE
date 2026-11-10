"""Classify every default-reached closed-set member of the five PORT-BATCH subsystems (one reason per row)."""
import csv, collections, sys
CL = 'verify/portbatch_20261109/closed/'
M = {r['rva']: r for r in csv.DictReader(open(CL + 'closed_members.tsv', encoding='utf-8'), delimiter='\t')}

# reason, note.  Reached rows (root_def=1) get their swap blocker; unreached rows get 'not reached' + own blocker.
R = {
 # swapped
 '0x00407b00': ('swapped', 'GAMEPLAY d80154ba9'), '0x00407b70': ('swapped', 'GAMEPLAY d80154ba9'),
 '0x00407d70': ('swapped', 'GAMEPLAY d80154ba9'), '0x00407db0': ('swapped', 'GAMEPLAY d80154ba9'),
 '0x005c9d00': ('swapped', 'GAMEPLAY d80154ba9'), '0x00496900': ('swapped', 'FRONTEND'),
 # already behind MASHED_BATCH_HUD (PORT-BATCH-TOOL, L-106 (b) keeps it OFF)
 '0x0041d870': ('switched', 'PORT-BATCH-TOOL body behind MASHED_BATCH_HUD, OFF per L-106 (b)'),
 '0x0041e850': ('switched', 'same'), '0x0041d410': ('switched', 'same (static callee of 0x0041d870)'),
 '0x0041e630': ('switched', 'same (static callee of 0x0041e850)'),
 # port-side value
 '0x0046c7b0': ('port-side value', 'VEHBASE: rebased vehicle record'), '0x0046cbb0': ('port-side value', 'VEHBASE'),
 '0x0046d4a0': ('port-side value', 'VEHBASE'), '0x0046c730': ('port-side value', 'VEHBASE'),
 '0x0046c750': ('port-side value', 'VEHBASE'), '0x0046d780': ('port-side value', 'VEHBASE'),
 '0x0046d7f0': ('port-side value', 'VEHBASE'),
 '0x00417cf0': ('port-side value', 'calls VEHBASE 0x0046d4a0; its matched file also defines 0x00417730 (exe has VehicleRaceAngleGet)'),
 '0x0041f360': ('port-side value', 'exe reads pinned g_dat_00636ad0 = 0 (CarMaterialTables.cpp:58), matched reads the exe pad'),
 # caller-supplied inputs
 '0x0040b890': ('caller-supplied inputs', 'GridCount8or12Core(cars, ea64, fd0): exe_main.cpp:4453, RwRaceFx.cpp:1473'),
 '0x0040b8e0': ('caller-supplied inputs', 'ScoreThreshold7or10Core(cars, ea64, fd0): exe_main.cpp:4455, RwRaceFx.cpp:1476'),
 # stand-in
 '0x0045c110': ('stand-in', 'SurfaceGate(const WorldHit*, uint32_t), PowerupContact.cpp:231 (no material colour)'),
 '0x0045bff0': ('stand-in', 'exe binds PK_SlotEmpty (PickupTick.cpp:140, host callback / replay)'),
 '0x00451820': ('stand-in', 'exe body is C++ Trigger(Obj&) over the port object and host callbacks (WarzonePontoon.cpp:361); port called, the tool did not resolve it as a root'),
 '0x0041db80': ('stand-in', 'exe forwarder calls LibRw::RaceFx_HudBindLights (librw start lights), MatchS02_0040dfc0.cpp:52'),
 '0x005554d0': ('stand-in', 'exe binds MenuBa_ExeMeasure (g_font glyph sum), MatchS06_004282a0.cpp:26'),
 # naked x87
 '0x004233e0': ('naked x87', 'verbatim FPATAN transcription kept for bit identity (HeadingAtan2.cpp); matched C calls atan under VS2022'),
 '0x00414490': ('naked x87', 'verbatim naked mode-7 M4 transcription with TG_ host bindings (AiModeSeven_naked.inc)'),
 '0x00414300': ('naked x87', 'verbatim naked P3 transcription (AiTargetClosest_naked.inc)'),
 '0x00414a70': ('naked x87', 'verbatim naked P3 transcription (AiTargetClosest_naked.inc)'),
 # RwGlobals
 '0x00403160': ('RWGLOBALS', 'reads RwGlobals (0x007d3ff8, 0 in the exe); exe forwarder is a no-op'),
 '0x004c3b30': ('RWGLOBALS', 'FastSqrt: matched body reads through RwGlobals; exe Math/RwLutTables.cpp'),
 '0x004c3bf0': ('RWGLOBALS', 'Vec2Length: matched body reads through RwGlobals; exe Math/RwV2d.cpp'),
 # DUPFILE (reached)
 '0x00430830': ('DUPFILE', 're/match/s08/004322c0.c also holds 0x0042f6b0 / 0x00430910 / 0x004322c0; exe already runs the matched MatchS07_00430830.cpp'),
 '0x00432b30': ('DUPFILE', 're/match/s08/00432b30.c also holds 0x0042ad10 (exe MatchS06_0042ad10.cpp) and 0x0042add0'),
}
def own(a):
    b = M[a]['blockers']
    for k in ('SEED', 'RWGLOBALS', 'DUPFILE', 'CROSS', 'DUP'):
        if k in b: return k
    return 'none'
SUBS = ['gameplay', 'hud', 'frontend', 'ai', 'render']
sets = {}
for s in SUBS:
    u = set()
    for r in csv.DictReader(open(CL + 'closed_%s.tsv' % s, encoding='utf-8'), delimiter='\t'):
        if 'default' in r['reached']: u |= set(r['rvas'].split())
    sets[s] = u
cls = {}
for s in SUBS:
    for a in sets[s]:
        if a in R: cls[a] = R[a]
        else:
            if M[a]['root_def'] == '1': print('UNCLASSIFIED reached', a, M[a]['name']); sys.exit(1)
            cls[a] = ('not reached', own(a))
if '--tables' in sys.argv:
    for s in SUBS:
        print('### %s (%d rows)' % (s, len(sets[s])))
        g = collections.defaultdict(list)
        for a in sorted(sets[s]): g[cls[a][0]].append(a)
        order = ['swapped', 'switched', 'stand-in', 'caller-supplied inputs', 'port-side value', 'naked x87', 'RWGLOBALS', 'DUPFILE', 'not reached']
        for k in order:
            if not g[k]: continue
            print('| %s (%d) | %s |' % (k, len(g[k]), '; '.join('%s %s%s' % (a, M[a]['name'], (' [' + cls[a][1] + ']') if k == 'not reached' else '') for a in g[k])))
        print()
# counts
allr = set().union(*sets.values())
print('distinct rows over the five sets:', len(allr))
c = collections.Counter(); cr = collections.Counter(); cu = collections.Counter()
for a in allr:
    c[cls[a][0]] += 1
    if cls[a][0] == 'not reached': cu[cls[a][1]] += 1
for s in SUBS:
    cc = collections.Counter(cls[a][0] for a in sets[s])
    print(s, len(sets[s]), dict(cc))
print('distinct by reason:', dict(c))
print('not reached, by own blocker:', dict(cu))
