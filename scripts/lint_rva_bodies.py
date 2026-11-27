"""One RVA, one body -- build-time guard.

Run from `mashedmod/build.bat` before the compile step. It scans the TUs compiled
into EACH target and reports an RVA that has more than one body, which is how
five shipping defects reached `mashed_re.exe` without any build or tracker
noticing (`re/analysis/DUAL_COPY_AUDIT_2026-09-29.md`).

Three checks:

  DUP-IN-TARGET   one RVA, two bodies in TUs compiled into the SAME target.
                  Only one of them can be the one that runs.
  CROSS-TARGET    one RVA with a body in an EXE-ONLY TU and a body in an
                  ASI-ONLY TU. This is the dual-copy class: Frida evidence
                  measures the .asi body, the shipping exe runs the other one,
                  and nothing links the two.
  DUP-INSTALL     one RVA with two ACTIVE `RH_ScopedInstall` in .asi-compiled
                  TUs. One body is then silently dead in the .asi -- the U-9065
                  failure (`path1-green-does-not-prove-install`). Reported as a
                  SEPARATE list because it is an independent defect: it is about
                  which body installs, not about which body exists.

Policy (2026-09-29): WARN on anything already listed in
`re/tools/dual_copy_allowlist.txt`, **FAIL on anything new**. The allowlist is a
burn-down list, not a permanent exemption -- ROADMAP D4 takes it to zero by
consolidating each pair into one shared TU judged against the original.

ORIGTOP-ONLY class (HB-LINT-ORIGTOP-CLASS, L-111 (a), 2026-11-09). The MASHED_ORIG_TOP route compiles byte-matched
bodies that run only when the exe enters the matched WinMain 0x00492370; until the ORIG_TOP flip they sit beside the
hand bodies the default path still runs. Such a pair is allowed, as its own class, when:
  * the matched body's TU carries the structural marker line `// RVA-LINT: ORIGTOP-ONLY` (anywhere in its own text,
    not an included file), and that TU is EXE-ONLY (ORIG_TOP is an exe path; a marked TU in asi_sources.rsp fails);
  * exactly one body of the finding is in a marked TU and at least one is in an unmarked TU (two ORIG_TOP bodies of
    one RVA are a real duplicate, not this class);
  * its allowlist line carries the tag: `ORIGTOP-ONLY <CHECK>:<rva>:<files>   # citation`.
A tagged line whose finding is not of the class, or a class finding on an untagged line, fails the build. The lint
prints the D4 burn-down count and the ORIGTOP-ONLY count separately. The whole class is deleted at the ORIG_TOP flip.

ORIGTOP-HELD class (LINT-ORIGTOP-HELDPAIR, L-179, 2026-11-21, a tool rule inside L-111). An rva whose only bodies are
static held copies inside ORIG_TOP wrappers: two or more bodies of one RVA in the same target, EVERY one of them in a TU
carrying the `// RVA-LINT: ORIGTOP-ONLY` marker (no unmarked body), and not already the L-178 scaffold dual. Its allowlist
line carries the tag `ORIGTOP-HELD <CHECK>:<rva>:<files>   # citation`. It is counted apart from the D4 burn-down and from
ORIGTOP-ONLY, and is deleted at the ORIG_TOP flip like ORIGTOP-ONLY. A pair with one unmarked body is not this class: it
stays D4 (or ORIGTOP-ONLY when exactly one body is marked). A held pair on an untagged line stays in D4 until its line is tagged
(so adding the class moves no count by itself). CROSS-TARGET and DUP-INSTALL are never this class.

ORIGTOP-HELD-HAND class (LINT-HELDPAIR-HANDCOPY, L-183 (1), 2026-11-27, a tool rule inside L-111). An rva with two or more
bodies in marked TUs plus at least one unmarked hand copy in the same target (DUP-IN-TARGET only). ORIGTOP-ONLY needs exactly
one marked body and ORIGTOP-HELD no unmarked body, so neither covers it. The tag `ORIGTOP-HELD-HAND <CHECK>:<rva>:<files>
# citation` is counted apart from D4, ORIGTOP-ONLY and ORIGTOP-HELD, and deleted at the ORIG_TOP flip (the hand copy itself is
frozen until T4). An untagged line of the shape stays in D4 until tagged. CROSS-TARGET and DUP-INSTALL are never this class.

  py -3.12 scripts/lint_rva_bodies.py              # guard mode (build.bat calls this)
  py -3.12 scripts/lint_rva_bodies.py --seed       # print an allowlist for today's findings (class lines tagged)
  py -3.12 scripts/lint_rva_bodies.py --verbose    # also print the allowlisted ones
  py -3.12 scripts/lint_rva_bodies.py --selftest   # the ORIGTOP-ONLY rules on synthetic pairs
"""
from __future__ import annotations

import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rva_body_scan as S            # noqa: E402

ROOT = S.ROOT
ALLOWLIST = ROOT / "re" / "tools" / "dual_copy_allowlist.txt"
OT_TAG = "ORIGTOP-ONLY"
HELD_TAG = "ORIGTOP-HELD"
HAND_TAG = "ORIGTOP-HELD-HAND"
OT_MARK = re.compile(r"^\s*//\s*RVA-LINT:\s*ORIGTOP-ONLY\b")


def key(check: str, rva: int, files) -> str:
    return f"{check}:{rva:08x}:{','.join(sorted(files))}"


def parse_allowlist(lines) -> dict[str, str]:
    """{key: tag}, tag in ("", ORIGTOP-ONLY, ORIGTOP-HELD). A line is `[<tag> ]<CHECK>:<rva>:<files>   # comment`."""
    keys = {}
    for ln in lines:
        ln = ln.split("#", 1)[0].strip()
        if not ln:
            continue
        tagged = ""
        for tag in (OT_TAG, HELD_TAG, HAND_TAG):
            if ln.startswith(tag + " ") or ln.startswith(tag + "	"):
                tagged = tag
                ln = ln[len(tag):].strip()
                break
        keys[ln] = tagged
    return keys


def load_allowlist() -> dict[str, str]:
    if not ALLOWLIST.exists():
        return {}
    return parse_allowlist(ALLOWLIST.read_text(encoding="utf-8").splitlines())


def marked_tus(tus) -> set[str]:
    """TUs whose own text carries the ORIGTOP-ONLY marker line."""
    out = set()
    for rel in tus:
        p = S.SRC / rel
        try:
            for ln in p.read_text(encoding="utf-8", errors="replace").splitlines():
                if OT_MARK.match(ln):
                    out.add(rel)
                    break
        except OSError:
            pass
    return out


def findings(scan=None) -> list[tuple[str, int, list[str], str]]:
    """[(check, rva, files, human_detail), ...] -- deterministic order."""
    scan = scan or S.scan_all()
    exe_tus, asi_tus = scan["targets"]["exe"], scan["targets"]["asi"]
    exe_only, asi_only = exe_tus - asi_tus, asi_tus - exe_tus
    out = []

    # DUP-IN-TARGET
    for target in ("exe", "asi"):
        for rva, tus in sorted(S.rva_to_tus(scan, target).items()):
            if len(tus) > 1:
                where = ", ".join(f"{t}:{scan['bodies'][t][rva][1]} "
                                  f"({scan['bodies'][t][rva][0]})" for t in sorted(tus))
                out.append(("DUP-IN-TARGET", rva, sorted(tus),
                            f"[{target}] two bodies: {where}"))

    # CROSS-TARGET
    eb, ab = S.rva_to_tus(scan, "exe"), S.rva_to_tus(scan, "asi")
    for rva in sorted(set(eb) & set(ab)):
        e = [t for t in eb[rva] if t in exe_only]
        a = [t for t in ab[rva] if t in asi_only]
        if e and a:
            out.append(("CROSS-TARGET", rva, sorted(set(e) | set(a)),
                        f"exe-only {', '.join(sorted(e))}  vs  "
                        f"asi-only {', '.join(sorted(a))}"))

    # DUP-INSTALL (.asi only -- in the exe RH_ScopedInstall is a no-op)
    per_rva: dict[int, list[tuple[str, str, int]]] = {}
    for rel, ins in scan["installs"].items():
        if rel not in asi_tus:
            continue
        for rva, sites in ins.items():
            for sym, line in sites:
                per_rva.setdefault(rva, []).append((rel, sym, line))
    for rva, sites in sorted(per_rva.items()):
        files = {f for f, _, _ in sites}
        if len(files) > 1:
            where = ", ".join(f"{f}:{ln} {sym}" for f, sym, ln in sorted(sites))
            out.append(("DUP-INSTALL", rva, sorted(files),
                        f"installed from {len(files)} files: {where}"))
    return out


RE_SCAFFOLD = re.compile(r"^RwCore/RwCore_(?:Lifted_\d+|Hand)\.cpp$")


def is_origtop(files, marked) -> bool:
    m = [f for f in files if f in marked]
    if len(m) == 1 and len(files) > len(m):
        return True
    # L-178 dual body (BIND-CUNCLEAN-AS-C, 2026-11-21): two marked ORIG_TOP bodies, one of them the RW core's
    # provisional scaffolding (L-137 lifted naked asm RwCore/RwCore_Lifted_NN.cpp, or RwCore/RwCore_Hand.cpp) and the
    # other a matched unit of the same rva (ddkobj "dual body": byte-checked, pre-bound). Exactly one scaffold TU.
    return (len(files) == 2 and len(m) == 2
            and sum(1 for f in files if RE_SCAFFOLD.match(f.replace("\\", "/"))) == 1)


def is_held(check, files, marked) -> bool:
    """ORIGTOP-HELD: two or more bodies of one rva, all in marked TUs, not the L-178 scaffold dual."""
    return (check == "DUP-IN-TARGET" and len(files) >= 2 and all(f in marked for f in files)
            and not is_origtop(files, marked))


def is_held_hand(check, files, marked) -> bool:
    """ORIGTOP-HELD-HAND: two or more bodies of one rva in marked TUs plus at least one unmarked hand copy."""
    return (check == "DUP-IN-TARGET" and sum(1 for f in files if f in marked) >= 2
            and any(f not in marked for f in files))


def evaluate(found, allow: dict[str, str], marked: set[str], asi_tus: set[str]):
    """-> (new, old_d4, old_ot, errors, old_held, old_hand). `errors` are class / tag / marker violations (each fails the build)."""
    new, old_d4, old_ot, old_held, old_hand, errors = [], [], [], [], [], []
    for rel in sorted(marked & asi_tus):
        errors.append(f"{OT_TAG} marker in {rel}, which asi_sources.rsp compiles (ORIG_TOP is an exe path)")
    for f in found:
        check, rva, files, _detail = f
        k = key(check, rva, files)
        cls = check != "DUP-INSTALL" and is_origtop(files, marked)
        held = is_held(check, files, marked)
        hand = is_held_hand(check, files, marked)
        if k not in allow:
            new.append(f)
            continue
        tagged = allow[k]
        if tagged == HAND_TAG:
            if hand:
                old_hand.append(f)
            else:
                errors.append(f"{k} is tagged {HAND_TAG} but is not of the class (needs two or more bodies in TUs marked "
                              f"`// RVA-LINT: ORIGTOP-ONLY` plus at least one unmarked body, DUP-IN-TARGET)")
        elif tagged == HELD_TAG:
            if held:
                old_held.append(f)
            else:
                errors.append(f"{k} is tagged {HELD_TAG} but is not of the class (needs two or more bodies of one "
                              f"rva, every one in a TU marked `// RVA-LINT: ORIGTOP-ONLY`, no unmarked body)")
        elif tagged and not cls:
            errors.append(f"{k} is tagged {OT_TAG} but is not of the class (needs exactly one body in a TU marked "
                          f"`// RVA-LINT: ORIGTOP-ONLY` beside an unmarked one)")
        elif cls and not tagged:
            errors.append(f"{k} is an {OT_TAG} pair on an untagged line: prefix the line with `{OT_TAG} `")
        elif cls:
            old_ot.append(f)
        else:
            old_d4.append(f)
    return new, old_d4, old_ot, errors, old_held, old_hand


def selftest() -> int:
    hand, ot, ot2, asi_hand = "Gameplay/Hand.cpp", "Match/OrigTop_Synth.cpp", "Match/OrigTop_Synth2.cpp", "Util/AsiOnly.cpp"
    sc, sc2 = "RwCore/RwCore_Lifted_03.cpp", "RwCore/RwCore_Hand.cpp"   # L-178 dual body fixtures
    marked = {ot, ot2, "Match/OrigTop_Synth3.cpp", sc, sc2}
    f_dual = ("DUP-IN-TARGET", 0x00401040, sorted([ot, sc]), "synthetic")
    f_scsc = ("DUP-IN-TARGET", 0x00401050, sorted([sc, sc2]), "synthetic")
    f_pair = ("DUP-IN-TARGET", 0x00401000, sorted([hand, ot]), "synthetic")
    f_cross = ("CROSS-TARGET", 0x00401010, sorted([asi_hand, ot]), "synthetic")
    f_both = ("DUP-IN-TARGET", 0x00401020, sorted([ot, ot2]), "synthetic")
    f_plain = ("DUP-IN-TARGET", 0x00401030, sorted([hand, asi_hand]), "synthetic")
    f_triple = ("DUP-IN-TARGET", 0x00401060, sorted([ot, ot2, "Match/OrigTop_Synth3.cpp"]), "synthetic")
    f_cross_marked = ("CROSS-TARGET", 0x00401070, sorted([ot, ot2]), "synthetic")
    f_hand = ("DUP-IN-TARGET", 0x00401080, sorted([ot, ot2, hand]), "synthetic")
    f_hand2 = ("DUP-IN-TARGET", 0x00401090, sorted([ot, ot2, hand, "Gameplay/Hand2.cpp"]), "synthetic")
    f_cross_hand = ("CROSS-TARGET", 0x004010a0, sorted([ot, ot2, asi_hand]), "synthetic")
    cases = [
        ("tagged ORIG_TOP pair is allowed, counted ORIGTOP-ONLY", [f_pair],
         [f"{OT_TAG} {key(*f_pair[:3])}"], set(), (0, 0, 1, 0)),
        ("tagged ORIG_TOP cross-target pair is allowed", [f_cross],
         [f"{OT_TAG} {key(*f_cross[:3])}"], set(), (0, 0, 1, 0)),
        ("untagged ORIG_TOP pair fails", [f_pair], [key(*f_pair[:3])], set(), (0, 0, 0, 1)),
        ("unlisted ORIG_TOP pair is NEW", [f_pair], [], set(), (1, 0, 0, 0)),
        ("two marked bodies are not the class: tag fails", [f_both],
         [f"{OT_TAG} {key(*f_both[:3])}"], set(), (0, 0, 0, 1)),
        ("L-178 dual body (marked unit + marked RW core scaffold) is the class", [f_dual],
         [f"{OT_TAG} {key(*f_dual[:3])}"], set(), (0, 0, 1, 0)),
        ("two marked scaffolds are not the class: tag fails", [f_scsc],
         [f"{OT_TAG} {key(*f_scsc[:3])}"], set(), (0, 0, 0, 1)),
        ("tag on a plain dual copy fails", [f_plain], [f"{OT_TAG} {key(*f_plain[:3])}"], set(), (0, 0, 0, 1)),
        ("plain dual copy untagged stays D4 burn-down", [f_plain], [key(*f_plain[:3])], set(), (0, 1, 0, 0)),
        ("marker on an asi-compiled TU fails", [f_pair], [f"{OT_TAG} {key(*f_pair[:3])}"], {ot}, (0, 0, 1, 1)),
        # ORIGTOP-HELD (L-179): all bodies in marked TUs, two or more, no unmarked body
        ("held pair (two marked bodies) tagged ORIGTOP-HELD lands in the held class", [f_both],
         [f"{HELD_TAG} {key(*f_both[:3])}"], set(), (0, 0, 0, 0, 1)),
        ("held triple (three marked bodies) tagged ORIGTOP-HELD lands in the held class", [f_triple],
         [f"{HELD_TAG} {key(*f_triple[:3])}"], set(), (0, 0, 0, 0, 1)),
        ("held pair on an untagged line stays D4 burn-down until tagged", [f_both], [key(*f_both[:3])], set(), (0, 1, 0, 0, 0)),
        ("held pair on an ORIGTOP-ONLY tag fails", [f_both], [f"{OT_TAG} {key(*f_both[:3])}"], set(), (0, 0, 0, 1, 0)),
        ("unlisted held pair is NEW", [f_both], [], set(), (1, 0, 0, 0, 0)),
        ("pair with one unmarked body tagged ORIGTOP-HELD fails", [f_pair],
         [f"{HELD_TAG} {key(*f_pair[:3])}"], set(), (0, 0, 0, 1, 0)),
        ("pair with one marked and one unmarked body, untagged, fails (ORIGTOP-ONLY class, unchanged)", [f_pair],
         [key(*f_pair[:3])], set(), (0, 0, 0, 1, 0)),
        ("plain dual copy tagged ORIGTOP-HELD fails", [f_plain], [f"{HELD_TAG} {key(*f_plain[:3])}"], set(), (0, 0, 0, 1, 0)),
        ("cross-target never held", [f_cross_marked], [f"{HELD_TAG} {key(*f_cross_marked[:3])}"], set(), (0, 0, 0, 1, 0)),
        ("L-178 scaffold dual keeps ORIGTOP-ONLY, not held", [f_dual], [f"{HELD_TAG} {key(*f_dual[:3])}"], set(), (0, 0, 0, 1, 0)),
        ("held pair beside a plain D4 line: counts stay apart", [f_both, f_plain],
         [f"{HELD_TAG} {key(*f_both[:3])}", key(*f_plain[:3])], set(), (0, 1, 0, 0, 1)),
        # ORIGTOP-HELD-HAND (L-183 (1)): two or more marked bodies plus an unmarked hand copy
        ("held pair + hand copy tagged ORIGTOP-HELD-HAND lands in the hand class", [f_hand],
         [f"{HAND_TAG} {key(*f_hand[:3])}"], set(), (0, 0, 0, 0, 0, 1)),
        ("held pair + hand copy untagged stays D4 burn-down until tagged", [f_hand], [key(*f_hand[:3])], set(), (0, 1, 0, 0, 0, 0)),
        ("unlisted held pair + hand copy is NEW", [f_hand], [], set(), (1, 0, 0, 0, 0, 0)),
        ("held pair + hand copy tagged ORIGTOP-HELD fails", [f_hand],
         [f"{HELD_TAG} {key(*f_hand[:3])}"], set(), (0, 0, 0, 1, 0, 0)),
        ("held pair + hand copy tagged ORIGTOP-ONLY fails", [f_hand],
         [f"{OT_TAG} {key(*f_hand[:3])}"], set(), (0, 0, 0, 1, 0, 0)),
        ("two marked + two unmarked bodies tagged ORIGTOP-HELD-HAND lands in the hand class", [f_hand2],
         [f"{HAND_TAG} {key(*f_hand2[:3])}"], set(), (0, 0, 0, 0, 0, 1)),
        ("two marked bodies, no hand copy, tagged ORIGTOP-HELD-HAND fails", [f_both],
         [f"{HAND_TAG} {key(*f_both[:3])}"], set(), (0, 0, 0, 1, 0, 0)),
        ("one marked + one unmarked tagged ORIGTOP-HELD-HAND fails", [f_pair],
         [f"{HAND_TAG} {key(*f_pair[:3])}"], set(), (0, 0, 0, 1, 0, 0)),
        ("plain dual copy tagged ORIGTOP-HELD-HAND fails", [f_plain],
         [f"{HAND_TAG} {key(*f_plain[:3])}"], set(), (0, 0, 0, 1, 0, 0)),
        ("L-178 scaffold dual tagged ORIGTOP-HELD-HAND fails", [f_dual],
         [f"{HAND_TAG} {key(*f_dual[:3])}"], set(), (0, 0, 0, 1, 0, 0)),
        ("held pair + hand copy cross-target never hand class", [f_cross_hand],
         [f"{HAND_TAG} {key(*f_cross_hand[:3])}"], set(), (0, 0, 0, 1, 0, 0)),
        ("hand class beside held pair and plain D4: counts stay apart", [f_hand, f_both, f_plain],
         [f"{HAND_TAG} {key(*f_hand[:3])}", f"{HELD_TAG} {key(*f_both[:3])}", key(*f_plain[:3])], set(), (0, 1, 0, 0, 1, 1)),
    ]
    bad = 0
    for name, found, lines, asi, want in cases:
        new, d4, otl, err, held, hnd = evaluate(found, parse_allowlist(lines), marked, asi)
        got = (len(new), len(d4), len(otl), len(err), len(held), len(hnd))
        want = want + (0,) * (6 - len(want))
        ok = got == want
        bad += not ok
        print(f"[rva-lint selftest] {'PASS' if ok else 'FAIL'} {name}: new/d4/origtop/errors/held/hand {got} want {want}")
    # the marker regex on real text shapes
    for line, want in (("// RVA-LINT: ORIGTOP-ONLY", True), ("  //RVA-LINT:ORIGTOP-ONLY (L-111)", True),
                       ("// RVA-LINT: ORIGTOP-ONLYX", False), ("/* RVA-LINT: ORIGTOP-ONLY */", False)):
        ok = bool(OT_MARK.match(line)) == want
        bad += not ok
        print(f"[rva-lint selftest] {'PASS' if ok else 'FAIL'} marker {line!r} -> {want}")
    print(f"[rva-lint selftest] {'OK' if not bad else 'FAILED'} ({bad} failing)")
    return 1 if bad else 0


def main(argv) -> int:
    if "--selftest" in argv:
        return selftest()
    seed = "--seed" in argv
    verbose = "--verbose" in argv
    allow = load_allowlist()
    scan = S.scan_all()
    found = findings(scan)
    exe_tus, asi_tus = scan["targets"]["exe"], scan["targets"]["asi"]
    marked = marked_tus(exe_tus | asi_tus)

    if seed:
        print("# dual_copy_allowlist.txt -- seeded "
              "by `py -3.12 scripts/lint_rva_bodies.py --seed`")
        for check, rva, files, detail in found:
            tag = (f"{OT_TAG} " if check != "DUP-INSTALL" and is_origtop(files, marked)
                   else f"{HELD_TAG} " if is_held(check, files, marked)
                   else f"{HAND_TAG} " if is_held_hand(check, files, marked) else "")
            print(f"{tag}{key(check, rva, files)}   # {detail}")
        return 0

    # HB-LINT-408A50: an unresolved re/match/ or .inc include hides bodies, so every
    # count below would be wrong. Fail before reporting them (typical cause: a tree
    # built from a `git archive` that left out re/match/).
    missing = sorted(set(S.MISSING_INCLUDES))
    if missing:
        for rel, line, inc in missing:
            print(f"[rva-lint] ERROR MISSING-INCLUDE {rel}:{line}  \"{inc}\" does not resolve")
        print(f"[rva-lint] FAILED: {len(missing)} include(s) the scanner could not follow, "
              "so their bodies are invisible and the duplicate check is unsound.")
        print("[rva-lint] Build from a tree that has them (scripts/lan_release/archive_paths.txt "
              "lists what a clean `git archive` build needs).")
        return 1

    new, old_d4, old_ot, errors, old_held, old_hand = evaluate(found, allow, marked, asi_tus)
    old = old_d4 + old_ot + old_held + old_hand

    by_check = {}
    for check, _, _, _ in found:
        by_check[check] = by_check.get(check, 0) + 1
    print(f"[rva-lint] {len(found)} finding(s): "
          + ", ".join(f"{k}={v}" for k, v in sorted(by_check.items()))
          + f"  |  allowlisted={len(old)} NEW={len(new)}")
    print(f"[rva-lint] D4 burn-down={len(old_d4)}  {OT_TAG}={len(old_ot)} "
          f"{HELD_TAG}={len(old_held)} {HAND_TAG}={len(old_hand)} "
          f"(classes deleted at the ORIG_TOP flip; {len(marked)} marked TU(s))")

    if verbose:
        for check, rva, files, detail in old_d4:
            print(f"[rva-lint]   WARN (allowlisted) {check} 0x{rva:08x}  {detail}")
        for check, rva, files, detail in old_ot:
            print(f"[rva-lint]   INFO ({OT_TAG}) {check} 0x{rva:08x}  {detail}")
        for check, rva, files, detail in old_held:
            print(f"[rva-lint]   INFO ({HELD_TAG}) {check} 0x{rva:08x}  {detail}")
        for check, rva, files, detail in old_hand:
            print(f"[rva-lint]   INFO ({HAND_TAG}) {check} 0x{rva:08x}  {detail}")

    dup_install_new = [f for f in new if f[0] == "DUP-INSTALL"]
    other_new = [f for f in new if f[0] != "DUP-INSTALL"]

    for check, rva, files, detail in other_new:
        tag = (f" [{OT_TAG} pair: allowlist it with the `{OT_TAG} ` tag]" if is_origtop(files, marked)
               else f" [{HELD_TAG} pair: allowlist it with the `{HELD_TAG} ` tag]" if is_held(check, files, marked)
               else f" [{HAND_TAG} class: allowlist it with the `{HAND_TAG} ` tag]" if is_held_hand(check, files, marked) else "")
        print(f"[rva-lint] ERROR {check} 0x{rva:08x}  {detail}{tag}")
    for check, rva, files, detail in dup_install_new:
        print(f"[rva-lint] ERROR {check} 0x{rva:08x}  {detail}")
    for e in errors:
        print(f"[rva-lint] ERROR {e}")

    if new or errors:
        print()
        if new:
            print(f"[rva-lint] FAILED: {len(new)} NEW duplicate-body finding(s).")
            print("[rva-lint] One RVA must have ONE body per target. Either share a "
                  "single TU between the two source lists, or -- if the divergence is "
                  "deliberate and judged against the original -- add the key above to")
            print(f"[rva-lint]   {ALLOWLIST.relative_to(ROOT)}")
            print("[rva-lint] with a citation. Seed a fresh list with --seed.")
        if errors:
            print(f"[rva-lint] FAILED: {len(errors)} {OT_TAG} class / tag / marker error(s).")
        return 1

    print("[rva-lint] OK -- no NEW duplicate bodies "
          f"({len(old)} known, tracked in {ALLOWLIST.relative_to(ROOT)})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
