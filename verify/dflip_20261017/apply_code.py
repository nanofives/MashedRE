"""dflip: flip the default of the heading assist and the Warzone pontoon carry (user decisions 2026-10-17)."""
import os
S = os.path.join(os.path.dirname(os.path.abspath(__file__)), r'..\..\mashedmod\src\mashed_re')
def rep(rel, old, new):
    p = os.path.join(S, rel)
    t = open(p, encoding='utf-8', newline='').read()
    if chr(13)+chr(10) in t:
        old, new = old.replace(chr(10), chr(13)+chr(10)), new.replace(chr(10), chr(13)+chr(10))
    if t.count(new) == 1 and t.count(old) == 0:
        return   # already applied
    assert t.count(old) == 1, (rel, old[:60], t.count(old))
    open(p, 'w', encoding='utf-8', newline='').write(t.replace(old, new))
rep(r'Race\HeadingAssist.h',
    '// MASHED_HEADING_ASSIST=<mask> (default 0 = off). 0x1: AI slots 1..3.',
    '// Default ON (mask 0x1, user decision 2026-10-17, verify/dflip_20261017). MASHED_NO_HEADING_ASSIST=1 opts out;\n'
    '// MASHED_HEADING_ASSIST=<mask> overrides the mask (0 = off). 0x1: AI slots 1..3.')
rep(r'Race\HeadingAssist.cpp',
    '// Mashed RE - steer-0 heading assist (exe only, default OFF behind MASHED_HEADING_ASSIST).',
    '// Mashed RE - steer-0 heading assist (exe only, default ON since dflip 2026-10-17; MASHED_NO_HEADING_ASSIST=1 opts out).')
rep(r'Race\HeadingAssist.cpp',
    '''        const char* e = std::getenv("MASHED_HEADING_ASSIST");
        return (e && e[0]) ? static_cast<unsigned>(std::strtoul(e, nullptr, 0)) : 0u;''',
    '''        const char* no = std::getenv("MASHED_NO_HEADING_ASSIST");
        if (no && no[0] && no[0] != '0') return 0u;                          // [dflip] opt-out
        const char* e = std::getenv("MASHED_HEADING_ASSIST");
        return (e && e[0]) ? static_cast<unsigned>(std::strtoul(e, nullptr, 0)) : 1u;   // [dflip] default 0x1''')
rep(r'Track\WarzonePontoon.h',
    '// Knobs, all default OFF (PREREG section 1). STATE implies ANIM, CARRY implies STATE.',
    '// Knobs. CARRY implies STATE implies ANIM. Default ON since dflip 2026-10-17 (user decision; the exit hand-off\n'
    '// approximation U-9738 accepted). Opt-outs: MASHED_NO_PONTOON_CARRY=1 (STATE + ANIM stay), MASHED_NO_PONTOON_STATE=1\n'
    '// (ANIM stays), MASHED_NO_PONTOON_ANIM=1 (everything off = the pre-dflip default). The positive knobs still force on.')
rep(r'Track\WarzonePontoon.cpp',
    '''bool Pontoon_CarryOn() { static const bool on = Env("MASHED_PONTOON_CARRY"); return on; }
bool Pontoon_StateOn() { static const bool on = Pontoon_CarryOn() || Env("MASHED_PONTOON_STATE"); return on; }
bool Pontoon_AnimOn()  { static const bool on = Pontoon_StateOn() || Env("MASHED_PONTOON_ANIM"); return on; }''',
    '''// [dflip 2026-10-17] default ON; an opt-out also turns off every knob that implies it.
namespace {
bool NoAnim()  { return Env("MASHED_NO_PONTOON_ANIM"); }
bool NoState() { return NoAnim() || Env("MASHED_NO_PONTOON_STATE"); }
bool NoCarry() { return NoState() || Env("MASHED_NO_PONTOON_CARRY"); }
}  // namespace
bool Pontoon_CarryOn() { static const bool on = Env("MASHED_PONTOON_CARRY") || !NoCarry(); return on; }
bool Pontoon_StateOn() { static const bool on = Pontoon_CarryOn() || Env("MASHED_PONTOON_STATE") || !NoState(); return on; }
bool Pontoon_AnimOn()  { static const bool on = Pontoon_StateOn() || Env("MASHED_PONTOON_ANIM") || !NoAnim(); return on; }''')
rep(r'D3d9Render\TrackRenderer.cpp',
    '''before the physics frame (0x00425a8d), AI slots only. Default OFF
        // (MASHED_HEADING_ASSIST=1).''',
    '''before the physics frame (0x00425a8d), AI slots only. Default ON since
        // dflip 2026-10-17 (MASHED_NO_HEADING_ASSIST=1 opts out).''')
rep(r'D3d9Render\TrackRenderer.cpp',
    '// MASHED_PONTOON_ANIM (default OFF): one unbaked model per atomic',
    '// MASHED_PONTOON_ANIM (default ON since dflip, MASHED_NO_PONTOON_ANIM=1 opts out): one unbaked model per atomic')
rep(r'D3d9Render\TrackRenderer.cpp',
    'FUN_00451760(&obj, clump 1, General_Anim 0). Default OFF (MASHED_PONTOON_ANIM / _STATE / _CARRY).',
    'FUN_00451760(&obj, clump 1, General_Anim 0). Default ON since dflip (opt-outs MASHED_NO_PONTOON_ANIM / _STATE / _CARRY).')
rep(r'D3d9Render\TrackRenderer.cpp',
    'Inactive unless MASHED_PONTOON_* is set.',
    'Inactive with MASHED_NO_PONTOON_ANIM=1.')
print('ok')
