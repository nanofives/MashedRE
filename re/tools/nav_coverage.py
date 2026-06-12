# nav_coverage.py — static screen-coverage walker for the standalone menu.
#
# Items 20-25 of the 2026-06-12 review were ENTIRE SCREENS the standalone
# lacked; nobody noticed until a human navigated there. This tool makes that
# class of gap mechanical: it parses the nav descriptor tables out of
# Frontend/MenuNavSM.cpp (the harvested kT<N> blobs), replays the
# ActionToScreen push map, BFS-walks every screen reachable from the root,
# and cross-references exe_main.cpp for screen-specific handling.
#
# Reported signals (all derived from source text, nothing inferred):
#   * kScreenTables holes (screen ids with no harvested descriptor table)
#   * reachable screens whose action codes dead-end (no push mapping and not
#     a literal screen id)
#   * kind-consistency: when SOME screens of a screen-kind have `sid == N`
#     special-casing in exe_main.cpp and others of the same kind do not,
#     the unhandled ones are flagged (likely missing bespoke draw/input)
#   * per-screen inventory: kind, item count, action codes, sid-references
#
# Usage:  py -3.12 re/tools/nav_coverage.py [--src mashedmod/src/mashed_re]
# Exit code is always 0 (informational tool, not a gate).
import argparse
import re
import sys
from collections import deque
from pathlib import Path

KEY_BACK = 0xFF000000
KEY_KIND = 0xFF080000
KEY_ITEM = 0xFF040000
KEY_ACT1 = 0xFF140000
KEY_ACT2 = 0xFF050000
KEY_ITEM_END = 0xFF060000
KEY_TABLE_END = 0xFF070000

ROOT_SCREEN = 1   # nav root = main menu (Nav_Init; #12 root-cause fix)


def parse_tables(text):
    """kT<N> arrays -> {table_name: [u32...]}"""
    tables = {}
    for m in re.finditer(
            r"const std::uint32_t (kT\d+)\[\]\s*=\s*\{([^}]*)\}", text):
        vals = [int(v, 16) for v in re.findall(r"0x[0-9a-fA-F]+", m.group(2))]
        tables[m.group(1)] = vals
    return tables


def parse_screen_map(text):
    """kScreenTables initializer -> {screen_id: table_name|None}"""
    m = re.search(r"kScreenTables\[\]\s*=\s*\{([^}]*)\}", text)
    if not m:
        sys.exit("cannot find kScreenTables[] in MenuNavSM.cpp")
    out = {}
    for i, tok in enumerate(t.strip() for t in m.group(1).split(",")):
        tok = tok.split("/*")[0].strip()
        if not tok:
            continue
        out[i] = tok if tok.startswith("kT") else None
    return out


def parse_push_map(text):
    """ActionToScreen body -> {action: set(child screens)} (all static return
    values >= 0 across every branch; sentinels ignored)."""
    m = re.search(r"int ActionToScreen\([^)]*\)\s*\{(.*?)\n\}", text, re.S)
    if not m:
        sys.exit("cannot find ActionToScreen in MenuNavSM.cpp")
    body = m.group(1)
    push = {}
    cur_cases = []
    for line in body.splitlines():
        line = line.split("//")[0]
        for cm in re.finditer(r"case\s+(0x[0-9a-fA-F]+)u?\s*:", line):
            cur_cases.append(int(cm.group(1), 16))
        targets = set()
        for rm in re.finditer(r"return\s+(0x[0-9a-fA-F]+|\d+)\s*;", line):
            targets.add(int(rm.group(1), 0))
        for tm in re.finditer(r"\?\s*(0x[0-9a-fA-F]+|\d+)\s*:\s*"
                              r"(0x[0-9a-fA-F]+|\d+)", line):
            targets.add(int(tm.group(1), 0))
            targets.add(int(tm.group(2), 0))
        # sentinel enums (kActNone etc.) produce no ints; negatives impossible
        if targets and cur_cases:
            for c in cur_cases:
                push.setdefault(c, set()).update(targets)
            cur_cases = []
        elif re.search(r"break\s*;|return\s+kAct", line) and cur_cases:
            for c in cur_cases:
                push.setdefault(c, set())
            cur_cases = []
    return push


def decode_table(vals):
    """descriptor blob -> {back, kind, items:[{sid, actions:[u32]}]}"""
    info = {"back": None, "kind": None, "items": []}
    i = 0
    item = None
    while i < len(vals):
        v = vals[i]
        if v == KEY_BACK and i + 1 < len(vals):
            info["back"] = vals[i + 1]; i += 2; continue
        if v == KEY_KIND and i + 1 < len(vals):
            info["kind"] = vals[i + 1]; i += 2; continue
        if v == KEY_ITEM and i + 1 < len(vals):
            item = {"sid": vals[i + 1], "actions": []}
            info["items"].append(item); i += 2; continue
        if v in (KEY_ACT1, KEY_ACT2) and i + 1 < len(vals):
            if item is not None:
                item["actions"].append(vals[i + 1])
            i += 2; continue
        if v == KEY_ITEM_END:
            item = None; i += 1; continue
        if v == KEY_TABLE_END:
            break
        i += 1
    return info


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--src", default="mashedmod/src/mashed_re")
    args = ap.parse_args()
    src = Path(args.src)

    nav_text = (src / "Frontend" / "MenuNavSM.cpp").read_text(errors="replace")
    exe_text = (src / "exe_main.cpp").read_text(errors="replace")

    tables = parse_tables(nav_text)
    screen_map = parse_screen_map(nav_text)
    push = parse_push_map(nav_text)
    n_screens = len(screen_map)

    screens = {}
    for sid, tname in screen_map.items():
        screens[sid] = decode_table(tables[tname]) if tname in tables else None

    # exe_main.cpp screen-specific handling: `sid == N` / `ScreenId() == N`
    sid_refs = set()
    for m in re.finditer(r"(?:sid|Nav_ScreenId\(\))\s*==\s*(0x[0-9a-fA-F]+|\d+)",
                         exe_text):
        sid_refs.add(int(m.group(1), 0))

    # ---- reachability BFS over the push map -------------------------------
    reach = {ROOT_SCREEN}
    dead_actions = []   # (screen, item sid, action)
    q = deque([ROOT_SCREEN])
    while q:
        s = q.popleft()
        info = screens.get(s)
        if not info:
            continue
        for it in info["items"]:
            for act in it["actions"]:
                kids = set()
                if act in push:
                    kids = {k for k in push[act] if 0 <= k < n_screens}
                elif act < n_screens:        # default rule: literal screen id
                    kids = {act}
                elif (act & 0xFFFF0000) == act and act != 0xFFFFFFFF:
                    dead_actions.append((s, it["sid"], act))
                for k in kids:
                    if k not in reach:
                        reach.add(k); q.append(k)

    # ---- report ------------------------------------------------------------
    print(f"screens: {n_screens} ids, {sum(1 for v in screens.values() if v)} "
          f"with tables, reachable from root {ROOT_SCREEN}: "
          f"{len(reach & set(screens))}")
    holes = [s for s, v in screens.items() if v is None]
    if holes:
        print(f"\nTABLE HOLES (no harvested descriptor): {holes}")

    unreachable = sorted(s for s, v in screens.items() if v and s not in reach)
    if unreachable:
        print(f"\nNO STATIC PUSH PATH from root (table exists, but only "
              f"ActionToScreen's static/gated returns are walked — screens "
              f"behind the 0xff260000 mode-tile sub-handler's internal codes "
              f"or behind modals are NOT visited): {unreachable}")

    if dead_actions:
        print("\nDEAD-END ACTIONS (0xffXX0000 code with no ActionToScreen case):")
        for s, sid, act in dead_actions:
            print(f"  screen {s:2} item 0x{sid:x}: action 0x{act:08x}")

    # kind-consistency
    by_kind = {}
    for s in sorted(reach):
        info = screens.get(s)
        if info:
            by_kind.setdefault(info["kind"], []).append(s)
    print("\nKIND CONSISTENCY (screens sharing a kind where only some have "
          "sid-specific code in exe_main.cpp):")
    flagged = False
    for kind, ss in sorted(by_kind.items()):
        handled = [s for s in ss if s in sid_refs]
        unhandled = [s for s in ss if s not in sid_refs]
        if handled and unhandled:
            flagged = True
            print(f"  kind {kind}: handled {handled} / NOT handled {unhandled}")
    if not flagged:
        print("  (none)")

    print("\nPER-SCREEN INVENTORY (reachable):")
    print(f"  {'scr':>3} {'kind':>4} {'back':>6} {'items':>5} "
          f"{'sid-ref':>7}  actions")
    for s in sorted(reach):
        info = screens.get(s)
        if not info:
            continue
        acts = sorted({a for it in info["items"] for a in it["actions"]})
        acts_s = ",".join(f"0x{a:x}" if a > 0xFFFF else str(a) for a in acts)
        print(f"  {s:>3} {info['kind']:>4} 0x{info['back'] or 0:>4x} "
              f"{len(info['items']):>5} "
              f"{'yes' if s in sid_refs else '-':>7}  {acts_s}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
