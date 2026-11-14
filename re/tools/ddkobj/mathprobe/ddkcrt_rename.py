"""L-120 (b): extract the XP DDK libc.lib objects behind sin/cos (and their libc closure) and rename every
symbol they define to `<prefix><name>`, references included, so a VS2022 exe can link them beside its own CRT and call
the DDK bodies under the new names only.

    py -3.12 ddkcrt_rename.py <ddk.map of a DDK-linked exe that used the functions> <outdir> <member> [<member> ...]
    py -3.12 ddkcrt_rename.py - <outdir> --fixed <member> ...     # exactly the named members, no map needed

Closure: start from the named libc members; every undefined symbol that the DDK map resolves to another libc member
pulls that member in. Symbols defined by the closure are renamed; undefined symbols outside the closure (kernel32
imports) are left alone. Prints the member list, the renamed count and the remaining externals.

DDKOBJ-CRTMATH: `build()` is the importable form, used by ddkobj.py (`build --ddkcrt`). MATH_MEMBERS is the member
list cut at the error path (RESULT_DDKOBJ-SPIKE section 7): the six names it leaves undefined resolve in the VS2022 CRT.
"""
import pathlib
import re
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
import coffedit as CE  # noqa: E402

DDK = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\tools\vc7ddk")
LIBC = DDK / "lib" / "wxp" / "i386" / "libc.lib"
PREFIX = "_ddkcrt"
MATH_MEMBERS = ["sin.obj", "cos.obj", "87disp.obj", "common.obj", "genexcep.obj", "87except.obj", "fpctrl.obj"]


def build(out, start, mp=None, fixed=True, ddk=DDK):
    """Extract `start` (libc members) into `out`, rename what they define to PREFIX+name. Returns
    (members, defined names (undecorated by nothing: raw COFF names), externals left, objs paths)."""
    out = pathlib.Path(out)
    libc = ddk / "lib" / "wxp" / "i386" / "libc.lib"
    out.mkdir(parents=True, exist_ok=True)
    sym2mem = {}
    if mp is not None:
        for line in pathlib.Path(mp).read_text(errors="replace").splitlines():
            m = re.match(r"\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+[0-9a-fA-F]{8}\s+(?:f\s+)?(?:i\s+)?libc:(\S+\.obj)", line)
            if m:
                sym2mem[m.group(1)] = m.group(2)
    env = {"PATH": str(ddk / "bin"), "SystemRoot": r"C:\Windows"}
    lst = subprocess.run([str(ddk / "bin" / "lib.exe"), "/nologo", "/LIST", str(libc)], capture_output=True, text=True,
                         env=env).stdout.split()
    full = {pathlib.PureWindowsPath(x).name.lower(): x for x in lst}
    todo, have = list(start), {}
    while todo:
        mem = todo.pop()
        if mem.lower() in have:
            continue
        dst = out / mem
        subprocess.run([str(ddk / "bin" / "lib.exe"), "/nologo", f"/EXTRACT:{full[mem.lower()]}", f"/OUT:{dst}",
                        str(libc)], capture_output=True, text=True, env=env)
        co = CE.Coff.load(dst)
        have[mem.lower()] = (dst, co)
        for s in co.syms:
            if not fixed and s.sec == 0 and s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL and s.value == 0 and s.name in sym2mem:
                todo.append(sym2mem[s.name])
    defined = set()
    for dst, co in have.values():
        for s in co.syms:
            if s.sec != 0 and s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL and not s.name.startswith(("@feat", ".")):
                defined.add(s.name)
    remaining = set()
    for dst, co in have.values():
        for s in co.syms:
            if s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL and s.name in defined:
                s.name = PREFIX + s.name
            elif s.cls == CE.IMAGE_SYM_CLASS_EXTERNAL and s.sec == 0 and s.value == 0:
                remaining.add(s.name)
        co.save(dst)
    objs = [d for d, _c in have.values()]
    (out / "objs.rsp").write_text("\n".join('"' + str(d) + '"' for d in objs) + "\n")
    return sorted(have), defined, remaining, objs


def main():
    mp = None if sys.argv[1] == "-" else pathlib.Path(sys.argv[1])
    out = pathlib.Path(sys.argv[2])
    start = [x for x in sys.argv[3:] if not x.startswith("--")]
    fixed = "--fixed" in sys.argv   # take exactly the named members, no closure
    members, defined, remaining, _objs = build(out, start, mp, fixed)
    print("members:", " ".join(members))
    print("renamed definitions:", len(defined))
    print("externals left (resolve against the VS2022 link):", " ".join(sorted(remaining)))


if __name__ == "__main__":
    main()
