"""Self-test for scripts/hookscsv_merge.py (HB-HOOKSCSV-ROWMERGE). Synthetic 3-way cases on scratch
text, one real git merge through the registered driver entry point in a throwaway repo, and
(--replay) the 2026-11-10 PORT-OT-FRONTEND case from this repo's history. Never touches hooks.csv.

    py -3.12 scripts/test_hookscsv_merge.py [--replay]
"""
from __future__ import annotations

import pathlib
import subprocess
import sys
import tempfile

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import hookscsv_merge as M  # noqa: E402

E = "\r\n"
HDR = "rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes,exe_file,port"
COMMENTS = ['# RVA: 0x00xxxxxx (8-hex),,,,,,,,,,', '"# exe_file: quoted, comment",,,,,,,,,,']


def row(rva, name="F", conf="C2", notes="n", port="none"):
    return f'{rva},{name},boot,{conf},mapped,a.md,,,"{notes}, with comma",,{port}'


def doc(rows, comments=COMMENTS, hdr=HDR, eol=E):
    return eol.join([hdr, *comments, *rows]) + eol


BASE = [row("00401000"), row("00401010"), row("00401020"), row("00401030")]


def case(name, base, ours, theirs, want_text=None, want_conf=(), check=None):
    text, conf = M.merge(base, ours, theirs)
    ok = True
    why = []
    if want_text is not None and text != want_text:
        ok = False
        why.append("text differs")
    got = sorted(c.split(":")[0] for c in conf)
    if sorted(want_conf) != got:
        ok = False
        why.append(f"conflicts {got} want {sorted(want_conf)}")
    if check:
        r = check(text, conf)
        if r:
            ok = False
            why.append(r)
    return ok, "; ".join(why), conf


def cases():
    b = doc(BASE)
    out = []
    # 1 different rows changed on both sides (adjacent rows: the line merge conflicted here)
    o = doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]])
    t = doc([*BASE[:2], row("00401020", notes="t"), BASE[3]])
    out.append(("different adjacent rows -> clean", b, o, t,
                doc([BASE[0], row("00401010", conf="C3"), row("00401020", notes="t"), BASE[3]]), ()))
    # 2 same row, same change
    o = doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]])
    out.append(("same row same change -> clean", b, o, o, o, ()))
    # 3 same row, different change -> halt, field names listed
    t = doc([BASE[0], row("00401010", conf="C4"), *BASE[2:]])

    def names_conf(text, conf):
        if not conf or "differ in confidence" not in conf[0]:
            return f"field names not reported: {conf}"
        if "<<<<<<< ours" not in text or ">>>>>>> theirs" not in text:
            return "no markers"
    out.append(("same row different -> halt (confidence)", b, o, t, None, ("0x00401010",), names_conf))
    # 4 derived-only difference (the 0x00496900 shape): halt, field `port`
    o = doc([BASE[0], row("00401010", notes="x", port="none"), *BASE[2:]])
    t = doc([BASE[0], row("00401010", notes="x", port="called"), *BASE[2:]])
    out.append(("same row, only port differs -> halt (port)", b, o, t, None, ("0x00401010",),
                lambda tx, c: None if "differ in port " in c[0] else f"not port-only: {c}"))
    # 5 added rows, either side, placed after nearest preceding key
    o = doc([BASE[0], row("00401008", name="OA"), *BASE[1:]])
    t = doc([*BASE[:3], row("00401028", name="TA"), BASE[3]])
    out.append(("added on each side -> clean, after preceding key", b, o, t,
                doc([BASE[0], row("00401008", name="OA"), *BASE[1:3], row("00401028", name="TA"), BASE[3]]), ()))
    # 6 theirs adds a row after a row ours deleted -> goes after the next earlier survivor
    o = doc([BASE[0], BASE[1], BASE[3]])
    t = doc([*BASE[:3], row("00401028", name="TA"), BASE[3]])
    out.append(("theirs add after a row ours deleted", b, o, t,
                doc([BASE[0], BASE[1], row("00401028", name="TA"), BASE[3]]), ()))
    # 7 added on both sides with the same key: same -> one copy, different -> halt
    o = doc([*BASE, row("00402000", name="X")])
    out.append(("added both sides identical -> clean", b, o, o, o, ()))
    t = doc([*BASE, row("00402000", name="Y")])
    out.append(("added both sides different -> halt", b, o, t, None, ("0x00402000",)))
    # 8 deletes
    o = doc([BASE[0], *BASE[2:]])
    out.append(("ours deletes, theirs untouched -> deleted", b, o, b, o, ()))
    out.append(("theirs deletes, ours untouched -> deleted", b, b, o, o, ()))
    out.append(("both delete -> deleted", b, o, o, o, ()))
    t = doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]])
    out.append(("ours deletes, theirs edits -> halt", b, o, t, None, ("0x00401010",)))
    out.append(("theirs deletes, ours edits -> halt", b, t, o, None, ("0x00401010",)))
    # 9 comment lines: ours keeps its comments, theirs' comment change taken when ours left them
    c2 = [COMMENTS[0], "# new note,,,,,,,,,,", COMMENTS[1]]
    o = doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]])
    t = doc(BASE, comments=c2)
    out.append(("theirs edits comments, ours edits a row -> both", b, o, t,
                doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]], comments=c2), ()))
    o2 = doc(BASE, comments=[COMMENTS[0], "# other,,,,,,,,,,", COMMENTS[1]])
    out.append(("both edit the comment block differently -> halt", b, o2, t, None, ("comment group after TOP",)))
    # comment line whose text starts like a data key must not be keyed
    cc = [*COMMENTS, "# 00401010 is special,,,,,,,,,,"]
    out.append(("comment sharing a key prefix is not a row", doc(BASE, comments=cc), doc(BASE, comments=cc),
                doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]], comments=cc),
                doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]], comments=cc), ()))
    # 10 CRLF preserved, including a row taken from an LF theirs
    o = doc(BASE)
    t = doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]], eol="\n")
    out.append(("CRLF kept (theirs LF)", doc(BASE, eol="\n"), o, t,
                doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]]), (),
                lambda tx, c: None if tx.count("\r\n") == tx.count("\n") else "bare LF in output"))
    # 11 header untouched (ours' bytes), even when theirs edits rows
    hdr_o = HDR + ' '
    out.append(("header kept from ours", doc(BASE, hdr=hdr_o), doc(BASE, hdr=hdr_o),
                doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]], hdr=hdr_o),
                doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]], hdr=hdr_o), ()))
    # 12 pre-`port` branch: theirs has no port column, edits a row; ours has port everywhere
    hdr10 = HDR.rsplit(",", 1)[0]
    nop = [r.rsplit(",", 1)[0] for r in BASE]
    t = doc([nop[0], row("00401010", conf="C3").rsplit(",", 1)[0], *nop[2:]], hdr=hdr10,
            comments=[c[:-1] for c in COMMENTS])
    ob = doc(nop, hdr=hdr10, comments=[c[:-1] for c in COMMENTS])
    o = doc([BASE[0], row("00401010", port="body"), *BASE[2:]], comments=[c[:-1] for c in COMMENTS])
    out.append(("theirs predates port column -> clean, port kept", ob, o, t,
                doc([BASE[0], row("00401010", conf="C3", port="body"), *BASE[2:]],
                    comments=[c[:-1] for c in COMMENTS]), ()))
    # 13 malformed row (unquoted comma) compared raw; one-sided repair taken
    bad = '00401020,F,boot,C2,mapped,a.md,,,n, with comma,,none'
    good = row("00401020")
    out.append(("malformed row repaired on theirs -> taken", doc([*BASE[:2], bad, BASE[3]]),
                doc([BASE[0], row("00401010", conf="C3"), bad, BASE[3]]), doc([*BASE[:2], good, BASE[3]]),
                doc([BASE[0], row("00401010", conf="C3"), good, BASE[3]]), ()))
    # 14 unchanged file -> byte-identical
    out.append(("no changes -> identical", b, b, b, b, ()))
    return out


def git_driver_case(tmp: pathlib.Path):
    """Real `git merge` in a scratch repo with hooksport registered as this checkout's backfill_port
    entry point. Adjacent-row edits on two branches must merge clean."""
    def g(*a):
        return subprocess.run(["git", "-C", str(tmp), *a], capture_output=True, text=True)
    g("init", "-q")
    g("config", "user.email", "t@t")
    g("config", "user.name", "t")
    g("config", "core.autocrlf", "false")
    drv = f'py -3.12 "{HERE / "backfill_port.py"}" --merge-driver %O %A %B'
    g("config", "merge.hooksport.driver", drv)
    (tmp / ".gitattributes").write_text("/hooks.csv merge=hooksport\n")
    f = tmp / "hooks.csv"
    f.write_bytes(doc(BASE).encode())
    g("add", "."), g("commit", "-qm", "base")
    g("checkout", "-qb", "t")
    f.write_bytes(doc([*BASE[:2], row("00401020", notes="t"), BASE[3]]).encode())
    g("commit", "-qam", "t")
    g("checkout", "-q", "-")
    f.write_bytes(doc([BASE[0], row("00401010", conf="C3"), *BASE[2:]]).encode())
    g("commit", "-qam", "o")
    # control: the same merge with the plain text merge conflicts
    ctl = subprocess.run(["git", "-C", str(tmp), "-c", "merge.hooksport.driver=false", "merge-tree",
                          "--write-tree", "HEAD", "t"], capture_output=True, text=True)
    r = g("-c", "merge.autoStash=false", "merge", "--no-edit", "t")
    want = doc([BASE[0], row("00401010", conf="C3"), row("00401020", notes="t"), BASE[3]]).encode()
    ok = r.returncode == 0 and f.read_bytes() == want
    return ok, f"git merge rc={r.returncode} text={'ok' if f.read_bytes() == want else 'differs'} " \
               f"control(no driver) rc={ctl.returncode}; {r.stderr.strip()[:200]}"


def replay():
    """2026-11-10: base = merge-base(3167e6545, 6144429be), ours = 6144429be (orch/integration),
    theirs = 3167e6545 (PORT-OT-FRONTEND). Expected: 5 rows from theirs, one halt, on 0x00496900, port only."""
    root = HERE.parent

    def show(ref):
        return subprocess.run(["git", "-C", str(root), "show", f"{ref}:hooks.csv"],
                              capture_output=True).stdout.decode("utf-8")
    mb = subprocess.run(["git", "-C", str(root), "merge-base", "3167e6545", "6144429be"],
                        capture_output=True, text=True).stdout.strip()
    O, A, B = show(mb), show("6144429be"), show("3167e6545")
    text, conf = M.merge(O, A, B)
    do, da, db = M.Doc(O), M.Doc(A), M.Doc(B)
    taken = [k for k in do.order
             if db.rows[k].body != do.rows[k].body and da.rows[k].body == do.rows[k].body]
    ok = (len(conf) == 1 and conf[0].startswith("0x00496900:") and "differ in port " in conf[0]
          and len(taken) == 5 and all(db.rows[k].body + "\n" in text or db.rows[k].body + "\r\n" in text
                                      for k in taken))
    return ok, f"merge-base {mb[:9]}; theirs-only rows {len(taken)} {taken}; conflicts {conf}"


def main(argv):
    cs = cases()
    total = len(cs) + 1 + ("--replay" in argv)
    fails = 0
    for i, c in enumerate(cs, 1):
        name, *rest = c
        ok, why, conf = case(name, *rest)
        fails += not ok
        print(f"[{i}/{total}] {name} {'PASS' if ok else 'FAIL'}" + (f"  ({why})" if why else "")
              + (f"  -> {conf[0][:110]}" if conf and ok else ""))
    with tempfile.TemporaryDirectory() as td:
        ok, why = git_driver_case(pathlib.Path(td))
    fails += not ok
    print(f"[{len(cs) + 1}/{total}] real git merge via hooksport driver {'PASS' if ok else 'FAIL'}  ({why})")
    if "--replay" in argv:
        ok, why = replay()
        fails += not ok
        print(f"[{total}/{total}] replay 2026-11-10 PORT-OT-FRONTEND {'PASS' if ok else 'FAIL'}  ({why})")
    print(f"{total - fails}/{total} PASS")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
