"""x87_port_images.py [--out D:/MashedWT/PORT-X87-PC24-AUDIT_data/img]

PORT-X87-PC24-AUDIT, emulation leg (PREREG_X87AUDIT.md section 3). Links the MATCHED TUs of x87_inventory.tsv
(every TU that #includes a re/match source) twice, unchanged, with the exe's flags (/EHa /W3 /O2 /DMASHED_STANDALONE):

  X  /arch:IA32 (the current build mode of every TU in x87_tus.txt)   /FIXED at 0x30000000
  S  /arch:SSE2 (MSVC's default)                                       /FIXED at 0x34000000

plus re/tools/veccap/port_image.cpp (stub HookSystem). Calls to a symbol that no linked TU defines go to the
original RVA the TU names for it (`#define FUN_<rva> <name>`): a naked `push <rva>; ret` forwarder bound with
/ALTERNATENAME, so the callee runs as the original's bytes in the emulator. Any other unresolved symbol binds to a
forwarder that executes int3 (the emulator reports it, never passes it). Nothing here runs natively.
"""
import argparse, os, re, subprocess, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[2]   # aispline_ddk/ copy (PORT-AISPLINE-DDKORDER)
SRC = REPO / 'mashedmod' / 'src' / 'mashed_re'
VCVARS = r'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
ap = argparse.ArgumentParser()
ap.add_argument('--out', default='D:/MashedWT/PORT-AISPLINE-DDKORDER_data/img')
ap.add_argument('--exe-build', default='', help='mashedmod/build dir of an exe build (callee objects + map); default this repo')
a = ap.parse_args()
OUT = Path(a.out)
OUT.mkdir(parents=True, exist_ok=True)
FLAGS = '/EHa /W3 /O2 /DMASHED_STANDALONE'
IMAGES = {'X': ('/arch:IA32', 0x30000000), 'S': ('', 0x34000000)}

inv = [l.split('\t') for l in (HERE / 'x87_inventory.tsv').read_text().splitlines()[1:]]
tus = []
for tu, sect, src, rva, how in inv:
    if how.startswith('match:') and src not in tus:
        tus.append(src)
defs = {}
for s in tus:
    for m in re.finditer(r'#define\s+FUN_([0-9a-f]{8})\s+(\w+)', (REPO / s).read_text(errors='replace')):
        defs.setdefault(m.group(2), int(m.group(1), 16))
env = {k: v for k, v in os.environ.items() if k.upper() not in ('CL', '_CL_')}
BUILD = Path(a.exe_build) if a.exe_build else REPO / 'mashedmod' / 'build'
EXEDIR = BUILD / 'obj' / 'exe'
EXEOBJ = {}   # symbol -> defining .obj in the exe link (mashed_re.map "Lib:Object" column)
for _l in (BUILD / 'mashed_re.map').read_text(errors='replace').splitlines():
    _p = _l.split()
    if len(_p) >= 4 and ':' in _p[0] and _p[-1].endswith('.obj') and ':' not in _p[-1]:
        EXEOBJ.setdefault(_p[1], _p[-1])


def sh(cmds, cwd):
    r = subprocess.run(f'call "{VCVARS}" >nul && ' + ' && '.join(cmds), shell=True, capture_output=True, text=True,
                       cwd=str(cwd), env=env)
    return r.returncode, r.stdout + r.stderr


def build(tag):
    arch, base = IMAGES[tag]
    d = OUT / tag
    d.mkdir(exist_ok=True)
    objs = []
    cmds = []
    for s in tus:
        objs.append(d / (Path(s).stem + '.obj'))
    NL = chr(10)
    (d / 'srcs.rsp').write_text(NL.join(f'"{REPO / s}"' for s in tus) + NL)
    cmds.append(f'cl /nologo /c /MP8 {FLAGS} {arch} /Fo{d}' + chr(92) + f' @"{d / "srcs.rsp"}"')
    host = d / 'port_image.obj'
    cmds.append(f'cl /nologo /c {FLAGS} /Fo"{host}" "{REPO / "re/tools/veccap/port_image.cpp"}"')
    rc, log = sh(cmds, d)
    if rc:
        sys.exit(f'[{tag}] compile failed:\n{log[-4000:]}')
    exe, mp = d / f'port_{tag}.exe', d / f'port_{tag}.map'
    (d / 'objs.rsp').write_text(NL.join(f'"{o}"' for o in objs) + NL)
    link = (f'link /nologo /OUT:"{exe}" /MAP:"{mp}" "{host}" @"{d / "objs.rsp"}"'
            + f' /BASE:0x{base:08x} /FIXED /DYNAMICBASE:NO /OPT:NOREF /OPT:NOICF /SUBSYSTEM:CONSOLE user32.lib')
    fwd = d / 'fwd.obj'

    def unresolved(lg):
        # LNK2019/LNK2001, English or Spanish linker locale ("simbolo externo _X sin resolver")
        return sorted(set(m.group(1) or m.group(2) for m in re.finditer(
            r'(?:unresolved external symbol|externo) (?:"[^"]*" \((\S+?)\)|([^\s"]+))', lg)))

    # Callees: up to 3 rounds of the exe's OWN objects (mashedmod/build/obj/exe, compiled with the exe's flags, the
    # exe's x87 list) that define a still-unresolved symbol, per the exe map. Then the port body calls what the exe
    # calls, register-argument conventions included. HookSystem stays stubbed (port_image.cpp).
    extra = []
    for rnd in range(3):
        rc, log = sh([link + ' /FORCE:UNRESOLVED /FORCE:MULTIPLE'], d)
        add = sorted({EXEOBJ[s] for s in unresolved(log) if s in EXEOBJ} - set(extra) - {o.name for o in objs})
        add = [o for o in add if (EXEDIR / o).exists() and not o.lower().startswith('hooksystem')]
        if not add:
            break
        extra += add
        (d / 'objs.rsp').write_text(NL.join(f'"{o}"' for o in objs) + NL + NL.join(f'"{EXEDIR / o}"' for o in extra) + NL)
    link += ' /FORCE:MULTIPLE'
    rc, log = sh([link + ' /FORCE:UNRESOLVED'], d)
    unres = unresolved(log)
    print(f'[{tag}] exe objects added for callees: {len(extra)}', flush=True)
    lines = ['// generated by x87_port_images.py', 'extern "C" {']
    alts = []
    unknown = []
    for i, sym in enumerate(unres):
        plain = re.sub(r'^[_@]', '', sym)
        plain = re.sub(r'@\d+$', '', plain)
        rva = defs.get(plain)
        mf = re.fullmatch(r'FUN_([0-9a-f]{8})', plain)
        if rva is None and mf:
            rva = int(mf.group(1), 16)   # a callee the TU calls by its original name
        fn = f'fwd_{i:04d}'
        if rva:
            lines.append(f'__declspec(naked) void {fn}(void) {{ __asm push 0x{rva:08x} __asm ret }}')
        else:
            lines.append(f'__declspec(naked) void {fn}(void) {{ __asm int 3 }}')
            unknown.append(sym)
        alts.append(f'/ALTERNATENAME:{sym}=_{fn}')
    lines.append('}')
    (d / 'fwd.cpp').write_text('\n'.join(lines) + '\n')
    (d / 'fwd.rsp').write_text('\n'.join(alts) + '\n')
    rc, log2 = sh([f'cl /nologo /c /O2 /Fo"{fwd}" "{d / "fwd.cpp"}"', link + f' "{fwd}" @"{d / "fwd.rsp"}"'], d)
    if rc:
        sys.exit(f'[{tag}] link failed:\n{log2[-4000:]}')
    print(f'[{tag}] {exe}  TUs={len(tus)} forwarders={len(unres) - len(unknown)} int3={len(unknown)} {unknown[:12]}')


for t in IMAGES:
    build(t)
