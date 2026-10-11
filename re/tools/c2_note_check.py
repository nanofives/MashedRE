"""Cross-check generated C2 plates (re/tools/c2_note.py) against two independent witnesses:

  1. the Ghidra decompilation of the same functions (`decomp_pc.py --create --callees --json`): every callee,
     every global (`DAT_`/`PTR_`/`s_..._<addr>`), every hex constant and every float literal the decompiler
     prints must also appear in the plate;
  2. hand-written C2 plates (optional `--hand rva<TAB>path` list): every hex token and every `FUN_` callee the
     hand plate cites must appear in the generated plate.

It also re-runs the generator's hedge/purpose lint over every plate. Output: one block per RVA listing the
witness items missing from the plate; the defect classes are judged by reading those, not by this tool.

    py -3.12 re/tools/c2_note_check.py --plates <dir> --decomp log/x.json [--hand log/hand.tsv]
"""
import argparse, json, pathlib, re, sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import c2_note  # noqa: E402

HEX = re.compile(r"\b0x([0-9a-fA-F]+)\b")
SYM = re.compile(r"\b(?:_?DAT|PTR_DAT|PTR_FUN|PTR_LAB|PTR|FUN|LAB|s_\w*?|u_\w*?)_([0-9a-fA-F]{8})\b")
FLT = re.compile(r"(?<![\w.])(-?\d+\.\d+(?:[eE][-+]?\d+)?)(?![\w.])")


def plate_ints(text):
    return {int(h, 16) for h in HEX.findall(text)} | {int(d) for d in re.findall(r"\((-?\d+)\)", text) if int(d) >= 0}


def plate_floats(text):
    out = []
    for m in re.finditer(r"(?:float32|float64|float32 bits:?|bits|pushes|\()\s*([-+]?\d+\.\d*(?:e[-+]?\d+)?)", text):
        try:
            out.append(float(m.group(1)))
        except ValueError:
            pass
    return out


def check_decomp(text, fn):
    miss = []
    ints = plate_ints(text)
    body = fn.get("decomp") or ""
    body = re.sub(r"^.*?\{", "{", body, count=1, flags=re.S)        # drop the signature line
    for c in fn.get("callees") or []:
        nm, _, a = c.partition("@")
        if "EXTERNAL" in a:
            if nm not in text:
                miss.append(f"import callee {nm}")
            continue
        if a and int(a, 16) not in ints:
            miss.append(f"callee {c}")
    for a in sorted(set(SYM.findall(body))):
        v = int(a, 16)
        if v not in ints:
            miss.append(f"symbol address 0x{v:08x}")
    for h in sorted(set(HEX.findall(body)), key=lambda x: int(x, 16)):
        v = int(h, 16)
        if v in ints or ((-v) & 0xFFFFFFFF) in ints or v < 2:
            continue
        miss.append(f"hex constant 0x{v:x}")
    pf = plate_floats(text)
    for f in sorted(set(FLT.findall(body))):
        x = float(f)
        if not any(abs(x - y) <= 1e-6 * max(1.0, abs(x)) for y in pf):
            miss.append(f"float literal {f}")
    return miss


def check_hand(text, hand):
    miss = []
    ints = plate_ints(text)
    fm = re.search(r"callees_depth1:\s*\[(.*?)\]", hand)
    for a in re.findall(r"FUN_([0-9a-fA-F]{8})", fm.group(1) if fm else ""):
        if int(a, 16) not in ints:
            miss.append(f"hand callee FUN_{a}")
    body = re.sub(r"^---.*?---", "", hand, count=1, flags=re.S)
    for h in sorted(set(HEX.findall(body)) | set(SYM.findall(body)), key=lambda x: int(x, 16)):
        v = int(h, 16)
        if v in ints or v < 2:
            continue
        miss.append(f"hand token 0x{v:x}")
    return miss


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--plates", required=True)
    ap.add_argument("--decomp", required=True)
    ap.add_argument("--hand")
    a = ap.parse_args()
    fns = {int(f["requested"], 16): f for f in json.loads(pathlib.Path(a.decomp).read_text(encoding="utf-8"))["functions"]}
    hand = {}
    if a.hand:
        for l in pathlib.Path(a.hand).read_text(encoding="utf-8").splitlines():
            if l.strip():
                r, p = l.split("\t")
                hand[int(r, 16)] = p
    total = 0
    for va, fn in sorted(fns.items()):
        p = pathlib.Path(a.plates) / f"{va:08x}.md"
        if not p.exists():
            print(f"0x{va:08x}: NO PLATE")
            continue
        text = p.read_text(encoding="utf-8")
        c2_note.lint(text)
        miss = check_decomp(text, fn)
        if va in hand:
            hp = pathlib.Path(hand[va])
            if not hp.exists():
                hp = c2_note.xref.MAIN / hand[va]
            miss += check_hand(text, hp.read_text(encoding="utf-8", errors="replace"))
        total += len(miss)
        print(f"0x{va:08x} {fn.get('name')}: {'clean' if not miss else str(len(miss)) + ' missing'}")
        for m in miss:
            print(f"    {m}")
    print(f"lint: clean on {len(fns)} plates; {total} witness items missing in total")


if __name__ == "__main__":
    main()
