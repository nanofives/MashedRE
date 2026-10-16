"""L1 livery check (PREREG_librwrest_checks.md section 2).

    py -3.12 l1_check.py <arm>=<carparts.json>[,<standalone mashed_re.log>] ... [-o report.txt]

Original side: rec+0x268 descriptor per car (camera_probe.py --carparts, `livery`), the colour
DAT_007f1a1c[i*4] it read, the poke (colours / mode) camera_probe applied at FUN_0040d110's entry.
  L1-a: id % 6 == effective colour (slot index when the effective mode word is 0xb) and
        dff == <name><id % 6>.DFF  (FUN_0040d110 @ 0x0040d110 + FUN_0041f330 @ 0x0041f330).
  L1-b: the standalone's `[librwrest] livery: slot s ... -> <DFF>` equals the descriptor DFF
        (case-insensitive) for slot s, last 4 slot lines of its private mashed_re.log.
"""
import json
import re
import sys
from pathlib import Path

SLOT_RE = re.compile(r"\[librwrest\] livery: slot (\d) mode=(-?\d+) colour=(-?\d+) -> (\S+) \(model (\d+)\)")


def stand_slots(log: Path):
    last = {}
    for ln in log.read_text(errors="replace").splitlines():
        m = SLOT_RE.search(ln)
        if m:
            last[int(m.group(1))] = (int(m.group(2)), int(m.group(3)), m.group(4))
    return last


def check_arm(arm, cp_path, log_path, out):
    cp = json.loads(Path(cp_path).read_text())
    liv = cp.get("livery")
    poke = cp.get("poke") or {}
    ok_a, ok_b, have_b = True, True, log_path is not None
    out.append(f"== {arm}: {cp_path}")
    if not liv or "cars" not in liv:
        out.append(f"   NO livery record: {liv}")
        return False, False
    mode_read = liv.get("mode_word_0067e9fc")
    mode_eff = poke.get("mode", -1)
    if mode_eff is None or mode_eff < 0:
        mode_eff = mode_read
    out.append(f"   poke={poke} mode word at read={mode_read} effective mode at FUN_0040d110={mode_eff}")
    st = stand_slots(Path(log_path)) if log_path else {}
    for i, c in enumerate(liv["cars"]):
        if "dff" not in c:
            out.append(f"   car {i}: no descriptor ({c})")
            ok_a = False
            continue
        want_liv = i if mode_eff == 0xb else c["colour"]
        idm = c["id"] % 6
        m = re.match(r"^(.*?)(\d)\.dff$", c["dff"], re.I)
        base, digit = (m.group(1), int(m.group(2))) if m else (c["dff"], -1)
        a_ok = (idm == want_liv) and (digit == idm)
        ok_a &= a_ok
        line = (f"   car {i}: colour={c['colour']} id={c['id']} (id%6={idm}) name={c['name']} "
                f"dff={c['dff']} txd={c['txd']} env={c['env']}  expected livery {want_liv}  "
                f"L1-a {'PASS' if a_ok else 'FAIL'}")
        if have_b:
            s = st.get(i)
            if not s:
                line += "  L1-b FAIL (no standalone slot line)"
                ok_b = False
            else:
                b_ok = s[2].lower() == c["dff"].lower()
                ok_b &= b_ok
                line += f"  standalone slot {i}: colour={s[1]} mode={s[0]} -> {s[2]}  L1-b {'PASS' if b_ok else 'FAIL'}"
        out.append(line)
    out.append(f"   arm verdict: L1-a {'PASS' if ok_a else 'FAIL'}" +
               (f", L1-b {'PASS' if ok_b else 'FAIL'}" if have_b else ", L1-b n/a (no standalone side)"))
    return ok_a, (ok_b if have_b else None)


def main():
    args = sys.argv[1:]
    outp = None
    if "-o" in args:
        k = args.index("-o")
        outp = args[k + 1]
        del args[k:k + 2]
    out, allA, allB = [], True, True
    for spec in args:
        arm, rest = spec.split("=", 1)
        parts = rest.split(",")
        a, b = check_arm(arm, parts[0], parts[1] if len(parts) > 1 else None, out)
        allA &= a
        if b is not None:
            allB &= b
    out.append(f"L1 OVERALL: L1-a {'PASS' if allA else 'FAIL'}, L1-b {'PASS' if allB else 'FAIL'}")
    txt = "\n".join(out)
    print(txt)
    if outp:
        Path(outp).write_text(txt + "\n")
    return 0 if (allA and allB) else 1


if __name__ == "__main__":
    sys.exit(main())
