"""PROV-CRASH-12: link a candidate fix of a provisional match source without editing the source.

usage: py -3.12 re/tools/prov/prov_fixlink.py --out <bodies_fix.json> --work <scratch dir>

Each FIX below is a textual edit applied to a COPY of the committed source (written under --work), linked with
provlink.link() exactly as PROV-INSITU links the committed file. The detorig.py replay then runs the fixed blob
(--prov <bodies_fix.json>) to test whether the first-fault diagnosis is the whole crash. The committed sources under
re/match/ are not touched: an accepted fix lands in its own row through match.py / equiv.py / PROVISIONAL.tsv.
"""
import argparse
import difflib
import json
import pathlib
import re
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(HERE))
import provlink as P  # noqa: E402


def addr_of_byte_decls(text: str) -> str:
    """Code symbols declared as one byte (MGLOBAL(undefined1, FUN_x, ..) / extern undefined1 FUN_x) and passed as
    values: pass their address instead. Calls through such a name are left alone (none exist in the fixed files)."""
    names = set(re.findall(r"(?:MGLOBAL\(undefined1, *|extern undefined1 )((?:FUN|LAB)_00[0-9a-f]{6})\b", text))
    out = []
    for ln in text.splitlines(keepends=True):
        if re.match(r"\s*(?:MGLOBAL\(undefined1,|extern undefined1 )", ln):
            out.append(ln)
            continue
        for n in names:
            ln = re.sub(r"(?<![&\w])" + n + r"\b(?!\s*\()", "&" + n, ln)
        out.append(ln)
    return "".join(out)


FIXES = {
    # sentinel test: the original compares the node with param_1 + 0x10 (0x005aa108 lea eax,[esi+0x10]; 0x005aa10b cmp edi,eax)
    "0x005aa0c0": ("re/match/s25/005aa0c0.c", lambda t: t.replace("if (param_1 == puVar1 + 4) {", "if (puVar1 == param_1 + 4) {")),
    # callback arguments: FUN_005ac770 / FUN_005aaaa0 are pushed as addresses (0x005ac324 push 0x5ac770)
    "0x005ac210": ("re/match/s25/005ac210.c", addr_of_byte_decls),
    # chunk header: FUN_005ab380 writes 5 dwords into its 2nd argument (0x005ab3cd / 0x005ab3e4 mov [ecx+0x10],edx)
    "0x005abfa0": ("re/match/s25/005abfa0.c", lambda t: t.replace("typedef struct { int id; uint size; int c; int d; } SH;",
                                                                    "typedef struct { int id; uint size; int c; int d; int e; } SH;")),
    # FUN_005ae3a0 takes 2 stack arguments (0x005ae3b7 / 0x005ae3e2 read [esp+0x18] after 4 pushes); the original
    # tail-jumps (0x005a9f3a) with its own param_2 still in that slot
    "0x005a9f10": ("re/match/s25/005a9f10.c", lambda t: t.replace("void FUN_005ae3a0(undefined4 p0);", "void FUN_005ae3a0(undefined4 p0, undefined4 p1);")
                   .replace("FUN_005ae3a0(*(undefined4 *)(param_1 + 0x48));", "FUN_005ae3a0(*(undefined4 *)(param_1 + 0x48),param_2);")),
    # param_4[2] takes FUN_005b3b60's return value (0x005abacb call, 0x005abad0 mov [ebp+8],eax); the source stored an
    # uninitialised local before the call
    "0x005aba20": ("re/match/s25/005aba20.c", lambda t: t.replace("  param_4[2] = uVar3;\n  uVar3 = FUN_005b3b60(param_3,0,param_4);\n",
                                                                    "  param_4[2] = FUN_005b3b60(param_3,0,param_4);\n")),
    # list insert: node->next = head->next (0x005ad2b6 mov ecx,[ebx]; 0x005ad2bf mov [eax],ecx, ebx = [esp+0x18] = head);
    # the source stored the container *(param_2 + 0x4c) instead
    "0x005ad0b0": ("re/match/s26/005ad0b0.c", lambda t: t.replace("        *piVar2 = iVar4;\n        iVar4 = *piVar1;\n",
                                                                    "        *piVar2 = *piVar1;\n")),
    # FUN_004671a0 takes 1 argument and FUN_004c1b40 2: 0x00481bb5 push &sph; push 0; call 0x4671a0; add esp,4 (pops the
    # 0 only); push eax; call 0x4c1b40 -> FUN_004c1b40(FUN_004671a0(0), &sph)
    "0x00481a30": ("re/match/s13/00481a30.c", lambda t: t.replace("int FUN_004671a0(int p0, Sphere *p1);", "int FUN_004671a0(int p0);")
                   .replace("int FUN_004c1b40(int p0);", "int FUN_004c1b40(int p0, Sphere *p1);")
                   .replace("FUN_004c1b40(FUN_004671a0(0, &sph));", "FUN_004c1b40(FUN_004671a0(0), &sph);")),
    # Ghidra's local_38[i - 4] is local_48[i] (local_48[4] sits right below local_38 in Ghidra's frame); the original
    # indexes one array, 0x005b3cd4 mov ecx,[esp+esi*4+0x24]
    "0x005b3b80": ("re/match/s27/005b3b80.c", lambda t: t.replace("local_38[(int)param_1 - 4]", "local_48[(int)param_1]")),
    # same header writer as 0x005abfa0: FUN_005ab380 fills 5 dwords, the source's hdr holds 3
    "0x005a7b60": ("re/match/s25/005a7b60.c", lambda t: t.replace("  int hdr[3];\n", "  int hdr[5];\n")),
    # three order defects against the original: the exponent is read from fVar11's bits (0x005a7655..0x005a7664, after
    # the store); node->prev takes the old prev before the target's prev is overwritten (0x005a777b..0x005a7788); the old
    # tail's next is linked before DAT_007dca1c moves (0x005a77e6 mov eax,[0x7dca1c]; mov [eax],ecx; then
    # 0x005a77ee mov [0x7dca1c],ecx)
    "0x005a75b0": ("re/match/s25/005a75b0.c", lambda t: t
                   .replace("      uVar10 = (u.u >> 0x17) - 0x7f;\n      u.f = fVar11;\n", "      u.f = fVar11;\n      uVar10 = (u.u >> 0x17) - 0x7f;\n")
                   .replace("      piVar9->next = piVar12;\n      *(V **)piVar12->prev = piVar9;\n      piVar12->prev = piVar9;\n      piVar9->prev = piVar12->prev;\n",
                            "      piVar9->next = piVar12;\n      piVar9->prev = piVar12->prev;\n      *(V **)piVar12->prev = piVar9;\n      piVar12->prev = piVar9;\n")
                   .replace("    DAT_007dca1c = param_1;\n    *(V **)DAT_007dca1c = param_1;\n", "    *(V **)DAT_007dca1c = param_1;\n    DAT_007dca1c = param_1;\n")),
    # the 66 handler arguments of FUN_0047b980 are passed as addresses
    "0x00440bc0": ("re/match/tlr/00440bc0.c", addr_of_byte_decls),
}

ap = argparse.ArgumentParser()
ap.add_argument("--out", required=True)
ap.add_argument("--work", required=True)
ap.add_argument("--rvas", default=",".join(FIXES))
a = ap.parse_args()
work = pathlib.Path(a.work)
work.mkdir(parents=True, exist_ok=True)
bodies = []
for r in [x.strip() for x in a.rvas.split(",") if x.strip()]:
    rel, fx = FIXES[r]
    text = (ROOT / rel).read_text(encoding="utf-8", errors="replace")
    new = fx(text)
    nchg = sum(1 for d in difflib.ndiff(text.splitlines(), new.splitlines()) if d[:1] in '-+')
    if nchg == 0:
        raise SystemExit(f"{r}: the fix did not apply to {rel}")
    dst = work / pathlib.Path(rel).name
    dst.write_text(new, encoding="utf-8")
    res = P.link(dst, int(r, 16))
    res.update({"rva": r, "source": rel + " (+fix)", "fix_lines": nchg})
    bodies.append(res)
    print(f"{r}\t{rel}\tlines changed {nchg}\t{res['status']}\tsize {res.get('size')}\t{','.join(res.get('unresolved', []))[:120]}", flush=True)
pathlib.Path(a.out).write_text(json.dumps({"bodies": bodies}, indent=0))
